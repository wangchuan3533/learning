#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/thread.h>
#include <event2/buffer.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "evthr.h"

evthr_pool_t *pool = NULL;

typedef struct client_s {
    struct event *read_event;
    pthread_mutex_t lock;
} client_t;

typedef struct task_s {
    int fd;
    struct evbuffer *buffer;
} task_t;

void readcb(evutil_socket_t fd, short events, void *arg);
void worker(evthr_t *thr, void *arg, void *shared);
void writer(evthr_t *thr, void *arg, void *shared);

client_t *client_new(struct event_base *base, evutil_socket_t fd)
{
    client_t *client = malloc(sizeof(client_t));
    if (!client) {
        return NULL;
    }

    if (pthread_mutex_init(&client->lock, NULL)) {
        free(client);
        return NULL;
    }

    client->read_event = event_new(base, fd, EV_READ|EV_PERSIST|EV_ET, readcb, client);
    if (!client->read_event) {
        free(client);
        return NULL;
    }

    return client;
}

void client_free(client_t *client)
{
    event_free(client->read_event);
    pthread_mutex_destroy(&client->lock);
    free(client);
}

task_t *task_new(int fd)
{    
    task_t *task = malloc(sizeof(task_t));
    task->buffer = evbuffer_new();
    task->fd = fd;
    return task;
}

void task_free(task_t *task)
{
    evbuffer_free(task->buffer);
    free(task);
}

void worker(evthr_t *thr, void *arg, void *shared)
{
    task_t *input, *output;
    int ret;
    input = (task_t *)arg;

    /* process the task */
    output = task_new(input->fd);
    ret = evbuffer_add_buffer(output->buffer, input->buffer);
    if (ret < 0) {
        printf("evbuffer_add_buffer\n");
        exit(1);
    }
    task_free(input);

    ret = evthr_pool_defer(pool, writer, output);
    if (ret == EVTHR_RES_OK) {
        return;
    } else if (ret == EVTHR_RES_RETRY || ret == EVTHR_RES_BACKLOG) {
        fprintf(stderr, "retry or full = %d\n", ret);
        exit(1);
    } else {
        fprintf(stderr, "thread pool error code = %d\n", ret);
        exit(1);
    }
}

void writer(evthr_t *thr, void *arg, void *shared)
{
    task_t *input;
    int ret;

    input = (task_t *)arg;
    while (1) {
        /* todo */
        ret = evbuffer_write(input->buffer, input->fd);
        if (evbuffer_get_length(input->buffer) == 0) {
            break;
        } else if (ret > 0) {
           continue;
        } else if (ret == 0) {
            /* connection closed TODO:close */
            /*close(input->fd);*/
            break;
        } else if (ret < 0 && errno == EAGAIN) {
            usleep(1000);
            continue;
        } else {
            perror("evbuffer_write\n");
            exit(1);
        }
    }
    task_free(input);
}

void readcb(evutil_socket_t fd, short events, void *arg)
{
    int ret;
    client_t *client = (client_t *)arg;
    task_t *task;

    task = task_new(fd);
    while (1) {
        ret = evbuffer_read(task->buffer, fd, 8192);
        if (ret == 0) {
            /* closed */

            shutdown(fd, SHUT_RDWR);
            close(fd);
            task_free(task);
            event_del(client->read_event);
            client_free(client);
            printf("closed fd=%d\n", fd);
            return;
        } else if (ret < 0 && errno == EAGAIN) {
            break;
        }
    }
    ret = evthr_pool_defer(pool, worker, task);
    if (ret == EVTHR_RES_OK) {
        return;
    } else if (ret == EVTHR_RES_RETRY || ret == EVTHR_RES_BACKLOG) {
        fprintf(stderr, "retry or full = %d\n", ret);
        exit(1);
    } else {
        fprintf(stderr, "thread pool error code = %d\n", ret);
        exit(1);
    }
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    struct timeval read_timeout = {300, 0};
    client_t *client;
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) {
        perror("accept");
/*    } else if (fd > FD_SETSIZE) {
        close(fd);*/
    } else {
        evutil_make_socket_nonblocking(fd);
        client = client_new(base, fd);
        event_add(client->read_event, &read_timeout);
    }
}

void start(void)
{
    evutil_socket_t listener;
    struct sockaddr_in sin;
    struct event_base *base;
    struct event *listener_event;

    evthread_use_pthreads();

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

    base = event_base_new();
    if (!base)
        return; /*XXXerr*/

    listener_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);

    /*XXX check it */
    event_add(listener_event, NULL);
    
    pool = evthr_pool_new(8, NULL, NULL);
    evthr_pool_set_backlog(pool, 1024);

    evthr_pool_start(pool);

    event_base_dispatch(base);
}

int main(int c, char **v)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    if (sysconf(_SC_OPEN_MAX) < 10000) {
        fprintf(stderr, "open max is %ld\n", sysconf(_SC_OPEN_MAX));
        exit(0);
    }
    start();
    return 0;
}

