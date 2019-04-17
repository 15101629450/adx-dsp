#!/usr/bin/python
#coding=utf-8

import os
import sys
from auto_cost_API import auto_cost, BID
import logging
# import psutil

MINI_NUM = 0	# 改价的最小流量
MAXPRICE = 900	# 最大价格限制
MINPRICE = 1	# 最小价格限制

obj = auto_cost()
if (obj.check() == False):
	print(obj.error())
	sys.exit(0)

logging.basicConfig(
	format='[%(asctime)s]%(message)s',
	level=logging.DEBUG,
	datefmt='%Y-%m-%d %H:%M:%S',
	filename=obj.get_conf())

def average_price_parse(count):
	imp = 0.00
	cost = 0.00
	for key in count.keys():
		try:
			if 'imp_' in key : imp += float(count[key])
			if 'cost_' in key : cost += float(count[key])
			if imp <= 0 : return 0
		except : pass
	
	return cost / imp / 100

def tune_price_element(count, key, ctr_kpi, ctr_cur, coef_cost):
	
	# 不是展现数据
	if not ('imp' in key) : return
	# print key
	
	# 截取当前维度信息
	keys = key.split('_')
	adx = keys[1]
	mapid = keys[2]

	# 计算维度信息
	try : imp = float(count[key])
	except : imp = 0
	
	try : clk = float(count[key.replace('imp_', 'clk_')])
	except : clk = 0
	
	try : cost = float(count[key.replace('imp_', 'cost_')]) / 100
	except : cost = 0

	# 没有展现的错误处理
	if imp <= 0 : return
	
	# 计算当前 维度 出价
	price = cost / imp
	# print adx,mapid,price

	# 计算当前 维度 CTR
	ctr = clk / imp
	# print adx,mapid,ctr
	
	# 公式: (ecmp + N) * M%  计算系数 N
	if ctr_cur < ctr_kpi : # 上调
		if ctr < ctr_kpi :
			coef_ctr = -10
		else :
			coef_ctr = 10
	else : # 下调
		if ctr < ctr_kpi :
			coef_ctr = 10
		else :
			coef_ctr = -10
	
	# 公式: (ecmp + N) * M%
	new_price = (price * 1.2 + coef_ctr) * coef_cost
	if new_price > MAXPRICE : new_price = MAXPRICE
	if new_price < MINPRICE : new_price = MINPRICE 
	
	# 没有价格(可能是定价)
	if cost == 0 : new_price = price = 0 
	
	logging.info("adx=%s mapid=%s imp=%d clk=%d ctr=%f coef_ctr=%d price=(%f==>%f)" % (adx, mapid, imp, clk, ctr, coef_ctr, price, new_price))
	# print ("adx=%s mapid=%s imp=%d clk=%d ctr=%f coef_ctr=%d price=(%f==>%f)" % (adx, mapid, imp, clk, ctr, coef_ctr, price, new_price))

	if new_price != price :
		# mapid, adxcode, mediacode, regioncode, value
		obj.bid_write(mapid, adx, "all", "all", str(new_price))
	
# 自动出价
def tune_price(info, count):

	# 解析INFO信息    
	try :
		ctr_kpi = float(info['ctr_kpi'])
		ctr_cur = float(info['ctr_cur'])
		coef_cost = float(info['coef_cost'])
	except : return

	logging.info("ctr_kpi=%f ctr_cur=%f coef_cost=%f" % (ctr_kpi, ctr_cur, coef_cost))

	# 出价
	for key in count.keys() : tune_price_element(count, key, ctr_kpi, ctr_cur, coef_cost)

# 清洗数据节点
def count_filter_element(new_count, count, key):

	# 数据格式不合法    
    	keys = key.split('_')
	if len(keys) != 5 : return
	
	# 清洗没有赋值的数据
	if count[key] is '' : return

	# 清洗投放数据
	if keys[0].find('bid') >= 0 : return
	# if 'bid' in keys : return
		
	# 获取value并且转换float
	try : value = float(count[key])
	except : return

	# keys[3], keys[2] # adx, mapid
	new_key = "_".join((keys[0], keys[3], keys[2]))

	if new_count.get(new_key) :
		new_count[new_key] += value
	else :
		new_count[new_key] = value

# 清洗数据
def count_filter(count):
	new_count = {}
	for key in count.keys():
		count_filter_element(new_count, count, key)
		
	# for key in new_count.keys(): print key, "=", new_count[key]
	return new_count
		
def auto_cost_loop():
	# 内存泄漏检测
	# print "Memory=",psutil.Process(os.getpid()).get_memory_info().rss / 1024 / 1024,"M"
	
	mess = obj.wait()
	info = mess['INFO']
	count = mess['COUNT']
	
	logging.info("policyid = " + info.get("policyid"))
	logging.info("surplus_10min = " + info.get("surplus_10min"))
	logging.info("complete_time = " + info.get("complete_time"))
	logging.info("daily_imp_kpi = " + info.get("daily_imp_kpi"))
	logging.info("daily_imp_count = " + info.get("daily_imp_count"))
	logging.info("daily_clk_kpi = " + info.get("daily_clk_kpi"))
	logging.info("daily_clk_count = " + info.get("daily_clk_count"))
	logging.info("10min_imp_count = " + info.get("this_10min_imp_count"))
	logging.info("10min_clk_count = " + info.get("this_10min_clk_count"))
	
	count = count_filter(count) # 清洗数据
	tune_price(info, count) # 自动出价
	
while(True) : auto_cost_loop()
# auto_cost_loop()

def count_dis(count):
	imp = 0.00
	clk = 0.00
	cost = 0.00
	for key in count.keys():
		if 'imp_' in key  : imp  += float(count[key])
		if 'clk_' in key  : clk  += float(count[key])
		if 'cost_' in key : cost += float(count[key])

	print imp,clk,cost

count = obj.get_count("auto_cost_bid_63_01_19")

count_dis(count)
count = count_filter(count)
count_dis(count)

info = {}
info["ctr_kpi"]	= 0.10
info["ctr_cur"]	= 0.99
info["coef_cost"] = 0.90
# tune_price(info, count)




