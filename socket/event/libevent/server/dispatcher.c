#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <unistd.h>
#include <stdlib.h>

#include "define.h"
#include "dispatcher.h"

void dispatcher_timer(int fd, short event, void *arg)
{
    dispatcher_t *d = (dispatcher_t *)arg;
    printf("dispatcher timer");
    if (d->stop) {
        event_base_loopexit(d->base, NULL);
    }
}

dispatcher_t *dispatcher_create()
{
    dispatcher_t *d = malloc(sizeof(dispatcher_t));
    if (NULL == d) {
        err_quit("malloc");
    }

    memset(d, 0, sizeof(dispatcher_t));
    d->base = event_base_new();
    if (NULL == d->base) {
        err_quit("event_base_new()");
    }

    // init hash
    d->worker_list = NULL;
    return d;
}

void dispatcher_destroy(dispatcher_t **d)
{
    if (d && *d) {
        free(*d);
    }
    *d = NULL;
}

int dispatcher_init(dispatcher_t *d)
{
    struct event *timer_event, *listener_event;
    struct timeval timeout = {1, 0};
    evutil_socket_t listener;
    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(8200);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_listen_socket_reuseable(listener);
    evutil_make_socket_nonblocking(listener);

    if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        err_quit("bind");
    }

    if (listen(listener, 128) < 0) {
        err_quit("listen");
    }

    listener_event = event_new(d->base, listener, EV_READ|EV_PERSIST, dispatcher_accept, d);
    if (NULL == listener_event) {
        err_quit("event_new");
    }

    timer_event = event_new(d->base, -1, EV_PERSIST, dispatcher_timer, d);
    if (NULL == timer_event) {
        err_quit("event_new");
    }
    event_add(timer_event, &timeout);
    event_add(listener_event, NULL);

    // create workers
}

void *dispatcher_loop(dispatcher_t *d)
{
    event_base_dispatch(d->base);
}

int dispatcher_run(dispatcher_t *d)
{
    int ret;
    ret = pthread_create(&(d->thread_id), NULL, dispatcher_loop, d);
    if (ret != 0) {
        err_quit("pthread_create");
    }
    return 0;
}

int dispatcher_stop(dispatcher_t *d)
{
    d->stop = 1;
    return 0;
}

void dispatcher_accept(int listener, short event, void *arg)
{
    dispatcher_t *d = (dispatcher_t *)arg;
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
        dispatcher_dispatch(d, fd);
    }
}

int dispatcher_dispatch(dispatcher_t *d, int fd)
{
    printf("dispatch");
    close(fd);
    return 0;
}

