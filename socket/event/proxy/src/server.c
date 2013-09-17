#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

/* libevent */
#include <event2/event.h>
#include <event2/thread.h>
#include <event2/buffer.h>

/* uthash curl evthr */
#include <uthash.h>
#include <curl/curl.h>
#include "evthr.h"
#include "server.h"
#include "http_parser.h"
#include "sha1.h"
#include "base64_enc.h"

/* Websocket RFC6455 */
#define OPCODE_CONTINUATION_FRAME  0x0
#define OPCODE_TEXT_FRAME          0x1
#define OPCODE_BINARY_FRAME        0x2
#define OPCODE_CONNECTION_CLOSE    0x8
#define OPCODE_PING                0x9
#define OPCODE_PONG                0xa

typedef struct http_request {
    char *url;
    char *Connection;
    char *Upgrade;
    char *Sec_WebSocket_Key;
    char *Sec_WebSocket_Protocol;
    char **current_field;
    int completed;
} http_request_t;

#define ERROR_EXIT do {\
    fprintf(stderr, "fatal errorat %d\n", __LINE__);\
    exit(1);\
} while(0)

#define TRACE(fmt, args...) fprintf(stderr, fmt, ##args)
#define TRACE_ERROR(fmt, args...) fprintf(stderr, "ERROR at line %d" fmt, __LINE__, ##args)

typedef struct conn {
    int fd;
    struct evbuffer *buffer;
    struct event *event;
} conn_t;

typedef struct client {
    char id[32];
    conn_t *conn;
    char *pull_cmd;
    int pull_cmd_len;

    int refcnt;
    pthread_rwlock_t lock;

    /* websocket */
    http_parser parser;
    http_parser_settings parser_settings;
    http_request_t request;
    int handshake;
    int frame_started;

    evthr_t *thr;

    UT_hash_handle hh;
} client_t;

typedef struct task {
    struct evbuffer *input;
    void *data;
} task_t;

typedef struct websocket_head {
    uint8_t fin;
    uint8_t rsv;
    uint8_t opcode;
    uint8_t mask;
    uint32_t length;
    uint8_t mask_key[4];

    int offset;
} websocket_head_t;


/* global variables */
static struct {
    int client_port;
    int rpc_port;
    int thread_num;
    int daemon_mode;
    const char *websocket_secret;
    const char *url;
    evthr_pool_t *pool;
    client_t *id_hash;
    FILE *fp_log;
    pthread_rwlock_t id_hash_lock;
} global = {
    54574,
    54575,
    16,
    0, 
    "258EAFA5-E914-47DA-95CA-C5AB0DC85B11",
    NULL,
    NULL,
    NULL,
    NULL
};

int http_post(const char *url, const char *content, int len, struct evbuffer *output);
size_t curl_cb(void *content, size_t size, size_t nmemb, void *ptr);
evutil_socket_t  listen_on(short port);

conn_t *conn_new();
void conn_free();
client_t *client_new();
void client_free();
task_t *task_new();
void task_free();
int parser_url_cb(http_parser *p, const char *buf, size_t len);
int parser_header_field_cb(http_parser *p, const char *buf, size_t len);
int parser_header_value_cb(http_parser *p, const char *buf, size_t len);
int parser_message_complete_cb(http_parser *p);
void free_request(http_request_t *request);

int http_response(int fd);
int handshake(client_t *client, int fd);
int send_close_frame(int fd);
int add_binary_frame_head(struct evbuffer *buffer);
int add_text_frame_head(struct evbuffer *buffer);
conn_t *conn_new()
{
    conn_t *conn = calloc(1, sizeof(conn_t));
    assert(conn != NULL);

    conn->buffer = evbuffer_new();
    assert(conn->buffer);

    return conn;
}

void conn_free(conn_t *conn)
{
    evbuffer_free(conn->buffer);
    free(conn);
}

client_t *client_new()
{
    client_t *client = calloc(1, sizeof(client_t));
    assert(client != NULL);

    if (pthread_rwlock_init(&client->lock, NULL)) {
        free(client);
        return NULL;
    }

    client->parser_settings.on_url = parser_url_cb;
    client->parser_settings.on_header_field = parser_header_field_cb;
    client->parser_settings.on_header_value = parser_header_value_cb;
    client->parser_settings.on_message_complete = parser_message_complete_cb;


    TRACE("new client 0x%lx\n", (unsigned long)client);
    return client;
}

