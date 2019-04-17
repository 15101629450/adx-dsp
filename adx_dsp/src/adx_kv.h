
#ifndef __ADX_URI_H__
#define __ADX_URI_H__

#include "adx_type.h"
#include "adx_list.h"
#include "adx_alloc.h"

#ifdef __cplusplus
extern "C" { 
#endif

    void adx_kv_add(adx_pool_t *pool, adx_list_t *head, char *key, char *val);
    char *adx_kv_find(adx_list_t *head, const char *key);

    char *adx_kv_get_string(adx_list_t *head, const char *key);
    int adx_kv_get_number(adx_list_t *head, const char *key);

    void adx_kv_display(adx_list_t *head);

    int adx_kv_from_uri(adx_pool_t *pool, adx_list_t *head, char *buf);
    int adx_kv_from_csv(adx_pool_t *pool, adx_list_t *head, const char *path);

#ifdef __cplusplus
}
#endif

#endif



