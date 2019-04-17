
#include "adx_cache.h"
#include <stdarg.h>

/*******************/
/* adx_cache_value */
/*******************/

// value string
adx_cache_value_t adx_cache_value_string(const char *key)
{
    adx_cache_value_t value;
    value.type = key ? ADX_CACHE_VALUE_STRING : ADX_CACHE_VALUE_ERROR;
    value.string = (char *)key;
    return value;
}

// value number
adx_cache_value_t adx_cache_value_number(int key)
{
    adx_cache_value_t value;
    value.type = ADX_CACHE_VALUE_NUMBER;
    value.number = key;
    return value;
}

// value array
adx_cache_value_t adx_cache_value_array(int key)
{
    adx_cache_value_t value;
    value.type = ADX_CACHE_VALUE_ARRAY;
    value.index = key;
    return value;
}

// value null
adx_cache_value_t adx_cache_value_null()
{
    adx_cache_value_t value;
    value.type = ADX_CACHE_VALUE_NULL;
    value.index = 0;
    return value;
}

// value error
adx_cache_value_t adx_cache_value_error()
{
    adx_cache_value_t value;
    value.type = ADX_CACHE_VALUE_ERROR;
    value.index = 0;
    return value;
}

// value copy
adx_cache_value_t adx_cache_value_copy(adx_pool_t *pool, adx_cache_value_t src)
{
    adx_cache_value_t dest = adx_cache_value_error();
    if (src.type == ADX_CACHE_VALUE_STRING) {

        int len = strlen(src.string);
        char *buf = adx_alloc(pool, len + 1);
        if (!buf) return dest;

        memcpy(buf, src.string, len);
        buf[len] = 0;

        return adx_cache_value_string(buf);
    }

    return src;
}

/**********************/
/* adx_cache add/find */
/**********************/

// create
adx_cache_t *adx_cache_create(adx_pool_t *pool)
{
    adx_cache_t *cache = adx_alloc(pool, sizeof(adx_cache_t));
    if (!cache) return NULL;

    // 初始化下级节点
    adx_list_init(&cache->child_list);
    cache->child_tree_string = RB_ROOT;
    cache->child_tree_number = RB_ROOT;
    cache->child_tree_array  = RB_ROOT;
    cache->pool = pool;
    return cache;
}

// add
adx_cache_t *adx_cache_add(adx_cache_t *cache, adx_cache_value_t key, adx_cache_value_t val)
{
    if (!cache) return NULL;

    // check key/val
    if (key.type == ADX_CACHE_VALUE_ERROR
            || val.type == ADX_CACHE_VALUE_ERROR)
        return NULL;

    // find
    adx_cache_t *node = NULL;
    switch(key.type) {
        case ADX_CACHE_VALUE_STRING :
        case ADX_CACHE_VALUE_NUMBER :
            node = adx_cache_find(cache, key);
            if (node) return node;
    }

    // alloc
    node = adx_cache_create(cache->pool);
    if (!node) return NULL;

    // KEY
    switch(key.type) {
        case ADX_CACHE_VALUE_STRING :
        case ADX_CACHE_VALUE_NUMBER :
        case ADX_CACHE_VALUE_ARRAY  :
            node->key = adx_cache_value_copy(cache->pool, key);
            break;

        default : return NULL;
    }

    // VALUE
    switch(val.type) {
        case ADX_CACHE_VALUE_STRING :
        case ADX_CACHE_VALUE_NUMBER :
        case ADX_CACHE_VALUE_NULL   :
            node->val = adx_cache_value_copy(cache->pool, val);
            break;

        default : return NULL;
    }

    // check node -> key/val
    if (node->key.type == ADX_CACHE_VALUE_ERROR
            || node->val.type == ADX_CACHE_VALUE_ERROR)
        return NULL;

    // INIT
    node->parent = cache;
    adx_list_add(&cache->child_list, &node->next_list);

    // TREE ADD ARRAY
    node->next_tree_array.number = cache->child_total++;
    adx_rbtree_number_add(&cache->child_tree_array, &node->next_tree_array);

    if (node->key.type == ADX_CACHE_VALUE_STRING) {

        // TREE ADD STRING
        node->next_tree_string.string = node->key.string;
        adx_rbtree_string_add(&cache->child_tree_string, &node->next_tree_string);

    } else if (node->key.type == ADX_CACHE_VALUE_NUMBER) {

        // TREE ADD NUMBER
        node->next_tree_number.number = node->key.number;
        adx_rbtree_number_add(&cache->child_tree_number, &node->next_tree_number);
    }

    return node;
}

