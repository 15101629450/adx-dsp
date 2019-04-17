
#include "adx_cache.h"
#include <stdarg.h>

/*******************/
/* adx_cache_value */
/*******************/

// value string
adx_cache_value_t adx_cache_value_string(const char *key)
{
    adx_cache_value_t value;
    value.type = key ? ADX_CACHE_TYPE_STRING : ADX_CACHE_TYPE_ERROR;
    value.string = (char *)key;
    return value;
}

// value number
adx_cache_value_t adx_cache_value_number(long key)
{
    adx_cache_value_t value;
    value.type = ADX_CACHE_TYPE_NUMBER;
    value.number = key;
    return value;
}

// value null
adx_cache_value_t adx_cache_value_null()
{
    adx_cache_value_t value;
    value.type = ADX_CACHE_TYPE_NULL;
    value.number = 0;
    return value;
}

// value free
void adx_cache_value_free(adx_cache_value_t value)
{
    if (value.type == ADX_CACHE_TYPE_STRING && value.string) {
        je_free(value.string);
    }
}

// buf copy
char *adx_cache_buf_copy(char *src)
{
    if (!src) return NULL;

    int len = strlen(src);
    char *buf = (char *)je_malloc(len + 1);
    if (!buf) return NULL;

    memcpy(buf, src, len);
    buf[len] = 0;
    return buf;
}

/******************/
/*   adx_cache    */
/******************/

// cache create
adx_cache_t *adx_cache_create()
{
    adx_cache_t *cache = (adx_cache_t *)je_malloc(sizeof(adx_cache_t));
    if (!cache) return NULL;
    memset(cache, 0, sizeof(adx_cache_t));

    // 初始化下级节点
    adx_list_init(&cache->child_list);
    cache->child_tree_string = RB_ROOT;
    cache->child_tree_number = RB_ROOT;
    cache->child_tree_rand = RB_ROOT;
    return cache;
}

// cache free
void adx_cache_free(adx_cache_t *cache)
{
    if (!cache) return;

    adx_list_t *p = NULL;
    adx_list_t *head = &cache->child_list;
    for (p = head->next; p != head; ) {

        adx_cache_t *node = adx_list_entry(p, adx_cache_t, next_list);
        p = p->next;

        adx_cache_free(node);	// 递归子节点
    }

    adx_cache_value_free(cache->key);	// 释放 key
    adx_cache_value_free(cache->value);	// 释放 value
    je_free(cache);				// 释放 cache
}

static inline adx_cache_t *__adx_cache_free(adx_cache_t *cache)
{
    adx_cache_free(cache);
    return NULL;
}

// find
adx_cache_t *adx_cache_find(adx_cache_t *cache, adx_cache_value_t key)
{
    if (!cache) return NULL;
    if (key.type == ADX_CACHE_TYPE_STRING) {

        adx_rbtree_node *node = adx_rbtree_string_find(&cache->child_tree_string, key.string);
        if (node) return rb_entry(node, adx_cache_t, next_tree_string);

    } else if (key.type == ADX_CACHE_TYPE_NUMBER) {

        adx_rbtree_node *node = adx_rbtree_number_find(&cache->child_tree_number, key.number);
        if (node) return rb_entry(node, adx_cache_t, next_tree_number);
    }

    return NULL;
}

// find args
adx_cache_t *adx_cache_find_args(adx_cache_t *cache, ...)
{
    va_list args;
    va_start(args, cache);

    do {

        adx_cache_value_t key = va_arg(args, adx_cache_value_t);
        switch(key.type) {
            case ADX_CACHE_TYPE_STRING :
            case ADX_CACHE_TYPE_NUMBER :
                cache = adx_cache_find(cache, key);
                break;

            case ADX_CACHE_TYPE_NULL :
                va_end(args);
                return cache;

            default :
                va_end(args);
                return NULL;
        }

    } while(cache);

    va_end(args);
    return NULL;
}

// find index loop
adx_cache_t *adx_cache_find_loop(adx_cache_t *cache, int index)
{
    if (!cache || cache->child_total <= 0 || index <= 0) return NULL;
    if (index > cache->child_total) index = index % cache->child_total + 1;
    return adx_cache_find(cache, ADX_CACHE_NUM(index));
}

// rand
adx_cache_t *adx_cache_rand(adx_cache_t *cache)
{
    if (!cache || cache->child_total <= 0) return NULL;

    int index = rand() % cache->child_total + 1;
    adx_rbtree_node *node = adx_rbtree_number_find(&cache->child_tree_rand, index);
    if (node) return rb_entry(node, adx_cache_t, next_tree_rand);
    return NULL;
}

