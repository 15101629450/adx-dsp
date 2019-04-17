#!/usr/bin/python
#coding=utf-8
"""

调价逻辑：
    每次调整 PRICE_RATE * region[regioncode] 这么多价格

展现不够，加价: tune_up
展现多了，减价: tune_down

不要求展现的，tune_ctr
"""

import os
import random
import sys
import logging
# 改价力度系数，由历史数据训练得到
from auto_cost_API import auto_cost, BID

CTR_ERROR = 0.2  ## ctr 相对误差
PRICE_RATE = 0.15  # 价格调整力度基准
MAXRANGE = 0.33  # 最大变化正负比例
MINI_NUM = 20  # 改价的最小流量
MAXPRICE = 999  # 最大价格限制
MINPRICE = 50  # 最小价格限制
region = {}
with open('/etc/dspads_odin/result_region.dict') as f:
    f.readline()
    for line in f.readlines():
        line = line.replace('\n', '')
        line = line.split('\t')
        region[line[0]] = float(line[1])

obj = auto_cost()
if (obj.check() == False):
    print(obj.error())
    sys.exit(0)

logging.basicConfig(
    format=
    '%(asctime)s %(filename)s[line:%(lineno)d] %(levelname)s %(message)s',
    level=logging.DEBUG,
    datefmt='%d %b %Y %H:%M:%S',
    filename=obj.get_conf())


def check_state(info):

    try:
        this_10min_imp_kpi = float(info["this_10min_imp_kpi"])
        this_10min_clk_kpi = float(info["this_10min_clk_kpi"])
        this_10min_imp_count = float(info["this_10min_imp_count"])
        this_10min_clk_count = float(info["this_10min_clk_count"])
        next_10min_imp_kpi = float(info["next_10min_imp_kpi"])
        next_10min_clk_kpi = float(info["next_10min_clk_kpi"])
    except:
        return "no"
    if (next_10min_clk_kpi <= 0) and (next_10min_imp_kpi <= 0):
        return 'no'

    if next_10min_clk_kpi <= 0:
        if this_10min_imp_count < next_10min_imp_kpi:
            return 'up'
        else:
            return 'down'
    if next_10min_imp_kpi <= 0:
        if this_10min_clk_count < next_10min_clk_kpi:
            return 'up'
        else:
            return 'down'

    if (this_10min_imp_count < next_10min_imp_kpi) and (this_10min_clk_count < next_10min_clk_kpi):
        return 'up'
    if (this_10min_imp_count > next_10min_imp_kpi) and (this_10min_clk_count > next_10min_clk_kpi):
        return 'down'

    if (this_10min_imp_count > next_10min_imp_kpi) and (this_10min_clk_count < next_10min_clk_kpi):
        # 买点击率高的
        return 'uphighctr'
    if (this_10min_imp_count < next_10min_imp_kpi) and (this_10min_clk_count > next_10min_clk_kpi):
        # 买便宜的
        return 'upcheap'


def write_price(bid, channel_list, price_list):
    """改价格写到redis
    """
    for (at, mapid, adxcode, regioncode), price in zip(channel_list,
                                                       price_list):
        valid_price = min(price, MAXPRICE)
        valid_price = max(valid_price, MINPRICE)
        bid.write(mapid, adxcode, 'all', regioncode, str(valid_price))


def tune(direction, bid, count, ctr_all):
    """改价格
    down:           - PRICE_RATE * region[regioncode]
    up:             + PRICE_RATE * region[regioncode]
    no:               0
    random:         - PRICE_RATE * region[regioncode] * (random.random() - 0.5)
                    给予价格的随机扰动
    """
    if direction == 'no':
        return

    # 首先计算平均价格， 当点击够了，展现不够的时候，多买低于平均价格的流量
    bidprice_all = 0
    bid_all = 0
    for key in count.keys():
        if 'bidprice' in key:
            try:
                bid_key = key.replace('bidprice_', 'bid_')
                bidprice_all += float(count[key])
                bid_all += float(count[bid_key])
            except:
                pass
    if bid_all <= 0.1:
        return
    average_price = bidprice_all / (bid_all + 0.1)

    price_list = []
    key_list = []

    for key in count.keys():
        if 'bidprice' not in key:
            continue
        try:

            key_bid = key.replace('bidprice_', 'bid_')
            key_imp = key.replace('bidprice_', 'imp_')
            # 没有展现的流量，不加价格
            if key_imp not in count:
                continue
            cur_price = float(count[key]) / (float(count[key_bid]) + 0.1)
            a = key.split('_')[1:]
            if a[3][4:] in region:
                coef = region[a[3][4:]] * PRICE_RATE
            else:
                coef = PRICE_RATE
            if coef > MAXRANGE:
                coef = MAXRANGE
            new_price = cur_price
            if direction == 'up':
                new_price = cur_price * (1 + coef)
            elif direction == 'down':
                new_price = cur_price * (1 - coef)
            elif direction == 'upcheap':
                if cur_price < average_price:
                    new_price = cur_price * (1 + coef)
            elif direction == 'uphighctr':
                try:
                    key_clk = key.replace('bidprice_', 'clk_')
                    ctr_cur = float(count[key_clk]) / (
                        float(count[key_imp]) + 0.1)
                except:
                    ctr_cur = 0
                if ctr_cur > ctr_all:
                    new_price = cur_price * (1 + coef)
            key_list.append(tuple(key.replace('bidprice_', '').split('_')))
            price_list.append(new_price)
            logging.info(direction + str(a) + count[key_imp] + ":" +
                         str(cur_price) + '-->' + str(new_price))
            print(direction + str(a) + count[key_imp] + ":" + str(cur_price) +
                  '-->' + str(new_price))
        except Exception, e:
            print e
    write_price(bid, key_list, price_list)


