#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include "http_parser.h"
#include "sha1.h"

#define BUF_SIZE 1024 * 128

/* Websocket RFC6455 */
#define OPCODE_CONTINUATION_FRAME  0x0
#define OPCODE_TEXT_FRAME          0x1
#define OPCODE_BINARY_FRAME        0x2
#define OPCODE_CONNECTION_CLOSE    0x8
#define OPCODE_PING                0x9
#define OPCODE_PONG                0xa


struct conn {
    char *url;
    char *Connection;
    char *Upgrade;
    char *Sec_WebSocket_Key;
    char *Sec_WebSocket_Protocol;
    char **current_field;
    int completed;
};

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

int parse_frame(uint8_t *frame, int len, uint8_t *dst)
{
    uint8_t *cur = frame;
    uint8_t fin, rsv, opcode, mask, mask_key[4];
    uint32_t i, length;
    uint8_t *data;
    int ret;

    
    fin = (*cur >> 7) & 0x01;
    rsv = (*cur >> 4) & 0x07;
    opcode = *cur & 0x0f;
    cur++;

    mask = (*cur >> 7) & 0x01;
    length = *cur & 0x7f;
    cur++;

    if (length == 0x7e) {
        length = ntohs(*(uint16_t *)cur);
        cur += 2;
    } else if (length == 0x7f) {
        length = ntohl(*(uint32_t *)cur);
        /* only support 32bit length now */
        if (length) {
            /* close */
        }
        cur += 4;
        length = ntohl(*(uint32_t *)cur);
        cur += 4;
    }

    if (mask) {
        mask_key[0] = *cur++;
        mask_key[1] = *cur++;
        mask_key[2] = *cur++;
        mask_key[3] = *cur++;
    }

    /* TODO */
    data = cur;

    if (!fin) {
        /* close */
        printf("not fin\n");
    }

    if (rsv) {
        /* close */
        printf("rsv\n");
    }

    switch (opcode) {
    case OPCODE_CONTINUATION_FRAME:
        /* close */
        break;
    case OPCODE_TEXT_FRAME:
        if (mask) {
            for (i = 0; i < length; i++)
                data[i] ^= mask_key[i % 4];
        }
        for (i = 0; i < length; i++)
            printf("%c", (char)data[i]);
        printf("\n");
        ret = new_text_frame(dst, data, length);
        /* close */
        break;
    case OPCODE_BINARY_FRAME:
        /* TODO */
        break;
    case OPCODE_CONNECTION_CLOSE:
        ret = new_close_frame(dst, data, length);
        /* TODO */
        break;
    case OPCODE_PING:
        /* TODO */
        break;
    case OPCODE_PONG:
        /* TODO */
        break;
    default:
        /* close */
        break;
    }
    return ret;
}

int new_text_frame(uint8_t *dst, uint8_t *src, uint32_t len)
{
    uint8_t *cur = dst;
    uint32_t i;
    *cur++ = 0x80 | OPCODE_TEXT_FRAME;
    /* no mask */
    *cur = 0x00;
    if (len <= 125)
        *cur++ |= (uint8_t)len;
    else if (len <= 0xffff) {
        *cur++ |= (uint8_t)0x7e;
        *((uint16_t *)cur) = htons((uint16_t)len);
        cur += 2;
    } else {
        *cur++ |= (uint8_t)0x7f;
        *((uint32_t *)cur) = 0x00000000;
        cur += 4;
        *((uint32_t *)cur) = htonl(len);
    }
    for (i = 0; i < len; i++)
        *cur++ = *src++;
    return cur - dst;
}

