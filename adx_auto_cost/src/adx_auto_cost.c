
#include "hiredis/hircluster.h"

int adx_auto_cost_get_count(redisClusterContext *conn, int policyid, char *date)
{

    if (conn->err) { // 连接失败
	redisClusterReconnect(conn); // 重新连接
	if (conn->err) {
	    return -1; 
	}   
    }   

    // dsp10minutecounter_(policyid)_(10分钟段)
    redisReply *reply = (redisReply *)redisClusterCommand(conn, "HGETALL auto_cost_bid_%d_%s", policyid, date);
    if (!reply) return -1; 

    if (reply->type != REDIS_REPLY_ARRAY) {
	freeReplyObject(reply); 
	return -1; 
    }   

    int i, n;
    for (i = 0, n = 0; i < reply->elements / 2; i++) {
	char *key = reply->element[n++]->str;
	char *val = reply->element[n++]->str;

	fprintf(stdout, "%s,%s\n", key, val);
    }   

    freeReplyObject(reply);    
    return 0;

}

#if 0
int main()
{

    redisClusterContext *conn_counter = redisClusterConnect("127.0.0.1", 9000, NULL);
    adx_auto_cost_get_count(conn_counter, 45, "01_05");

    return 0;
}
#endif






