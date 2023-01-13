
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "mexti_lbs.h"

#include "lib/lbs.h"

PHPAPI zend_class_entry     * mexti_ce_Lbs;
       zend_object_handlers   lbs_ce_handlers;

typedef struct mexti_lbs_t
{
    /* data */
    zend_object std;
}mexti_lbs_t;

static inline mexti_lbs_t *mexti_lbs_from_obj(zend_object * obj) /* {{{ */ {
	return (mexti_lbs_t*)((char*)(obj) - XtOffsetOf(mexti_lbs_t, std));
}
#define Z_MINHEAP_P(zv)  mexti_lbs_from_obj(Z_OBJ_P((zv)))



PHP_METHOD(Lbs, bd2wgs)
{
    double lat,lng,dlat,dlng;
    ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(lat)
		Z_PARAM_DOUBLE(lng)
	ZEND_PARSE_PARAMETERS_END();

    BD2WGS(lat,lng,&dlat,&dlng);

    array_init(return_value);
	add_assoc_double(return_value,"lat", dlat);
	add_assoc_double(return_value,"lng", dlng);
}

PHP_METHOD(Lbs, wgs2bd)
{
	double lat,lng,dlat,dlng;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(lat)
		Z_PARAM_DOUBLE(lng)
	ZEND_PARSE_PARAMETERS_END();

	WGS2BD(lat,lng,&dlat,&dlng);
	array_init(return_value);
	add_assoc_double(return_value,"lat", dlat);
	add_assoc_double(return_value,"lng", dlng);
}

PHP_METHOD(Lbs, wgs2gcj)
{
	double lat,lng,dlat,dlng;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(lat)
		Z_PARAM_DOUBLE(lng)
	ZEND_PARSE_PARAMETERS_END();

	WGS2GCJ(lat,lng,&dlat,&dlng);
	array_init(return_value);
	add_assoc_double(return_value,"lat", dlat);
	add_assoc_double(return_value,"lng", dlng);
}

PHP_METHOD(Lbs, gcj2wgs)
{
	double lat,lng,dlat,dlng;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(lat)
		Z_PARAM_DOUBLE(lng)
	ZEND_PARSE_PARAMETERS_END();

	GCJ2WGS(lat,lng,&dlat,&dlng);
	array_init(return_value);
	add_assoc_double(return_value,"lat", dlat);
	add_assoc_double(return_value,"lng", dlng);
}

ZEND_BEGIN_ARG_INFO(arginfo_class_Lbs_bd2wgs, 0)
	ZEND_ARG_TYPE_INFO(0, latitude, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, longitude, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry class_Lbs_methods[] = {
    ZEND_ME(Lbs, bd2wgs, arginfo_class_Lbs_bd2wgs,   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_ME(Lbs, wgs2bd, arginfo_class_Lbs_bd2wgs,   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_ME(Lbs, wgs2gcj, arginfo_class_Lbs_bd2wgs,   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_ME(Lbs, gcj2wgs, arginfo_class_Lbs_bd2wgs,   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FE_END
};

// 创建对象
static zend_object * mexti_lbs_object_new(zend_class_entry *class_type)
{
    mexti_lbs_t * obj = zend_object_alloc(sizeof(mexti_lbs_t), class_type);
    zend_object_std_init(&obj->std, class_type);
	object_properties_init(&obj->std, class_type);

    obj->std.handlers = &lbs_ce_handlers;

    return &obj->std;
}

// 释放对象
static void mexti_lbs_free_object(zend_object *object)
{
    mexti_lbs_t * obj = mexti_lbs_from_obj(object);
    // 释放所有标准属性.
    zend_object_std_dtor(&obj->std);

}

zend_class_entry * register_class_Lbs()
{
	 zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "mexti\\Lbs", class_Lbs_methods);
	//class_entry = zend_register_internal_interface(&ce);
    class_entry = zend_register_internal_class_ex(&ce, NULL);
	//class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	//zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);
    class_entry->create_object          = mexti_lbs_object_new;

    memcpy(&lbs_ce_handlers, zend_get_std_object_handlers(), sizeof(lbs_ce_handlers));

    lbs_ce_handlers.offset          = XtOffsetOf(mexti_lbs_t, std);
    //lbs_ce_handlers.dtor_obj        = mexti_lbs_dtor_object;        /* This is the destroy handler */
    lbs_ce_handlers.free_obj        = mexti_lbs_free_object;        /* This is the free handler */
    

	return class_entry;
}