// add array
adx_cache_t *adx_cache_array_add(adx_cache_t *cache, adx_cache_value_t val)
{
    adx_cache_value_t key = adx_cache_value_array(0);
    return adx_cache_add(cache, key, val);
}

// add cache splice
int adx_cache_splice_r(adx_cache_t *root, adx_cache_t *dest, adx_cache_t *src)
{
    if (!dest || !src) return -1;
    if (adx_cache_child_check(src) != 0) return 0;
    if (root == src) return 0; // 避免死循环(自己拼接自己)

    adx_list_t *p = NULL;
    adx_list_for_each(p, &src->child_list) {
        adx_cache_t *node = adx_list_entry(p, adx_cache_t, next_list);
        adx_cache_t *new = adx_cache_add(dest, node->key, node->val);
        if (!new) return -1;

        adx_cache_splice_r(root, new, node);
    }

    return 0;
}

int adx_cache_splice(adx_cache_t *dest, adx_cache_t *src)
{
    return adx_cache_splice_r(dest, dest, src);
}

// find
adx_cache_t *adx_cache_find(adx_cache_t *cache, adx_cache_value_t key)
{
    if (!cache) return NULL;
    if (key.type == ADX_CACHE_VALUE_STRING) {

        adx_rbtree_node *node = adx_rbtree_string_find(&cache->child_tree_string, key.string);
        if (node) return rb_entry(node, adx_cache_t, next_tree_string);

    } else if (key.type == ADX_CACHE_VALUE_NUMBER) {

        adx_rbtree_node *node = adx_rbtree_number_find(&cache->child_tree_number, key.number);
        if (node) return rb_entry(node, adx_cache_t, next_tree_number);

    } else if (key.type == ADX_CACHE_VALUE_ARRAY) {

        adx_rbtree_node *node = adx_rbtree_number_find(&cache->child_tree_array, key.index);
        if (node) return rb_entry(node, adx_cache_t, next_tree_array);
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
            case ADX_CACHE_VALUE_STRING :
            case ADX_CACHE_VALUE_NUMBER :
            case ADX_CACHE_VALUE_ARRAY  :
                cache = adx_cache_find(cache, key);
                break;

            case ADX_CACHE_VALUE_NULL :
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

// child check
int adx_cache_child_check(adx_cache_t *cache)
{
    if (!cache) return -1;
    adx_list_t *p = &cache->child_list;
    if (p->next == p) return -1;
    return 0;
}

size_t adx_cache_memory_size(adx_cache_t *cache)
{
    if (!cache || !cache->pool) return 0;
    return cache->pool->size;
}

// find array
adx_cache_t *adx_cache_find_array(adx_cache_t *cache, int index)
{
    if (!cache || cache->child_total <= 0) return NULL;
    return adx_cache_find(cache, ADX_CACHE_ARRAY(index));
}

// find array loop
adx_cache_t *adx_cache_find_array_loop(adx_cache_t *cache, int index)
{
    if (!cache || cache->child_total <= 0) return NULL;
    if (index > cache->child_total) index = index % cache->child_total;
    return adx_cache_find(cache, ADX_CACHE_ARRAY(index));
}

// find array lenght
int adx_cache_find_array_lenght(adx_cache_t *cache)
{
    if (adx_cache_child_check(cache) != 0) return 0;
    return cache->child_total;
}

// rand
adx_cache_t *adx_cache_rand(adx_cache_t *cache)
{
    if (!cache || cache->child_total <= 0) return NULL;
    int index = rand() % cache->child_total;
    return adx_cache_find(cache, ADX_CACHE_ARRAY(index));
}

/*********************/
/* adx_cache display */
/*********************/

char *adx_cache_value_type(int type)
{
    switch(type){
        case ADX_CACHE_VALUE_STRING : return "ADX_CACHE_VALUE_STRING";
        case ADX_CACHE_VALUE_NUMBER : return "ADX_CACHE_VALUE_NUMBER";
        case ADX_CACHE_VALUE_ARRAY  : return "ADX_CACHE_VALUE_ARRAY";
        case ADX_CACHE_VALUE_NULL   : return "ADX_CACHE_VALUE_NULL";
    }

    return "ADX_CACHE_VALUE_ERROR";
}

// value display
void adx_cache_value_display(adx_cache_value_t value)
{
    switch(value.type) {
        case ADX_CACHE_VALUE_STRING:
            fprintf(stdout, "[%s]", value.string);
            // fprintf(stdout, "[%s][%s]", value.string, adx_cache_value_type(value.type));
            break;

        case ADX_CACHE_VALUE_NUMBER:
            fprintf(stdout, "[%d]", value.number);
            // fprintf(stdout, "[%d][%s]", value.number, adx_cache_value_type(value.type));
            break;

        case ADX_CACHE_VALUE_NULL:
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
        adx_cache_value_display(node->val);
        fprintf(stdout, "\n");

        adx_cache_display(node);
    }
}

/*******************/
/* adx_cache OUPUT */
/*******************/

// cache to string
char *adx_cache_to_string(adx_cache_t *cache)
{
    if (!cache || cache->val.type != ADX_CACHE_VALUE_STRING)
        return NULL;
    return cache->val.string;
}

// cache to number
int adx_cache_to_number(adx_cache_t *cache)
{
    if (!cache || cache->val.type != ADX_CACHE_VALUE_NUMBER)
        return 0;
    return cache->val.number;
}

// value to string
char *adx_cache_value_to_string(adx_cache_value_t value)
{
    if (value.type == ADX_CACHE_VALUE_STRING)
        return value.string;
    return NULL;
}

// value to number
int adx_cache_value_to_number(adx_cache_value_t value)
{
    if (value.type == ADX_CACHE_VALUE_NUMBER)
        return value.number;
    return 0;
}

// cache to key
adx_cache_value_t adx_cache_to_key(adx_cache_t *cache)
{
    if (!cache) return adx_cache_value_string(NULL);
    return cache->key;
}

// cache to val
adx_cache_value_t adx_cache_to_val(adx_cache_t *cache)
{
    if (!cache) return adx_cache_value_string(NULL);
    return cache->val;
}

/******************/
/* adx_cache json */
/******************/

int adx_cache_from_json_value(adx_cache_t *cache, adx_json_t *json)
{
    if (json->type == ADX_JSON_OBJECT
            || json->type == ADX_JSON_ARRAY
            || json->type == ADX_JSON_EMPTY
            || json->type == ADX_JSON_ERROE)
        return -1;

    adx_cache_value_t value = ADX_CACHE_NULL();

    if (json->val) {
        if (json->type == ADX_JSON_NUMBER)
            value = adx_cache_value_number(atoi(json->val));
        else 
            value = adx_cache_value_string(json->val);
    }

    if (value.type == ADX_CACHE_VALUE_ERROR)
        return -1;

    if (json->key)
        adx_cache_add(cache, ADX_CACHE_STR(json->key), value);

    else if (json->val)
        adx_cache_array_add(cache, value);

    return 0;
}

int adx_cache_from_json_value_array(adx_cache_t *cache, adx_json_t *json)
{
    if (json->key) cache = adx_cache_add(cache, ADX_CACHE_STR(json->key), ADX_CACHE_NULL());

    int ret = 0;
    adx_list_t *p = NULL;
    adx_list_for_each(p, &json->child) {
        adx_json_t *json_node = adx_list_entry(p, adx_json_t, next);
        switch(json_node->type) {

            case ADX_JSON_OBJECT : 
            case ADX_JSON_ARRAY  : 
                ret = adx_cache_from_json(adx_cache_array_add(cache, ADX_CACHE_NULL()), json_node);
                if (ret) return ret;
                break;
            case ADX_JSON_STRING :
            case ADX_JSON_NUMBER : 
            case ADX_JSON_BOOL   :
            case ADX_JSON_NULL   : 
                ret = adx_cache_from_json_value(cache, json_node);
                if (ret) return ret;
            case ADX_JSON_EMPTY  : 
                break;
        }
    }

    return 0;
}

int adx_cache_from_json(adx_cache_t *cache, adx_json_t *json)
{
    if (!json) return -1;
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

    return 0;
}

int adx_cache_from_json_string(adx_cache_t *cache, adx_json_t *json)
{
    if (json && json->type == ADX_JSON_STRING) {
        adx_cache_add(cache, ADX_CACHE_STR(json->key), ADX_CACHE_STR(json->val));
        return 0;
    }

    return -1;
}

int adx_cache_from_json_number(adx_cache_t *cache, adx_json_t *json)
{
    if (json && json->type == ADX_JSON_NUMBER) {
        adx_cache_add(cache, ADX_CACHE_STR(json->key), ADX_CACHE_NUM(atoi(json->val)));
        return 0;
    }

    return -1;
}

int adx_cache_from_json_array(adx_cache_t *cache, adx_json_t *json)
{
    if (adx_json_array_check(json) != 0)
        return -1;

    cache = adx_cache_add(cache, ADX_CACHE_STR(json->key), ADX_CACHE_NULL());

    adx_list_t *p = NULL;
    adx_list_for_each(p, &json->child) {
        adx_json_t *json_node = adx_list_entry(p, adx_json_t, next);
        adx_cache_from_json_value(cache, json_node);
    }

    return 0;
}

#if 0
int main(int argc, char *argv[])
{
    adx_pool_t *pool = adx_pool_create();
    adx_cache_t *root = adx_cache_create(pool);

    adx_cache_t *node = NULL;
    node = adx_cache_add(root, ADX_CACHE_STR("aaa"), ADX_CACHE_NULL());
    adx_cache_add(node, ADX_CACHE_STR("a1"), ADX_CACHE_NULL());
    adx_cache_add(node, ADX_CACHE_STR("a2"), ADX_CACHE_STR("A2"));
    adx_cache_add(node, ADX_CACHE_STR("a3"), ADX_CACHE_STR("A3"));
    node = adx_cache_add(root, ADX_CACHE_STR("bbb"), ADX_CACHE_STR("BBB"));
    adx_cache_t *array = node;

    adx_cache_array_add(node, ADX_CACHE_NUM(11));
    adx_cache_array_add(node, ADX_CACHE_NUM(12));
    adx_cache_array_add(node, ADX_CACHE_NUM(13));
    node = adx_cache_array_add(node, ADX_CACHE_NUM(14));
    adx_cache_array_add(node, ADX_CACHE_NUM(15));
    adx_cache_array_add(node, ADX_CACHE_NUM(16));

    node = adx_cache_add(root, ADX_CACHE_STR("ccc"), ADX_CACHE_STR("CCC"));
    node = adx_cache_add(node, ADX_CACHE_STR("c1"), ADX_CACHE_STR("C1"));
    node = adx_cache_add(node, ADX_CACHE_STR("c2"), ADX_CACHE_STR("C2"));
    node = adx_cache_add(node, ADX_CACHE_STR("c3"), ADX_CACHE_STR("C3"));
    node = adx_cache_add(node, ADX_CACHE_NUM(4), ADX_CACHE_STR("C4"));
    adx_cache_t *splice = node;

    adx_cache_display(root);
    fprintf(stdout, "===> find <===\n");
    fprintf(stdout, "===>%s\n", 
            adx_cache_to_string(
                adx_cache_find_args(root,
                    ADX_CACHE_STR("ccc"), 
                    ADX_CACHE_STR("c1"), 
                    ADX_CACHE_STR("c2"), 
                    ADX_CACHE_STR("c3"), 
                    ADX_CACHE_NUM(4),
                    ADX_CACHE_NULL()
                    )));

    fprintf(stdout, "===> find array <===\n");
    fprintf(stdout, "array_lenght=%d\n", adx_cache_find_array_lenght(array));
    node = adx_cache_find_array(array, (0));
    fprintf(stdout, "==>%d\n", adx_cache_to_number(node));
    node = adx_cache_find_array(array, (1));
    fprintf(stdout, "==>%d\n", adx_cache_to_number(node));
    node = adx_cache_find_array(array, (2));
    fprintf(stdout, "==>%d\n", adx_cache_to_number(node));

    fprintf(stdout, "===> rand <===\n");

    node = adx_cache_rand(array);
    fprintf(stdout, "==>%d\n", adx_cache_to_number(node));
    node = adx_cache_rand(array);
    fprintf(stdout, "==>%d\n", adx_cache_to_number(node));
    node = adx_cache_rand(array);
    fprintf(stdout, "==>%d\n", adx_cache_to_number(node));
    node = adx_cache_rand(array);
    fprintf(stdout, "==>%d\n", adx_cache_to_number(node));

    int ret = adx_cache_splice(splice, root);
    adx_cache_display(root);
    fprintf(stdout, "ret=%d\n", ret);

    adx_pool_free(root->pool);
    return 0;
}
#endif



