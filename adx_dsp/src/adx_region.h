
#ifndef __ADX_REGION_H__
#define __ADX_REGION_H__

#include <iostream>

using namespace std;

size_t openIPB(const char *fname);
void closeIPB(size_t addr);

int getRegionCode(size_t addr, char *IPstr);
int getRegionCode(size_t addr, string &IPstr);

#endif


