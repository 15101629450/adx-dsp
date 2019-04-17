
#include <nginx.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <adx_string.h>
#include <adx_network.h>
#include <ngx_baidu_proto.h>

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
    NULL,                       /* create location configuration */
    NULL                        /* merge location configuration */
};

/* hook */
ngx_module_t demo = {
    NGX_MODULE_V1,
    &demo_ctx,		/* module context */
    demo_commands,	/* module directives */
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

static int count = 0;
static void demo_post_handler(ngx_http_request_t *r)
{
    ngx_protobuf_context_t ctx;
    ngx_str_t buffer = adx_network_get_post_buffer(r);

    ctx.pool = r->pool;
    ctx.buffer.start = buffer.data;
    ctx.buffer.pos = ctx.buffer.start;
    ctx.buffer.last = buffer.data + buffer.len;
    ctx.reuse_strings = 1;

    ngx_bid_request_t *bid_request = ngx_bid_request__alloc(r->pool);
    int ret = ngx_bid_request__unpack(bid_request, &ctx);
    // fprintf(stdout, "pid=%d sock=%d\n", getpid(), r->connection->fd);
    fprintf(stdout, "pid=%d sock=%d ret=%d count=%d\n", getpid(), r->connection->fd, ret, ++count);
    adx_network_post_send_str(r, NULL, "ok");
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



