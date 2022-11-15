#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"
#include <zend_interfaces.h>

#include "../lib/minheap.h"
#include "mexti_minheap.h"
#include "mexti_heapnode.h"

#include <stddef.h>

PHPAPI zend_class_entry     * mexti_ce_MinHeap;
       zend_object_handlers   minheap_ce_handlers;


#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

typedef struct mexti_minheap_t
{
    /* data */
    minheap_t e;
    zend_object std;
}mexti_minheap_t;

static inline mexti_minheap_t *mexti_minheap_from_obj(zend_object * obj) /* {{{ */ {
	return (mexti_minheap_t*)((char*)(obj) - XtOffsetOf(mexti_minheap_t, std));
}
#define Z_MINHEAP_P(zv)  mexti_minheap_from_obj(Z_OBJ_P((zv)))


PHP_METHOD(MinHeap, top)
{
    mexti_minheap_t * obj = Z_MINHEAP_P(ZEND_THIS);
    minheapnode_t * n = minheap_pop(&obj->e);
    if(NULL != n ){
        RETURN_LONG(obj->e.n);
    }
    RETURN_LONG(obj->e.n);
}

PHP_METHOD(MinHeap, count)
{
    mexti_minheap_t * obj = Z_MINHEAP_P(ZEND_THIS);
    RETURN_LONG(obj->e.n);
}

PHP_METHOD(MinHeap, isEmpty)
{
    mexti_minheap_t * obj = Z_MINHEAP_P(ZEND_THIS);
    RETURN_BOOL(obj->e.n == 0);
}


PHP_METHOD(MinHeap, extract)
{
    mexti_minheap_t * obj = Z_MINHEAP_P(ZEND_THIS);
    minheapnode_t * e = minheap_pop(&obj->e);
    if(NULL != e ){
        mexti_heapnode_t * n = container_of(e, mexti_heapnode_t, e);
        ZVAL_COPY(return_value, &n->z);
        Z_TRY_DELREF_P(&n->z);
        Z_TRY_DELREF_P(&n->zc);
        return;
    }
    RETURN_NULL();
}

PHP_METHOD(MinHeap, insert)
{
    zval *value;
    mexti_minheap_t * obj = Z_MINHEAP_P(ZEND_THIS);

    ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

    mexti_heapnode_t * node = mexti_minheapnode_from_obj(Z_OBJ_P(value));
    // 
    ZVAL_COPY(&node->z, value);
    ZVAL_COPY(&node->zc, ZEND_THIS);
    node->c = &obj->e;

    if(-1 == minheap_push(&obj->e, &node->e)){
        Z_TRY_DELREF_P(&node->z);
        RETURN_FALSE;
    }
    RETURN_LONG(obj->e.n);
}