void client_free(client_t *client)
{
    if (client->pull_cmd) {
        free(client->pull_cmd);
    }

    TRACE("free client 0x%lx\n", (unsigned long)client);
    free(client);
}

void client_inc_ref(client_t * client)
{
    pthread_rwlock_wrlock(&client->lock);
    client->refcnt++;
    pthread_rwlock_unlock(&client->lock);
}

void client_dec_ref(client_t * client)
{
    pthread_rwlock_wrlock(&client->lock);
    if (client->refcnt == 0) {
        pthread_rwlock_unlock(&client->lock);
        client_free(client);
        return;
    } else {
        client->refcnt--;
    }
    pthread_rwlock_unlock(&client->lock);
}

task_t *task_new()
{
    task_t *task = calloc(1, sizeof(task_t));
    assert(task != NULL);

    task->input = evbuffer_new();
    assert(task->input != NULL);
    return task;
}

void task_free(task_t *task)
{
    evbuffer_free(task->input);
    free(task);
}

void hash_del(client_t *client)
{
    client_t *tmp;

    pthread_rwlock_wrlock(&global.id_hash_lock);
    HASH_FIND(hh, global.id_hash, client->id, 32, tmp);
    if (tmp != NULL && tmp == client) {
        HASH_DELETE(hh, global.id_hash, client);
    }
    pthread_rwlock_unlock(&global.id_hash_lock);
}

void echo(evthr_t *thr, void *arg, void *shared)
{
    task_t *task = (task_t *)arg;
    client_t *client = (client_t *)task->data;
    int fd, ret, length;

    /* send output */
    pthread_rwlock_rdlock(&client->lock);
    if (client->conn == NULL) {
        pthread_rwlock_unlock(&client->lock);
        goto closed;
    } else {
        fd = client->conn->fd;
    }
    pthread_rwlock_unlock(&client->lock);

    /* add wesocket frame head */
    ret = add_text_frame_head(task->input);
    assert(ret == 0);

    TRACE("before output\n");
    /* output */
    while (1) {
        length = evbuffer_get_length(task->input);
        TRACE("output buffer length = %d\n", length);
        if (length == 0)
            break;
        ret = evbuffer_write(task->input, fd);
        if (ret > 0) {
            continue;
        } else if (ret == 0) {
            goto closed;
        } else {
            if (errno == EAGAIN) {
                usleep(1000);
                continue;
            }
            goto closed;
        }
    }
    TRACE("after output\n");
closed:
    client_dec_ref(client);
    task_free(task);
}

