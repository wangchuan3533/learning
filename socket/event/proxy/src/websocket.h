#ifndef __WEB__SOCKET__H
#define __WEB__SOCKET__H

/* Websocket protocol RFC6455 */
#define OPCODE_CONTINUATION_FRAME  0x0
#define OPCODE_TEXT_FRAME          0x1
#define OPCODE_BINARY_FRAME        0x2
#define OPCODE_CONNECTION_CLOSE    0x8
#define OPCODE_PING                0x9
#define OPCODE_PONG                0xa

typedef struct websocket_frame_head {
    uint8_t fin;
    uint8_t rsv;
    uint8_t opcode;
    uint8_t mask;
    uint32_t length;
    uint8_t mask_key[4];

    int offset;
} websocket_frame_head_t;


#endif /* __WEB__SOCKET__H */

