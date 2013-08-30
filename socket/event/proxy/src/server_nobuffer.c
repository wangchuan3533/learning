#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/thread.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#define threadpool 1

#ifdef threadpool
#include "threadpool.h"
#else
#include "thpool.h"
#endif

#define THREAD 4
#define QUEUE  1024
#define MAX_LINE 16384


struct server;
#ifdef threadpool
threadpool_t *pool;
#else
thpool_t* threadpool;             /* make a new thread pool structure     */
#endif

void readcb(evutil_socket_t fd, short events, void *arg);
void writecb(evutil_socket_t fd, short events, void *arg);
struct client_t {
    char input[MAX_LINE];
    char output[MAX_LINE];
    size_t in_len, out_len;
    int fd;

    struct event *read_event;
    struct event *write_event;
};

struct client_t *client_new(struct event_base *base, evutil_socket_t fd)
{
    struct client_t *client = malloc(sizeof(struct client_t));
    if (!client)
        return NULL;
    client->read_event = event_new(base, fd, EV_READ|EV_PERSIST, readcb, client);
    if (!client->read_event) {
        free(client);
        return NULL;
    }
    client->write_event = event_new(base, fd, EV_WRITE, writecb, client);

    if (!client->write_event) {
        event_free(client->read_event);
        free(client);
        return NULL;
    }

    client->in_len = client->out_len = 0;

    assert(client->write_event);
    return client;
}

void client_free(struct client_t *client)
{
    event_free(client->read_event);
    event_free(client->write_event);
    free(client);
}



#define BUF_SIZE 8192
void worker(void *arg)
{
    struct client_t *client = (struct client_t *)arg;
    int n, count = 0, offset = 0;
    while (1) {
        n = recv(client->fd, client->input + offset, BUF_SIZE, 0);
        if (n <= 0)
            break;
        count += n;
        offset += n;
    }
    if (n == 0) {
        client_free(client);
    } else if (n < 0) {
        if (errno != EAGAIN) {
            perror("recv");
            client_free(client);
        } else if (count > 0) {
            client->in_len = count;
            memcpy(client->output, client->input, count);
            client->out_len = count;
            event_add(client->write_event, NULL);
        }
    }
}

void readcb(evutil_socket_t fd, short events, void *arg)
{
    struct client_t *client = (struct client_t *)arg;
    int ret;

#ifdef threadpool 
    ret = threadpool_add(pool, &worker, client, 0);
    if (ret == 0) {
    } else  if (ret == threadpool_queue_full) {
        fprintf(stderr, "task queue full\n");
    } else {
        fprintf(stderr, "thread pool error code = %d\n", ret);
    }
#else 
	thpool_add_work(threadpool, &worker, client);
#endif
}
void writecb(evutil_socket_t fd, short events, void *arg)
{
    int offset = 0, n;
    struct client_t *client = (struct client_t *)arg;
    while (offset < client->out_len) {
        n = send(fd, client->input + offset, client->out_len - offset, 0);
        if (n < 0) {
            if (errno == EAGAIN)
                break;
            client_free(client);
            perror("send");
            return;
        }
        offset += n;
    }
    if (offset == client->out_len)
        client->out_len = 0;
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    struct timeval read_timeout = {30, 0};
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) {
        perror("accept");
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        struct client_t *client;
        evutil_make_socket_nonblocking(fd);
        client = client_new(base, fd);
        if (client == NULL) {
            printf(stderr, "client");
            exit(1);
        }
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

#ifdef threadpool 
    assert((pool = threadpool_create(THREAD, QUEUE, 0)) != NULL);
    fprintf(stderr, "Pool started with %d threads and "
            "queue size of %d\n", THREAD, QUEUE);
#else
	threadpool=thpool_init(4);        /* initialise it to 4 number of threads */
#endif
    event_base_dispatch(base);
}

int main(int c, char **v)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    start();
    return 0;
}

