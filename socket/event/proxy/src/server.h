#ifndef __SERVER__H
#define __SERVER__H

typedef struct client_s client_t;
typedef struct server_s server_t;

struct client_s {
    int fd;
    server_t *serv;
    struct evbuffer *input;
    struct event *event;
    void *data;
    pthread_mutex_t lock;
};
client_t *client_create();
void client_free();

struct server {
    int listener;
    uint16_t port;
    pthread_t thread;
    struct event_base *base;
    struct event *listener_event;
    int thread_num;
    evthr_pool_t *pool;
    int (*on_connect)(conn_t *conn);
    int (*on_close)(conn_t *conn);
    int (*on_receive)(conn_t *conn);
};
server_t *server_create();
void server_free(server_t *serv);

int server_start(server_t *serv);
int server_stop(server_t *serv);

#endif/* __SERVER__H */

