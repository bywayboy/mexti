
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "mexti_face.h"
#include "php_mexti.h"

#include "lib/crypto/xxtea.h"

#include "lib/face/zzFaceDLL.h"
#include "lib/face/mxImageTool.h"
#include "lib/face/MXFaceInfoEx.h"


#define FACEALF_MASK_QUALITY	(1 << 0)
#define FACEALF_MASK_MASK		(1 << 1)
#define FACEALF_MASK_LIVENESS	(1 << 2)

static const char * faceErrorString(int code){
    switch(code){
    case 1: return "校验失败";
    case 11: return "初始化失败";
    case 12: return "未初始化";
    case 13: return "人脸检测失败";
    case 14: return "关键点定位失败";
    case 15: return "多张人脸";
    case 16: return "去网纹失败";
    case 17: return "特征格式校验失败";
    case 18: return "人脸特征提取失败";
    case 19: return "人脸比对失败";
    case 20: return "模型文件不存在";
    case 21: return "授权文件不合法（系统与授权文件不匹配）";
    case 22: return "授权过期";
    case 23: return "人脸个数超过额定值（100）或者授权算法类型不一致";
    case 24: return "检测模型不存在";
    case 25: return "质量评价模型不存在";
    case 26: return "识别模型不存在";
    case 27: return "待检测人脸图像小于 100x100";
    case 31: return "图像质量评价失败";
    case 32: return "图像质量评价失败";
    case 100: return "授权过期或无效";
    case 101: return "授权文件非法";
    case 102: return "授权文件不找到";
    case 103: return "授权 UKey 无效";
    case 1000: return "人脸索引超出范围.";
    default:
        return "未知错误.";
    }
}

PHPAPI zend_class_entry     * mexti_ce_Face, * mexti_ce_FaceException;
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
    char * image = NULL;
	size_t limage = 0;
    zend_long maxNum;
    bool maxNum_isNull;
    mexti_face_t * face = Z_FACE_P(ZEND_THIS);

    if(0 != MEXTI_G(nError)){
        zend_throw_exception(mexti_ce_FaceException, faceErrorString(MEXTI_G(nError)), MEXTI_G(nError));
    }

    ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STRING(image, limage)
    Z_PARAM_OPTIONAL
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
                    zend_throw_exception(mexti_ce_FaceException, faceErrorString(ret), ret);
                }
                zend_update_property_long(mexti_ce_Face, &face->std, ZEND_STRL("num"), face->nMaxFaceNum);
                return;
            }
            zend_throw_exception(mexti_ce_FaceException, faceErrorString(ret), ret);
        }
    }
    zend_throw_exception(mexti_ce_FaceException, "Decode Image Failed!", ret);
}

