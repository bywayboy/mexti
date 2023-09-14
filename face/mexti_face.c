
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "mexti_face.h"
#include "php_mexti.h"

#include "lib/face/zzFaceDLL.h"
#include "lib/face/mxImageTool.h"
#include "lib/face/MXFaceInfoEx.h"

#define FACEALF_MASK_QUALITY	(1 << 0)
#define FACEALF_MASK_MASK		(1 << 1)
#define FACEALF_MASK_LIVENESS	(1 << 2)

PHPAPI zend_class_entry     * mexti_ce_Face;
       zend_object_handlers   face_ce_handlers;

typedef struct mexti_face_t
{
    int nImgWidth;
	int nImgHeight;
	int nError;
	int nMaxFaceNum;
	char * bin;
	int iSizeBin;
	MXFaceInfoEx FaceInfo[100];
	int mask[100];
    /* data */
    zend_object std;
}mexti_face_t;

static inline mexti_face_t * mexti_face_from_obj(zend_object * obj) /* {{{ */ {
	return (mexti_face_t*)((char*)(obj) - XtOffsetOf(mexti_face_t, std));
}
#define Z_FACE_P(zv)  mexti_face_from_obj(Z_OBJ_P((zv)))

PHP_METHOD(Face, __construct)
{
    char * image;
	size_t limage;
    zend_long maxNum;
    bool maxNum_isNull;
    mexti_face_t * face = Z_FACE_P(ZEND_THIS);

    ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STRING(image, limage)
    Z_PARAM_LONG_EX(maxNum, maxNum_isNull, 0, 0)
	ZEND_PARSE_PARAMETERS_END();

    if(maxNum_isNull){
        maxNum  = 100;
    }else{
        maxNum = maxNum > 100 ? 100 : (maxNum < 1 ? 1 : maxNum);
    }

    int ret = ImageDecode(image, (int)limage, NULL, &face->nImgWidth, &face->nImgHeight);
    if(0 == ret){
        face->iSizeBin = face->nImgWidth * face->nImgHeight * 3;
        face->bin = emalloc(face->iSizeBin);

        ret = ImageDecode(image, (int)limage, face->bin, &face->nImgWidth, &face->nImgHeight);
        if(0 <= ret) {
            zend_update_property_long(mexti_ce_Face, &face->std, ZEND_STRL("width"), face->nImgWidth);
		    zend_update_property_long(mexti_ce_Face, &face->std, ZEND_STRL("height"), face->nImgHeight);

            ret = zzDetectFaceThread(MEXTI_G(pAlgEngine), face->bin, face->nImgWidth, face->nImgHeight, &face->nMaxFaceNum, &face->FaceInfo[0]);
            if(0 == ret){
                if(face->nMaxFaceNum > maxNum){
                    zend_throw_exception(NULL, "too much face!", ret);
                }
                zend_update_property_long(mexti_ce_Face, &face->std, ZEND_STRL("num"), face->nMaxFaceNum);
                return;
            }
            zend_throw_exception(NULL, "detect face failed!", ret);
        }
    }
    zend_throw_exception(NULL, "Decode Image Failed!", ret);
}

// 获取图像质量最好的人脸索引.
PHP_METHOD(Face, best)
{
    int i, quality = 0, idx = -1;
    mexti_face_t * face = Z_FACE_P(ZEND_THIS);
    for(i=0; i < face->nMaxFaceNum; i ++){
        if(!(face->mask[i] & FACEALF_MASK_QUALITY)){
            if(0 == zzFaceQualityThread(MEXTI_G(pAlgEngine), face->bin, face->nImgWidth, face->nImgHeight, 1, &face->FaceInfo[i])){
                face->mask[i] |= FACEALF_MASK_QUALITY;
            }
        }
        if(face->mask[i] & FACEALF_MASK_QUALITY){
            if(quality < face->FaceInfo[i].quality){
                quality = face->FaceInfo[i].quality;
                idx = i;
            }
        }
    }
    RETURN_LONG(i);
}

