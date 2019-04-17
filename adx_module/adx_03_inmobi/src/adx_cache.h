
#ifndef __ADX_CACHE_H__
#define __ADX_CACHE_H__

#include "adx_type.h"
#include "adx_list.h"
#include "adx_rbtree.h"
#include "adx_json.h"

#define ADX_CACHE_TYPE_STRING   1
#define ADX_CACHE_TYPE_NUMBER   2
#define ADX_CACHE_TYPE_NULL     3
#define ADX_CACHE_TYPE_ERROR    4

#define ADX_CACHE_STR(o)  adx_cache_value_string(o)
#define ADX_CACHE_NUM(o)  adx_cache_value_number(o)
#define ADX_CACHE_NULL()  adx_cache_value_null()

#define GET_CACHE_VALUE_STR(o,k) adx_cache_to_string(adx_cache_find(o, ADX_CACHE_STR(k)))
#define GET_CACHE_VALUE_NUM(o,k) adx_cache_to_number(adx_cache_find(o, ADX_CACHE_STR(k)))

#ifdef __cplusplus
extern "C" { 
#endif

    typedef struct {
        int type; // 1:str 2:int 3:null
        union {
            char *string;
            long number;
        };

    } adx_cache_value_t;

    typedef struct adx_cache_t adx_cache_t;
    struct adx_cache_t {

        adx_cache_value_t       key;
        adx_cache_value_t       value;

        adx_cache_t             *parent;		// 父节点
        int                     child_total;		// 子节点数量

        adx_list_t 		child_list;		// 子节点根 -> next_list
        adx_rbtree_head         child_tree_string;	// 子节点根 -> next_tree_string
        adx_rbtree_head         child_tree_number;	// 子节点根 -> next_tree_number
        adx_rbtree_head         child_tree_rand;	// 字节点跟 -> 随机节点

        adx_list_t              next_list;		// 节点 list 		<- 父节点 child_list
        adx_rbtree_node         next_tree_string;	// 节点 tree string	<- 父节点 child_tree_string
        adx_rbtree_node         next_tree_number;	// 节点 tree number	<- 父节点 child_tree_number
        adx_rbtree_node         next_tree_rand;		// 随机节点  		<- 父节点 child_tree_rand
    };

    /*** cache create/free ***/
    adx_cache_t *adx_cache_create();
    void adx_cache_free(adx_cache_t *cache);

    /*** cache add ***/
    adx_cache_t *adx_cache_add(adx_cache_t *cache, adx_cache_value_t key, adx_cache_value_t value);
    adx_cache_t *adx_cache_add_index(adx_cache_t *root, adx_cache_value_t value);

    /*** cache find ***/
    adx_cache_t *adx_cache_find(adx_cache_t *cache, adx_cache_value_t key);
    adx_cache_t *adx_cache_find_args(adx_cache_t *cache, ...);
    adx_cache_t *adx_cache_find_loop(adx_cache_t *cache, int index);

    /*** cache rand ***/
    adx_cache_t *adx_cache_rand(adx_cache_t *cache);

    /*** display ***/
    void adx_cache_display(adx_cache_t *cache);

    /*** INPUT from type ***/
    adx_cache_value_t adx_cache_value_string(const char *key);
    adx_cache_value_t adx_cache_value_number(long key);
    adx_cache_value_t adx_cache_value_null();

    /*** OUPUT to type ***/
    char *adx_cache_to_string(adx_cache_t *cache); // cache to string
    long adx_cache_to_number(adx_cache_t *cache); // cache to number

    /*** OUPUT value to type ***/
    char *adx_cache_value_to_string(adx_cache_value_t value); // value to string
    long adx_cache_value_to_number(adx_cache_value_t value); // value to number

    /*** child check ***/
    int adx_cache_child_check(adx_cache_t *cache);

    /*** json to cache ***/
    void adx_cache_from_json(adx_cache_t *cache, adx_json_t *json);
    void adx_cache_from_json_array(adx_cache_t *cache, adx_json_t *json_array);

#ifdef __cplusplus
}
#endif

#endif



