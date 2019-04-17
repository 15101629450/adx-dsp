
#include "adx_dump.h"
#define ADX_DUMP_ALL "all"

typedef void (adx_element_call_t)(adx_cache_t *adx,
        adx_cache_t *policy,
        adx_cache_t *creative,
        adx_cache_t *parent);

void adx_element_add(
        adx_cache_t *adx,
        adx_cache_t *policy,
        adx_cache_t *creative,
        adx_cache_t *parent,
        const char *fields_name,
        adx_cache_t *fields_cache,
        adx_element_call_t *call)
{
    adx_cache_t *fields = adx_cache_find(fields_cache, ADX_CACHE_STR(fields_name));
    if (adx_cache_child_check(fields) != 0)
        return call(adx, policy, creative,
                adx_cache_add(adx_cache_add(parent, ADX_CACHE_STR(fields_name), ADX_CACHE_NULL()),
                    ADX_CACHE_STR(ADX_DUMP_ALL), ADX_CACHE_NULL()));

    adx_list_t *p = NULL;
    adx_list_for_each(p, &fields->child_list) {
        adx_cache_t *node = adx_list_entry(p, adx_cache_t, next_list);
        if (node) call(adx, policy, creative,
                adx_cache_add(adx_cache_add(parent, ADX_CACHE_STR(fields_name), ADX_CACHE_NULL()),
                    node->key, ADX_CACHE_NULL()));
    }
}

typedef struct {
    int adx;
    int app;
    int region;
    int price;
} adx_element_bid_t;
adx_element_bid_t adx_element_bid_parse(adx_cache_t *bid_node)
{
    char buf[128];
    adx_element_bid_t bid = {-1, -1, -1, -1};
    if (!bid_node || !bid_node->key.string) return bid;
    if (bid_node->key.type != ADX_CACHE_VALUE_STRING) return bid;
    if (strlen(bid_node->key.string) > 100) return bid;
    strcpy(buf, bid_node->key.string);

    char *saveptr = NULL;
    char *adx = strtok_r(buf, "_", &saveptr);
    char *app = strtok_r(NULL, "_", &saveptr);
    char *region = strtok_r(NULL, "_", &saveptr);
    int price = adx_cache_value_to_number(bid_node->val);
    if (!adx || !app || !region || !price) return bid;

    bid.adx = atoi(adx);
    bid.app = atoi(app);
    bid.region = atoi(region);
    bid.price = price;
    return bid;
}

void (adx_element_bid)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    if (!parent) return;
    adx_cache_t *bids = adx_cache_find(creative, ADX_CACHE_STR("bids"));
    if (adx_cache_child_check(bids) != 0) return;

    int price = 0;
    adx_list_t *p = NULL;
    adx_list_for_each(p, &bids->child_list) {
        adx_cache_t *node = adx_list_entry(p, adx_cache_t, next_list);
        adx_element_bid_t bid = adx_element_bid_parse(node);
        if (bid.adx == 0 && bid.app == 0 && bid.region == 0)
            price = bid.price;
        // fprintf(stdout, "[%s][%zd]\n",
        // adx_cache_value_to_string(node->key),
        // adx_cache_value_to_number(node->value));
    }

    adx_list_for_each(p, &bids->child_list) {
        adx_cache_t *node = adx_list_entry(p, adx_cache_t, next_list);
        adx_element_bid_t bid = adx_element_bid_parse(node);
        if (bid.adx == adx_cache_value_to_number(adx->key))
            if (bid.app >= 0 && bid.region >= 0)
                price = bid.price;
    }

    adx_cache_add(parent, ADX_CACHE_STR("price"), ADX_CACHE_NUM(price));
    // fprintf(stdout, "adx=%zd price=%d\n", 
    // adx_cache_value_to_number(adx->key),
    // price);
}

int (adx_element_bid_sort)(void *p, adx_list_t *a, adx_list_t *b)
{
    adx_cache_t *a_cache = adx_list_entry(a, adx_cache_t, next_list);
    adx_cache_t *b_cache = adx_list_entry(b, adx_cache_t, next_list);
    int a_price = GET_CACHE_FIND_NUM(a_cache, "price");
    int b_price = GET_CACHE_FIND_NUM(b_cache, "price");
    if (a_price < b_price) return 1;
    return -1;
}

void (adx_element_mapid)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    adx_cache_t *map_id = adx_cache_find(creative, ADX_CACHE_STR("map_id"));
    if (map_id) {
        adx_cache_t *maps = adx_cache_add(parent, map_id->key, ADX_CACHE_NULL());
        adx_cache_t *mapid_node = adx_cache_add(maps, map_id->val, ADX_CACHE_NULL());
        adx_element_bid(adx, policy, creative, mapid_node);
        if (maps) adx_list_sort(NULL, &maps->child_list, 40960, adx_element_bid_sort);
    }
}

