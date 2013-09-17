#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
} global;
void *thread(void *arg)
{
    int ret;

    pthread_mutex_lock(&global.lock);
    pthread_cond_signal(&global.cond);
    pthread_mutex_unlock(&global.lock);

}
int main()
{
    pthread_t thr;
    int ret;
    struct timespec ts;

    pthread_mutex_init(&global.lock, NULL);
    pthread_cond_init(&global.cond, NULL);

    clock_getttime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 1000000000;
    ret = pthread_create(&thr, NULL, thread, NULL);

    sleep(1);
    pthread_mutex_lock(&global.lock);
    pthread_cond_timedwait(&global.cond, &global.lock, &ts);
    pthread_mutex_unlock(&global.lock);

    printf("pass\n");
    pthread_join(thr, NULL);
    return 0;

}

