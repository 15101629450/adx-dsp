
#ifndef __ADX_CURL_H__
#define __ADX_CURL_H__

#ifdef __cplusplus
extern "C" { 
#endif

    int adx_curl_open(const char *url);
    int adx_curl_post(char *url, char *buf);

#ifdef __cplusplus
}
#endif

#endif


