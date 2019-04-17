
#ifndef __ADX_ALLOC_H__
#define __ADX_ALLOC_H__

#include "adx_type.h"
#include "adx_list.h"

#ifdef __cplusplus
extern "C" { 
#endif

    typedef struct {
        adx_list_t head;
        size_t size;
    } adx_pool_t;

    adx_pool_t *adx_pool_create();
    void adx_pool_free(adx_pool_t *pool);

    void *adx_alloc(adx_pool_t *pool, size_t size);
    size_t adx_poll_size(adx_pool_t *pool);

#ifdef __cplusplus
}
#endif

#endif


