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
#include "face/mexti_face.h"
#include "finger/mexti_finger.h"

#include "lib/face/zzFaceDLL.h"

ZEND_DECLARE_MODULE_GLOBALS(mexti)

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("mexti.face_on", "On", PHP_INI_ALL, OnUpdateBool, faceAlgOn, zend_mexti_globals, mexti_globals)
	STD_PHP_INI_ENTRY("mexti.face_license", "/usr/local/php/etc", PHP_INI_ALL, OnUpdateStringUnempty, license, zend_mexti_globals, mexti_globals)
	STD_PHP_INI_ENTRY("mexti.face_serch_num", "5000", PHP_INI_ALL, OnUpdateLong, iSearchNum, zend_mexti_globals, mexti_globals)
PHP_INI_END()

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

PHP_MINIT_FUNCTION(mexti)
{
	MEXTI_G(pAlgEngine) = NULL;		// 算法引擎
	MEXTI_G(nError) = 12;			// 未初始化 
	MEXTI_G(faceAlgOn) = false;		// 默认不自动启动人脸算法引擎.

	REGISTER_INI_ENTRIES();

	register_class_Lbs();
	register_class_HeapNode();
	register_class_MinHeap();
	register_class_Lua();
	register_class_Crypto();
	register_class_Finger();
	register_class_Face();

	if(MEXTI_G(faceAlgOn)){
		MEXTI_G(pAlgEngine) = zzInitAlgNThread(MEXTI_G(license), MEXTI_G(iSearchNum), &MEXTI_G(nError));
	}

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(mexti)
{
	if(NULL != MEXTI_G(pAlgEngine)){
        zzFreeAlgThread(MEXTI_G(pAlgEngine));
        MEXTI_G(pAlgEngine) = NULL;
    }
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
	PHP_MSHUTDOWN(mexti),				/* 模块卸载 */
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
