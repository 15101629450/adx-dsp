
#include "adx_dsp.h"

static pthread_mutex_t fcgi_accept_mutex = PTHREAD_MUTEX_INITIALIZER;
int adx_dsp_doit(adx_request_t *r)
{    
    // 获取请求
    pthread_mutex_lock(&fcgi_accept_mutex);
    int errcode = FCGX_Accept_r(&r->fcgi_app);
    pthread_mutex_unlock(&fcgi_accept_mutex);
    if (errcode != 0) return -1;

    // 解析请求
    if (adx_request(r)) return adx_response_empty(r);

    // 匹配广告
    if (adx_select(r)) return adx_response_empty(r);

    // 返回数据
    return adx_response(r);
}

void *adx_dsp_thread(void *p)
{
    adx_request_t *r = (adx_request_t *)p;
    for(;;) {

        r->pool = adx_pool_create();
        if (r->pool) adx_dsp_doit(r);
        adx_pool_free(r->pool);
    }

    pthread_exit(NULL);
}

#if 10
int main(int argc, char *argv[])
{
    FCGX_Init();
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    // 初始化配置文件
    adx_list_t *conf = adx_conf_file_load(DSP_CONF_PATH);
    if (!conf) return -1;

    // 初始化日志
    // if (adx_log_init(
    //  GET_CONF_STR(conf, "default", "log_path"),
    //  GET_CONF_NUM(conf, "default", "log_level")))
    //    return -1;

    // 初始化 redis
    adx_redis_conn *redis_conn = adx_redis_init(conf);
    if (!redis_conn) return -1;

    // 初始化 IP库
    size_t IPB = openIPB(DSP_REGION_DB_PATH);
    if (!IPB) return -1;

    // ADX DUMP
    if (adx_dump_main(conf))
        return -1;

    int i, cpu_count = 4;
    pthread_t tid[cpu_count];
    for (i = 0; i < cpu_count; i++) {

        adx_request_t *request = (adx_request_t *)je_malloc(sizeof(adx_request_t));
        if (!request) return -1;

        request->IPB = IPB;
        request->conf = conf;
        request->redis_conn = redis_conn;
        FCGX_InitRequest(&request->fcgi_app, 0, 0);
        pthread_create(&tid[i], NULL, adx_dsp_thread, request);
    }

    for (i = 0; i < cpu_count; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
#endif



