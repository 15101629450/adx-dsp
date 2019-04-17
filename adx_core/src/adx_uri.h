
#ifndef __ADX_URI_H__
#define __ADX_URI_H__

#include "adx_type.h"
#include "adx_list.h"
#include "adx_alloc.h"

#ifdef __cplusplus
extern "C" { 
#endif

    void adx_uri_parse(adx_pool_t *pool, adx_list_t *head, char *buf);
    void adx_uri_display(adx_list_t *head);

    char *adx_uri_find(adx_list_t *head, const char *key);

    char *adx_uri_get_string(adx_list_t *head, const char *key);
    int adx_uri_get_number(adx_list_t *head, const char *key);

#ifdef __cplusplus
}
#endif

#endif



