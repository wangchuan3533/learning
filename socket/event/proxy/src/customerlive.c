#include "server.h"
#include "customerlive.h"

client_t *client_new()
{
    client_t *client = calloc(1, sizeof(client_t));
    assert(client != NULL);

    if (pthread_rwlock_init(&client->lock, NULL)) {
        free(client);
        return NULL;
    }

    client->parser_settings.on_url = parser_url_cb;
    client->parser_settings.on_header_field = parser_header_field_cb;
    client->parser_settings.on_header_value = parser_header_value_cb;
    client->parser_settings.on_message_complete = parser_message_complete_cb;


    TRACE("new client 0x%lx\n", (unsigned long)client);
    return client;
}

void client_free(client_t *client)
{
    if (client->pull_cmd) {
        free(client->pull_cmd);
    }

    free_request(&client->request);
    TRACE("free client 0x%lx\n", (unsigned long)client);
    free(client);
}

void client_inc_ref(client_t * client)
{
    pthread_rwlock_wrlock(&client->lock);
    client->refcnt++;
    pthread_rwlock_unlock(&client->lock);
}

void client_dec_ref(client_t * client)
{
    pthread_rwlock_wrlock(&client->lock);
    if (client->refcnt == 0) {
        pthread_rwlock_unlock(&client->lock);
        client_free(client);
        return;
    } else {
        client->refcnt--;
    }
    pthread_rwlock_unlock(&client->lock);
}


/* print the time now */
int fprint_time(FILE *fp)
{
    time_t now;
    struct tm tm_now;
    char str_now[32];

    time(&now);
    localtime_r(&now, &tm_now);
    asctime_r(&tm_now, str_now);
    return fprintf(fp, "%s:", str_now);
}
