#include <unistd.h>
#include <errno.h>
#include <getopt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include <event2/buffer.h>

#include "uthash.h"
#include "evthr.h"

#include "server.h"
#include "http.h"
#include "websocket.h"
#include "customerlive.h"

int fprint_time(FILE *fp);
#define LOG(fmt, args...) do {\
    FILE *_fp_log = global.fp_log ? global.fp_log : stderr;\
    fprint_time(_fp_log);\
    fprintf(_fp_log, fmt, ##args);\
} while (0)

#define ERROR_EXIT do {\
    fprintf(stderr, "FATAL ERROR at %s:%d\n", __FILE__, __LINE__);\
    exit(1);\
} while(0)

/* global variables */
static struct {
    int client_port;
    int rpc_port;
    int client_thread_num;
    int rpc_thread_num;
    int daemon_mode;
    const char *url;
    client_t *hash;
    FILE *fp_log;
    pthread_rwlock_t hash_lock;
} global = {
    54574,
    54575,
    6,
    2,
    0, 
    NULL,
    NULL,
    NULL,
};

client_t *client_new()
{
    client_t *client = calloc(1, sizeof(client_t));
    assert(client != NULL);

    if (pthread_rwlock_init(&client->lock, NULL)) {
        free(client);
        return NULL;
    }

    client->request = http_request_header_new();
    assert(client->request);
    client->parser_settings.on_url = parser_url_cb;
    client->parser_settings.on_header_field = parser_header_field_cb;
    client->parser_settings.on_header_value = parser_header_value_cb;
    client->parser_settings.on_message_complete = parser_message_complete_cb;

    LOG("new client 0x%lx\n", (unsigned long)client);
    return client;
}

void client_free(client_t *client)
{
    if (client->pull_cmd) {
        free(client->pull_cmd);
    }
    http_request_header_free(client->request);
    LOG("free client 0x%lx\n", (unsigned long)client);
    hash_del(client);
    free(client);
}

void hash_del(client_t *client)
{
    client_t *tmp;

    pthread_rwlock_wrlock(&global.hash_lock);
    HASH_FIND(hh, global.hash, client->id, 32, tmp);
    if (tmp != NULL && tmp == client) {
        HASH_DELETE(hh, global.hash, client);
    }
    pthread_rwlock_unlock(&global.hash_lock);
}

int print_usage(const char *cmd)
{
    return printf("Usage: %s [OPTION]\n\n"
            "  -c, --client-port   client port\n"
            "  -r, --rpc-port      msg push port\n"
            "  -l, --log-file      log file\n"
            "  -p, --pid-file      pid file\n"
            "  -t, --thread-number thread number\n"
            "  -u, --url-open      url to open.life.qq.com\n"
            "  -d, --daemon-mode   run as daemon\n"
            "  -h, --help          show this usage\n",
            cmd);
}

int client_on_connect(conn_t *conn)
{
    client_t *client = client_new();
    assert(client);

    conn->data = client;
    return 0;
}

int client_on_close(conn_t *conn)
{
    client_t *client = (client_t *)conn->data;
    assert(client);

    client_free(client);
    return 0;
}

int send_output(struct evbuffer *buffer, int fd)
{
    int ret;
    while (1) {
        if (evbuffer_get_length(buffer) == 0)
            return 0;
        ret = evbuffer_write(buffer, fd);
        if (ret > 0) {
            continue;
        } else if (ret == 0) {
            return -1;
        } else {
            if (errno == EAGAIN) {
                usleep(1000);
                continue;
            }
            return -1;
        }
    }
}

