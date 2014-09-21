#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "uthash.h"

struct element {
    UT_hash_handle hh;
    uint32_t id;
};

struct element *hash = NULL;

int add_element(uint32_t id)
{
    struct element *e = NULL;
    HASH_FIND(hh, hash, &id, sizeof(uint32_t), e);
    if (NULL == e) {
        e = (struct element *)malloc(sizeof(struct element));
        if (NULL == e) {
            return -1;
        }

        e->id = id;
        HASH_ADD(hh, hash, id, sizeof(uint32_t), e);
    } else {
        return -1;
    }

    return 0;
}

int delete_element(uint32_t id)
{
    struct element *e = NULL;
    HASH_FIND(hh, hash, &id, sizeof(uint32_t), e);
    if (NULL == e) {
        return -1;
    } else {
        HASH_DELETE(hh, hash, e);
        free(e);
    }
    return 0;
}

int main()
{
    int ret;

    ret = add_element(1);
    printf("ret of add_element is %d\n", ret);
    ret = delete_element(1);
    printf("ret of add_element is %d\n", ret);
}
