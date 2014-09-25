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
#include <assert.h>

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
#define HTTP_HEADER_SEC_WEBSOCKET_VERSION "Sec-WebSocket-Version:"

/* Websocket protocol RFC6455 */
#define OPCODE_CONTINUATION_FRAME  0x0
#define OPCODE_TEXT_FRAME          0x1
#define OPCODE_BINARY_FRAME        0x2
#define OPCODE_CLOSE_FRAME         0x8
#define OPCODE_PING_FRAME          0x9
#define OPCODE_PONG_FRAME          0xa

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

typedef enum frame_state_e {
    FRAME_STATE_STEP_0 = 0,
    FRAME_STATE_STEP_1,
    FRAME_STATE_STEP_2,
    FRAME_STATE_STEP_3,
    FRAME_STATE_FINISHED
} frame_state_t;

#define MAX_WEBSOCKET_HEADER_LENGTH 14 //(2 + 8 + 4)
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

    // mask
    uint8_t mask_key[4];

    // state
    frame_state_t state;

    // data;
    uint8_t *data;
    uint64_t length;
    uint64_t cur;

    // double linked list
    struct websocket_frame_s *next;
    struct websocket_frame_s *prev;

} websocket_frame_t;

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
    CLIENT_STATE_ACCEPTED = 0,
    CLIENT_STATE_HTTP_PARSE_STARTED,
    CLIENT_STATE_HANDSHAKE_STARTED,
    CLIENT_STATE_WEBSOCKET_FRAME_LOOP,
} client_state;

struct client_s {
    uint64_t client_id;
    struct bufferevent *bev;
    // http headers
    http_headers_t *headers;
    // websocket frame
    websocket_frame_t *frame;
    // client state
    client_state state;
    // hash handle
    UT_hash_handle hh;
    // close_flag
    int close_flag;
    // context
    server_ctx_t *ctx;
};

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

// TODO filter invalid request
int parse_http(client_t *c)
{
    char *line, *tmp;
    size_t n, len;
    struct evbuffer *b = bufferevent_get_input(c->bev);
    http_headers_t *h = c->headers;
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

void print_frame(websocket_frame_t *f)
{

}


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

void ws_frame_clear(websocket_frame_t *f)
{
    if (f) {
        if (f->data) {
            free(f->data);
        }
        memset(f, 0, sizeof(websocket_frame_t));
    }
}

int parse_frame(client_t *c)
{
    uint8_t buf[MAX_WEBSOCKET_HEADER_LENGTH];
    size_t to_be_read, n, len;
    struct evbuffer *b = bufferevent_get_input(c->bev);
    websocket_frame_t *f = c->frame;
    int i;

    len = evbuffer_get_length(b);
    switch (f->state) {
    case FRAME_STATE_STEP_0:
        to_be_read = 2;
        if (len >= to_be_read) {
            n = evbuffer_remove(b, buf, to_be_read);
            if (n != to_be_read) {
                err_quit("evbuffer_remove step 0");
            }
            // fist byte
            f->fin = (buf[0] >> 7) & 0x1;
            f->rsv1 = (buf[0] >> 6) & 0x1;
            f->rsv2 = (buf[0] >> 5) & 0x1;
            f->rsv3 = (buf[0] >> 4) & 0x1;
            f->opcode = buf[0] & 0xf;

            // second byte
            f->mask = (buf[1] >> 7) & 0x1;
            f->len_7 = buf[1] & 0x7f;
            f->state = FRAME_STATE_STEP_1;
            len -= to_be_read;
        } else {
            return 0;
        }
    case FRAME_STATE_STEP_1:
        switch (f->len_7) {
        case 127:
            to_be_read = 8;
            break;
        case 126:
            to_be_read = 2;
            break;
        default:
            to_be_read = 0;
        }
        if (len >= to_be_read) {
            if (to_be_read) {
                n = evbuffer_remove(b, buf, to_be_read);
                switch (n) {
                case 2:
                    f->len_16 = ntohs(*(uint16_t *)buf);
                    f->length = f->len_16;
                    break;
                case 8:
                    f->len_64 = buf[0];
                    f->len_64 = f->len_64 << 8 + buf[1];
                    f->len_64 = f->len_64 << 8 + buf[2];
                    f->len_64 = f->len_64 << 8 + buf[3];
                    f->len_64 = f->len_64 << 8 + buf[4];
                    f->len_64 = f->len_64 << 8 + buf[5];
                    f->len_64 = f->len_64 << 8 + buf[6];
                    f->len_64 = f->len_64 << 8 + buf[7];
                    f->length = f->len_64;
                    break;
                default:
                    err_quit("Oops evbuffer_remove step 1");
                }
            } else {
                f->length = f->len_7;
            }
            len -= to_be_read;
            f->state = FRAME_STATE_STEP_2;
        } else {
            return 0;
        }
    case FRAME_STATE_STEP_2:
        to_be_read = f->mask ? 4 : 0;
        if (len >= to_be_read) {
            if (to_be_read) {
                n = evbuffer_remove(b, buf, to_be_read);
                if (n != to_be_read) {
                    err_quit("evbuffer_remove step 2");
                }
                memcpy(f->mask_key, buf, to_be_read);
            }
            len -= to_be_read;
            f->state = FRAME_STATE_STEP_3;
            f->data = (uint8_t *)malloc((size_t)f->length);
            if (NULL == f->data) {
                err_quit("malloc");
            }
            f->cur = 0;
        } else {
            return 0;
        }
    case FRAME_STATE_STEP_3:
        n = evbuffer_remove(b, f->data + f->cur, len);
        f->cur += n;
        if (f->cur == f->length) {
            for (i = 0; i < f->cur; i++) {
                f->data[i] ^= f->mask_key[i % 4];
            }
            f->state = FRAME_STATE_FINISHED;
        }
        return 0;
    default:
        return -1;
    }
}


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
        ws_frame_destroy(&((*c)->frame));
        http_headers_destroy(&((*c)->headers));
        free(*c);
        *c = NULL;
    }
}

