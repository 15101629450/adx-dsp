#!/usr/bin/python
# coding=utf-8

import os
import sys
import adx_auto_cost
import psutil
import time
from auto_cost_API import auto_cost, BID

clk_path = '/make/auto_cost-bak/testdata/clkCount_20171212.csv'
cost_path = '/make/auto_cost-bak/testdata/costSum_20171212.csv'
imp_path = '/make/auto_cost-bak/testdata/impCount_20171212.csv'
bid_path = '/make/auto_cost-bak/testdata/requestCount_20171212.csv'
bidprice_path = '/make/auto_cost-bak/testdata/requestPriceSum_20171212.csv'

stamp_dic = {}

def read_file():
    count_result = []
    with open(clk_path) as f:
        for line in f.readlines():
            line = line.replace('\n', '')
            line = line.split(",")
            if len(line) < 6:
                continue
            time_bucket, at, map_id, adx_code, region_code, number = line
            time_bucket = time_bucket[-2]
            print time_bucket
            # if time_bucket not in stamp_dic:
            #     time_ = time.strptime(time_bucket, "%Y-%m-%d %H:%M")
            #     timestamp = time.mktime(time_)
            #     stamp_dic[time_bucket] = str(int(timestamp))
            # time_bucket = stamp_dic[time_bucket]

            policy_id = map_id[-1]
            suffix = '_' + at + '_' + map_id + '_' + adx_code + '_' + region_code
            count_result.append(
                [policy_id, time_bucket, 'clk' + suffix, number])

    with open(cost_path) as f:
        for line in f.readlines():
            line = line.replace('\n', '')
            line = line.split(",")
            if len(line) < 6:
                continue
            time_bucket, at, map_id, adx_code, region_code, number = line
            time_bucket = time_bucket[-2]
            # if time_bucket not in stamp_dic:
            #     time_ = time.strptime(time_bucket, "%Y-%m-%d %H:%M")
            #     timestamp = time.mktime(time_)
            #     stamp_dic[time_bucket] = timestamp
            # time_bucket = stamp_dic[time_bucket]

            policy_id = map_id[-1]
            suffix = '_' + at + '_' + map_id + '_' + adx_code + '_' + region_code
            count_result.append(
                [policy_id, time_bucket, 'cost' + suffix, number])

    with open(imp_path) as f:
        for line in f.readlines():
            line = line.replace('\n', '')
            line = line.split(",")
            if len(line) < 6:
                continue
            time_bucket, at, map_id, adx_code, region_code, number = line
            time_bucket = time_bucket[-2]
            # if time_bucket not in stamp_dic:
            #     time_ = time.strptime(time_bucket, "%Y-%m-%d %H:%M")
            #     timestamp = time.mktime(time_)
            #     stamp_dic[time_bucket] = timestamp
            # time_bucket = stamp_dic[time_bucket]

            policy_id = map_id[-1]
            suffix = '_' + at + '_' + map_id + '_' + adx_code + '_' + region_code
            count_result.append(
                [policy_id, time_bucket, 'imp' + suffix, number])

    with open(bid_path) as f:
        for line in f.readlines():
            line = line.replace('\n', '')
            line = line.split(",")
            if len(line) < 6:
                continue
            time_bucket, at, map_id, adx_code, region_code, number = line
            time_bucket = time_bucket[-2]
            # if time_bucket not in stamp_dic:
            #     time_ = time.strptime(time_bucket, "%Y-%m-%d %H:%M")
            #     timestamp = time.mktime(time_)
            #     stamp_dic[time_bucket] = timestamp
            # time_bucket = stamp_dic[time_bucket]

            policy_id = map_id[-1]
            suffix = '_' + at + '_' + map_id + '_' + adx_code + '_' + region_code
            count_result.append(
                [policy_id, time_bucket, 'bid' + suffix, number])

    with open(bidprice_path) as f:
        for line in f.readlines():
            line = line.replace('\n', '')
            line = line.split(",")
            if len(line) < 6:
                continue
            time_bucket, at, map_id, adx_code, region_code, number = line
            time_bucket = time_bucket[-2]
            # if time_bucket not in stamp_dic:
            #     time_ = time.strptime(time_bucket, "%Y-%m-%d %H:%M")
            #     timestamp = time.mktime(time_)
            #     stamp_dic[time_bucket] = timestamp
            # time_bucket = stamp_dic[time_bucket]
            
            policy_id = map_id[-1]
            suffix = '_' + at + '_' + map_id + '_' + adx_code + '_' + region_code
            count_result.append(
                [policy_id, time_bucket, 'bidprice' + suffix, number])

    info_result = []
    
    return count_result, info_result


obj = auto_cost()
if (obj.check() == False):
    print "auto_cost init: error"
    sys.exit(0)

count_result, info_result = read_file()
print len(count_result)

from tqdm import tqdm
for each in tqdm(count_result):
    if each[3] is not '':
        obj.forge_count(*each)

# for index, line in enumerate(count_result):
#     if index % 2000 = 0:
#         print index
#     if 'imp_' in line[2]:
#         if int(line[3]) > max_imp:
#             max_imp = int(line[3])
#             max_index = index
#     if not obj.forge_count(line[0], line[1], line[2], line[3]):
#         print line
# print info_result[0]
#
# print count_result[index]

# for line in info_result:
#     if not obj.mess_push(line):
#         print line
