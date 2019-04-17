
#include <stdio.h>
#include <string.h>
#include <adx_string.h>

#include <math.h>
#include <ctype.h>
#include <cpuid.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

ngx_int_t adx_chain_len(ngx_chain_t *chain)
{
    ngx_int_t len = 0;
    ngx_chain_t *p = NULL;
    for (p = chain; p; p = p->next) {
        ngx_buf_t *buf = p->buf;
        len += buf->last - buf->pos;
    }

    return len;
}

ngx_chain_t *adx_chain_alloc(ngx_pool_t *pool, void *buffer, size_t size)
{
    if (!pool || !buffer || !size) return NULL;
    ngx_chain_t *chain = ngx_pcalloc(pool, sizeof(ngx_chain_t));
    if (!chain) return NULL;

    chain->buf = ngx_pcalloc(pool, sizeof(ngx_buf_t));
    if (!chain->buf) return NULL;

    chain->buf->pos = ngx_pcalloc(pool, size);
    if (!chain->buf->pos) return NULL;

    chain->next = NULL;
    chain->buf->last = chain->buf->pos + size;
    ngx_memcpy(chain->buf->pos, buffer, size);

    chain->buf->memory = 1;
    chain->buf->last_buf = 1;
    return chain;
}

ngx_chain_t *adx_chain_alloc_r(ngx_pool_t *pool, void *buffer)
{
    if (!buffer) return NULL;
    int len = strlen(buffer);
    if (!len) return NULL;
    return adx_chain_alloc(pool, buffer, len);
}

ngx_str_t adx_chain_to_str(ngx_pool_t *pool, ngx_chain_t *src)
{
    ngx_str_t dest = ngx_null_string;
    ngx_int_t len = adx_chain_len(src);
    if (len <= 0) return dest;

    char *s = ngx_pcalloc(pool, len + 1);
    if (!s) return dest;

    int pos = 0;
    ngx_chain_t *p = NULL;
    for (p = src; p; p = p->next) {
        ngx_buf_t *buf = p->buf;
        memcpy(&s[pos], buf->pos, buf->last - buf->pos);
        pos += buf->last - buf->pos;
    }

    s[len] = 0;
    dest.data = (void *)s;
    dest.len = len;
    return dest;
}

void adx_chain_print(ngx_chain_t *chain)
{
    ngx_chain_t *p = NULL;
    for (p = chain; p; p = p->next)
        fwrite(p->buf->pos, (p->buf->last - p->buf->pos), 1, stdout);
    fprintf(stdout, "\n");
}

ngx_str_t adx_number_to_str(ngx_pool_t *pool, ngx_int_t number)
{
    u_char *s = ngx_pcalloc(pool, 64);
    int len = sprintf((char *)s, "%zd", number);
    ngx_str_t str = {len, s};
    return str;
}

char *adx_str_to_pointer(ngx_pool_t *pool, ngx_str_t s)
{
    char *dest = ngx_pcalloc(pool, s.len + 1);
    if (!dest) return NULL;

    memcpy(dest, s.data, s.len);
    dest[s.len] = 0;
    return dest;
}

ngx_str_t *adx_str_from_pointer(ngx_pool_t *pool, char *str)
{
    if (!str) return NULL;
    ngx_str_t *s = ngx_pcalloc(pool, sizeof(ngx_str_t));
    if (!s) return NULL;

    s->data = (u_char *)str;
    s->len = strlen(str);
    return s;
}

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

char *adx_strtok(const char *src, char *dest, char **save_ptr, int ch)
{

    *dest = 0;
    if (save_ptr && *save_ptr) 
        src = *save_ptr;

    char *p = dest;
    char *s = (char *)src;
    for (; s && *s; s++) {

        if (*s == ch) {

            *p = 0;
            *save_ptr = s + 1;
            return dest;
        }

        *p++ = *s;
    }

    if (src || *save_ptr) {

        *p = 0;
        *save_ptr = NULL;
        return dest;
    }

    return NULL;
}

