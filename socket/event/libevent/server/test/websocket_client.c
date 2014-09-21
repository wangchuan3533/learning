#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
const char *websocket_request = 
        "GET /chat HTTP/1.1\r\n"
        "Host: server.example.com\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Origin: http://example.com\r\n"
        "Sec-WebSocket-Protocol: chat, superchat\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";

int main(int argc, char **argv)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[128];
    int fd, ret, n;
    
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        perror("gethostbyname\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = 0;
    serv_addr.sin_port = htons(2006);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(1);
    }
    ret = connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        perror("connect");
        exit(1);
    }

    n = send(fd, websocket_request, strlen(websocket_request), 0);
    if (n != strlen(websocket_request)) {
        printf("send failed\n");
        exit(1);
    }
    n = recv(fd, buffer, sizeof(buffer), 0);
    buffer[n] = 0;
    printf(buffer);
    close(fd);
}
