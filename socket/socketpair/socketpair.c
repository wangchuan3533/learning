#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int sockfd[2];
    pid_t pid;
    char send[] = "hello world";
    char recv[] = "hello world";

    if (socketpair(AF_LOCAL, SOCK_DGRAM, 0, sockfd) < 0) {
        fprintf(stderr, "socketpair");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork()");
        exit(1);
    }
    if (pid == 0) {
        close(sockfd[0]);
        read(sockfd[1], recv, sizeof(recv));
        printf("%s\n", recv);
    } else {
        close(sockfd[1]);
        write(sockfd[0], send, sizeof(send));
        wait(NULL);
        printf("child exited\n");
    }
    return 0;
}
