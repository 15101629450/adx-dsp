#/bin/sh

zip -r auto_cost.zip ../*
scp auto_cost.zip lichunguang@192.168.30.26:/Users/lichunguang/Downloads/
rm -f auto_cost.zip

