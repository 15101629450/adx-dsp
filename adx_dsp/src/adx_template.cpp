
#include "adx_dsp.h"

#define MACRO_REPLACE_EMPTY (char*)"#"

#if 0

char *adx_template_device_id(adx_pool_t *pool, adx_com_device *device, char *buffer)
{
    switch(device->device_id_code) {
        case ADX_DEVICEID_IMEI:
        case ADX_DEVICEID_IMEI_SHA1:
        case ADX_DEVICEID_IMEI_MD5:
            return adx_macro_replace(pool, buffer, "__IMEI__", device->device_id);

        case ADX_DEVICEID_ANDROID_ID:
        case ADX_DEVICEID_ANDROID_ID_SHA1:
        case ADX_DEVICEID_ANDROID_ID_MD5:
            return adx_macro_replace(pool, buffer, "__AndroidID__", device->device_id);

        case ADX_DEVICEID_IDFA:
        case ADX_DEVICEID_IDFA_SHA1:
        case ADX_DEVICEID_IDFA_MD5:
            return adx_macro_replace(pool, buffer, "__IDFA__", device->device_id);

        case ADX_DEVICEID_MAC:
        case ADX_DEVICEID_MAC_SHA1:
        case ADX_DEVICEID_MAC_MD5:
            return adx_macro_replace(pool, buffer, "__MAC__", device->device_id);
    }

    return buffer;
}

char *adx_template_adm(adx_request_t *r, adx_select_t *select, adx_com_device *device)
{
    adx_cache_t *adms = adx_cache_find(r->adx_template, ADX_CACHE_STR("adms"));
    if (!adms) return NULL;

    for (int i = 0; i < adx_cache_find_array_lenght(adms); i++) {
        adx_cache_t *node = adx_cache_find_array(adms, i);
        if (device->os == GET_CACHE_VALUE_NUM(node, "os"))
            return GET_CACHE_VALUE_STR(node, "adm");
    }

    return NULL;
}

int adx_template(adx_request_t *r, adx_select_t *select)
{
    char *SOURCEURL = select->img_path;

    adx_cache_t *imonitorurl_list = adx_cache_find(select->creative, ADX_CACHE_STR("imonitorurl"));
    adx_cache_t *cmonitorurl_list = adx_cache_find(select->creative, ADX_CACHE_STR("cmonitorurl"));
    char *IMONITORURL1 = adx_cache_to_string(adx_cache_find_array(imonitorurl_list, 0));
    char *IMONITORURL2 = adx_cache_to_string(adx_cache_find_array(imonitorurl_list, 1));
    char *CMONITORURL = adx_cache_to_string(adx_cache_find_array(cmonitorurl_list, 0));
    if (!IMONITORURL1) IMONITORURL1 = MACRO_REPLACE_EMPTY;
    if (!IMONITORURL2) IMONITORURL2 = MACRO_REPLACE_EMPTY;
    if (!CMONITORURL) CMONITORURL = MACRO_REPLACE_EMPTY;

    IMONITORURL1 = adx_template_device_id(r->pool, r->device, IMONITORURL1);
    IMONITORURL2 = adx_template_device_id(r->pool, r->device, IMONITORURL2);
    CMONITORURL = adx_template_device_id(r->pool, r->device, CMONITORURL);

    IMONITORURL1 = adx_macro_replace(r->pool, IMONITORURL1, "__IP__", r->device->ip);
    IMONITORURL2 = adx_macro_replace(r->pool, IMONITORURL2, "__IP__", r->device->ip);
    CMONITORURL = adx_macro_replace(r->pool, CMONITORURL, "__IP__", r->device->ip);

    fprintf(stdout, "IMONITORURL1=%s\n", IMONITORURL1);
    fprintf(stdout, "IMONITORURL2=%s\n", IMONITORURL2);
    fprintf(stdout, "CMONITORURL=%s\n", CMONITORURL);

#if 0
    char *IURL = GET_CACHE_VALUE_STR(r->adx_template, "iurl");
    char *CURL = GET_CACHE_VALUE_STR(select->creative, "curl");
    char *CTURL = GET_CACHE_VALUE_STR(r->adx_template, "cturl");
    char *NURL = GET_CACHE_VALUE_STR(r->adx_template, "nurl");
    char *LANDINGURL = GET_CACHE_VALUE_STR(select->creative, "landingurl");
    if (!IURL) IURL = MACRO_REPLACE_EMPTY;
    if (!CURL) CURL = MACRO_REPLACE_EMPTY;
    if (!CTURL) CTURL = MACRO_REPLACE_EMPTY;
    if (!NURL) NURL = MACRO_REPLACE_EMPTY;
    if (!LANDINGURL) LANDINGURL = MACRO_REPLACE_EMPTY;

    IURL = adx_template_device_id(r->pool, r->device, IURL);
    CURL = adx_template_device_id(r->pool, r->device, CURL);
    CTURL = adx_template_device_id(r->pool, r->device, CTURL);
    NURL = adx_template_device_id(r->pool, r->device, NURL);
    LANDINGURL = adx_template_device_id(r->pool, r->device, LANDINGURL);

    fprintf(stdout, "IURL=%s\n", IURL);
    fprintf(stdout, "CURL=%s\n", CURL);
    fprintf(stdout, "CTURL=%s\n", CTURL);
    fprintf(stdout, "NURL=%s\n", NURL);
    fprintf(stdout, "LANDINGURL=%s\n", LANDINGURL);
#else
    char *IURL = MACRO_REPLACE_EMPTY;
    char *CURL = MACRO_REPLACE_EMPTY;
    char *CTURL = MACRO_REPLACE_EMPTY;
#endif
    fprintf(stdout, "\n");

    char *buffer = adx_template_adm(r, select, r->device);
    fprintf(stdout, "%s\n", buffer);
    fprintf(stdout, "\n");

    buffer = adx_macro_replace(r->pool, buffer, "#SOURCEURL#", SOURCEURL);
    buffer = adx_macro_replace(r->pool, buffer, "#IMONITORURL1#", IMONITORURL1);
    buffer = adx_macro_replace(r->pool, buffer, "#IMONITORURL2#", IMONITORURL2);
    buffer = adx_macro_replace(r->pool, buffer, "#CMONITORURL#", CMONITORURL);
    buffer = adx_macro_replace(r->pool, buffer, "#IURL#", IURL);
    buffer = adx_macro_replace(r->pool, buffer, "#CURL#", CURL);
    buffer = adx_macro_replace(r->pool, buffer, "#CTURL#", CTURL);

    select->template_buffer = buffer;
    fprintf(stdout, "%s\n\n", buffer);
    return 0;
}

