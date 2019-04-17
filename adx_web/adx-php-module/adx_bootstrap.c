
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_adx.h"
#include "Zend/zend_API.h"
#include "Zend/zend_interfaces.h"

zend_class_entry *adx_bootstrap_ce = NULL;
const zend_function_entry adx_bootstrap_functions[] = { 
    PHP_ME(adx_bootstrap, adx_conf_file_path, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(adx_bootstrap)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "adx_bootstrap", adx_bootstrap_functions);
    adx_bootstrap_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

    zend_declare_property_null(adx_bootstrap_ce, ZEND_STRL("conf"), ZEND_ACC_PUBLIC TSRMLS_CC);
    return SUCCESS;
}

PHP_METHOD(adx_bootstrap, adx_conf_file_path)
{
    zval *conf = adx_fun_call("parse_ini_file", "app/adx.conf");
    if (conf) zend_update_property(adx_bootstrap_ce, getThis(), ZEND_STRL("conf"), conf TSRMLS_DC);
}


