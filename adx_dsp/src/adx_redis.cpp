
#include <stdlib.h>
#include <string.h>
#include <hiredis/hircluster.h>

#include <iostream>
#include <vector>
#include <map>

#include "adx_type.h"
#include "adx_redis.h"
#include "errorcode.h"

using namespace std;

int adx_redis_reply_check(redisReply *reply)
{
    int errcode = E_SUCCESS;
    if (!reply) return E_REDIS_COMMAND;

    switch(reply->type) {

        case REDIS_REPLY_STRING :
        case REDIS_REPLY_ARRAY :
        case REDIS_REPLY_INTEGER :
            break;

        case REDIS_REPLY_NIL :
            errcode = E_REDIS_KEY_NOT_EXIST; // KEY 为空
            break;

        case REDIS_REPLY_STATUS :
            if (reply->str && strncmp(reply->str, "OK", 2) == 0)
                break;

        case REDIS_REPLY_ERROR :
        default :
            errcode = E_REDIS_COMMAND; // redis 指令错误
    }

    return errcode;
}

int adx_redis_reply_value(redisReply *reply, string &value)
{
    value = "";
    int errcode = E_SUCCESS;
    if (!reply) return E_REDIS_COMMAND;

    if (reply->type == REDIS_REPLY_NIL) {
        errcode = E_REDIS_KEY_NOT_EXIST; // KEY 为空

    } else if (reply->type ==REDIS_REPLY_STATUS && reply->type && strcmp(reply->str, "OK") == 0) {
        value = reply->str;

    } else if (reply->type == REDIS_REPLY_STRING) { // string
        value = reply->str;

    } else if (reply->type == REDIS_REPLY_INTEGER) { // number
        char buf[64];
        snprintf(buf, 64, "%lld", reply->integer);
        value = buf;

    } else {
        errcode = E_REDIS_COMMAND; // redis 指令错误
    }

    return errcode;
}

/*********************************
 * command ==> not return
 *********************************/
