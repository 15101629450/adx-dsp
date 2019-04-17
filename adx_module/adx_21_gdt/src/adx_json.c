
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adx_json.h"

typedef struct {
    char *str;
    int len;
} adx_json_str;

typedef struct {
    char *pos;
    adx_pool_t *pool;
} adx_json_parse_t;
int adx_json_parse_value(adx_json_parse_t *parse, adx_json_t *node);

char *adx_json_status(int status)
{
    switch(status) {
        case ADX_JSON_OBJECT : return "ADX_JSON_OBJECT";
        case ADX_JSON_ARRAY  : return "ADX_JSON_ARRAY";
        case ADX_JSON_STRING : return "ADX_JSON_STRING";
        case ADX_JSON_NUMBER : return "ADX_JSON_NUMBER";
        case ADX_JSON_EMPTY  : return "ADX_JSON_EMPTY";
        case ADX_JSON_BOOL   : return "ADX_JSON_BOOL";
        case ADX_JSON_NULL   : return "ADX_JSON_NULL";
        default : return "";
    }
}

void adx_json_skip_space(adx_json_parse_t *parse)
{
    for (; *parse->pos; parse->pos++) {
        if (*parse->pos > 32)
            return;
    }
}

char *adx_json_strdup(adx_pool_t *pool, const char *src, int len)
{
    int i;
    char *str, *dest;
    dest = str = adx_alloc(pool, len + 1);
    if (!dest) return NULL;

    for (i = 0; i < len; i++) {

        if (src[i] == '\\') {
            switch(src[++i]) {
                case 'b' : *dest++ = '\b'; break;
                case 'f' : *dest++ = '\f'; break;
                case 'r' : *dest++ = '\r'; break;
                case 'n' : *dest++ = '\n'; break;
                case 't' : *dest++ = '\t'; break;
                default  : *dest++ = src[i];
            }

        } else {
            *dest++ = src[i];
        }
    }

    *dest = 0;
    return str;
}

adx_json_t *adx_json_create(adx_pool_t *pool, adx_json_t *parent, const char *key, int len)
{
    adx_json_t *json = adx_alloc(pool, sizeof(adx_json_t));
    if (!json) return NULL;

    json->type = 0;
    json->key = NULL;
    json->val = NULL;
    json->parent = parent;

    if (key) {
        json->key = adx_json_strdup(pool, key, len);
        if (!json->key) return NULL;
    }

    adx_list_init(&json->child);
    if (parent) adx_list_add(&parent->child, &json->next);
    return json;
}

adx_json_t *adx_json_create_key(adx_pool_t *pool, adx_json_t *parent, const char *key, int len)
{
    if (!key || !len) return NULL;
    return adx_json_create(pool, parent, key, len);
}

static int adx_json_parse_string(adx_json_parse_t *parse, adx_json_str *str)
{
    if (*parse->pos++ != '\"') // skip \"
        return ADX_JSON_ERROE;

    if (*parse->pos == '\"') { // empty
        parse->pos++;
        return ADX_JSON_EMPTY;
    }

    char *pos = NULL;
    for (pos = parse->pos; *pos; pos++) {

        if (*pos == '\\') {
            if (!(*(++pos))) return ADX_JSON_ERROE;

        } else if (*pos == '\"') {

            str->str = parse->pos;
            str->len = (pos - parse->pos);
            parse->pos = ++pos;
            return ADX_JSON_STRING;
        }
    }

    return ADX_JSON_ERROE;
}

adx_json_t *adx_json_parse_key(adx_json_parse_t *parse, adx_json_t *parent)
{
    adx_json_str str;
    int json_type = adx_json_parse_string(parse, &str);
    if (json_type != ADX_JSON_STRING) return NULL;

    return adx_json_create_key(parse->pool, parent, str.str, str.len);
}

int adx_json_parse_value_string(adx_json_parse_t *parse, adx_json_t *node)
{
    adx_json_str str;
    int json_type = adx_json_parse_string(parse, &str);
    if (json_type == ADX_JSON_ERROE) return ADX_JSON_ERROE;

    if (json_type != ADX_JSON_EMPTY) {
        node->val = adx_json_strdup(parse->pool, str.str, str.len);
        if (!node->val) return ADX_JSON_ERROE;
    }

    return json_type;
}

