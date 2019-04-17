#/bin/sh

# socket 的最大连接数
echo 50000 > /proc/sys/net/core/somaxconn

# 加快系统的tcp回收机制
echo  1 > /proc/sys/net/ipv4/tcp_tw_recycle

# 允许空的tcp回收利用 方法如下
echo 1 > /proc/sys/net/ipv4/tcp_tw_reuse 

# 让系统不做洪水抵御保护(当系统检测到大量的请求时,会自动给返回信息中增加cookie,验证客户端身份)
echo 0 > /proc/sys/net/ipv4/tcp_syncookies 

# 进程允许打开的文件数量
ulimit -n 10240;
ulimit -u unlimited

APP_NAME=adx_dsp
APP_PATH="`pwd`/$APP_NAME"
SPAWN_FCGI="`pwd`/sh/spawn-fcgi"

# echo $APP_PATH
# echo $SPAWN_FCGI

pkill -9 $APP_NAME
sleep 1

$SPAWN_FCGI -a "127.0.0.1" -p 4000 -f $APP_PATH


