
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>
#include "adx_queue.h"
#include "adx_log.h"

#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "adx_type.h"
#include "adx_list.h"

/**********************/
/* LOG CORE */
/**********************/
adx_log_t *adx_log_open(const char *path, int level)
{
    FILE *fp = fopen(path, "a+");
    if (!fp) return NULL;

    adx_log_t *log = (adx_log_t *)je_malloc(sizeof(adx_log_t));
    if (!log) {
        fclose(fp);
        return NULL;
    }

    log->fp = fp;
    log->level = level;
    log->path = path;
    pthread_mutex_init(&log->mutex, NULL);
    return log;
}

void adx_log_close(adx_log_t *log)
{
    if (!log) return;
    fclose(log->fp);
    je_free(log);
}

void adx_log_write_buffer(adx_log_t *log, const char *buffer, int len)
{

    struct tm tm;
    time_t t = time(NULL);
    localtime_r(&t, &tm);

    pthread_mutex_lock(&log->mutex); // lock

    fprintf(log->fp, "[%04d-%02d-%02d %02d:%02d:%02d]",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec);
    fwrite(buffer, 1, len, log->fp);
    fputc('\n', log->fp);

    pthread_mutex_unlock(&log->mutex); // unlock
}

void adx_log_write_vlist(adx_log_t *log, const char *format, va_list ap)
{
    struct tm tm;
    time_t t = time(NULL);
    localtime_r(&t, &tm);

    pthread_mutex_lock(&log->mutex); // lock

    fprintf(log->fp, "[%04d-%02d-%02d %02d:%02d:%02d]",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec);
    vfprintf(log->fp, format, ap);
    fputc('\n', log->fp);

    pthread_mutex_unlock(&log->mutex); // unlock
}

void adx_log_write(adx_log_t *log, int level, const char *format, ...)
{
    if (!log || !log->fp) return;
    if (level < LOGINFO || level > LOGDEBUG || level < log->level) return;

    va_list ap;
    va_start(ap, format);
    adx_log_write_vlist(log, format, ap);
    va_end(ap);	
}

/**********************/
/* LOG QUEUE */
/**********************/
void *adx_log_queue_thread(void *p)
{
    adx_log_queue_t *log_queue = (adx_log_queue_t *)p;
    adx_log_t *log = log_queue->log;
    // pthread_detach(pthread_self());

    for(;;) {

        pthread_mutex_lock(&log_queue->mutex);
        adx_str_t buffer = adx_queue_pop(&log_queue->queue);
        pthread_mutex_unlock(&log_queue->mutex);

        if (adx_str_empty(buffer)) {
            sleep(1);
            continue;
        }

        adx_log_write_buffer(log, buffer.str, buffer.len);
        adx_str_free(buffer);
    }

    pthread_exit(NULL);
}

void adx_log_queue_vlist(adx_log_queue_t *log_queue, int level, const char *format, va_list ap)
{
    if (!log_queue) return;
    adx_log_t *log = log_queue->log;

    if (!log || !log->fp) return;
    if (level < LOGINFO || level > LOGDEBUG || level < log->level) return;

    int len = 1024;
    char *buf = NULL;
    va_list ap2;
loop:	
    buf = (char *)je_malloc(len);
    if (!buf) return;

    va_copy(ap2, ap);
    int size = vsnprintf(buf, len, format, ap2);
    if (size >= len) {
        len = size + 1;
        je_free(buf);
        goto loop;
    }

    pthread_mutex_lock(&log_queue->mutex);
    adx_queue_push(&log_queue->queue, buf, size);
    pthread_mutex_unlock(&log_queue->mutex);

    je_free(buf);
    va_end(ap);
}

void adx_log_queue_write(adx_log_queue_t *log_queue, int level, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    adx_log_queue_vlist(log_queue, level, format, ap);
    va_end(ap);
}

adx_log_queue_t *adx_log_queue_create(const char *path, int level)
{
    adx_log_t *log = adx_log_open(path, level);
    if (!log) return NULL;

    adx_log_queue_t *log_queue = (adx_log_queue_t *)je_malloc(sizeof(adx_log_queue_t));
    if (!log_queue) goto err;

    log_queue->log = log;
    adx_list_init(&log_queue->queue);
    pthread_mutex_init(&log_queue->mutex, NULL);

    pthread_t tid;
    if (pthread_create(&tid, NULL, adx_log_queue_thread, log_queue))
        goto err;

    return log_queue;
err:
    adx_log_close(log);
    if (log_queue) je_free(log_queue);
    return NULL;
}

void adx_log_queue_close(adx_log_queue_t *log_queue)
{
    if (!log_queue) return;
    adx_log_close(log_queue->log);
    je_free(log_queue);
}

/**********************/
/* 全局LOG (线程安全) */
/**********************/
static adx_log_queue_t *log_queue_global = NULL; // 队列全局指针
int adx_log_init(const char *path, int level)
{
    log_queue_global = adx_log_queue_create(path, level);
    if (!log_queue_global) {
        fprintf(stderr, "adx_log_init: err\n");
        return -1;
    }
    return 0;    
}

void adx_log(int level, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    adx_log_queue_vlist(log_queue_global, level, format, ap);
    va_end(ap);
}

#if 0
int main()
{

    adx_log_init("/make/web_site/log", LOGDEBUG);

    int i;
    for (i = 0; i < 1024; i++) {
        adx_log(LOGDEBUG, "%0100000d", i);
    }

    sleep(3);
    return 0;
}
#endif




