
#include "adx_auto_cost.h"

int adx_auto_cost_push_event(adx_auto_cost_t *auto_cost_handle, char *mess)
{
    redisContext *conn = auto_cost_handle->conn_setting;
    if (conn->err) { // 连接失败
	redisReconnect(conn); // 重新连接
	if (conn->err) {
	    return -1;
	}
    }

    if (!mess) return -1;

    redisReply *reply = NULL;
    pthread_mutex_lock(&auto_cost_handle->lock);
    reply = redisCommand(conn, "PUBLISH auto_cost_queue %s", mess);
    pthread_mutex_unlock(&auto_cost_handle->lock);
    if (!reply) return -1;

    freeReplyObject(reply);
    return 0;
}

int adx_auto_cost_forge_event(adx_auto_cost_t *auto_cost_handle, char *policyid, char *timebucket, char *key, char *val)
{
    redisClusterContext *conn = auto_cost_handle->conn_counter;
    if (conn->err) { // 连接失败
	redisClusterReconnect(conn); // 重新连接
	if (conn->err) {
	    return -1;
	}
    }

    if (!policyid || !timebucket || !key || !val) return -1;

    redisReply *reply = NULL;
    reply = redisClusterCommand(conn, "HSET dsp10minutecounter_%s_%s %s %s", policyid, timebucket, key, val);
    if (!reply) return -1;

    freeReplyObject(reply);
    return 0;
}



