#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "evthr.h"

evthr_pool_t *pool = NULL;

typedef struct client {
    struct bufferevent *bev;
    pthread_mutex_t lock;
} client_t;

client_t *client_new(struct bufferevent *bev)
{
    client_t *client = malloc(sizeof(client_t));
    if (!client) {
        return NULL;
    }
    client->bev = bev;

    if (pthread_mutex_init(&client->lock, NULL)) {
        evthr_free(client);
        return NULL;
    }
    return client;
}

void client_free(client_t *client)
{
    free(client);
}


void worker(evthr_t * thr, void * arg, void * shared)
{

    client_t *client = (client_t *)arg;
    struct evbuffer *input, *output;

    pthread_mutex_lock(&client->lock);
    bufferevent_lock(client->bev);
    output = bufferevent_get_output(client->bev);
    input = bufferevent_get_input(client->bev);
    evbuffer_add_buffer(output, input);
    bufferevent_unlock(client->bev);
    pthread_mutex_unlock(&client->lock);

    /*buf = malloc(length);
    n = evbuffer_remove(input, buf, length);
    if (n != length) {
        fprintf(stderr, "Fatal error on evbuffer_remove n = %d, length = %d\n", n, length);
    }
    evbuffer_add(output, buf, length);
    free(buf);
    */
}

void readcb(struct bufferevent *bev, void *ctx)
{

    int ret;
    struct evbuffer *input;
    client_t *client = (client_t *)ctx;
    input = bufferevent_get_input(bev);
    for (;;) {
        ret = evthr_pool_defer(pool, worker, client);
        if (ret == EVTHR_RES_OK) {
            /*fprintf(stderr, "add task success\n");*/
            break;
        }
        if (ret == EVTHR_RES_RETRY || ret == EVTHR_RES_BACKLOG) {
            fprintf(stderr, "retry or full = %d\n", ret);
            usleep(1000);
            continue;
        }
        fprintf(stderr, "thread pool error code = %d\n", ret);
        break;
    }
}

void errorcb(struct bufferevent *bev, short error, void *ctx)
{
    client_t *client = (client_t *)ctx;
    if (error & BEV_EVENT_EOF) {
        /* connection has been closed */
    } else if (error & BEV_EVENT_ERROR) {
        /* check errno to see what error occurred */
        perror("what?");
        /*exit(1);*/
    } else if (error & BEV_EVENT_TIMEOUT) {
        /* must be a timeout event handle, handle it */
    } else {
        printf("what?");
        exit(1);
    }

//    pthread_mutex_lock(&client->lock);
    bufferevent_free(bev);
//    pthread_mutex_unlock(&client->lock);
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    struct timeval read_timeout = {30, 0};
    struct bufferevent *bev;
    client_t *client;
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) {
        perror("accept");
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        evutil_make_socket_nonblocking(fd);
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
        client = client_new(bev);
        bufferevent_setcb(bev, readcb, NULL, errorcb, client);
        bufferevent_set_timeouts(bev, &read_timeout, NULL);
        bufferevent_enable(bev, EV_READ|EV_WRITE);
    }
}

void start(void)
{
    evutil_socket_t listener;
    struct sockaddr_in sin;
    struct event_base *base;
    struct event *listener_event;

    evthread_use_pthreads();

    pool = evthr_pool_new(8, NULL, NULL);

    evthr_pool_start(pool);


    base = event_base_new();
    if (!base)
        return; /*XXXerr*/

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(9501);

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

    listener_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);

    /*XXX check it */
    event_add(listener_event, NULL);

    event_base_dispatch(base);
}

int main(int c, char **v)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    start();
    return 0;
}