char *adx_string_domain_parse(char *url)
{
    if (!url) return NULL;

    struct in_addr addr;
    if (inet_pton(AF_INET, url, &addr))
        return url;

    //    if (lg_string_ch_count(url, '.') == 1)
    //        return url;

    char *p = NULL;
    for (p = url; p && *p; p++)
        if (*p == '.')return ++p;

    return NULL;
}

void license_cpuid(char *cpu_id)
{
    unsigned int cpu_info[4];
    __cpuid_count(0, 0, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
    sprintf(cpu_id, "%.8X%.8X%.8X%.8X", cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);

    __cpuid(0, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
    sprintf(cpu_id, "%.8X%.8X%.8X%.8X", cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
}

int license_lan_mac(char *lan_name, char *lan_mac)
{  
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, lan_name);

    int sockfd = socket( AF_INET, SOCK_STREAM, 0);
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr, sizeof(ifr)))
        return -1;

    unsigned char *str = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    sprintf(lan_mac, "%02X%02X%02X%02X%02X%02X", str[0], str[1], str[2], str[3], str[4], str[5]);
    close(sockfd);

    if (strcmp("000000000000", lan_mac) == 0)
        return 1;
    return 0;
}

int license_lan_loop(char *_lan_name, char *_lan_mac, char *_lan_ip)
{
    int i = 0;
    struct ifconf ifc;
    char buf[512];

    // 初始化ifconf
    ifc.ifc_len = 512;
    ifc.ifc_buf = buf;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    ioctl(sockfd, SIOCGIFCONF, &ifc); //获取所有接口信息

    // 逐个网卡的名称
    struct ifreq *ifr = (struct ifreq *)buf;
    for(i = (ifc.ifc_len / sizeof(struct ifreq)); i > 0; i--, ifr++) {

        char *lan_name = ifr->ifr_name;
        char *lan_ip = inet_ntoa(((struct sockaddr_in*)&(ifr->ifr_addr))->sin_addr);
        char lan_mac[128];
        int ret = license_lan_mac(lan_name, lan_mac);
        if (ret == 0) {
            strcpy(_lan_name, lan_name);
            strcpy(_lan_mac, lan_mac);
            strcpy(_lan_ip, lan_ip);
            close(sockfd);
            return 0;
        }
    }

    close(sockfd);
    return -1;
} 

void license_lan(char *lan_mac)
{
    char lan_name[128], lan_ip[128];
    license_lan_loop(lan_name, lan_mac, lan_ip);
}

void license_key_coding(char *buffer, int size)
{

    int i, key;
    for (i = 0; i < size; i++) {

        if (i % 2)
            buffer[i] = ~buffer[i];

        if (!(i % 3))
            key = ((i + 1) * 5);
        else if (!(i % 4))
            key = ((i + 2) / 3);
        else if (i % 5)
            key = ((i + 3) * 4);
        else
            key = ((i + 4) / 2);

        if (key % 2)
            buffer[i] = (buffer[i] + key);
        else
            buffer[i] = (buffer[i] - key);
    }
}

void license_key_decoding(char *buffer, int size)
{

    int i, key;
    for (i = 0; i < size; i++) {

        if (!(i % 3)) 
            key = ((i + 1) * 5);
        else if (!(i % 4))
            key = ((i + 2) / 3);
        else if (i % 5)
            key = ((i + 3) * 4);
        else
            key = ((i + 4) / 2);

        if (key % 2)
            buffer[i] = (buffer[i] - key);
        else
            buffer[i] = (buffer[i] + key);

        if (i % 2)
            buffer[i] = ~buffer[i];
    }
}

void license_key(char *key)
{
    license_lan(key);
    license_cpuid(&key[12]);
}

void license_file(char *path, char *key)
{
    memset(key, 0, 128);
    FILE *fp = fopen(path, "r");
    if (!fp) return;

    int size = fread(key, 1, 128, fp);
    if (size <= 0) return;

    key[size - 1] = 0;
    fclose(fp);
}


