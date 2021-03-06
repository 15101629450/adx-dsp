
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "adx_type.h"
#include "adx_dump.h"
#include "adx_time.h"
#include "adx_json.h"
#include "adx_util.h"
#include "adx_conf_file.h"
#include "adx_log.h"
#include "adx_redis.h"
#include "errorcode.h"

#define AUTO_COST_VERSION	"1.0"
#define	AUTO_COST_CONF_PATH	"/etc/dspads_odin/"
#define	AUTO_COST_CONF_FILE	"dsp_auto_cost.conf"

typedef struct {

    char *log_path; // 本地日志
    int log_level; // 日志等级

    char *adx_setting_host;
    int adx_setting_port;
    char *adx_setting_pass;

    char *adx_controller_host;
    int adx_controller_port;
    char *adx_controller_pass;

    char *adx_counter_host;
    int adx_counter_port;
    char *adx_counter_pass;

} auto_cost_conf_t;

#define IF_ADX_DSP_CONF_STR(o, s1, s2, s3) {o=GET_CONF_STR(s1,s2,s3);if(o==NULL){cout<<"conf err:"<<s3<<endl;return NULL;}}
#define IF_ADX_DSP_CONF_NUM(o, s1, s2, s3) {o=GET_CONF_NUM(s1,s2,s3);if(o==0){cout<<"conf err:"<<s3<<endl;return NULL;}}
auto_cost_conf_t *auto_cost_load_conf()
{
    auto_cost_conf_t *conf = new auto_cost_conf_t;
    if (!conf) {
        fprintf(stderr, "auto_cost_load_conf: err\n");
        return NULL;
    }

    adx_conf_file_t *auto_conf = adx_conf_file_load((char *)AUTO_COST_CONF_PATH AUTO_COST_CONF_FILE);
    if (!auto_conf) {
        fprintf(stderr, "adx_conf_file_load: err\n");
        return NULL;
    }

    IF_ADX_DSP_CONF_STR(conf->log_path, 		auto_conf,	"default",	"log_path");
    IF_ADX_DSP_CONF_NUM(conf->log_level, 		auto_conf,	"default",	"log_level");

    IF_ADX_DSP_CONF_STR(conf->adx_setting_host,		auto_conf,	"redis",	"AdSetting_ip");
    IF_ADX_DSP_CONF_NUM(conf->adx_setting_port,		auto_conf,	"redis",	"AdSetting_port");
    IF_ADX_DSP_CONF_STR(conf->adx_setting_pass,		auto_conf,	"redis",	"AdSetting_pass");

    IF_ADX_DSP_CONF_STR(conf->adx_controller_host,	auto_conf,	"redis",	"AdController_ip");
    IF_ADX_DSP_CONF_NUM(conf->adx_controller_port,	auto_conf,	"redis",	"AdController_port");
    IF_ADX_DSP_CONF_STR(conf->adx_controller_pass,	auto_conf,	"redis",	"AdController_pass");

    IF_ADX_DSP_CONF_STR(conf->adx_counter_host,		auto_conf,	"redis",	"AdCounter_ip");
    IF_ADX_DSP_CONF_NUM(conf->adx_counter_port,		auto_conf,	"redis",	"AdCounter_port");
    IF_ADX_DSP_CONF_STR(conf->adx_counter_pass,		auto_conf,	"redis",	"AdCounter_pass");

    return conf;
}

static int check_status = 0;
int auto_bid_check_time()
{
    struct tm tm;
    time_t t = time(NULL);
    localtime_r(&t, &tm);

    if (tm.tm_min % 10 == 0) 
    {

        switch(tm.tm_sec) {

            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                if (check_status == 0) check_status = 1;
                else if (check_status == 1) check_status = 2;
                break;

            default:
                check_status = 0;
                break;
        }
    }

    if (check_status == 1) return 0;
    return -1;
}

