
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "mexti_finger.h"
#include "php_mexti.h"

#include "lib/base64.h"
#include "lib/finger/miaxis_api.h"
#include "lib/finger/miaxis_api_id.h"

PHPAPI zend_class_entry     * mexti_ce_Finger, * mexti_ce_FingerException;
       zend_object_handlers   finger_ce_handlers;

typedef struct mexti_finger_t
{
    /* data */
    zend_object std;
}mexti_finger_t;

static inline mexti_finger_t * mexti_finger_from_obj(zend_object * obj) /* {{{ */ {
	return (mexti_finger_t*)((char*)(obj) - XtOffsetOf(mexti_finger_t, std));
}
#define Z_FACE_P(zv)  mexti_finger_from_obj(Z_OBJ_P((zv)))

#define arraycopy(src, srcpos, dest, destpos, length)	memcpy(dest + destpos , src + srcpos, length)
#define array_fill(arr, from, to, var)	memset(arr + from, to - from, var)

// 静态函数.
PHP_METHOD(Finger, compress)
{
    char * sfinger_base64;
	size_t lfinger_base64;
    int fplen, rawPos, highPos;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_STRING(sfinger_base64, lfinger_base64)
	ZEND_PARSE_PARAMETERS_END();

    size_t lhexFp = ceil(lfinger_base64 * 3 / 4);  // 计算base64_decode 后的数据长度
	unsigned char * hexFp = emalloc(lhexFp);
	unsigned char * compFp = emalloc(512); //正常情况 压缩后是小于256字节的, 极端情况 可能压缩不到256字节, 申请512字节内存,避免内存越界.

	base64_decode(hexFp, lhexFp, &lhexFp, sfinger_base64, lfinger_base64);
	if(lhexFp != 512) RETURN_FALSE;
	if(hexFp[0] != 0x43 || hexFp[4] != 0x01) RETURN_FALSE;
	
	arraycopy(hexFp, 1, compFp, 0, 3);
	arraycopy(hexFp, 5, compFp, 3, 2);

	compFp[5] = hexFp[19];
	if (compFp[5] > 76) {
		compFp[5] = 76;
	}
	
	fplen = 6;
	rawPos = 31;
	unsigned char i;
	for (i=0; i<compFp[5]; i+=4){
		highPos = fplen;
		compFp[highPos] = 0;
		fplen++;
		
		arraycopy(hexFp, rawPos, compFp, fplen, 3);
		compFp[highPos] = (hexFp[rawPos+3]&3);
		rawPos += 4;
		fplen += 3;
		
		if (i+1 < compFp[5]){
			arraycopy(hexFp, rawPos, compFp, fplen, 3);
			compFp[highPos] |= (hexFp[rawPos+3]&3)<<2;
			rawPos += 4;
			fplen += 3;
		}

		if (i+2 < compFp[5]){
			arraycopy(hexFp, rawPos, compFp, fplen, 3);
			compFp[highPos] |= (hexFp[rawPos+3]&3)<<4;
			rawPos += 4;
			fplen += 3;
		}

		if (i+1 < compFp[5]){
			arraycopy(hexFp, rawPos, compFp, fplen, 3);
			compFp[highPos] |= (hexFp[rawPos+3]&3)<<6;
			rawPos += 4;
			fplen += 3;
		}
	}
	efree(hexFp);
	RETVAL_STRINGL(compFp, fplen);
	efree(compFp);
}

// 静态函数.
PHP_METHOD(Finger, decompress)
{
    char * compFinger;
	size_t lcompFinger;
	int tzLen, rawPos, compPos;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_STRING(compFinger, lcompFinger)
	ZEND_PARSE_PARAMETERS_END();

    unsigned char * rawFinger = (unsigned char *)ecalloc(0, 512);
	rawFinger[0] = 0x43;
	arraycopy(compFinger, 0, rawFinger, 1, 3);
	rawFinger[4] = 1;
	arraycopy(compFinger, 3, rawFinger, 5, 2);
	array_fill(rawFinger, 7, 19, (unsigned char)0xff);
	rawFinger[19] = compFinger[5];
	tzLen = compFinger[5]*4+9;
	rawFinger[20] = (unsigned char)(tzLen>>8);
	rawFinger[21] = (unsigned char)(tzLen&0xff);
	array_fill(rawFinger, 22, 31, (unsigned char)0);
	
	compPos = 6;
	rawPos = 31;
	int i;
	for (i=0; i<compFinger[5]; i+=4){
		unsigned char compHA = compFinger[compPos];
		compPos++;
		
		arraycopy(compFinger, compPos, rawFinger, rawPos, 3);
		rawFinger[rawPos+3] = (unsigned char)((compHA&3)|0xfc);
		compPos += 3;
		rawPos += 4;
		
		if (i+1 < compFinger[5]){
			arraycopy(compFinger, compPos, rawFinger, rawPos, 3);
			rawFinger[rawPos+3] = (unsigned char)(((compHA>>2)&3)|0xfc);
			compPos += 3;
			rawPos += 4;
		}
		
		if (i+2 < compFinger[5]){
			arraycopy(compFinger, compPos, rawFinger, rawPos, 3);
			rawFinger[rawPos+3] = (unsigned char)(((compHA>>4)&3)|0xfc);
			compPos += 3;
			rawPos += 4;
		}

		if (i+3 < compFinger[5]){
			arraycopy(compFinger, compPos, rawFinger, rawPos, 3);
			rawFinger[rawPos+3] = (unsigned char)(((compHA>>6)&3)|0xfc);
			compPos += 3;
			rawPos += 4;
		}
	}

	rawFinger[511] = crc8(rawFinger, 511);
	size_t lencodeFinger = (4 + (512 * 4 / 3));
	unsigned char * encodeFinger = emalloc(lencodeFinger);

	base64_encode(encodeFinger, lencodeFinger, &lencodeFinger, rawFinger, 512);
	efree(rawFinger);
	RETVAL_STRINGL(encodeFinger, lencodeFinger);
	efree(encodeFinger);
}