// 手机品牌
void (adx_element_make)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
}

// 操作系统
void (adx_element_os)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    adx_element_add(adx, policy, creative, parent,
            "os", policy, adx_element_mapid);
}

// 手机/平板/PC
void (adx_element_devicetype)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    adx_element_add(adx, policy, creative, parent,
            "devicetype", policy, adx_element_os);
}

// 运营商
void (adx_element_carrier)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    adx_element_add(adx, policy, creative, parent,
            "carrier", policy, adx_element_devicetype);
}

// 网络类型
void (adx_element_connectiontype)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    adx_element_add(adx, policy, creative, parent,
            "connectiontype", policy, adx_element_carrier);
}

// 地理位置
void (adx_element_region)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    adx_element_add(adx, policy, creative, parent,
            "regioncode", policy, adx_element_mapid);
    // "regioncode", policy, adx_element_connectiontype);
}

void (adx_element_banner)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    adx_element_add(adx, policy, creative, parent,
            "imgs", creative, adx_element_region);
}

void (adx_element_native)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    if (adx_cache_child_check(adx_cache_find(creative, ADX_CACHE_STR("icon"))) == 0)
        adx_element_add(adx, policy, creative, parent,
                "icon", creative, adx_element_banner);
}

void (adx_element_video)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
}

void (adx_element_creative_type)(adx_cache_t *adx, adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    switch(GET_CACHE_FIND_NUM(creative, "type")) {
        case 2 : return adx_element_banner(adx, policy, creative, parent);
        case 9 : return adx_element_native(adx, policy, creative, parent);
        case 6 : return adx_element_video(adx, policy, creative, parent);
    }
}

void (adx_element_adx)(adx_cache_t *policy, adx_cache_t *creative, adx_cache_t *parent)
{
    adx_cache_t *adx_list = adx_cache_find(creative, ADX_CACHE_STR("adx"));
    if (adx_cache_child_check(adx_list) != 0) return;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &adx_list->child_list) {
        adx_cache_t *node = adx_list_entry(p, adx_cache_t, next_list);
        adx_cache_t *adx = adx_cache_add(
                adx_cache_add(parent, ADX_CACHE_STR("adx"), ADX_CACHE_NULL()),
                node->key, ADX_CACHE_NULL());
        adx_element_creative_type(adx, policy, creative, adx);
    }
}

adx_cache_t *adx_element_templates(adx_cache_t *templates, int adx)
{
    int lenght = adx_cache_find_array_lenght(templates);
    for (int i = 0; i < lenght; i++) {
        adx_cache_t *node = adx_cache_find_array(templates, i);
        if (GET_CACHE_FIND_NUM(node, "adx") == adx)
            return node;
    }
    return NULL;
}

void adx_element_init(adx_cache_t *root, adx_cache_t *element_root)
{
    adx_cache_t *policy_list = adx_cache_find(root, ADX_CACHE_STR("policy_list"));
    if (!policy_list) return;

    adx_cache_t *creative_list = adx_cache_find(root, ADX_CACHE_STR("creative_list"));
    if (!creative_list) return;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &creative_list->child_list) {
        adx_cache_t *creative = adx_list_entry(p, adx_cache_t, next_list);
        adx_cache_t *policy = adx_cache_find(policy_list, ADX_CACHE_NUM(GET_CACHE_FIND_NUM(creative, "policyid")));
        adx_element_adx(policy, creative, element_root);
    }
}




adx_cache_t *adx_select(adx_cache_t *root, char *key)
{
    adx_cache_t *cache = adx_cache_find(root, ADX_CACHE_STR(key));
    return cache ? cache : adx_cache_find(root, ADX_CACHE_STR(ADX_DUMP_ALL));
}

adx_cache_t *adx_select(adx_cache_t *root, int key)
{
    adx_cache_t *cache = adx_cache_find(root, ADX_CACHE_NUM(key));
    return cache ? cache : adx_cache_find(root, ADX_CACHE_STR(ADX_DUMP_ALL));
}

adx_cache_t *adx_select(adx_cache_t *root, const char *section, char *key)
{
    adx_cache_t *cache = adx_cache_find(root, ADX_CACHE_STR(section));
    return cache ? adx_select(cache, key) : NULL;
}

adx_cache_t *adx_select(adx_cache_t *root, const char *section, int key)
{
    adx_cache_t *cache = adx_cache_find(root, ADX_CACHE_STR(section));
    return cache ? adx_select(cache, key) : NULL;
}

