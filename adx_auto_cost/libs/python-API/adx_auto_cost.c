
#include "adx_auto_cost.h"

#define AUTO_COST_VERSION	"1.0"
#define	AUTO_COST_CONF_PATH	"/etc/dspads_odin/"
#define	AUTO_COST_CONF_FILE	"dsp_auto_cost.conf"
#define IF_ADX_DSP_CONF_STR(o, s1, s2, s3) {o=GET_CONF_STR(s1,s2,s3);if(o==0){fprintf(stderr,"[ERR][CONF FILE][%s][%s]\n",s2,s3);return NULL;}}
#define IF_ADX_DSP_CONF_NUM(o, s1, s2, s3) {o=GET_CONF_NUM(s1,s2,s3);if(o==0){fprintf(stderr,"[ERR][CONF FILE][%s][%s]\n",s2,s3);return NULL;}}

adx_auto_cost_conf_t *adx_auto_cost_conf_load(char *auto_cost_error)
{
    adx_auto_cost_conf_t *conf = (adx_auto_cost_conf_t *)je_malloc(sizeof(adx_auto_cost_conf_t));
    if (!conf) {
	sprintf(auto_cost_error, "malloc: err");
	return NULL;
    }

    adx_conf_file_t *auto_conf = adx_conf_file_load((char *)AUTO_COST_CONF_PATH AUTO_COST_CONF_FILE);
    if (!auto_conf) {
	sprintf(auto_cost_error, "conf file err: %s", AUTO_COST_CONF_PATH AUTO_COST_CONF_FILE);
	return NULL;
    }

    IF_ADX_DSP_CONF_STR(conf->AdSetting_ip,		auto_conf,	"redis",	"AdSetting_ip");
    IF_ADX_DSP_CONF_NUM(conf->AdSetting_port,		auto_conf,	"redis",	"AdSetting_port");
    IF_ADX_DSP_CONF_STR(conf->AdSetting_pass,		auto_conf,	"redis",	"AdSetting_pass");

    IF_ADX_DSP_CONF_STR(conf->AdController_ip,		auto_conf,	"redis",	"AdController_ip");
    IF_ADX_DSP_CONF_NUM(conf->AdController_port,	auto_conf,	"redis",	"AdController_port");
    IF_ADX_DSP_CONF_STR(conf->AdController_pass,	auto_conf,	"redis",	"AdController_pass");

    IF_ADX_DSP_CONF_STR(conf->AdCounter_ip,		auto_conf,	"redis",	"AdCounter_ip");
    IF_ADX_DSP_CONF_NUM(conf->AdCounter_port,		auto_conf,	"redis",	"AdCounter_port");
    IF_ADX_DSP_CONF_STR(conf->AdCounter_pass,		auto_conf,	"redis",	"AdCounter_pass");

    IF_ADX_DSP_CONF_STR(conf->auto_cost_python_log_path,		auto_conf,	"auto_cost_python",	"log_path");

    return conf;
}

void adx_auto_cost_signal(int sig)
{
    switch(sig) {
	case SIGINT:
	case SIGTERM:
	    exit(0);
    }
}

int adx_auto_cost_dict_add(PyObject *dict, char *key, char *val)
{
    int ret = -1;
    if (!key || !val) return -1;

    // PyObject *p_key = PyString_FromString(key);
    PyObject *p_val = PyString_FromString(val);
    if (!p_val) goto err;

    // ret = PyDict_SetItem(dict, p_key, p_val);
    ret = PyDict_SetItemString(dict, key, p_val);
err:
    // if (p_key) {Py_DECREF(p_key);}
    if (p_val) {Py_DECREF(p_val);}
    return ret;
}

adx_auto_cost_t *auto_cost_handle_create(adx_auto_cost_conf_t *conf, char *auto_cost_error)
{

    adx_auto_cost_t *auto_cost_handle = (adx_auto_cost_t *)je_malloc(sizeof(adx_auto_cost_t));
    if (!auto_cost_handle) return NULL;

    redisContext	*conn_wait	= redisConnect(conf->AdSetting_ip, conf->AdSetting_port, conf->AdSetting_pass);
    redisContext	*conn_setting	= redisConnect(conf->AdSetting_ip, conf->AdSetting_port, conf->AdSetting_pass);
    redisClusterContext	*conn_counter	= redisClusterConnect(conf->AdCounter_ip, conf->AdCounter_port, conf->AdCounter_pass);
    if (!conn_wait || !conn_setting || !conn_counter || conn_wait->err || conn_setting->err || conn_counter->err) {
	sprintf(auto_cost_error, "redis conn: err");
	return NULL;
    }

    auto_cost_handle->conn_wait = conn_wait;
    auto_cost_handle->conn_setting = conn_setting;
    auto_cost_handle->conn_counter = conn_counter;
    auto_cost_handle->auto_cost_error = auto_cost_error;

    signal(SIGINT, adx_auto_cost_signal);
    signal(SIGTERM, adx_auto_cost_signal);
    pthread_mutex_init(&auto_cost_handle->lock, NULL);

    auto_cost_handle->conf = conf;

    return auto_cost_handle;
}

