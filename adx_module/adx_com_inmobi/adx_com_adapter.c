
#include <adx_com_module.h>

#define SEAT_INMOBI "fc56e2b620874561960767c39427298c"

void adx_com_inmobi_parse_banner(adx_pool_t *pool, adx_json_t *banner)
{
    int w = adx_json_find_value_number(banner, "w");
    int h = adx_json_find_value_number(banner, "h");
    fprintf(stdout, "banner -> w = %d\n", w);
    fprintf(stdout, "banner -> h = %d\n", h);

    adx_json_t *mimes = adx_json_find(banner, "mimes");
    if (adx_json_array_check(mimes) != 0) return;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &mimes->child) {
        adx_json_t *node = adx_list_entry(p, adx_json_t, next);
        if (node->type == ADX_JSON_STRING)
            fprintf(stdout, "banner -> mimes = %s\n", node->val);
    }
}

/*
   Native->Asset:

   img->type:  
   1   icon
   2   logo(brand/app)
   3   main(image)

   data->type:
   2   desc        广告的产品或服务相关联的描述性文本。
   3   rating      评分,提供给用户的产品评级。 例如，应用商店的评分从0到5。
   5   downloads   下载,数字格式为字符串该产品的下载/安装次数
   12  CTA         描述性文字，描述目标网址的按钮
   */
void adx_com_inmobi_parse_native(adx_pool_t *pool, adx_json_t *native)
{
    adx_json_t *requestobj = adx_json_find(native, "requestobj");
    int layout = adx_json_find_value_number(requestobj, "layout");
    fprintf(stdout, "native -> layout = %d\n", layout);

    adx_json_t *assets = adx_json_find(requestobj, "assets");
    if (adx_json_array_check(assets) != 0) return;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &assets->child) {
        adx_json_t *node = adx_list_entry(p, adx_json_t, next);
        adx_json_t *img = adx_json_find(node, "img");
        int type = adx_json_find_value_number(img, "type");
        if (type == 1) { // icon

            int w = adx_json_find_value_number(img, "wmin");
            int h = adx_json_find_value_number(img, "hmin");
            fprintf(stdout, "native -> w = %d\n", w);
            fprintf(stdout, "native -> h = %d\n", h);

        } else if (type == 3) { // image

            int w = adx_json_find_value_number(img, "wmin");
            int h = adx_json_find_value_number(img, "hmin");
            fprintf(stdout, "native -> w = %d\n", w);
            fprintf(stdout, "native -> h = %d\n", h);
        }
    }
}

void adx_com_inmobi_parse_device(adx_pool_t *pool, adx_json_t *device)
{
    /* ifa: ID sanctioned for advertiser use in the clear (i.e., not hashed).
     * didsha1: Hardware device ID (only IMEI); hashed via SHA1.
     * didmd5: Hardware device ID (only IMEI); hashed via MD5
     * dpidsha1: Platform device ID (e.g., Android ID); hashed via SHA1
     * dpidmd5: Platform device ID (e.g., Android ID); hashed via MD5 */
    char *ifa = adx_json_find_value_string(device, "ifa");
    char *imei_sha1 = adx_json_find_value_string(device, "didsha1"); // IMEI SHA1
    char *imei_md5 = adx_json_find_value_string(device, "didmd5"); // IMEI MD5
    char *android_id_sha1 = adx_json_find_value_string(device, "dpidsha1"); // Android id SHA1
    char *android_id_md5 = adx_json_find_value_string(device, "dpidmd5"); // Android id MD5

    fprintf(stdout, "device -> device_id -> ifa = %s\n", ifa);
    fprintf(stdout, "device -> device_id -> imei_sha1 = %s\n", imei_sha1);
    fprintf(stdout, "device -> device_id -> imei_md5 = %s\n", imei_md5);
    fprintf(stdout, "device -> device_id -> android_id_sha1 = %s\n", android_id_sha1);
    fprintf(stdout, "device -> device_id -> android_id_md5 = %s\n", android_id_md5);

    /* device -> ext
     * idfa: IDFA of the iOS device.
     * idfasha1: Sha1 hashed privacy aware unique identifier on iOS6 and above. Replacement for UDID.
     * idfamd5: MD5 hashed privacy aware unique identifier on iOS6 and above.Replacement for UDID.
     * gpid: If the device is android, the advertising id is stored here. */
    adx_json_t *ext = adx_json_find(device, "ext");
    // char *idfa = adx_json_find_value_string(ext, "idfa");
    char *idfa_sha1 = adx_json_find_value_string(ext, "idfasha1");
    char *idfa_md5 = adx_json_find_value_string(ext, "idfamd5");
    char *android_id = adx_json_find_value_string(ext, "gpid");

    // fprintf(stdout, "device -> device_id -> idfa = %s\n", idfa);
    fprintf(stdout, "device -> device_id -> idfa_sha1 = %s\n", idfa_sha1);
    fprintf(stdout, "device -> device_id -> idfa_md5 = %s\n", idfa_md5);
    fprintf(stdout, "device -> device_id -> android_id = %s\n", android_id);

    /* 1: Mobile/Tablet
     * 2: Personal Computer
     * 3: Connected TV
     * 4: Phone
     * 5: Tablet
     * 6: Connected Device
     * 7: Set Top Box */
    int device_type = adx_json_find_value_number(device, "devicetype");
    // int device_code = ADX_DEVICE_UNKNOWN;
    // if (device_type == 1 || device_type == 4) device_code = ADX_DEVICE_MOBILE;
    // else if (device_type == 5) device_code = ADX_DEVICE_PAD;
    // else if (device_type == 2) device_code = ADX_DEVICE_PC;
    fprintf(stdout, "device -> device_type = %d\n", device_type);

    // int os_code = ADX_OS_UNKNOWN;
    char *os = adx_json_find_value_string(device, "os");
    if (os) {
        // if (adx_strstr(os, "ios")) os_code = ADX_OS_IOS;
        // else if (adx_strstr(os, "android")) os_code = ADX_OS_ANDROID;
        // else if (adx_strstr(os, "windows")) os_code = ADX_OS_WINDOWS;
    }
    fprintf(stdout, "device -> os = %s\n", os);

    char *osv = adx_json_find_value_string(device, "osv");
    fprintf(stdout, "device -> osv = %s\n", osv);

    /* 0: Unknown
     * 1: Ethernet
     * 2: WIFI
     * 3: Cellular Network – Unknown Generation
     * 4: Cellular Network – 2G */
    int connection_type = adx_json_find_value_number(device, "connectiontype");
    // int connection_code = ADX_CONNECTION_UNKNOWN;
    // if (connection_type == 2) connection_code = ADX_CONNECTION_WIFI;
    // else if (connection_type == 3) connection_code = ADX_CONNECTION_4G;
    // else if (connection_type == 4) connection_code = ADX_CONNECTION_2G;
    fprintf(stdout, "device -> connection_type = %d\n", connection_type);

    // 设备机型
    char *model = adx_json_find_value_string(device, "model");
    fprintf(stdout, "device -> model = %s\n", model);

    // 设备品牌
    char *brand = adx_json_find_value_string(device, "make");
    fprintf(stdout, "device -> brand = %s\n", brand);

    // UA
    char *ua = adx_json_find_value_string(device, "ua");
    fprintf(stdout, "device -> ua = %s\n", ua);

    // IP
    char *ip = adx_json_find_value_string(device, "ip");
    fprintf(stdout, "device -> ip = %s\n", ip);
}