// add
adx_cache_t *adx_cache_add(adx_cache_t *cache, adx_cache_value_t key, adx_cache_value_t value)
{
    if (!cache) return NULL;
    adx_cache_t *node = adx_cache_find(cache, key);
    if (node) return node;

    node = adx_cache_create();
    if (!node) return NULL;

    switch(key.type) {

        case ADX_CACHE_TYPE_STRING: // add string
            node->key.type = ADX_CACHE_TYPE_STRING;
            node->key.string = adx_cache_buf_copy(key.string);
            if (!node->key.string) return __adx_cache_free(node);
            node->next_tree_string.string = node->key.string;
            break;

        case ADX_CACHE_TYPE_NUMBER: // add number
            node->key.type = ADX_CACHE_TYPE_NUMBER;
            node->key.number = key.number;
            node->next_tree_number.number = node->key.number;
            break;

        default: return __adx_cache_free(node);
    }

    switch(value.type) {

        case ADX_CACHE_TYPE_STRING: // add string
            node->value.type = ADX_CACHE_TYPE_STRING;
            node->value.string = adx_cache_buf_copy(value.string);
            if (!node->value.string) return __adx_cache_free(node);
            break;

        case ADX_CACHE_TYPE_NUMBER: // add number
            node->value.type = ADX_CACHE_TYPE_NUMBER;
            node->value.number = value.number;
            break;

        case ADX_CACHE_TYPE_NULL: // add null
            node->value.type = ADX_CACHE_TYPE_NULL;
            node->value.number = 0;
            break;

        default: return __adx_cache_free(node);
    }

    // LIST OR parent
    cache->child_total++;
    node->parent = cache;
    adx_list_add(&cache->child_list, &node->next_list);

    // RAND
    node->next_tree_rand.number = cache->child_total;
    adx_rbtree_number_add(&cache->child_tree_rand, &node->next_tree_rand);

    // STRING OR NUMBER
    if (node->key.type == ADX_CACHE_TYPE_STRING) {
        adx_rbtree_string_add(&cache->child_tree_string, &node->next_tree_string);

    } else if (node->key.type == ADX_CACHE_TYPE_NUMBER) {
        adx_rbtree_number_add(&cache->child_tree_number, &node->next_tree_number);
    }

    return node;
}

// add index
adx_cache_t *adx_cache_add_index(adx_cache_t *root, adx_cache_value_t value)
{
    adx_cache_value_t key = adx_cache_value_number(root->child_total + 1);
    return adx_cache_add(root, key, value);
}

#define ADX_CACHE_PRINTF_SIZE 15
void adx_cache_printf(char *str)
{
    char buf[ADX_CACHE_PRINTF_SIZE * 2] = {0};
    if (strlen(str) > ADX_CACHE_PRINTF_SIZE) {
        memcpy(buf, str, ADX_CACHE_PRINTF_SIZE);
        sprintf(&buf[ADX_CACHE_PRINTF_SIZE], " ...");
        fprintf(stdout, "[%s]", buf);

    } else {
        fprintf(stdout, "[%s]", str);
    }
}

// value display
void adx_cache_value_display(adx_cache_value_t value)
{
    switch(value.type) {
        case ADX_CACHE_TYPE_STRING:
            // adx_cache_printf(value.string);
            fprintf(stdout, "[%s]", value.string);
            break;

        case ADX_CACHE_TYPE_NUMBER:
            fprintf(stdout, "[%ld]", value.number);
            break;

        case ADX_CACHE_TYPE_NULL:
            // fprintf(stdout, "[null]");
            break;
    }
}

// display
void adx_cache_display(adx_cache_t *cache)
{
    if (!cache) return;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &cache->child_list) {

        adx_cache_t *node = adx_list_entry(p, adx_cache_t, next_list);
        adx_cache_t *parent = node->parent;
        while(parent && parent->parent) {
            fprintf(stdout, "│    ");
            parent = parent->parent;
        }

        fprintf(stdout, "├──");
        adx_cache_value_display(node->key);
        adx_cache_value_display(node->value);
        // fprintf(stdout, "[%d]", node->child_total);
        fprintf(stdout, "\n");

        adx_cache_display(node);
    }
}

// cache to string
char *adx_cache_to_string(adx_cache_t *cache)
{
    if (!cache || cache->value.type != ADX_CACHE_TYPE_STRING)
        return NULL;
    return cache->value.string;
}

