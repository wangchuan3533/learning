#ifndef  __WORKER_H_
#define  __WORKER_H_
#include "define.h"
#include "websocket.h"

// client states
typedef enum client_state_e {
    CLIENT_STATE_ACCEPTED = 0,
    CLIENT_STATE_HTTP_PARSE_STARTED,
    CLIENT_STATE_HANDSHAKE_STARTED,
    CLIENT_STATE_WEBSOCKET_FRAME_LOOP,
} client_state;


typedef struct client_s client_t;
typedef struct worker_s {

    // hash of clients
    client_t *client_list;
    // count of clients
    unsigned int client_count;
    // event base
    struct event_base *base;
    // hash handle
    UT_hash_handle hh;

    int pipe[2];// TODO

} worker_t;

struct client_s {
    uint64_t client_id;
    struct bufferevent *bev;
    // http headers
    http_headers_t *headers;
    // websocket frame
    websocket_frame_t *frame;
    // client state
    client_state state;
    // hash handle
    UT_hash_handle hh;
    // close_flag
    int close_flag;
    // context
    worker_t *worker;
};


#endif  //__WORKER_H_
