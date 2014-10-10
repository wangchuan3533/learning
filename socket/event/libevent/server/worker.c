#include "define.h"
#include "websocket.h"
#include "worker.h"

worker_t *worker_create()
{
    worker_t *w = (worker_t *)malloc(sizeof(worker_t));
    if (NULL == w) {
        err_quit("malloc");
    }
    memset(w, 0, sizeof(worker_t));
    w->base = event_base_new();
    if (NULL == w->base) {
        err_quit("event_base_new");
    }

    if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, w->sockpair_dispatcher) < 0) {
        err_quit("socketpair");
    }

    if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, w->sockpair_pusher) < 0) {
        err_quit("socketpair");
    }

    return w;
}

void worker_destroy(worker_t **w)
{
    client_t *itr = NULL, *tmp = NULL;
    // destroy event_base
    if (w && *w) {
        // delete hash
        HASH_ITER(h1, (*w)->clients, itr, tmp) {
            // delete the client?
            HASH_DELETE(h1, (*w)->clients, itr);
        }
        free(*w);
        *w = NULL;
    }
}

client_t *client_create()
{
    client_t *c = (client_t *)malloc(sizeof(client_t));
    if (NULL == c) {
        err_quit("malloc");
    }
    // memset
    memset(c, 0, sizeof(client_t));
    // client_id
    c->headers = http_headers_create();
    c->frame = ws_frame_create();
    return c;
}

void client_destroy(client_t **c)
{
    if (c && *c) {
        ws_frame_destroy(&((*c)->frame));
        http_headers_destroy(&((*c)->headers));
        free(*c);
        *c = NULL;
    }
}

int websocket_broadcast(worker_t *w, void *data, size_t len)
{
    int ret;
    void *tmp;
    size_t length;
    cmd_t cmd;
    struct evbuffer *buffer = evbuffer_new();

    ret = send_text_frame(buffer, data, len);
    if (ret != 0) {
        // TODO
    }

    length = evbuffer_get_length(buffer);
    tmp = malloc(length);
    if (evbuffer_remove(buffer, tmp, length) != length) {
        err_quit("evbuffer_remove");
    }

    cmd.cmd_no = CMD_BROADCAST;
    cmd.data = tmp;
    cmd.length = length;
    if (evbuffer_add(bufferevent_get_output(w->bev_pusher[0]), &cmd, sizeof cmd) != 0) {
        err_quit("evbuffer_add");
    }

    return 0;
}

int websocket_handle(client_t *c)
{
    websocket_frame_t *f = c->frame;
    int ret;

    switch (f->opcode) {
    case OPCODE_PING_FRAME:
        ret = send_pong_frame(bufferevent_get_output(c->bev), f->data, f->length);
        if (ret) {
            // TODO error handling
        }
        break;
    case OPCODE_TEXT_FRAME:
        //ret = send_text_frame(bufferevent_get_output(c->bev), f->data, f->length);
        ret = websocket_broadcast(c->worker, f->data, f->length);
        if (ret) {
            // TODO
        }
        break;
    case OPCODE_PONG_FRAME:
        // TODO
        break;
    case OPCODE_CONTINUATION_FRAME:
        // TODO not surpport
        break;
    case OPCODE_CLOSE_FRAME:
        ret = send_close_frame(bufferevent_get_output(c->bev), f->data, f->length);
        if (ret) {
            // TODO error handling
        }
        c->close_flag = 1;
        break;
    default:
        break;
    }
    return 0;
}

void websocket_readcb(struct bufferevent *bev, void *arg)
{
    client_t *c = (client_t *)arg;
    struct evbuffer *input = bufferevent_get_input(bev);
    int ret;

    while (evbuffer_get_length(input)) {
        switch (c->state) {
        case CLIENT_STATE_ACCEPTED:
            c->state = CLIENT_STATE_HTTP_PARSE_STARTED;
        case CLIENT_STATE_HTTP_PARSE_STARTED:
            ret = parse_http(input, c->headers);
            // http parse error
            if (ret != 0) {
                // TODO error handling
                return;
            }
            if (c->headers->state != HTTP_HEADERS_STATE_FINISHED) {
                break;
            }

            // parse finished
            print_http_headers(c->headers);
            // check the websocket request
            ret = check_websocket_request(c->headers);
            if (ret == 0) {
                ret = send_handshake(bufferevent_get_output(c->bev), c->headers->sec_websocket_key);
                c->state = CLIENT_STATE_HANDSHAKE_STARTED;
            // not websocket, send 200 ok, and close socket when finish
            } else {
                ret = send_200_ok(bufferevent_get_output(c->bev));
                c->close_flag = 1;
            }
            break;
        case CLIENT_STATE_WEBSOCKET_FRAME_LOOP:
            ret = parse_frame(input, c->frame);
            if (ret != 0) {
                // TODO error handling
                return;
            }
            if (c->frame->state == FRAME_STATE_FINISHED) {
                // debug
                ret = websocket_handle(c);
                if (ret) {
                    // TODO error handling
                }
                // clear
                ws_frame_clear(c->frame);
                c->state = CLIENT_STATE_WEBSOCKET_FRAME_LOOP;
            }
            // send client's frame to co worker

            break;
        case CLIENT_STATE_HANDSHAKE_STARTED:
            // waiting write to complete, do not read
            return;
        default:
            err_quit("Oops read client state:[%d]\n", c->state);
        }
    }
}

