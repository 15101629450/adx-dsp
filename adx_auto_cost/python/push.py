#!/usr/bin/python
#coding=utf-8

import os
import sys
from auto_cost_API import auto_cost, BID
    
obj = auto_cost()
if (obj.check() == False) :
    print obj.error()
    sys.exit(0)

# mess = "time=2017-12-12 18:00"
# mess += "|policyid=3"
# mess += "|surplus_10min=2"
# mess += "|count_key=dsp10minutecounter_3_666"
# mess += "|daily_imp_kpi=50000"
# mess += "|daily_clk_kpi=50000"
# mess += "|daily_imp_count=5000"
# mess += "|daily_clk_count=5000"
# mess += "|this_10min_imp_kpi=50000"
# mess += "|this_10min_clk_kpi=583"
# mess += "|this_10min_imp_count=32629"
# mess += "|this_10min_clk_count=500"
# mess += "|next_10min_imp_kpi=50000"
# mess += "|next_10min_clk_kpi=500"
# print obj.push(mess)
# print obj.error()


def push(count_key, this_10min_imp_kpi, this_10min_clk_kpi,
         this_10min_imp_count, this_10min_clk_count, next_10min_imp_kpi,
         next_10min_clk_kpi):
    # 忽略所有daily指标
    fmt = 'count_key={count_key}|surplus_10min={surplus_10min}|'
    fmt += 'daily_imp_kpi={daily_imp_kpi}|daily_clk_kpi={daily_clk_kpi}|'
    fmt += 'daily_imp_count={daily_imp_count}|daily_clk_count={daily_clk_count}|'
    fmt += 'this_10min_imp_kpi={this_10min_imp_kpi}|this_10min_clk_kpi={this_10min_clk_kpi}|'
    fmt += 'this_10min_imp_count={this_10min_imp_count}|this_10min_clk_count={this_10min_clk_count}|'
    fmt += 'next_10min_imp_kpi={next_10min_imp_kpi}|next_10min_clk_kpi={next_10min_clk_kpi}'
    surplus_10min = 2
    daily_imp_kpi = 0
    daily_clk_kpi = 0
    daily_imp_count = 0
    daily_clk_count = 0

    print obj.push(fmt.format(**locals()))



push('dsp10minutecounter_3_0', 32629, 583, 50000, 583, 32629, 500)
