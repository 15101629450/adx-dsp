
#ifndef __ADX_JSON_H__
#define __ADX_JSON_H__

#include "adx_list.h"
#include "adx_alloc.h"

#define ADX_JSON_ROOT(o) adx_json_create(o, NULL, NULL, 0);

#ifdef __cplusplus
extern "C" { 
#endif

    typedef enum {

        ADX_JSON_OBJECT = 100,
        ADX_JSON_ARRAY,
        ADX_JSON_EMPTY,
        ADX_JSON_STRING,
        ADX_JSON_NUMBER,
        ADX_JSON_BOOL,
        ADX_JSON_NULL,
        ADX_JSON_ERROE,

    } adx_json_type;

    typedef struct adx_json_t {

        int type;
        char *key, *val;
        struct adx_json_t *parent;
        adx_list_t child;
        adx_list_t next;

    } adx_json_t;

    adx_json_t *adx_json_parse(adx_pool_t *pool, char *str);
    adx_json_t *adx_json_find(adx_json_t *root, const char *key);
    char *adx_json_find_value(adx_json_t *root, const char *key);

    adx_json_t *adx_json_create(adx_pool_t *pool, adx_json_t *parent, const char *key, int len);
    adx_json_t *adx_json_add(adx_pool_t *pool, adx_json_t *root, int json_type, const char *key, const char *val);

    adx_json_t *adx_json_add_string(adx_pool_t *pool, adx_json_t *root, const char *key, const char *val);
    adx_json_t *adx_json_add_number(adx_pool_t *pool, adx_json_t *root, const char *key, long val);
    adx_json_t *adx_json_add_double(adx_pool_t *pool, adx_json_t *root, const char *key, double val);

    char *adx_json_to_buffer(adx_pool_t *pool, adx_json_t *node);
    void adx_json_display(adx_json_t *root);
    char *adx_json_status(int status);

#ifdef __cplusplus
}
#endif

#endif