int adx_json_parse_value_number(adx_json_parse_t *parse, adx_json_t *node)
{
    char *pos = NULL;
    for (pos = parse->pos; *pos; pos++) {
        switch(*pos) {
            case '0' :
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
            case '7' :
            case '8' :
            case '9' :
            case '+' :
            case '-' :
            case 'e' :
            case 'E' :
            case '.' : break;
            default  : {
                           char *str = parse->pos;
                           int len = pos - parse->pos;
                           node->val = adx_json_strdup(parse->pool, str, len);
                           if (!node->val) return ADX_JSON_ERROE;

                           parse->pos = pos;
                           return ADX_JSON_NUMBER;
                       }
        }
    }

    return ADX_JSON_ERROE;
}

int adx_json_parse_other(adx_json_parse_t *parse, adx_json_t *node, int type, char *str, int len)
{
    parse->pos += len;
    node->val = adx_json_strdup(parse->pool, str, len);
    if (!node->val) return ADX_JSON_ERROE;
    return type;
}

int adx_json_parse_object(adx_json_parse_t *parse, adx_json_t *parent)
{
    if (*parse->pos++ != '{') // skip {
        return ADX_JSON_ERROE;

    adx_json_skip_space(parse);
    if (*parse->pos == '}') { // empty
        parse->pos++;
        return ADX_JSON_EMPTY;
    }

    while(*parse->pos) {

        // key
        adx_json_skip_space(parse);
        adx_json_t *node = adx_json_parse_key(parse, parent);
        if (!node) return ADX_JSON_ERROE;

        adx_json_skip_space(parse);
        if (*parse->pos++ != ':') return ADX_JSON_ERROE;

        // value
        int json_type = adx_json_parse_value(parse, node);
        if (json_type == ADX_JSON_ERROE) return ADX_JSON_ERROE;
        node->type = json_type;

        adx_json_skip_space(parse);
        if (*parse->pos != ',')
            break;

        parse->pos++;
    }

    if (*parse->pos++ != '}') return ADX_JSON_ERROE;
    return ADX_JSON_OBJECT;
}

int adx_json_parse_array(adx_json_parse_t *parse, adx_json_t *parent)
{
    if (*parse->pos++ != '[') // skip {
        return ADX_JSON_ERROE;

    adx_json_skip_space(parse);
    if (*parse->pos == ']') { // empty
        parse->pos++;
        return ADX_JSON_EMPTY;
    }

    while(*parse->pos) {

        adx_json_t *node = adx_json_create(parse->pool, parent, NULL, 0);
        if (!node) return ADX_JSON_ERROE;

        int json_type = adx_json_parse_value(parse, node);
        if (json_type == ADX_JSON_ERROE) return ADX_JSON_ERROE;
        node->type = json_type;

        adx_json_skip_space(parse);
        if (*parse->pos != ',')
            break;

        parse->pos++;
    }

    if (*parse->pos++ != ']') return ADX_JSON_ERROE;
    return ADX_JSON_ARRAY;
}

int adx_json_parse_value(adx_json_parse_t *parse, adx_json_t *node)
{
    adx_json_skip_space(parse);
    const char *pos = parse->pos;

    if (*pos == '{') // object
        return adx_json_parse_object(parse, node);

    if (*pos == '[') // array
        return adx_json_parse_array(parse, node);

    if (*pos == '\"') // string
        return adx_json_parse_value_string(parse, node);

    if (*pos == '-' || (*pos >= '0' && *pos <= '9')) // number
        return adx_json_parse_value_number(parse, node);

    if (strncmp(pos, "true", 4) == 0) // true
        return adx_json_parse_other(parse, node, ADX_JSON_BOOL, "true", 4);

    if (strncmp(pos, "false", 5) == 0) // false
        return adx_json_parse_other(parse, node, ADX_JSON_BOOL, "false", 5);

    if (strncmp(pos, "null", 4) == 0) // null
        return adx_json_parse_other(parse, node, ADX_JSON_NULL, "null", 4);

    return ADX_JSON_ERROE;
}

adx_json_t *adx_json_parse(adx_pool_t *pool, char *str)
{
    adx_json_parse_t parse = {str, pool};
    adx_json_t *root = ADX_JSON_ROOT(pool);
    if (!root) return NULL;

    adx_json_skip_space(&parse);
    if (adx_json_parse_object(&parse, root) != ADX_JSON_OBJECT)
        return NULL;

    return root;
}