int load_policy_control_load(redisContext *adx_setting, redisClusterContext *adx_counter, adx_cache_t *root, int campaignid, int policyid, int complete_time)
{
    int errcode = E_SUCCESS;

    map<string, adx_size_t> dsp_policy_control_info;
    errcode = adx_redis_command(adx_setting, dsp_policy_control_info, "HGETALL dsp_policy_control_%ld", policyid); // policy control 信息
    if (errcode != E_SUCCESS) return errcode;

    // 是否自动出价
    // fprintf(stdout, "===>%zd\n", dsp_policy_control_info["autobid"]);
    if (dsp_policy_control_info["autobid"] != 1) return -1;

    adx_cache_t *policyid_cache = adx_cache_add(root, CACHE_NUM(policyid), CACHE_NULL());
    if (!policyid_cache) return E_MALLOC;

    map<string, adx_size_t>::iterator iter;
    for(iter = dsp_policy_control_info.begin(); iter != dsp_policy_control_info.end(); iter++) {
        adx_cache_add(policyid_cache, CACHE_STR(iter->first.c_str()), CACHE_NUM(iter->second));
    }

    adx_cache_add(policyid_cache, CACHE_STR("campaignid"), CACHE_NUM(campaignid));
    adx_cache_add(policyid_cache, CACHE_STR("complete_time"), CACHE_NUM(complete_time));
    return E_SUCCESS;
}

int auto_cost_policy_info_load(redisContext *conn_setting, redisClusterContext *conn_counter, adx_cache_t *root, int policyid, int complete_time)
{
    int errcode = E_SUCCESS;

    string json_value;
    errcode = adx_redis_command(conn_setting, json_value, "GET dsp_policy_info_%d", policyid);
    if (errcode != E_SUCCESS) return errcode;

    string campaign_id;
    errcode = adx_json_key_value(json_value.c_str(), "campaignid", campaign_id);
    if (errcode != E_SUCCESS) return errcode;

    // string autob_id;
    // errcode = adx_json_key_value(json_value.c_str(), "autobid", autob_id);
    // if (errcode != E_SUCCESS) return errcode;

    int campaignid = atoi(campaign_id.c_str());
    if (!campaignid) return -1;

    // int autobid = atoi(autob_id.c_str());
    // if (autobid != 1) return -1;

    return load_policy_control_load(conn_setting, conn_counter, root, campaignid, policyid, complete_time);
}

int auto_cost_policys_load_all(redisContext *conn_setting, redisClusterContext *conn_counter, adx_cache_t *root, map<string, string> complete_policyids)
{
    int errcode = E_SUCCESS;

    vector<string> policyids;
    errcode = adx_redis_command(conn_setting, policyids, "SMEMBERS dsp_policyids"); // 全部 policyid
    if (errcode != E_SUCCESS) return errcode;

    // 遍历 policyids
    for (size_t i = 0; i < policyids.size(); i++) {
        int policyid = atoi(policyids[i].c_str());
        int complete_time = 600;

        if (complete_policyids.find(policyids[i]) == complete_policyids.end()) {
            auto_cost_policy_info_load(conn_setting, conn_counter, root, policyid, complete_time);
        }
    }

    // adx_cache_display(root);
    return errcode;
}

int auto_cost_policys_load(redisContext *conn_setting, redisClusterContext *conn_counter, adx_cache_t *root)
{
    int errcode = E_SUCCESS;

    map<string, string> policyids;
    errcode = adx_redis_command(conn_setting, policyids, "HGETALL auto_cost_policy_complete");
    if (errcode == E_SUCCESS) {

        map<string, string>::iterator  iter;
        for(iter = policyids.begin(); iter != policyids.end(); iter++) {

            int policyid = atoi(iter->first.c_str());
            int complete_time = atoi(iter->second.c_str());
            auto_cost_policy_info_load(conn_setting, conn_counter, root, policyid, complete_time);
        }
    }

    adx_redis_command(conn_setting, "DEL auto_cost_policy_complete");
    return auto_cost_policys_load_all(conn_setting, conn_counter, root, policyids);
}

