#ifndef _php_mexti_minheapnode_h
#define _php_mexti_minheapnode_h

#include "../lib/minheap.h"

typedef struct mexti_heapnode{
    minheapnode_t e;
    minheap_t * c;
    zval z, zc;
    zend_object std;
}mexti_heapnode_t;


extern PHPAPI zend_class_entry *mexti_ce_ExHeapNode;

zend_class_entry *register_class_HeapNode();

inline mexti_heapnode_t * mexti_minheapnode_from_obj(zend_object * obj) /* {{{ */ {
	return (mexti_heapnode_t*)((char*)(obj) - XtOffsetOf(mexti_heapnode_t, std));
};

#endif