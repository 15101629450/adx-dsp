
#ifndef __ADX_STRING_H__
#define __ADX_STRING_H__

#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>

// chain 
ngx_int_t adx_chain_len(ngx_chain_t *chain);
ngx_chain_t *adx_chain_alloc(ngx_pool_t *pool, void *buffer, size_t size);
ngx_chain_t *adx_chain_alloc_r(ngx_pool_t *pool, void *buffer);
ngx_str_t adx_chain_to_str(ngx_pool_t *pool, ngx_chain_t *src);
void adx_chain_print(ngx_chain_t *chain);

// number to str
ngx_str_t adx_number_to_str(ngx_pool_t *pool, ngx_int_t number);

// str to pointer
char *adx_str_to_pointer(ngx_pool_t *pool, ngx_str_t s);
ngx_str_t *adx_str_from_pointer(ngx_pool_t *pool, char *str);
#define ADX_STR(pool, str) adx_str_from_pointer(pool, str)

// 向上取整
int adx_except_ceil(int x, int y);

// 转换二进制
char *adx_string_to_binary(int src, char *dest);

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
char *adx_strtok(const char *src, char *dest, char **save_ptr, int ch);

#endif



