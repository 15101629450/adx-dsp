

#if 0

#include "lg_string.h"
#include "lg_ngx_session.h"
#include "lg_ngx_network.h"
#include "adx_uri.h"

#define SEARCH_ENGINE_BAIDU		29// 百度
#define SEARCH_ENGINE_GOOGLE		30// 谷歌
#define SEARCH_ENGINE_SOSO		31
#define SEARCH_ENGINE_360		32// 360
#define SEARCH_ENGINE_YAHOO		33
#define SEARCH_ENGINE_YOUDAO		34
#define SEARCH_ENGINE_SOGOU		35// 搜狗
#define SEARCH_ENGINE_GOUGOU		36
#define SEARCH_ENGINE_YISOU		37// 神马
#define SEARCH_ENGINE_HAOSOU		38// 好搜
#define SEARCH_ENGINE_BINGBOT		39// 必应
#define SEARCH_ENGINE_SEMRUSHBOT	40


void adx_uri_parse_domain(ngx_http_request_t *r, adx_uri_t *uri)
{
    ngx_http_headers_in_t *headers_in = &r->headers_in;
    if (!headers_in || !headers_in->server.data || !headers_in->server.len)
        return;

    uri->domain = (char *)headers_in->server.data;
    uri->domain_root = lg_string_url_to_domain(uri->domain);
}

void adx_uri_parse_domain_s(ngx_http_request_t *r, adx_uri_t *uri)
{
    if (!uri->domain || !uri->domain_root)
        return;

    if (strcmp(uri->domain, uri->domain_root) == 0)
        return;

    int domain_s_len = uri->domain_root - uri->domain - 1;
    if (domain_s_len <= 0) return;

    uri->domain_s = ngx_palloc(r->pool, domain_s_len + 1);
    if (!uri->domain_s) return;

    memcpy(uri->domain_s, uri->domain, domain_s_len);
    uri->domain_s[domain_s_len] = 0;
}

void adx_uri_parse_domain_e(ngx_http_request_t *r, adx_uri_t *uri)
{
    char *s = (char *)r->uri_start;
    char *e = (char *)r->uri_end;

    if ((s == e) || (*s == '/' && (s + 1) == e)) { 
        uri->uri_type = ADX_URI_TYPE_ROOT;
        uri->domain_e = "/";
        return;
    }

    if (strstr(s, ".html")) uri->uri_type = ADX_URI_TYPE_HTML;
    else uri->uri_type = ADX_URI_TYPE_LIST;

    uri->domain_e = (char *)s;
    if (*uri->domain_e == '/') uri->domain_e = s + 1;
    for (; s <= e; s++) {
        if (!(*s == '/' || isalnum(*s))) { // 字母 or 数字 or /
            *s = 0;
            return;
        }
    }
}

void adx_uri_parse_user_agent(ngx_http_request_t *r, adx_uri_t *uri)
{
    char *user_agent = lg_ngx_network_get_user_agent(r);
    if (user_agent) {

        uri->os = "pc";
        uri->user_agent = user_agent;

        if (lg_string_strstr(user_agent, "ipad") || lg_string_strstr(user_agent, "iphone")) {

            uri->os = "ios";
            uri->mobile_type = 1;
        }

        if (lg_string_strstr(user_agent, "android")
                || lg_string_strstr(user_agent, "windows ce")
                || lg_string_strstr(user_agent, "windows mobile")
           ) {

            uri->os = "android";
            uri->mobile_type = 1;
        }

        uri->search_engine_type = 0;
        if (lg_string_strstr(user_agent, "baidu"))uri->search_engine_type = 			SEARCH_ENGINE_BAIDU;
        else if (lg_string_strstr(user_agent, "google"))uri->search_engine_type =               SEARCH_ENGINE_GOOGLE;
        else if (lg_string_strstr(user_agent, "soso"))uri->search_engine_type =                 SEARCH_ENGINE_SOSO;
        else if (lg_string_strstr(user_agent, "360"))uri->search_engine_type =                  SEARCH_ENGINE_360;
        else if (lg_string_strstr(user_agent, "yahoo"))uri->search_engine_type =                SEARCH_ENGINE_YAHOO;
        else if (lg_string_strstr(user_agent, "youdao"))uri->search_engine_type =               SEARCH_ENGINE_YOUDAO;
        else if (lg_string_strstr(user_agent, "sogou"))uri->search_engine_type =                SEARCH_ENGINE_SOGOU;
        else if (lg_string_strstr(user_agent, "gougou"))uri->search_engine_type =               SEARCH_ENGINE_GOUGOU;
        else if (lg_string_strstr(user_agent, "yisou"))uri->search_engine_type =                SEARCH_ENGINE_YISOU;
        else if (lg_string_strstr(user_agent, "haosou"))uri->search_engine_type =               SEARCH_ENGINE_HAOSOU;
        else if (lg_string_strstr(user_agent, "bingbot"))uri->search_engine_type =              SEARCH_ENGINE_BINGBOT;
        else if (lg_string_strstr(user_agent, "SemrushBot"))uri->search_engine_type =           SEARCH_ENGINE_SEMRUSHBOT;

        if (lg_string_strstr(user_agent, "spider")) uri->search_engine = 1;
        if (uri->search_engine_type) uri->search_engine = 1;
#if 0
        if (lg_string_strstr(user_agent, "spider")
                || lg_string_strstr(user_agent, "baidu")		// 百度
                || lg_string_strstr(user_agent, "google")		// 谷歌
                || lg_string_strstr(user_agent, "soso")
                || lg_string_strstr(user_agent, "360")			// 360
                || lg_string_strstr(user_agent, "yahoo")
                || lg_string_strstr(user_agent, "youdao")
                || lg_string_strstr(user_agent, "sogou")		// 搜狗
                || lg_string_strstr(user_agent, "gougou")
                || lg_string_strstr(user_agent, "yisou")		// 神马
                || lg_string_strstr(user_agent, "haosou")		// 好搜
                || lg_string_strstr(user_agent, "bingbot")		// 必应
                || lg_string_strstr(user_agent, "SemrushBot")		// 
           ) {
            uri->search_engine = 1;
        }
#endif
    }
}

adx_uri_t *adx_uri_parse(ngx_http_request_t *r)
{
    adx_uri_parse_domain(r, uri);
    adx_uri_parse_domain_s(r, uri);
    adx_uri_parse_domain_e(r, uri);
    adx_uri_parse_user_agent(r, uri);
}


#endif





