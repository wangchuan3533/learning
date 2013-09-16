#include "http_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct header {
    char *field;
    char *value;
} headers[1024];
int count = 0;

int request_url_cb (http_parser *p, const char *buf, size_t len)
{
    char *str = malloc(1024);
    memcpy(str, buf, len);
    str[len] = 0;
    printf("url %s\n", str);
    free(str);
    return 0;
}
int header_field_cb (http_parser *p, const char *buf, size_t len)
{
    printf("field\n");
    headers[count].field = malloc(len + 1);
    memcpy(headers[count].field, buf, len);
    headers[count].field[len] = 0;
    return 0;
}

int header_value_cb (http_parser *p, const char *buf, size_t len)
{
    printf("value\n");
    headers[count].value = malloc(len + 1);
    memcpy(headers[count].value, buf, len);
    headers[count++].field[len] = 0;
    return 0;
}

int main()
{
    int i, nparsed;
    const char *req = "CONNECT /demo HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                "Upgrade: websocket\r\n"
                "Origin: http://example.com\r\n"
                "Sec-WebSocket-Version: 13\r\n"
                "WebSocket-Protocol: sample\r\n"
                "Connection: Upgrade\r\n";

    http_parser_settings settings;
    memset(&settings, 0, sizeof(settings));
    settings.on_url = request_url_cb;
    settings.on_header_field = header_field_cb;
    settings.on_header_value = header_value_cb;

    /* ... */

    http_parser *parser = malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST);
    //parser->data = req;

    nparsed = http_parser_execute(parser, &settings, req, strlen(req));

    if (nparsed != strlen(req)) {
          /* Handle error. Usually just close the connection. */
    }

    for (i = 0; i < count; i++) {
        printf("filed = %s, value = %s\n", headers[i].field, headers[i].value);
    }
}
