import os
import time
while True:
    t = os.popen('./get_coef.R')
    print('finish')
    time.sleep(24 * 3600)
