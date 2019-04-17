
#include "adx_com_module.h"
#include "ngx_adx_com_proto.h"

#define NGX_PROTOBUF_SET_STR(o, k, v)     \
{                                         \
    ngx_str_t s = ngx_string(v);          \
    NGX_PROTOBUF_SET_STRING(o,k,(&s));    \
}                                         \

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

ngx_bid_request_t *adx_com_baidu_request_init(ngx_pool_t *pool, ngx_chain_t *src);
void adx_com_baidu_request_print(ngx_pool_t *pool, ngx_bid_request_t *bid_request);
ngx_com_request_t *adx_com_baidu_request_to_com(ngx_pool_t *pool, ngx_bid_request_t *bid_request);

ngx_chain_t *demo_adapter_request(ngx_pool_t *pool, ngx_chain_t *src)
{
    fprintf(stdout, "demo_adapter_request\n");
    ngx_bid_request_t *bid_request = adx_com_baidu_request_init(pool, src);
    ngx_com_request_t *com_request = adx_com_baidu_request_to_com(pool, bid_request);

    //    adx_com_baidu_request_init(pool, src);
    //    ngx_str_t out = demo_com_request(pool);
    //    ngx_chain_t *dest = adx_chain_alloc(pool, out.data, out.len);
    //    return dest;
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

ngx_str_t demo_com_request(ngx_pool_t *pool)
{
    ngx_com_request_t *com_request = ngx_com_request__alloc(pool);
    NGX_PROTOBUF_SET_STR(com_request, bid, "aaa");

    ngx_com_request_adx_slot_t *adx_slot = ngx_com_request__add__adx_slot(com_request, pool);
    NGX_PROTOBUF_SET_STRING(adx_slot, adx_slot_id, ADX_STR(pool, "bbb"));
    NGX_PROTOBUF_SET_NUMBER(adx_slot, adx_slot_type, 1);
    NGX_PROTOBUF_SET_NUMBER(adx_slot, width, 100);
    NGX_PROTOBUF_SET_NUMBER(adx_slot, height, 200);

    ngx_com_request_device_t *device = ngx_com_request_device__alloc(pool);
    ngx_com_request__set_device(com_request, device);

    ngx_com_request_device_device_id_t *device_id = NULL;
    device_id = ngx_com_request_device__add__device_id(device, pool);
    NGX_PROTOBUF_SET_STR(device_id, id, "AAA");
    NGX_PROTOBUF_SET_NUMBER(device_id, type, 2);

    device_id = ngx_com_request_device__add__device_id(device, pool);
    NGX_PROTOBUF_SET_STR(device_id, id, "BBB");
    NGX_PROTOBUF_SET_NUMBER(device_id, type, 3);


    NGX_PROTOBUF_SET_NUMBER(device, device_type, 3);
    NGX_PROTOBUF_SET_NUMBER(device, os, 1);
    NGX_PROTOBUF_SET_STR(device, os_version, "1.0");
    NGX_PROTOBUF_SET_NUMBER(device, carrier, 2);
    NGX_PROTOBUF_SET_NUMBER(device, connection, 3);
    NGX_PROTOBUF_SET_STR(device, model, "Model");
    NGX_PROTOBUF_SET_STR(device, brand, "Brand");
    NGX_PROTOBUF_SET_STR(device, user_agent, "UA");
    NGX_PROTOBUF_SET_STR(device, ip, "IP");

    ngx_com_request_app_t *app = ngx_com_request_app__alloc(pool);
    ngx_com_request__set_app(com_request, app);

    NGX_PROTOBUF_SET_STR(app, app_id, "ddd");
    NGX_PROTOBUF_SET_NUMBER(app, app_category, 100);

    ngx_str_t output;
    output.len = ngx_com_request__size(com_request);
    output.data = ngx_palloc(pool, output.len);

    ngx_protobuf_context_t ctx;
    ctx.pool = pool;
    ctx.buffer.start = output.data;
    ctx.buffer.pos = ctx.buffer.start;
    ctx.buffer.last = ctx.buffer.start + output.len;

    int ret = ngx_com_request__pack(com_request, &ctx);
    int len = ctx.buffer.pos - ctx.buffer.start;
    fprintf(stdout, "ret=%d [%zd][%d]\n", ret, output.len, len);

    return output;
}

