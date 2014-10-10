#include <stdio.h>
#include "define.h"
#include "dispatcher.h"
#include "worker.h"
#include "pusher.h"

int main(int argc, char **argv)
{
    int ret, i;
    dispatcher_t *d;
    worker_t *w;
    pusher_t *p;

    for (i = 0; i < WORKER_NUM; i++) {
        w = worker_create();
        w->next = global.workers;
        global.workers = w;
        ret = worker_start(w);
        if (ret != 0) {
            err_quit("worker_start");
        }
    }

    p = pusher_create();
    ret = pusher_start(p);
    if (ret != 0) {
        err_quit("dispatcher_start");
    }

    d = dispatcher_create();
    ret = dispatcher_start(d);
    if (ret != 0) {
        err_quit("dispatcher_start");
    }

    for (;;)
        sleep(1);
    return 0;
}
