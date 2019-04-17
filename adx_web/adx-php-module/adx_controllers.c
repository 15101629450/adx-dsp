
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_adx.h"

zend_class_entry *adx_controllers_ce = NULL;

const zend_function_entry adx_controllers_functions[] = {
    PHP_ME(adx_controllers, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_ME(adx_controllers, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(adx_controllers)
{
    zend_class_entry ce; 
    INIT_CLASS_ENTRY(ce, "adx_controllers", adx_controllers_functions);
    adx_controllers_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
    return SUCCESS;
}

PHP_METHOD(adx_controllers, __construct)
{
}

PHP_METHOD(adx_controllers, __destruct)
{
}

