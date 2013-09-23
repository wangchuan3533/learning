#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/buffer.h>
#include "sha1.h"
#include "base64_enc.h"

#include "http.h"
#include "websocket.h"

int parse_frame(struct evbuffer *input, websocket_frame_head_t *head)
{
    uint8_t tmp[16], *cur = tmp;
    uint32_t buf_len;
    buf_len = evbuffer_get_length(input);
    memset(head, 0, sizeof(websocket_frame_head_t));

    if (buf_len < 2) {
        return -1;
    } else if (buf_len < sizeof(tmp)) {
        evbuffer_copyout(input, tmp, buf_len);
    } else {
        evbuffer_copyout(input, tmp, sizeof(tmp));
    }

    head->fin = (*cur >> 7) & 0x01;
    head->rsv = (*cur >> 4) & 0x07;
    head->opcode = *cur & 0x0f;
    cur++;

    head->mask = (*cur >> 7) & 0x01;
    head->length = *cur & 0x7f;
    cur++;

    if (head->length < 0x7e) {
        if (buf_len < 2 + head->mask * 4 + head->length)
            return -1;
    } else if (head->length == 0x7e) {
        head->length = ntohs(*(uint16_t *)cur);
        cur += 2;
        if (buf_len < 4 + head->mask * 4 + head->length)
            return -1;
    } else if (head->length == 0x7f) {
        head->length = ntohl(*(uint32_t *)cur);
        if (head->length) {
            /* head->length > 2^32 - 1 not support now */
        }
        cur += 4;
        head->length = ntohl(*(uint32_t *)cur);
        cur += 4;
        if (buf_len < 10 + head->mask * 4 + head->length)
            return -1;
    }
    if (head->mask) {
        head->mask_key[0] = *cur++;
        head->mask_key[1] = *cur++;
        head->mask_key[2] = *cur++;
        head->mask_key[3] = *cur++;
    }
    head->offset = cur - tmp;
    return 0;
}

int send_close_frame(int fd)
{
    uint8_t tmp[2];
    tmp[0] = 0x88;
    tmp[1] = 0x00;
    while (write(fd, tmp, sizeof(tmp)) < 0) {
        if (errno == EAGAIN || errno == EINTR) {
            usleep(1000);
            continue;
        }
    }
    return 0;
}

int add_binary_frame_head(struct evbuffer *output)
{
    uint8_t tmp[16], *cur = tmp;
    uint32_t length = evbuffer_get_length(output);

    *cur++ = 0x80 | OPCODE_BINARY_FRAME;
    /* no mask */
    *cur = 0x00;
    if (length < 0x7e) {
        *cur++ |= (uint8_t)length;
    } else if (length <= 0xffff) {
        *cur++ |= (uint8_t)0x7e;
        *((uint16_t *)cur) = htons((uint16_t)length);
        cur += 2;
    } else {
        *cur++ |= (uint8_t)0x7f;
        *((uint32_t *)cur) = 0x00000000;
        cur += 4;
        *((uint32_t *)cur) = htonl(length);
        cur += 4;
    }
    return evbuffer_prepend(output, tmp, cur - tmp);
}

int add_text_frame_head(struct evbuffer *output)
{
    uint8_t tmp[16], *cur = tmp;
    uint32_t length = evbuffer_get_length(output);

    *cur++ = 0x80 | OPCODE_TEXT_FRAME;
    /* no mask */
    *cur = 0x00;
    if (length < 0x7e) {
        *cur++ |= (uint8_t)length;
    } else if (length <= 0xffff) {
        *cur++ |= (uint8_t)0x7e;
        *((uint16_t *)cur) = htons((uint16_t)length);
        cur += 2;
    } else {
        *cur++ |= (uint8_t)0x7f;
        *((uint32_t *)cur) = 0x00000000;
        cur += 4;
        *((uint32_t *)cur) = htonl(length);
        cur += 4;
    }
    return evbuffer_prepend(output, tmp, cur - tmp);
}

static const char *_websocket_secret = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
int handshake(http_request_header_t *request, int fd)
{

    char tmp1[128], tmp2[128], response[1024];
    int ret, length, offset = 0;
    sprintf(tmp1, "%s%s", request->Sec_WebSocket_Key, _websocket_secret);
    sha1(tmp2, tmp1, strlen(tmp1) << 3);
    base64enc(tmp1, tmp2, 20);
    length = sprintf(response, "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n"
            "\r\n", tmp1);
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
