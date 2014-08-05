#include <stdio.h>
int main()
{
    int i;
    FILE *fp;

    for (i = 0; i < 100000; i++) {
        printf("hello world\n");
    }
    fp = fopen("/tmp/the_end", "w");
    fprintf(fp, "the end\n");
    fclose(fp);
    return 0;
}
