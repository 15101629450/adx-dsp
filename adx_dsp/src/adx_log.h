
#ifndef __ADX_LOG_H__
#define __ADX_LOG_H__

#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "adx_type.h"
#include "adx_list.h"

#ifdef __cplusplus
extern "C" { 
#endif

#define LOGINFO			1
#define LOGWARNING		2
#define LOGERROR		3
#define LOGDEBUG		4

    /**********************/
    /* LOG CORE */
    /**********************/
    typedef struct {
        FILE *fp;
        int level;
        const char *path;
        pthread_mutex_t mutex; // 写入时序

    } adx_log_t;

    adx_log_t *adx_log_open(const char *path, int level);
    void adx_log_close(adx_log_t *log);

    void adx_log_write(adx_log_t *log, int level, const char *format, ...);

    /**********************/
    /* LOG QUEUE */
    /**********************/
    typedef struct {
        adx_log_t *log;
        adx_list_t queue;
        pthread_mutex_t mutex; // queue

    } adx_log_queue_t;

    adx_log_queue_t *adx_log_queue_create(const char *path, int level);
    void adx_log_queue_close(adx_log_queue_t *log_queue);

    void adx_log_queue_write(adx_log_queue_t *log_queue, int level, const char *format, ...);

    /**********************/
    /* 全局LOG (线程安全) */
    /**********************/
    int adx_log_init(const char *path, int level);
    void adx_log(int level, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif



