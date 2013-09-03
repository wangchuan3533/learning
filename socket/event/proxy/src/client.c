#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#define BUF_SIZE 16384
#define COUNT 100
int main()
{
    int fd[COUNT], ret, n, i;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char send_buf[BUF_SIZE], recv_buf[BUF_SIZE];

    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        perror("gethostbyname\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = 0;
    serv_addr.sin_port = htons(9501);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    for (i = 0; i < COUNT; i++) {

        fd[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (fd[i] < 0) {
            perror("socket\n");
            exit(1);
        }

        ret = connect(fd[i], (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (ret < 0) {
            perror("connect");
            exit(1);
        }
    }

    for (i = 0; i < COUNT; i++) {
        memset(send_buf, i % 256, sizeof(send_buf));
        n = send(fd[i], send_buf, sizeof(send_buf), 0);
        if (n != sizeof(send_buf)) {
            fprintf(stderr, "ERROR send\n");
            exit(1);
        }
        n = recv(fd[i], recv_buf, sizeof(recv_buf), 0);
        if (n != sizeof(recv_buf)) {
            fprintf(stderr, "ERROR recv\n");
        }
        if (strncmp(send_buf, recv_buf, sizeof(send_buf))) {
            fprintf(stderr, "mismatch\n");
        } else {
            printf("yes");
        }
    }
    for (i = 0; i < COUNT; i++) {
        close(fd[i]);
    }
    exit(0);
}
