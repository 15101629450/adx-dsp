
#ifndef __ADX_AUTO_COST_H__
#define __ADX_AUTO_COST_H__

#include <Python.h>
#include <signal.h>
#include <pthread.h>

#include "adx_type.h"
#include "adx_list.h"
#include "adx_conf_file.h"
#include "hiredis/hircluster.h"

typedef struct {

    char *AdSetting_ip;
    int AdSetting_port;
    char *AdSetting_pass;

    char *AdController_ip;
    int AdController_port;
    char *AdController_pass;

    char *AdCounter_ip;
    int AdCounter_port;
    char *AdCounter_pass;

    char *auto_cost_python_log_path;

} adx_auto_cost_conf_t; 

typedef struct {

    redisContext		*conn_wait; // wait
    redisContext		*conn_setting; // 物料
    redisClusterContext		*conn_counter; // redis集群 展现/点击/价格(累加)
    char			*auto_cost_error;
    pthread_mutex_t		lock;

    adx_auto_cost_conf_t	*conf;

} adx_auto_cost_t;

int adx_auto_cost_dict_add(PyObject *dict, char *key, char *val);
int adx_auto_cost_get_count(redisClusterContext *conn, PyObject *dict, char *count_key);

int adx_auto_cost_cmd(redisContext *conn, char *cmd);
int adx_auto_cost_get_map(redisContext *conn, PyObject *dict, char *count_key);

adx_auto_cost_conf_t *adx_auto_cost_conf_load(char *auto_cost_error);
adx_auto_cost_t *auto_cost_handle_create(adx_auto_cost_conf_t *conf, char *auto_cost_error);

int adx_auto_cost_wait_init(redisContext *conn);
PyObject *adx_auto_cost_wait_get(adx_auto_cost_t *auto_cost_handle);

int adx_auto_cost_bid_create_event(adx_auto_cost_t *auto_cost_handle);
int adx_auto_cost_bid_write_event(adx_auto_cost_t *auto_cost_handle, char *mapid, char *adxcode, char *mediacode, char *regioncode, char *value);
int adx_auto_cost_bid_flush_event(adx_auto_cost_t *auto_cost_handle);

int adx_auto_cost_push_event(adx_auto_cost_t *auto_cost_handle, char *mess);
int adx_auto_cost_forge_event(adx_auto_cost_t *auto_cost_handle, char *policyid, char *timebucket, char *key, char *val);

#endif



