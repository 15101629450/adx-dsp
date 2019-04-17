
#ifndef __ADX_URI_H__
#define __ADX_URI_H__

#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define ADX_URI_TYPE_ROOT	0x01
#define ADX_URI_TYPE_LIST	0x02
#define ADX_URI_TYPE_HTML	0x03

typedef struct {

    char *domain;	// 001.xxx.com/002 => 001.xxx.com
    char *domain_root;	// 001.xxx.com/002 => xxx.com
    char *domain_s;	// 001.xxx.com/002 => 001
    char *domain_e;	// 001.xxx.com/002 => 002

    int uri_type;	// 1:root 2:list 3:html
    int mobile_type;	// 0:pc 1:mobile
    int search_engine;	// 0:null 1:search engine
    int search_engine_type;

    char *user_agent;
    char *os;

} adx_uri_t;

adx_uri_t *adx_uri_parse(ngx_http_request_t *r);

#endif