#if 0
adx_request_t *adx_template_init()
{
    adx_list_t *conf = adx_conf_file_load(DSP_CONF_PATH);
    if (!conf) return NULL;

    adx_pool_t *pool = adx_pool_create();
    adx_redis_conn *conn = adx_redis_init(conf);
    if (!conn) return NULL;

    adx_cache_t *root = adx_cache_create();
    adx_cache_t *map_list = adx_cache_add(root, ADX_CACHE_STR("map_list"), ADX_CACHE_NULL()); // 创建 map_list
    adx_cache_t *policy_list = adx_cache_add(root, ADX_CACHE_STR("policy_list"), ADX_CACHE_NULL()); // 创建 policy_list
    adx_cache_t *creative_list = adx_cache_add(root, ADX_CACHE_STR("creative_list"), ADX_CACHE_NULL()); // 创建 creative_list

    adx_cache_t *policy_node = adx_cache_add(policy_list, ADX_CACHE_NUM(398), ADX_CACHE_NULL());
    adx_dump_load_policy_mapid(pool, conn, policy_node, map_list);
    adx_dump_load_policy_target(pool, conn, policy_node);

    adx_dump_load_creative(pool, conn, creative_list, map_list);
    adx_dump_load_redis_json(pool, conn, root, "adx_templates");

    adx_request_t *r = new adx_request_t;
    r->pool = pool;
    r->map_list = map_list;
    r->policy_list = policy_list;
    r->creative_list = creative_list;

    r->template_list = adx_cache_find(root, ADX_CACHE_STR("templates"));
    r->adx_template = adx_element_templates(r->template_list, 3);

    adx_com_device *device = new adx_com_device;
    device->device_id = (char*)"13F1659C-27F3-4868-8C1B-94530E9070ED";
    device->device_id_code = ADX_DEVICEID_IDFA;
    device->os = ADX_OS_IOS;

    r->device = device;
    return r;
}

adx_select_t *adx_select_init(adx_request_t *r)
{
    adx_select_t *s = new adx_select_t;
    adx_cache_t *map = adx_cache_find(r->map_list, ADX_CACHE_NUM(6413));
    adx_cache_t *creative = adx_cache_find(r->creative_list, ADX_CACHE_NUM(GET_CACHE_VALUE_NUM(map, "creative_id")));
    s->creative = creative;
    s->map = map;

    adx_com_imp *imp = new adx_com_imp;
    s->imp = imp;

    imp->type = ADX_IMP_BANNER;
    imp->icon = NULL;
    imp->img = (char *)"320X480";

    return s;
}

int main(int argc, char *argv[])
{
    adx_request_t *r = adx_template_init();
    adx_select_t *select = adx_select_init(r);
    adx_template(r, select);
    fprintf(stdout, "%s\n", select->template_buffer);

    return 0;
}
#endif


#endif



