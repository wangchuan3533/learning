#ifndef __SERVER__H
#define __SERVER__H

/* package type */
#define    TYPE_HEARTBEAT      0
#define    TYPE_ERROR          1
#define    TYPE_PULL           2
#define    TYPE_PUSH           3
#define    TYPE_POST           4
#define    TYPE_RESP           5
#define    TYPE_RPC_PUSH       6

typedef struct package_head_s {
    uint32_t length;
    uint32_t type;
    uint32_t index;
} package_head_t;

#endif/* __SERVER__H */

