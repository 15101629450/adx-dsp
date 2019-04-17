#!/usr/bin/python
#coding=utf-8

import os
import sys
from auto_cost_API import auto_cost, BID

obj = auto_cost()
if (obj.check() == False):
	print(obj.error())
	sys.exit(0)

def mapid_bid_display(mapid):
	info = obj.get_mapid_bid(mapid)
	if not info : return
	print "mapid =", mapid, "info =", info

def mapid_all():
	for mapid in range(1, 10000):
		mapid_bid_display(str(mapid))

def mapid_file():
	path = sys.argv[1]
	if not os.path.exists(path) :
		print "file err :", path
		return

	fp = open(sys.argv[1], "r")
	for mapid in fp:
		mapid = mapid.replace("\r", "")
		mapid = mapid.replace("\n", "")
		mapid_bid_display(mapid)


argc = len(sys.argv)
if argc == 1 : mapid_all()
elif argc == 2 : mapid_file()

