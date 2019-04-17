
#ifndef __ADX_COM_MODULE_H__
#define __ADX_COM_MODULE_H__

#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <adx_json.h>
#include <adx_network.h>
#include <adx_string.h>
#include <adx_com.h>

#include <ngx_baidu_proto.h>

typedef struct {
    ngx_str_t adx_com_location;
} adx_com_baidu_conf;

ngx_chain_t *adx_com_baidu_adapter_request(ngx_pool_t *pool, ngx_chain_t *src);
ngx_chain_t *adx_com_baidu_adapter_response(ngx_pool_t *pool, ngx_chain_t *src);

#endif


