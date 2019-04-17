
#include <stdio.h>
#include <string.h>
#include "adx_kv.h"

typedef struct {
    adx_list_t next;
    char *key, *val;
} adx_kv_t;

void adx_kv_add(adx_pool_t *pool, adx_list_t *head, char *key, char *val)
{
    if (!key || !val) return;
    adx_kv_t *node = adx_alloc(pool, sizeof(adx_kv_t));
    if (!node) return;

    node->key = adx_alloc(pool, strlen(key) + 1);
    node->val = adx_alloc(pool, strlen(val) + 1);
    if (!node->key || !node->val) return;

    strcpy(node->key, key);
    strcpy(node->val, val);
    adx_list_add(head, &node->next);
}

char *adx_kv_find(adx_list_t *head, const char *key)
{
    adx_list_t *p = NULL;
    adx_list_for_each(p, head) {
        adx_kv_t *node = (adx_kv_t *)p;
        if (strcmp(key, node->key) == 0)
            return node->val;
    }
    return NULL;
}

char *adx_kv_get_string(adx_list_t *head, const char *key)
{
    return adx_kv_find(head, key);
}

int adx_kv_get_number(adx_list_t *head, const char *key)
{
    char *str = adx_kv_find(head, key);
    if (!str) return 0;
    return atoi(str);
}

void adx_kv_display(adx_list_t *head)
{
    adx_list_t *p = NULL;
    adx_list_for_each(p, head) {
        adx_kv_t *node = (adx_kv_t *)p;
        fprintf(stdout, "==>[%s][%s]\n", node->key, node->val);
    }
}

/*************/
/* URI TO KV */
/*************/
static char *adx_uri_value_parse(char *str)
{
    char *value = strchr(str, '=');
    if (!value) return NULL;

    *value++ = 0;
    if (!(*value)) return NULL;

    return value;
}

int adx_kv_from_uri(adx_pool_t *pool, adx_list_t *head, char *buf)
{
    adx_list_init(head);
    char *saveptr = NULL;
    char *str = strtok_r(buf, "&", &saveptr);
    while(str) {
        char *key = str;
        char *val = adx_uri_value_parse(str);
        adx_kv_add(pool, head, key, val);
        str = strtok_r(NULL, "&", &saveptr);
    }

    return 0;
}

/*************/
/* CSV TO KV */
/*************/
void adx_kv_from_csv_line(adx_pool_t *pool, adx_list_t *head, char *buf)
{
    char *saveptr = NULL;
    char *str = strtok_r(buf, ",", &saveptr);
    while(str) {
        char *v1 = str;
        char *v2 = strtok_r(NULL, ",\r\n\t", &saveptr);
        char *v3 = strtok_r(NULL, ",\r\n\t", &saveptr);

        fprintf(stdout, "===> %s::%s::%s\n", v1, v2, v3);
        str = strtok_r(NULL, ",", &saveptr);
    }
}

int adx_kv_from_csv(adx_pool_t *pool, adx_list_t *head, const char *path)
{
    adx_list_init(head);
    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;

    char buf[4096];
    while(fgets(buf, 4096, fp)) {
        if (buf[0] != '#') adx_kv_from_csv_line(pool, head, buf);
    }

    return 0;
}

#if 0
int main()
{
    adx_list_t head;
    adx_pool_t *pool = adx_pool_create();
    adx_kv_from_csv(pool, &head, "./adx_native.dict");

    adx_list_t *p = NULL;
    adx_list_for_each(p, &head) {
        adx_kv_t *node = (adx_kv_t *)p;
        fprintf(stdout, "%s=%s\n", node->key, node->val);
    }

    return 0;
}
#endif



