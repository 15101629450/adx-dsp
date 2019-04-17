
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include "baidu.pb.h"
#include "adx_curl.h"

using namespace std;

int baidu_request_parse(char *buffer, int size)
{
    // return post_send("http://www.aaa.com/a", buffer, size);
    // return post_send("http://www.aaa.com/b", buffer, size);
    // return post_send("http://www.aaa.com/c", buffer, size);
    return post_send("http://www.aaa.com/d", buffer, size);

    BidRequest bidrequest;
    if (!bidrequest.ParseFromArray(buffer, size))
        return -1;

    // bidrequest.set_id("abc");
    // BidRequest_AdSlot *adslot = bidrequest.mutable_adslot(0);
    // adslot->set_width(300);
    // adslot->set_height(250);

    // bid
    const char *bid = bidrequest.id().c_str();
    fprintf(stdout, "bid = %s\n", bid);

    for (int i = 0; i < bidrequest.adslot_size(); i++) {
        BidRequest_AdSlot adslot = bidrequest.adslot(i);

        // 尺寸
        int w = adslot.width();
        int h = adslot.height();
        fprintf(stdout, "adx_slot[%d] -> w = %d\n", i, w); 
        fprintf(stdout, "adx_slot[%d] -> h = %d\n", i, h); 

        // 底价
        int bid_floor = adslot.minimum_cpm();
        fprintf(stdout, "adx_slot[%d] -> bid_floor = %d\n", i, bid_floor);

        // 展示位置
        int adx_place = adslot.slot_visibility();
        fprintf(stdout, "adx_slot[%d] -> adx_place = %d\n", i, adx_place);

        // 展示类型
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
        int adx_slot_type = adslot.adslot_type();
        fprintf(stdout, "adx_slot[%d] -> adx_slot_type = %d\n", i, adx_slot_type);

        // 0 文本
        // 1 图片
        // 2 FLASH
        // 4 图文
        // 5 链接单元
        // 7 Video
        for (int j = 0; j < adslot.creative_type_size(); j++) {
            int creative_type = adslot.creative_type(j);
            fprintf(stdout, "adx_slot[%d] -> creative_type = %d\n", i, creative_type);
        }
    }

    BidRequest_Mobile mobile = bidrequest.mobile();

    // UNKNOWN = 0; // 未知
    // IMEI = 1; // IMEI
    // MAC = 2; // MAC地址
    for (long i = 0; i < mobile.id_size(); i++) {
        const char *device_id = mobile.id(i).id().c_str();
        int device_id_type = mobile.id(i).type();
        fprintf(stdout, "device -> id -> id = %s\n", device_id);
        fprintf(stdout, "device -> id -> type = %d\n", device_id_type);
    }

    // UNKNOWN = 0;    // 未知
    // ANDROID_ID = 4; // Android适用
    // IDFA = 5;       // IOS适用
    for (long i = 0; i < mobile.for_advertising_id_size(); i++) {
        const char *device_id = mobile.for_advertising_id(i).id().c_str();
        int device_id_type = mobile.for_advertising_id(i).type();
        fprintf(stdout, "device -> id2 -> id = %s\n", device_id);
        fprintf(stdout, "device -> id2 -> type = %d\n", device_id_type);
    }

    // 设备类型
    // BidRequest_Mobile::HIGHEND_PHONE
    // UNKNOWN_DEVICE = 0;
    // HIGHEND_PHONE = 1;
    // TABLET = 2;
    int device_type = mobile.device_type();
    fprintf(stdout, "device -> device_type = %d\n", device_type);

    // 操作系统
    // BidRequest_Mobile::UNKNOWN_OS
    // UNKNOWN_OS = 0;
    // IOS = 1;
    // ANDROID = 2;
    // WINDOWS_PHONE = 3;
    int os = mobile.platform();
    fprintf(stdout, "device -> os = %d\n", os);

    // 操作系统版本
    int os_version_major = mobile.os_version().os_version_major();
    int os_version_minor = mobile.os_version().os_version_minor();
    int os_version_micro = mobile.os_version().os_version_micro();

    char os_version[128];
    sprintf(os_version, "%d.%d.%d", os_version_major, os_version_minor, os_version_micro);
    fprintf(stdout, "device -> os_version = %s\n", os_version);

    // 运营商编号（MCC+MNC编号）
    // 例如中国移动 46000
    // 前三位是Mobile Country Code
    // 后两位是Mobile Network Code
    int carrier = mobile.carrier_id();
    fprintf(stdout, "device -> carrier = %d\n", carrier);

    // 网络类型
    // BidRequest_Mobile::WIFI
    // UNKNOWN_NETWORK = 0;
    // WIFI = 1;
    // MOBILE_2G = 2;
    // MOBILE_3G = 3;
    // MOBILE_4G = 4;
    int connection_type = mobile.wireless_network_type();
    fprintf(stdout, "device -> connection_type = %d\n", connection_type);

    // 设备机型
    const char *model = mobile.model().c_str();
    fprintf(stdout, "device -> model = %s\n", model);

    // 设备品牌
    const char *brand = mobile.brand().c_str();
    fprintf(stdout, "device -> brand = %s\n", brand);

    // 不允许的广告行业
    for (long i = 0; i < bidrequest.excluded_product_category_size(); i++) {
        // int category = bidrequest.excluded_product_category(i);
    }

    // 商店下载地址
    // const char *store_url = bidrequest.url().c_str();

    // UA
    const char *user_agent = bidrequest.user_agent().c_str();
    fprintf(stdout, "device -> user_agent = %s\n", user_agent);

    // ip
    const char *ip = bidrequest.ip().c_str();
    fprintf(stdout, "device -> ip = %s\n", ip);

#if 1
    // 地理位置(百度坐标系)
    // float latitude = bidrequest.user_geo_info().user_coordinate(0).latitude();
    // float longitude = bidrequest.user_geo_info().user_coordinate(0).longitude();
#else
    BidRequest_Geo geo = bidrequest.user_geo_info();
    if (geo.user_coordinate_size() > 0) {
        float latitude = geo.user_coordinate(0).latitude();
        float longitude = geo.user_coordinate(0).longitude();
        cout << "device -> geo -> latitude = " << latitude << endl;
        cout << "device -> geo -> longitude = " << longitude << endl;
    }
#endif

    // 百度定义的app id
    // const char *app_baidu_id = mobile.mobile_app().app_id().c_str();
    // cout << "app -> app_baidu_id = " << app_baidu_id << endl;

    // 如果来自苹果商店，则直接是app-store id
    // 如果来自Android设备，则是package的全名 
    const char *app_id = mobile.mobile_app().app_bundle_id().c_str();
    fprintf(stdout, "app -> app_id = %s\n", app_id);

    // App应用分类
    int app_category = mobile.mobile_app().app_category();
    fprintf(stdout, "app -> app_category = %d\n", app_category);

#if 0
    int len = bidrequest.ByteSize();
    void *buf = malloc(len);
    bidrequest.SerializeToArray(buf, len);

    int fd = open("log", O_WRONLY);
    write(fd, buf, len);
    close(fd);
#endif
    return 0;
}








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

void demo1()
{
    char *buffer = (char *)malloc(200000000);
    FILE *fp = fopen("./baidu.log", "r");
    int size = fread(buffer, 1, 200000000, fp);

    int index = 0;
    char temp[4096];
    for (int i = 0; i < size; i++) {

        char *str = &buffer[i];
        if (buffer_parse(str) == 0) {

            if (index > 0) baidu_request_parse(temp, index - 1);
            index = 0;
            i += 22;

        } else {
            temp[index++] = *str;
        }
    }

    // cout << "============================================" << endl;
}

void demo2()
{
    FILE *fp = fopen("baidu.mess2", "rb");

    char buffer[4096];
    int size = fread(buffer, 1, 4096, fp);

    //    for(;;)
    baidu_request_parse(buffer, size);
}

int main(int argc, char *argv[])
{
    // demo1();
    demo2();
    return 0;
}


