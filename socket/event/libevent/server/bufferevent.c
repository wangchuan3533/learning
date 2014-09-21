/* For sockaddr_in */
#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "uthash.h"
#include "base64_enc.h"
#include "sha1.h"

#define err_quit(fmt, args...) do {\
    fprintf(stderr, fmt, ##args);\
    exit(1);\
} while (0)


#define MAX_HTTP_HEADERS 128
#define HTTP_HEADER_HOST "Host:"
#define HTTP_HEADER_USER_AGENT "User-Agent:"
#define HTTP_HEADER_CONNECTION "Connection:"
#define HTTP_HEADER_UPGRADE "Upgrade:"
#define HTTP_HEADER_SEC_WEBSOCKET_KEY "Sec-WebSocket-Key:"
#define HTTP_HEADER_SEC_WEBSOCKET_PROTOCOL "Sec-WebSocket-Protocol:"
#define HTTP_HEADER_SEC_WEBSOCKET_VERSION "Sec-WebSocket-Verion:"

typedef enum http_headers_state_s {
    HTTP_HEADERS_STATE_STEP_0 = 0,
    HTTP_HEADERS_STATE_PARSED_REQUEST_LINE,
    HTTP_HEADERS_STATE_FINISHED
} http_headers_state_t;

typedef struct http_headers_s {
    //first line

    char *method;
    char *request_uri;
    char *http_version;

    // headers
    char *host;
    char *user_agent;
    char *connection;
    char *upgrade;
    char *sec_websocket_key;
    char *sec_websocket_protocol;
    char *sec_websocket_version;

    // buffers & buffers count
    char *buffers[MAX_HTTP_HEADERS];
    size_t count;
    // state
    http_headers_state_t state;
} http_headers_t;

http_headers_t *http_headers_create()
{
    http_headers_t *h = (http_headers_t *)malloc(sizeof(http_headers_t));
    if (NULL == h) {
        err_quit("malloc");
    }
    memset(h, 0, sizeof(http_headers_t));
    return h;
}

void http_headers_destroy(http_headers_t **h)
{
    int i;
    if (h && *h) {
        for (i = 0; i < (*h)->count; i++) {
            if ((*h)->buffers[i])
                free((*h)->buffers[i]);
        }
        free(*h);
        *h = NULL;
    }
}

int http_parse(struct evbuffer *b, http_headers_t *h)
{
    char *line, *tmp;
    size_t n, len;
    while (line = evbuffer_readln(b, &n, EVBUFFER_EOL_CRLF)) {
        h->buffers[h->count++] = line;
        if (n == 0) {
            // finish
            h->state = HTTP_HEADERS_STATE_FINISHED;
            break;
        }
        if (h->state == HTTP_HEADERS_STATE_STEP_0) {
            h->method = line;

            tmp = strchr(line, ' ');
            if (tmp != NULL) {
                *tmp++ = '\0';
                h->request_uri = tmp;
                tmp = strrchr(tmp, ' ');
                if (tmp != NULL) {
                    *tmp++ = '\0';
                    h->http_version = tmp;
                }
            }
            h->state = HTTP_HEADERS_STATE_PARSED_REQUEST_LINE;
        }
        if (strncasecmp(HTTP_HEADER_HOST, line, strlen(HTTP_HEADER_HOST)) == 0) {
            tmp = strchr(line, ' ');
            h->host = tmp + 1;
        }
        if (strncasecmp(HTTP_HEADER_USER_AGENT, line, strlen(HTTP_HEADER_USER_AGENT)) == 0) {
            tmp = strchr(line, ' ');
            h->user_agent = tmp + 1;
        }
        if (strncasecmp(HTTP_HEADER_CONNECTION, line, strlen(HTTP_HEADER_CONNECTION)) == 0) {
            tmp = strchr(line, ' ');
            h->connection = tmp + 1;
        }
        if (strncasecmp(HTTP_HEADER_UPGRADE, line, strlen(HTTP_HEADER_UPGRADE)) == 0) {
            tmp = strchr(line, ' ');
            h->upgrade = tmp + 1;
        }
        if (strncasecmp(HTTP_HEADER_SEC_WEBSOCKET_KEY, line, strlen(HTTP_HEADER_SEC_WEBSOCKET_KEY)) == 0) {
            tmp = strchr(line, ' ');
            h->sec_websocket_key = tmp + 1;
        }
        if (strncasecmp(HTTP_HEADER_SEC_WEBSOCKET_PROTOCOL, line, strlen(HTTP_HEADER_SEC_WEBSOCKET_PROTOCOL)) == 0) {
            tmp = strchr(line, ' ');
            h->sec_websocket_protocol = tmp + 1;
        }
        if (strncasecmp(HTTP_HEADER_SEC_WEBSOCKET_VERSION, line, strlen(HTTP_HEADER_SEC_WEBSOCKET_VERSION)) == 0) {
            tmp = strchr(line, ' ');
            h->sec_websocket_version = tmp + 1;
        }
    }
    return 0;
}

