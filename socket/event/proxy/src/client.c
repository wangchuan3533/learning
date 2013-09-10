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
#define COUNT 10
#include "server.h"
int main()
{
    int fd[COUNT], ret, n, i;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char recv_buf[BUF_SIZE];
    char id[COUNT][32];
    char request[1024];
    package_head_t head, *recv_head;


    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        perror("gethostbyname\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = 0;
    serv_addr.sin_port = htons(54574);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);


    memset(id, 0, sizeof(id));
    memset(request, 0, sizeof(request));
    strncpy(request, "uin=&skey=&client=customerlive&format=JSON&ip=&cid=&ts=1378354082&method=message.listMessage&args%5Baccount%5D=50008%402481399642&ver=1.0&sig=a9e1453d3db361ed9865e89a31e9ff08", sizeof(request));
    head.length = sizeof(id) + strlen(request) + sizeof(package_head_t) - sizeof(uint32_t);
    head.type = 2;
    head.index = 1;
    srandom(time(NULL));
    for (i = 0; i < COUNT; i++) {
        
        sprintf(id[i], "%ld@2481399642", random());

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
            n = send(fd[i], &head, sizeof(head), 0);
            if (n != sizeof(head)) {
                fprintf(stderr, "ERROR send\n");
                exit(1);
            }
            n = send(fd[i], id, sizeof(id), 0);
            if (n != sizeof(id)) {
                fprintf(stderr, "ERROR send\n");
                exit(1);
            }
            n = send(fd[i], request, strlen(request), 0);
            if (n != strlen(request)) {
                fprintf(stderr, "ERROR send\n");
                exit(1);
            }

            n = recv(fd[i], recv_buf, sizeof(recv_buf), 0);
            recv_head = (package_head_t *) recv_buf;
            if (n != recv_head->length + sizeof(uint32_t)) {
                n += recv(fd[i], recv_buf + n, sizeof(recv_buf), 0);
            }
            printf("length=%d type=%d index=%d\n", recv_head->length, recv_head->type, recv_head->index);
            printf("Received: %s\n", recv_buf + sizeof(package_head_t));
        }
        usleep(random() % 100000);
    }
    for (i = 0; i < COUNT; i++) {
        close(fd[i]);
    }
    exit(0);
}

