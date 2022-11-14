/* mexti extension for PHP */

#ifndef PHP_MEXTI_H
# define PHP_MEXTI_H

extern zend_module_entry mexti_module_entry;
# define phpext_mexti_ptr &mexti_module_entry

# define PHP_MEXTI_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_MEXTI)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_MEXTI_H */