void adx_json_display(adx_json_t *root)
{
    if (!root) return;
    adx_list_t *p = NULL;
    adx_list_for_each(p, &root->child) {
        adx_json_t *node = adx_list_entry(p, adx_json_t, next);
        adx_json_t *parent = node->parent;
        while(parent && parent->parent) {
            fprintf(stdout, "│    ");
            parent = parent->parent;
        }

        fprintf(stdout, "├──");
        if (node->key && node->val) fprintf(stdout, "[%s][%s][%s]\n", node->key,  node->val, adx_json_status(node->type));
        else if (node->key) fprintf(stdout, "[%s][%s]\n", node->key,  adx_json_status(node->type));
        else if (node->val) fprintf(stdout, "[%s][%s]\n", node->val,  adx_json_status(node->type));
        else fprintf(stdout, "[%s]\n", adx_json_status(node->type));

        adx_json_display(node);
    }
}

adx_json_t *adx_json_find(adx_json_t *root, const char *key)
{
    if (!root) return NULL;
    adx_list_t *p = NULL;
    adx_list_for_each(p, &root->child) {
        adx_json_t *node = adx_list_entry(p, adx_json_t, next);
        if (strcmp(node->key, key) == 0)
            return node;
    }

    return NULL;
}

char *adx_json_find_value(adx_json_t *root, const char *key)
{
    adx_json_t *node = adx_json_find(root, key);
    if (!node) return NULL;
    return node->val;
}

adx_json_t *adx_json_add(adx_pool_t *pool, adx_json_t *root, int json_type, const char *key, const char *val)
{
    if(!root || !pool) return NULL;

    int len = 0;
    if (key) len = strlen(key);
    adx_json_t *node = adx_json_create(pool, root, key, len);
    if (!node) return NULL;

    node->type = json_type;
    switch(json_type) {
        case ADX_JSON_OBJECT :
        case ADX_JSON_ARRAY  :
        case ADX_JSON_EMPTY  :
            return node;

        case ADX_JSON_STRING :
        case ADX_JSON_NUMBER :
        case ADX_JSON_BOOL   :
        case ADX_JSON_NULL   :
            node->val = adx_json_strdup(pool, val, strlen(val));
            if (node->val) return node;
    }

    return NULL;
}

adx_json_t *adx_json_add_string(adx_pool_t *pool, adx_json_t *root, const char *key, const char *val)
{
    return adx_json_add(pool, root, ADX_JSON_STRING, key, val);
}

adx_json_t *adx_json_add_number(adx_pool_t *pool, adx_json_t *root, const char *key, long int val)
{
    char number[64];
    snprintf(number, 64, "%ld", val);
    return adx_json_add(pool, root, ADX_JSON_NUMBER, key, number);
}

adx_json_t *adx_json_add_double(adx_pool_t *pool, adx_json_t *root, const char *key, double val)
{
    char number[64];
    snprintf(number, 64, "%f", val);
    return adx_json_add(pool, root, ADX_JSON_NUMBER, key, number);
}

typedef struct {
    char *buf;
    int total, len;
    adx_pool_t *pool;
} adx_json_buffer;

int adx_json_buffer_from_array(adx_json_buffer *buffer, adx_json_t *root);
int adx_json_buffer_from_string(adx_json_buffer *buffer, adx_json_t *node);
int adx_json_buffer_from_other(adx_json_buffer *buffer, adx_json_t *node);

int adx_json_buffer_realloc(adx_json_buffer *buffer)
{
    buffer->total = buffer->total * 2;
    char *new_buf = adx_alloc(buffer->pool, buffer->total);
    if (!new_buf) return -1;

    memcpy(new_buf, buffer->buf, buffer->len);
    buffer->buf = new_buf;
    return 0;
}

int adx_json_buffer_strcat(adx_json_buffer *buffer, char *str)
{
    int len = strlen(str);
    if (buffer->len + len >= buffer->total - 1)
        if (adx_json_buffer_realloc(buffer)) return -1;

    memcpy(buffer->buf + buffer->len, str, len);
    buffer->len += len;
    return 0;
}

int adx_json_buffer_strcat_escapes(adx_json_buffer *buffer, char *str)
{
    int len = 0;
    char *p = str;
    char *dest = buffer->buf + buffer->len;
    for (; *p; p++) {
        switch(*p) {
            case '\b' : *dest++ = '\\'; *dest++ = 'b';  len += 2; break;
            case '\f' : *dest++ = '\\'; *dest++ = 'f';  len += 2; break;
            case '\r' : *dest++ = '\\'; *dest++ = 'r';  len += 2; break;
            case '\n' : *dest++ = '\\'; *dest++ = 'n';  len += 2; break;
            case '\t' : *dest++ = '\\'; *dest++ = 't';  len += 2; break;
            case '\\' : *dest++ = '\\'; *dest++ = '\\'; len += 2; break;
            case '\"' : *dest++ = '\\'; *dest++ = '\"'; len += 2; break;
            case '/'  : *dest++ = '\\'; *dest++ = '/';  len += 2; break;
            default : *dest++ = *p; len++;
        }

        if (buffer->len + len >= buffer->total - 1) {
            if (adx_json_buffer_realloc(buffer)) return -1;
            return adx_json_buffer_strcat_escapes(buffer, str);
        }
    }

    buffer->len += len;
    return 0;
}

