
#include <stdio.h>
#include <iostream>
#include "adx_alloc.h"
#include "adx_json.h"
#include "pack.pb.h"

enum {
    ADX_DEVICE_UNKNOWN = 100,   // 未知
    ADX_DEVICE_PC,              // PC
    ADX_DEVICE_MOBILE,          // 移动手机
    ADX_DEVICE_PAD,             // 平板电脑

    ADX_OS_UNKNOWN,             // 未知
    ADX_OS_IOS,                 // iOS
    ADX_OS_ANDROID,             // Android
    ADX_OS_WINDOWS,             // Windows

    ADX_CARRIER_UNKNOWN,        // 未知
    ADX_CARRIER_CHINAMOBILE,    // 中国移动
    ADX_CARRIER_CHINAUNICOM,    // 中国联通
    ADX_CARRIER_CHINATELECOM,   // 中国电信

    ADX_CONNECTION_2G,          // 2G
    ADX_CONNECTION_3G,          // 3G
    ADX_CONNECTION_4G,          // 4G
    ADX_CONNECTION_WIFI,        // WIFI
};

using namespace std;

// 01234567890123456789012
// [11:29:48.889 f700,DBG]
int buffer_parse(char *str) 
{
    if (!str || !(*str))
        return -1;

    int len = strlen(str);
    if (len <= 23) return -1;

    if (str[0] == '['
            && str[19] == 'D'
            && str[20] == 'B'
            && str[21] == 'G'
            && str[22] == ']')
        return 0;
    return -1;
}

