
#include "pv.h"
#include "adx_region.h"
#include "adx_mysql.h"
#include "adx_conf_file.h"
#include "adx_log.h"

char success_msg[4096] = "Status: 200 OK\r\nContent-Type: image/gif; charset=utf-8\r\nConnection: close\r\n\r\n";
int success_msg_len = 0;
void make_success_message()
{
    char gif[64] = {
	0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00, 0x01, 0x00, 0x80, 0x00, 0x00, 0xff, 0x00, 0xff,
	0x00, 0x00, 0x00, 0x21, 0xf9, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x01, 0x00, 0x00, 0x02, 0x02, 0x44, 0x01, 0x00, 0x3b, 0x00};

    success_msg_len = strlen(success_msg);
    memcpy(success_msg + success_msg_len, gif, 44);
    success_msg_len += 44;
}

static size_t ipdb = 0;
static pv_conf_t *conf = NULL;
static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
void doit(adx_pool_t *pool, FCGX_Request *request)
{
    if (!pool || !request) return;

    pthread_mutex_lock(&accept_mutex);
    int rc = FCGX_Accept_r(request);
    pthread_mutex_unlock(&accept_mutex);
    if (rc < 0) return;

    char *requestaddr = FCGX_GetParam("REMOTE_ADDR", request->envp);
    char *requestparam = FCGX_GetParam("QUERY_STRING", request->envp);
    if (!requestaddr || !requestparam) return;
    if (!(*requestaddr) || !(*requestparam)) return;

    int ret = parseRequest(pool, ipdb, requestaddr, requestparam);
    fprintf(stdout, "parseRequest = %X\n", ret);

    FCGX_FPrintF(request->out, success_msg);
}

void *mysql_pv_thread(void *p)
{
    for(;;) {

	sleep(5);
	mysql_pv_loop(conf->mysql_host, conf->mysql_base, conf->mysql_user, conf->mysql_pass);
    }

    pthread_exit(NULL);
}

void *adx_thread(void *p)
{
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

    for (;;) {

	adx_pool_t *pool = adx_pool_create();
	doit(pool, &request);

	adx_free(pool);
	FCGX_Finish_r(&request);
    }

    pthread_exit(NULL);
}

#define IF_ADX_DSP_CONF_STR(o, s1, s2, s3) {o=GET_CONF_STR(s1,s2,s3);if(o==NULL){cout<<"conf err:"<<s3<<endl;return NULL;}}
#define IF_ADX_DSP_CONF_NUM(o, s1, s2, s3) {o=GET_CONF_NUM(s1,s2,s3);if(o==0){cout<<"conf err:"<<s3<<endl;return NULL;}}
pv_conf_t *pv_conf_load()
{
    pv_conf_t *conf = (pv_conf_t *)malloc(sizeof(pv_conf_t));
    if (!conf) return NULL;

    adx_conf_file_t *adx_conf = adx_conf_file_load("/etc/dspads_odin/dsp_pv.conf");
    if (!adx_conf) {
	fprintf(stdout, "adx_conf_file_load : ERR\n");
	return NULL;
    }

    IF_ADX_DSP_CONF_STR(conf->mysql_host, adx_conf, "mysql", "mysql_host");
    IF_ADX_DSP_CONF_STR(conf->mysql_base, adx_conf, "mysql", "mysql_base");
    IF_ADX_DSP_CONF_STR(conf->mysql_user, adx_conf, "mysql", "mysql_user");
    IF_ADX_DSP_CONF_STR(conf->mysql_pass, adx_conf, "mysql", "mysql_pass");
    
    IF_ADX_DSP_CONF_NUM(conf->log_level, adx_conf, "log", "log_level");
    IF_ADX_DSP_CONF_STR(conf->log_path, adx_conf, "log", "log_path");

    return conf;
}

#define THREAD_COUNT 64
int main(int argc, char *argv[])
{
    FCGX_Init();
    make_success_message();

    ipdb = openIPB("/etc/dspads_odin/ipb");
    if (!ipdb) return -1;

    conf = pv_conf_load();
    if (!conf) return -1;
    
    int ret = adx_log_init(conf->log_path, "pv", conf->log_level);
    if (ret != 0) return -1;

    ret = mysql_pv_init(conf->mysql_host, conf->mysql_base, conf->mysql_user, conf->mysql_pass);
    if (ret != 0) return -1;

    pthread_t mysql_pv_tid;
    pthread_create(&mysql_pv_tid, NULL, mysql_pv_thread, NULL);

    pthread_t tids[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) pthread_create(&tids[i], NULL, adx_thread, NULL);

    for (int i = 0; i < THREAD_COUNT; i++) pthread_join(tids[i], NULL);
    pthread_join(mysql_pv_tid, NULL);
    return 0;
}



