
#include "adx_auto_cost.h"

typedef struct {
    int bid;
    adx_list_t head;
} adx_auto_cost_bid_t;

typedef struct {
    adx_list_t next;
    char *mapid;
    char *adxcode;
    char *mediacode;
    char *regioncode;
    char *value;

} adx_auto_cost_bid_node;

int adx_auto_cost_bid_create_event(adx_auto_cost_t *auto_cost_handle)
{
    return 0;
}

int adx_auto_cost_bid_write_event(adx_auto_cost_t *auto_cost_handle, char *mapid, char *adxcode, char *mediacode, char *regioncode, char *value)
{
    redisContext *conn = auto_cost_handle->conn_setting;
    if (conn->err) { // 连接失败
	redisReconnect(conn); // 重新连接
	if (conn->err) {
	    return -1;
	}
    }

    if (!mapid || !adxcode || !mediacode || !regioncode || !value)
	return -1;

    redisReply *reply = NULL;
    pthread_mutex_lock(&auto_cost_handle->lock);
    reply = redisCommand(conn,
	    "HSET dsp_mapid_bid_%s %s_%s_%s %s", mapid, adxcode, mediacode, regioncode, value);
    pthread_mutex_unlock(&auto_cost_handle->lock);

    if (!reply) return -1;
    if (reply->type != REDIS_REPLY_INTEGER) {
	freeReplyObject(reply);
	return -1;
    }

    freeReplyObject(reply);
    return 0;
}

int adx_auto_cost_bid_flush_event(adx_auto_cost_t *auto_cost_handle)
{
    return 0;
}



