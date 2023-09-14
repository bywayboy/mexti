#pragma once
#if defined(_WIN32) || defined(_WIN64)
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#else
#define WINAPI
#endif

#include"MXFaceInfoEx.h"

#if defined(__linux__) 
#ifdef __cplusplus
extern "C" {
#endif
#endif
/****************************************************************************
功  能：	算法版本
参  数：	szVersion - 输入，版本信息
返	回：	0-成功，其他-失败
*****************************************************************************/
int WINAPI zzGetAlgVersionThread(char *szVersion);

/****************************************************************************
功  能：	初始化算法
参  数：	szConfigFile  - 输入，模型文件所在目录路径
			Error         - 输入/输出，模型加载情况，0-成功，其他失败
返	回：	引擎
*****************************************************************************/
void* WINAPI zzInitAlgThread(char *szConfigFile, int *Error);

/****************************************************************************
功  能：	初始化算法
参  数：	szConfigFile  - 输入，模型文件所在目录路径
			iMaxSearchNum - 输入，最大搜索库容
			Error         - 输入/输出，模型加载情况，0-成功，其他失败
返	回：	引擎
*****************************************************************************/
void* WINAPI zzInitAlgNThread(char *szConfigFile, int iMaxSearchNum, int *Error);

/****************************************************************************
功  能：	释放算法
参  数：    pAlgEngine - 输入，引擎
返	回：	0-成功，其他-失败
*****************************************************************************/
int WINAPI zzFreeAlgThread(void *pAlgEngine);

/****************************************************************************
功  能：	特征长度
参  数：	pAlgEngine - 输入，引擎
返	回：	特征长度
*****************************************************************************/
int WINAPI zzGetFeatureSizeThread(void *pAlgEngine);

/*******************************************************************************************
功	能：	检测人脸
参	数：	pAlgEngine  - 输入，引擎
			pImage      - 输入，RGB图像数据
			nImgWidth   - 输入，图像宽度
			nImgHeight  - 输入，图像高度
			pFaceNum    - 输出，人脸数
			pFaceInfo   - 输出，人脸检测结果
返	回：	0-成功，其他-失败
备  注：    最多检测100人
*******************************************************************************************/
int WINAPI zzDetectFaceThread(void *pAlgEngine, unsigned char *pImage, int nImgWidth, int nImgHeight, int* pFaceNum, MXFaceInfoEx *pFaceInfo);

/*******************************************************************************************
功	能：	提取人脸特征
参	数：	pAlgEngine  - 输入，引擎
			pImage      - 输入，RGB图像数据
			nImgWidth   - 输入，图像宽度
			nImgHeight  - 输入，图像高度
			pFaceInfo	- 输入，人脸位置信息
			pFaceFea    - 输出，人脸特征
返	回：	0-成功，其他-失败
*******************************************************************************************/
int WINAPI zzExtractFeatureThread(void *pAlgEngine, unsigned char *pImage, int nImgWidth, int nImgHeight, int pFaceNum, MXFaceInfoEx *pFaceInfo, unsigned char *pFaceFea);

/*******************************************************************************************
功	能：	比对人脸特征
参	数：	pAlgEngine - 输入，引擎
			pFaceFeaA  - 输入，人脸特征A
			pFaceFeaB  - 输入，人脸特征B
			dScore     - 输出，相似性度量值，0~1.0 ，越大越相似。
返	回：	0-成功，其他-失败
备	注：	比对推荐阈值：0.76
*******************************************************************************************/
int WINAPI zzMatchFeatureThread(void *pAlgEngine, unsigned char *pFaceFeaA, unsigned char *pFaceFeaB, float *fScore);

/*******************************************************************************************
功	能：	检测人脸(跟踪人脸)，用于动态视频检测
参	数：	pAlgEngine  - 输入，引擎
			pImage      - 输入，RGB图像数据
			nImgWidth   - 输入，图像宽度
			nImgHeight  - 输入，图像高度
			pFaceNum    - 输入/输出，人脸数，须申请为全局变量
			pFaceInfo   - 输入/输出，人脸检测结果，须申请为全局变量
返	回：	0-成功，其他-失败
*******************************************************************************************/
int WINAPI zzTrackFaceThread(void *pAlgEngine, unsigned char *pImage, int nImgWidth, int nImgHeight, int* pFaceNum, MXFaceInfoEx *pFaceInfo);

/*******************************************************************************************
功	能：	计算人脸质量，用于比对/识别
参	数：	pAlgEngine    - 输入，引擎
			pImage        - 输入，图像数据
			nImgWidth     - 输入，图像宽度
			nImgHeight    - 输入，图像高度
			pFaceNum      - 输入，人脸数
			pFaceInfo	  - 通过MXFaceInfoEx结构体的quality属性获取，建议大于65用于比对/识别
返	回：	0-成功，其他-失败
*******************************************************************************************/
int WINAPI zzFaceQualityThread(void *pAlgEngine, unsigned char *pImage, int nImgWidth, int nImgHeight, int pFaceNum, MXFaceInfoEx* pFaceInfo);

/*******************************************************************************************
功	能：	计算人脸质量，用于注册
参	数：	pAlgEngine  - 输入，引擎
			pImage      - 输入，RGB图像数据
			nImgWidth   - 输入，图像宽度
			nImgHeight  - 输入，图像高度
			pFaceNum    - 输入，人脸数，必须为1.
			pFaceInfo	- 输入，人脸信息
返	回：	质量分(0-100),建议大于90用于注册
*******************************************************************************************/
int WINAPI zzFaceQuality4RegisterThread(void *pAlgEngine, unsigned char *pImage, int nImgWidth, int nImgHeight, int pFaceNum, MXFaceInfoEx* pFaceInfo);

/*******************************************************************************************
功	能：	根据输入人脸特征与人脸模板集合，查找匹配人脸特征的序号
参	数：	pAlgEngine  - 输入，引擎
			pFaceFeaList - 输入，人脸模板集合(模板1+模板2+...+模板N)
			iFaceNum	 - 输入，人脸模板个数（建议小于5000）
			score        - 输入，比对分通过阈值（建议76）
			pFaceFea     - 输入，人脸特征
			pFaceInfo    - 输入/输出，通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，-1-识别中，其他-失败
*******************************************************************************************/
int WINAPI zzSearchFeatureThread(void *pAlgEngine, unsigned char *pFaceFeaList, int iFaceNum, int score, unsigned char *pFaceFea, MXFaceInfoEx* pFaceInfo);

/*******************************************************************************************
功	能：	根据输入人脸特征与人脸模板集合，查找匹配人脸特征的序号(同一人可含有多张图片)
参	数：	pAlgEngine	  - 输入，引擎
			pFaceFeaList  - 输入，人脸模板集合(模板1+模板2+...+模板N)
			iPicNum       - 输入，库中图片总数
			pFaceNumList  - 输入，顺序记录模板库中每个人的模板图片数(建议<=10)
			iPersonNum    - 输入，库中总人数(ps:模板库中一个人可能有多张模板图片，所以总人数小于或等于图片总数)
			score         - 输入，比对分通过阈值（建议76）
			pFaceFea      - 输入，人脸特征
			pFaceInfo     - 输入/输出，通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，-1-识别中，-2-陌生人，-3人脸模板个数过多，其他失败
*******************************************************************************************/
int WINAPI zzSearchNFeatureThread(void *pAlgEngine, unsigned char *pFaceFeaList, int iPictureNum, int *pFaceNumList, int iPersonNum, int score, unsigned char *pFaceFea, MXFaceInfoEx* pFaceInfo);

/*******************************************************************************************
	功	能：	根据输入人脸信息与人脸模板集合，查找匹配人脸特征的序号(同一人可含有多张图片)
	参	数：	pAlgEngine	  - 输入，引擎
				pFaceFeaList  - 输入，人脸模板集合(模板1+模板2+...+模板N)
				iPicNum       - 输入，库中图片总数
				pFaceNumList  - 输入，顺序记录模板库中每个人的模板图片数(建议<=10)
				iPersonNum    - 输入，库中总人数(ps:模板库中一个人可能有多张模板图片，所以总人数小于或等于图片总数)
				iMatchThreshold  - 输入，比对分通过阈值（建议76）
				iMatchThreshold  - 输入，图片质量评价阈值（建议50）
				pImage		  - 输入，RGB图像数据
				nImgWidth     - 输入，图像宽度
				nImgHeight    - 输入，图像高度
				iFaceNum      - 输入，人脸数，须申请为全局变量
				pFaceInfo     - 输入/输出，人脸检测结果，须申请为全局变量
								通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
	返	回：	0-成功，其他失败
	备	注：   配合zzTrackFaceThread一起使用
*******************************************************************************************/
int WINAPI zzSearchFaceThread(void *pAlgEngine,
							  unsigned char *pFaceFeaList, int iPictureNum, int *pFaceNumList, int iPersonNum,
							  int iMatchThreshold, int iQualityThreshold,
							  unsigned char *pImage, int nImgWidth, int nImgHeight, int iFaceNum, MXFaceInfoEx *pFaceInfo);

/*******************************************************************************************
功	能：	可见光人脸图像活体检测（指定型号摄像头效果最佳）
参	数：	pAlgEngine	  - 输入，引擎
			pVisImage   - 输入，可见光人脸图像
			nImgWidth   - 输入，图像宽度
			nImgHeight  - 输入，图像高度
			pFaceNum    - 输入，人脸数
			pFaceInfo   - 输入/输出，通过MXFaceInfoEx结构体的liveness属性获取，0~100，建议大于80表示活体
返	回：	0-成功，其他-失败
备	注：	需要先进行人脸检测，如果有戴口罩的需要进行戴口罩检测
*******************************************************************************************/
int WINAPI zzVisLivenessDetectThread(void *pAlgEngine,
									 unsigned char *pVisImage, int nImgWidth, int nImgHeight, int pFaceNum, MXFaceInfoEx* pFaceInfo);

/*******************************************************************************************
功	能：	近红外人脸图像活体检测（指定型号摄像头效果最佳）
参	数：	pAlgEngine	  - 输入，引擎
			pNirImage   - 输入，近红外人脸图像
			nImgWidth   - 输入，图像宽度
			nImgHeight  - 输入，图像高度
			pFaceNum    - 输入，人脸数
			pFaceInfo   - 输入/输出，通过MXFaceInfoEx结构体的liveness属性获取，0~100，建议大于80表示活体
返	回：	0-成功，其他-失败
备	注：	需要先进行人脸检测，如果有戴口罩的需要进行戴口罩检测
*******************************************************************************************/
int WINAPI zzNirLivenessDetectThread(void *pAlgEngine,
									 unsigned char *pNirImage, int nImgWidth, int nImgHeight, int pFaceNum, MXFaceInfoEx* pFaceInfo);

/*******************************************************************************************
功	能：检测人脸是否戴口罩
参	数：pRGBImage     - 输入，RGB图像数据
		nImgWidth     - 输入，图像宽度
		nImgHeight    - 输入，图像高度
		pFaceNum      - 输入/输出，人脸数
		pMXFaceInfoEx - 输入/输出，通过MXFaceInfoEx结构体的mask属性获取，0~100，建议大于40表示有口罩
返	回：0-成功，其他-失败
*******************************************************************************************/
int WINAPI zzFaceMaskThread(void *pAlgEngine, unsigned char *pRGBImage, int nImgWidth, int nImgHeight,
							int pFaceNum, MXFaceInfoEx* pMXFaceInfoEx);

/*******************************************************************************************
功	能：	提取人脸特征
参	数：	pAlgEngine  - 输入，引擎
			pImage      - 输入，RGB图像数据
			nImgWidth   - 输入，图像宽度
			nImgHeight  - 输入，图像高度
			pFaceInfo	- 输入，人脸位置信息
			pFaceFea    - 输出，人脸特征
返	回：	0-成功，其他-失败
*******************************************************************************************/
int WINAPI zzMaskExtractFeatureThread(void *pAlgEngine, unsigned char *pImage, int nImgWidth, int nImgHeight, int pFaceNum, MXFaceInfoEx *pFaceInfo, unsigned char *pFaceFea);

/*******************************************************************************************
功	能：	提取人脸特征，用于注册
参	数：	pAlgEngine  - 输入，引擎
			pImage      - 输入，RGB图像数据
			nImgWidth   - 输入，图像宽度
			nImgHeight  - 输入，图像高度
			pFaceInfo	- 输入，人脸位置信息
			pFaceFea    - 输出，人脸特征
返	回：	0-成功，其他-失败
*******************************************************************************************/
int WINAPI zzMaskExtractFeature4RegThread(void *pAlgEngine, unsigned char *pImage, int nImgWidth, int nImgHeight, int pFaceNum, MXFaceInfoEx *pFaceInfo, unsigned char *pFaceFea);

/*******************************************************************************************
功	能：	比对人脸特征
参	数：	pAlgEngine - 输入，引擎
pFaceFeaA  - 输入，人脸特征A
pFaceFeaB  - 输入，人脸特征B
dScore     - 输出，相似性度量值，0~1.0 ，越大越相似。
返	回：	0-成功，其他-失败
备	注：	比对推荐阈值：0.76
*******************************************************************************************/
int WINAPI zzMaskMatchFeatureThread(void *pAlgEngine, unsigned char *pFaceFeaA, unsigned char *pFaceFeaB, float *fScore);

/*******************************************************************************************
功	能：	根据输入人脸特征与人脸模板集合，查找匹配人脸特征的序号
参	数：	pAlgEngine  - 输入，引擎
pFaceFeaList - 输入，人脸模板集合(模板1+模板2+...+模板N)
iFaceNum	 - 输入，人脸模板个数（建议小于5000）
score        - 输入，比对分通过阈值（建议76）
pFaceFea     - 输入，人脸特征
pFaceInfo    - 输入/输出，通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，-1-识别中，其他-失败
*******************************************************************************************/
int WINAPI zzMaskSearchFeatureThread(void *pAlgEngine, unsigned char *pFaceFeaList, int iFaceNum, int score, unsigned char *pFaceFea, MXFaceInfoEx* pFaceInfo);

/*******************************************************************************************
功	能：	根据输入人脸特征与人脸模板集合，查找匹配人脸特征的序号(同一人可含有多张图片)
参	数：	pAlgEngine	  - 输入，引擎
pFaceFeaList  - 输入，人脸模板集合(模板1+模板2+...+模板N)
iPicNum       - 输入，库中图片总数
pFaceNumList  - 输入，顺序记录模板库中每个人的模板图片数(建议<=10)
iPersonNum    - 输入，库中总人数(ps:模板库中一个人可能有多张模板图片，所以总人数小于或等于图片总数)
score         - 输入，比对分通过阈值（建议76）
pFaceFea      - 输入，人脸特征
pFaceInfo     - 输入/输出，通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，-1-识别中，-2-陌生人，-3人脸模板个数过多，其他失败
*******************************************************************************************/
int WINAPI zzMaskSearchNFeatureThread(void *pAlgEngine, unsigned char *pFaceFeaList, int iPictureNum, int *pFaceNumList, int iPersonNum, int score, unsigned char *pFaceFea, MXFaceInfoEx* pFaceInfo);

/*******************************************************************************************
功	能：	根据输入人脸信息与人脸模板集合，查找匹配人脸特征的序号(同一人可含有多张图片)
参	数：	pAlgEngine	  - 输入，引擎
pFaceFeaList  - 输入，人脸模板集合(模板1+模板2+...+模板N)
iPicNum       - 输入，库中图片总数
pFaceNumList  - 输入，顺序记录模板库中每个人的模板图片数(建议<=10)
iPersonNum    - 输入，库中总人数(ps:模板库中一个人可能有多张模板图片，所以总人数小于或等于图片总数)
iMatchThreshold  - 输入，比对分通过阈值（建议73）
iMatchThreshold  - 输入，图片质量评价阈值（建议50）
pImage		  - 输入，RGB图像数据
nImgWidth     - 输入，图像宽度
nImgHeight    - 输入，图像高度
iFaceNum      - 输入，人脸数，须申请为全局变量
pFaceInfo     - 输入/输出，人脸检测结果，须申请为全局变量
通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，其他失败
备	注：   配合zzTrackFaceThread一起使用
*******************************************************************************************/
int WINAPI zzMaskSearchFaceThread(void *pAlgEngine,
								  unsigned char *pFaceFeaList, int iPictureNum, int *pFaceNumList, int iPersonNum,
								  int iMatchThreshold, int iQualityThreshold,
								  unsigned char *pImage, int nImgWidth, int nImgHeight, int iFaceNum, MXFaceInfoEx *pFaceInfo);

/*******************************************************************************************
功	能：	根据输入人脸信息与人脸模板集合，查找匹配人脸特征的序号(同一人可含有多张图片)
参	数：	pAlgEngine			- 输入，引擎
			pFaceFeaList		- 输入，人脸模板集合(模板1+模板2+...+模板N)
			pMaskFaceFeaList	- 输入，戴口罩人脸模板集合(模板1+模板2+...+模板N)
			iPicNum				- 输入，库中图片总数
			pFaceNumList		- 输入，顺序记录模板库中每个人的模板图片数(建议<=10)
			iPersonNum			- 输入，库中总人数(ps:模板库中一个人可能有多张模板图片，所以总人数小于或等于图片总数)
			iMatchThreshold		- 输入，比对分通过阈值（建议76）
			iMaskMatchThreshold - 输入，戴口罩人脸的比对分通过阈值（建议73）
			iQualityThreshold	- 输入，图片质量评价阈值（建议50）
			pImage				- 输入，RGB图像数据
			nImgWidth			- 输入，图像宽度
			nImgHeight			- 输入，图像高度
			iFaceNum			- 输入，人脸数，须申请为全局变量
			pFaceInfo			- 输入/输出，人脸检测结果，须申请为全局变量
										通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，其他失败
备	注：   配合zzTrackFaceThread一起使用
*******************************************************************************************/
int WINAPI zzSearchAllFaceThread(void *pAlgEngine,
								 unsigned char *pFaceFeaList, unsigned char *pMaskFaceFeaList,
								 int iPictureNum, int *pFaceNumList, int iPersonNum,
								 int iMatchThreshold, int iMaskMatchThreshold, int iQualityThreshold,
								 unsigned char *pImage, int nImgWidth, int nImgHeight, int iFaceNum, MXFaceInfoEx *pFaceInfo);

/*******************************************************************************************
功	能：	单帧最大人脸与人脸模板集合，查找匹配单帧最大人脸的序号
参	数：	pAlgEngine			- 输入，引擎
			pFaceFeaList		- 输入，人脸模板集合(模板1+模板2+...+模板N)
			pMaskFaceFeaList	- 输入，戴口罩人脸模板集合(模板1+模板2+...+模板N)
			iPicNum				- 输入，库中图片总数
			pFaceNumList		- 输入，顺序记录模板库中每个人的模板图片数(建议<=10)
			iPersonNum			- 输入，库中总人数(ps:模板库中一个人可能有多张模板图片，所以总人数小于或等于图片总数)
			iMatchThreshold		- 输入，比对分通过阈值（建议76）
			iMaskMatchThreshold - 输入，戴口罩人脸的比对分通过阈值（建议72）
			iQualityThreshold	- 输入，图片质量评价阈值（建议50）
			pImage				- 输入，RGB图像数据
			nImgWidth			- 输入，图像宽度
			nImgHeight			- 输入，图像高度
			iFaceNum			- 输入，人脸数
			pFaceInfo			- 输入/输出，人脸检测结果
									通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，其他失败
备	注：   配合zzDetectFaceThread一起使用
*******************************************************************************************/
int WINAPI zzMaxFaceRecogThread(void *pAlgEngine,
	unsigned char *pFaceFeaList, unsigned char *pMaskFaceFeaList,
	int iPictureNum, int *pFaceNumList, int iPersonNum,
	int iMatchScoreThreshold, int iMaskMatchScoreThreshold, int iQualityScoreThreshold,
	unsigned char *pImage, int nImgWidth, int nImgHeight, int iFaceNum, MXFaceInfoEx *pFaceInfo);



/*******************************************************************************************
功	能：	根据输入人脸特征与人脸模板集合，查找匹配人脸特征的序号
参	数：	pFaceFeaList		- 输入，人脸模板集合(模板1+模板2+...+模板N)
iFaceNum			- 输入，人脸模板个数（建议小于5000）
iScoreThreshold		- 输入，比对分通过阈值（建议76）
pFaceFea			- 输入，人脸特征
pFaceInfo			- 输入/输出，通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，其他-失败
*******************************************************************************************/
int WINAPI zzDetectSearchFeatureThread(void *pAlgEngine, unsigned char *pFaceFeaList, int iFaceNum, int iScoreThreshold, unsigned char *pFaceFea, MXFaceInfoEx* pFaceInfo);

/*******************************************************************************************
功	能：	根据输入人脸信息与人脸模板集合，查找匹配人脸特征的序号
参	数：	pFaceFeaList  - 输入，人脸模板集合(模板1+模板2+...+模板N)
			iMatchThreshold  - 输入，比对分通过阈值（建议76）
			iMatchThreshold  - 输入，图片质量评价阈值（建议50）
			pImage		  - 输入，RGB图像数据
			nImgWidth     - 输入，图像宽度
			nImgHeight    - 输入，图像高度
			iFaceNum      - 输入，人脸数，须申请为全局变量
			pFaceInfo     - 输入/输出，人脸检测结果，须申请为全局变量
			通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，其他失败
备	注：	配合zzDetectFaceThread一起使用
*******************************************************************************************/
int WINAPI zzDetectSearchFaceThread(void *pAlgEngine, unsigned char *pFaceFeaList, int iPersonNum, int iMatchThreshold, int iQualityThreshold,
	unsigned char *pImage, int nImgWidth, int nImgHeight, int iFaceNum, MXFaceInfoEx *pFaceInfo);

/*******************************************************************************************
功	能：	根据输入人脸信息与人脸模板集合，查找匹配人脸特征的序号
参	数：
			pFaceFeaList		- 输入，人脸模板集合(模板1+模板2+...+模板N)
			pMaskFaceFeaList	- 输入，戴口罩人脸模板集合(模板1+模板2+...+模板N)
			iPersonNum			- 输入，库中总人数(ps:模板库中每人一张图片)
			iMatchThreshold		- 输入，比对分通过阈值（建议76）
			iMaskMatchThreshold - 输入，戴口罩人脸的比对分通过阈值（建议73）
			iQualityThreshold	- 输入，图片质量评价阈值（建议50）
			pImage				- 输入，RGB图像数据
			nImgWidth			- 输入，图像宽度
			nImgHeight			- 输入，图像高度
			iFaceNum			- 输入，人脸数
			pFaceInfo			- 输入/输出，通过MXFaceInfoEx结构体的reCog/reCogId/reCogScore属性获取
返	回：	0-成功，其他失败
备	注：   配合zzDetectFaceThread一起使用
*******************************************************************************************/
int WINAPI zzDetectSearchAllFaceThread(void *pAlgEngine, unsigned char *pFaceFeaList, unsigned char *pMaskFaceFeaList, int iPersonNum,
	int iMatchThreshold, int iMaskMatchThreshold, int iQualityThreshold,
	unsigned char *pImage, int nImgWidth, int nImgHeight, int iFaceNum, MXFaceInfoEx *pFaceInfo);


#if defined(__linux__) 
#ifdef __cplusplus
}
#endif
#endif