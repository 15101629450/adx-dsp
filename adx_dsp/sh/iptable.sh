#!/bin/sh

service iptables stop
iptables -F

iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

iptables -A INPUT -p tcp --dport 22 -j ACCEPT
# iptables -A OUTPUT -p tcp --sport 22 -j ACCEPT

iptables -A INPUT -p tcp --dport 80 -j ACCEPT
# iptables -A OUTPUT -p tcp --sport 80 -j ACCEPT

iptables -A INPUT -s 127.0.0.1 -d 127.0.0.1 -j ACCEPT
# iptables -A OUTPUT -s 127.0.0.1 -d 127.0.0.1 -j ACCEPT

iptables -P INPUT DROP 
# iptables -P OUTPUT DROP
iptables -P FORWARD DROP

service iptables save  
service iptables restart
service iptables status


