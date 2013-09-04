#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uthash.h>
typedef struct node_s {
    char id[32];
    char name[32];
    UT_hash_handle hh;
} node_t;
int main()
{
    node_t *id_hash = NULL, *name_hash = NULL, *tmp, *nodes[16];
    int i, ret;
    char id[32], name[32];

    /* insert into id_hash */
    for (i = 0; i < 16; i++) {
        nodes[i] = malloc(sizeof(node_t));
        sprintf(nodes[i]->id, "id%#x", i);
        sprintf(nodes[i]->name, "name%#X", i);
        HASH_ADD_STR(id_hash, id, nodes[i]);
    }

    /* find in id_hash */
    for (i = 0; i < 16; i++) {
        sprintf(id, "id%#x", i);
        HASH_FIND_STR(id_hash, id, tmp);
        if (tmp != NULL)
            printf("found %d node id = %s name = %s\n", i, tmp->id, tmp->name);
        else
            printf("%d node not found\n", i);
    }

    /* delete from id_hash & insert into name_hash */
    for (i = 0; i < 16; i++) {
        sprintf(id, "id%#x", i);
        HASH_FIND_STR(id_hash, id, tmp);
        HASH_DEL(id_hash, tmp);
        HASH_ADD_STR(name_hash, name, tmp);
    }

    /* find in name_hash */
    for (i = 0; i < 16; i++) {
        sprintf(name, "name%#X", i);
        HASH_FIND_STR(name_hash, name, tmp);
        if (tmp != NULL)
            printf("found %d node id = %s name = %s\n", i, tmp->id, tmp->name);
        else
            printf("%d node not found\n", i);
    }

    return 0;
}
