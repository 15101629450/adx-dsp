
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "adx_mysql.h"

static void *adx_mysql_error(MYSQL *conn, MYSQL_STMT *stmt)
{
    if (conn) mysql_close(conn);
    if (stmt) mysql_stmt_close(stmt);
    return NULL;
}

void adx_mysq_bind_buffer(MYSQL_BIND *bind, char *buffer, size_t *length)
{
    bind->buffer_type = MYSQL_TYPE_STRING;
    // bind->buffer_type = length ? MYSQL_TYPE_STRING : MYSQL_TYPE_LONG;
    bind->buffer = buffer;
    bind->length = length;
    bind->is_null = NULL;
}

adx_mysql_t *adx_mysql_init(const char *host, const char *base, const char *user, const char *pass)
{
    MYSQL *conn = mysql_init(NULL);
    if (!conn) return NULL;

    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) return adx_mysql_error(conn, NULL);

    if(!mysql_real_connect(conn, host, user, pass, base, 0, 0, 0))
        return adx_mysql_error(conn, stmt);

    if (mysql_set_character_set(conn, "utf8"))
        return adx_mysql_error(conn, stmt);

    adx_mysql_t *adx_mysql = malloc(sizeof(adx_mysql_t));
    if (!adx_mysql) return adx_mysql_error(conn, stmt);
    memset(adx_mysql, 0, sizeof(adx_mysql));

    adx_mysql->host = host;
    adx_mysql->base = base;
    adx_mysql->user = user;
    adx_mysql->pass = pass;
    adx_mysql->conn = conn;
    adx_mysql->stmt = stmt;

    int i; // 绑定数据结构
    for (i = 0; i < PARAM_SIZE; i++)
        adx_mysq_bind_buffer(&adx_mysql->bind[i], adx_mysql->buffer[i], &adx_mysql->length[i]);

    return adx_mysql;
}

void adx_mysql_close(adx_mysql_t *adx_mysql)
{
    if (!adx_mysql) return;
    if (adx_mysql->conn) mysql_close(adx_mysql->conn);
    if (adx_mysql->stmt) mysql_stmt_close(adx_mysql->stmt);
    free(adx_mysql);
}

int adx_mysq_bind_sql(adx_mysql_t *adx_mysql, char *sql)
{
    // 分析SQL
    if (mysql_stmt_prepare(adx_mysql->stmt, sql, strlen(sql)))
        return -1;

    // 参数数量不正确
    int param_count = mysql_stmt_param_count(adx_mysql->stmt);
    if (param_count <= 0 || param_count >= PARAM_SIZE) return -1;

    // 绑定参数
    if (mysql_stmt_bind_param(adx_mysql->stmt, adx_mysql->bind))
        return -1;

    return 0;
}

void adx_mysq_bind_param(adx_mysql_t *adx_mysql, int index, const char *buffer)
{
    if (!adx_mysql) return;
    if (!buffer) {
        adx_mysql->length[index] = 0;
        return;
    }

    int len = strlen(buffer);
    if (len <= 0 || len >= STRING_SIZE) {
        adx_mysql->length[index] = 0;
        return;
    }

    adx_mysql->length[index] = len;
    strcpy(adx_mysql->buffer[index], buffer);
}

void adx_mysq_bind_param_r(adx_mysql_t *adx_mysql, int index, const char *format, ...)
{
    if (!adx_mysql) return;
    int param_count = mysql_stmt_param_count(adx_mysql->stmt);
    if (param_count <= 0) return;
    if (index >= param_count) return;

    if (!format) {
        adx_mysql->length[index] = 0;
        return;
    }

    va_list ap;
    va_start(ap, format);
    adx_mysql->length[index] = vsnprintf(adx_mysql->buffer[index], STRING_SIZE - 1, format, ap);
    if (adx_mysql->length[index] >= STRING_SIZE - 1) {
        adx_mysql->length[index] = 0;
        return;
    }

    va_end(ap);
}

int adx_mysq_bind_execute(adx_mysql_t *adx_mysql)
{
    if (!adx_mysql) return -1;
    return mysql_stmt_execute(adx_mysql->stmt);
}

