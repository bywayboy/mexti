/* mexti extension for PHP */

#ifndef PHP_MEXTI_H
# define PHP_MEXTI_H

extern zend_module_entry mexti_module_entry;
# define phpext_mexti_ptr &mexti_module_entry

# define PHP_MEXTI_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_MEXTI)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

ZEND_BEGIN_MODULE_GLOBALS(mexti)
char * license;
int iSearchNum;
void * pAlgEngine;
ZEND_END_MODULE_GLOBALS(mexti)

ZEND_EXTERN_MODULE_GLOBALS(mexti)

#ifdef ZTS
#define MEXTI_G(v) TSRMG(mexti_globals_id, zend_mexti_globals *, v)
#else
#define MEXTI_G(v) (mexti_globals.v)
#endif

#endif	/* PHP_MEXTI_H */