void print_http_headers(http_headers_t *h)
{
    if (h->method)
        printf("[method]:%s\n", h->method);
    else
        printf("[method]:NULL\n");
    if (h->request_uri)
        printf("[request_uri]:%s\n", h->request_uri);
    else
        printf("[request_uri]:NULL\n");
    if (h->http_version)
        printf("[http_version]:%s\n", h->http_version);
    else
        printf("[http_version]:NULL\n");
    if (h->host)
        printf("[host]:%s\n", h->host);
    else
        printf("[host]:NULL\n");
    if (h->user_agent)
        printf("[user_agent]:%s\n", h->user_agent);
    else
        printf("[user_agent]:NULL\n");
    if (h->connection)
        printf("[connection]:%s\n", h->connection);
    else
        printf("[connection]:NULL\n");
    if (h->upgrade)
        printf("[upgrade]:%s\n", h->upgrade);
    else
        printf("[upgrade]:NULL\n");
    if (h->sec_websocket_key)
        printf("[sec_websocket_key]:%s\n", h->sec_websocket_key);
    else
        printf("[sec_websocket_key]:NULL\n");
    if (h->sec_websocket_protocol)
        printf("[sec_websocket_protocol]:%s\n", h->sec_websocket_protocol);
    else
        printf("[sec_websocket_protocol]:NULL\n");
    if (h->sec_websocket_version)
        printf("[sec_websocket_version]:%s\n", h->sec_websocket_version);
    else
        printf("[sec_websocket_version]:NULL\n");
}

typedef enum frame_state_e {
    FRAME_STATE_STEP_0 = 0,
    FRAME_STATE_STEP_1,
    FRAME_STATE_STEP_2,
    FRAME_STATE_STEP_3,
    FRAME_STATE_FINISHED
} frame_state_t;

#define MAX_websocket_frame_LENGTH 14 //(2 + 8 + 4)
typedef struct websocket_frame_s {
    //first byte
    unsigned fin:1;
    unsigned rsv1:1;   // must be 0
    unsigned rsv2:1;   // must be 0
    unsigned rsv3:1;   // must be 0
    unsigned opcode:4; // 

    // second type
    unsigned mask:1;
    unsigned len_7:7;
    uint16_t len_16;
    uint64_t len_64;
    uint64_t length;

    // mask
    uint8_t mask_key[4];

    // state
    frame_state_t state;

    // data;
    uint8_t *data;
    uint64_t cur;

} websocket_frame_t;

websocket_frame_t *ws_frame_create()
{
    websocket_frame_t *f = (websocket_frame_t *)malloc(sizeof(websocket_frame_t));
    if (NULL == f) {
        err_quit("malloc");
    }
    memset(f, 0, sizeof(websocket_frame_t));
    return f;
}

void ws_frame_destroy(websocket_frame_t **f)
{
    if (f && *f) {
        if ((*f)->data) {
            free((*f)->data);
        }
        free(*f);
        *f = NULL;
    }
}

