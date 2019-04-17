
#ifndef __ADX_TYPE_H__
#define __ADX_TYPE_H__

// #define __DEBUG__
// #define __JEMALLOC__

#ifdef __JEMALLOC__
#include <jemalloc/jemalloc.h>
#else
#include <stdlib.h>
#define je_malloc malloc
#define je_free free
#endif

#endif


