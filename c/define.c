#include <stdio.h>
#define trace(fmt, args...) \
    fprintf(stderr, fmt, ##args)

int main()
{
    trace("hello %s\n", "world");
    return 0;
}

