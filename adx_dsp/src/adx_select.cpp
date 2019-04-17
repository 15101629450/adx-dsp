
#include "adx_dsp.h"

// 广告位 
typedef struct {

    adx_list_t next;

    /*** input ***/
    char *adx_slot_id;
    int adx_slot_sequence_id;
    int adx_slot_type;
    int price_floor;

    adx_com_banner banner;
    adx_com_native native;
    adx_com_video video;

    adx_com_device *device;
    adx_com_app *app;
    adx_com_geo *geo;

    /*** ouput ***/
    adx_cache_t *map;
    adx_cache_t *creative;
    char *template_buffer;
    int status;
    int price;

} __adx_com_slot;


int adx_select_creative(adx_request_t *r, adx_com_slot *adx_slot, adx_cache_t *cache)
{
    adx_list_t *p = NULL;
    adx_list_for_each(p, &cache->child_list) {
        adx_cache_t *node = adx_list_entry(p, adx_cache_t, next_list);

        int map_id = adx_cache_value_to_number(GET_CACHE_TO_KEY(node));
        int bid_price = adx_cache_value_to_number(GET_CACHE_TO_VAL(node));
        fprintf(stdout, "[%d][%d]\n", map_id, bid_price);

        // int campaign_id = GET_CACHE_FIND_NUM(node, "campaign_id");
        // int policy_id = GET_CACHE_FIND_NUM(node, "policy_id");
        // int creative_id = GET_CACHE_FIND_NUM(node, "creative_id");

        // regioncode
        // connectiontype
        // carrier
        // devicetype
        // os
        // make

        // icon -> key -> url
        // imgs -> key -> url
    }

    return 0;
}

int adx_select_banner(adx_request_t *r, adx_com_slot *adx_slot, adx_cache_t *cache)
{
    cache = adx_cache_find(cache, ADX_CACHE_STR("banner"));
    fprintf(stdout, "[banner][%p]\n", cache);
    if (!cache) return -1;

    cache = adx_cache_find(cache, ADX_CACHE_STR(adx_slot->banner.key));
    fprintf(stdout, "[size-key][%p]\n", cache);
    if (!cache) return -1;

    return adx_select_creative(r, adx_slot, cache);
}

int adx_select_native(adx_request_t *r, adx_com_slot *adx_slot, adx_cache_t *cache)
{
    cache = adx_cache_find(cache, ADX_CACHE_STR("native"));
    if (!cache) return -1;
    return 0;
}

int adx_select_video(adx_request_t *r, adx_com_slot *adx_slot, adx_cache_t *cache)
{
    cache = adx_cache_find(cache, ADX_CACHE_STR("video"));
    if (!cache) return -1;
    return 0;
}

int adx_select_adx(adx_request_t *r, adx_com_slot *adx_slot)
{
    adx_cache_t *cache = adx_cache_find(get_cache_root(), ADX_CACHE_STR("adx"));
    fprintf(stdout, "[root][%p]\n", cache);
    if (!cache) return -1;

    cache = adx_cache_find(cache, ADX_CACHE_NUM(r->adx));
    fprintf(stdout, "[adx][%d][%p]\n", r->adx, cache);
    if (!cache) return -1;

    if (adx_slot->adx_slot_type == ADX_IMP_BANNER)
        return adx_select_banner(r, adx_slot, cache);
    if (adx_slot->adx_slot_type == ADX_IMP_NATIVE)
        return adx_select_native(r, adx_slot, cache);
    if (adx_slot->adx_slot_type == ADX_IMP_VIDEO)
        return adx_select_video(r, adx_slot, cache);

    return -1;
}

int adx_select(adx_request_t *r)
{
    adx_list_t *p = NULL;
    adx_list_for_each(p, &r->adx_slot_list) {
        adx_com_slot *adx_slot = (adx_com_slot *)p;
        adx_slot->status = adx_select_adx(r, adx_slot);
    }

    return 0;
}


