#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
    int fd, ret;

    fd = open("/tmp/hello.txt", O_WRONLY | O_CREAT);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    ret = daemon(0, 0);
    if (ret) {
        perror("daemon");
        exit(1);
    }

    ret = dup2(fd, STDOUT_FILENO);
    if (ret != STDOUT_FILENO) {
        perror("dup2");
        exit(1);
    }

    printf("hello world\n");
    close(fd);
}