void websocket_writecb(struct bufferevent *bev, void *arg)
{
    client_t *c = (client_t *)arg;
    cmd_t cmd;

    switch (c->state) {
    case CLIENT_STATE_HANDSHAKE_STARTED:
        c->state = CLIENT_STATE_WEBSOCKET_FRAME_LOOP;
        break;
    case CLIENT_STATE_HTTP_PARSE_STARTED:
    case CLIENT_STATE_WEBSOCKET_FRAME_LOOP:
        break;
    default:
        break;
    }

    if (c->close_flag) {
        // delete from hash
        HASH_DELETE(h1, c->worker->clients, c);
        // notify the pusher
        cmd.cmd_no = CMD_DEL_CLIENT;
        cmd.client = c;
        if (evbuffer_add(bufferevent_get_output(c->worker->bev_pusher[0]), &cmd, sizeof cmd) != 0) {
            err_quit("evbuffer_add");
        }
        bufferevent_free(bev);
    }
}

void websocket_errorcb(struct bufferevent *bev, short error, void *arg)
{
    client_t *c = (client_t *)arg;
    cmd_t cmd;
    if (error & BEV_EVENT_EOF) {
        /* connection has been closed, do any clean up here */
        /* ... */
    } else if (error & BEV_EVENT_ERROR) {
        /* check errno to see what error occurred */
        /* ... */
    } else if (error & BEV_EVENT_TIMEOUT) {
        /* must be a timeout event handle, handle it */
        switch (c->state) {
        case CLIENT_STATE_ACCEPTED:
        case CLIENT_STATE_HTTP_PARSE_STARTED:
        case CLIENT_STATE_WEBSOCKET_FRAME_LOOP:
            // read timeout -> close in write cb
            if (error & BEV_EVENT_READING) {
                break;
            }
            return;
        case CLIENT_STATE_HANDSHAKE_STARTED:
            if (error & BEV_EVENT_WRITING) {
                break;
            }
            return;
        default:
            err_quit("Opps client's state = %d\n", c->state);
        }
            // write time out
        /* ... */
    }
    // delete from hash
    HASH_DELETE(h1, c->worker->clients, c);
    // notify the pusher
    cmd.cmd_no = CMD_DEL_CLIENT;
    cmd.client = c;
    if (evbuffer_add(bufferevent_get_output(c->worker->bev_pusher[0]), &cmd, sizeof cmd) != 0) {
        err_quit("evbuffer_add");
    }
    bufferevent_free(bev);
}

void worker_timer(int fd, short event, void *arg)
{
    worker_t *w = (worker_t *)arg;
    //printf("worker timer\n");
    if (w->stop) {
        event_base_loopexit(w->base, NULL);
    }
}

void echo_readcb(struct bufferevent *bev, void *arg)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);

    evbuffer_add_buffer(output, input);
}

void echo_errorcb(struct bufferevent *bev, short error, void *arg)
{
    client_t *c = (client_t *)arg;
    cmd_t cmd;

    if (error | BEV_EVENT_TIMEOUT) {
        // TODO
    } else if (error | BEV_EVENT_ERROR) {
        // TODO
    }
    // delete from hash
    HASH_DELETE(h1, c->worker->clients, c);
    // notify the pusher
    cmd.cmd_no = CMD_DEL_CLIENT;
    cmd.client = c;
    if (evbuffer_add(bufferevent_get_output(c->worker->bev_pusher[0]), &cmd, sizeof cmd) != 0) {
        err_quit("evbuffer_add");
    }
    bufferevent_free(bev);
}

int broadcast(worker_t *w, void *data, size_t length)
{
    client_t *itr, *tmp;
    HASH_ITER(h1, w->clients, itr, tmp) {
        if (evbuffer_add(bufferevent_get_output(itr->bev), data, length) != 0) {
            err_quit("evbuffer_add");
        }
    }
    return 0;
}

void broadcast_inner_cb(struct bufferevent *bev, void *arg)
{
    client_t *c = (client_t *)arg;
    struct evbuffer *input = bufferevent_get_input(bev);
    void *data; 
    size_t len;
    len = evbuffer_get_length(input);
    data = malloc(len);
    if (evbuffer_remove(input, data, len) != len) {
        err_quit("evbuffer_remove");
    }
    broadcast(c->worker, data, len);
    free(data);
}

void broadcast_inter_cb(struct bufferevent *bev, void *arg)
{
    client_t *c = (client_t *)arg;
    struct evbuffer *input = bufferevent_get_input(bev);
    void *data; 
    size_t len;
    cmd_t cmd;
    len = evbuffer_get_length(input);
    data = malloc(len);
    if (evbuffer_remove(input, data, len) != len) {
        err_quit("evbuffer_remove");
    }
    cmd.cmd_no = CMD_BROADCAST;
    cmd.data = data;
    cmd.length = len;
    if (evbuffer_add(bufferevent_get_output(c->worker->bev_pusher[0]), &cmd, sizeof cmd) != 0) {
        err_quit("evbuffer_add");
    }
}

