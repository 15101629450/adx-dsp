
#include "adx_com_module.h"
#include "ngx_adx_com_proto.h"

static char *demo_command(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

/* Commands */
static ngx_command_t demo_commands[] = {
    {
        ngx_string("demo"),
        NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
        demo_command,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL },

    ngx_null_command
};

/* http module */
static ngx_http_module_t demo_ctx = {
    NULL,				        /* preconfiguration */
    NULL,				        /* postconfiguration */
    NULL,				        /* create main configuration */
    NULL,				        /* init main configuration */
    NULL,				        /* create server configuration */
    NULL,				        /* merge server configuration */
    NULL,       /* create location configuration */
    NULL                        /* merge location configuration */
};

/* hook */
ngx_module_t demo = {
    NGX_MODULE_V1,
    &demo_ctx,         /* module context */
    demo_commands,     /* module directives */
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

ngx_bid_request_t *adx_com_baidu_adapter_init(ngx_pool_t *pool, ngx_chain_t *src);
ngx_com_request_t *adx_com_baidu_adapter_to_com(ngx_pool_t *pool, ngx_bid_request_t *bid_request);

ngx_chain_t *demo_adapter_request(ngx_pool_t *pool, ngx_chain_t *src)
{
    ngx_bid_request_t *bid_request = adx_com_baidu_adapter_init(pool, src);
    fprintf(stdout, "bid_request=%p\n", bid_request);

    ngx_com_request_t *com_request = adx_com_baidu_adapter_to_com(pool, bid_request);
    fprintf(stdout, "com_request=%p\n", com_request);

    return adx_com_to_chain(pool, com_request);
    return src;
}

ngx_chain_t *demo_adapter_response(ngx_pool_t *pool, ngx_chain_t *src)
{
    return src;
}

void demo_write_event_handler(ngx_http_request_t *r)
{
    ngx_http_send_header(r);
    ngx_int_t rc = ngx_http_output_filter(r, r->request_body->bufs);
    ngx_http_finalize_request(r, rc);
}

ngx_int_t demo_subrequest_handler(ngx_http_request_t *r, void *data, ngx_int_t rc)
{
    ngx_http_request_t *parent_request = r->parent;
    parent_request->write_event_handler = demo_write_event_handler;
    parent_request->headers_out.status = r->headers_out.status;
    parent_request->headers_out.content_length_n = 0;
    parent_request->request_body->bufs = NULL;

    if (r->upstream && r->upstream->out_bufs) {
        ngx_chain_t *bufs = demo_adapter_response(r->pool, r->upstream->out_bufs);
        parent_request->headers_out.content_length_n = adx_chain_len(bufs);
        parent_request->request_body->bufs = bufs;
    }

    return rc;
}

static void demo_post_handler(ngx_http_request_t *r)
{
    r->request_body->bufs = demo_adapter_request(r->pool, r->request_body->bufs);
    r->request_body->busy = r->request_body->bufs;

    ngx_int_t content_length = adx_chain_len(r->request_body->bufs);
    r->headers_in.content_length->value = adx_number_to_str(r->pool, content_length);
    r->headers_in.content_length_n = content_length;

    ngx_http_post_subrequest_t *post_subrequest = ngx_palloc(r->pool, sizeof(ngx_http_post_subrequest_t));
    if (!post_subrequest) return ngx_http_finalize_request(r, NGX_ERROR);

    ngx_str_t location = {13, (u_char *)"/adx_com_fcgi"};
    ngx_http_request_t *sub_request = NULL;
    post_subrequest->handler = demo_subrequest_handler;
    ngx_http_subrequest(r,
            &location,
            NULL,
            &sub_request,
            post_subrequest,
            NGX_HTTP_SUBREQUEST_IN_MEMORY | NGX_HTTP_SUBREQUEST_WAITED);
}

static ngx_int_t demo_handler(ngx_http_request_t *r)
{
    if (r->method != NGX_HTTP_POST)
        return adx_network_send_not_found(r);

    ngx_int_t rc = ngx_http_read_client_request_body(r, demo_post_handler);
    if (rc >= NGX_HTTP_SPECIAL_RESPONSE)
        return rc; 

    return NGX_DONE;
}

static char *demo_command(ngx_conf_t *cf, ngx_command_t *cmd, void *location_conf)
{
    ngx_http_core_loc_conf_t *core_conf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    core_conf->handler = demo_handler;
    return NGX_CONF_OK;
}