// 获取图像质量最好的人脸索引.
PHP_METHOD(Face, best)
{
    int i, quality = 0, idx = -1;
    mexti_face_t * face = Z_FACE_P(ZEND_THIS);

    for(i=0; i < face->nMaxFaceNum; i ++) {
        if(!(face->mask[i] & FACEALF_MASK_QUALITY)) {
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
    RETURN_LONG(idx);
}

// 获取制定索引的图像
PHP_METHOD(Face, quality)
{
    zend_long index;
    mexti_face_t * face = Z_FACE_P(ZEND_THIS);

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();

    if(index < 0 || index > face->nMaxFaceNum - 1){
        zend_throw_exception(mexti_ce_FaceException, faceErrorString(1000), 1000);
    }

    if(!(face->mask[index] & FACEALF_MASK_QUALITY)) {
        if(0 == zzFaceQualityThread(MEXTI_G(pAlgEngine), face->bin, face->nImgWidth, face->nImgHeight, 1, &face->FaceInfo[index])){
            face->mask[index] |= FACEALF_MASK_QUALITY;
        }
    }

    if(face->mask[index] & FACEALF_MASK_QUALITY){
        RETURN_LONG(face->FaceInfo[index].quality);
    }
    RETURN_LONG(0);
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

    if(iFaceIndex < 0 || iFaceIndex > face->nMaxFaceNum - 1){
        zend_throw_exception(mexti_ce_FaceException, faceErrorString(1000), 1000);
    }

    size_t iFaceFeaSize = zzGetFeatureSizeThread(MEXTI_G(pAlgEngine));
	unsigned char * pFaceFea = emalloc(iFaceFeaSize);

	if(0 == (ret  = zzExtractFeatureThread(MEXTI_G(pAlgEngine), face->bin, face->nImgWidth, face->nImgHeight, 1, &face->FaceInfo[iFaceIndex], pFaceFea))){
        RETVAL_STRINGL_FAST(pFaceFea, iFaceFeaSize);
        efree(pFaceFea);
        return;
    }
    zend_throw_exception(mexti_ce_FaceException, faceErrorString(ret), ret);
}

// 获取人脸特征
PHP_METHOD(Face, liveness)
{
    int ret;
    zend_long iFaceIndex;
    mexti_face_t * face = Z_FACE_P(ZEND_THIS);

    ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(iFaceIndex)
	ZEND_PARSE_PARAMETERS_END();

    if(iFaceIndex < 0 || iFaceIndex > face->nMaxFaceNum - 1){
        zend_throw_exception(mexti_ce_FaceException, faceErrorString(1000), 1000);
    }

    if(!(face->mask[iFaceIndex] & FACEALF_MASK_LIVENESS)) {
        if(0 == (ret  = zzVisLivenessDetectThread(MEXTI_G(pAlgEngine), face->bin, face->nImgWidth, face->nImgHeight, 1, &face->FaceInfo[iFaceIndex]))){
            face->mask[iFaceIndex] |= FACEALF_MASK_LIVENESS;
        }
    }

    if((face->mask[iFaceIndex] & FACEALF_MASK_LIVENESS)) {
        RETURN_LONG(face->FaceInfo[iFaceIndex].liveness);
    }

    RETURN_LONG(0);
}

// 静态函数.
PHP_METHOD(Face, compare)
{
    float score = 0;
    char * featureA, * featureB;
	size_t lFeatureA, lFeatureB;

    if(0 != MEXTI_G(nError)){
        zend_throw_exception(mexti_ce_FaceException, faceErrorString(MEXTI_G(nError)), MEXTI_G(nError));
    }

    ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STRING(featureA, lFeatureA)
	Z_PARAM_STRING(featureB, lFeatureB)
	ZEND_PARSE_PARAMETERS_END();

    if(0 == zzMatchFeatureThread(MEXTI_G(pAlgEngine), (unsigned char *)featureA, (unsigned char *)featureB, &score)){
		RETURN_DOUBLE(score);
	}
	RETURN_FALSE;
}

// 静态函数
PHP_METHOD(Face, init)
{
    char * license = NULL;
    size_t lLicense;
    zend_long iSearchNum;
    bool iSearchNum_isNull;

    if(NULL != MEXTI_G(pAlgEngine)) {
        RETURN_TRUE;
    }

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(license, lLicense)
        Z_PARAM_LONG_EX(iSearchNum, iSearchNum_isNull, 0, 0)
    ZEND_PARSE_PARAMETERS_END();

    if(iSearchNum_isNull || iSearchNum > 5000) {
        iSearchNum = 5000;
    }

    if(NULL == license){
        license = MEXTI_G(license);
    }

    if(NULL != MEXTI_G(pAlgEngine)){
        zzFreeAlgThread(MEXTI_G(pAlgEngine));
        MEXTI_G(pAlgEngine) = NULL;
    }

    MEXTI_G(pAlgEngine) = zzInitAlgNThread(MEXTI_G(license), iSearchNum, &MEXTI_G(nError));

    if(0 != MEXTI_G(nError)){
        zend_throw_exception(mexti_ce_FaceException, faceErrorString(MEXTI_G(nError)), MEXTI_G(nError));
    }
    RETURN_TRUE;
}

// 静态函数
PHP_METHOD(Face, decode) {
    char * buffer;
    size_t lBuffer;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
	    Z_PARAM_STRING(buffer, lBuffer)
	ZEND_PARSE_PARAMETERS_END();

    if(NULL == buffer || 0 == lBuffer){
        RETURN_NULL();
    }

    if(lBuffer <= 2){
        RETURN_STRINGL_FAST(buffer, lBuffer);
    }

    if((uint8_t)(buffer[0]) == 0xFE && (uint8_t)(buffer[1]) == 0xAA){
        xxtea_long new_size = 0;
		char * newImage = xxtea_decrypt_file((unsigned char *)buffer, lBuffer, &new_size);
        RETVAL_STRINGL_FAST(newImage, new_size);
        efree(newImage);
        return;
    }
    RETVAL_STRINGL_FAST(buffer, lBuffer);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_Face___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, ImageData, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, MaxFaceNum, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Face__best, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Face__compare, 0, 0, 0)
    ZEND_ARG_TYPE_INFO(0, Feature1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, Feature2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Face__quality, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, faceIndex, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Face__init, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Face_decode, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, faceIndex, IS_STRING, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry class_Face_methods[] = {
    PHP_ME(Face, __construct, arginfo_Face___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Face, best, arginfo_Face__best, ZEND_ACC_PUBLIC)
    PHP_ME(Face, quality, arginfo_Face__quality, ZEND_ACC_PUBLIC)
    PHP_ME(Face, feature, arginfo_Face__quality, ZEND_ACC_PUBLIC)
    PHP_ME(Face, liveness, arginfo_Face__quality, ZEND_ACC_PUBLIC)
    PHP_ME(Face, compare, arginfo_Face__compare, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Face, init, arginfo_Face__init, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Face, decode, arginfo_Face_decode, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FE_END
};

static const zend_function_entry class_FaceException_methods[] = {
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
	zend_class_entry ce, cee;


	INIT_CLASS_ENTRY(ce, "mexti\\Face", class_Face_methods);
    INIT_CLASS_ENTRY(cee, "mexti\\FaceException", class_FaceException_methods);
	//class_entry = zend_register_internal_interface(&ce);
    mexti_ce_Face = zend_register_internal_class_ex(&ce, NULL);
    mexti_ce_FaceException = zend_register_internal_class_ex(&cee, zend_ce_exception);
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