int client_handle(client_t *client, uint8_t *buf, int length, struct evbuffer *output)
{
    int ret, post_cmd_len;
    uint8_t type, *cur, *post_cmd;
    client_t *tmp;

    assert(client);
    assert(buf);
    assert(output);
    assert(evbuffer_get_length(output) == 0);

    /* package type */
    cur = buf;
    type = *cur++;

    switch (type) {
    case TYPE_HEARTBEAT:
        type = TYPE_HEARTBEAT;
        break;
    case TYPE_PULL:
        /* package format: [type|id|pull_cmd] */
        if (client->id[0] == 0) {
            /* id: 32 bytes */ 
            memcpy(client->id, cur, 32);
            cur += 32;

            /* if not in hash, store it into it */
            pthread_rwlock_wrlock(&global.hash_lock);
            HASH_FIND(hh, global.hash, client->id, 32, tmp);
            if (tmp == NULL) {
                HASH_ADD(hh, global.hash, id, 32, client);
            }
            pthread_rwlock_unlock(&global.hash_lock);

            /* store the pull_cmd */
            client->pull_cmd_len = length - 32 - 1;
            client->pull_cmd = calloc(1, client->pull_cmd_len);
            assert(client->pull_cmd);
            memcpy(client->pull_cmd, cur, client->pull_cmd_len);
        }

        /* POST */
        ret = http_post(global.url, client->pull_cmd, client->pull_cmd_len, output);
        if (ret < 0) {
            evbuffer_drain(output, evbuffer_get_length(output));
            type = TYPE_ERROR;
            evbuffer_add_printf(output, "http error");
        } else {
            type = TYPE_PUSH;
        }
        break;
    case TYPE_POST:
        /* package format: [type|id|post_cmd] */
        cur += 32;

        post_cmd_len = length - 32 - 1;
        post_cmd = calloc(1, post_cmd_len);
        assert(post_cmd);
        memcpy(post_cmd, cur, post_cmd_len);

        /* POST */
        ret = http_post(global.url, (const char *)post_cmd, post_cmd_len, output);
        if (ret < 0) {
            evbuffer_drain(output, evbuffer_get_length(output));
            type = TYPE_ERROR;
            evbuffer_add_printf(output, "http error");
        } else {
            type = TYPE_RESP;
        }
        free(post_cmd);
        break;
    default:
        type = TYPE_ERROR;
        evbuffer_add_printf(output, "wrong type");
    }

    /* package head */
    ret = evbuffer_prepend(output, &type, sizeof(type));
    assert(ret == 0);

    /* add wesocket frame head */
    return add_binary_frame_head(output);
}

int client_on_receive(conn_t *conn)
{
    struct evbuffer *input = conn->input, *output;
    client_t *client = (client_t *)conn->data;
    websocket_frame_head_t head;
    uint8_t *tmp;
    int i, ret, length, fd = conn->fd;

    assert(client);
    assert(input);

    if (!client->handshake) {
        /* copy the data to a tmp buffer */
        length = evbuffer_get_length(input);
        tmp = malloc(length);
        evbuffer_copyout(input, tmp, length); 

        /* parse http headers */
        http_parser_init(&client->parser, HTTP_REQUEST);
        client->parser.data = client->request;
        ret = http_parser_execute(&client->parser, &client->parser_settings, (const char *)tmp, length);
        if (ret < length) {
            LOG("http parse less than length ret = %d, length = %d\n", ret, length);
        }

        /* remove the parsed data */
        ret = evbuffer_drain(input, ret);
        assert(ret == 0);
        free(tmp);

        if (!client->request->completed) {
            send_http_response(fd);
            return -1;
        }

        /* check request */
        if (!(client->request->url && client->request->Sec_WebSocket_Key
                    && client->request->Upgrade && client->request->Connection)) {
            send_http_response(fd);
            return -1;
        }

        ret = handshake(client->request, fd);
        if (ret == 0) {
            client->handshake = 1;
        }
    }

    while (1) {
        ret = parse_frame(input, &head);
        if (ret < 0) {
            break;
        }
        switch (head.opcode) {
            /* only support binary data */
        case OPCODE_TEXT_FRAME:
            ret = evbuffer_drain(input, head.offset);
            assert(ret == 0);
            tmp = malloc(head.length);
            ret = evbuffer_remove(input, tmp, head.length);
            assert(ret == head.length);
            for (i = 0; i < head.length; i++) {
                tmp[i] ^= head.mask_key[i % 4];
            }
            output = evbuffer_new();
            assert(output);
            ret = evbuffer_add(output, tmp, head.length);
            assert(ret == 0);
            ret = add_text_frame_head(output);
            assert(ret == 0);
            ret = send_output(output, fd);
            if (ret < 0) {
                evbuffer_free(output);
                free(tmp);
                return -1;
            }
            evbuffer_free(output);
            free(tmp);
            break;
        case OPCODE_BINARY_FRAME:

            ret = evbuffer_drain(input, head.offset);
            assert(ret == 0);
            tmp = malloc(head.length);
            ret = evbuffer_remove(input, tmp, head.length);
            assert(ret == head.length);
            for (i = 0; i < head.length; i++) {
                tmp[i] ^= head.mask_key[i % 4];
            }
            output = evbuffer_new();
            assert(output);
            ret = client_handle(client, tmp, head.length, output);
            assert(ret == 0);
            ret = send_output(output, fd);
            if (ret < 0) {
                evbuffer_free(output);
                free(tmp);
                return -1;
            }
            evbuffer_free(output);
            free(tmp);
            break;
        case OPCODE_CONNECTION_CLOSE:
        case OPCODE_PING:
        case OPCODE_PONG:
        default:
            send_close_frame(fd);
            return -1;
        }
    }
    return 0;
}

