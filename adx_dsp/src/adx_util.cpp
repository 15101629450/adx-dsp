
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "adx_util.h"

using namespace std;

int replaceMacro_strtok(string &left, string &right, string delim)
{
    string str = right;
    int size = str.find(delim);
    if (size < 0) return -1;

    left = str.substr(0, size);
    right = str.substr(size + delim.size(), str.size());
    return 0;
}

string replaceMacro(string buf, string src, string dest)
{
    string res;
    string left;
    string right = buf;
    for (;;) {

        if (replaceMacro_strtok(left, right, src)) {

            res += right;
            return res;
        }

        res += left;
        res += dest;
    }
}

string HexToString(int arg)
{
    char num[64];
    snprintf(num, 64, "0x%X", arg);
    return string(num);
}

string DoubleToString(double arg)
{
    char num[64];
    snprintf(num, 64, "%.2f", arg);
    return string(num);
}

string intToString(int arg)
{
    char num[64];
    snprintf(num, 64, "%d", arg);
    return string(num);
}

string longToString(long arg)
{
    char num[64];
    snprintf(num, 64, "%ld", arg);
    return string(num);
}

char *StrDup(adx_pool_t *pool, string s)
{
    return adx_strdup(pool, s.c_str(), s.length());
}






