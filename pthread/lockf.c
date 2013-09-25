#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define BUF_SIZE 1024 * 128 
pthread_mutex_t lock;
int count = 0;
int write_0(int fd)
{
    unsigned char buf[BUF_SIZE];
    int ret, offset = 0, len = sizeof(buf);
    memset(buf, 0, sizeof(buf));
    ret = lockf(fd, F_LOCK, 0);
    assert(ret == 0);
    assert(count == 0);
    count++;
    //pthread_mutex_lock(&lock);
    while (1) {
        ret = write(fd, buf + offset, len - offset);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EINTR) {
                usleep(1000);
                continue;
            }
            printf("closed\n");
            exit(1);
        } else if (ret == 0) {
            printf("closed\n");
            exit(1);
        } else {
            offset += ret;
            if (offset == len)
                break;
        }
    }
    //pthread_mutex_unlock(&lock);
    count--;
    assert(count == 0);
    ret = lockf(fd, F_ULOCK, 0);
    assert(ret == 0);
    return 0;
}

int write_1(int fd)
{
    unsigned char buf[BUF_SIZE];
    int ret, offset = 0, len = sizeof(buf);
    memset(buf, 0xff, sizeof(buf));
    lockf(fd, F_LOCK, 0);
    assert(ret == 0);
    assert(count == 0);
    count++;
    //pthread_mutex_lock(&lock);
    while (1) {
        ret = write(fd, buf + offset, len - offset);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EINTR) {
                usleep(1000);
                continue;
            }
            printf("closed\n");
            exit(1);
        } else if (ret == 0) {
            printf("closed\n");
            exit(1);
        } else {
            offset += ret;
            if (offset == len)
                break;
        }
    }
    //pthread_mutex_unlock(&lock);
    count--;
    assert(count == 0);
    lockf(fd, F_ULOCK, 0);
    assert(ret == 0);
    return 0;
}

int read_01(int fd)
{
    unsigned char buf[BUF_SIZE];
    int i, ret, offset = 0, len = sizeof(buf);
    memset(buf, 0x55, sizeof(buf));
    while (1) {
        ret = read(fd, buf + offset, len - offset);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EINTR) {
                usleep(1000);
                continue;
            }
            printf("closed\n");
            exit(1);
        } else if (ret == 0) {
            printf("closed\n");
            exit(1);
        } else {
            offset += ret;
            if (offset == len)
                break;
        }
    }
    for (i = 0; i < sizeof(buf); i++) {
        if (buf[i] != buf[0] ||( buf[i] != 0 && buf[i] != 0xff)) {
            printf("%d: %d %d %d\n", i, buf[i - 1], buf[i], buf[i + 1]);
            printf("mismatch\n");
            exit(1);
        }
    }
    printf("read passed\n");
    return 0;
}

void *thread_write_0(void *arg)
{
    int fd = (int)arg;
    while (1) {
        write_0(fd);
        usleep(1000);
    }
}

void *thread_write_1(void *arg)
{
    int fd = (int)arg;
    while (1) {
        write_1(fd);
        usleep(1000);
    }
}
void *thread_read(void *arg)
{
    int fd = (int)arg;
    while (1) {
        read_01(fd);
        usleep(1000);
    }
}
int main()
{
    int fd[2];
    pthread_t thr1, thr2, thr3;
    pthread_mutex_init(&lock, NULL);
    
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) < 0) {
        printf("socketpair\n");
        exit(1);
    }

    if (pthread_create(&thr1, NULL, thread_write_0, (void *)fd[0]) < 0) {
        printf("pthread_create\n");
        exit(1);
    }

    if (pthread_create(&thr2, NULL, thread_write_1, (void *)fd[0]) < 0) {
        printf("pthread_create\n");
        exit(1);
    }

    if (pthread_create(&thr3, NULL, thread_read, (void *)fd[1]) < 0) {
        printf("pthread_create\n");
        exit(1);
    }

    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);
    pthread_join(thr3, NULL);

    return 0;
}
