
#ifndef __ADX_UTIL_H__
#define __ADX_UTIL_H__

#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>

extern char *adx_http_host(ngx_http_request_t *r);
extern char *adx_http_user_agent(ngx_http_request_t *r);
extern char *adx_http_ipaddr(ngx_http_request_t *r, char *addr);
extern char *adx_http_ipaddr_forwarded(ngx_http_request_t *r);

extern int adx_http_set_cookie(ngx_http_request_t *r, char *cookie);
extern char *adx_http_get_cookie(ngx_http_request_t *r, char *cookie);

extern int adx_http_set_session(ngx_http_request_t *r, char *key, char *session_id);
extern int adx_http_get_session(ngx_http_request_t *r, char *key, char **session_id);

#endif


