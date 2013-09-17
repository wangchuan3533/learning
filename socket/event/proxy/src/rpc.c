#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define COUNT 1024

char *id[COUNT];
int count;
pthread_mutex_t lock;

void *read_id(void *arg)
{
    char line[32];
    int serv, client;
    struct sockaddr_in sin;
    struct sockaddr_storage ss;
    int len, ret, port = 54576;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);
    len = sizeof(ss);

    serv = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(serv, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(serv, 16)<0) {
        perror("listen");
        return -1;
    }
 
    while (1) {
        client = accept(serv, (struct sockaddr *)&ss, &len);
        if (client < 0) {
            perror("accept");
            exit(1);
        }
        ret = read(client, line, sizeof(line));
        if (ret > 0) {
            line[ret] = 0;
            pthread_mutex_lock(&lock);
            id[count] = malloc(32);
            memset(id[count], 0, 32);
            strncpy(id[count], line, 32);
            count++;
            pthread_mutex_unlock(&lock);
            close(client);
        } else if (ret == 0) {
            close(client);
        } else {
            perror("ret");
            exit(1);
        }
    }
}

int rpc_push(char *id)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sock, n, ret;
    uint32_t length, type, index;

    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        perror("gethostbyname\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = 0;
    serv_addr.sin_port = htons(54575);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket\n");
        exit(1);
    }

    ret = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        perror("connect");
        exit(1);
    }
    length = 32 + 2 * sizeof(uint32_t);
    type = 6;
    index = 0;
    ret = send(sock, &length, sizeof(length), 0);
    ret = send(sock, &type, sizeof(type), 0);
    ret = send(sock, &index, sizeof(index), 0);
    ret = send(sock, id, 32, 0);
    close(sock);
}
int main()
{
    pthread_t thread_read;
    int i, ret, n;
    if (pthread_mutex_init(&lock, NULL)) {
        perror("mutex_init");
        exit(1);
    }
    count = 0;;

    ret = pthread_create(&thread_read, NULL, read_id, NULL);
    if (ret) {
        perror("thread_create");
        exit(1);
    }

    while (1) {
        pthread_mutex_lock(&lock);
        n = count;
        pthread_mutex_unlock(&lock);
        if (n > 0) {
            i = random() % n;
            printf("%s\n", id[i]);
            rpc_push(id[i]);
        }
        usleep(1000000);
    }

}
