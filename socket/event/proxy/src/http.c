#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <curl/curl.h>
#include <event2/buffer.h>

#include "http.h"

http_request_header_t *http_request_header_new()
{
    http_request_header_t *header = calloc(sizeof(http_request_header_t), 1);
    assert(header != NULL);
    return header;
}

void http_request_header_free(http_request_header_t *header)
{
    if (header->url)
        free(header->url);
    if (header->Connection)
        free(header->Connection);
    if (header->Upgrade)
        free(header->Upgrade);
    if (header->Sec_WebSocket_Key)
        free(header->Sec_WebSocket_Key);
    if (header->Sec_WebSocket_Protocol)
        free(header->Sec_WebSocket_Protocol);
}

int send_http_response(int fd)
{
    const char *response = "HTTP/1.1 200 OK\r\n"
            "Server: WebSocket\r\n"
            "Content-Length: 8\r\n"
            "Connection: close\r\n"
            "Content-Type: text/html\r\n"
            "\r\nIt works";
    int ret, length, offset = 0;
    length = strlen(response);
    while (1) {
        ret = write(fd, response + offset, length - offset);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EINTR) {
                usleep(1000);
                continue;
            }
            return -1;
        } else if (ret == 0) {
            return -1;
        } else {
            offset += ret;
            if (offset == length)
                break;
        }
    }
    return 0;
}

int parser_url_cb(http_parser *p, const char *buf, size_t len)
{
    http_request_header_t *request = (http_request_header_t *)p->data;
    request->url = malloc(len + 1);
    memcpy(request->url, buf, len);
    request->url[len] = 0;
    return 0;
}

int parser_header_field_cb(http_parser *p, const char *buf, size_t len)
{
    http_request_header_t *request = (http_request_header_t *)p->data;
    if (!strncmp("Connection", buf, len)) {
        request->current_field = &request->Connection;
    } else if (!strncmp("Upgrade", buf, len)) {
        request->current_field = &request->Upgrade;
    } else if (!strncmp("Sec-WebSocket-Key", buf, len)) {
        request->current_field = &request->Sec_WebSocket_Key;
    } else if (!strncmp("Sec-WebSocket-Protocal", buf, len)) {
        request->current_field = &request->Sec_WebSocket_Protocol;
    } else {
        request->current_field = 0;
    }
    return 0;
}

int parser_header_value_cb(http_parser *p, const char *buf, size_t len)
{
    http_request_header_t *request = (http_request_header_t *)p->data;
    if (!request->current_field)
        return 0;
    *(request->current_field) = malloc(len + 1);
    memcpy(*(request->current_field), buf, len);
    (*(request->current_field))[len] = 0;
    request->current_field = 0;
    return 0;
}

int parser_message_complete_cb(http_parser *p)
{
    http_request_header_t *request = (http_request_header_t *)p->data;
    request->completed = 1;
    return 0;
}

size_t curl_cb(void *content, size_t size, size_t nmemb, void *ptr)
{
    struct evbuffer *output = (struct evbuffer *)ptr;
    int ret;
    ret = evbuffer_add(output, content, size * nmemb);
    if (ret < 0) {
        return 0;
    } else {
        return size * nmemb;
    }
}

int http_post(const char *url, const char *content, int len, struct evbuffer *output)
{
    CURL *curl;
    int ret;

    curl = curl_easy_init();
    if (!curl) {
        return -1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

    ret = curl_easy_perform(curl);

    if (ret != CURLE_OK) {
       return -1;
    }
    curl_easy_cleanup(curl);
    return 0;
}
