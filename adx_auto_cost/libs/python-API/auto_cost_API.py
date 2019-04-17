#!/usr/bin/python
#coding=utf-8

import os
import sys
import adx_auto_cost

class auto_cost:
	def __init__(self) : adx_auto_cost.init()
	def check(self) : return adx_auto_cost.check()
	def error(self) : return adx_auto_cost.error()
	def wait(self) : return adx_auto_cost.wait()
	
	def bid_create(self) : return adx_auto_cost.bid_create()
	def bid_write(self, mapid, adxcode, mediacode, regioncode, value) :
		return adx_auto_cost.bid_write(mapid, adxcode, mediacode, regioncode, value)
	def bid_flush(self) : return adx_auto_cost.bid_flush()
	def push(self, mess) : return adx_auto_cost.push(mess)

	def forge_count(self, policyid, timebucket, key, val) :
		return adx_auto_cost.forge_count(policyid, timebucket, key, val)

	def cmd(key) : adx_auto_cost.cmd(cmd)
	
	def get_conf(self) : return adx_auto_cost.get_conf()
	def get_count(self, key) : return adx_auto_cost.get_conf(key)
	
	def get_mapid_bid(self, key) : return adx_auto_cost.get_mapid_bid(key)
	def set_mapid_bid(self, mapid, cost) : return adx_auto_cost.set_mapid_bid(mapid, cost)
    
class BID:
	auto_cost = None
	def __init__(self, auto_cost) : self.auto_cost = auto_cost
	def write(self, mapid, adxcode, mediacode, regioncode, value) :
		return self.auto_cost.bid_write(mapid, adxcode, mediacode, regioncode, value)
	def flush(self) : return self.auto_cost.bid_flush()



