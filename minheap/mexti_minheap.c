#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "../lib/minheap.h"
#include "mexti_minheap.h"

PHPAPI zend_class_entry     * mexti_ce_ExMinHeap;
       zend_object_handlers   minheap_ce_handlers;


typedef struct _mexti_ptr_minheap
{
    /* data */
    int count;

}mexti_ptr_minheap;


ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ExMinHeap_top, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ExMinHeap_isEmpty arginfo_class_ExMinHeap_top
#define arginfo_class_ExMinHeap_extract arginfo_class_ExMinHeap_top


static const zend_function_entry class_ExMinHeap_methods[] = {
    ZEND_ME(ExMinHeap, insert, arginfo_class_ExMinHeap_insert,   ZEND_ACC_PUBLIC)
    ZEND_ME(ExMinHeap, adjust, arginfo_class_ExMinHeap_adjust,   ZEND_ACC_PUBLIC)
    ZEND_ME(ExMinHeap, top, arginfo_class_ExMinHeap_top,         ZEND_ACC_PUBLIC)
    ZEND_ME(ExMinHeap, extract, arginfo_class_ExMinHeap_extract, ZEND_ACC_PUBLIC)
    ZEND_ME(ExMinHeap, isEmpty, arginfo_class_ExMinHeap_isEmpty, ZEND_ACC_PUBLIC)
};


// 创建对象
static void mexti_minheap_free_object(zend_object *object)
{

}

// 
static void mexti_minheap_dtor_object(zend_object * object)
{

}

// 释放对象
static zend_object * mexti_minheap_object_new(zend_class_entry *class_type)
{
    intern = zend_object_alloc(sizeof(spl_heap_object), parent);
    zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

    retval.handlers = &minheap_ce_handlers;
}

zend_class_entry *register_class_ExMinHeap()
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ExMinHeap", class_ExMinHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	//zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);
    class_entry->create_object          = mexti_minheap_object_new;

    memcpy(&minheap_ce_handlers, zend_get_std_object_handlers(), sizeof(minheap_ce_handlers));

    minheap_ce_handlers.free_obj        = mexti_minheap_free_object;        /* This is the free handler */
    minheap_ce_handlers.dtor_obj        = mexti_minheap_dtor_object;        /* This is the destroy handler */

	return class_entry;
}

PHP_METHOD(ExMinHeap, top)
{

}

PHP_METHOD(ExMinHeap, isEmpty)
{

}


PHP_METHOD(ExMinHeap, extract)
{

}

PHP_METHOD(ExMinHeap, insert)
{

}

PHP_METHOD(ExMinHeap, adjust)
{

}


