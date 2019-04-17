
#ifndef __ADX_DUMP_H__
#define __ADX_DUMP_H__

#include "adx_type.h"
#include "adx_list.h"
#include "adx_string.h"
#include "adx_json.h"
#include "adx_redis.h"
#include "adx_cache.h"

#define ADX_SIZE_IMG   1
#define ADX_SIZE_ICON  2
#define ADX_SIZE_INFO  3

int adx_dump_main(adx_list_t *adx_conf);
adx_cache_t *get_cache_root();

#endif