#define ADX_JSON_BUFFER_STRCAT(buffer, str) {if(adx_json_buffer_strcat(buffer, str))return -1;}
#define ADX_JSON_BUFFER_STRCAT_ESCAPES(buffer, str) {if(adx_json_buffer_strcat_escapes(buffer, str))return -1;}

int adx_json_buffer_from_object(adx_json_buffer *buffer, adx_json_t *root)
{
    if (root->key) {
        ADX_JSON_BUFFER_STRCAT(buffer, "\"");
        ADX_JSON_BUFFER_STRCAT_ESCAPES(buffer, root->key);
        ADX_JSON_BUFFER_STRCAT(buffer, "\":{");
    } else {
        ADX_JSON_BUFFER_STRCAT(buffer, "{");
    }

    char comma = 0;
    adx_list_t *p = NULL;
    adx_list_for_each(p, &root->child) {
        adx_json_t *node = adx_list_entry(p, adx_json_t, next);
        if (comma) ADX_JSON_BUFFER_STRCAT(buffer, ",");
        switch(node->type) {
            case ADX_JSON_OBJECT : comma = 1;
                                   if (adx_json_buffer_from_object(buffer, node)) return -1;
                                   break;
            case ADX_JSON_ARRAY  : comma = 1;
                                   if (adx_json_buffer_from_array(buffer, node)) return -1;
                                   break;
            case ADX_JSON_STRING : comma = 1;
                                   if (adx_json_buffer_from_string(buffer, node)) return -1;
                                   break;
            case ADX_JSON_NUMBER :
            case ADX_JSON_BOOL   :
            case ADX_JSON_NULL   : comma = 1;
                                   if (adx_json_buffer_from_other(buffer, node)) return -1;
                                   break;
            case ADX_JSON_EMPTY  : comma = 0;
                                   break;
            default : return -1;
        }
    }

    ADX_JSON_BUFFER_STRCAT(buffer, "}");
    return 0;
}

int adx_json_buffer_from_array(adx_json_buffer *buffer, adx_json_t *root)
{
    if (root->key) {
        ADX_JSON_BUFFER_STRCAT(buffer, "\"");
        ADX_JSON_BUFFER_STRCAT_ESCAPES(buffer, root->key);
        ADX_JSON_BUFFER_STRCAT(buffer, "\":[");
    } else {
        ADX_JSON_BUFFER_STRCAT(buffer, "[");
    }

    char comma = 0;
    adx_list_t *p = NULL;
    adx_list_for_each(p, &root->child) {
        adx_json_t *node = adx_list_entry(p, adx_json_t, next);
        if (comma) ADX_JSON_BUFFER_STRCAT(buffer, ",");
        switch(node->type) {
            case ADX_JSON_OBJECT : comma = 1;
                                   if (adx_json_buffer_from_object(buffer, node)) return -1;
                                   break;
            case ADX_JSON_ARRAY  : comma = 1;
                                   if (adx_json_buffer_from_array(buffer, node)) return -1;
                                   break;
            case ADX_JSON_STRING : comma = 1;
                                   if (adx_json_buffer_from_string(buffer, node)) return -1;
                                   break;
            case ADX_JSON_NUMBER :
            case ADX_JSON_BOOL   :
            case ADX_JSON_NULL   : comma = 1;
                                   if (adx_json_buffer_from_other(buffer, node)) return -1;
                                   break;
            case ADX_JSON_EMPTY  : comma = 0;
                                   break;
            default : return -1;
        }
    }

    ADX_JSON_BUFFER_STRCAT(buffer, "]");
    return 0;
}

int adx_json_buffer_from_string(adx_json_buffer *buffer, adx_json_t *node)
{
    if (node->key) {
        ADX_JSON_BUFFER_STRCAT(buffer, "\"");
        ADX_JSON_BUFFER_STRCAT_ESCAPES(buffer, node->key);
        ADX_JSON_BUFFER_STRCAT(buffer, "\":\"");
        ADX_JSON_BUFFER_STRCAT_ESCAPES(buffer, node->val);
        ADX_JSON_BUFFER_STRCAT(buffer, "\"");
    } else {
        ADX_JSON_BUFFER_STRCAT(buffer, "\"");
        ADX_JSON_BUFFER_STRCAT_ESCAPES(buffer, node->val);
        ADX_JSON_BUFFER_STRCAT(buffer, "\"");
    }
    return 0;
}

