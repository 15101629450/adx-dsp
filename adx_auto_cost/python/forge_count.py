#!/usr/bin/python
#coding=utf-8

import os
import sys
from auto_cost_API import auto_cost, BID

obj = auto_cost()
if (obj.check() == False) :
    print obj.error()
    sys.exit(0)

# 参数描述 policyid, timebucket, key, val  (key=cost_(at)_(mapid)_(adxcode)_(regioncode))
#	redis[cost_1_1023_8_4421] = 765.24
#       redis[_2_6666_11_4401]

print obj.forge_count("3", "0", "aa", "11")
print obj.forge_count("3", "2", "bb", "22")
print obj.forge_count("3", "5", "cc", "33")



