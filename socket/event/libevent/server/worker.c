#include "define.h"
#include "websocket.h"
#include "worker.h"
worker_t *worker_create()
{
    worker_t *w = (worker_t *)malloc(sizeof(worker_t));
    int ret;
    if (NULL == w) {
        err_quit("malloc");
    }
    memset(w, 0, sizeof(worker_t));
    w->client_list = NULL;
    w->client_count = 0;
    w->base = event_base_new();
    if (NULL == w->base) {
        err_quit("event_base_new");
    }

    ret = evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, w->fd);
    if (ret < 0) {
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
        //HASH_ITER(hh, w->client_list, itr, tmp) {
            // delete the client?
        //}
        //if (NULL != w->base)
        //event_base_destroy(w->base);
        free(*w);
        *w = NULL;
    }
}

client_t *client_create(worker_t *w)
{
    client_t *c = (client_t *)malloc(sizeof(client_t));
    if (NULL == c) {
        err_quit("malloc");
    }
    // memset
    memset(c, 0, sizeof(client_t));
    // client_id
    c->worker = w;
    c->client_id = w->client_count++;
    c->headers = http_headers_create();
    c->frame = ws_frame_create();
    // add to hash
    HASH_ADD(hh, w->client_list, client_id, sizeof(uint64_t), c);
    return c;
}

void client_destroy(client_t **c)
{
    if (c && *c) {
        HASH_DELETE(hh, (*c)->worker->client_list, *c);
        ws_frame_destroy(&((*c)->frame));
        http_headers_destroy(&((*c)->headers));
        free(*c);
        *c = NULL;
    }
}

int websocket_broadcast(worker_t *w, void *data, unsigned int len)
{

    client_t *itr = NULL, *tmp = NULL;
    int ret;
    HASH_ITER(hh, w->client_list, itr, tmp) {
        ret = send_text_frame(bufferevent_get_output(itr->bev), data, len);
        if (ret) {
            // TODO error handling
            return -1;
        }
    } 
    return 0;
}

int handle(client_t *c)
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
        websocket_broadcast(c->worker, f->data, f->length);
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
    }
    return 0;
}

void websocket_readcb(struct bufferevent *bev, void *arg)
{
    client_t *c = (client_t *)arg;
    struct evbuffer *input = bufferevent_get_input(bev);
    char buf[1024];
    size_t n;
    int i, ret;

    // iterate the clients

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
            //broadcast(c->worker, "http", 4);
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
                ret = handle(c);
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

    switch (c->state) {
    case CLIENT_STATE_HANDSHAKE_STARTED:
        c->state = CLIENT_STATE_WEBSOCKET_FRAME_LOOP;
        break;
    case CLIENT_STATE_HTTP_PARSE_STARTED:
    case CLIENT_STATE_WEBSOCKET_FRAME_LOOP:
        break;
    }

    if (c->close_flag) {
        client_destroy(&c);
        bufferevent_free(bev);
    }
}

void websocket_errorcb(struct bufferevent *bev, short error, void *arg)
{
    client_t *c = (client_t *)arg;
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
    client_destroy(&c);
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
    client_t *c = (client_t *)arg;

    evbuffer_add_buffer(output, input);
}

void echo_errorcb(struct bufferevent *bev, short error, void *arg)
{
    client_t *c = (client_t *)arg;

    if (error | BEV_EVENT_TIMEOUT) {
        // TODO
    } else if (error | BEV_EVENT_ERROR) {
        // TODO
    }
    client_destroy(&c);
    bufferevent_free(bev);
}

void broadcast_cb(struct bufferevent *bev, void *arg)
{
    client_t *c = (client_t *)arg, *itr = NULL, *tmp = NULL;
    struct evbuffer *input = bufferevent_get_input(bev);
    void *data; 
    size_t len;
    len = evbuffer_get_length(input);
    data = malloc(len);
    if (evbuffer_remove(input, data, len) != len) {
        err_quit("evbuffer_remove");
    }
    HASH_ITER(hh, c->worker->client_list, itr, tmp) {
        if (evbuffer_add(bufferevent_get_output(itr->bev), data, len) != 0) {
            err_quit("evbuffer_add");
        }
    }
    free(data);
}
void worker_task_readcb(struct bufferevent *bev, void *arg)
{
    worker_t *w = (worker_t *)arg;
    struct bufferevent *bev_client;
    struct timeval timeout = {1000, 0};
    struct evbuffer *input = bufferevent_get_input(bev);
    client_t *c;
    int fd, n;

    while (evbuffer_get_length(input) >= sizeof fd) {
        n = evbuffer_remove(input, &fd, sizeof fd);
        if (n != sizeof fd) {
            err_quit("evbuffer_remove");
        }
        evutil_make_socket_nonblocking(fd);
        c = client_create(w);
        bev_client = bufferevent_socket_new(w->base, fd, BEV_OPT_CLOSE_ON_FREE);
        if (!bev_client) {
            err_quit("bufferevent_socket_new");
        }
        c->bev = bev_client;
        bufferevent_setcb(bev_client, broadcast_cb, NULL, echo_errorcb, c);
        bufferevent_enable(bev_client, EV_READ|EV_WRITE);
        bufferevent_set_timeouts(bev_client, &timeout, &timeout);
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

void *worker_loop(void *arg)
{
    worker_t *w = (worker_t *)arg;
    struct event *timer_event;
    struct bufferevent *bev;
    struct timeval timeout = {1, 0};

    bev = bufferevent_socket_new(w->base, w->fd[0], BEV_OPT_CLOSE_ON_FREE);
    if (!bev) {
        err_quit("bufferevent_socket_new");
    }
    bufferevent_setcb(bev, worker_task_readcb, NULL, worker_task_errorcb, w);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    timer_event = event_new(w->base, -1, EV_PERSIST, worker_timer, w);
    if (NULL == timer_event) {
        err_quit("event_new");
    }
    event_add(timer_event, &timeout);
    event_base_dispatch(w->base);
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


