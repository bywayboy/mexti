#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "mexti_heapnode.h"


PHPAPI zend_class_entry     * mexti_ce_HeapNode;
       zend_object_handlers   heapnode_ce_handlers;

inline mexti_heapnode_t * mexti_minheapnode_from_obj(zend_object * obj) /* {{{ */ {
	return (mexti_heapnode_t*)((char*)(obj) - XtOffsetOf(mexti_heapnode_t, std));
}

#define Z_MINHEAPNODE_P(zv)  mexti_minheapnode_from_obj(Z_OBJ_P((zv)))


PHP_METHOD(HeapNode, erase)
{
    
}

PHP_METHOD(HeapNode, inHeap)
{
    
}

PHP_METHOD(HeapNode, adjust)
{
    zend_printf("HeapNode::adjust\n");
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_class_HeapNode_compare, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, b, mexti\\MinHeapNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_HeapNode_adjust, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_HeapNode_erase   arginfo_class_HeapNode_adjust
#define arginfo_class_HeapNode_inHeap  arginfo_class_HeapNode_adjust

static zend_function_entry class_HeapNode_methods [] = {
    // 比较方法 需继承实现
    ZEND_ABSTRACT_ME(HeapNode, compare, arginfo_class_HeapNode_compare)
    // 将自身从 MinHeap 中移除
    ZEND_ME(HeapNode, erase, arginfo_class_HeapNode_erase, ZEND_ACC_PUBLIC)
    // 判断自己是否处于一个MinHeap中
    ZEND_ME(HeapNode, inHeap, arginfo_class_HeapNode_inHeap, ZEND_ACC_PUBLIC)
    // 比较键值 变更后, 调用该方法触发位置调整.
    ZEND_ME(HeapNode, adjust, arginfo_class_HeapNode_adjust, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


static zend_object * mexti_heapnode_object_new(zend_class_entry * class_type)
{
    mexti_heapnode_t * obj = zend_object_alloc(sizeof(mexti_heapnode_t), class_type);
    zend_object_std_init(&obj->std, class_type);
	object_properties_init(&obj->std, class_type);

    minheap_elm_init(&obj->e);
    obj->std.handlers = &heapnode_ce_handlers;
    zend_printf("HeapNode::__construct\n");

    return &obj->std;
}

static void mexti_heapnode_free_object(zend_object * object)
{
    mexti_heapnode_t * obj =  mexti_minheapnode_from_obj(object);

    // 释放所有属性.
    zend_object_std_dtor(&obj->std);

    zend_printf("HeapNode::free\n");
}

static void mexti_heapnode_dtor_object(zend_object * object)
{
    zend_printf("HeapNode::dtor\n");
}

zend_class_entry *register_class_HeapNode()
{
    zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "mexti\\MinHeapNode", class_HeapNode_methods);
	//class_entry = zend_register_internal_interface(&ce);
    class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	//zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);
    class_entry->create_object          = mexti_heapnode_object_new;

    memcpy(&heapnode_ce_handlers, zend_get_std_object_handlers(), sizeof(heapnode_ce_handlers));

    heapnode_ce_handlers.offset          = XtOffsetOf(mexti_heapnode_t, std);

    //heapnode_ce_handlers.dtor_obj        = mexti_heapnode_dtor_object;        /* This is the destroy handler */
    heapnode_ce_handlers.free_obj        = mexti_heapnode_free_object;        /* This is the free handler */

	return class_entry;
}