int broadcast(server_ctx_t *ctx, void *data, unsigned int len)
{

    client_t *itr = NULL, *tmp = NULL;
    int ret;
    HASH_ITER(hh, ctx->client_list, itr, tmp) {
        ret = send_text_frame(itr, data, len);
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
        ret = send_pong_frame(c, f->data, f->length);
        if (ret) {
            // TODO error handling
        }
        break;
    case OPCODE_TEXT_FRAME:
        broadcast(c->ctx, f->data, f->length);
        break;
    case OPCODE_PONG_FRAME:
        // TODO
        break;
    case OPCODE_CONTINUATION_FRAME:
        // TODO not surpport
        break;
    case OPCODE_CLOSE_FRAME:
        ret = send_close_frame(c, f->data, f->length);
        if (ret) {
            // TODO error handling
        }
        c->close_flag = 1;
        break;
    }
    return 0;
}

int send_200_ok(client_t *c)
{
    const char *http_200_ok_response = 
        "HTTP/1.1 200 OK\r\n"
        "Server: WebSocket\r\n"
        "Content-Length: 8\r\n"
        "Connection: close\r\n"
        "Content-Type: text/html\r\n\r\n"
        "It Works";
    size_t len = strlen(http_200_ok_response);

    if (evbuffer_add(bufferevent_get_output(c->bev), http_200_ok_response, len) != 0) {
        return -1;
    }
    return 0;
}

int check_websocket_request(http_headers_t *h)
{
    if (strncasecmp(h->method, "GET", strlen("GET")))
        return -1;
    if (strncasecmp(h->request_uri, "/chat", strlen("/chat")))
        return -1;
    if (strncasecmp(h->connection, "Upgrade", strlen("Upgrade")))
        return -1;
    if (!h->sec_websocket_key)
        return -1;
    return 0;
}

int send_handshake(client_t *c)
{
    const char *_websocket_secret = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const char *handshake_template =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "\r\n";
    char tmp1[256], tmp2[256];
    size_t len;
    int ret;

    snprintf(tmp1, sizeof(tmp1), "%s%s", c->headers->sec_websocket_key, _websocket_secret);
    sha1(tmp2, tmp1, strlen(tmp1) << 3);
    base64enc(tmp1, tmp2, 20);
    ret = evbuffer_add_printf(bufferevent_get_output(c->bev), handshake_template, tmp1);
    //debug
    printf(handshake_template, tmp1);
    if (ret != strlen(handshake_template) + strlen(tmp1) - 2) {
        return -1;
    }
    return 0;
}

