
#ifndef __ADX_NETWORK_H__
#define __ADX_NETWORK_H__

#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {

    int status;
    size_t content_length;
    ngx_str_t content_type;
    ngx_chain_t *head;
    ngx_chain_t *next;

} adx_network_t;

adx_network_t *adx_network_init(ngx_http_request_t *r);
int adx_network_add_buffer(ngx_http_request_t *r, adx_network_t *network, void *p_buffer, size_t size);
int adx_network_add_str(ngx_http_request_t *r, adx_network_t *network, void *str);

/*** SEND GET ***/
int adx_network_send(ngx_http_request_t *r, adx_network_t *p);
int adx_network_send_buffer(ngx_http_request_t *r, adx_network_t *network, void *p_buffer, size_t size);
int adx_network_send_str(ngx_http_request_t *r, adx_network_t *network, void *str);
int adx_network_send_not_found(ngx_http_request_t *r);

/*** SEND POST ***/
void adx_network_post_send(ngx_http_request_t *r, adx_network_t *p);
void adx_network_post_send_buffer(ngx_http_request_t *r, adx_network_t *network, void *p_buffer, size_t size);
void adx_network_post_send_str(ngx_http_request_t *r, adx_network_t *network, void *str);
void adx_network_post_send_not_found(ngx_http_request_t *r);

/*** GET POST BUF ***/
ngx_str_t adx_network_get_post_buffer(ngx_http_request_t *r);

/*** UP LOAD FILE ***/
ngx_str_t adx_network_upload_file(ngx_http_request_t *r, ngx_str_t src);

/*** ADD HEADERS ***/
int adx_network_add_headers(ngx_http_request_t *r, char *key, char *value);

#endif


