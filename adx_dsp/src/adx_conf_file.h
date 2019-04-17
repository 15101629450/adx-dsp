
#ifndef __ADX_CONF_FILE_H__
#define __ADX_CONF_FILE_H__

#include "adx_type.h"
#include "adx_list.h"

#define GET_CONF_STR(o,s,k) get_adx_conf_file_string(o,s,k)
#define GET_CONF_NUM(o,s,k) get_adx_conf_file_number(o,s,k)

#ifdef __cplusplus
extern "C" { 
#endif

    adx_list_t *adx_conf_file_load(const char *path);
    void adx_conf_file_free();

    char *get_adx_conf_file_string(adx_list_t *cf, const char *section, const char *key);
    int get_adx_conf_file_number(adx_list_t *cf, const char *section, const char *key);

#ifdef __cplusplus
}
#endif

#endif