int parse_frame(struct evbuffer *b, websocket_frame_t *h)
{
    uint8_t buf[MAX_websocket_frame_LENGTH];
    size_t to_be_read, n, len;
    len = evbuffer_get_length(b);
    switch (h->state) {
    case FRAME_STATE_STEP_0:
        to_be_read = 2;
        if (len >= to_be_read) {
            n = evbuffer_remove(b, buf, to_be_read);
            if (n != to_be_read) {
                err_quit("evbuffer_remove step 0");
            }
            // fist byte
            h->fin = (buf[0] >> 7) & 0x1;
            h->rsv1 = (buf[0] >> 6) & 0x1;
            h->rsv2 = (buf[0] >> 5) & 0x1;
            h->rsv3 = (buf[0] >> 4) & 0x1;
            h->opcode = buf[0] & 0xff;

            // second byte
            h->mask = (buf[1] >> 7) & 0x1;
            h->len_7 = buf[1] & 0x7f;
            h->state = FRAME_STATE_STEP_1;
            len -= to_be_read;
        } else {
            return 0;
        }
    case FRAME_STATE_STEP_1:
        switch (h->len_7) {
        case 127:
            to_be_read = 8;
            break;
        case 126:
            to_be_read = 2;
            break;
        default:
            to_be_read = 0;
        }
        if (len > to_be_read) {
            if (to_be_read) {
                n = evbuffer_remove(b, buf, to_be_read);
                switch (n) {
                case 2:
                    h->len_16 = ntohs(*(uint16_t *)buf);
                    h->length = h->len_16;
                    break;
                case 8:
                    h->len_64 = buf[0];
                    h->len_64 = h->len_64 << 8 + buf[1];
                    h->len_64 = h->len_64 << 8 + buf[2];
                    h->len_64 = h->len_64 << 8 + buf[3];
                    h->len_64 = h->len_64 << 8 + buf[4];
                    h->len_64 = h->len_64 << 8 + buf[5];
                    h->len_64 = h->len_64 << 8 + buf[6];
                    h->len_64 = h->len_64 << 8 + buf[7];
                    h->length = h->len_64;
                    break;
                default:
                    err_quit("evbuffer_remove step 1");
                }
            } else {
                h->length = h->len_7;
            }
            len -= to_be_read;
            h->state = FRAME_STATE_STEP_2;
        } else {
            return 0;
        }
    case FRAME_STATE_STEP_2:
        to_be_read = h->mask ? 4 : 0;
        if (len > to_be_read) {
            if (to_be_read) {
                n = evbuffer_remove(b, buf, to_be_read);
                if (n != to_be_read) {
                    err_quit("evbuffer_remove step 2");
                }
                memcpy(h->mask_key, buf, to_be_read);
            }
            len -= to_be_read;
            h->state = FRAME_STATE_STEP_3;
            h->data = (uint8_t *)malloc((size_t)h->length);
            if (NULL == h->data) {
                err_quit("malloc");
            }
            h->cur = 0;
        } else {
            return 0;
        }
    case FRAME_STATE_STEP_3:
        n = evbuffer_remove(b, h->data + h->cur, len);
        h->cur += n;
        if (h->cur == h->length) {
            h->state = FRAME_STATE_FINISHED;
        }
        return 0;
    default:
        return -1;
    }
}

typedef struct client_s client_t;
// server context
typedef struct server_ctx_s {
    // hash of clients
    client_t *client_list;
    // count of clients
    unsigned int client_count;
    // event base
    struct event_base *base;
} server_ctx_t;

// client states
typedef enum client_state_e {
    CLIENT_STATE_STEP_0 = 0,
    CLIENT_STATE_HTTP_FINISHED,
    CLIENT_STATE_HANDSHAKED,
    CLIENT_STATE_CLOSED,
} client_state;

struct client_s {
    uint64_t client_id;
    int fd;
    struct bufferevent *bev;
    // http headers
    http_headers_t *headers;
    // websocket frame
    websocket_frame_t *frame;
    // client state
    client_state state;
    // hash handle
    UT_hash_handle hh;
    // context
    server_ctx_t *ctx;
};

