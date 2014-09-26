#include <stdio.h>
#include "define.h"
#include "dispatcher.h"
#include "worker.h"

int main(int argc, char **argv)
{
    int ret;

    dispatcher_t *d = dispatcher_create();
    worker_t *w = worker_create();
    ret = dispatcher_init(d);
    if (ret != 0) {
        err_quit("dispatcher_init");
    }
    ret = dispatcher_run(d);
    if (ret != 0) {
        err_quit("dispatcher_run");
    }
    ret = worker_run(w);
    for (;;)
        sleep(1);
    return 0;
}
