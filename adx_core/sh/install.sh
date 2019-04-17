#/bin/sh

pkill -9 odin_pv
sleep 1

spawn-fcgi -a "127.0.0.1" -p 4000 -f /make/PV/odin_pv


