#include "define.h"
#include "websocket.h"
#include "worker.h"
worker_t *worker_create()
{
    worker_t *s = (worker_t *)malloc(sizeof(worker_t));
    if (NULL == s) {
        err_quit("malloc");
    }
    s->client_list = NULL;
    s->client_count = 0;
    s->base = event_base_new();
    if (NULL == s->base) {
        err_quit("event_base_new");
    }
    return s;
}

void server_destroy(worker_t **s)
{
    client_t *itr = NULL, *tmp = NULL;
    // destroy event_base
    if (s && *s) {
        // delete hash
        //HASH_ITER(hh, s->client_list, itr, tmp) {
            // delete the client?
        //}
        //if (NULL != s->base)
        //event_base_destroy(s->base);
        free(*s);
        *s = NULL;
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

int broadcast(worker_t *w, void *data, unsigned int len)
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
        broadcast(c->worker, f->data, f->length);
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

void readcb(struct bufferevent *bev, void *arg)
{
    client_t *client = (client_t *)arg;
    struct evbuffer *input = bufferevent_get_input(bev);
    char buf[1024];
    size_t n;
    int i, ret;

    assert(bev == client->bev);
    // iterate the clients

    while (evbuffer_get_length(input)) {
        switch (client->state) {
        case CLIENT_STATE_ACCEPTED:
            client->state = CLIENT_STATE_HTTP_PARSE_STARTED;
        case CLIENT_STATE_HTTP_PARSE_STARTED:
            ret = parse_http(bufferevent_get_input(client->bev), client->headers);
            // http parse error
            if (ret != 0) {
                // TODO error handling
                return;
            }
            if (client->headers->state != HTTP_HEADERS_STATE_FINISHED) {
                break;
            }

            // parse finished
            print_http_headers(client->headers);
            //broadcast(client->worker, "http", 4);
            // check the websocket request
            ret = check_websocket_request(client->headers);
            if (ret == 0) {
                ret = send_handshake(bufferevent_get_output(client->bev), client->headers->sec_websocket_key);
                client->state = CLIENT_STATE_HANDSHAKE_STARTED;
            // not websocket, send 200 ok, and close socket when finish
            } else {
                ret = send_200_ok(bufferevent_get_output(client->bev));
                client->close_flag = 1;
            }
            break;
        case CLIENT_STATE_WEBSOCKET_FRAME_LOOP:
            ret = parse_frame(bufferevent_get_input(client->bev), client->frame);
            if (ret != 0) {
                // TODO error handling
                return;
            }
            if (client->frame->state == FRAME_STATE_FINISHED) {
                // debug
                ret = handle(client);
                if (ret) {
                    // TODO error handling
                }
                // clear
                ws_frame_clear(client->frame);
                client->state = CLIENT_STATE_WEBSOCKET_FRAME_LOOP;
            }
            // send client's frame to co worker

            break;
        case CLIENT_STATE_HANDSHAKE_STARTED:
            // waiting write to complete, do not read
            return;
        default:
            err_quit("Oops read client state:[%d]\n", client->state);
        }
    }
}

void writecb(struct bufferevent *bev, void *arg)
{
    client_t *client = (client_t *)arg;

    switch (client->state) {
    case CLIENT_STATE_HANDSHAKE_STARTED:
        client->state = CLIENT_STATE_WEBSOCKET_FRAME_LOOP;
        break;
    case CLIENT_STATE_HTTP_PARSE_STARTED:
    case CLIENT_STATE_WEBSOCKET_FRAME_LOOP:
        break;
    }

    if (client->close_flag) {
        client_destroy(&client);
        bufferevent_free(bev);
    }
}

void errorcb(struct bufferevent *bev, short error, void *arg)
{
    client_t *client = (client_t *)arg;
    if (error & BEV_EVENT_EOF) {
        /* connection has been closed, do any clean up here */
        /* ... */
    } else if (error & BEV_EVENT_ERROR) {
        /* check errno to see what error occurred */
        /* ... */
    } else if (error & BEV_EVENT_TIMEOUT) {
        /* must be a timeout event handle, handle it */
        switch (client->state) {
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
            err_quit("Opps client's state = %d\n", client->state);
        }
            // write time out
        /* ... */
    }
    client_destroy(&client);
    bufferevent_free(bev);
}