void worker(evthr_t *thr, void *arg, void *shared)
{
    int ret, post_cmd_len, fd;
    uint32_t length, type, index;
    char *post_cmd;
    struct evbuffer *output;
    task_t *task = (task_t *)arg;
    client_t *tmp, *client = (client_t *)task->data;

    assert(client->refcnt >= 0);
    length = evbuffer_get_length(task->input);
    assert (length > 2 * sizeof(uint32_t));
    ret = evbuffer_remove(task->input, &type, sizeof(type));
    assert(ret == sizeof(type));
    ret = evbuffer_remove(task->input, &index, sizeof(index));
    assert(ret == sizeof(index));

    /* calc the output index */
    if (index != 0) {
        index = index ^ length ^ type;
    }

    pthread_rwlock_rdlock(&client->lock);
    if (client->conn == NULL) {
        pthread_rwlock_unlock(&client->lock);
        goto closed;
    }
    pthread_rwlock_unlock(&client->lock);

    output = evbuffer_new();
    assert(output != NULL);
    switch (type) {
    case TYPE_HEARTBEAT:
        type = TYPE_HEARTBEAT;
        break;
    case TYPE_PULL:
        if (client->id[0] == 0) {
            ret = evbuffer_remove(task->input, client->id, 32);
            assert(ret == 32);

            /* if not in id_hash, store it into id_hash */
            pthread_rwlock_wrlock(&global.id_hash_lock);
            HASH_FIND(hh, global.id_hash, client->id, 32, tmp);
            if (tmp == NULL) {
                HASH_ADD(hh, global.id_hash, id, 32, client);
            }
            pthread_rwlock_unlock(&global.id_hash_lock);

            /* store the pull_cmd */
            client->pull_cmd = calloc(1, 1024);
            assert(client->pull_cmd != NULL);
            ret = evbuffer_remove(task->input, client->pull_cmd, 1024);
            if (ret < 0) {
                /* error */
                type = TYPE_ERROR;
                evbuffer_add_printf(output, "Internal error");
                /*ERROR_EXIT;*/
                TRACE_ERROR("evbuffer_remove failue in client 0x%lx\n", (unsigned long)client);
            } else if (ret == 1024) {
                /* error */
                type = TYPE_ERROR;
                evbuffer_add_printf(output, "Request too long for pull message");
                /*ERROR_EXIT;*/
                TRACE_ERROR("request too long for pull message in client 0x%lx\n", (unsigned long)client);
            }
            client->pull_cmd_len = ret;
        } else {
            ret = evbuffer_drain(task->input, 32);
            assert(ret == 0);
        }

        /* POST */
        ret = http_post(global.url, client->pull_cmd, client->pull_cmd_len, output);
        if (ret < 0) {
            evbuffer_drain(output, evbuffer_get_length(output));
            type = TYPE_ERROR;
            evbuffer_add_printf(output, "http error");
            break;
        }
        /*
        evbuffer_add_printf(output, "helloadfadfasdfasdfsadfadfasdfasdfsadfasdfasdfewifkh iohasdnfasfhsoadfhsdfksafadifioasdfnsafi");
        */
        type = TYPE_PUSH;
        break;
    case TYPE_POST:
        ret = evbuffer_drain(task->input, 32);
        assert(ret == 0);

        post_cmd_len = evbuffer_get_length(task->input);
        post_cmd = calloc(1, post_cmd_len);
        assert(post_cmd != NULL);

        ret = evbuffer_remove(task->input, post_cmd, post_cmd_len);
        assert(ret == post_cmd_len);

        /* POST */
        ret = http_post(global.url, post_cmd, post_cmd_len, output);
        if (ret < 0) {
            evbuffer_drain(output, evbuffer_get_length(output));
            type = TYPE_ERROR;
            evbuffer_add_printf(output, "http error");
            free(post_cmd);
            break;
        }

        free(post_cmd);
        type = TYPE_RESP;
        break;
    default:
        type = TYPE_ERROR;
        evbuffer_add_printf(output, "wrong type");
    }

    /* package head */
    ret = evbuffer_prepend(output, &index, sizeof(index));
    assert(ret == 0);
    ret = evbuffer_prepend(output, &type, sizeof(type));
    assert(ret == 0);
    length = evbuffer_get_length(output);
    ret = evbuffer_prepend(output, &length, sizeof(length));
    assert(ret == 0);


    /* add wesocket frame head */
    ret = add_binary_frame_head(output);
    assert(ret == 0);

    /* send output */
    pthread_rwlock_rdlock(&client->lock);
    if (client->conn == NULL) {
        pthread_rwlock_unlock(&client->lock);
        goto closed;
    } else {
        fd = client->conn->fd;
    }
    pthread_rwlock_unlock(&client->lock);

    while (1) {
        if (evbuffer_get_length(output) == 0)
            break;
        ret = evbuffer_write(output, fd);
        if (ret > 0) {
            continue;
        } else if (ret == 0) {
            goto closed;
        } else {
            if (errno == EAGAIN) {
                usleep(1000);
                continue;
            }
            goto closed;
        }
    }
closed:
    client_dec_ref(client);
    task_free(task);
}

