#ifndef  __DEFINE_H_
#define  __DEFINE_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "base64_enc.h"
#include "sha1.h"
#include "uthash.h"

#define err_quit(fmt, args...) do {\
    fprintf(stderr, fmt, ##args);\
    exit(1);\
} while (0)

#define WORKER_NUM 4

#endif  //__DEFINE_H_
