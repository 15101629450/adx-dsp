
#include "pv.h"
#include "adx_region.h"
#include "adx_mysql.h"
#include "adx_log.h"

#define GET_URI_VALUE_STR(o) adx_uri_get_string(&URI, o)
#define GET_URI_VALUE_NUM(o) adx_uri_get_number(&URI, o)

static int old_mday = 0;
static adx_mysql_t *mysql_pv = NULL;
static pthread_mutex_t mysql_pv_mutex = PTHREAD_MUTEX_INITIALIZER;
adx_mysql_t *mysql_pv_conn(char *host, char *base, char *user, char *pass)
{
    struct tm tm;
    time_t t = time(NULL);
    localtime_r(&t, &tm);

    adx_mysql_t *adx_mysql = adx_mysql_init(host, base, user, pass);
    if (!adx_mysql) return NULL;

    char sql[4096];
    int len = sprintf(sql, 
	    "CREATE TABLE `tb_data_pv_%04d%02d%02d` ("
	    "  `time` datetime DEFAULT NULL COMMENT '时间',"

	    "  `id` varchar(64) DEFAULT NULL COMMENT '落地页id',"
	    "  `ty` int(11) DEFAULT NULL COMMENT '请求的类型:1,访问 2,跳出 3,二跳',"
	    "  `bid` varchar(64) DEFAULT NULL COMMENT 'bid',"
	    "  `mid` varchar(64) DEFAULT NULL COMMENT '创意id',"
	    "  `datasrc` varchar(32) DEFAULT NULL COMMENT '数据来源平台:dsp/pap',"

	    "  `os` int(11) DEFAULT NULL COMMENT '操作系统',"
	    "  `ds` varchar(64) DEFAULT NULL COMMENT '移动设备分别率',"

	    "  `tp` int(11) DEFAULT NULL COMMENT '移动类型',"
	    "  `op` int(11) DEFAULT NULL COMMENT '运营商',"
	    "  `nw` int(11) DEFAULT NULL COMMENT '联网类型',"
	    "  `mb` int(11) DEFAULT NULL COMMENT '设备型号',"

	    "  `pu` varchar(1024) DEFAULT NULL COMMENT '上游连接',"
	    "  `cl` varchar(1024) DEFAULT NULL COMMENT '受访页面',"

	    "  `pv` int(11) DEFAULT NULL COMMENT '展现数',"
	    "  `uv` int(11) DEFAULT NULL COMMENT '用户数',"

	    "  `hf` varchar(1024) DEFAULT NULL COMMENT '二跳的URL',"
	    "  `at` int(11) DEFAULT NULL COMMENT '平均访问时长',"

	    "  `ip` varchar(32) DEFAULT NULL COMMENT 'ip',"
	    "  `region` int(11) DEFAULT NULL COMMENT '地域编码'"
	    ") CHARSET=utf8;",

	tm.tm_year + 1900, 
	tm.tm_mon + 1, 
	tm.tm_mday
	    );
    mysql_real_query(adx_mysql->conn, sql, len);
    // fprintf(stdout, "%s\n", sql);

    sprintf(sql, 
	    "INSERT INTO `tb_data_pv_%04d%02d%02d` VALUES (%s"
	    ",?,?,?,?,?"
	    ",?,?,?,?,?"
	    ",?,?,?,?,?"
	    ",?,?,?,?"
	    ");",

	    tm.tm_year + 1900, 
	    tm.tm_mon + 1, 
	    tm.tm_mday,
	    "STR_TO_DATE(DATE_FORMAT(now(), '%Y-%m-%d %H:00:00'), '%Y-%m-%d %H:%i:%s')"
	   );

    // fprintf(stdout, "%s\n", sql);
    fprintf(stdout, "new table : tb_data_pv_%04d%02d%02d\n",
	    tm.tm_year + 1900,
	    tm.tm_mon + 1,
	    tm.tm_mday
	   );

    int ret = adx_mysq_bind_sql(adx_mysql, sql);
    if (ret != 0) {
	adx_mysql_close(adx_mysql);
	return NULL;
    }

    old_mday = tm.tm_mday;
    return adx_mysql;
}

int mysql_pv_init(char *host, char *base, char *user, char *pass)
{
    // fprintf(stdout, "==>[%s][%s][%s][%s]\n", host, base, user, pass);
    adx_mysql_t *adx_mysql = mysql_pv_conn(host, base, user, pass);
    if (!adx_mysql) {
	fprintf(stderr, "mysql conn : err\n");
	return -1;
    }

    mysql_pv = adx_mysql;
    return 0;
}

void mysql_pv_re_init(char *host, char *base, char *user, char *pass)
{
    // old
    adx_mysql_t *old_mysql = mysql_pv;

    // new
    pthread_mutex_lock(&mysql_pv_mutex);
    mysql_pv = mysql_pv_conn(host, base, user, pass);
    pthread_mutex_unlock(&mysql_pv_mutex);

    // free old
    sleep(30);
    adx_mysql_close(old_mysql);
}

