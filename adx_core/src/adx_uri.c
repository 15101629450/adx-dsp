
#include <stdio.h>
#include <string.h>
#include "adx_uri.h"

typedef struct {
    adx_list_t next;
    char *key;
    char *val;

} adx_uri_t;

char *adx_uri_value_parse(char *str)
{
    char *value = strchr(str, '=');
    if (!value) return NULL;

    *value++ = 0;
    if (!(*value)) return NULL;

    return value;
}

void adx_uri_add(adx_pool_t *pool, adx_list_t *head, char *key, char *val)
{
    if (!key || !val) return;
    adx_uri_t *node = adx_alloc(pool, sizeof(adx_uri_t));
    if (!node) return;

    node->key = adx_alloc(pool, strlen(key) + 1);
    node->val = adx_alloc(pool, strlen(val) + 1);
    if (!node->key || !node->val) return;

    strcpy(node->key, key);
    strcpy(node->val, val);
    adx_list_add(head, &node->next);
}

char *adx_uri_find(adx_list_t *head, const char *key)
{
    adx_list_t *p = NULL;
    adx_list_for_each(p, head) {

	adx_uri_t *node = (adx_uri_t *)p;
	if (strcmp(key, node->key) == 0)
	    return node->val;
    }

    return NULL;
}

void adx_uri_display(adx_list_t *head)
{
    adx_list_t *p = NULL;
    adx_list_for_each(p, head) {

	adx_uri_t *node = (adx_uri_t *)p;
	fprintf(stdout, "==>[%s][%s]\n", node->key, node->val);
    }
}

void adx_uri_parse(adx_pool_t *pool, adx_list_t *head, char *buf)
{
    adx_list_init(head);
    char *saveptr = NULL;
    char *str = strtok_r(buf, "&", &saveptr);
    while(str) {

	char *key = str;
	char *val = adx_uri_value_parse(str);
	adx_uri_add(pool, head, key, val);
	str = strtok_r(NULL, "&", &saveptr);
    }
}

char *adx_uri_get_string(adx_list_t *head, const char *key)
{
    return adx_uri_find(head, key);
}

int adx_uri_get_number(adx_list_t *head, const char *key)
{
    char *str = adx_uri_find(head, key);
    if (!str) return 0;
    return atoi(str);
}