void rpc_push(evthr_t *thr, void *arg, void *shared)
{
    int ret;
    char id[32];
    uint32_t type, index;
    client_t *tmp;
    task_t *rpc, *task = (task_t *)arg;

    /* type & index */
    ret = evbuffer_drain(task->input, 2 * sizeof(uint32_t));
    assert(ret == 0);

    /* id */
    ret = evbuffer_remove(task->input, id, 32);
    assert(ret == 32);

    pthread_rwlock_rdlock(&global.id_hash_lock);
    HASH_FIND(hh, global.id_hash, id, 32, tmp);
    pthread_rwlock_unlock(&global.id_hash_lock);
    if (tmp != NULL) {
        if (tmp->conn != NULL && tmp->pull_cmd) {

            /* new task push */
            rpc = task_new();
            rpc->data = tmp;
            type = TYPE_PULL;
            index = 0;

            ret = evbuffer_add(rpc->input, &type, sizeof(type));
            assert(ret == 0);
            ret = evbuffer_add(rpc->input, &index, sizeof(index));
            assert(ret == 0);
            ret = evbuffer_add(rpc->input, tmp->pull_cmd, tmp->pull_cmd_len);
            assert(ret == 0);

            /* add task to pool */
            client_inc_ref(tmp);
            if (!tmp->thr) {
                tmp->thr = evthr_pool_find_min(global.pool);
                assert(tmp->thr != NULL);
            }
            ret = evthr_defer(tmp->thr, worker, rpc);
            if (ret != EVTHR_RES_OK) {
                /* TODO */
                client_dec_ref(tmp);
                TRACE("thread pool error code = %d\n", ret);
                /*ERROR_EXIT;*/
                TRACE_ERROR("thread defer failed with thr: 0x%lx\n", (unsigned long)tmp->thr);
            }
            TRACE("rpc push %s\n", id);
        }
    }

    task_free(task);
}

#define READ_SIZE 1024
void client_readcb(evutil_socket_t fd, short events, void *arg)
{
    int ret;
    client_t *client = (client_t *)arg;
    conn_t *conn = client->conn;
    task_t *task;
    uint32_t i, length;
    const char *request;
    websocket_head_t head;
    uint8_t *tmp;

    assert(conn != NULL);
    /* timeout close client */
    if (events & EV_TIMEOUT) {
        TRACE("client 0x%lx timeout\n", (unsigned long)client);
        goto closed;
    }

    while (1) {
        ret = evbuffer_read(conn->buffer, fd, READ_SIZE);
        if (ret < 0) {
            if (errno == EAGAIN)
                break;
            if (errno == EINTR)
                continue;
            goto closed;
        } else if (ret == 0) {
            goto closed;
        } else if (ret == READ_SIZE) {
            continue;
        } else {
            break;
        }
    }

    if (!client->handshake) {
        /* TODO handshake */
        length = evbuffer_get_length(conn->buffer);
        TRACE("request length=%d\n", length);
        request = malloc(length);
        evbuffer_copyout(conn->buffer, request, length); 

        http_parser_init(&client->parser, HTTP_REQUEST);
        client->parser.data = client;
        ret = http_parser_execute(&client->parser, &client->parser_settings, request, length);
        if (ret < length) {
            printf("less than length\n");
        }
        ret = evbuffer_drain(conn->buffer, ret);
        assert(ret == 0);
        free(request);

        if (!client->request.completed) {
            http_response(fd);
            goto closed;
        }

        /* TODO: check request */
        if (!client->request.url) {
            http_response(fd);
            goto closed;
        }

        if (!client->request.Sec_WebSocket_Key) {
            http_response(fd);
            goto closed;
        }

        if (!client->request.Upgrade) {
            http_response(fd);
            goto closed;
        }

        if (!client->request.Connection) {
            http_response(fd);
            goto closed;
        }

        ret = handshake(client, fd);
        if (ret == 0) {
            TRACE("handshake done\n");
            client->handshake = 1;
        }
    }

    while (1) {
        TRACE("before parse\n");
        ret = parse_frame(conn->buffer, &head);
        if (ret < 0) {
            break;
        }
        TRACE("after parse\n");
        switch (head.opcode) {
            /* only support binary data */
        case OPCODE_TEXT_FRAME:
        case OPCODE_BINARY_FRAME:

            /* add new task */
            task = task_new();
            assert(task != NULL);
            task->data = client;

            ret = evbuffer_drain(conn->buffer, head.offset);
            assert(ret == 0);
            tmp = malloc(head.length);
            ret = evbuffer_remove(conn->buffer, tmp, head.length);
            assert(ret == head.length);
            for (i = 0; i < head.length; i++) {
                tmp[i] ^= head.mask_key[i % 4];
                printf("%c", tmp[i]);
            }
            printf("\n");
            ret = evbuffer_add(task->input, tmp, head.length);
            assert(ret == 0);
            free(tmp);

            /*
            ret = evbuffer_remove_buffer(conn->buffer, task->input, head.offset + head.length);
            assert(ret == head.offset + head.length);
            */

            client_inc_ref(client);
            /* add task to pool */
            if (!client->thr) {
                client->thr = evthr_pool_find_min(global.pool);
                assert(client->thr != NULL);
            }

            if (head.opcode == OPCODE_BINARY_FRAME)
                ret = evthr_defer(client->thr, worker, task);
            else
                ret = evthr_defer(client->thr, echo, task);
            if (ret != EVTHR_RES_OK) {
                TRACE_ERROR("thread defer failed with errcode: %d\n", ret);
                client_dec_ref(client);
                goto closed;
            }
            break;
        case OPCODE_CONNECTION_CLOSE:
        case OPCODE_PING:
        case OPCODE_PONG:
        default:
            send_close_frame(fd);
            goto closed;
        }
#if 0
        ret = evbuffer_copyout(conn->buffer, &length, sizeof(length));
        if (ret != sizeof(length))
            break;
        if (length + sizeof(length) <= evbuffer_get_length(conn->buffer)) {
            /* add new task */
            task_t *task = task_new();
            assert(task != NULL);
            task->data = client;
            ret = evbuffer_remove(conn->buffer, &length, sizeof(length));
            assert(ret == sizeof(length));
            ret = evbuffer_remove_buffer(conn->buffer, task->input, length);
            assert(ret == length);

            client_inc_ref(client);
            /* add task to pool */
            if (!client->thr) {
                client->thr = evthr_pool_find_min(global.pool);
                assert(client->thr != NULL);
            }

            ret = evthr_defer(client->thr, worker, task);
            if (ret != EVTHR_RES_OK) {
                TRACE_ERROR("thread defer failed with errcode: %d\n", ret);
                client_dec_ref(client);
                goto closed;
            }
        } else {
            break;
        }
#endif
    }
    return;
closed:

    /* close */
    event_del(conn->event);
    close(fd);

    /* delete from hash */
    hash_del(client);
    /* client closed */

    pthread_rwlock_wrlock(&client->lock);
    client->conn = NULL;
    pthread_rwlock_unlock(&client->lock);
    client_dec_ref(client);

    /* free conn */
    event_free(conn->event);
    conn_free(conn);
}

