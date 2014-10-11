#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_NUM 10
int main(int argc,char **argv)
{
    char    *str;
    int i;
    str = (char*)malloc(sizeof(char)*MAX_NUM);
    if(!str)
        exit(-1);

    for(i=0;i<MAX_NUM;i++){
        *(str - i) = 'a';
    }

    free(str);
    return 0;
}