int adx_redis_command(redisContext *conn, const char *format, ...)
{
    if (conn->err) { // 连接失败
        redisReconnect(conn); // 重新连接
        if (conn->err) {
            return E_REDIS_CONNECT_INVALID; //依然失败
        }
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisvCommand(conn, format, ap);
    va_end(ap);
#if 0
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    fputc('\n', stdout);
    fflush(stdout);
    va_end(ap);
#endif
    int errcode = adx_redis_reply_check(reply);
    if (reply) freeReplyObject(reply);
    return errcode;
}

int adx_redis_cluster_command(redisClusterContext *conn, const char *format, ...)
{
    if (conn->err) {
        redisClusterReconnect(conn);
        if (conn->err) return E_REDIS_CONNECT_INVALID;
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisClustervCommand(conn, format, ap);
    va_end(ap);

    int errcode = adx_redis_reply_check(reply);
    if (reply) freeReplyObject(reply);
    return errcode;
}

/*********************************
 * command ==> return string
 *********************************/
int adx_redis_command(redisContext *conn, string &value, const char *format, ...)
{
    if (conn->err) { // 连接失败
        redisReconnect(conn); // 重新连接
        if (conn->err) {
            return E_REDIS_CONNECT_INVALID; //依然失败
        }
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisvCommand(conn, format, ap);
    va_end(ap);

    int errcode = adx_redis_reply_value(reply, value);
    if (reply) freeReplyObject(reply);
    return errcode;
}

int adx_redis_cluster_command(redisClusterContext *conn, string &value, const char *format, ...)
{
    if (conn->err) {
        redisClusterReconnect(conn);
        if (conn->err) return E_REDIS_CONNECT_INVALID;
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisClustervCommand(conn, format, ap);
    va_end(ap);

    int errcode = adx_redis_reply_value(reply, value);
    if (reply) freeReplyObject(reply);
    return errcode;
}

/*********************************
 * command ==> return number
 *********************************/
int adx_redis_command(redisContext *conn, int &value, const char *format, ...)
{
    if (conn->err) { // 连接失败
        redisReconnect(conn); // 重新连接
        if (conn->err) {
            return E_REDIS_CONNECT_INVALID; //依然失败
        }
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisvCommand(conn, format, ap);
    va_end(ap);

    string str;
    int errcode = adx_redis_reply_value(reply, str);
    if (errcode == E_SUCCESS) value = atol(str.c_str());

    if (reply) freeReplyObject(reply);
    return errcode;
}

int adx_redis_cluster_command(redisClusterContext *conn, int &value, const char *format, ...)
{
    if (conn->err) {
        redisClusterReconnect(conn);
        if (conn->err) return E_REDIS_CONNECT_INVALID;
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisClustervCommand(conn, format, ap);
    va_end(ap);

    string str;
    int errcode = adx_redis_reply_value(reply, str);
    if (errcode == E_SUCCESS) value = atol(str.c_str());

    if (reply) freeReplyObject(reply);
    return errcode;
}

/*********************************
 * check array and map
 *********************************/
int adx_redis_reply_array(redisReply *reply)
{
    int errcode = E_SUCCESS;
    if (!reply) return E_REDIS_COMMAND;
    if (reply->type == REDIS_REPLY_NIL || reply->elements == 0) {
        errcode = E_REDIS_KEY_NOT_EXIST;

    } else if (reply->type != REDIS_REPLY_ARRAY) {
        errcode = E_REDIS_COMMAND;
    }

    return errcode;
}

int adx_redis_command(redisContext *conn, vector<string> &values, const char *format, ...)
{
    if (conn->err) { // 连接失败
        redisReconnect(conn); // 重新连接
        if (conn->err) {
            return E_REDIS_CONNECT_INVALID; //依然失败
        }
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisvCommand(conn, format, ap);
    va_end(ap);

    int errcode = adx_redis_reply_array(reply);
    if (errcode == E_SUCCESS) {
        for (size_t i = 0; i < reply->elements; i++) {
            values.push_back(reply->element[i]->str);
        }   
    }

    if (reply) freeReplyObject(reply);
    return errcode;
}

int adx_redis_command(redisContext *conn, vector<int> &values, const char *format, ...)
{
    if (conn->err) { // 连接失败
        redisReconnect(conn); // 重新连接
        if (conn->err) {
            return E_REDIS_CONNECT_INVALID; //依然失败
        }
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisvCommand(conn, format, ap);
    va_end(ap);

    int errcode = adx_redis_reply_array(reply);
    if (errcode == E_SUCCESS) {
        for (size_t i = 0; i < reply->elements; i++) {
            values.push_back(atol(reply->element[i]->str));
        }   
    }

    if (reply) freeReplyObject(reply);
    return errcode;
}

int adx_redis_command(redisContext *conn, map<string, string> &values, const char *format, ...)
{
    if (conn->err) { // 连接失败
        redisReconnect(conn); // 重新连接
        if (conn->err) {
            return E_REDIS_CONNECT_INVALID; //依然失败
        }
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisvCommand(conn, format, ap);
    va_end(ap);

    int errcode = adx_redis_reply_array(reply);
    if (errcode == E_SUCCESS) {
        for (size_t i = 0, n = 0; i < reply->elements / 2; i++) {
            char *key = reply->element[n++]->str;
            char *value = reply->element[n++]->str;
            values[key] = value;
        }
#if 0
        map<string, string>::iterator  iter;
        for(iter = values.begin(); iter != values.end(); iter++) {
            fprintf(stdout, "[map][%s][%s]\n", iter->first.c_str(), iter->second.c_str());
        }
#endif
    }

    if (reply) freeReplyObject(reply);
    return errcode;
}

int adx_redis_command(redisContext *conn, map<string, int> &values, const char *format, ...)
{
    if (conn->err) { // 连接失败
        redisReconnect(conn); // 重新连接
        if (conn->err) {
            return E_REDIS_CONNECT_INVALID; //依然失败
        }
    }

    va_list ap;
    va_start(ap,format);
    redisReply *reply = (redisReply *)redisvCommand(conn, format, ap);
    va_end(ap);

    int errcode = adx_redis_reply_array(reply);
    if (errcode == E_SUCCESS) {
        for (size_t i = 0, n = 0; i < reply->elements / 2; i++) {
            char *key = reply->element[n++]->str;
            int value = atol(reply->element[n++]->str);
            values[key] = value;
        }
    }

    if (reply) freeReplyObject(reply);
    return errcode;
}

adx_redis_conn *adx_redis_init(
        char *setting_host, int setting_port, char *setting_pass,
        char *controller_host, int controller_port, char *controller_pass,
        char *counter_host, int counter_port, char *counter_pass
        )
{
    adx_redis_conn *conn = NULL;
    redisContext *setting = redisConnect(setting_host, setting_port, setting_pass); // 物料
    redisContext *controller = redisConnect(controller_host, controller_port, controller_pass); // 投放控制
    redisClusterContext *counter = redisClusterConnect(counter_host, counter_port, counter_pass); // 集群
    if (!setting || !controller || !counter) goto err;
    if (setting->err || controller->err || counter->err) goto err;

    conn = (adx_redis_conn *)je_malloc(sizeof(adx_redis_conn));
    if (!conn) goto err;

    conn->setting = setting;
    conn->controller = controller;
    conn->counter = counter;
    return conn;

err:
    if (setting) redisFree(setting);
    if (controller) redisFree(controller);
    if (counter) redisClusterFree(counter);
    return NULL;
}

adx_redis_conn *adx_redis_init(adx_list_t *adx_conf)
{
    return adx_redis_init(
            GET_CONF_STR(adx_conf, "redis", "redis_setting_ip"),
            GET_CONF_NUM(adx_conf, "redis", "redis_setting_port"),
            GET_CONF_STR(adx_conf, "redis", "redis_setting_pass"),

            GET_CONF_STR(adx_conf, "redis", "redis_controller_ip"),
            GET_CONF_NUM(adx_conf, "redis", "redis_controller_port"),
            GET_CONF_STR(adx_conf, "redis", "redis_controller_pass"),

            GET_CONF_STR(adx_conf, "redis", "redis_counter_ip"),
            GET_CONF_NUM(adx_conf, "redis", "redis_counter_port"),
            GET_CONF_STR(adx_conf, "redis", "redis_counter_pass")
            );
}