PHP_METHOD(Finger, compare){
    char * fingerA = NULL, * fingerB = NULL;
	size_t lFingerA = 0, lFingerB = 0;
	zend_long level;

    ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_STRING(fingerA, lFingerA)
	Z_PARAM_STRING(fingerB, lFingerB)
	Z_PARAM_LONG(level)
	ZEND_PARSE_PARAMETERS_END();

    if(lFingerA != 512 || lFingerB != 512){
		zend_throw_exception_ex(mexti_ce_FingerException, 1000, "指纹大小必须是%d字节\n", 512);
		RETURN_FALSE;
	}

	if(0 == mxFingerMatch512_id(fingerA, fingerB, level)) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

PHP_METHOD(Finger, compare344){

    char * fingerA = NULL, * fingerB = NULL;
	size_t lFingerA = 0, lFingerB = 0;
	zend_long level;
	int r;

    ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_STRING(fingerA, lFingerA)
	Z_PARAM_STRING(fingerB, lFingerB)
	Z_PARAM_LONG(level)
	ZEND_PARSE_PARAMETERS_END();

    if(lFingerA != 344 || lFingerB != 344){
		zend_throw_exception_ex(mexti_ce_FingerException, 1000, "指纹大小必须是%d字节\n", 344);
		RETURN_FALSE;
	}

	if(0 == (r = mxFingerMatchBase64(fingerA, fingerB, (int)level))) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

PHP_METHOD(Finger, feature)
{
    char * Pixels = NULL, tzbuffer[512];;
	size_t lPixels = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STRING(Pixels, lPixels)
	ZEND_PARSE_PARAMETERS_END();
    switch(lPixels){
    case 256 * 360:
        if(0 == mxGetTz512From256X360_id(Pixels, tzbuffer)){
            RETURN_STRINGL(tzbuffer, 512);
        }
        RETURN_NULL();
    case 152 * 200:
        if(0 == mxGetTz512From152X200_id(Pixels, tzbuffer)) {
            RETURN_STRINGL(tzbuffer, 512);
        }
        RETURN_NULL();
    default:
        zend_throw_exception_ex(mexti_ce_FingerException, 1001, "Wrong finger pixel size(%d), the correct size is: %d or %d.\n", lPixels, 256 * 360, 152 * 200);
    }
	RETURN_NULL();
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_Finger__compress, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, ImageData, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Finger__best, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Finger__compare, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, Feature1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, Feature2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Finger__init, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Finger__feature, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, pixelData, IS_STRING, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry class_Finger_methods[] = {
    PHP_ME(Finger, compress, arginfo_Finger__compress, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Finger, decompress, arginfo_Finger__compress, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Finger, compare, arginfo_Finger__compare, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Finger, compare344, arginfo_Finger__compare, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Finger, feature, arginfo_Finger__feature, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FE_END
};

static const zend_function_entry class_FingerException_methods[] = {
    ZEND_FE_END
};

// 创建对象
static zend_object * mexti_finger_object_new(zend_class_entry *class_type)
{
    mexti_finger_t * obj = zend_object_alloc(sizeof(mexti_finger_t), class_type);
    
    zend_object_std_init(&obj->std, class_type);
	object_properties_init(&obj->std, class_type);

    obj->std.handlers = &finger_ce_handlers;

    return &obj->std;
}

// 释放对象
static void mexti_finger_free_object(zend_object *object)
{
    mexti_finger_t * obj = mexti_finger_from_obj(object);

    // 释放所有标准属性.
    zend_object_std_dtor(&obj->std);
}

zend_class_entry * register_class_Finger()
{
	zend_class_entry ce, cee;

	INIT_CLASS_ENTRY(ce, "mexti\\Finger", class_Finger_methods);
    INIT_CLASS_ENTRY(cee, "mexti\\FingerException", class_FingerException_methods);
	//class_entry = zend_register_internal_interfinger(&ce);
    mexti_ce_Finger = zend_register_internal_class_ex(&ce, NULL);
    mexti_ce_FingerException = zend_register_internal_class_ex(&cee, zend_ce_exception);
	//class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	//zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);
    mexti_ce_Finger->create_object          = mexti_finger_object_new;

    memcpy(&finger_ce_handlers, zend_get_std_object_handlers(), sizeof(finger_ce_handlers));

    finger_ce_handlers.offset          = XtOffsetOf(mexti_finger_t, std);
    //finger_ce_handlers.dtor_obj        = mexti_finger_dtor_object;        /* This is the destroy handler */
    finger_ce_handlers.free_obj        = mexti_finger_free_object;        /* This is the free handler */

	return mexti_ce_Finger;
}