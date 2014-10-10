#include "define.h"
#include "pusher.h"
#include "worker.h"

void pusher_timer(int fd, short event, void *arg)
{
    pusher_t *p = (pusher_t *)arg;
    client_t *c;
    cmd_t cmd;

    //printf("pusher timer\n");
    for (c = global.clients; c != NULL; c = c->h2.next) {
        cmd.cmd_no = CMD_HEATBEAT;
        cmd.data = c;
        cmd.length = sizeof c;
        evbuffer_add(bufferevent_get_output(c->worker->bev_pusher[1]), &cmd, sizeof cmd);
    }
    if (p->stop) {
        event_base_loopexit(p->base, NULL);
    }
}

pusher_t *pusher_create()
{
    pusher_t *p = (pusher_t *)malloc(sizeof(pusher_t));
    if (NULL == p) {
        err_quit("malloc");
    }
    memset(p, 0, sizeof(pusher_t));
    p->base = event_base_new();
    if (NULL == p->base) {
        err_quit("event_base_new");
    }
    return p;
}
void pusher_destroy(pusher_t **p)
{
    if (p && *p) {
        free(*p);
    }
    *p = NULL;
}

void pusher_worker_readcb(struct bufferevent *bev, void *arg)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    client_t *c;
    cmd_t cmd;

    while (evbuffer_get_length(input) >= sizeof cmd) {
        if (evbuffer_remove(input, &cmd, sizeof cmd) != sizeof cmd) {
            err_quit("evbuffer_remove");
        }
        switch (cmd.cmd_no) {
        case CMD_ADD_CLIENT:
            printf("client added\n");
            c = (client_t *)cmd.data;
            HASH_ADD(h2, global.clients, client_id, sizeof(c->client_id), c);
            break;
        case CMD_DEL_CLIENT:
            printf("client deleted\n");
            c = (client_t *)cmd.data;
            HASH_DELETE(h2, global.clients, c);
            client_destroy(&c);
            break;
        }
    }
}

void pusher_worker_writecb(struct bufferevent *bev, void *arg)
{
}
void pusher_worker_errorcb(struct bufferevent *bev, short error, void *arg)
{
}

void *pusher_loop(void *arg)
{
    pusher_t *p = (pusher_t *)arg;
    event_base_dispatch(p->base);
}

int pusher_start(pusher_t *p)
{
    struct event *timer_event;
    struct timeval timeout = {1, 0};
    worker_t *w;
    int ret, i;

    timer_event = event_new(p->base, -1, EV_PERSIST, pusher_timer, p);
    if (NULL == timer_event) {
        err_quit("event_new");
    }
    event_add(timer_event, &timeout);

    // create bev to workers
    for (w = global.workers; w != NULL; w = w->next) {
        w->bev_pusher[1] = bufferevent_socket_new(p->base, w->sockpair_pusher[1], BEV_OPT_CLOSE_ON_FREE);
        if (!w->bev_pusher[1]) {
            err_quit("bufferevent_socket_new");
        }
        bufferevent_setcb(w->bev_pusher[1], pusher_worker_readcb, pusher_worker_writecb, pusher_worker_errorcb, w);
        bufferevent_enable(w->bev_pusher[1], EV_READ | EV_WRITE);
    }

    ret = pthread_create(&(p->thread_id), NULL, pusher_loop, p);
    if (ret != 0) {
        err_quit("pthread_create");
    }
    return 0;
}

int pusher_stop(pusher_t *p)
{
    p->stop = 1;
    return 0;
}

