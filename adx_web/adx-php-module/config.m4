
PHP_ARG_ENABLE(adx, whether to enable adx support,
Make sure that the comment is aligned:
[  --enable-adx           Enable adx support])

if test "$PHP_ADX" != "no"; then
  PHP_NEW_EXTENSION(adx, adx.c adx_app.c adx_bootstrap.c adx_controllers.c $ext_shared)
fi

