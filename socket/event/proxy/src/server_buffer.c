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

#define threadpool 1

#ifdef threadpool
#include "threadpool.h"
#else
#include "thpool.h"
#endif

#define THREAD 4
#define QUEUE  1024


struct server;
#ifdef threadpool
threadpool_t *pool;
#else
thpool_t* threadpool;             /* make a new thread pool structure     */
#endif

void worker(void *arg)
{

    char *buf;
    struct evbuffer *input, *output;
    size_t length, n;
    struct bufferevent *bev = (struct bufferevent *)arg;

    input = bufferevent_get_input(bev);
    output = bufferevent_get_output(bev);
    length = evbuffer_get_length(input);
    evbuffer_remove_buffer(input, output, length);

    //buf = malloc(length);
    //n = evbuffer_remove(input, buf, length);
    //if (n != length) {
        // TODO
        //fprintf(stderr, "Fatal error on evbuffer_remove n = %d, length = %d\n", n, length);
    //}
    // HTTP POST
    //evbuffer_add(output, buf, length);
    //free(buf);
}

void readcb(struct bufferevent *bev, void *ctx)
{

    int ret, length, n;
    struct evbuffer *input, *output;
    /*
    input = bufferevent_get_input(bev);
    output = bufferevent_get_output(bev);
    length = evbuffer_get_length(input);
    n = evbuffer_drain(input, length);
    evbuffer_add(output, "hello", 6);
    */

    //worker(bev);
#ifdef threadpool 
    ret = threadpool_add(pool, &worker, bev, 0);
    if (ret == 0) {
        //fprintf(stderr, "add task success\n");
    } else  if (ret == threadpool_queue_full) {
        fprintf(stderr, "task queue full\n");
    } else {
        fprintf(stderr, "thread pool error code = %d\n", ret);
    }
#else 
	thpool_add_work(threadpool, &worker, bev);
#endif
}

void errorcb(struct bufferevent *bev, short error, void *ctx)
{
    if (error & BEV_EVENT_EOF) {
        /* connection has been closed */
    } else if (error & BEV_EVENT_ERROR) {
        /* check errno to see what error occurred */
        // TODO
        perror("what?");
        exit(1);
    } else if (error & BEV_EVENT_TIMEOUT) {
        /* must be a timeout event handle, handle it */
        // TODO
    } else {
        printf("what?");
        exit(1);
    }
    bufferevent_free(bev);
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    struct timeval read_timeout = {30, 0};
    struct bufferevent *bev;
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) {
        perror("accept");
        // TODO
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        evutil_make_socket_nonblocking(fd);
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
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