server_ctx_t *server_ctx_create()
{
    server_ctx_t *s = (server_ctx_t *)malloc(sizeof(server_ctx_t));
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

void server_destroy(server_ctx_t **s)
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

client_t *client_create(server_ctx_t *ctx)
{
    client_t *c = (client_t *)malloc(sizeof(client_t));
    if (NULL == c) {
        err_quit("malloc");
    }
    // memset
    memset(c, 0, sizeof(client_t));
    // client_id
    c->ctx = ctx;
    c->client_id = ctx->client_count++;
    c->headers = http_headers_create();
    c->frame = ws_frame_create();
    // add to hash
    HASH_ADD(hh, ctx->client_list, client_id, sizeof(uint64_t), c);
    return c;
}

void client_destroy(client_t **c)
{
    if (c && *c) {
        HASH_DELETE(hh, (*c)->ctx->client_list, *c);
        free((*c)->frame);
        free((*c)->headers);
        free(*c);
        *c = NULL;
    }
}

int broadcast(server_ctx_t *ctx, void *data, unsigned int len)
{

    client_t *itr = NULL, *tmp = NULL;
    struct evbuffer *output;
    HASH_ITER(hh, ctx->client_list, itr, tmp) {
        printf("[DEBUG] [print_list] client_id %lu\n", itr->client_id);
        output = bufferevent_get_output(itr->bev);
        evbuffer_add(output, data, len);
    } 
    return 0;
}

int http_200_ok(struct evbuffer *b)
{
    const char *http_200_ok_response = 
        "HTTP/1.1 200 OK\r\n"
        "Server: WebSocket\r\n"
        "Content-Length: 8\r\n"
        "Connection: close\r\n"
        "Content-Type: text/html\r\n\r\n"
        "It Works";

    evbuffer_add(b, http_200_ok_response, strlen(http_200_ok_response));
    return 0;
}

int handshake(client_t *client)
{
    const char *_websocket_secret = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const char *handshak_template =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "\r\n";
    char tmp1[128], tmp2[128], response[1024];

    sprintf("%s%s", tmp1, client->headers->sec_websocket_key, _websocket_secret);
    sha1(tmp2, tmp1, strlen(tmp1) << 3);
    base64enc(tmp1, tmp2, 20);
    evbuffer_add_printf(handshak_template, tmp1);
    return 0;
}

void readcb(struct bufferevent *bev, void *arg)
{
    struct evbuffer *input, *output;
    client_t *client = (client_t *)arg;
    char buf[1024];
    size_t n;
    int i, ret;

    input = bufferevent_get_input(bev);
    output = bufferevent_get_output(bev);
    // iterate the clients

    while (evbuffer_get_length(input)) {
        switch (client->state) {
        case CLIENT_STATE_STEP_0:
            ret = http_parse(input, client->headers);
            if (ret) {
                err_quit("http parse");
            }
            if (client->headers->state != HTTP_HEADERS_STATE_FINISHED) {
                break;
            }
            client->state = CLIENT_STATE_HTTP_FINISHED;
            print_http_headers(client->headers);
            //broadcast(client->ctx, "http", 4);
        case CLIENT_STATE_HTTP_FINISHED:
            //http_200_ok(output);
            handshake(client);
            client->state = CLIENT_STATE_CLOSED;
            break;
        default:
            err_quit("client state:[%d]\n", client->state);
        }

#if 0
        int n = evbuffer_remove(input, buf, sizeof(buf));
        // debug print out
        printf("[%lu]", client->client_id);
        buf[n] = '\0';
        printf("%s", buf);
        broadcast(client->ctx, buf, n);
#endif
    }
}

void writecb(struct bufferevent *bev, void *arg)
{
    client_t *client = (client_t *)arg;
    if (client->state = CLIENT_STATE_CLOSED) {
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
        /* ... */
    }
    client_destroy(&client);
    bufferevent_free(bev);
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    server_ctx_t *ctx = (server_ctx_t *)arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    char buf[128];
    if (fd < 0) {
        perror("accept");
        //FD_SETSIZE
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        struct bufferevent *bev;
        client_t *c = client_create(ctx);

        evutil_make_socket_nonblocking(fd);
        bev = bufferevent_socket_new(ctx->base, fd, BEV_OPT_CLOSE_ON_FREE);
        c->fd   = fd;
        c->bev  = bev;
        bufferevent_setcb(bev, readcb, writecb, errorcb, c);
        bufferevent_enable(bev, EV_READ|EV_WRITE);
        // debug
        snprintf(buf, sizeof(buf), "client %lu joined\n", c->client_id);
        //broadcast(ctx, buf, strlen(buf));
    }
}

void run(void)
{
    evutil_socket_t listener;
    struct sockaddr_in sin;
    server_ctx_t *ctx;
    struct event *listener_event;

    ctx = server_ctx_create();

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(40713);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(listener);

    if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return;
    }

    if (listen(listener, 16)<0) {
        perror("listen");
        return;
    }

    listener_event = event_new(ctx->base, listener, EV_READ|EV_PERSIST, do_accept, ctx);
    /*XXX check it */
    event_add(listener_event, NULL);

    event_base_dispatch(ctx->base);
}

int main(int argc, char **argv)
{
    run();
    return 0;
}
