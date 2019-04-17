
#include "adx_com_module.h"

static void *adx_com_baidu_create(ngx_conf_t *cf);
static char *adx_com_baidu_command(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

/* Commands */
static ngx_command_t adx_com_baidu_commands[] = {
    {
        ngx_string("adx_com_baidu"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1, // NGX_CONF_NOARGS,
        adx_com_baidu_command,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL },

    ngx_null_command
};

/* http module */
static ngx_http_module_t adx_com_baidu_ctx = {
    NULL,				        /* preconfiguration */
    NULL,				        /* postconfiguration */
    NULL,				        /* create main configuration */
    NULL,				        /* init main configuration */
    NULL,				        /* create server configuration */
    NULL,				        /* merge server configuration */
    adx_com_baidu_create,       /* create location configuration */
    NULL                        /* merge location configuration */
};

/* hook */
ngx_module_t adx_com_baidu = {
    NGX_MODULE_V1,
    &adx_com_baidu_ctx,         /* module context */
    adx_com_baidu_commands,     /* module directives */
    NGX_HTTP_MODULE,	        /* module type */
    NULL,				        /* init master */
    NULL,				        /* init module */
    NULL,		                /* init process */
    NULL,				        /* init thread */
    NULL,				        /* exit thread */
    NULL,				        /* exit process */
    NULL,				        /* exit master */
    NGX_MODULE_V1_PADDING
};

void *adx_com_baidu_create(ngx_conf_t *cf)
{
    adx_com_baidu_conf *conf = ngx_pcalloc(cf->pool, sizeof(adx_com_baidu_conf));
    return conf ? conf : NGX_CONF_ERROR;
}

void adx_com_baidu_write_event_handler(ngx_http_request_t *r)
{
    ngx_http_send_header(r);
    ngx_int_t rc = ngx_http_output_filter(r, r->request_body->bufs);
    ngx_http_finalize_request(r, rc);
}

ngx_int_t adx_com_baidu_subrequest_handler(ngx_http_request_t *r, void *data, ngx_int_t rc)
{
#if 0
    if(r->headers_out.status != NGX_HTTP_OK) {
        ngx_http_finalize_request(r, r->headers_out.status);
        return rc;
    }
#endif
    ngx_http_request_t *parent_request = r->parent;
    parent_request->write_event_handler = adx_com_baidu_write_event_handler;
    parent_request->headers_out.status = r->headers_out.status;
    parent_request->headers_out.content_length_n = 0;
    parent_request->request_body->bufs = NULL;

    if (r->upstream && r->upstream->out_bufs) {
        ngx_chain_t *bufs = adx_com_baidu_adapter_response(parent_request->pool, r->upstream->out_bufs);
        parent_request->headers_out.content_length_n = adx_chain_len(bufs);
        parent_request->request_body->bufs = bufs;
    }

    return rc;
}

static void adx_com_baidu_post_handler(ngx_http_request_t *r)
{
    r->request_body->bufs = adx_com_baidu_adapter_request(r->pool, r->request_body->bufs);
    r->request_body->busy = r->request_body->bufs;

    ngx_int_t content_length = adx_chain_len(r->request_body->bufs);
    r->headers_in.content_length->value = adx_number_to_str(r->pool, content_length);
    r->headers_in.content_length_n = content_length;

    ngx_http_post_subrequest_t *post_subrequest = ngx_palloc(r->pool, sizeof(ngx_http_post_subrequest_t));
    if (!post_subrequest) return ngx_http_finalize_request(r, NGX_ERROR);

    ngx_http_request_t *sub_request = NULL;
    adx_com_baidu_conf *conf = ngx_http_get_module_loc_conf(r, adx_com_baidu);
    post_subrequest->handler = adx_com_baidu_subrequest_handler;
    ngx_http_subrequest(r,
            &conf->adx_com_location,
            NULL,
            &sub_request,
            post_subrequest,
            NGX_HTTP_SUBREQUEST_IN_MEMORY | NGX_HTTP_SUBREQUEST_WAITED);
}

static ngx_int_t adx_com_baidu_handler(ngx_http_request_t *r)
{
    if (r->method != NGX_HTTP_POST)
        return adx_network_send_not_found(r);

    ngx_int_t rc = ngx_http_read_client_request_body(r, adx_com_baidu_post_handler);
    if (rc >= NGX_HTTP_SPECIAL_RESPONSE)
        return rc; 

    return NGX_DONE;
}

static char *adx_com_baidu_command(ngx_conf_t *cf, ngx_command_t *cmd, void *location_conf)
{
    if (cf->args->nelts != 2) return NGX_CONF_ERROR;
    adx_com_baidu_conf *conf = ngx_http_conf_get_module_loc_conf(cf, adx_com_baidu);
    ngx_str_t *value = cf->args->elts;
    conf->adx_com_location = value[1];

    ngx_http_core_loc_conf_t *core_conf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    core_conf->handler = adx_com_baidu_handler;
    return NGX_CONF_OK;
}





