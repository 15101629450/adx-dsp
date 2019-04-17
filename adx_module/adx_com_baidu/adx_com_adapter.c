
#include <adx_com_module.h>

ngx_bid_request_t *adx_com_baidu_adapter_init(ngx_pool_t *pool, ngx_chain_t *src)
{
    ngx_str_t buffer = adx_chain_to_str(pool, src);
    ngx_protobuf_context_t ctx;
    ctx.pool = pool;
    ctx.buffer.start = buffer.data;
    ctx.buffer.pos = ctx.buffer.start;
    ctx.buffer.last = buffer.data + buffer.len;
    ctx.reuse_strings = 1;

    ngx_bid_request_t *bid_request = ngx_pcalloc(pool, sizeof(ngx_bid_request_t));
    ngx_bid_request__unpack(bid_request, &ctx);
    return bid_request;
}

ngx_com_request_t *adx_com_baidu_adapter_to_com(ngx_pool_t *pool, ngx_bid_request_t *bid_request)
{
    ngx_uint_t i;

    ngx_bid_request_mobile_t *mobile = bid_request->mobile;
    ngx_bid_request_mobile_mobile_app_t *app = mobile->mobile_app;

    ngx_com_request_t *com_request = ngx_pcalloc(pool, sizeof(ngx_com_request_t));
    ngx_com_request_device_t *com_device = ngx_pcalloc(pool, sizeof(ngx_com_request_device_t));
    ngx_com_request_app_t *com_app = ngx_pcalloc(pool, sizeof(ngx_com_request_app_t));

    NGX_PROTOBUF_SET_MEMBER(com_request, device, com_device);
    NGX_PROTOBUF_SET_MEMBER(com_request, app, com_app);

    // adx    
    NGX_PROTOBUF_SET_NUMBER(com_request, adx, 8);

    // bid
    ngx_str_t bid = bid_request->id;
    NGX_PROTOBUF_SET_STRING(com_request, bid, &bid);

    ngx_bid_request_ad_slot_t *adx_slots = bid_request->adslot->elts;
    for (i = 0; i < bid_request->adslot->nelts; i++) {
        ngx_bid_request_ad_slot_t *adx_slot = &adx_slots[i];
        ngx_com_request_adx_slot_t *com_adx_slot =
            ngx_com_request__add__adx_slot(com_request, pool);

        // 广告位ID, 全局唯一id
        char adx_slot_id_str[128];
        long adx_slot_id_num = adx_slot->ad_block_key;
        int adx_slot_id_len = sprintf(adx_slot_id_str, "%lu", adx_slot_id_num);
        ngx_str_t adx_slot_id = {adx_slot_id_len, (u_char *)adx_slot_id_str};
        NGX_PROTOBUF_SET_STRING(com_adx_slot, adx_slot_id, &adx_slot_id);

        // 当前页面广告位顺序id,同一页面从1开始
        int adx_slot_sequence_id = adx_slot->sequence_id;
        NGX_PROTOBUF_SET_NUMBER(com_adx_slot, adx_slot_sequence_id, adx_slot_sequence_id);

        // 底价
        int price_floor = adx_slot->minimum_cpm;
        NGX_PROTOBUF_SET_NUMBER(com_adx_slot, price_floor, price_floor);

        // 展示类
        // 0  固定
        // 1  悬浮
        // 8  图+ICON
        // 11 插屏
        // 12 开屏
        // 13 原生
        // 21 视频前贴片
        // 22 视频中贴片
        // 23 视频后贴片
        // 26 视频暂停
        int adx_slot_type = adx_slot->adslot_type;
        int com_adx_slot_type = NGX_COMREQUEST_ADXSLOT_ADX_SLOT_UNKNOWN;
        // banner
        if (adx_slot_type == 0
                || adx_slot_type == 1
                || adx_slot_type == 11
                || adx_slot_type == 12
                || adx_slot_type == 26)
            com_adx_slot_type = NGX_COMREQUEST_ADXSLOT_ADX_SLOT_BANNER;

        // native
        else if (adx_slot_type == 8 || adx_slot_type == 13)
            com_adx_slot_type = NGX_COMREQUEST_ADXSLOT_ADX_SLOT_NATIVE;

        // video
        else if (adx_slot_type == 21
                || adx_slot_type == 22
                || adx_slot_type == 23)
            com_adx_slot_type = NGX_COMREQUEST_ADXSLOT_ADX_SLOT_VIDEO;
        NGX_PROTOBUF_SET_NUMBER(com_adx_slot, adx_slot_type, com_adx_slot_type);

        // banner 信息
        if (com_adx_slot_type == NGX_COMREQUEST_ADXSLOT_ADX_SLOT_BANNER) {

            int w = adx_slot->width;
            int h = adx_slot->height;
            ngx_com_request_adx_slot_banner_t *com_banner =
                ngx_pcalloc(pool, sizeof(ngx_com_request_adx_slot_banner_t));
            NGX_PROTOBUF_SET_MEMBER(com_adx_slot, banner, com_banner);
            NGX_PROTOBUF_SET_NUMBER(com_banner, width, w);
            NGX_PROTOBUF_SET_NUMBER(com_banner, height, h);

            // native 信息
        } else if (com_adx_slot_type == NGX_COMREQUEST_ADXSLOT_ADX_SLOT_NATIVE) {
#if 0
            ngx_com_request_adx_slot_native_t *com_native = 
                ngx_pcalloc(pool, sizeof(ngx_com_request_adx_slot_native_t));
            NGX_PROTOBUF_SET_MEMBER(com_adx_slot, native, com_native);
            NGX_PROTOBUF_SET_NUMBER(com_native, width, w);
            NGX_PROTOBUF_SET_NUMBER(com_native, height, h);
#endif
            // video 信息
        } else if (com_adx_slot_type == NGX_COMREQUEST_ADXSLOT_ADX_SLOT_VIDEO) {
#if 0
            ngx_com_request_adx_slot_video_t *com_video =
                ngx_pcalloc(pool, sizeof(ngx_com_request_adx_slot_video_t));
            NGX_PROTOBUF_SET_MEMBER(com_adx_slot, video, com_video);
            NGX_PROTOBUF_SET_NUMBER(com_video, width, w);
            NGX_PROTOBUF_SET_NUMBER(com_video, height, h);
#endif
        }
#if 0
        // 允许的创意类型
        // 0 文本
        // 1 图片
        // 2 FLASH
        // 4 图文
        // 5 链接单元
        // 7 Video
        ngx_uint_t j;
        int *creative_types = adx_slot->creative_type->elts;
        for (j = 0; j < adx_slot->creative_type->nelts; j++) {
            int creative_type = creative_types[j];
            fprintf(stdout, "adx_slot[%ld] -> creative_type = %d\n", i, creative_type);
        }

        // 动态/静态创意
        int *creative_desc_types = adx_slot->creative_desc_type->elts;
        for (j = 0; j < adx_slot->creative_desc_type->nelts; j++) {
            int creative_desc_type = creative_desc_types[j];
            fprintf(stdout, "adx_slot[%ld] -> creative_type = %d\n", i, creative_desc_type);
        }

        // 展示位置
        // int adx_place = adslot.slot_visibility();
#endif
    }

    // 设备ID
    ngx_bid_request_mobile_mobile_id_t *device_ids = mobile->id->elts;
    for (i = 0; i < mobile->id->nelts; i++) {
        ngx_bid_request_mobile_mobile_id_t *device_id = &device_ids[i];
        ngx_com_request_device_device_id_t *com_device_id =
            ngx_com_request_device__add__device_id(com_device, pool);

        ngx_str_t id = device_id->id;
        NGX_PROTOBUF_SET_STRING(com_device_id, id, &id);

        // UNKNOWN = 0; // 未知
        // IMEI = 1; // IMEI
        // MAC = 2; // MAC地址
        int type = device_id->type;
        int com_type = NGX_COMREQUEST_DEVICE_ADX_DEVICEID_UNKNOW;
        if (type == 1) com_type = NGX_COMREQUEST_DEVICE_ADX_DEVICEID_IMEI;
        else if (type == 2) com_type = NGX_COMREQUEST_DEVICE_ADX_DEVICEID_MAC;
        NGX_PROTOBUF_SET_NUMBER(com_device_id, type, com_type);
    }

    // 设备ID
    ngx_bid_request_mobile_for_advertising_id_t *device2_ids = mobile->for_advertising_id->elts;
    for (i = 0; i < mobile->for_advertising_id->nelts; i++) {
        ngx_bid_request_mobile_for_advertising_id_t *device_id = &device2_ids[i];
        ngx_com_request_device_device_id_t *com_device_id =
            ngx_com_request_device__add__device_id(com_device, pool);

        ngx_str_t id = device_id->id;
        NGX_PROTOBUF_SET_STRING(com_device_id, id, &id);

        // UNKNOWN = 0;    // 未知
        // ANDROID_ID = 4; // Android适用
        // IDFA = 5;       // IOS适用
        int type = device_id->type;
        int com_type = NGX_COMREQUEST_DEVICE_ADX_DEVICEID_UNKNOW;
        if (type == 4) com_type = NGX_COMREQUEST_DEVICE_ADX_DEVICEID_ANDROID_ID;
        else if (type == 5) com_type = NGX_COMREQUEST_DEVICE_ADX_DEVICEID_IDFA;
        NGX_PROTOBUF_SET_NUMBER(com_device_id, type, com_type);
    }

    // 设备类型
    // BidRequest_Mobile::HIGHEND_PHONE
    // UNKNOWN_DEVICE = 0;
    // HIGHEND_PHONE = 1;
    // TABLET = 2;
    int device_type = mobile->device_type;
    int com_device_type = NGX_COMREQUEST_DEVICE_ADX_DEVICE_UNKNOWN;
    if (device_type == 1) com_device_type = NGX_COMREQUEST_DEVICE_ADX_DEVICE_MOBILE;
    else if (device_type == 2) com_device_type = NGX_COMREQUEST_DEVICE_ADX_DEVICE_PAD;
    NGX_PROTOBUF_SET_NUMBER(com_device, device_type, com_device_type);

    // 操作系统
    // BidRequest_Mobile::UNKNOWN_OS
    // UNKNOWN_OS = 0;
    // IOS = 1;
    // ANDROID = 2;
    // WINDOWS_PHONE = 3;
    int os = mobile->platform;
    int com_os = NGX_COMREQUEST_DEVICE_ADX_OS_UNKNOWN;
    if (os == 1) com_os = NGX_COMREQUEST_DEVICE_ADX_OS_IOS;
    else if (os == 2) com_os = NGX_COMREQUEST_DEVICE_ADX_OS_ANDROID;
    else if (os == 3) com_os = NGX_COMREQUEST_DEVICE_ADX_OS_WINDOWS;
    NGX_PROTOBUF_SET_NUMBER(com_device, os, com_os);

    // 操作系统版本
    ngx_bid_request_mobile_device_os_version_t *os_version = mobile->os_version;
    int os_version_major = os_version->os_version_major;
    int os_version_minor = os_version->os_version_minor;
    int os_version_micro = os_version->os_version_micro;

    char os_version_str[256];
    int os_version_len = sprintf(os_version_str, "%d.%d.%d", 
            os_version_major, os_version_minor, os_version_micro);
    ngx_str_t com_os_version = {os_version_len, (u_char *)os_version_str};
    NGX_PROTOBUF_SET_STRING(com_device, os_version, &com_os_version);

    // 运营商编号（MCC+MNC编号）
    // 例如中国移动 46000
    // 前三位是Mobile Country Code
    // 后两位是Mobile Network Code
    int carrier = mobile->carrier_id;
    int com_carrier = NGX_COMREQUEST_DEVICE_ADX_CARRIER_UNKNOWN;
    if (carrier == 46000 || carrier == 46002 || carrier == 46007 || carrier == 46008 || carrier == 46020)
        com_carrier = NGX_COMREQUEST_DEVICE_ADX_CARRIER_CHINA_MOBILE;
    else if (carrier == 46001 || carrier == 46006 || carrier == 46009)
        com_carrier = NGX_COMREQUEST_DEVICE_ADX_CARRIER_CHINA_UNICOM;
    else if (carrier == 46003 || carrier == 46005 || carrier == 46011)
        com_carrier = NGX_COMREQUEST_DEVICE_ADX_CARRIER_CHINA_TELECOM;
    NGX_PROTOBUF_SET_NUMBER(com_device, carrier, com_carrier);

    // 网络类型
    // BidRequest_Mobile::WIFI
    // UNKNOWN_NETWORK = 0;
    // WIFI = 1;
    // MOBILE_2G = 2;
    // MOBILE_3G = 3;
    // MOBILE_4G = 4;
    int connection = mobile->wireless_network_type;
    int com_connection = NGX_COMREQUEST_DEVICE_ADX_CONNECTION_UNKNOWN;
    if (connection == 1) 
        com_connection = NGX_COMREQUEST_DEVICE_ADX_CONNECTION_WIFI;
    else if (connection == 2)
        com_connection = NGX_COMREQUEST_DEVICE_ADX_CONNECTION_2G;
    else if (connection == 3)
        com_connection = NGX_COMREQUEST_DEVICE_ADX_CONNECTION_3G;
    else if (connection == 4)
        com_connection = NGX_COMREQUEST_DEVICE_ADX_CONNECTION_4G;
    NGX_PROTOBUF_SET_NUMBER(com_device, connection, com_connection);

    // 设备机型
    ngx_str_t model = mobile->model;
    NGX_PROTOBUF_SET_STRING(com_device, model, &model);

    // 设备品牌
    ngx_str_t brand = mobile->brand;
    NGX_PROTOBUF_SET_STRING(com_device, brand, &brand);

    // UA
    ngx_str_t user_agent = bid_request->user_agent;
    NGX_PROTOBUF_SET_STRING(com_device, user_agent, &user_agent);

    // IP
    ngx_str_t ip = bid_request->ip;
    NGX_PROTOBUF_SET_STRING(com_device, ip, &ip);

    // 如果来自苹果商店，则直接是app-store id
    // 如果来自Android设备，则是package的全名
    ngx_str_t app_id = app->app_bundle_id;
    NGX_PROTOBUF_SET_STRING(com_app, app_id, &app_id);

    // app应用分类
    int app_category = app->app_category; // 暂时未匹配
    NGX_PROTOBUF_SET_NUMBER(com_app, app_category, app_category);

    return com_request;
}

ngx_bid_request_t *adx_com_baidu_adapter_from_com(ngx_pool_t *pool, ngx_com_response_t *com_response)
{
    // optional int32 max_cpm = 3;
    // optional string extdata = 5;
    return NULL;
}

ngx_chain_t *adx_com_baidu_adapter_request(ngx_pool_t *pool, ngx_chain_t *src)
{
    ngx_bid_request_t *bid_request = adx_com_baidu_adapter_init(pool, src);
    ngx_com_request_t *com_request = adx_com_baidu_adapter_to_com(pool, bid_request);
    return adx_com_to_chain(pool, com_request);
}

ngx_chain_t *adx_com_baidu_adapter_response(ngx_pool_t *pool, ngx_chain_t *src)
{
    // adx_chain_print(src);
    ngx_chain_t *dest = adx_chain_alloc_r(pool, "OK\r\n");
    return dest;
}



