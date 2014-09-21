#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <pthread.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* libevent */
#include <event2/event.h>
#include <event2/thread.h>
#include <event2/buffer.h>

#include "evthr.h"
#include "server.h"
#include "http.h"

server_t *server_create()
{
    server_t *serv = malloc(sizeof(server_t));
    if (NULL == serv) {
        fprintf(stderr, "malloc\n");
        exit(1);
    }
    return serv;
}

void server_free(server_t *serv)
{
    free(serv);
}

client_t *client_create()
{
    client_t *client = malloc(sizeof(client_t));
    if (NULL == client) {
        fprintf(stderr, "malloc\n");
        exit(1);
    }

    client->input = evbuffer_new();
    assert(client->input);

    if (pthread_mutex_init(&client->lock, NULL) < 0) {
        evbuffer_free(client->input);
        free(client);
        return NULL;
    }

    return client;
}

void client_close(client_t *client)
{
    if (client->serv->on_close) {
        client->serv->on_close(client);
    }
    close(client->fd);

    if (client->event) {
        event_del(client->event);
        event_free(client->event);
    }
}

void client_free(client_t *client)
{
    evbuffer_free(client->input);
    pthread_mutex_destroy(&client->lock);
    free(client);
}

#define READ_SIZE 1024
void readcb(evutil_socket_t fd, short events, void *arg)
{
    int ret, closed = 0;
    client_t *client = (client_t *)arg;
    server_t *serv = client->serv;

    assert(client);
    assert(serv);

    /* timeout close */
    if (events & EV_TIMEOUT) {
        /* TODO */
        client_close(client);
        return;
    }

    /* read */
    while (1) {
        ret = evbuffer_read(client->input, fd, READ_SIZE);
        if (ret < 0) {
            if (errno == EAGAIN)
                break;
            if (errno == EINTR)
                continue;
            closed = 1;
            break;
        } else if (ret == 0) {
            closed = 1;
            break;
        } else if (ret == READ_SIZE) {
            continue;
        } else {
            break;
        }
    }

    /* call the on_receive cb */
    if (evbuffer_get_length(client->input) && serv->on_receive) {
        ret = serv->on_receive(client);
        if (ret < 0) {
            /* TODO */
            closed = 1;
        }
    }
    if (closed) {
        client_close(client);
        client_free(client);
    }
}

void dispatch(evthr_t *thr, void *arg, void *shared)
{
    int fd = (int)arg;
    server_t *serv = (server_t *)shared;
    struct timeval read_timeout = {1800, 0};
    struct event_base *base = evthr_get_base(thr);
    client_t *client;

    evutil_make_socket_nonblocking(fd);
    client = client_create();
    assert(client);
    client->fd = fd;
    client->serv = serv;
    client->event = event_new(base, fd, EV_READ | EV_PERSIST | EV_ET, readcb, client);
    assert(client->event);
    event_add(client->event, &read_timeout);
    
    if (serv->on_clientect) {
        serv->on_clientect(client);
    }
}


void do_accept(evutil_socket_t listener, short event, void *arg)
{
    server_t *serv = (server_t *)arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int ret, fd = accept(listener, (struct sockaddr *)&ss, &slen);
    if (fd < 0) {
        perror("accept");
    } else {
        ret = evthr_pool_defer(serv->pool, dispatch, (void *)fd);
        if (ret != EVTHR_RES_OK) {
            /* TODO */
            printf("evthr_pool_defer\n");
            exit(1);
        }
    }
}

int listen_on(uint16_t port)
{
    evutil_socket_t listener;
    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(listener);
    evutil_make_listen_socket_reuseable(listener);

    if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(listener, 16) < 0) {
        perror("listen");
        return -1;
    }
    return listener;
}

void *server_loop(void *arg);

int server_start(server_t *serv)
{
    int ret;

    ret = pthread_create(&serv->thread, NULL, server_loop, serv);
    assert(ret == 0);
    ret = pthread_detach(serv->thread);
    assert(ret == 0);
    return ret;
}

void *server_loop(void *arg)
{
    server_t *serv = (server_t *)arg;
    assert(serv);

    serv->base = event_base_new();
    assert(serv->base);

    serv->listener = listen_on(serv->port);
    assert(serv->listener > 0);
    serv->listener_event = event_new(serv->base, serv->listener, EV_READ|EV_PERSIST, do_accept, serv);
    assert(serv->listener_event);
    event_add(serv->listener_event, NULL);

    /* thread pool */
    serv->pool = evthr_pool_new(serv->thread_num, NULL, serv);
    evthr_pool_set_backlog(serv->pool, 128);
    evthr_pool_start(serv->pool);

    /* main event loop */
    event_base_dispatch(serv->base);
    pthread_exit(0);
}

int server_stop(server_t *serv)
{
    assert(serv);
    assert(serv->pool);
    evthr_pool_stop(serv->pool);
    return event_base_loopexit(serv->base, NULL);
}

#if 0
/***************TEST*********************/
int my_on_clientect(client_t *client)
{
    return 0;
}

int my_on_close(client_t *client)
{
    return 0;
}

int my_on_receive_echo(client_t *client)
{
    int ret, length;
    assert(client);
    while (1) {
        if ((length = evbuffer_get_length(client->input)) == 0)
            break;
        ret = evbuffer_write(client->input, client->fd);
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
    return 0;
}

int my_on_receive_http(client_t *client)
{
    int ret;
    assert(client);
    ret = http_response(client->fd);
    return -1;
}


int main()
{
    server_t *serv, *serv2; 

    evthread_use_pthreads();

    serv = server_create();
    assert(serv);
    serv->port = 54573;
    serv->thread_num = 4;
    serv->on_clientect = my_on_clientect;
    serv->on_close = my_on_close;
    serv->on_receive = my_on_receive_echo;
    server_start(serv);

    serv2 = server_create();
    assert(serv2);
    serv2->port = 54574;
    serv2->thread_num = 16;
    serv2->on_clientect = my_on_clientect;
    serv2->on_close = my_on_close;
    serv2->on_receive = my_on_receive_http;
    server_start(serv2);


    while (1) {
        sleep(1);
    }
    server_stop(serv);
    server_free(serv);
    server_stop(serv2);
    server_free(serv2);
    return 0;
}
#endif
