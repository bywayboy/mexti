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

#include "lbs/mexti_lbs.h"
#include "minheap/mexti_heapnode.h"
#include "minheap/mexti_minheap.h"
#include "lua/mexti_lua.h"
#include "crypto/mexti_crypto.h"



/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

PHP_MINIT_FUNCTION(mexti)
{
	register_class_Lbs();
	register_class_HeapNode();
	register_class_MinHeap();
	register_class_Lua();
	register_class_Crypto();
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