void rpc_readcb(evutil_socket_t fd, short events, void *arg)
{
    int ret;
    conn_t *conn = (conn_t *)arg;
    uint32_t length;

    /* timeout close */
    if (events & EV_TIMEOUT) {
        goto closed;
    }

    while (1) {
        ret = evbuffer_read(conn->buffer, fd, READ_SIZE);
        if (ret < 0) {
            if (errno == EAGAIN)
                break;
            if (errno == EINTR)
                continue;
            goto closed;
        } else if (ret == 0) {
            goto closed;
        } else if (ret == READ_SIZE) {
            continue;
        } else {
            break;
        }
    }

    while (1) {
        ret = evbuffer_copyout(conn->buffer, &length, sizeof(length));
        if (ret != sizeof(length))
            break;
        if (length + sizeof(length) <= evbuffer_get_length(conn->buffer)) {
            /* add new task */
            task_t *task = task_new();
            assert(task != NULL);
            ret = evbuffer_remove(conn->buffer, &length, sizeof(length));
            assert(ret == sizeof(length));
            ret = evbuffer_remove_buffer(conn->buffer, task->input, length);
            assert(ret == length);

            /* add task to pool */
            ret = evthr_pool_defer(global.pool, rpc_push, task);
            if (ret != EVTHR_RES_OK) {
                /*ERROR_EXIT;*/
                TRACE_ERROR("thread pool defer failed with errcode %d\n", ret);
                goto closed;
            }
        } else {
            break;
        }
    }
    return;
closed:
    close(fd);
    event_del(conn->event);
    event_free(conn->event);
    conn_free(conn);
}


