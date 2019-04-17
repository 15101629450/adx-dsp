
#ifndef DSP_URLMONITOR_H_
#define DSP_URLMONITOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcgiapp.h>
#include <fcgi_config.h>

#include "adx_uri.h"
#include "adx_alloc.h"
#include "errorcode.h"

#define IN
#define OUT
#define INOUT
#define	FIRST_JUMP 1
#define OUT_JUMP 2
#define TWICE_JUMP 3

typedef struct {

    char *mysql_host;
    char *mysql_base;
    char *mysql_user;
    char *mysql_pass;

    int log_level;
    char *log_path;

} pv_conf_t;;

int mysql_pv_init(char *host, char *base, char *user, char *pass);
void mysql_pv_loop(char *host, char *base, char *user, char *pass);
int parseRequest(adx_pool_t *pool, size_t ipdb, char *requestaddr, char *requestparam);

#endif


