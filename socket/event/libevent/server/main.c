#include <stdio.h>
#include "define.h"
#include "dispatcher.h"

int main(int argc, char **argv)
{
    int ret;

    dispatcher_t *d = dispatcher_create();
    ret = dispatcher_start(d);
    if (ret != 0) {
        err_quit("dispatcher_start");
    }
    for (;;)
        sleep(1);
    return 0;
}