// 获取人脸特征
PHP_METHOD(Face, feature)
{
    int ret;
    zend_long iFaceIndex;
    mexti_face_t * face = Z_FACE_P(ZEND_THIS);

    ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(iFaceIndex)
	ZEND_PARSE_PARAMETERS_END();

    if(iFaceIndex >= face->nMaxFaceNum || iFaceIndex < 0){
        zend_throw_exception(NULL, "Bad Face Index!", ret);
        return;
    }

    size_t iFaceFeaSize = zzGetFeatureSizeThread(MEXTI_G(pAlgEngine));
	MXFaceInfoEx * info = &face->FaceInfo[iFaceIndex];
	unsigned char * pFaceFea = emalloc(iFaceFeaSize);
	if(0 == (ret  = zzExtractFeatureThread(MEXTI_G(pAlgEngine), face->bin, face->nImgWidth, face->nImgHeight, 1, info, pFaceFea))){
        RETVAL_STRINGL(pFaceFea, iFaceFeaSize);
    }
    zend_throw_exception(NULL, "Get Face Feature failed!", ret);
}

PHP_METHOD(Face, compare)
{
    float score = 0;
    char * featureA, * featureB;
	size_t lFeatureA, lFeatureB;

    ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STRING(featureA, lFeatureA)
	Z_PARAM_STRING(featureB, lFeatureB)
	ZEND_PARSE_PARAMETERS_END();

    if(0 == zzMatchFeatureThread(MEXTI_G(pAlgEngine), (unsigned char *)featureA, (unsigned char *)featureB, &score)){
		RETURN_DOUBLE(score);
	}
	RETURN_FALSE;
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_Face___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, ImageData, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Face__best, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Face__compare, 0, 0, 0)
    ZEND_ARG_TYPE_INFO(0, Feature1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, Feature2, IS_STRING, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry class_Face_methods[] = {
    PHP_ME(Face, __construct, arginfo_Face___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Face, best, arginfo_Face__best, ZEND_ACC_PUBLIC)
    PHP_ME(Face, compare, arginfo_Face__compare, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FE_END
};

// 创建对象
static zend_object * mexti_face_object_new(zend_class_entry *class_type)
{
    mexti_face_t * obj = zend_object_alloc(sizeof(mexti_face_t), class_type);
    
    zend_object_std_init(&obj->std, class_type);
	object_properties_init(&obj->std, class_type);

    obj->std.handlers = &face_ce_handlers;

    return &obj->std;
}

// 释放对象
static void mexti_face_free_object(zend_object *object)
{
    mexti_face_t * obj = mexti_face_from_obj(object);

    if(NULL != obj->bin){
		efree(obj->bin);
	}

    // 释放所有标准属性.
    zend_object_std_dtor(&obj->std);
}

zend_class_entry * register_class_Face()
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "mexti\\Face", class_Face_methods);
	//class_entry = zend_register_internal_interface(&ce);
    mexti_ce_Face = zend_register_internal_class_ex(&ce, NULL);
	//class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	//zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);
    mexti_ce_Face->create_object          = mexti_face_object_new;

    memcpy(&face_ce_handlers, zend_get_std_object_handlers(), sizeof(face_ce_handlers));

    face_ce_handlers.offset          = XtOffsetOf(mexti_face_t, std);
    //face_ce_handlers.dtor_obj        = mexti_face_dtor_object;        /* This is the destroy handler */
    face_ce_handlers.free_obj        = mexti_face_free_object;        /* This is the free handler */
    
    zend_declare_class_constant_stringl(mexti_ce_Face, ZEND_STRL("version"), ZEND_STRL("v5.4.7"));

    // 图像宽高
	zend_declare_property_long(mexti_ce_Face, ZEND_STRL("width"), 0, ZEND_ACC_READONLY | ZEND_ACC_PUBLIC);
	zend_declare_property_long(mexti_ce_Face, ZEND_STRL("height"), 0, ZEND_ACC_READONLY | ZEND_ACC_PUBLIC);

    // 人脸数目
	zend_declare_property_long(mexti_ce_Face, ZEND_STRL("num"), 0, ZEND_ACC_READONLY | ZEND_ACC_PUBLIC);
	zend_declare_property_long(mexti_ce_Face, ZEND_STRL("errCode"), 0, ZEND_ACC_READONLY | ZEND_ACC_PUBLIC);

	return mexti_ce_Face;
}