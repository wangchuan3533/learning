#ifndef __PUSHER_H_
#define __PUSHER_H_
#include "define.h"

struct client_s;
struct pusher_s {
    struct event_base *base;
    pthread_t thread_id;
    int stop;
    worker_t *worker_list;
    // client hash
    client_t *client_hash;
};

pusher_t *pusher_create();
void pusher_destroy(pusher_t **p);

int pusher_start();
int pusher_stop();
#endif