void client_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    struct timeval read_timeout = {300, 0};
    client_t *client;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) {
        perror("accept");
        /*ERROR_EXIT;*/
        TRACE_ERROR("accept failed in client_accpet\n");
    } else {
        evutil_make_socket_nonblocking(fd);
        client = client_new();
        assert(client != NULL);
        client->conn = conn_new();
        assert(client->conn != NULL);
        client->conn->event = event_new(base, fd, EV_READ|EV_PERSIST|EV_ET, client_readcb, client);
        assert(client->conn->event != NULL);
        client->conn->fd = fd;
        event_add(client->conn->event, &read_timeout);
    }
}

void rpc_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    struct timeval read_timeout = {3, 0};
    conn_t *conn;
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) {
        perror("accept");
        /*ERROR_EXIT;*/
        TRACE_ERROR("accept failed in client_accpet\n");
    } else {
        evutil_make_socket_nonblocking(fd);
        conn = conn_new();
        assert(conn != NULL);
        conn->event = event_new(base, fd, EV_READ|EV_PERSIST|EV_ET, rpc_readcb, conn);
        assert(conn->event != NULL);
        conn->fd = fd;
        event_add(conn->event, &read_timeout);
    }
}

evutil_socket_t  listen_on(short port)
{
    evutil_socket_t listener;
    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(listener);

    if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(listener, 16)<0) {
        perror("listen");
        return -1;
    }
    return listener;
}

void start(void)
{
    evutil_socket_t listener;
    struct event_base *base;
    struct event *listener_event;

    evthread_use_pthreads();

    /* hash rw locks */
    if (pthread_rwlock_init(&global.id_hash_lock, NULL)) {
        perror("rwlock");
        return;
    }
    base = event_base_new();
    assert(base != NULL);

    /* listern on client_port */
    listener = listen_on(global.client_port);
    assert(listener > 0);
    listener_event = event_new(base, listener, EV_READ|EV_PERSIST, client_accept, (void*)base);
    assert(listener_event != NULL);
    event_add(listener_event, NULL);

    /* listern on rpc_port */
    listener = listen_on(global.rpc_port);
    assert(listener > 0);
    listener_event = event_new(base, listener, EV_READ|EV_PERSIST, rpc_accept, (void*)base);
    assert(listener_event != NULL);
    event_add(listener_event, NULL);

    /* thread pool */
    global.pool = evthr_pool_new(global.thread_num, NULL, NULL);
    evthr_pool_set_backlog(global.pool, 128);

    evthr_pool_start(global.pool);

    event_base_dispatch(base);
}

size_t curl_cb(void *content, size_t size, size_t nmemb, void *ptr)
{
    struct evbuffer *output = (struct evbuffer *)ptr;
    int ret;
    ret = evbuffer_add(output, content, size * nmemb);
    if (ret < 0) {
        return 0;
    } else {
        return size * nmemb;
    }
}

