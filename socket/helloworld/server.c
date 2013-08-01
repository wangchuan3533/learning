#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define BUF_SIZE 1024
#define MAX_PENDING 5
int main()
{
    char buf[BUF_SIZE];
    int sock_s, sock_c;
    struct sockaddr_in addr_s, addr_c;
    unsigned int len;
    int received;

    /* Create the TCP socket */
    if ((sock_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket\n");
        exit(1);
    }

    /* Construct the server sockaddr_in structure */
    memset(&addr_s, 0, sizeof(addr_s));
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_s.sin_port = htons(9555);

    /* Bind the server socket */
    if (bind(sock_s, (struct sockaddr *) &addr_s, sizeof(addr_s)) < 0) {
        perror("Failed to bind the server socket\n");
        exit(1);
    }

    /* Listen on the server socket */
    if (listen(sock_s, MAX_PENDING) < 0) {
        perror("Failed to linten on the server socket\n");
        exit(1);
    }

    /* Accept connections */
    while (1) {
        if ((sock_c = accept(sock_s, (struct sockaddr *) &addr_c, &len)) < 0) {
            perror("Failed to accept the connections\n");
            exit(1);
        }
        printf("Client connected: fd=%d\n", sock_c);

        /* Receive messages */
        if ((received = recv(sock_c, buf, BUF_SIZE, 0)) < 0) {
            perror("Failed to receive message from client\n");
            exit(1);
        }

        /* Send bytes and check for more incoming data in the loop */
        while (received > 0) {
            /* Send data back to client */
            if (send(sock_c, buf, received, 0) != received) {
                perror("Failed to send messages to client\n");
                exit(1);
            }
            /* Check for more data */
            if ((received = recv(sock_c, buf, BUF_SIZE, 0)) < 0) {
                perror("Failed to receive more data from client\n");
                exit(1);
            }
        }
        close(sock_c);
    }
    close(sock_s);
    exit(0);
}

