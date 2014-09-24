/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file pipe.c
 * @author wangchuan02(com@baidu.com)
 * @date 2014/09/24 09:05:45
 * @brief 
 *  
 **/
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define err_quit(fmt, args...) do {\
    fprintf(stderr, fmt, ##args);\
    exit(1);\
} while (0)

int set_nonblock(int fd)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_quit("fcntl");
    val |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, val) < 0)
        err_quit("fcntl");
    return 0;
}


void readcb(struct bufferevent *bev, void *arg)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output((struct bufferevent *)arg);
    int len;
    char buffer[1024];
    len = evbuffer_remove(input, buffer, sizeof(buffer));
    evbuffer_add(output, buffer, len);

}

void writecb(struct bufferevent *bev, void *arg)
{
    printf("write\n");
}

int main(int argc, char **argv)
{
    int fd[2];
    int ret;
    struct event_base *base = NULL;
    struct bufferevent *bev[2];

    fd[0] = STDIN_FILENO;
    fd[1] = STDOUT_FILENO;

    ret = set_nonblock(fd[0]);
    ret = set_nonblock(fd[1]);

    base = event_base_new();
    if (base == NULL) {
        err_quit("event_base_new");
    }

    bev[0] = bufferevent_socket_new(base, fd[0], BEV_OPT_CLOSE_ON_FREE);
    if (!bev[0]) {
        err_quit("bufferevent_socket_new");
    }

    bev[1] = bufferevent_socket_new(base, fd[1], BEV_OPT_CLOSE_ON_FREE);
    if (!bev[1]) {
        err_quit("bufferevent_socket_new");
    }

    bufferevent_setcb(bev[0], readcb, NULL, NULL, bev[1]);
    bufferevent_enable(bev[0], EV_READ);

    bufferevent_setcb(bev[1], writecb, NULL, NULL, bev[0]);
    bufferevent_enable(bev[1], EV_WRITE);
    event_base_dispatch(base);
    return 0;
}
