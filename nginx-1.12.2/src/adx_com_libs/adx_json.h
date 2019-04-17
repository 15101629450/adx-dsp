
#ifndef __ADX_JSON_H__
#define __ADX_JSON_H__

#include <ngx_core.h>
#include <adx_list.h>

#define adx_pool_t ngx_pool_t
#define adx_alloc ngx_pcalloc
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
        int array_len;
        struct adx_json_t *parent;
        adx_list_t child;
        adx_list_t next;

    } adx_json_t;

    /*** parse ***/
    adx_json_t *adx_json_parse(adx_pool_t *pool, const char *str);

    /*** find ***/
    adx_json_t *adx_json_find(adx_json_t *root, const char *key);
    adx_json_t *adx_json_find_array(adx_json_t *root, int index);

    char *adx_json_find_value(adx_json_t *root, const char *key);
    char *adx_json_find_value_string(adx_json_t *root, const char *key);
    long adx_json_find_value_number(adx_json_t *root, const char *key);
    double adx_json_find_value_double(adx_json_t *root, const char *key);

    int adx_json_array_check(adx_json_t *json);

    /*** create ***/
    adx_json_t *adx_json_create(adx_pool_t *pool, adx_json_t *parent, const char *key, int len);

    adx_json_t *adx_json_add_object(adx_pool_t *pool, adx_json_t *root, const char *key); // object
    adx_json_t *adx_json_add_array(adx_pool_t *pool, adx_json_t *root, const char *key); // array

    adx_json_t *adx_json_add_string(adx_pool_t *pool, adx_json_t *root, const char *key, const char *val); // string
    adx_json_t *adx_json_add_number(adx_pool_t *pool, adx_json_t *root, const char *key, long val); // number
    adx_json_t *adx_json_add_double(adx_pool_t *pool, adx_json_t *root, const char *key, double val); // number

    adx_json_t *adx_json_add_true(adx_pool_t *pool, adx_json_t *root, const char *key); // true
    adx_json_t *adx_json_add_false(adx_pool_t *pool, adx_json_t *root, const char *key); // false
    adx_json_t *adx_json_add_null(adx_pool_t *pool, adx_json_t *root, const char *key); // null

    /*** json to buf ***/
    char *adx_json_to_buffer(adx_pool_t *pool, adx_json_t *json);

    /*** other ***/
    void adx_json_display(adx_json_t *root);
    char *adx_json_status(int status);

#ifdef __cplusplus
}
#endif

#endif


