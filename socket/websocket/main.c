#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_parser.h"
#include "sha1.h"

struct conn {
    char *url;
    char *Connection;
    char *Upgrade;
    char *Sec_WebSocket_Key;
    char *Sec_WebSocket_Protocol;
    char **current_field;
};

void print_conn(struct conn *conn)
{
    if (conn->url)
        printf("url: %s\n", conn->url);
    if (conn->Connection)
        printf("Connection: %s\n", conn->Connection);
    if (conn->Upgrade)
        printf("Upgrade: %s\n", conn->Upgrade);
    if (conn->Sec_WebSocket_Key)
        printf("Sec_WebSocket_Key: %s\n", conn->Sec_WebSocket_Key);
    if (conn->Sec_WebSocket_Protocol)
        printf("Sec_WebSocket_Protocol: %s\n", conn->Sec_WebSocket_Protocol);
}
int request_url_cb(http_parser *p, const char *buf, size_t len)
{
    struct conn *conn = (struct conn *)p->data;
    conn->url = malloc(len + 1);
    memcpy(conn->url, buf, len);
    conn->url[len] = 0;
    return 0;
}

int header_field_cb(http_parser *p, const char *buf, size_t len)
{
    struct conn *conn = (struct conn *)p->data;
    if (!strncmp("Connection", buf, len)) {
        conn->current_field = &conn->Connection;
    } else if (!strncmp("Upgrade", buf, len)) {
        conn->current_field = &conn->Upgrade;
    } else if (!strncmp("Sec-WebSocket-Key", buf, len)) {
        conn->current_field = &conn->Sec_WebSocket_Key;
    } else if (!strncmp("Sec-WebSocket-Protocal", buf, len)) {
        conn->current_field = &conn->Sec_WebSocket_Protocol;
    } else {
        conn->current_field = 0;
    }
    return 0;
}

int header_value_cb(http_parser *p, const char *buf, size_t len)
{
    struct conn *conn = (struct conn *)p->data;
    if (!conn->current_field)
        return 0;
    *(conn->current_field) = malloc(len + 1);
    memcpy(*(conn->current_field), buf, len);
    (*(conn->current_field))[len] = 0;
    conn->current_field = 0;
    return 0;
}

int handshake(char *key)
{
    printf("HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n"
            "\r\n", key);

}
int main()
{
    int i, n;
    struct conn conn;
    const char *secret = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const char *req = "GET /demo HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Connection: Upgrade\r\n"
        "Upgrade: websocket\r\n"
        "Origin: http://example.com\r\n"
        "Sec-WebSocket-Protocol: sample\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";

    char msg[128];
    char dst[20];


    http_parser_settings settings;
    http_parser *parser;

    memset(&settings, 0, sizeof(settings));
    settings.on_url = request_url_cb;
    settings.on_header_field = header_field_cb;
    settings.on_header_value = header_value_cb;

    memset(&conn, 0, sizeof(conn));

    parser = malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST);
    parser->data = &conn;
    
    n = http_parser_execute(parser, &settings, req, strlen(req));
    if (parser->upgrade) {
        printf("upgrade\n");
    } else if (n != strlen(req)) {
        printf("execute error\n");
    }
    /*print_conn(&conn);*/

    sprintf(msg, "%s%s", conn.Sec_WebSocket_Key, secret);
    sha1(dst, msg, strlen(msg) << 3);
    base64enc(msg, dst, 20);
    handshake(msg);
}