void mysql_pv_loop(char *host, char *base, char *user, char *pass)
{
    struct tm tm;
    time_t t = time(NULL);
    localtime_r(&t, &tm);

    if (old_mday != tm.tm_mday) mysql_pv_re_init(host, base, user, pass);
}

int parseRequest(adx_pool_t *pool, size_t ipdb, char *requestaddr, char *requestparam)
{
    adx_list_t URI;
    adx_uri_parse(pool, &URI, requestparam);
    // adx_uri_display(&URI);

    char *id		= GET_URI_VALUE_STR( "id");// 落地页id
    int ty		= GET_URI_VALUE_NUM( "ty");//请求的类型：1、访问(FIRST_JUMP)；2、跳出(OUT_JUMP)；3、二跳(TWICE_JUMP)
    char *bid		= GET_URI_VALUE_STR( "bid");//bid
    char *mid		= GET_URI_VALUE_STR( "mid");//创意id
    char *datasrc	= GET_URI_VALUE_STR( "datasrc");//数据来源平台 目前仅 dsp 和 pap 平台, NULL 的都是 dsp 平台

    int os		= GET_URI_VALUE_NUM( "sys");//操作系统
    char *ds		= GET_URI_VALUE_STR( "ds");//移动设备分别率

    int tp		= GET_URI_VALUE_NUM( "tp");//移动类型(false)
    int op		= GET_URI_VALUE_NUM( "op");//运营商(false)
    int nw		= GET_URI_VALUE_NUM( "nw");//联网类型(false)
    int mb		= GET_URI_VALUE_NUM( "mb");//设备型号

    char*pu		= GET_URI_VALUE_STR( "pu");//上游连接(访问true)
    char *cl		= GET_URI_VALUE_STR( "cl");//受访页面

    int pv		= GET_URI_VALUE_NUM( "pv");//展现数
    int uv		= GET_URI_VALUE_NUM( "uv");//用户数

    char *hf		= GET_URI_VALUE_STR( "hf");//二跳的URL
    int at		= GET_URI_VALUE_NUM( "at");//平均访问时长

    char *ip		= requestaddr;
    int region		= getRegionCode(ipdb, requestaddr);


    if (!id) return E_INVALID_PARAM;
    if (ty <= 0 || ty >= 4) return E_PV_INVALID_TY;
    if (!os || !ds) return E_PV_INVALID_SYSDS;
    if(ty == 1 && !cl) return E_PV_INVALID_CL;
    
    if (!datasrc) datasrc = (char *)"dsp";

    pthread_mutex_lock(&mysql_pv_mutex);

    if (mysql_pv) {
	adx_mysq_bind_param(mysql_pv,	0, id);
	adx_mysq_bind_param_r(mysql_pv,	1, "%d", ty);
	adx_mysq_bind_param(mysql_pv,	2, bid);
	adx_mysq_bind_param(mysql_pv,	3, mid);
	adx_mysq_bind_param(mysql_pv,	4, datasrc);
	adx_mysq_bind_param_r(mysql_pv,	5, "%d", os);
	adx_mysq_bind_param(mysql_pv,	6, ds);
	adx_mysq_bind_param_r(mysql_pv,	7, "%d", tp);
	adx_mysq_bind_param_r(mysql_pv,	8, "%d", op);
	adx_mysq_bind_param_r(mysql_pv,	9, "%d", nw);
	adx_mysq_bind_param_r(mysql_pv,	10, "%d", mb);
	adx_mysq_bind_param(mysql_pv,	11, pu);
	adx_mysq_bind_param(mysql_pv,	12, cl);
	adx_mysq_bind_param_r(mysql_pv,	13, "%d", pv);
	adx_mysq_bind_param_r(mysql_pv,	14, "%d", uv);
	adx_mysq_bind_param(mysql_pv,	15, hf);
	adx_mysq_bind_param_r(mysql_pv,	16, "%d", at);
	adx_mysq_bind_param(mysql_pv,	17, ip);
	adx_mysq_bind_param_r(mysql_pv, 18, "%u", region);

	adx_mysq_bind_execute(mysql_pv);
    }

    pthread_mutex_unlock(&mysql_pv_mutex);

    adx_log(LOGINFO,
	    "id=%s"
	    "|ty=%d"
	    "|bid=%s"
	    "|mid=%s"
	    "|datasrc=%s"
	    "|os=%d"
	    "|ds=%s"
	    "|tp=%d"
	    "|op=%d"
	    "|nw=%d"
	    "|mb=%d"
	    "|pu=%s"
	    "|cl=%s"
	    "|pv=%d"
	    "|uv=%d"
	    "|hf=%s"
	    "|at=%d"
	    "|ip=%s"
	    "|region=%d",
	    id ,
	    ty ,
	    bid ,
	    mid ,
	    datasrc ,
	    os ,
	    ds ,
	    tp ,
	    op ,
	    nw ,
	    mb ,
	    pu ,
	    cl ,
	    pv ,
	    uv ,
	    hf ,
	    at ,
	    ip ,
	    region 
		);

    return 0;
}