int send_frame(client_t *c, websocket_frame_t *f)
{
    int ret, cur = 0;
    uint8_t header[MAX_WEBSOCKET_HEADER_LENGTH];
    struct evbuffer *output = bufferevent_get_output(c->bev);
    
    memset(header, 0, sizeof(header));
    // length
    if (f->length < 126) {
        f->len_7 = f->length;
    } else if (f->length <= 0xffff) {
        f->len_7 = 126;
        f->len_16 = f->length;
    } else {
        f->len_7 = 127;
        f->len_64 = f->length;
    }

    // first byte
    header[cur]  = (f->fin << 7)  & 0xff;
    header[cur] |= (f->rsv1 << 6) & 0xff;
    header[cur] |= (f->rsv2 << 5) & 0xff;
    header[cur] |= (f->rsv3 << 4) & 0xff;
    header[cur] |= (f->opcode)    & 0xff;
    cur++;

    // second byte
    header[cur]  = (f->mask << 7) & 0xff;
    header[cur] |= (f->len_7)     & 0xff;
    cur++;

    // payload length
    switch (f->len_7) {
    case 127:
        header[cur++] = (f->len_64 >> 56) & 0xff;
        header[cur++] = (f->len_64 >> 48) & 0xff;
        header[cur++] = (f->len_64 >> 40) & 0xff;
        header[cur++] = (f->len_64 >> 32) & 0xff;
        header[cur++] = (f->len_64 >> 24) & 0xff;
        header[cur++] = (f->len_64 >> 16) & 0xff;
        header[cur++] = (f->len_64 >> 8)  & 0xff;
        header[cur++] = (f->len_64)       & 0xff;
        break;
    case 126:
        header[cur++] = (f->len_16 >> 8)  & 0xff;
        header[cur++] = (f->len_16)       & 0xff;
        break;
    default:
        break;
    }

    // mask
    if (f->mask) {
        memcpy(header + cur, f->mask_key, 4);
        cur += 4;
    }

    ret = evbuffer_add(output, header, cur);
    if (ret != 0)
        return -1;
    ret = evbuffer_add(output, f->data, f->length);
    if (ret != 0)
        return -1;
    return ret;
}

int send_text_frame(client_t *c, void *data, size_t len)
{
    websocket_frame_t f;

    memset(&f, 0, sizeof(f));

    f.fin = 1;
    f.rsv1 = 0;
    f.rsv2 = 0;
    f.rsv3 = 0;
    f.opcode = OPCODE_TEXT_FRAME;

    f.mask = 0;
    f.length = len;
    f.data = data;

    return send_frame(c, &f);
}

int send_binary_frame(client_t *c, void *data, size_t len)
{
    websocket_frame_t f;

    memset(&f, 0, sizeof(f));

    f.fin = 1;
    f.rsv1 = 0;
    f.rsv2 = 0;
    f.rsv3 = 0;
    f.opcode = OPCODE_BINARY_FRAME;

    f.mask = 0;
    f.length = len;
    f.data = data;

    return send_frame(c, &f);
}

int send_close_frame(client_t *c, void *data, size_t len)
{
    websocket_frame_t f;

    memset(&f, 0, sizeof(f));

    f.fin = 1;
    f.rsv1 = 0;
    f.rsv2 = 0;
    f.rsv3 = 0;
    f.opcode = OPCODE_CLOSE_FRAME;;

    f.mask = 0;
    f.length = len;
    f.data = data;

    return send_frame(c, &f);
}

int send_ping_frame(client_t *c, void *data, size_t len)
{
    websocket_frame_t f;

    memset(&f, 0, sizeof(f));

    f.fin = 1;
    f.rsv1 = 0;
    f.rsv2 = 0;
    f.rsv3 = 0;
    f.opcode = OPCODE_PING_FRAME;;

    f.mask = 0;
    f.length = len;
    f.data = data;

    return send_frame(c, &f);
}

int send_pong_frame(client_t *c, void *data, size_t len)
{
    websocket_frame_t f;

    memset(&f, 0, sizeof(f));

    f.fin = 1;
    f.rsv1 = 0;
    f.rsv2 = 0;
    f.rsv3 = 0;
    f.opcode = OPCODE_PONG_FRAME;;

    f.mask = 0;
    f.length = len;
    f.data = data;

    return send_frame(c, &f);
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
            ret = parse_http(client);
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
            //broadcast(client->ctx, "http", 4);
            // check the websocket request
            ret = check_websocket_request(client->headers);
            if (ret == 0) {
                ret = send_handshake(client);
                client->state = CLIENT_STATE_HANDSHAKE_STARTED;
            // not websocket, send 200 ok, and close socket when finish
            } else {
                ret = send_200_ok(client);
                client->close_flag = 1;
            }
            break;
        case CLIENT_STATE_WEBSOCKET_FRAME_LOOP:
            ret = parse_frame(client);
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

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    server_ctx_t *ctx = (server_ctx_t *)arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    char buf[128];
    struct timeval tv_10_seconds = {1000, 0};

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
        c->bev = bev;
        bufferevent_setcb(bev, readcb, writecb, errorcb, c);
        bufferevent_enable(bev, EV_READ|EV_WRITE);
        bufferevent_set_timeouts(bev, &tv_10_seconds, &tv_10_seconds);
        // debug
        //snprintf(buf, sizeof(buf), "client %lu joined\n", c->client_id);
        //broadcast(ctx, buf, strlen(buf));
    }
}

void run(void)
{
    evutil_socket_t listener;
    struct sockaddr_in sin;
    server_ctx_t *ctx;
    struct event *listener_event;
    int opt = 1;

    ctx = server_ctx_create();

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(8200);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_listen_socket_reuseable(listener);
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
