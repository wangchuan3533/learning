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

    pthread_rwlock_t lock;

    /* http header */
    http_request_header_t *request;
    http_parser parser;
    http_parser_settings parser_settings;

    /* websocket */
    int handshake;
    int frame_started;

    /* hash table handle */
    UT_hash_handle hh;
} client_t;
client_t *client_new();
void client_free();


int client_on_connect(conn_t *conn);
int client_on_close(conn_t *conn);
int client_on_receive(conn_t *conn);

int rpc_on_connect(conn_t *conn);
int rpc_on_close(conn_t *conn);
int rpc_on_receive(conn_t *conn);


#endif /* __CUSTOMERLIVE__H */