void auto_cost_policy_calculate(redisContext *conn_setting, redisClusterContext *conn_counter, adx_cache_t *node, char *TIME_MONTH_DAY)
{
    int errcode = E_SUCCESS;

    /**********************************
     * 获取计算信息
     **********************************/

    // time_t t = time(NULL);
    int policyid = adx_cache_value_to_number(node->key);
    int campaignid = GET_DUMP_VALUE_INT(node, "campaignid");
    int complete_time = GET_DUMP_VALUE_INT(node, "complete_time");
    int daily_imp_kpi = adx_cache_to_number(adx_cache_find_str(node, "dayimp_%s", TIME_MONTH_DAY));
    int daily_clk_kpi = adx_cache_to_number(adx_cache_find_str(node, "dayclk_%s", TIME_MONTH_DAY));
    int allowtime_day = adx_cache_to_number(adx_cache_find_str(node, "allowtime_%s", TIME_MONTH_DAY));
    if (campaignid <= 0 || complete_time <= 0 || allowtime_day <= 0) // 信息不完整
        return;

    // 格式化参数 (-1 or 0) = 0
    if (daily_imp_kpi <= 0) daily_imp_kpi = 0;
    if (daily_clk_kpi <= 0) daily_clk_kpi = 0;

    // 是否是允许时段
    int allowtime_type = 0;
    if (allowtime_day & (1 << get_time_hour())) allowtime_type = 1;

    // 不允许时段
    if (!allowtime_type) return;

    // 获取今日 展现/点击 数量
    adx_size_t daily_imp_count = 0;
    adx_size_t daily_clk_count = 0;
    errcode = adx_redis_cluster_command(conn_counter, daily_imp_count, "HGET dspcounter_%d d_imp_p_%d_%s", campaignid, policyid, TIME_MONTH_DAY);
    errcode = adx_redis_cluster_command(conn_counter, daily_clk_count, "HGET dspcounter_%d d_clk_p_%d_%s", campaignid, policyid, TIME_MONTH_DAY);
    if (daily_imp_count <= 0 || daily_clk_count <= 0) // 暂无数据(未开始)
        return;

    // 获取上个 10分钟时段
    int this_10min = get_time_hour_split(10);
    this_10min = this_10min == 0 ? 5 : this_10min - 1; // 实际是上个十分钟时段

    // 获取10分钟时段 展现/点击 数量
    adx_size_t this_10min_imp_count = 0;
    adx_size_t this_10min_clk_count = 0;
    errcode = adx_redis_cluster_command(conn_counter, this_10min_imp_count, "HGET dsp10minutecounter_%lu_%d imp_count", policyid, this_10min);
    errcode = adx_redis_cluster_command(conn_counter, this_10min_clk_count, "HGET dsp10minutecounter_%lu_%d clk_count", policyid, this_10min);

    /****************/
    /*** CTR 计算 ***/
    /****************/

    // 计算CTR
    double ctr_kpi = 0;
    double ctr_cur = 0;
    if (daily_imp_kpi >= 0 && daily_imp_count >= 0) {
        ctr_kpi = (double)daily_clk_kpi / (double)daily_imp_kpi;
        ctr_cur = (double)daily_clk_count / (double)daily_imp_count;
    }

    /******************/
    /***探低系数计算***/
    /******************/

    // 计算剩余 小时 数量(下个小时开始)
    int surplus_hour = 0;
    for (int i = get_time_hour() + 1 ; i < 24; i++)
        if (allowtime_day & (1 << i)) surplus_hour++;

    // 计算剩余 10分钟 数量
    int surplus_10min = (surplus_hour * 6) + (6 - get_time_hour_split(10));
    if (surplus_10min <= 0) return;

    // 计算下个 10分钟 KPI
    // double next_10min_imp_kpi = (daily_imp_kpi - daily_imp_count) / surplus_10min
    // double next_10min_clk_kpi = (daily_clk_kpi - daily_clk_count) / surplus_10min
    double next_10min_imp_kpi = adx_except_ceil((daily_imp_kpi - daily_imp_count), surplus_10min);
    double next_10min_clk_kpi = adx_except_ceil((daily_clk_kpi - daily_clk_count), surplus_10min);
    if (next_10min_imp_kpi <= 0 && next_10min_clk_kpi <= 0) // 已完成
        return;

    double coef_cost = 0;
    if (this_10min_imp_count <= 0 || this_10min_clk_count <= 0) { // 暂无数据 200% 出价
        coef_cost = 2;
    }

    // 计算系数
    if (coef_cost == 0) {
        double coef_imp = (double)next_10min_imp_kpi / ((double)this_10min_imp_count / (double)complete_time * 600);
        double coef_clk = (double)next_10min_clk_kpi / ((double)this_10min_clk_count/ (double)complete_time * 600);
        coef_cost = coef_imp > coef_clk ? coef_imp : coef_clk;
    }

    char MONTH_DAY[128];
    get_time_str(6, "_", MONTH_DAY);

    string mess = "policyid=" + intToString(policyid);
    mess += "|complete_time=" + intToString(complete_time);

    mess += "|surplus_10min=" + intToString(surplus_10min);
    mess += "|count_key=auto_cost_bid_" + intToString(policyid) + "_" + string(MONTH_DAY);

    mess += "|daily_imp_kpi=" + intToString(daily_imp_kpi);
    mess += "|daily_imp_count=" + longToString(daily_imp_count);

    mess += "|daily_clk_kpi=" + intToString(daily_clk_kpi);
    mess += "|daily_clk_count=" + longToString(daily_clk_count);

    mess += "|this_10min_imp_count=" + longToString(this_10min_imp_count);
    mess += "|this_10min_clk_count=" + longToString(this_10min_clk_count);

    mess += "|next_10min_imp_kpi=" + longToString(next_10min_imp_kpi);
    mess += "|next_10min_clk_kpi=" + longToString(next_10min_clk_kpi);

    mess += "|ctr_kpi=" + DoubleToString(ctr_kpi);
    mess += "|ctr_cur=" + DoubleToString(ctr_cur);
    mess += "|coef_cost=" + DoubleToString(coef_cost);

    adx_redis_command(conn_setting, "PUBLISH auto_cost_queue %s", mess.c_str());
    adx_log(LOGINFO, "%s", mess.c_str());
}