int request_parse(adx_pool_t *pool, char *buffer, int size)
{
    adx_json_t *root = ADX_JSON_ROOT(pool);
    adx_json_t *DEV = adx_json_add(pool, root, ADX_JSON_OBJECT, "dev", NULL);
    adx_json_t *APP = adx_json_add(pool, root, ADX_JSON_OBJECT, "app", NULL);
    adx_json_t *GEO = adx_json_add(pool, root, ADX_JSON_OBJECT, "geo", NULL);
    adx_json_t *IMPS = adx_json_add(pool, root, ADX_JSON_ARRAY, "imps", NULL);
    cout << "=================================================" << endl;

    gdt::adx::BidRequest adx_request;
    if (!adx_request.ParseFromArray(buffer, size))
        return -1;

    // bid
    const char *bid = adx_request.id().c_str();
    cout << "bid = " << bid << endl;
    adx_json_add_string(pool, root, "bid", bid);

    // 广告位LIST
    for (long i = 0; i < adx_request.impressions_size(); i++) {
        gdt::adx::BidRequest::Impression impression = adx_request.impressions(i);

        cout << "impression -> id = " << impression.id() << endl; // string
        cout << "impression -> placement_id = " << impression.placement_id() << endl; // long

        // 底价
        long bid_floor = impression.bid_floor();
        cout << "impression -> bid_floor = " << bid_floor << endl;

        // 广告位
        for (long i = 0; i < impression.creative_specs_size(); i++) {

            long creative_specs = impression.creative_specs(i);
            cout << "impression -> creative_specs = " << creative_specs << endl;

            adx_json_t *obj = adx_json_add(pool, IMPS, ADX_JSON_OBJECT, NULL, NULL);
            adx_json_add_number(pool, obj,  "imp", creative_specs);

            if (bid_floor)
                adx_json_add_number(pool, obj,  "bid_floor", bid_floor);

            // 合同号
            adx_json_t *deal_array = NULL;
            if (impression.deal_ids_size() > 0) 
                deal_array = adx_json_add(pool, obj, ADX_JSON_ARRAY, "deal_id", NULL);

            for (long i = 0; i < impression.deal_ids_size(); i++) {
                const char *deal_id = impression.deal_ids(i).c_str();
                cout << "impression -> deal_id = " << deal_id << endl;
                adx_json_add_string(pool, deal_array, NULL, deal_id);
            }
        }

        // 广告位过滤的行业 ID 
        for (long i = 0; i < impression.blocking_industry_id_size(); i++) {
            cout << "impression -> blocking_industry_id = " << impression.blocking_industry_id(i) << endl; // long
        }

        // 原生广告
        for (long i = 0; i < impression.natives_size(); i++) {
            long required_fields = impression.natives(i).required_fields(); // long
            cout << "impression -> natives -> required_fields = " << required_fields << endl;

            long type = impression.natives(i).type(); // long
            cout << "impression -> natives -> type = " << type << endl;
        }

        // 废弃
        for (long i = 0; i < impression.contract_code_size(); i++) {
            cout << "impression -> contract_code = " << impression.contract_code(i) << endl; // string
        }

        // 广告位支持的素材类型: gif", "jpeg", "png", "swf", "psd", "bmp", "tiff", 
        // "tiff", "jpc", "jpx", "jb2", "swc", "iff", "wbmp", "xbm", "webp", "flv", "wav", "mp3", "mp4", "avi"
        // 字段为空时，格式 不做限制。
        for (long i = 0; i < impression.multimedia_type_white_list_size(); i++) {
            cout << "impression -> multimedia_type_white_list = " << impression.multimedia_type_white_list(i) << endl; // string
        }

        // 广告位过滤的关键字
        for (long i = 0; i < impression.blocking_keyword_size(); i++) {
            cout << "impression -> blocking_keyword = " << impression.blocking_keyword(i) << endl; // string
        }

        // 广告位的广告主白名单。 当 advertiser_whitelist 非 空时，该广告位只接受白 名单广告主的广告
        for (long i = 0; i < impression.advertiser_whitelist_size(); i++) {
            cout << "impression -> advertiser_whitelist = " << impression.advertiser_whitelist(i) << endl; // string
        }

        // 广告位过滤的广告主
        for (long i = 0; i < impression.advertiser_blacklist_size(); i++) {
            cout << "impression -> advertiser_blacklist = " << impression.advertiser_blacklist(i) << endl; // string
        }

        // 请求命中的 Pretargeting 配置包 ID
        for (long i = 0; i < impression.pretargeting_ids_size(); i++) {
            cout << "impression -> pretargeting_ids = " << impression.pretargeting_ids(i) << endl; // string
        }
    }

    // Device
    gdt::adx::BidRequest::Device device = adx_request.device(); // obj

    // 设备ID
    const char *device_id = device.id().c_str();
    cout << "device -> id = " << device_id << endl;
    adx_json_add_string(pool, DEV, "device_id", device_id);

    // iOS 设备 IDFA
    const char *idfa = device.idfa().c_str();
    cout << "device -> idfa = " << idfa << endl;
    // adx_json_add_string(pool, DEV, "idfa", idfa);

    // Android ID
    const char *android_id = device.android_id().c_str();
    cout << "device -> android_id = " << android_id << endl;
    // adx_json_add_string(pool, DEV, "android_id", android_id);

    // 设备ID 类型
    int device_id_type = 1;
    if (*idfa) device_id_type = 2;
    adx_json_add_number(pool, DEV, "device_id_type", device_id_type);


    // 设备类型
    long device_type = device.device_type();
    cout << "device -> device_type = " << device_type << endl;
    adx_json_add_number(pool, DEV,  "device_type", device_type);

    // 操作系统
    // gdt::adx::BidRequest::OS_ANDROID
    // gdt::adx::BidRequest::OS_IOS
    // gdt::adx::BidRequest::OS_WINDOWS
    long os = device.os();
    cout << "device -> os = " << os << endl;
    adx_json_add_number(pool, DEV,  "os", os);

    // 操作系统版本
    const char *os_version = device.os_version().c_str();
    cout << "device -> os_version = " << os_version << endl;
    adx_json_add_string(pool, DEV, "os_version", os_version);

    // UA
    const char *user_agent = device.user_agent().c_str();
    cout << "device -> user_agent = " << user_agent << endl;
    adx_json_add_string(pool, DEV, "user_agent", user_agent);

    // 设备屏幕大小
    cout << "device -> screen_width = " << device.screen_width() << endl; // long
    cout << "device -> screen_height = " << device.screen_height() << endl; // long

    // 屏幕每英寸像素
    cout << "device -> dpi = " << device.dpi() << endl; // long

    // 运营商
    // gdt::adx::BidRequest::CARRIER_CHINAMOBILE
    // gdt::adx::BidRequest::CARRIER_CHINATELECOM
    // gdt::adx::BidRequest::CARRIER_CHINAUNICOM
    long carrier = device.carrier();
    cout << "device -> carrier = " << carrier << endl;
    adx_json_add_number(pool, DEV,  "carrier", carrier);

    // 网络类型
    // gdt::adx::BidRequest::CONNECTIONTYPE_2G
    // gdt::adx::BidRequest::CONNECTIONTYPE_3G
    // gdt::adx::BidRequest::CONNECTIONTYPE_4G
    // gdt::adx::BidRequest::CONNECTIONTYPE_WIFI
    long connection_type = device.connection_type();
    cout << "device -> connection_type = " << connection_type << endl;
    adx_json_add_number(pool, DEV,  "connection_type", connection_type);

    // 设备品牌型号
    const char *brand_and_model = device.brand_and_model().c_str();
    cout << "device -> brand_and_model = " << brand_and_model << endl;
    adx_json_add_string(pool, DEV, "brand_and_model", brand_and_model);

    // 设备语言
    cout << "device -> language = " << device.language() << endl; // string

    // 设备制造商
    const char *manufacturer = device.manufacturer().c_str();
    cout << "device -> manufacturer = " << manufacturer << endl;
    adx_json_add_string(pool, DEV, "manufacturer", manufacturer);

    // IP
    const char *ip = adx_request.ip().c_str();
    cout << "ip = " << ip << endl;
    adx_json_add_string(pool, root, "ip", ip);

    // 部分流量可能没有IP 此时用地域码做定向
    cout << "area_code = " << adx_request.area_code() << endl; // long

    // User
    gdt::adx::BidRequest::User user = adx_request.user();

    // Cookie Mapping ID
    cout << "user -> id = " << user.id() << endl; // string

    // 用户ID 命中的 DMP 人群包 ID
    for (long i = 0; i < user.audience_ids_size(); i++) {
        cout << "user -> audience_ids = " << user.audience_ids(i) << endl; // long
    }

    // 地理位置
    gdt::adx::BidRequest::Geo geo = adx_request.geo();

    // 纬度 * 1 000 000 (火星坐标系)
    long long latitude = geo.latitude();
    cout << "geo -> latitude = " << latitude << endl;
    adx_json_add_number(pool, GEO,  "latitude", latitude);

    // 经度 * 1 000 000 (火星坐标系)
    long long longitude = geo.longitude();
    cout << "geo -> longitude = " << longitude << endl;
    adx_json_add_number(pool, GEO,  "longitude", longitude);


    // 经纬度精度半径(单位:米)
    double accuracy = geo.accuracy();
    cout << "geo -> accuracy = " << accuracy << endl;
    adx_json_add_double(pool, GEO, "accuracy", accuracy);

    // APP 信息
    const gdt::adx::BidRequest::App app = adx_request.app();

    // App 的唯一标识。Android 应用是 packagename，iOS 应用是 bundle id
    const char *app_bundle_id = app.app_bundle_id().c_str();
    cout << "app -> app_bundle_id = " << app_bundle_id << endl;
    adx_json_add_string(pool, APP, "app_id", app_bundle_id);

    // App 所属行业 id
    long industry_id = app.industry_id();
    cout << "app -> industry_id = " << industry_id << endl;
    adx_json_add_number(pool, APP,  "category_id", industry_id);

    // 广告位是否支持 deep link，默认为不支持
    long support_deep_link = adx_request.support_deep_link();
    cout << "support_deep_link = " << support_deep_link << endl;
    adx_json_add_number(pool, root,  "support_deep_link", support_deep_link);

    // adx_json_display(root);
    cout << adx_json_to_buffer(pool, root) << endl; exit(0);
    return 0;
}


int main(int argc, char *argv[])
{
    char *buffer = (char *)malloc(40000000);
    FILE *fp = fopen("./gdt.log", "r");
    int i, size = fread(buffer, 1, 40000000, fp);

    adx_pool_t *pool = adx_pool_create();

    int index = 0;
    char temp[4096];
    for (i = 0; i < size; i++) {

        char *str = &buffer[i];
        if (buffer_parse(str) == 0) {

            if (index > 0) request_parse(pool, temp, index - 1);
            index = 0;
            i += 22;

        } else {
            temp[index++] = *str;
        }
    }

    return 0;
}




