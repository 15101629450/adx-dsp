
#ifndef __ADX_CACHE_H__
#define __ADX_CACHE_H__

#include "adx_type.h"
#include "adx_list.h"
#include "adx_rbtree.h"
#include "adx_alloc.h"
#include "adx_json.h"

#define ADX_CACHE_VALUE_STRING   1
#define ADX_CACHE_VALUE_NUMBER   2
#define ADX_CACHE_VALUE_ARRAY    3
#define ADX_CACHE_VALUE_NULL     4
#define ADX_CACHE_VALUE_ERROR    5

#define ADX_CACHE_STR(o)     adx_cache_value_string(o)
#define ADX_CACHE_NUM(o)     adx_cache_value_number(o)
#define ADX_CACHE_ARRAY(o)   adx_cache_value_array(o)
#define ADX_CACHE_NULL()     adx_cache_value_null()

#define GET_CACHE_FIND_STR(o,k) adx_cache_to_string(adx_cache_find(o, ADX_CACHE_STR(k)))
#define GET_CACHE_FIND_NUM(o,k) adx_cache_to_number(adx_cache_find(o, ADX_CACHE_STR(k)))

#define GET_CACHE_TO_KEY(o)  adx_cache_to_key(o)
#define GET_CACHE_TO_VAL(o)  adx_cache_to_val(o)

#ifdef __cplusplus
extern "C" { 
#endif

    typedef struct {
        int type; // 1:str 2:int 3:null
        union {
            char *string; // string
            int number; // number
            int index; // array
        };

    } adx_cache_value_t;

    typedef struct adx_cache_t adx_cache_t;
    struct adx_cache_t {

        adx_cache_value_t       key;
        adx_cache_value_t       val;

        adx_cache_t             *parent;           // 父节点
        int                     child_total;       // 子节点数量

        adx_list_t              child_list;        // 子节点根->next_list
        adx_rbtree_head         child_tree_string; // 子节点根->next_tree_string
        adx_rbtree_head         child_tree_number; // 子节点根->next_tree_number
        adx_rbtree_head         child_tree_array;  // 字节点跟->随机节点/数组节点

        adx_list_t              next_list;         // 节点(list)<-父节点(child_list)
        adx_rbtree_node         next_tree_string;  // 节点(string)<-父节点(child_tree_string)
        adx_rbtree_node         next_tree_number;  // 节点(number)<-父节点(child_tree_number)
        adx_rbtree_node         next_tree_array;   // 随机节点/数组节点<-父节点(child_tree_array)

        adx_pool_t              *pool; // 内存池
    };

    /*** cache create ***/
    adx_cache_t *adx_cache_create(adx_pool_t *pool);

    /*** cache add ***/
    adx_cache_t *adx_cache_add(adx_cache_t *cache, adx_cache_value_t key, adx_cache_value_t value);
    adx_cache_t *adx_cache_array_add(adx_cache_t *root, adx_cache_value_t value);

    /*** cache splice ***/
    int adx_cache_splice(adx_cache_t *dest, adx_cache_t *src);

    /*** cache find ***/
    adx_cache_t *adx_cache_find(adx_cache_t *cache, adx_cache_value_t key);
    adx_cache_t *adx_cache_find_args(adx_cache_t *cache, ...);

    /*** cache find array ***/
    int adx_cache_find_array_lenght(adx_cache_t *cache);
    adx_cache_t *adx_cache_find_array(adx_cache_t *cache, int index);
    adx_cache_t *adx_cache_find_array_loop(adx_cache_t *cache, int index);

    /*** cache rand ***/
    adx_cache_t *adx_cache_rand(adx_cache_t *cache);

    /*** display ***/
    void adx_cache_display(adx_cache_t *cache);

    /*** INPUT from type ***/
    adx_cache_value_t adx_cache_value_string(const char *key);
    adx_cache_value_t adx_cache_value_number(int key);
    adx_cache_value_t adx_cache_value_array(int key);
    adx_cache_value_t adx_cache_value_null();

    /*** OUPUT to type ***/
    char *adx_cache_to_string(adx_cache_t *cache); // cache to string
    int adx_cache_to_number(adx_cache_t *cache); // cache to number

    /*** OUPUT value to type ***/
    char *adx_cache_value_to_string(adx_cache_value_t value); // value to string
    int adx_cache_value_to_number(adx_cache_value_t value); // value to number

    /*** OUPUT key/val ***/
    adx_cache_value_t adx_cache_to_key(adx_cache_t *cache);
    adx_cache_value_t adx_cache_to_val(adx_cache_t *cache);

    /*** child check ***/
    int adx_cache_child_check(adx_cache_t *cache);

    /*** cache memory size ***/
    size_t adx_cache_memory_size(adx_cache_t *cache);

    /*** json to cache ***/
    int adx_cache_from_json(adx_cache_t *cache, adx_json_t *json);
    int adx_cache_from_json_string(adx_cache_t *cache, adx_json_t *json);
    int adx_cache_from_json_number(adx_cache_t *cache, adx_json_t *json);
    int adx_cache_from_json_array(adx_cache_t *cache, adx_json_t *json);

#ifdef __cplusplus
}
#endif

#endif