int new_close_frame(uint8_t *dst, uint8_t *src, uint32_t len)
{
    uint8_t *cur = dst;
    uint32_t i;
    *cur++ = 0x80 | OPCODE_CONNECTION_CLOSE;
    /* no mask */
    *cur = 0x00;
    if (len <= 125)
        *cur++ |= (uint8_t)len;
    else if (len <= 0xffff) {
        *cur++ |= (uint8_t)0x7e;
        *((uint16_t *)cur) = htons((uint16_t)len);
        cur += 2;
    } else {
        *cur++ |= (uint8_t)0x7f;
        *((uint32_t *)cur) = 0x00000000;
        cur += 4;
        *((uint32_t *)cur) = htonl(len);
    }
    for (i = 0; i < len; i++)
        *cur++ = *src++;
    return cur - dst;
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
int message_complete_cb(http_parser *p)
{
    struct conn *conn = (struct conn *)p->data;
    conn->completed = 1;
    return 0;
}

int handshake(struct conn *conn, char *dst)
{

    char tmp1[128], tmp2[128];
    sprintf(tmp1, "%s%s", conn->Sec_WebSocket_Key, secret);
    sha1(tmp2, tmp1, strlen(tmp1) << 3);
    base64enc(tmp1, tmp2, 20);
 
    sprintf(dst, "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n"
            "\r\n", tmp1);
    return 0;
}

int http_response(struct conn *conn, uint8_t *dst)
{
    sprintf(dst, "HTTP/1.1 200 OK\r\n"
            "Server: WebSocket\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "Content-Type: text/html\r\n"
            "\r\n%s", strlen("hello world"), "hello world");
    return 0;
}

int main()
{
    uint8_t buf_recv[BUF_SIZE], buf_send[BUF_SIZE];
    int sock_s, sock_c;
    struct sockaddr_in addr_s, addr_c;
    unsigned int len;
    int received;

    int i, n;
    struct conn conn;

    /* http_parser */
    http_parser_settings settings;
    http_parser *parser;


    /* Create the TCP socket */
    if ((sock_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket\n");
        exit(1);
    }

    /* Construct the server sockaddr_in structure */
    memset(&addr_s, 0, sizeof(addr_s));
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_s.sin_port = htons(9555);

    /* Bind the server socket */
    if (bind(sock_s, (struct sockaddr *) &addr_s, sizeof(addr_s)) < 0) {
        perror("Failed to bind the server socket\n");
        exit(1);
    }

    /* Listen on the server socket */
    if (listen(sock_s, 128) < 0) {
        perror("Failed to linten on the server socket\n");
        exit(1);
    }


    memset(&settings, 0, sizeof(settings));
    settings.on_url = request_url_cb;
    settings.on_header_field = header_field_cb;
    settings.on_header_value = header_value_cb;
    settings.on_message_complete = message_complete_cb;

    parser = malloc(sizeof(http_parser));
 
    /* Accept connections */
    while (1) {
        len = sizeof(addr_c);
        if ((sock_c = accept(sock_s, (struct sockaddr *) &addr_c, &len)) < 0) {
            perror("Failed to accept the connections\n");
            exit(1);
        }

        /* Receive messages */
        if ((received = recv(sock_c, buf_recv, BUF_SIZE, 0)) < 0) {
            perror("Failed to receive message from client\n");
            exit(1);
        }

        buf_recv[received] = 0;
        printf("receieved:%s\n", buf_recv);
        /* handshake */

        http_parser_init(parser, HTTP_REQUEST);
        parser->data = &conn;
        memset(&conn, 0, sizeof(conn));
        parser->data = &conn;
        n = http_parser_execute(parser, &settings, buf_recv, received);

        if (parser->upgrade) {
            handshake(&conn, buf_send);
        } else if (n == received) {
            http_response(&conn, buf_send);
        } else {
            printf("n=%d,  received=%d\n", n, received);
        }
        if (send(sock_c, buf_send, strlen(buf_send), 0) != strlen(buf_send)) {
            perror("Failed to send messages to client\n");
            exit(1);
        }
 
        if (parser->upgrade) {
            while (1) {
                received = recv(sock_c, buf_recv, BUF_SIZE, 0);
                n = parse_frame(buf_recv, received, buf_send);
#if 0
                for (i = 0; i < n; i++)
                    printf("%02x", (unsigned char)buf_send[i]);
                printf("\n");
#endif
                if (send(sock_c, buf_send, n, 0) != n) {
                    perror("send");
                    exit(1);
                }
            }
        } else {
            close(sock_c);
        }
   }
    close(sock_s);
    exit(0);
}
