
#ifndef __ADX_MYSQL_H__
#define __ADX_MYSQL_H__

#include <mysql/mysql.h>

#define PARAM_SIZE 32
#define STRING_SIZE 1024

#ifdef __cplusplus
extern "C" { 
#endif

    typedef struct {

        const char	*host;
        const char	*base;
        const char	*user;
        const char	*pass;

        MYSQL 		*conn;
        MYSQL_STMT	*stmt;

        MYSQL_BIND	bind[PARAM_SIZE];
        char		buffer[PARAM_SIZE][STRING_SIZE];
        size_t		length[PARAM_SIZE];

    } adx_mysql_t;

    adx_mysql_t *adx_mysql_init(const char *host, const char *base, const char *user, const char *pass);
    void adx_mysql_close(adx_mysql_t *adx_mysql);

    // INSERT
    int adx_mysq_bind_sql(adx_mysql_t *adx_mysql, char *sql);
    void adx_mysq_bind_param(adx_mysql_t *adx_mysql, int index, const char *buffer);
    void adx_mysq_bind_param_r(adx_mysql_t *adx_mysql, int index, const char *format, ...);
    int adx_mysq_bind_execute(adx_mysql_t *adx_mysql);

    // SELECT
    typedef void (adx_mysql_select_t)(int len, int *type, char **field_name, char **data, void *p);
    int adx_mysql_select(adx_mysql_t *adx_mysql, char *sql, adx_mysql_select_t *call, void *p);

    int lg_mysql_select_r(char *host, char *base, char *user, char *pass, char *sql, adx_mysql_select_t *call, void *p);
    int lg_mysql_load_table(char *host, char *base, char *user, char *pass, char *table, adx_mysql_select_t *call, void *p);

#ifdef __cplusplus
}
#endif

#endif


