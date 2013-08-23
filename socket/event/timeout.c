#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>

void timeout_cb(evutil_socket_t fd, short what, void *arg)
{
    printf("timeout occured\n");
}

int main()
{
    int fd;
	struct event_base *base;
    struct event *ev;
    struct timeval five_seconds = {5, 0};

    fd = 10;
    base = event_base_new();
    ev = event_new(base, fd, EV_TIMEOUT|EV_PERSIST, timeout_cb, NULL);
    event_add(ev, &five_seconds);

    // Start the event loop`
    event_base_dispatch(base);
	
	return 0;
}

