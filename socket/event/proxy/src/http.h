#ifndef __HTTP__H
#define __HTTP__H

typedef struct http_request_header {
    char *url;
    char *Connection;
    char *Upgrade;
    char *Sec_WebSocket_Key;
    char *Sec_WebSocket_Protocol;
    char **current_field;
    int completed;
} http_request_header_t;

http_request_header_t *http_request_header_new();
void http_request_header_free(http_request_header_t *header);

#include "http_parser.h"
int parser_url_cb(http_parser *p, const char *buf, size_t len);
int parser_header_field_cb(http_parser *p, const char *buf, size_t len);
int parser_header_value_cb(http_parser *p, const char *buf, size_t len);
int parser_message_complete_cb(http_parser *p);

struct evbuffer;

int http_post(const char *url, const char *content, int len, struct evbuffer *output);
size_t curl_cb(void *content, size_t size, size_t nmemb, void *ptr);

#endif /* __HTTP__H */


