#!/usr/bin/python

from distutils.core import setup, Extension
module = Extension('adx_auto_cost', 
		sources = ['adx_list.c', 'adx_conf_file.c', 'adx_auto_cost.c', 'adx_bid_write.c', 'adx_push.c', 'main.c'],
		libraries = ['hiredis'],
		library_dirs=['/usr/local/lib'])
setup(name = 'adx_auto_cost', version = '1.0', ext_modules = [module])
setup(name = 'auto_cost_API', version = '1.0', py_modules=['auto_cost_API'])