int adx_mysql_select(adx_mysql_t *adx_mysql, char *sql, adx_mysql_select_t *call, void *p)
{
    int ret = mysql_real_query(adx_mysql->conn, sql, strlen(sql));
    if (ret != 0) return -1;

    MYSQL_RES *res = mysql_store_result(adx_mysql->conn);
    if (!res) return -1;

    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    if (!fields) {
        mysql_free_result(res);
        return -1;
    }

    // int row_len = mysql_num_rows(res);
    int i, field_len = mysql_num_fields(res);
    int type[field_len];
    char *field_name[field_len];

    for (i = 0; i < field_len; i++) {
        type[i] = fields[i].type;
        field_name[i] = fields[i].name;
    }

    MYSQL_ROW data;
    while ((data = mysql_fetch_row(res))) {
        call(field_len, type, field_name, data, p);
    }

    mysql_free_result(res);
    return 0;
}

void lg_mysql_type_print(void)
{
    fprintf(stdout, "MYSQL_TYPE_DECIMAL=%d\n",		MYSQL_TYPE_DECIMAL);
    fprintf(stdout, "MYSQL_TYPE_NEWDECIMAL=%d\n",           MYSQL_TYPE_NEWDECIMAL);
    fprintf(stdout, "MYSQL_TYPE_TINY=%d\n",                 MYSQL_TYPE_TINY);
    fprintf(stdout, "MYSQL_TYPE_SHORT=%d\n",                MYSQL_TYPE_SHORT);
    fprintf(stdout, "MYSQL_TYPE_LONG=%d\n",                 MYSQL_TYPE_LONG);
    fprintf(stdout, "MYSQL_TYPE_FLOAT=%d\n",                MYSQL_TYPE_FLOAT);
    fprintf(stdout, "MYSQL_TYPE_DOUBLE=%d\n",               MYSQL_TYPE_DOUBLE);
    fprintf(stdout, "MYSQL_TYPE_NULL=%d\n",                 MYSQL_TYPE_NULL);
    fprintf(stdout, "MYSQL_TYPE_TIMESTAMP=%d\n",            MYSQL_TYPE_TIMESTAMP);
    fprintf(stdout, "MYSQL_TYPE_LONGLONG=%d\n",             MYSQL_TYPE_LONGLONG);
    fprintf(stdout, "MYSQL_TYPE_INT24=%d\n",                MYSQL_TYPE_INT24);
    fprintf(stdout, "MYSQL_TYPE_DATE=%d\n",                 MYSQL_TYPE_DATE);
    fprintf(stdout, "MYSQL_TYPE_TIME=%d\n",                 MYSQL_TYPE_TIME);
    fprintf(stdout, "MYSQL_TYPE_DATETIME=%d\n",             MYSQL_TYPE_DATETIME);
    fprintf(stdout, "MYSQL_TYPE_YEAR=%d\n",                 MYSQL_TYPE_YEAR);
    fprintf(stdout, "MYSQL_TYPE_NEWDATE=%d\n",              MYSQL_TYPE_NEWDATE);
    fprintf(stdout, "MYSQL_TYPE_ENUM=%d\n",                 MYSQL_TYPE_ENUM);
    fprintf(stdout, "MYSQL_TYPE_SET=%d\n",                  MYSQL_TYPE_SET);
    fprintf(stdout, "MYSQL_TYPE_TINY_BLOB=%d\n",            MYSQL_TYPE_TINY_BLOB);
    fprintf(stdout, "MYSQL_TYPE_MEDIUM_BLOB=%d\n",          MYSQL_TYPE_MEDIUM_BLOB);
    fprintf(stdout, "MYSQL_TYPE_LONG_BLOB=%d\n",            MYSQL_TYPE_LONG_BLOB);
    fprintf(stdout, "MYSQL_TYPE_BLOB=%d\n",                 MYSQL_TYPE_BLOB);
    fprintf(stdout, "MYSQL_TYPE_VAR_STRING=%d\n",           MYSQL_TYPE_VAR_STRING);
    fprintf(stdout, "MYSQL_TYPE_STRING=%d\n",               MYSQL_TYPE_STRING);
    fprintf(stdout, "MYSQL_TYPE_TINY=%d\n",                 MYSQL_TYPE_TINY);
    fprintf(stdout, "MYSQL_TYPE_ENUM=%d\n",                 MYSQL_TYPE_ENUM);
    fprintf(stdout, "MYSQL_TYPE_GEOMETRY=%d\n",             MYSQL_TYPE_GEOMETRY);
    fprintf(stdout, "MYSQL_TYPE_BIT=%d\n",                  MYSQL_TYPE_BIT);
}

