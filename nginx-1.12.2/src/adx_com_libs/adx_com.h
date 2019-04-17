
#ifndef __ADX_COM_H__
#define __ADX_COM_H__

#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_adx_com_proto.h>
#include <adx_string.h>

ngx_chain_t *adx_com_to_chain(ngx_pool_t *pool, ngx_com_request_t *com_request);

#endif


