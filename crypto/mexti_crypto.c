
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "lib/crypto/sm3.h"

PHPAPI zend_class_entry     * mexti_ce_Crypto;
       zend_object_handlers   crypto_ce_handlers;

typedef struct mexti_crypto_t
{
    /* data */
    zend_object std;
}mexti_crypto_t;

static inline mexti_crypto_t *mexti_crypto_from_obj(zend_object * obj) /* {{{ */ {
	return (mexti_crypto_t*)((char*)(obj) - XtOffsetOf(mexti_crypto_t, std));
}
#define Z_MINHEAP_P(zv)  mexti_crypto_from_obj(Z_OBJ_P((zv)))

static char hexDigit[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
PHP_METHOD(Crypto, sm3)
{
    char * buffer = NULL, out[32];
    size_t size;
    struct sm3_state ctx;
    bool raw;
    
    ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(buffer, size);
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL_EX(raw, _dummy, 0, 0)
	ZEND_PARSE_PARAMETERS_END();
    
    sm3_init(&ctx);
    sm3_update(&ctx, (uint8_t*) buffer, (uint32_t)size);
    sm3_final(&ctx, &out[0]);

    if(!raw){
        char hexout[64];
        size_t p = 0;
        for(size=0; size<32; size++){
            hexout[ p ++ ] = hexDigit[ (uint8_t)(out[size]) >> 4];
            hexout[ p ++ ] = hexDigit[ (uint8_t)(out[size]) & 0xF];
        }
        RETURN_STRINGL(hexout, 64);
    }
    RETURN_STRINGL(out, 32);
    
}
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Crypto_sm3, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, code, IS_STRING, 0)
    ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

static const zend_function_entry class_Crypto_methods[] = {
    ZEND_ME(Crypto, sm3, arginfo_class_Crypto_sm3,   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FE_END
};

// 创建对象
static zend_object * mexti_crypto_object_new(zend_class_entry *class_type)
{
    mexti_crypto_t * obj = zend_object_alloc(sizeof(mexti_crypto_t), class_type);
    zend_object_std_init(&obj->std, class_type);
	object_properties_init(&obj->std, class_type);

    obj->std.handlers = &crypto_ce_handlers;

    return &obj->std;
}

// 释放对象
static void mexti_crypto_free_object(zend_object *object)
{
    mexti_crypto_t * obj = mexti_crypto_from_obj(object);
    // 释放所有标准属性.
    zend_object_std_dtor(&obj->std);

}

zend_class_entry * register_class_Crypto()
{
	 zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "mexti\\Crypto", class_Crypto_methods);
	//class_entry = zend_register_internal_interface(&ce);
    class_entry = zend_register_internal_class_ex(&ce, NULL);
	//class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	//zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);
    class_entry->create_object          = mexti_crypto_object_new;

    memcpy(&crypto_ce_handlers, zend_get_std_object_handlers(), sizeof(crypto_ce_handlers));

    crypto_ce_handlers.offset          = XtOffsetOf(mexti_crypto_t, std);
    //crypto_ce_handlers.dtor_obj        = mexti_crypto_dtor_object;        /* This is the destroy handler */
    crypto_ce_handlers.free_obj        = mexti_crypto_free_object;        /* This is the free handler */
    

	return class_entry;
}