int start()
{
    server_t *serv; 

    evthread_use_pthreads();

    serv = server_new();
    assert(serv);
    serv->port = 54574;
    serv->thread_num = 8;
    serv->on_connect = client_on_connect;
    serv->on_close = client_on_close;
    serv->on_receive = client_on_receive;
    server_start(serv);

    while (1) {
        sleep(1);
    }
    server_stop(serv);
    server_free(serv);
    return 0;
}

int main(int argc, char **argv)
{
    FILE *fp;
    int opt;
    const char *pid_file = NULL, *log_file = NULL, *opts_short = "c:r:l:p:t:u:dh";
    struct option opts_long[] = {
        {"client-port", 1, NULL, 'c'},
        {"rpc-port", 1, NULL, 'r'},
        {"log-file", 1, NULL, 'l'},
        {"pid-file", 1, NULL, 'p'},
        {"thread-number", 1, NULL, 't'},
        {"url-open", 1, NULL, 'u'},
        {"daemon-mode", 0, NULL, 'd'},
        {"help", 0, NULL, 'h'},
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
            global.client_thread_num = atoi(optarg);
            global.rpc_thread_num = 4;
            break;
        case 'u':
            global.url = optarg;
            break;
        case 'd':
            global.daemon_mode = 1;
            break;
        case 'h':
            print_usage(argv[0]);
            exit(0);
        default:
            printf("unkown option\n");
            break;
        }
    }

    /* run as daemon */
    if (global.daemon_mode) {
        /* redirect the std* file later */
        daemon(0, 0);
    }

    if (log_file) {
        global.fp_log = fopen(log_file, "a+");
        if (global.fp_log == NULL) {
            fprintf(stderr, "fopen");
            ERROR_EXIT;
        }
        setvbuf(global.fp_log, NULL, _IONBF, 0);
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

    /*
    if (sysconf(_SC_OPEN_MAX) < 10000) {
        fprintf(stderr, "open max is %ld\n", sysconf(_SC_OPEN_MAX));
        exit(0);
    }
    */
    start();
    return 0;
}


/* print the time now */
int fprint_time(FILE *fp)
{
    time_t time_now;
    struct tm tm_now;
    char str_now[32];

    time(&time_now);
    localtime_r(&time_now, &tm_now);
    asctime_r(&tm_now, str_now);
    return fprintf(fp, "%s:", str_now);
}