#if 0
#define INSERT_DEMO "INSERT INTO t_demo VALUES(curdate(),?,?)"
void (demo)(int col_len, char **col_name, int *col_type, char **data, void *p)
{
    int i;
    for (i = 1; i < col_len; i++) {
        fprintf(stdout, "[%d][%s:%s]\n", col_type[i], col_name[i], data[i]);
    }
}

int main()
{
    adx_mysql_t *adx_mysql = adx_mysql_init("127.0.0.1", "pap-manager", "root", "lcglcg");
    fprintf(stdout, "adx_mysql=%p\n", adx_mysql);

    int ret = adx_mysq_bind_sql(adx_mysql, INSERT_DEMO);
    fprintf(stdout, "adx_mysq_bind_sql=%d\n", ret);

    char *str = "http%3A%2F%2Fwww.jplushome.com%2FActivity%2Fm%2F14%3Fhmsr%3Dpengjing%26hmpl%3Dgdt%26bid%3D7d7e9870-bcb7-4f95-8b9b-26c9cbed5cbc-1516787138469%26mapid%3Dfc37c16b-ecc5-4447-94aa-6fd2dc43ce09%26gp%3D1156310000%26op%3D1%26nw%3D5%26mb%3D4%26tp%3D1";

    int i;
    for (i = 0; i < 10; i++) {
        adx_mysq_bind_param(adx_mysql, 0, NULL);
        adx_mysq_bind_param(adx_mysql, 1, str);
        adx_mysq_bind_execute(adx_mysql);
    }

    // adx_mysql_select(adx_mysql, "select * from t_demo", demo, NULL);
    adx_mysql_close(adx_mysql);
    return 0;
}
#endif

int lg_mysql_select_r(char *host, char *base, char *user, char *pass, char *sql, adx_mysql_select_t *call, void *p)
{

    MYSQL *conn = mysql_init(NULL);
    if (!conn) return -1;

    if(!mysql_real_connect(conn, host, user, pass, base, 0, 0, 0)) {
        mysql_close(conn);
        return -1;
    }

    if (mysql_set_character_set( conn, "utf8")) {
        mysql_close(conn);
        return -1;
    }   

    int ret = mysql_real_query(conn, sql, strlen(sql));
    if (ret != 0) {
        mysql_close(conn);
        return -1;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) {
        mysql_close(conn);
        return -1;
    }

    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    if (!fields) {
        mysql_free_result(res);
        mysql_close(conn);
        return -1;
    }

    int i, field_len = mysql_num_fields(res);
    int type[field_len];
    char *field_name[field_len];
    for (i = 0; i < field_len; i++) {
        type[i] = fields[i].type;
        field_name[i] = fields[i].name;
    }

    MYSQL_ROW data;
    while ((data = mysql_fetch_row(res))) {
        call(field_len, type, field_name, data, p);
    }

    mysql_free_result(res);
    mysql_close(conn);
    return 0;
}

int lg_mysql_load_table(char *host, char *base, char *user, char *pass, char *table, adx_mysql_select_t *call, void *p)
{
    char sql[1024];
    sprintf(sql, "select * from %s", table);
    return lg_mysql_select_r(host, base, user, pass, sql, call, p);
}


#if 0
static char *host = "192.168.30.30";
static char *base = "web_site";
static char *user = "root";
static char *pass = "lcglcg";

void (demo)(int len, int *type, char **field_name, char **data, void *p)
{

    int i;
    for (i = 0; i < len; i++) {

        fprintf(stdout, "[%d][%s][%s]\n", type[i], field_name[i], data[i]);

    }

    fprintf(stdout, "================================================\n");
}

int main()
{

    char *sql = "select * from t_keyword";
    lg_mysql_select_r(host, base, user, pass, sql, demo, NULL);

    return 0;
}
#endif