int adx_auto_cost_wait_init(redisContext *conn)
{
    if (conn->err) { // 连接失败
	redisReconnect(conn); // 重新连接
	if (conn->err) {
	    return -1;
	}
    }

    // SUBSCRIBE
    redisReply *reply = (redisReply *)redisCommand(conn, "SUBSCRIBE auto_cost_queue");
    if (!reply) return -1;

    if (reply->elements != 3 || !reply->element[0]->str || strcmp(reply->element[0]->str, "subscribe") != 0) {
	freeReplyObject(reply);
	return -1;
    }

    freeReplyObject(reply);
    return 0;
}

int adx_auto_cost_wait_get_info(redisContext *conn, PyObject *dict)
{
    if (conn->err) { // 连接失败
	if (adx_auto_cost_wait_init(conn))
	    return -1;
    }

    redisReply *reply = NULL;
    if (redisGetReply(conn, (void **)&reply) != REDIS_OK)
	return -1;

    char *saveptr = NULL;
    char *str = strtok_r(reply->element[2]->str, "|", &saveptr);
    if (!str) goto err;

    while(str) {

	char *save_kv = NULL;
	char *key = strtok_r(str, "=", &save_kv);
	char *val = strtok_r(NULL, "=", &save_kv);
	if (!key || !val) goto err;

	if (adx_auto_cost_dict_add(dict, key, val) != 0)
	    goto err;

	str = strtok_r(NULL, "|", &saveptr);
    }

    freeReplyObject(reply);
    return 0;
err:
    freeReplyObject(reply);
    return -1;
}

int adx_auto_cost_get_count(redisClusterContext *conn, PyObject *dict, char *count_key)
{

    if (conn->err) { // 连接失败
	redisClusterReconnect(conn); // 重新连接
	if (conn->err) {
	    return -1;
	}
    }

    // dsp10minutecounter_(policyid)_(10分钟段)
    redisReply *reply = (redisReply *)redisClusterCommand(conn, "HGETALL %s", count_key);
    if (!reply) return -1;

    if (reply->type != REDIS_REPLY_ARRAY) {
	freeReplyObject(reply);	
	return -1;
    }

    int i, n;
    for (i = 0, n = 0; i < reply->elements / 2; i++) {
	char *key = reply->element[n++]->str;
	char *val = reply->element[n++]->str;

	if (adx_auto_cost_dict_add(dict, key, val) != 0) {
	    freeReplyObject(reply);	
	    return -1;
	}
    }

    freeReplyObject(reply);	
    return 0;

}

int adx_auto_cost_wait_get_count(redisClusterContext *conn, PyObject *dict, PyObject *count_key)
{
    if (!count_key) return -1;
    char *str = PyString_AsString(count_key);
    if (!str) return -1;

    return adx_auto_cost_get_count(conn, dict, str);
}

PyObject *adx_auto_cost_wait_get(adx_auto_cost_t *auto_cost_handle)
{
    PyObject *dict = PyDict_New();
    PyObject *dict_info = PyDict_New();
    PyObject *dict_count = PyDict_New();
    if (!dict || !dict_info || !dict_count)
	goto err;

    if (adx_auto_cost_wait_get_info(auto_cost_handle->conn_wait, dict_info))
	goto err;

    if (adx_auto_cost_wait_get_count(auto_cost_handle->conn_counter, dict_count, PyDict_GetItemString(dict_info, "count_key")))
	goto err;

    if (PyDict_SetItemString(dict, "INFO", dict_info))
	goto err;
    if (PyDict_SetItemString(dict, "COUNT", dict_count))
	goto err;

    if (dict_info)	{Py_DECREF(dict_info);}
    if (dict_count)	{Py_DECREF(dict_count);}
    return dict;

err:
    if (dict)		{Py_DECREF(dict);}
    if (dict_info)	{Py_DECREF(dict_info);}
    if (dict_count)	{Py_DECREF(dict_count);}
    return NULL;
}

int adx_auto_cost_get_map(redisContext *conn, PyObject *dict, char *count_key)
{

    if (conn->err) { // 连接失败
	redisReconnect(conn); // 重新连接
	if (conn->err) {
	    return -1;
	}
    }

    // fprintf(stdout, "HGETALL %s", count_key);
    redisReply *reply = (redisReply *)redisCommand(conn, "HGETALL %s", count_key);
    if (!reply) return -1;

    if (reply->type != REDIS_REPLY_ARRAY) {
	freeReplyObject(reply);	
	return -1;
    }

    int i, n;
    for (i = 0, n = 0; i < reply->elements / 2; i++) {
	char *key = reply->element[n++]->str;
	char *val = reply->element[n++]->str;

	if (adx_auto_cost_dict_add(dict, key, val) != 0) {
	    freeReplyObject(reply);	
	    return -1;
	}
    }

    freeReplyObject(reply);	
    return 0;

}

int adx_auto_cost_cmd(redisContext *conn, char *cmd)
{

    if (conn->err) { // 连接失败
	redisReconnect(conn); // 重新连接
	if (conn->err) {
	    return -1;
	}
    }

    redisReply *reply = (redisReply *)redisCommand(conn, cmd);
    if (!reply) return -1;

    freeReplyObject(reply);	
    return 0;

}



