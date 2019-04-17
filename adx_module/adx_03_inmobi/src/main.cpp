
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "adx_json.h"

#define IS_NULL(o,n) (o?o:n)

int post_recv(void *buf, size_t size, size_t nmemb, void *stream)
{
    return size * nmemb;
}

int post_send(const char *url, const char *buf)
{
    CURL *curl = curl_easy_init();
    if(curl == NULL) return -1;

    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "x-openrtb-version: 2.3.1");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, post_recv);
    // curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

    CURLcode ret = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return ret;
}

int adx_com_parse_banner(adx_pool_t *pool, adx_json_t *banner, adx_json_t *new_imp)
{
    adx_json_add_number(pool, new_imp, "type", 1);

    int w = adx_json_find_value_number(banner, "w");
    int h = adx_json_find_value_number(banner, "h");
    if (!w || !h) return -1;

    char val[128];
    sprintf(val, "%dX%d", w, h);
    adx_json_add_string(pool, new_imp, "img", val);

#if 0
    adx_json_t *new_mimes = adx_json_add_array(pool, new_imp, "mimes");
    adx_json_t *mimes = adx_json_find(banner, "mimes");
    if (adx_json_array_check(mimes) != 0) return -1;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &mimes->child) {
        adx_json_t *node = adx_list_entry(p, adx_json_t, next);
        if (node->type == ADX_JSON_STRING)
            adx_json_add_string(pool, new_mimes, NULL, node->val);
    }
#endif
    return 0;
}

/*
   Native->Asset

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

int adx_com_parse_native(adx_pool_t *pool, adx_json_t *native, adx_json_t *new_imp)
{
    adx_json_add_number(pool, new_imp, "type", 2);
    adx_json_t *requestobj = adx_json_find(native, "requestobj");
    // int layout = adx_json_find_value_number(requestobj, "layout");
    // fprintf(stdout, "layout=%d\n", layout);

    adx_json_t *assets = adx_json_find(requestobj, "assets");
    if (adx_json_array_check(assets) != 0) return -1;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &assets->child) {
        adx_json_t *node = adx_list_entry(p, adx_json_t, next);
        adx_json_t *img = adx_json_find(node, "img");
        int type = adx_json_find_value_number(img, "type");
        if (type == 1) { // icon

            char val[128];
            int w = adx_json_find_value_number(img, "wmin");
            int h = adx_json_find_value_number(img, "hmin");
            sprintf(val, "%dX%d", w, h);
            adx_json_add_string(pool, new_imp, "icon", val);

        } else if (type == 3) { // image

            char val[128];
            int w = adx_json_find_value_number(img, "wmin");
            int h = adx_json_find_value_number(img, "hmin");
            sprintf(val, "%dX%d", w, h);
            adx_json_add_string(pool, new_imp, "img", val);
        }
    }

    // adx_json_display(assets);
    return 0;
}

void adx_com_parse_app(adx_pool_t *pool, adx_json_t *app, adx_json_t *new_app)
{
    char *id = adx_json_find_value_string(app, "id");
    if (id) adx_json_add_string(pool, new_app, "id", id);

    char *name = adx_json_find_value_string(app, "name");
    if (name) adx_json_add_string(pool, new_app, "name", name);
}

void adx_com_parse_device(adx_pool_t *pool, adx_json_t *device, adx_json_t *new_device)
{
    char *ua = adx_json_find_value_string(device, "ua");
    if (ua) adx_json_add_string(pool, new_device, "ua", ua);

    /*
       "dnt": 0,
       "lmt": 0,
       "ip": "14.18.29.121",
       "devicetype": 4,
       "make": "Apple",
       "model": "iPhone (undetected)",
       "os": "iOS",
       "osv": "10.3",
       "pxratio": 3.0,
       "language": "zh",
       "connectiontype": 2,
       "ifa": "13F1659C-27F3-4868-8C1B-94530E9070ED",
       */

