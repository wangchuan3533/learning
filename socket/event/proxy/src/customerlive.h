#ifndef __CUSTOMERLIVE__H
#define __CUSTOMERLIVE__H

/* package type */
#define    TYPE_HEARTBEAT      0
#define    TYPE_ERROR          1
#define    TYPE_PULL           2
#define    TYPE_PUSH           3
#define    TYPE_POST           4
#define    TYPE_RESP           5
#define    TYPE_MSG_PUSH       6

/* client */
typedef struct client {
    char id[32];
    char *pull_cmd;
    int pull_cmd_len;

    int refcnt;
    pthread_rwlock_t lock;

    /* websocket */
    http_parser parser;
    http_parser_settings parser_settings;
    http_request_t request;
    int handshake;
    int frame_started;

    evthr_t *thr;

    UT_hash_handle hh;

} client_t;
client_t *client_new();
void client_free();

#define ERROR_EXIT do {\
    fprintf(stderr, "FATAL ERROR at %s:%d\n", __FILE__, __LINE__);\
    exit(1);\
} while(0)


int fprint_time(FILE *fp);
#define LOG(fmt, args...) do {\
    fprintf_time(global.fp_Log);\
    fprintf(global.fp_log, fmt, ##args);\
} while (0)

#endif /* __CUSTOMERLIVE__H */
