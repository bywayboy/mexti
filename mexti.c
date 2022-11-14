/* mexti extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_main.h"
#include "ext/standard/info.h"

#include "php_mexti.h"
#include "mexti_arginfo.h"

#include "minheap/mexti_minheap.h"


/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ void test1() */
PHP_FUNCTION(test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "mexti");
}
/* }}} */

/* {{{ string test2( [ string $var ] ) */
PHP_FUNCTION(test2)
{
	char *var = "World";
	size_t var_len = sizeof("World") - 1;
	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(var, var_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = strpprintf(0, "Hello %s", var);

	RETURN_STR(retval);
}
/* }}}*/



PHP_MINIT_FUNCTION(mexti)
{
	mexti_ce_ExMinHeap = register_class_ExMinHeap();
	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(mexti)
{
#if defined(ZTS) && defined(COMPILE_DL_MEXTI)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(mexti)
{

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(mexti)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mexti support", "enabled");
	php_info_print_table_end();
}
/* }}} */


static const zend_module_dep mexti_deps[] = {
	ZEND_MOD_END
};

zend_module_entry mexti_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	mexti_deps,							/* 模块依赖 */
	"mexti",							/* 扩展名称 */
	ext_functions,						/* 模块函数入口 */
	PHP_MINIT(mexti),					/* 模块初始化 */
	NULL,								/* 模块卸载 */
	PHP_RINIT(mexti),					/* 请求初始化 */
	PHP_RSHUTDOWN(mexti),				/* 请求结束 */
	PHP_MINFO(mexti),					/* 模块信息 */
	PHP_MEXTI_VERSION,					/* 模块版本 */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MEXTI
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(mexti)
#endif