#if 0
0x00 : return "unknow_unknow";
0x10 : return "imei_Clear";
0x11 : return "imei_SHA1";
0x12 : return "imei_md5";
0x20 : return "mac_Clear";
0x21 : return "mac_SHA1";
0x22 : return "mac_md5";
0x60 : return "androidid_Clear";
0x61 : return "androidid_SHA1";
0x62 : return "androidid_md5";
0x70 : return "idfa_Clear";
0x71 : return "idfa_SHA1";
0x72 : return "idfa_md5";
0xFF : return "other_other";
#endif

       /* ifa: ID sanctioned for advertiser use in the clear (i.e., not hashed).
        * didsha1: Hardware device ID (only IMEI); hashed via SHA1.
        * didmd5: Hardware device ID (only IMEI); hashed via MD5
        * dpidsha1: Platform device ID (e.g., Android ID); hashed via SHA1
        * dpidmd5: Platform device ID (e.g., Android ID); hashed via MD5
        * ext:
        * idfa: IDFA of the iOS device.
        * idfasha1: Sha1 hashed privacy aware unique identifier on iOS6 and above. Replacement for UDID.
        * idfamd5: MD5 hashed privacy aware unique identifier on iOS6 and above.Replacement for UDID.
        * gpid: If the device is android, the advertising id is stored here. */

       char *ifa = adx_json_find_value_string(device, "ifa");
       char *didsha1 = adx_json_find_value_string(device, "didsha1");
       char *didmd5 = adx_json_find_value_string(device, "didmd5");
       char *dpidsha1 = adx_json_find_value_string(device, "dpidsha1");
       char *dpidmd5 = adx_json_find_value_string(device, "dpidmd5");

       adx_json_t *ext = adx_json_find(device, "ext");
       char *idfa = adx_json_find_value_string(ext, "idfa");
       char *idfasha1 = adx_json_find_value_string(ext, "idfasha1");
       char *idfamd5 = adx_json_find_value_string(ext, "idfamd5");
       char *android = adx_json_find_value_string(ext, "gpid");


       if (!ifa && idfa)
           fprintf(stdout, "[%s][%s]\n", ifa, idfa);

       return;

       // fprintf(stdout, "%s\n", ifa);
       // fprintf(stdout, "%s\n", didsha1);
       // fprintf(stdout, "%s\n", didmd5);
       // fprintf(stdout, "%s\n", dpidsha1);
       // fprintf(stdout, "%s\n", dpidmd5);

       // fprintf(stdout, "%s\n", idfa);
       // fprintf(stdout, "%s\n", idfasha1);
       // fprintf(stdout, "%s\n", idfamd5);
       // if (android) fprintf(stdout, "%s\n", android);

       // if (idfa || idfasha1 || idfamd5 || android)
       //        if (!idfa && !idfasha1 && !idfamd5 && !android)

       // if (ifa || didsha1 || didmd5 || dpidsha1 || dpidmd5)
       if (!ifa && !didsha1 && !didmd5 && !dpidsha1 && !dpidmd5)

           fprintf(stdout, "[%s][%s][%s][%s][%s]ext[%s][%s][%s][%s]\n",
                   IS_NULL(ifa,         "1"),
                   IS_NULL(didsha1,     "2"),
                   IS_NULL(didmd5,      "3"),
                   IS_NULL(dpidsha1,    "4"),
                   IS_NULL(dpidmd5,     "5"),
                   IS_NULL(idfa,        "1"),
                   IS_NULL(idfasha1,    "2"),
                   IS_NULL(idfamd5,     "3"),
                   IS_NULL(android,        "4")
                  );
}

int adx_com_parse(adx_pool_t *pool, char *buffer)
{
    adx_json_t *json = adx_json_parse(pool, buffer);
    adx_json_t *new_json = ADX_JSON_ROOT(pool);
    adx_json_t *new_imps = adx_json_add_array(pool, new_json, "imps");
    adx_json_t *new_imp = adx_json_add_object(pool, new_imps, NULL);

    char *bid = adx_json_find_value_string(json, "id");
    if (!bid) return -1;
    adx_json_add_string(pool, new_imp, "bid", bid);

    adx_json_t *imps = adx_json_find(json, "imp");
    if (adx_json_array_check(imps) != 0) return -1;

    adx_json_t *imp = adx_json_find_array(imps, 0);
    if (!imp) return -1;

    adx_json_t *banner = adx_json_find(imp, "banner");
    adx_json_t *native = adx_json_find(imp, "native");

    // fprintf(stdout, "banner=%p\n", banner);
    // fprintf(stdout, "native=%p\n", native);

    if (banner) {
        if (adx_com_parse_banner(pool, banner, new_imp)) return -1;

    } else if (native) {
        if (adx_com_parse_native(pool, native, new_imp)) return -1;

    } else {
        return -1;
    }

    adx_json_t *new_app = adx_json_add_object(pool, new_json, "app");
    adx_com_parse_app(pool, adx_json_find(json, "app"), new_app);

    adx_json_t *new_device = adx_json_add_object(pool, new_json, "device");
    adx_com_parse_device(pool, adx_json_find(json, "device"), new_device);

    // adx_json_display(new_json);
    // fprintf(stdout, "================\n");
    // fprintf(stdout, "%s\n", adx_json_to_buffer(pool, new_json));
    return 0;
}

int main(int argc, char *argv[])
{
    // int count = 0;
    char buffer[40960] = {0};
    FILE *fp = fopen("./req", "r");
    // FILE *fp = fopen("./inmobi.log", "r");
    while(fgets(buffer, 40960, fp)) {

        // adx_pool_t *pool = adx_pool_create();
        // adx_com_parse(pool, buffer);
        // fprintf(stdout, buffer);

        // fprintf(stdout, "count=%d\n", ++count);
        post_send("http://127.0.0.1/demo", buffer);
        fprintf(stdout, "\r\n");
        return 0;
    }

    return 0;
}



