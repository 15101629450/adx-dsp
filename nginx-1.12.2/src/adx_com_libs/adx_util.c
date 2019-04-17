
#include "adx_util.h"

char *adx_http_host(ngx_http_request_t *r)
{
    ngx_http_headers_in_t *headers_in = &r->headers_in;
    if (!headers_in) return NULL;

    ngx_table_elt_t *p = headers_in->host;
    if (!p || !p->value.data) return NULL;

    return (char *)p->value.data;
}

char *adx_http_user_agent(ngx_http_request_t *r)
{
    ngx_http_headers_in_t *headers_in = &r->headers_in;
    if (!headers_in) return NULL;

    ngx_table_elt_t *p = headers_in->user_agent;
    if (!p || !p->value.data) return NULL;

    return (char *)p->value.data;
}

char *adx_http_ipaddr(ngx_http_request_t *r, char *addr)
{
    ngx_connection_t *connection = r->connection;
    if (!connection) return NULL;

    ngx_str_t *ip_addr = &connection->addr_text;
    if (!ip_addr) return NULL;

    memcpy(addr, ip_addr->data, ip_addr->len);
    addr[ip_addr->len] = 0;
    return addr;
}

char *adx_http_ipaddr_forwarded(ngx_http_request_t *r)
{
    ngx_http_headers_in_t *headers_in = &r->headers_in;
    if (!headers_in) return NULL;

    ngx_array_t *x_forwarded_for = &headers_in->x_forwarded_for;
    if (!x_forwarded_for) return NULL;
#if 1
    ngx_int_t count = x_forwarded_for->nelts;
    if (count <= 0) return NULL;

    ngx_table_elt_t **p = x_forwarded_for->elts;
    return (char *)p[0]->value.data;
#else
    ngx_uint_t i;
    for (i = 0; i < x_forwarded_for->nelts; i++) {
        ngx_table_elt_t **p = x_forwarded_for->elts;
        return p[i]->value.data;
    }
#endif
}

int adx_http_set_cookie(ngx_http_request_t *r, char *cookie)
{
    ngx_table_elt_t *set_cookie;
    set_cookie = ngx_list_push (&r->headers_out.headers);
    if (set_cookie == NULL)
        return -1;

    int cookie_size = strlen(cookie);
    char *buffer = ngx_pcalloc(r->pool, cookie_size);
    if (buffer == NULL)
        return -1;

    memcpy(buffer, cookie, cookie_size);
    set_cookie->hash = 1;
    ngx_str_set (&set_cookie->key, "Set-Cookie");
    set_cookie->value.data = (void *)buffer;
    set_cookie->value.len = cookie_size;
    return 0;
}

char *adx_http_get_cookie(ngx_http_request_t *r, char *cookie)
{
    ngx_str_t name;
    name.data = (u_char *)cookie;
    name.len = strlen(cookie);

    ngx_str_t *value = ngx_pcalloc(r->pool, sizeof(ngx_str_t));
    if (ngx_http_parse_multi_header_lines(&r->headers_in.cookies, &name, value) == NGX_DECLINED)
        return NULL;

    return (char *)value->data;
}

int adx_http_set_session(ngx_http_request_t *r, char *key, char *session_id)
{
    char session[256];
    sprintf(session, "%s=%s; Path=/action;", key, session_id);
    return adx_http_set_cookie(r, session);
}

int adx_http_get_session(ngx_http_request_t *r, char *key, char **session_id)
{
    char *cookie = NULL;
    if (!(cookie = adx_http_get_cookie(r, key)))
        return -1;

    *session_id = ngx_pcalloc(r->pool, 40);
    memset(*session_id, 0, 40);
    memcpy(*session_id, cookie, 36);
    return 0;
}


