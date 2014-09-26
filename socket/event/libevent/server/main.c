#include <stdio.h>
#include "define.h"
#include "dispatcher.h"

int main(int argc, char **argv)
{
    int ret;

    dispatcher_t *ctx = dispatcher_create();
    ret = dispatcher_run(ctx);
    if (ret != 0) {
        err_quit("dispatcher_run");
    }
    for (;;)
        sleep(1);
    return 0;
}
