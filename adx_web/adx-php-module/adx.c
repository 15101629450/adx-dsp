
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_adx.h"
#include "ext/standard/info.h"

// install module call
PHP_MINIT_FUNCTION(adx)
{
    ZEND_MODULE_STARTUP_N(adx_app)(INIT_FUNC_ARGS_PASSTHRU);// init class adx_app
    ZEND_MODULE_STARTUP_N(adx_bootstrap)(INIT_FUNC_ARGS_PASSTHRU);// init class adx_bootstrap
    ZEND_MODULE_STARTUP_N(adx_controllers)(INIT_FUNC_ARGS_PASSTHRU);// init class adx_controllers
    return SUCCESS;
}

// uninstall module call
PHP_MSHUTDOWN_FUNCTION(adx)
{
    return SUCCESS;
}

// request call
PHP_RINIT_FUNCTION(adx)
{
    return SUCCESS;
}

// request end call
PHP_RSHUTDOWN_FUNCTION(adx)
{
    return SUCCESS;
}

// phpinfo
PHP_MINFO_FUNCTION(adx)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "adx_module", "enabled");
    php_info_print_table_end();
}

const zend_function_entry adx_functions[] = {NULL, NULL, NULL};
zend_module_entry adx_module_entry = {
    STANDARD_MODULE_HEADER,
    "adx",
    adx_functions,
    PHP_MINIT(adx),
    PHP_MSHUTDOWN(adx),
    PHP_RINIT(adx),
    PHP_RSHUTDOWN(adx),
    PHP_MINFO(adx),
    PHP_ADX_VERSION,
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(adx)

zval *adx_fun_call(char *fun, char *arg)
{
    zval *function_name = NULL;
    MAKE_STD_ZVAL(function_name);
    ZVAL_STRING(function_name, fun, 1);

    zval *params[2] = {0};
    if (arg) {

        zval *z_val = NULL;
        MAKE_STD_ZVAL(z_val);
        ZVAL_STRING(z_val, arg, 1);
        params[0] = z_val;
    }

    zval *retval = NULL;
    MAKE_STD_ZVAL(retval);
    call_user_function(CG(function_table), 
            NULL, 
            function_name,
            retval,
            arg?1:0,
            arg?params:NULL TSRMLS_DC);

    return retval;
}

void adx_require_file(char *path)
{
    zend_file_handle zfd;
    zfd.type = ZEND_HANDLE_FILENAME;
    zfd.filename = path;
    zfd.free_filename = 0;
    zfd.opened_path = NULL;
    zend_execute_scripts(ZEND_REQUIRE TSRMLS_CC, NULL, 1, &zfd);
}

void adx_require_dir(char *path)
{

    DIR *dir = NULL;
    struct dirent *dir_info = NULL;
    char _path[1024];

    if (!(dir = opendir(path)))
        return; 

    while((dir_info = readdir(dir))) {

        if (dir_info->d_type == 8 
                && strstr(dir_info->d_name, ".php")) {

            sprintf(_path, "%s/%s", path, dir_info->d_name);
            adx_require_file(_path);
        }
    }

    closedir(dir);
}

void adx_zval_check(zval *z) 
{
    switch (Z_TYPE_P(z)) {
        case IS_NULL:
            php_printf("NULL\n");
            break;
        case IS_BOOL:
            php_printf("Boolean: %s\n", Z_LVAL_P(z) ? "TRUE" : "FALSE");
            break;
        case IS_LONG:
            php_printf("Long: %ld\n", Z_LVAL_P(z));
            break;
        case IS_DOUBLE:
            php_printf("Double: %f\n", Z_DVAL_P(z));
            break;
        case IS_STRING:
            php_printf("String: ");
            PHPWRITE(Z_STRVAL_P(z), Z_STRLEN_P(z));
            php_printf("\n");
            break;
        case IS_RESOURCE:
            php_printf("Resource\n");
            break;
        case IS_ARRAY:
            php_printf("Array\n");
            break;
        case IS_OBJECT:
            php_printf("Object\n");
            break;
        default:
            php_printf("Unknown\n");
    }
}






