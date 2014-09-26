#ifndef  __DISPATCHER_H_
#define  __DISPATCHER_H_

#include <pthread.h>
#include "worker.h"

typedef struct dispatcher_s {
    struct event_base *base;
    pthread_t thread_id;
    int stop;
    worker_t *worker_list;
} dispatcher_t;

dispatcher_t *dispatcher_create();
void dispatcher_destroy(dispatcher_t **ctx);
int dispatcher_run(dispatcher_t *ctx);
int dispatcher_stop(dispatcher_t *ctx);
int dispatcher_dispatch(dispatcher_t *ctx, int fd);

// private
void dispatcher_accept(int listener, short event, void *arg);

#endif  //__DISPATCHER_H_