int http_post(const char *url, const char *content, int len, struct evbuffer *output)
{
    CURL *curl;
    int ret;

    curl = curl_easy_init();
    if (!curl) {
        TRACE_ERROR("could not init curl\n");
        return -1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

    ret = curl_easy_perform(curl);


    if (ret != CURLE_OK) {
       TRACE_ERROR("curl_easy_perform failed: %s\n", curl_easy_strerror(ret));
       return -1;
    }
    curl_easy_cleanup(curl);
    return 0;
}


int main(int argc, char **argv)
{
    FILE *fp;
    int opt, fd;
    const char *pid_file = NULL, *log_file = NULL, *opts_short = "c:r:l:p:t:u:d";
    struct option opts_long[] = {
        {"client-port", 1, NULL, 'c'},
        {"rpc-port", 1, NULL, 'r'},
        {"log-file", 1, NULL, 'l'},
        {"pid-file", 1, NULL, 'p'},
        {"thread-number", 1, NULL, 't'},
        {"url-open", 1, NULL, 'u'},
        {"daemon-mode", 0, NULL, 'd'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, opts_short, opts_long, NULL)) != -1) {
        switch (opt) {
        case 'c':
            global.client_port = atoi(optarg);
            break;
        case 'r':
            global.rpc_port = atoi(optarg);
            break;
        case 'l':
            log_file = optarg;
            break;
        case 'p':
            pid_file = optarg;
            break;
        case 't':
            global.thread_num = atoi(optarg);
            break;
        case 'u':
            global.url = optarg;
            break;
        case 'd':
            global.daemon_mode = 1;
            break;
        default:
            printf("unkown option\n");
            break;
        }
    }

    /* run as daemon */
    if (global.daemon_mode) {
        /* redirect the std* file later */
        daemon(0, 1);
    }

    if (log_file) {
        global.fp_log = fopen(log_file, "a+");
        if (global.fp_log == NULL) {
            fprintf(stderr, "fopen");
            ERROR_EXIT;
        }
    }

    if (pid_file) {
        fp = fopen(pid_file, "w");
        if (fp == NULL) {
            fprintf(stderr, "fopen: %s", pid_file);
            ERROR_EXIT;
        }
        fprintf(fp, "%d", getpid());
        fclose(fp);
    }

    if (global.daemon_mode) {
        fd = open("/dev/null", O_RDWR);
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

    /*
    if (sysconf(_SC_OPEN_MAX) < 10000) {
        fprintf(stderr, "open max is %ld\n", sysconf(_SC_OPEN_MAX));
        exit(0);
    }
    */
    start();
    return 0;
}

int parser_url_cb(http_parser *p, const char *buf, size_t len)
{
    client_t *client = (client_t *)p->data;
    client->request.url = malloc(len + 1);
    memcpy(client->request.url, buf, len);
    client->request.url[len] = 0;
    return 0;
}

int parser_header_field_cb(http_parser *p, const char *buf, size_t len)
{
    client_t *client = (client_t *)p->data;
    if (!strncmp("Connection", buf, len)) {
        client->request.current_field = &client->request.Connection;
    } else if (!strncmp("Upgrade", buf, len)) {
        client->request.current_field = &client->request.Upgrade;
    } else if (!strncmp("Sec-WebSocket-Key", buf, len)) {
        client->request.current_field = &client->request.Sec_WebSocket_Key;
    } else if (!strncmp("Sec-WebSocket-Protocal", buf, len)) {
        client->request.current_field = &client->request.Sec_WebSocket_Protocol;
    } else {
        client->request.current_field = 0;
    }
    return 0;
}

int parser_header_value_cb(http_parser *p, const char *buf, size_t len)
{
    client_t *client = (client_t *)p->data;
    if (!client->request.current_field)
        return 0;
    *(client->request.current_field) = malloc(len + 1);
    memcpy(*(client->request.current_field), buf, len);
    (*(client->request.current_field))[len] = 0;
    client->request.current_field = 0;
    return 0;
}

int parser_message_complete_cb(http_parser *p)
{
    client_t *client = (client_t *)p->data;
    client->request.completed = 1;
    return 0;
}

int http_response(int fd)
{
    const char *response = "HTTP/1.1 200 OK\r\n"
            "Server: WebSocket\r\n"
            "Content-Length: 8\r\n"
            "Connection: close\r\n"
            "Content-Type: text/html\r\n"
            "\r\nIt works";
    int ret, length, offset = 0;
    length = strlen(response);
    while (1) {
        ret = write(fd, response + offset, length - offset);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EINTR) {
                usleep(1000);
                continue;
            }
            return -1;
        } else if (ret == 0) {
            return -1;
        } else {
            offset += ret;
            if (offset == length)
                break;
        }
    }
    return 0;
}

int handshake(client_t *client, int fd)
{

    char tmp1[128], tmp2[128], response[1024];
    int ret, length, offset = 0;
    sprintf(tmp1, "%s%s", client->request.Sec_WebSocket_Key, global.websocket_secret);
    sha1(tmp2, tmp1, strlen(tmp1) << 3);
    base64enc(tmp1, tmp2, 20);
    length = sprintf(response, "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n"
            "\r\n", tmp1);
    while (1) {
        ret = write(fd, response + offset, length - offset);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EINTR) {
                usleep(1000);
                continue;
            }
            return -1;
        } else if (ret == 0) {
            return -1;
        } else {
            offset += ret;
            if (offset == length)
                break;
        }
    }
    return 0;
}

/*
 * return values:
 * 0  ---- frame complete
 * -1 ---- frame not complete
 */
