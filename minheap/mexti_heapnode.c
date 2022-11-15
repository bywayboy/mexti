#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "mexti_heapnode.h"


PHPAPI zend_class_entry     * mexti_ce_MinHeapNode;
       zend_object_handlers   minheapnode_ce_handlers;


#define Z_MINHEAPNODE_P(zv)  mexti_minheapnode_from_obj(Z_OBJ_P((zv)))


PHP_METHOD(MinHeapNode, erase)
{
    mexti_heapnode_t * obj = Z_MINHEAPNODE_P(ZEND_THIS);
    if(NULL != obj->c){
        minheap_erase(obj->c, &obj->e);
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

PHP_METHOD(MinHeapNode, inHeap)
{
    mexti_heapnode_t * obj = Z_MINHEAPNODE_P(ZEND_THIS);
}

PHP_METHOD(MinHeapNode, adjust)
{
    mexti_heapnode_t * obj = Z_MINHEAPNODE_P(ZEND_THIS);
    if(NULL != obj->c){
        minheap_adjust(obj->c, &obj->e);
        RETURN_TRUE;
    }
    RETURN_FALSE;
}


ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX2(arginfo_class_MinHeapNode_compare, 0, 1, IS_LONG, 0, 0)
    ZEND_ARG_OBJ_INFO(0, b, mexti\\MinHeapNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MinHeapNode_adjust, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MinHeapNode_erase   arginfo_class_MinHeapNode_adjust
#define arginfo_class_MinHeapNode_inHeap  arginfo_class_MinHeapNode_adjust

static zend_function_entry class_MinHeapNode_methods [] = {
    // 比较方法 需继承实现
    ZEND_ABSTRACT_ME(MinHeapNode, compare, arginfo_class_MinHeapNode_compare)
    // 将自身从 MinHeap 中移除
    ZEND_ME(MinHeapNode, erase, arginfo_class_MinHeapNode_erase, ZEND_ACC_PUBLIC)
    // 判断自己是否处于一个MinHeap中
    ZEND_ME(MinHeapNode, inHeap, arginfo_class_MinHeapNode_inHeap, ZEND_ACC_PUBLIC)
    // 比较键值 变更后, 调用该方法触发位置调整.
    ZEND_ME(MinHeapNode, adjust, arginfo_class_MinHeapNode_adjust, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};


static zend_object * mexti_heapnode_object_new(zend_class_entry * class_type)
{
    mexti_heapnode_t * obj = zend_object_alloc(sizeof(mexti_heapnode_t), class_type);
    zend_object_std_init(&obj->std, class_type);
	object_properties_init(&obj->std, class_type);

    minheap_elm_init(&obj->e);
    obj->c = NULL;
    obj->std.handlers = &minheapnode_ce_handlers;

    return &obj->std;
}

static void mexti_heapnode_free_object(zend_object * object)
{
    mexti_heapnode_t * obj =  mexti_minheapnode_from_obj(object);

    // 释放所有属性.
    zend_object_std_dtor(&obj->std);

    //zend_printf("HeapNode::free\n");
}

/*
static void mexti_heapnode_dtor_object(zend_object * object)
{
    zend_printf("HeapNode::dtor\n");
}
*/

zend_class_entry *register_class_HeapNode()
{
    zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "mexti\\MinHeapNode", class_MinHeapNode_methods);
	//class_entry = zend_register_internal_interface(&ce);
    class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	//zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);
    class_entry->create_object          = mexti_heapnode_object_new;

    memcpy(&minheapnode_ce_handlers, zend_get_std_object_handlers(), sizeof(minheapnode_ce_handlers));

    minheapnode_ce_handlers.offset          = XtOffsetOf(mexti_heapnode_t, std);

    //minheapnode_ce_handlers.dtor_obj        = mexti_heapnode_dtor_object;        /* This is the destroy handler */
    minheapnode_ce_handlers.free_obj        = mexti_heapnode_free_object;        /* This is the free handler */

	return class_entry;
}