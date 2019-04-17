
#ifndef __ADX_REDIS_H__
#define __ADX_REDIS_H__

#include <hiredis/hircluster.h>
#include <iostream>
#include <vector>
#include <map>

#include "adx_conf_file.h"
#include "errorcode.h"

using namespace std;

int adx_redis_command(redisContext *conn, const char *format, ...);
int adx_redis_command(redisClusterContext *conn, const char *format, ...);

int adx_redis_command(redisContext *conn, string &value, const char *format, ...);
int adx_redis_command(redisClusterContext *conn, string &value, const char *format, ...);

int adx_redis_command(redisContext *conn, int &value, const char *format, ...);
int adx_redis_command(redisClusterContext *conn, int &value, const char *format, ...);

int adx_redis_command(redisContext *conn, vector<string> &values, const char *format, ...);
int adx_redis_command(redisContext *conn, vector<int> &values, const char *format, ...);

int adx_redis_command(redisContext *conn, map<string, string> &values, const char *format, ...);
int adx_redis_command(redisContext *conn, map<string, int> &values, const char *format, ...);

typedef struct {
    redisContext *setting; // redis 物料
    redisContext *controller; // redis 投放控制
    redisClusterContext *counter; // redis集群 展现/点击/价格/匀速/KPI/频次等 
} adx_redis_conn;
adx_redis_conn *adx_redis_init(adx_list_t *adx_conf);
adx_redis_conn *adx_redis_init(
        char *setting_host, int setting_port, char *setting_pass,
        char *controller_host, int controller_port, char *controller_pass,
        char *counter_host, int counter_port, char *counter_pass
        );

#endif




