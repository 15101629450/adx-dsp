#!/bin/sh

INPUT_BID="/user/luokun/spark/sparkMerge/*"
INPUT_TRACKER="/user/root/odinCounter/*"
OUPUT_PATH="/user/auto_cost/"

# hadoop fs -rm -r -f $OUPUT_PATH
# hadoop jar auto_cost_merge.jar auto_cost_merge $INPUT_BID $INPUT_TRACKER $OUPUT_PATH

INPUT_AUTO_COST=$OUPUT_PATH
OUPUT_AUTO_COST="/user/auto_cost_summary"

hadoop fs -rm -r -f $OUPUT_AUTO_COST
hadoop jar auto_cost_summary.jar auto_cost_summary $INPUT_AUTO_COST $OUPUT_AUTO_COST



