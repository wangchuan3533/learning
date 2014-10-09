#include "define.h"
#include "pusher.h"
#include "worker.h"

typedef struct pusher_ctx_s {
    struct event_base *base;
    // client hash
    client_t *client_hash;
} pusher_ctx_t;
pusher_t *pusher_create();

void pusher_destroy(pusher_t **p);

int pusher_start();
int pusher_stop():