int parse_frame(struct evbuffer *buffer, websocket_head_t *head)
{
    uint8_t tmp[16], *cur = tmp;
    uint32_t buf_len;
    buf_len = evbuffer_get_length(buffer);
    memset(head, 0, sizeof(websocket_head_t));

    TRACE("buf_len = %d\n", buf_len);
    if (buf_len < 2) {
        return -1;
    } else if (buf_len < sizeof(tmp)) {
        evbuffer_copyout(buffer, tmp, buf_len);
    } else {
        evbuffer_copyout(buffer, tmp, sizeof(tmp));
    }

    head->fin = (*cur >> 7) & 0x01;
    head->rsv = (*cur >> 4) & 0x07;
    head->opcode = *cur & 0x0f;
    cur++;

    head->mask = (*cur >> 7) & 0x01;
    head->length = *cur & 0x7f;
    cur++;

    TRACE("length1 = %d\n", head->length);
    if (head->length < 0x7e) {
        if (buf_len < 2 + head->mask * 4 + head->length)
            return -1;
    } else if (head->length == 0x7e) {
        head->length = ntohs(*(uint16_t *)cur);
        cur += 2;
        if (buf_len < 4 + head->mask * 4 + head->length)
            return -1;
    } else if (head->length == 0x7f) {
        head->length = ntohl(*(uint32_t *)cur);
        if (head->length) {
            /* head->length > 2^32 - 1 not support now */
        }
        cur += 4;
        head->length = ntohl(*(uint32_t *)cur);
        cur += 4;
        if (buf_len < 10 + head->mask * 4 + head->length)
            return -1;
    }
    if (head->mask) {
        head->mask_key[0] = *cur++;
        head->mask_key[1] = *cur++;
        head->mask_key[2] = *cur++;
        head->mask_key[3] = *cur++;
    }
    head->offset = cur - tmp;
    return 0;
}

int send_close_frame(int fd)
{
    uint8_t tmp[2];
    tmp[0] = 0x88;
    tmp[1] = 0x00;
    while (write(fd, tmp, sizeof(tmp)) < 0) {
        if (errno == EAGAIN || errno == EINTR) {
            usleep(1000);
            continue;
        }
    }
}

int add_binary_frame_head(struct evbuffer *buffer)
{
    uint8_t tmp[16], *cur = tmp;
    uint32_t ret, length = evbuffer_get_length(buffer);

    *cur++ = 0x80 | OPCODE_BINARY_FRAME;
    /* no mask */
    *cur = 0x00;
    if (length < 0x7e) {
        *cur++ |= (uint8_t)length;
    } else if (length <= 0xffff) {
        *cur++ |= (uint8_t)0x7e;
        *((uint16_t *)cur) = htons((uint16_t)length);
        cur += 2;
    } else {
        *cur++ |= (uint8_t)0x7f;
        *((uint32_t *)cur) = 0x00000000;
        cur += 4;
        *((uint32_t *)cur) = htonl(length);
        cur += 4;
    }
    return evbuffer_prepend(buffer, tmp, cur - tmp);
}

int add_text_frame_head(struct evbuffer *buffer)
{
    uint8_t tmp[16], *cur = tmp;
    uint32_t ret, length = evbuffer_get_length(buffer);

    *cur++ = 0x80 | OPCODE_TEXT_FRAME;
    /* no mask */
    *cur = 0x00;
    if (length < 0x7e) {
        *cur++ |= (uint8_t)length;
    } else if (length <= 0xffff) {
        *cur++ |= (uint8_t)0x7e;
        *((uint16_t *)cur) = htons((uint16_t)length);
        cur += 2;
    } else {
        *cur++ |= (uint8_t)0x7f;
        *((uint32_t *)cur) = 0x00000000;
        cur += 4;
        *((uint32_t *)cur) = htonl(length);
        cur += 4;
    }
    return evbuffer_prepend(buffer, tmp, cur - tmp);
}

void free_request(http_request_t *request)
{
    if (request->url)
        free(request->url);
    if (request->Connection)
        free(request->Connection);
    if (request->Upgrade)
        free(request->Upgrade);
    if (request->Sec_WebSocket_Key)
        free(request->Sec_WebSocket_Key);
    if (request->Sec_WebSocket_Protocol)
        free(request->Sec_WebSocket_Protocol);
    memset(request, 0, sizeof(http_request_t));
}

