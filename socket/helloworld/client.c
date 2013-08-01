#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define BUF_SIZE 1024
int main()
{
    int sock;
    struct sockaddr_in addr_s;
    char buf[BUF_SIZE], buf_recv[BUF_SIZE];

    int received, i;

    /* Create the socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create the socket\n");
        exit(1);
    }

    /* Construct the sockaddr_in structure */
    memset(&addr_s, 0, sizeof(addr_s));
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_s.sin_port = htons(9555);

    /* Establish the connection */
    if (connect(sock, (struct sockaddr *) &addr_s, sizeof(addr_s)) < 0) {
        perror("Failed to connect to the server\n");
        exit(1);
    }

    /* Send the message to server */
    for (i = 0; i < 10; i++) {
        memset(buf, 'A' + i, 128);
        if (send(sock, buf, 128, 0) != 128) {
            perror("Failed to send message to server\n");
            exit(1);
        }
        /* Receive data */
        if ((received = recv(sock, buf_recv, BUF_SIZE, 0) ) < 0) {
            perror("Failed to receive message from server\n");
            exit(1);
        }
        buf_recv[128] = 0;
        printf("Received:%s\n", buf_recv);
        sleep(random()%5);
    }
    exit(0);
}
