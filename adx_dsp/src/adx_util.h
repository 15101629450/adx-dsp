
#ifndef __ADX_UTIL_H__
#define __ADX_UTIL_H__

#include <iostream>
#include "adx_string.h"

using namespace std;

string replaceMacro(string buf, string src, string dest);
string HexToString(int arg);
string DoubleToString(double arg);
string intToString(int arg);
string longToString(long arg);

char *StrDup(adx_pool_t *pool, string str);

#endif


