#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *ent;
    dir = opendir(argv[1]);
    if (NULL == dir) {
        perror("failed to open dir");
        exit(1);
    }

    while ((ent = readdir(dir)) != NULL) {
        printf("%s\tino:%d\toff:%d\treclen:%d\ttype:%d\n",
                ent->d_name,
                ent->d_ino,
                ent->d_off,
                ent->d_reclen,
                ent->d_type);
    }
    exit(0);
}
