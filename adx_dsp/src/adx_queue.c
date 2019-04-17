
#include "adx_queue.h"
#include <string.h>

/****************/
/* QUEUE STRING */
/****************/
static void *adx_str_alloc(adx_pool_t *pool, int size)
{
    if (!pool) return je_malloc(size);
    return adx_alloc(pool, size);
}

adx_str_t adx_str_strdup(adx_pool_t *pool, const char *buf, int len)
{
    adx_str_t str;
    str.str = (char *)adx_str_alloc(pool, len + 1);
    if (!str.str) return str;

    str.len = len;
    memcpy(str.str, buf, len);
    str.str[len] = 0;
    return str;
}

adx_str_t adx_str_init(const char *buf, int len)
{
    return adx_str_strdup(NULL, buf, len);
}

void adx_str_free(adx_str_t str)
{
    if (!adx_str_empty(str)) je_free(str.str);
}

int adx_str_empty(adx_str_t str)
{
    return (!str.str || !str.len) ? -1 : 0;
}

/******************/
/* QUEUE PUSH/POP */
/******************/
void adx_queue_push_r(adx_pool_t *pool, adx_list_t *queue, const char *buf, int len)
{
    if (!buf || !len) return;
    adx_queue_t *node = (adx_queue_t *)adx_str_alloc(pool, sizeof(adx_queue_t));
    if (!node) return;

    node->str = adx_str_strdup(pool, buf, len);
    if (adx_str_empty(node->str)) {
        if (!pool) je_free(node);
        return;
    }

    adx_list_add(queue, &node->queue);
}

adx_str_t adx_queue_pop_r(adx_list_t *queue)
{
    adx_str_t str = {0};
    adx_queue_t *node = (adx_queue_t *)adx_queue(queue);
    if (node) str = node->str;
    return str;
}

void adx_queue_push(adx_list_t *queue, const char *buf, int len)
{
    return adx_queue_push_r(NULL, queue, buf, len);
}

adx_str_t adx_queue_pop(adx_list_t *queue)
{
    adx_str_t str = {0};
    adx_queue_t *node = (adx_queue_t *)adx_queue(queue);
    if (node) {
        str = node->str;
        je_free(node);
    }

    return str;
}

#if 0
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int n = 0;
adx_list_t queue = {&queue, &queue};
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void *t1(void *p)
{

    for (;;) {
        char buf[128];
        int size = sprintf(buf, "%010d", n++);

        pthread_mutex_lock(&queue_mutex);
        adx_queue_push(&queue, buf, size);
        pthread_mutex_unlock(&queue_mutex);
    }

    pthread_exit(NULL);
}

void *t2(void *p)
{
    for(;;) {
        pthread_mutex_lock(&queue_mutex);
        adx_str_t str = adx_queue_pop(&queue);
        pthread_mutex_unlock(&queue_mutex);

        if (adx_str_empty(str)) {
            usleep(1);
            continue;
        }

        fprintf(stdout, "[%lu]==>%s\n", pthread_self(), str.str);
        adx_str_free(str);
    }
}

int main()
{

    pthread_t tid;
    pthread_create(&tid, NULL, t1, NULL);
    pthread_create(&tid, NULL, t1, NULL);
    pthread_create(&tid, NULL, t1, NULL);

    pthread_create(&tid, NULL, t2, NULL);
    pthread_create(&tid, NULL, t2, NULL);

    for(;;)sleep(1);
    return 0;
}

#endif