void auto_cost_policys_for(redisContext *conn_setting, redisClusterContext *conn_counter, adx_cache_t *root)
{
    char TIME_MONTH_DAY[32];
    get_time_str(6, NULL, TIME_MONTH_DAY);

    adx_list_t *p = NULL;
    adx_list_for_each(p, &root->child_list) {
        adx_cache_t *node = adx_list_entry(p, adx_cache_t, next_list);
        auto_cost_policy_calculate(conn_setting, conn_counter, node, TIME_MONTH_DAY);
    }
}

int main()
{

    int errcode = E_SUCCESS;

    auto_cost_conf_t *conf = auto_cost_load_conf();
    if (!conf) return -1;

    // 初始化本地日志
    errcode = adx_log_init(conf->log_path, "auto_cost", conf->log_level);
    if (errcode != E_SUCCESS) return errcode;

    redisContext	*conn_setting	= redisConnect(conf->adx_setting_host, conf->adx_setting_port, conf->adx_setting_pass);
    redisClusterContext	*conn_counter	= redisClusterConnect(conf->adx_counter_host, conf->adx_counter_port, conf->adx_counter_pass);
    if (!conn_setting || !conn_counter || conn_setting->err || conn_counter->err) {
        fprintf(stderr, "redis conn: err\n");
        return -1;
    }

#ifdef __DEBUG__
    adx_cache_t	*root = adx_cache_create();
    auto_cost_policys_load(conn_setting, conn_counter, root);
    auto_cost_policys_for(conn_setting, conn_counter, root);
    adx_cache_free(root);
    sleep(1);
#else
    for (;;) {

        if (auto_bid_check_time() == 0) {

            adx_cache_t	*root = adx_cache_create();
            auto_cost_policys_load(conn_setting, conn_counter, root);
            auto_cost_policys_for(conn_setting, conn_counter, root);
            adx_cache_free(root);
        }

        sleep(1);
    }
#endif

    return 0;
}



