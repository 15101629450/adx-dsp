
#include "adx_network.h"
#include "adx_string.h"

adx_network_t *adx_network_init(ngx_http_request_t *r)
{
    adx_network_t *network = ngx_pcalloc(r->pool, sizeof(adx_network_t));
    if (!network) return NULL;

    memset(network, 0, sizeof(adx_network_t));
    return network;
}

int adx_network_add_buffer(ngx_http_request_t *r, adx_network_t *network, void *p_buffer, size_t size)
{
    if (!p_buffer || !size) return -1;

    ngx_chain_t *buffer = adx_chain_alloc(r->pool, p_buffer, size);
    if (!buffer) return -1;

    if (!network->head) {
        network->head = buffer;
        network->head->next = NULL;

    } else if (!network->next) {
        network->head->buf->last_buf = 0;
        network->head->next = network->next = buffer;

    } else {
        network->next->buf->last_buf = 0;
        network->next->next = buffer;
        network->next = buffer;
    }

    network->content_length += size;
    return 0;
}

int adx_network_add_str(ngx_http_request_t *r, adx_network_t *network, void *str)
{
    if (!str) return -1;
    int len = strlen(str);
    if (!len) return -1;
    return adx_network_add_buffer(r, network, str, len);
}

/*** SEND GET ***/
int adx_network_send(ngx_http_request_t *r, adx_network_t *p)
{
    r->headers_out.status = p->status; // NGX_HTTP_OK;
    r->headers_out.content_type = p->content_type;
    r->headers_out.content_length_n = p->content_length;

    int rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only)
        return rc;

    return ngx_http_output_filter(r, p->head);
}

int adx_network_send_buffer(ngx_http_request_t *r, adx_network_t *network, void *p_buffer, size_t size)
{
    adx_network_t *p = network;
    if (!network) {
        p = ngx_pcalloc(r->pool, sizeof(adx_network_t));
        memset(p, 0, sizeof(adx_network_t));
        p->status = NGX_HTTP_OK;
    }

    if (adx_network_add_buffer(r, p, p_buffer, size))
        return -1;

    return adx_network_send(r, p);
}

int adx_network_send_str(ngx_http_request_t *r, adx_network_t *network, void *str)
{
    if (!str) return -1;
    int len = strlen(str);
    if (!len) return -1;
    return adx_network_send_buffer(r, network, str, len);
}

int adx_network_send_not_found(ngx_http_request_t *r)
{
    r->headers_out.status = NGX_HTTP_NOT_FOUND;
    r->headers_out.content_length_n = 15;
    int rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only)
        return rc;

    ngx_chain_t *buffer = adx_chain_alloc(r->pool, "404 Not Found\r\n", 15);
    if (!buffer) return NGX_ERROR;

    return ngx_http_output_filter(r, buffer);
}

/*** SEND POST ***/
void adx_network_post_send(ngx_http_request_t *r, adx_network_t *p)
{
    int rc = adx_network_send(r, p);
    return ngx_http_finalize_request(r, rc); 
}

void adx_network_post_send_buffer(ngx_http_request_t *r, adx_network_t *network, void *p_buffer, size_t size)
{
    int rc = adx_network_send_buffer(r, network, p_buffer, size);
    return ngx_http_finalize_request(r, rc); 
}

void adx_network_post_send_str(ngx_http_request_t *r, adx_network_t *network, void *str)
{
    return adx_network_post_send_buffer(r, network, str, strlen(str));
}

void adx_network_post_send_not_found(ngx_http_request_t *r)
{
    int rc = adx_network_send_not_found(r);
    return ngx_http_finalize_request(r, rc); 
}

/*** GET POST BUF ***/
ngx_str_t adx_network_get_post_buffer(ngx_http_request_t *r)
{
    ngx_str_t str = ngx_null_string;

    if (!r->headers_in.content_length)
        return str;

    if (!r->headers_in.content_length->value.data || r->headers_in.content_length->value.len <= 0)
        return str;

    size_t content_length = atoi((char *)r->headers_in.content_length->value.data);
    if (content_length <= 0) return str;

    char *buffer = ngx_palloc(r->pool, content_length + 1);
    if (!buffer) return str;

    size_t size = 0;
    ngx_chain_t *p = NULL;
    for (p = r->request_body->bufs; p; p = p->next) {

        ngx_buf_t *node_buffer = p->buf;
        memcpy(&buffer[size], node_buffer->pos, node_buffer->last - node_buffer->pos);
        size += node_buffer->last - node_buffer->pos;
        if (size > content_length)
            return str;
    }

    if (content_length != size)
        return str;

    buffer[size] = 0;
    str.data = (void *)buffer;
    str.len = size;
    return str;
}

/*** UP LOAD FILE ***/
ngx_str_t adx_network_upload_file(ngx_http_request_t *r, ngx_str_t src)
{
    ngx_str_t dest = ngx_null_string;

    // buffer
    char *buffer = (char *)src.data;
    buffer[src.len - 1] = 0; //防止越界

    // Content-Disposition
    char *Content_Disposition = strstr(buffer, "\r\n");
    if (!Content_Disposition) return dest;
    *Content_Disposition = 0;	// key END
    Content_Disposition++;	// \r
    Content_Disposition++;	// \n

    // Content-Type
    char *Content_Type = strstr(Content_Disposition, "\r\n");
    if (!Content_Type) return dest;
    *Content_Type = 0;		// Content_Disposition END
    Content_Type++;		// \r
    Content_Type++;		// \n

    // data
    char *data= (strstr(Content_Type, "\r\n\r\n"));
    if (!data) return dest;
    *data = 0;			// Content_Type END
    data++;			// \r
    data++;			// \n
    data++;			// \r
    data++;			// \n

    // key
    char key[128];
    size_t key_size = sprintf(key, "\r\n%s", buffer);
    if (key_size < 32 || key_size > 120)
        return dest;

    // data END
    size_t i, size = 0, max = src.len - (data - buffer);
    for (i = 0; i < max; i++) {

        if (strncmp(&data[i], key, key_size) == 0) {

            size = i;
            break;
        }
    }

    if (!size) return dest;

    // fprintf(stdout, "[key][%s]\n", key);
    // fprintf(stdout, "[Content_Disposition][%s]\n", Content_Disposition);
    // fprintf(stdout, "[Content_Type][%s]\n", Content_Type);
    // fprintf(stdout, "[data][%s]\n", data);
    dest.data = (void *)data;
    dest.len = size;
    return dest;
}

/*** ADD HEADERS ***/
int adx_network_add_headers(ngx_http_request_t *r, char *key, char *value)
{
    if (!key || !value)
        return -1;

    ngx_table_elt_t *node = ngx_list_push (&r->headers_out.headers);
    if (node == NULL) return -1;

    node->key.len = strlen(key);
    node->value.len = strlen(value);

    node->key.data = ngx_pcalloc(r->pool, node->key.len);
    node->value.data = ngx_pcalloc(r->pool, node->value.len);
    if (!node->key.data || !node->value.data)
        return -1;

    memcpy(node->key.data, key, node->key.len);
    memcpy(node->value.data, value, node->value.len);

    node->hash = 1;
    return 0;
}




