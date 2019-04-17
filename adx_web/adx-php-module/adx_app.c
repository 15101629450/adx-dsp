
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_adx.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_hash.h"

zend_class_entry *adx_app_ce = NULL;
const zend_function_entry adx_app_functions[] = {
    PHP_ME(adx_app, run, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(adx_app)
{
    zend_class_entry ce; 
    INIT_CLASS_ENTRY(ce, "adx_app", adx_app_functions);
    adx_app_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
    return SUCCESS;
}

PHP_METHOD(adx_app, run)
{
    adx_require_dir("app/library");
    adx_require_file("app/bootstrap.php");

    /******************/
    /* find bootstrap */
    /******************/
    zend_class_entry **ce = NULL;
    if (zend_hash_find(EG(class_table), ZEND_STRS("bootstrap"), (void **) &ce) != SUCCESS)
        return;

    zval *bootstrap = NULL;
    MAKE_STD_ZVAL(bootstrap);
    object_init_ex(bootstrap, *ce);

    /*********************/
    /* bootstrap -> init */
    /*********************/
    zval *retval = NULL;
    MAKE_STD_ZVAL(retval);
    char *key = "adx_conf_file_path"; uint len = 18;
    zend_call_method(&bootstrap, *ce, NULL, key, len, NULL, 1, retval, NULL TSRMLS_CC);

    HashTable *methods = &((*ce)->function_table);
    for(zend_hash_internal_pointer_reset(methods);
            zend_hash_has_more_elements(methods) == SUCCESS;
            zend_hash_move_forward(methods)) {

        zend_hash_get_current_key_ex(methods, &key, &len, NULL, 0, NULL);
        if (strncmp(key, "adx_conf_file_path", len) != 0)
            zend_call_method(&bootstrap, *ce, NULL, key, len - 1, NULL, 1, retval, NULL TSRMLS_CC);
    }

    zval_ptr_dtor(&bootstrap);

    /*****************/
    /* get post */
    /*****************/
    zval **z_post = NULL;
    char controllers_func[1024] = "adx_controllers_default";

    zend_is_auto_global(ZEND_STRL("_REQUEST") TSRMLS_CC);
    zend_hash_find(&EG(symbol_table), ZEND_STRS("_REQUEST"), (void **)&z_post);
    if (!z_post || !*z_post) return;

    /*****************/
    /* get post */
    /*****************/
    zval **val = NULL;
    for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(*z_post));
            zend_hash_has_more_elements(Z_ARRVAL_P(*z_post)) == SUCCESS;
            zend_hash_move_forward(Z_ARRVAL_P(*z_post))) {

        zend_hash_get_current_key_ex(Z_ARRVAL_P(*z_post), &key, &len, NULL, 0, NULL);
        zend_hash_get_current_data(Z_ARRVAL_P(*z_post), (void **)&val);
        php_printf("[%s][%s]\n", key, Z_STRVAL_PP(val));       

        char *value = Z_STRVAL_PP(val);
        if (strcmp(key, "packet_type") == 0 && value && *value)
            sprintf(controllers_func, "adx_controllers_%s", value);
        adx_require_dir("app/modules");
    }

    /*************************/
    /* call controllers func */
    /*************************/

    // php_printf("fun=%s\n", controllers_func);
    // adx_fun_call(controllers_func, NULL);
}






