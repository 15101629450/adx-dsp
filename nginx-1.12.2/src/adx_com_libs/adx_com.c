
#include <adx_com.h>

ngx_com_request_t *adx_com_init(ngx_pool_t *pool)
{
    ngx_com_request_t *com_request = ngx_com_request__alloc(pool);
    ngx_com_request_device_t *com_device = ngx_com_request_device__alloc(pool);
    ngx_com_request_app_t *com_app = ngx_com_request_app__alloc(pool);

    ngx_com_request__set_device(com_request, com_device);
    ngx_com_request__set_app(com_request, com_app);

    return com_request;
}

ngx_chain_t *adx_com_to_chain(ngx_pool_t *pool, ngx_com_request_t *com_request)
{
    int len = ngx_com_request__size(com_request);
    u_char *buffer = ngx_palloc(pool, len);
    if (!buffer) return NULL;

    ngx_protobuf_context_t ctx;
    ctx.pool = pool;
    ctx.buffer.start = buffer;
    ctx.buffer.pos = ctx.buffer.start;
    ctx.buffer.last = ctx.buffer.start + len;

    int ret = ngx_com_request__pack(com_request, &ctx);
    if (ret != NGX_OK) return NULL;

    return adx_chain_alloc(pool, buffer, len);
}


