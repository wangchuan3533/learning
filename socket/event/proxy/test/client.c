#include <netinet/in.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#define BUF_SIZE 1024*64
#define COUNT 100
int main()
{
    int fd[COUNT], ret, i, j, offset;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buf_send[BUF_SIZE], buf_recv[BUF_SIZE];

    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        perror("gethostbyname\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = 0;
    serv_addr.sin_port = htons(54573);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);


    srandom(time(NULL));
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

   while (1) {
        for (i = 0; i < COUNT; i++) {
            for (j = 0; j < i; j++) {
                buf_send[j] = random() % 0x100;
            }
            /* send */
            offset = 0;
            while (1) {
                ret = write(fd[i], buf_send + offset, sizeof(buf_send) - offset);
                if (ret == 0) {
                    printf("closed\n");
                    goto closed;
                } else if (ret < 0) {
                    printf("send error\n");
                    goto closed;
                } else {
                    offset += ret;
                    if (offset == sizeof(buf_send))
                        break;
                }
            }

            /* receive */
            offset = 0;
            while (1) {
                ret = read(fd[i], buf_recv + offset, sizeof(buf_recv) - offset);
                if (ret == 0) {
                    printf("closed\n");
                    goto closed;
                } else if (ret < 0) {
                    printf("send error\n");
                    goto closed;
                } else {
                    offset += ret;
                    if (offset == sizeof(buf_send))
                        break;
                }
            }
            for (j = 0; j < sizeof(buf_send); j++) {
                if (buf_send[j] != buf_recv[j]) {
                    printf("mismatch\n");
                    goto closed;
                }
            }
            printf("yes\n");
            /*usleep(random() % 100000);*/
        }
    }

closed:
    for (i = 0; i < COUNT; i++) {
        close(fd[i]);
    }
    exit(0);
}

