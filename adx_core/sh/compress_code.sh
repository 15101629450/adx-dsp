#/bin/sh

zip -r PV.zip ../*
scp PV.zip lichunguang@192.168.30.30:/Users/lichunguang/Downloads/
rm -f PV.zip

