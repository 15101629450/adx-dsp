
#ifndef __ADX_STRING_H__
#define __ADX_STRING_H__

#include "adx_type.h"
#include "adx_alloc.h"

#define IS_NULL(o) (o?o:"")
#define adx_atoi(o) atoi(IS_NULL(o))

#ifdef __cplusplus
extern "C" { 
#endif

    typedef struct {
        adx_pool_t *pool;
        int total, len;
        char *str;
    } adx_buffer;

    // 向上取整
    int adx_except_ceil(int x, int y);

    // 转换二进制
    char *adx_string_to_binary(int src, char *dest);

    // strdup
    char *adx_strdup(adx_pool_t *pool, const char *str, int len);

    // 转换 大写/小写
    char *adx_string_to_upper(char *buf); // 全部转大写
    char *adx_string_to_lower(char *buf); // 全部转小写

    // URL ENCODE/DECODE
    char *url_encode(char const *s, int len, int *new_length);
    int url_decode(char *str, int len);

    // BASE64 ENCODE/DECODE
    char *base64_encode(const char *src, char *dest);
    char *base64_decode(const char *src, char *dest);

    // 不分大小写strstr
    char *adx_strstr(const char *str, const char *needle);

    // strcat
    adx_buffer *adx_strcat(adx_buffer *buffer, char *str, int len);

    // 宏替换
    char *adx_macro_replace(adx_pool_t *pool, char *buf, const char *src, char *dest);

#ifdef __cplusplus
}
#endif

#endif