void adx_com_inmobi_parse_app(adx_pool_t *pool, adx_json_t *app)
{
    char *id = adx_json_find_value_string(app, "id");
    fprintf(stdout, "app -> id = %s\n", id);

    char *name = adx_json_find_value_string(app, "name");
    fprintf(stdout, "app -> name = %s\n", name);
}

void adx_com_inmobi_parse(adx_pool_t *pool, char *buffer)
{
    adx_json_t *json = adx_json_parse(pool, buffer);
    char *bid = adx_json_find_value_string(json, "id");
    fprintf(stdout, "bid = %s\n", bid);

    adx_json_t *adx_slots = adx_json_find(json, "imp");
    if (adx_json_array_check(adx_slots) != 0) return;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &adx_slots->child) {
        adx_json_t *adx_slot = adx_list_entry(p, adx_json_t, next);
        adx_json_t *banner = adx_json_find(adx_slot, "banner");
        adx_json_t *native = adx_json_find(adx_slot, "native");

        if (banner) adx_com_inmobi_parse_banner(pool, banner);
        if (native) adx_com_inmobi_parse_native(pool, native);
    }

    adx_com_inmobi_parse_device(pool, adx_json_find(json, "device"));
    adx_com_inmobi_parse_app(pool, adx_json_find(json, "app"));
}

ngx_chain_t *adx_com_inmobi_adapter_request(ngx_pool_t *pool, ngx_chain_t *src)
{
    ngx_str_t str = adx_chain_to_str(pool, src);
    adx_com_inmobi_parse(pool, (char *)str.data);
    return src;
}

ngx_chain_t *adx_com_inmobi_adapter_response(ngx_pool_t *pool, ngx_chain_t *src)
{
#if 0
    adx_json_t *json = ADX_JSON_ROOT(pool);
    adx_json_add_string(pool, json, "id", bid);
    adx_json_add_string(pool, json, "bidid", bid);
    adx_json_add_string(pool, json, "cur", "CNY");

    adx_json_t *seatbid = adx_json_add_array(pool, json, "seatbid");
    adx_json_t *seatbid_obj = adx_json_add_object(pool, seatbid, NULL);
    adx_json_add_string(pool, seatbid_obj, "seat", SEAT_INMOBI);
    adx_json_t *bid = adx_json_add_array(pool, seatbid_obj, "bid");
    adx_json_t *bid_obj = adx_json_add_object(pool, bid, NULL);

    adx_json_add_number(pool, bid_obj, "id", 1);
    adx_json_add_number(pool, bid_obj, "price", price);

    // adx_json_display(json);

    char *buf = adx_json_to_buffer(pool, json);
    ngx_chain_t *dest = adx_chain_alloc_r(pool, buf);
#endif
    return src;
}



