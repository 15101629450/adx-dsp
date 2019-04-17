
#ifndef __ADX_DSP_H__
#define __ADX_DSP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <fcgiapp.h>

#include "adx_type.h"
#include "adx_alloc.h"
#include "adx_cache.h"
#include "adx_dump.h"
#include "adx_conf_file.h"
#include "adx_region.h"
#include "adx_redis.h"
#include "adx_json.h"
#include "adx_log.h"
#include "adx_util.h"
#include "adx_com.pb.h"

#define DSP_CONF_PATH "/etc/adx_dsp.conf"
#define DSP_REGION_DB_PATH "/etc/adx_region.db"
#define ADX_POST_SIZE 1048576
#define ADX_KEY_SIZE 128

enum {
    ADX_DEVICEID_UNKNOW,            // 未知
    ADX_DEVICEID_IMEI,              // IMEI 明文
    ADX_DEVICEID_IMEI_SHA1,         // IMEI sha1
    ADX_DEVICEID_IMEI_MD5,          // IMEI md5
    ADX_DEVICEID_ANDROID_ID,        // ANDROID_ID
    ADX_DEVICEID_ANDROID_ID_SHA1,   // ANDROID_ID sha
    ADX_DEVICEID_ANDROID_ID_MD5,    // ANDROID_ID md5
    ADX_DEVICEID_IDFA,              // idfa 明文
    ADX_DEVICEID_IDFA_SHA1,         // idfa sha1
    ADX_DEVICEID_IDFA_MD5,          // idfa md5
    ADX_DEVICEID_MAC,               // mac 明文
    ADX_DEVICEID_MAC_SHA1,          // mac sha1
    ADX_DEVICEID_MAC_MD5,           // mac md5
};

enum {
    ADX_DEVICE_UNKNOWN,             // 未知
    ADX_DEVICE_MOBILE,              // 手机
    ADX_DEVICE_PAD,                 // 平板
    ADX_DEVICE_PC,                  // PC
};

enum {
    ADX_OS_UNKNOWN,                 // 未知
    ADX_OS_IOS,                     // iOS
    ADX_OS_ANDROID,                 // Android
    ADX_OS_WINDOWS,                 // Windows
};

enum {
    ADX_CARRIER_UNKNOWN,            // 未知
    ADX_CARRIER_CHINA_MOBILE,        // 中国移动
    ADX_CARRIER_CHINA_UNICOM,        // 中国联通
    ADX_CARRIER_CHINA_TELECOM,       // 中国电信
};

enum {
    ADX_CONNECTION_UNKNOWN,         // 未知
    ADX_CONNECTION_WIFI,            // WIFI
    ADX_CONNECTION_CELLULAR,        // 蜂窝网络
    ADX_CONNECTION_2G,              // 2G
    ADX_CONNECTION_3G,              // 3G
    ADX_CONNECTION_4G,              // 4G
    ADX_CONNECTION_5G,              // 5G
};

enum {
    ADX_IMP_UNKNOWN,
    ADX_IMP_BANNER,                     // BANNER广告
    ADX_IMP_NATIVE,                     // 原生广告
    ADX_IMP_VIDEO,                      // 视频广告
};

typedef struct {

    adx_list_t next;
    char *id; // 设备ID
    int type; // 设备ID类型

} adx_com_device_id;

// 设备
typedef struct {

    adx_list_t device_id; // 设备ID adx_com_device_id
    int device_type; // 设备类型
    int os; // 操作系统
    char *os_version; // 操作系统版本
    int carrier; // 网络运营商(移动/电信/联通)
    int connection; // 设备联网方式 (2G/3G/4G/5G/wifi)
    char *brand; // 设备制造商信息
    char *model; // 设备品牌型号
    char *user_agent; // user agent
    char *ip; // 用户IP地址
    int ip_code; // ip -> region code

} adx_com_device;

// app
typedef struct {

    char *app_id; // app id
    char *app_name; // app 名字
    int app_category; // app应用分类

} adx_com_app;

// 地理位置
typedef struct {

    int latitude; // 纬度 * 1 000 000 (火星坐标系)
    int longitude; // 经度 * 1 000 000 (火星坐标系)
    double accuracy; // 经纬度精度半径(单位:米)

} adx_com_geo;

// banner
typedef struct {

    int width;
    int height;
    char key[ADX_KEY_SIZE];

} adx_com_banner;

// native
typedef struct {

    int width;
    int height;
    char key[ADX_KEY_SIZE];
    struct adx_com_icon {
        int width;
        int height;
        char key[ADX_KEY_SIZE];
    } icon;

} adx_com_native;

// video
typedef struct {

    int width;
    int height;
    char key[ADX_KEY_SIZE];

    int min_duration;
    int max_duration;

} adx_com_video;

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

} adx_com_slot;

typedef struct {

    size_t IPB;
    adx_list_t *conf;
    adx_redis_conn *redis_conn;

    adx_pool_t *pool;
    FCGX_Request fcgi_app;
    char body_buffer[ADX_POST_SIZE];
    int content_length;

    adx_cache_t *map_list;
    adx_cache_t *creative_list;
    adx_cache_t *policy_list;
    adx_cache_t *template_list;

    int adx;
    char *bid;
    adx_list_t adx_slot_list; // adx_com_slot
    adx_com_device device;
    adx_com_app app;
    adx_com_geo geo;

} adx_request_t;

int adx_request(adx_request_t *r);
int adx_select(adx_request_t *r);
int adx_response(adx_request_t *r);
int adx_response_empty(adx_request_t *r);

#endif



