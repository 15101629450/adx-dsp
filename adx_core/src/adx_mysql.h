
#ifndef __ADX_MYSQL_H__
#define __ADX_MYSQL_H__

#include <mysql/mysql.h>

#ifdef __cplusplus
extern "C" { 
#endif

#define PARAM_SIZE 128
#define STRING_SIZE 4096

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
    typedef void (adx_mysql_select_t)(int col_len, char **col_name, int *col_type, char **data, void *p);
    int adx_mysql_select(adx_mysql_t *adx_mysql, char *sql, adx_mysql_select_t *call, void *p);

#ifdef __cplusplus
}
#endif

#endif




