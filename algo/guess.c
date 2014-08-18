#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

static uint32_t maze;

void init();

int judge(uint32_t value);

int main()
{
    uint32_t maze, min, max, current;
    int ret;

    init();

    for (min = 0, max = 0xffffffff;;) {
        current = (min + max) >> 1;
        ret = judge(current);
        if (ret < 0) {
            max = current;
        } else if (ret > 0) {
            min = current;
        } else {
            printf("yeah, I found out the maze! It's %u\n", current);
            break;
        }
    }
}

void init()
{
    srandom(time(NULL));
    maze = (uint32_t)random();
}

int judge(uint32_t value)
{
    return value - maze;
}
