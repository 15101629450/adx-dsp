
#ifndef PHP_ADX_H
#define PHP_ADX_H

extern zend_module_entry adx_module_entry;
#define phpext_adx_ptr &adx_module_entry

#define PHP_ADX_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_ADX_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ADX_API __attribute__ ((visibility("default")))
#else
#	define PHP_ADX_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define ADX_G(v) TSRMG(adx_globals_id, zend_adx_globals *, v)
#else
#define ADX_G(v) (adx_globals.v)
#endif

PHP_MINIT_FUNCTION(adx_app);
PHP_METHOD(adx_app, run);

PHP_MINIT_FUNCTION(adx_bootstrap);
PHP_METHOD(adx_bootstrap, adx_conf_file_path);

PHP_MINIT_FUNCTION(adx_controllers);
PHP_METHOD(adx_controllers, __construct);
PHP_METHOD(adx_controllers, __destruct);



extern void adx_log(const char *format, ...);
extern zval *adx_fun_call(char *fun, char *arg);
extern void adx_require_file(char *path);
extern void adx_require_dir(char *path);
extern void adx_zval_check(zval *z);

#endif	/* PHP_ADX_H */