int adx_json_buffer_from_other(adx_json_buffer *buffer, adx_json_t *node)
{
    if (node->key) {
        ADX_JSON_BUFFER_STRCAT(buffer, "\"");
        ADX_JSON_BUFFER_STRCAT_ESCAPES(buffer, node->key);
        ADX_JSON_BUFFER_STRCAT(buffer, "\":");
        ADX_JSON_BUFFER_STRCAT(buffer, node->val);
    } else {
        ADX_JSON_BUFFER_STRCAT(buffer, node->val);
    }
    return 0;
}

#define ADX_JSON_BUFF_SIZE 2048
char *adx_json_to_buffer(adx_pool_t *pool, adx_json_t *node)
{
    adx_json_buffer buffer;
    buffer.len = 0;
    buffer.total = ADX_JSON_BUFF_SIZE;
    buffer.pool = pool;
    buffer.buf = adx_alloc(pool, buffer.total);
    if (!buffer.buf) return NULL;

    if (adx_json_buffer_from_object(&buffer, node))
        return NULL;

    if (buffer.len <= 0)
        return NULL;

    buffer.buf[buffer.len] = 0;
    return buffer.buf;
}


#if 0
void array_print(adx_json_t *root)
{
    if (!root || !root->key) return;

    adx_list_t *p = NULL;
    adx_list_for_each(p, &root->child) {
        adx_json_t *node = adx_list_entry(p, adx_json_t, next);
        fprintf(stdout, "%s -> %s\n", root->key, node->val);
    }
}

int main(int argc, char *argv[])
{
    char buffer[4096] = {0};
    FILE *fp = fopen("./sh/json", "r");
    fread(buffer, 4096, 1, fp);

    adx_pool_t *pool = adx_pool_create();
    adx_json_t *root = adx_json_parse(pool, buffer);
    // adx_json_display(root); return 0; 

    root = ADX_JSON_ROOT(pool);
    adx_json_t *node = ADX_JSON_NODE_ADD(pool, root, ADX_JSON_STRING, "id", "xxx");

    node = ADX_JSON_NODE_ADD(pool, root, ADX_JSON_ARRAY, "a", NULL);
    ADX_JSON_NODE_ADD(pool, node, ADX_JSON_NUMBER, NULL, "1");
    ADX_JSON_NODE_ADD(pool, node, ADX_JSON_NUMBER, NULL, "2");
    ADX_JSON_NODE_ADD(pool, node, ADX_JSON_NUMBER, NULL, "3");

    node = ADX_JSON_NODE_ADD(pool, root, ADX_JSON_ARRAY, "b", NULL);
    ADX_JSON_NODE_ADD(pool, node, ADX_JSON_STRING, NULL, "a1");
    ADX_JSON_NODE_ADD(pool, node, ADX_JSON_STRING, NULL, "a2");
    ADX_JSON_NODE_ADD(pool, node, ADX_JSON_STRING, NULL, "a3");

    node = ADX_JSON_NODE_ADD(pool, root, ADX_JSON_OBJECT, "c", NULL);
    node = ADX_JSON_NODE_ADD(pool, node, ADX_JSON_OBJECT, "d", NULL);
    ADX_JSON_NODE_ADD(pool, node, ADX_JSON_STRING, "e", "s1");
    ADX_JSON_NODE_ADD(pool, node, ADX_JSON_STRING, "f", "s2");
    ADX_JSON_NODE_ADD(pool, node, ADX_JSON_STRING, "g", "s3");

    adx_json_display(root);
    fprintf(stdout, "%s\n", adx_json_to_buffer(pool, root));

    fprintf(stdout, "id = %s\n", adx_json_find_value(root, "id"));
    array_print(adx_json_find(root, "a"));
    array_print(adx_json_find(root, "b"));

    node = adx_json_find(root, "c");
    node = adx_json_find(node, "d");
    fprintf(stdout, "a -> b -> e = %s\n", adx_json_find_value(node, "e"));
    fprintf(stdout, "a -> b -> f = %s\n", adx_json_find_value(node, "f"));
    fprintf(stdout, "a -> b -> g = %s\n", adx_json_find_value(node, "g"));

    adx_free(pool);
    return 0;
}
#endif