// cache to number
long adx_cache_to_number(adx_cache_t *cache)
{
    if (!cache || cache->value.type != ADX_CACHE_TYPE_NUMBER)
        return 0;
    return cache->value.number;
}

// value to string
char *adx_cache_value_to_string(adx_cache_value_t value)
{
    if (value.type == ADX_CACHE_TYPE_STRING)
        return value.string;
    return NULL;
}

// value to number
long adx_cache_value_to_number(adx_cache_value_t value)
{
    if (value.type == ADX_CACHE_TYPE_NUMBER)
        return value.number;
    return 0;
}

int adx_cache_child_check(adx_cache_t *cache)
{
    if (!cache) return -1;
    adx_list_t *p = &cache->child_list;
    if (p->next == p) return -1;
    return 0;
}

/*** json to cache ***/
void adx_cache_from_json_value(adx_cache_t *cache, adx_json_t *json)
{
    if (json->type == ADX_JSON_OBJECT
            || json->type == ADX_JSON_ARRAY
            || json->type == ADX_JSON_EMPTY
            || json->type == ADX_JSON_ERROE)
        return;

    adx_cache_value_t value = ADX_CACHE_NULL();

    if (json->val) {
        if (json->type == ADX_JSON_NUMBER)
            value = adx_cache_value_number(atoi(json->val));
        else 
            value = adx_cache_value_string(json->val);
    }

    if (value.type == ADX_CACHE_TYPE_ERROR)
        return;

    if (json->key)
        adx_cache_add(cache, ADX_CACHE_STR(json->key), value);

    else if (json->val)
        adx_cache_add_index(cache, value);
}

void adx_cache_from_json_value_array(adx_cache_t *cache, adx_json_t *json)
{
    if (json->key) cache = adx_cache_add(cache, ADX_CACHE_STR(json->key), ADX_CACHE_NULL());

    adx_list_t *p = NULL;
    adx_list_for_each(p, &json->child) {
        adx_json_t *json_node = adx_list_entry(p, adx_json_t, next);
        switch(json_node->type) {

            case ADX_JSON_OBJECT : 
            case ADX_JSON_ARRAY  : adx_cache_from_json(adx_cache_add_index(cache, ADX_CACHE_NULL()), json_node);
                                   break;
            case ADX_JSON_STRING :
            case ADX_JSON_NUMBER : 
            case ADX_JSON_BOOL   :
            case ADX_JSON_NULL   : adx_cache_from_json_value(cache, json_node);
            case ADX_JSON_EMPTY  : break;
        }
    }
}

void adx_cache_from_json(adx_cache_t *cache, adx_json_t *json)
{
    if (!json) return;
    if (json->key) cache = adx_cache_add(cache, ADX_CACHE_STR(json->key), ADX_CACHE_NULL());

    adx_list_t *p = NULL;
    adx_list_for_each(p, &json->child) {
        adx_json_t *json_node = adx_list_entry(p, adx_json_t, next);
        switch(json_node->type) {

            case ADX_JSON_OBJECT : adx_cache_from_json(cache, json_node);
                                   break;
            case ADX_JSON_ARRAY  : adx_cache_from_json_value_array(cache, json_node);
                                   break;
            case ADX_JSON_STRING :
            case ADX_JSON_NUMBER : 
            case ADX_JSON_BOOL   :
            case ADX_JSON_NULL   : adx_cache_from_json_value(cache, json_node);
            case ADX_JSON_EMPTY  : break;
        }
    }
}

void adx_cache_from_json_array(adx_cache_t *cache, adx_json_t *json_array)
{
    if (!json_array || json_array->type != ADX_JSON_ARRAY || json_array->array_len == 0)
        return;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &json_array->child) {
        adx_json_t *json_node = adx_list_entry(p, adx_json_t, next);
        adx_cache_from_json_value(cache, json_node);
    }
}

#if 0
int main()
{
    char buf[40960] = {0};
    FILE *fp = fopen("mess", "r");
    fread(buf, 1, 40960, fp);

    adx_pool_t *pool = adx_pool_create();
    adx_cache_t *root = adx_cache_create();

    adx_json_t *json = adx_json_parse(pool, buf);
    adx_cache_from_json(root, json);

    fprintf(stdout, "%s\n", buf);
    fprintf(stdout, "===================================================================\n");
    adx_json_display(json);
    fprintf(stdout, "===================================================================\n");
    adx_cache_display(root);
    return 0;
}
#endif