def auto_cost_loop():
    # 阻塞等待消息
    mess = obj.wait()
    # print mess
    info = mess['INFO']
    count = mess['COUNT']
    infostr = "clk:%s\t\tcount:%s\n" % (info['this_10min_clk_kpi'],
                                        info['this_10min_clk_count'])
    infostr += "imp:%s\t\tcount:%s\n" % (info['this_10min_imp_kpi'],
                                         info['this_10min_imp_count'])
    infostr += "nclk:%s\t\tnimp:%s\n" % (info['next_10min_clk_kpi'],
                                         info['next_10min_imp_kpi'])
    logging.info('RECEIVED:INFO' + infostr)

    ################
    # 数据结构描述 #
    ################
    # info = mess["INFO"]
    # count = mess["COUNT"]
    #
    # info[time]                 = 本次消息生产时间
    # info[policyid]             = 要计算的投放策略ID
    # info[surplus_10min]        = 剩余10分钟时段数
    #
    # info[daily_imp_kpi]        = 当日要完成的展示KPI
    # info[daily_clk_kpi]        = 当日要完成的点击KPI
    #
    # info[daily_imp_count]      = 当日已完成的展示量
    # info[daily_clk_count]      = 当日已完成的点击量
    #
    # info[this_10min_imp_kpi]   = 当前十分钟展现KPI
    # info[this_10min_clk_kpi]   = 当前十分钟点击KPI
    #
    # info[this_10min_imp_count] = 当前十分钟展现量
    # info[this_10min_clk_count] = 当前十分钟点击量
    #
    # info[next_10min_imp_kpi]   = 下个十分钟展现KPI
    # info[next_10min_clk_kpi]   = 下个十分钟点击KPI
    #
    # count[cost_(at)_(mapid)_(adxcode)_(regioncode)]     = 10分钟时段的投放成本
    # count[imp_(at)_(mapid)_(adxcode)_(regioncode)]      = 10分钟时段内展示计数
    # count[clk_(at)_(mapid)_(adxcode)_(regioncode)]      = 10分钟时段内点击计数
    # count[bid_(at)_(mapid)_(adxcode)_(regioncode)]      = 当前10分钟时段出价次数
    # count[bidprice_(at)_(mapid)_(adxcode)_(regioncode)] = 当前10分钟时段出价累计金额
    # ...

    # 字典中可能会有''字符串，设为1
    for each in count:
        if count[each] is '':
            count[each] = 1

    total_sum = sum([int(count[each]) for each in count if 'imp' in each])
    print('====', total_sum)

    # 过滤掉所有展现数目小于200的channel，这部分不处理
    # 过滤掉at=0的channel， 定价不能修改
    filtered_name = set()
    for each in list(count.keys()):
        try:
            channel_name = each.split('_', 1)[1]
        except:
            del count[each]
            continue
        if ('imp' in each) and int(count[each]) < MINI_NUM:
            filtered_name.add(channel_name)
        # at=1 是定价，去掉
        if channel_name[0] == '1':
            filtered_name.add(channel_name)

    left_count = {}
    for each in count:
        channel_name = each.split('_', 1)[1]
        if channel_name in filtered_name:
            continue
        left_count[each] = count[each]

    count = left_count
    # logging.info('RECEIVED:COUNT' + str(count))
    # TODO: call algorithm
    bid = BID(obj)  # 线程中创建

    direction = check_state(info)
    print infostr
    print direction
    try:
        this_10min_clk_count = float(info['this_10min_clk_count'])
        this_10min_imp_count = float(info['this_10min_imp_count'])
        ctr_all = this_10min_clk_count / this_10min_imp_count
    except:
        return
    tune(direction, bid, count, ctr_all)

    # 修改出价
    # mapid, adxcode, mediacode, regioncode, value
    # bid.write("888", "1", "all", "3", "-1")
    # bid.write("888", "4", "5", "6", "-1")
    bid.flush()  # 提交到redis


while (True):
    auto_cost_loop()



