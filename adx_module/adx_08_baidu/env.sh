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

