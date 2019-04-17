#!/bin/sh

export HADOOP_HOME=/app/hadoop-2.5.2
export CLASSPATH=$($HADOOP_HOME/bin/hadoop classpath):$CLASSPATH
export PATH=.:$PATH:$HADOOP_HOME/bin


