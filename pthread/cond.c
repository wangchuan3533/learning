#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
static struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
} global;
void *thread(void *arg)
{
    int ret;

    pthread_mutex_lock(&global.lock);
    sleep(1);
    pthread_cond_signal(&global.cond);
    pthread_mutex_unlock(&global.lock);

}
int main()
{
    pthread_t thr;
    int ret;

    pthread_mutex_init(&global.lock, NULL);
    pthread_cond_init(&global.cond, NULL);

    ret = pthread_create(&thr, NULL, thread, NULL);

    pthread_mutex_lock(&global.lock);
    pthread_cond_wait(&global.cond, &global.lock);
    pthread_mutex_unlock(&global.lock);

    printf("pass\n");
    pthread_join(thr, NULL);
    return 0;

}

