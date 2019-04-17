
#ifndef __ADX_QUEUE_H__
#define __ADX_QUEUE_H__

#include "adx_type.h"
#include "adx_list.h"
#include "adx_alloc.h"

#ifdef __cplusplus
extern "C" { 
#endif

    /****************/
    /* QUEUE STRING */
    /****************/
    typedef struct {
        char *str;
        int len;
    } adx_str_t;

    adx_str_t adx_str_init(const char *buf, int len); // not pool
    void adx_str_free(adx_str_t str); // not pool

    int adx_str_empty(adx_str_t str);

    /******************/
    /* QUEUE PUSH/POP */
    /******************/
    typedef struct {
        adx_list_t queue;
        adx_str_t str;
    } adx_queue_t;

    void adx_queue_push_r(adx_pool_t *pool, adx_list_t *queue, const char *buf, int len);
    adx_str_t adx_queue_pop_r(adx_list_t *queue);

    void adx_queue_push(adx_list_t *queue, const char *buf, int len);
    adx_str_t adx_queue_pop(adx_list_t *queue);

#ifdef __cplusplus
}
#endif

#endif