void worker_task_readcb(struct bufferevent *bev, void *arg)
{
    worker_t *w = (worker_t *)arg;
    struct bufferevent *bev_client;
    struct timeval timeout = {1000, 0};
    struct evbuffer *input = bufferevent_get_input(bev);
    client_t *c;
    cmd_t cmd;

    while (evbuffer_get_length(input) >= sizeof cmd) {
        if (evbuffer_remove(input, &cmd, sizeof cmd) != sizeof cmd) {
            err_quit("evbuffer_remove");
        }
        switch (cmd.cmd_no) {
        case CMD_ADD_CLIENT:
            c = cmd.client;
            bev_client = bufferevent_socket_new(w->base, c->fd, BEV_OPT_CLOSE_ON_FREE);
            if (!bev_client) {
                err_quit("bufferevent_socket_new");
            }
            c->bev = bev_client;
            c->client_id = c->fd;
            //bufferevent_setcb(bev_client, echo_readcb, NULL, echo_errorcb, c);
            //bufferevent_setcb(bev_client, broadcast_inner_cb, NULL, echo_errorcb, c);
            //bufferevent_setcb(bev_client, broadcast_inter_cb, NULL, echo_errorcb, c);
            bufferevent_setcb(bev_client, websocket_readcb, websocket_writecb, websocket_errorcb, c);
            bufferevent_enable(bev_client, EV_READ|EV_WRITE);
            bufferevent_set_timeouts(bev_client, &timeout, &timeout);

            // add to hash
            HASH_ADD(h1, w->clients, client_id, sizeof(c->client_id), c);
            // pass cmd to pusher
            if (evbuffer_add(bufferevent_get_output(w->bev_pusher[0]), &cmd, sizeof cmd) != 0) {
                err_quit("evbuffer_add");
            }
            break;
        default:
            break;
        }
    }
}

void worker_task_errorcb(struct bufferevent *bev, short error, void *arg)
{
    printf("worker error\n");
    worker_t *w = (worker_t *)arg;
    if (error | BEV_EVENT_TIMEOUT) {
        // TODO
    } else if (error | BEV_EVENT_ERROR) {
        // TODO
    }
    w->stop = 1;
}

void worker_pusher_readcb(struct bufferevent *bev, void *arg)
{
    worker_t *w = (worker_t *)arg;
    // push
    cmd_t cmd;
    while (evbuffer_get_length(bufferevent_get_input(bev)) >= sizeof cmd) {
        if (evbuffer_remove(bufferevent_get_input(bev), &cmd, sizeof cmd) != sizeof cmd) {
            err_quit("evbuffer_remove");
        }
        switch (cmd.cmd_no) {
        case CMD_BROADCAST:
            broadcast(w, cmd.data, cmd.length);
            free(cmd.data);
            break;
        default:
            break;
        }
    }
}

void worker_pusher_errorcb(struct bufferevent *bev, short error, void *arg)
{
}

void *worker_loop(void *arg)
{
    worker_t *w = (worker_t *)arg;
    struct event *timer_event;
    struct timeval timeout = {1, 0};

    // bufferevent to dispatcher
    w->bev_dispatcher[0] = bufferevent_socket_new(w->base, w->sockpair_dispatcher[0], BEV_OPT_CLOSE_ON_FREE);
    if (!w->bev_dispatcher[0]) {
        err_quit("bufferevent_socket_new");
    }
    bufferevent_setcb(w->bev_dispatcher[0], worker_task_readcb, NULL, worker_task_errorcb, w);
    bufferevent_setwatermark(w->bev_dispatcher[0], EV_READ, sizeof(cmd_t), 0);
    bufferevent_enable(w->bev_dispatcher[0], EV_READ | EV_WRITE);

    // bufferevent to pusher
    w->bev_pusher[0] = bufferevent_socket_new(w->base, w->sockpair_pusher[0], BEV_OPT_CLOSE_ON_FREE);
    if (!w->bev_pusher[0]) {
        err_quit("bufferevent_socket_new");
    }
    bufferevent_setcb(w->bev_pusher[0], worker_pusher_readcb, NULL, worker_pusher_errorcb, w);
    bufferevent_setwatermark(w->bev_pusher[0], EV_READ, sizeof(cmd_t), 0);
    bufferevent_enable(w->bev_pusher[0], EV_READ | EV_WRITE);

    timer_event = event_new(w->base, -1, EV_PERSIST, worker_timer, w);
    if (NULL == timer_event) {
        err_quit("event_new");
    }
    event_add(timer_event, &timeout);
    event_base_dispatch(w->base);
    return (void *)0;
}

int worker_start(worker_t *w)
{
    int ret;
    ret = pthread_create(&(w->thread_id), NULL, worker_loop, w);
    if (ret != 0) {
        err_quit("pthread_create");
    }
    return 0;
}

int worker_stop(worker_t *w)
{
    w->stop = 1;
    return 0;
}

