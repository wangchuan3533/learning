#include <stdio.h>

struct hello {
    int a;
    int b;
};

int main()
{
    printf("%d %d\n", (int)&((struct hello *)0)->a, (int)&((struct hello *)0)->b);
    return 0;
}