PHP_METHOD(MinHeap, adjust)
{
    zval *value;
    ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

    mexti_minheap_t * obj = Z_MINHEAP_P(ZEND_THIS);
    mexti_heapnode_t * node = mexti_minheapnode_from_obj(Z_OBJ_P(value));
    if(node->c == &obj->e){
        minheap_adjust(&obj->e, &node->e);
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

PHP_METHOD(MinHeap, erase)
{
    zval *value;
    ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

    mexti_minheap_t * obj = Z_MINHEAP_P(ZEND_THIS);
    mexti_heapnode_t * node = mexti_minheapnode_from_obj(Z_OBJ_P(value));
    if(minheap_elm_inheap(&node->e)){
        minheap_erase(&obj->e, &node->e);
        node->c = NULL;
        Z_TRY_DELREF_P(&node->zc);
        Z_TRY_DELREF_P(&node->z);
        RETURN_TRUE;
    }
    RETURN_FALSE;
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MinHeap_top, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX2(arginfo_class_MinHeap_count, 0, 0, IS_LONG, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MinHeap_insert, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, b, mexti\\MinHeapNode, 0)
ZEND_END_ARG_INFO()



#define arginfo_class_MinHeap_isEmpty arginfo_class_MinHeap_top
#define arginfo_class_MinHeap_extract arginfo_class_MinHeap_top
#define arginfo_class_MinHeap_adjust arginfo_class_MinHeap_top
#define arginfo_class_MinHeap_erase arginfo_class_MinHeap_insert

static const zend_function_entry class_MinHeap_methods[] = {
    ZEND_ME(MinHeap, insert, arginfo_class_MinHeap_insert,   ZEND_ACC_PUBLIC)
    ZEND_ME(MinHeap, adjust, arginfo_class_MinHeap_adjust,   ZEND_ACC_PUBLIC)
    ZEND_ME(MinHeap, top, arginfo_class_MinHeap_top,         ZEND_ACC_PUBLIC)
    ZEND_ME(MinHeap, count, arginfo_class_MinHeap_count,     ZEND_ACC_PUBLIC)
    ZEND_ME(MinHeap, extract, arginfo_class_MinHeap_extract, ZEND_ACC_PUBLIC)
    ZEND_ME(MinHeap, erase, arginfo_class_MinHeap_erase,     ZEND_ACC_PUBLIC)
    ZEND_ME(MinHeap, isEmpty, arginfo_class_MinHeap_isEmpty, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


// 释放对象
static void mexti_minheap_free_object(zend_object *object)
{
    mexti_minheap_t * obj = mexti_minheap_from_obj(object);
    unsigned int i;

    // 释放所有标准属性.
    zend_object_std_dtor(&obj->std);

    // 释放所有内部成员引用
    for(i = 0; i < obj->e.n; i ++){
        mexti_heapnode_t * n = container_of(obj->e.p[i], mexti_heapnode_t, e);
        minheap_elm_init(&n->e);
        n->c = NULL;
        Z_TRY_DELREF_P(&n->zc);
        Z_TRY_DELREF_P(&n->z);
    }
    minheap_uninit(&obj->e);

    zend_printf("\\mexti\\MinHeap::free\n");
}

/*
static void mexti_minheap_dtor_object(zend_object * object)
{
    zend_printf("\\mexti\\MinHeap::dtor\n");
}
*/

static int mexti_compare_callback(minheapnode_t * _a, minheapnode_t * _b)
{
    zval zresult;
    mexti_heapnode_t * a = container_of(_a, mexti_heapnode_t, e), * b = container_of(_b, mexti_heapnode_t, e);
    //zend_call_method_with_1_params(&a->std, a->std.ce, NULL, "compare", &zresult, b->z);
    zend_call_method(&a->std, a->std.ce, NULL, "compare", sizeof("compare") -1, &zresult, 1, &b->z, NULL);
    //zend_printf("compare(a,b) = %d \n", zresult.value.lval);
    if(zresult.value.lval > 0) return 1;
    return 0;
}

// 创建对象
static zend_object * mexti_minheap_object_new(zend_class_entry *class_type)
{
    mexti_minheap_t * obj = zend_object_alloc(sizeof(mexti_minheap_t), class_type);
    zend_object_std_init(&obj->std, class_type);
	object_properties_init(&obj->std, class_type);

    minheap_init(&obj->e, mexti_compare_callback);
    obj->std.handlers = &minheap_ce_handlers;

    return &obj->std;
}


zend_class_entry * register_class_MinHeap()
{
	 zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "mexti\\MinHeap", class_MinHeap_methods);
	//class_entry = zend_register_internal_interface(&ce);
    class_entry = zend_register_internal_class_ex(&ce, NULL);
	//class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	//zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);
    class_entry->create_object          = mexti_minheap_object_new;

    memcpy(&minheap_ce_handlers, zend_get_std_object_handlers(), sizeof(minheap_ce_handlers));

    minheap_ce_handlers.offset          = XtOffsetOf(mexti_minheap_t, std);
    //minheap_ce_handlers.dtor_obj        = mexti_minheap_dtor_object;        /* This is the destroy handler */
    minheap_ce_handlers.free_obj        = mexti_minheap_free_object;        /* This is the free handler */
    

	return class_entry;
}



