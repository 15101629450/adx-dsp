
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "adx_string.h"

int adx_except_ceil(int x, int y)
{
    double X = x, Y = y;
    if (x == 0 || y == 0) return 0;
    return ceil(X / Y); 
}

char *adx_string_to_binary(int src, char *dest)
{
    int i, size = 0, split = 0;
    for (i = 31; i >= 0; i--) {

        if ((1 << i) & src) dest[size++] = '1';
        else dest[size++] = '0';

        if (++split >= 4) {
            dest[size++] = ' ';
            split = 0;
        }
    }

    dest[size] = 0;
    return dest;
}

char *adx_strdup(adx_pool_t *pool, const char *str, int len)
{
    if (!str || !len) return NULL;
    char *buf = (char *)adx_alloc(pool, len + 1);
    if (!buf) return NULL;

    memcpy(buf, str, len);
    buf[len] = 0;
    return buf;
}

char *adx_string_to_upper(char *buf)
{
    char *str = NULL;
    if (!buf) return NULL;
    for (str = buf; *str; str++) {
        *str = toupper(*str);
    }

    return buf;
}

char *adx_string_to_lower(char *buf)
{
    char *str = NULL;
    if (!buf) return NULL;
    for (str = buf; *str; str++) {
        *str = tolower(*str);
    }

    return buf;
}

char *url_encode(char const *s, int len, int *new_length)
{
    const char *from, *end;
    from = s;
    end = s + len;
    unsigned char *start,*to;
    start = to = (unsigned char *) malloc(3 * len + 1);
    unsigned char c;
    unsigned char hexchars[] = "0123456789ABCDEF";
    while (from < end) 
    {
        c = *from++;
        if (c == ' ')
        {
            *to++ = '+';
        }
        else if ((c < '0' && c != '-' && c != '.')||(c < 'A' && c > '9')||(c > 'Z' && c < 'a' && c != '_')||(c > 'z'))
        {
            to[0] = '%';
            to[1] = hexchars[c >> 4];
            to[2] = hexchars[c & 15];
            to += 3;
        }
        else
        {
            *to++ = c;
        }		     
    }
    *to = 0;
    if (new_length)
    {
        *new_length = to - start;
    }
    return (char *) start;
}

int url_decode(char *str, int len)
{
    char *dest = str;
    char *data = str;
    int value;
    unsigned char  c;
    while (len--)
    {
        if (*data == '+')
        {
            *dest = ' ';
        }
        else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1))&& isxdigit((int) *(data + 2)))
        {
            c = ((unsigned char *)(data+1))[0];
            if (isupper(c))
                c = tolower(c);
            value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
            c = ((unsigned char *)(data+1))[1];
            if (isupper(c))
                c = tolower(c);
            value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
            *dest = (char)value ;
            data += 2;
            len -= 2;
        }
        else
        {
            *dest = *data;
        }
        ++data;
        ++dest;
    }
    *dest = '\0';
    return (dest - str);
}

/* Base64 编码 */ 
static const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="; 
char *base64_encode(const char *src, char *dest)
{ 
    int i = 0;
    int tmp = 0;
    int temp = 0;
    int prepare = 0; 
    char changed[4]; 
    int src_len = strlen(src); 

    while (tmp < src_len) {

        temp = 0; 
        prepare = 0; 
        memset(changed, '\0', 4); 
        while (temp < 3) {
            if (tmp >= src_len) break;
            prepare = ((prepare << 8) | (src[tmp] & 0xFF)); 
            tmp++;
            temp++; 
        }

        prepare = (prepare<<((3-temp)*8)); 
        for (i = 0; i < 4 ;i++ ) {

            if (temp < i) 
                changed[i] = 0x40; 
            else 
                changed[i] = (prepare>>((3-i)*6)) & 0x3F; 

            int ch = changed[i];
            *dest++ = base[ch];
        } 
    } 

    *dest = '\0'; 
    return dest;
} 

/* 转换算子 */ 
static char base64_find_pos(char ch)
{ 
    char *ptr = (char*)strrchr(base, ch);//the last position (the only) in base[] 
    return (ptr - base); 
} 

/* Base64 解码 */ 
char *base64_decode(const char *src, char *dest)
{ 
    int i = 0; 
    int tmp = 0; 
    int temp = 0;
    int prepare = 0; 
    int equal_count = 0; 
    int src_len = strlen(src);

    if (*(src + src_len - 1) == '=') 
        equal_count += 1; 

    if (*(src + src_len - 2) == '=') 
        equal_count += 1; 

    if (*(src + src_len - 3) == '=') 
        equal_count += 1; 

    while (tmp < (src_len - equal_count)) {

        temp = 0; 
        prepare = 0; 
        while (temp < 4) {
            if (tmp >= (src_len - equal_count)) break; 
            prepare = (prepare << 6) | (base64_find_pos(src[tmp]));
            temp++;
            tmp++;
        }

        prepare = prepare << ((4-temp) * 6); 
        for (i = 0; i < 3; i++) {

            if (i == temp) break;
            *dest++ = (char)((prepare>>((2-i)*8)) & 0xFF);
        } 
    } 

    *dest = '\0'; 
    return dest; 
}

// 不分大小写strstr
char *adx_strstr(const char *str, const char *needle)
{
    if (!str || !needle) return NULL;
    size_t len = strlen(needle);
    if(len == 0) return NULL;

    while(*str) {
        if(strncasecmp(str, needle, len) == 0)
            return (char *)str;
        ++str;
    }

    return NULL;
}

// strcat
adx_buffer *adx_strcat(adx_buffer *buffer, char *str, int len)
{
    if (!len) return NULL;
    if (!buffer->str) buffer->str = adx_alloc(buffer->pool, buffer->total);
    if (!buffer->str) return NULL;

    for (;;) {
        if (len + buffer->len < buffer->total - 1) break;
        buffer->total = buffer->total * 2;
        char *p_buffer = adx_alloc(buffer->pool, buffer->total);
        if (!p_buffer) return NULL;

        memcpy(p_buffer, buffer->str, buffer->len);
        buffer->str = p_buffer;
    }

    memcpy(&buffer->str[buffer->len], str, len);
    buffer->len += len;
    buffer->str[buffer->len] = 0;
    return buffer;
}

// 宏替换
char *adx_macro_replace(adx_pool_t *pool, char *buf, const char *src, char *dest)
{
    adx_buffer buffer = {pool, 1024, 0, NULL};
    if (!pool || !buf || !src || !dest)
        return buf;

    int src_len = strlen(src);
    int dest_len = strlen(dest);
    if (!src_len) return buf;

    char *s = NULL;
    for (s = buf; *s; s++) {
        if (strncmp(s, src, src_len) == 0) {
            s += src_len - 1;
            if (!adx_strcat(&buffer, dest, dest_len))
                return buf;
        } else {
            if (!adx_strcat(&buffer, s, 1))
                return buf;
        }
    }

    return buffer.str;
}


