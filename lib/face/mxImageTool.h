
#pragma once

#if defined(_WIN32) || defined(_WIN64)
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#else
#define WINAPI 
#endif

#if defined(__linux__) 
#ifdef __cplusplus
extern "C" {
#endif
#endif
	void WINAPI CVTimeStart();
	/*
	功能：获取运行时间，单位：秒
	*/
	double WINAPI CVTimeRun();

	/****************************************************************************
		功  能：获取算法信息
		参  数：
		返	回：算法信息
	*****************************************************************************/
	void WINAPI CVToolVersion(char *szVersion);

	/*******************************************************************************************
	功	能：	图像文件加载到内存
	参	数：	szLoadFilePath		- 输入	图像路径
				iChannels			- 输入  图像通道数，1-加载为灰度图像，3-加载为RGB图像
				pImgBuf				- 输出	外部图像缓冲区,用于接收图像数据，如果为NULL，则不接收
				oImgWidth			- 输出	图像宽度
				oImgHeight			- 输出	图像高度
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageLoad(const char* szLoadFilePath, int iChannels,
		unsigned char *pImgBuf, int *oImgWidth, int *oImgHeight);

	/*******************************************************************************************
	功	能：	保存图像数据
	参	数：	szSaveFilePath		- 输入	保存图像路径
				pImgBuf				- 输入	图像缓冲区
				iImgWidth			- 输入	图像宽度
				iImgHeight			- 输入	图像高度
				iChannels           - 输入  图像通道
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageSave(const char* szSaveFilePath, 
		unsigned char *pImgBuf, int iImgWidth, int iImgHeight, int iChannels);
	
	/*******************************************************************************************
	功	能：	YUV数据转换为RGB数据(Android摄像头获取的数据为YUV格式)
	参	数：	pYUVImage	- 输入	YUV图像数据
				iImgWidth	- 输入	图像宽度
				iImgHeight	- 输入	图像高度
				pRGBImage	- 输出	RGB图像数据
	返	回：	1-成功，其他-失败
	备	注：	https://www.cnblogs.com/snailgardening/p/opencv_yuv_rgb.html
	*******************************************************************************************/
	void ImageYUV2RGB(unsigned char *pYUVImage, int iImgWidth, int iImgHeight,
		unsigned char *pRGBImage);

	/*******************************************************************************************
	功	能：	RGB图像数据转换为灰度图像数据
	参	数：	pRGBImage	- 输入	RGB图像数据
				iImgWidth	- 输入	图像宽度
				iImgHeight	- 输入	图像高度
				pGrayImage	- 输出	灰度图像数据
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	void ImageRGB2GRAY(unsigned char *pRGBImage, int iImgWidth, int iImgHeight,
					unsigned char *pGrayImage);

	/*******************************************************************************************
	功	能：	对输入图像根据输入的目标宽度进行按比例缩放。
	参	数：	pImgBuf  		- 输入	图像缓冲区
				iImgWidth		- 输入	图像宽度
				iImgHeight		- 输入	图像高度
				iChannels       - 输入  图像通道
				iDstImgWidth    - 输入  目标图像宽度
				iDstImgHeight   - 输入	目标图像高度
				pDstImgBuf  	- 输出	目标图像缓冲区
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageZoom(unsigned char *pImgBuf, int iImgWidth, int iImgHeight, int iChannels,
		int iDstImgWidth, int iDstImgHeight, unsigned char *pDstImgBuf);

	/*******************************************************************************************
	功	能：	在输入的RGB图像上根据输入的Rect绘制矩形框	
	参	数：	pRgbImgBuf  		- 输入	RGB图像缓冲区
				iImgWidth			- 输入	图像宽度
				iImgHeight			- 输入	图像高度
				iRect				- 输入	Rect[0]	=x;
											Rect[1]	=y;
											Rect[2]	=width;
											Rect[3]	=height;
				iRectColor          - 输入  框颜色，0:R,1:G,2:B
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageDrawRect(unsigned char *pRgbImgBuf, int iImgWidth, int iImgHeight, int iRect[4], int iRectColor/*=0*/);

	/*******************************************************************************************
		功	能：	在输入的RGB图像上根据输入的Rect,进行裁剪
		参	数：	pRgbImgBuf  		- 输入	RGB图像缓冲区
					iImgWidth			- 输入	图像宽度
					iImgHeight			- 输入	图像高度
					iRect				- 输入	Rect[0]	=x;
												Rect[1]	=y;
												Rect[2]	=width;
												Rect[3]	=height;
					pDstImgBuf  		- 输出	RGB图像缓冲区
					iDstWidth			- 输出	图像宽度
					iDstHeight			- 输出	图像高度
		返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageCutRect(unsigned char *pRgbImgBuf, int iImgWidth, int iImgHeight, int iRect[4],
		unsigned char *pDstImgBuf, int* iDstWidth, int* iDstHeight);

	/*******************************************************************************************
	功	能：	在输入的RGB图像上根据输入的点坐标绘制点
	参	数：	pRgbImgBuf  		- 输入	RGB图像缓冲区
				iImgWidth			- 输入	图像宽度
				iImgHeight			- 输入	图像高度
				iPointPos			- 输入	点坐标序列（x1,y1,x2,y2,...）
				iPointNum			- 输入  点个数
				iPointColor         - 输入  点颜色，0:R,1:G,2:B
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageDrawPoint(unsigned char *pRgbImgBuf, int iImgWidth, int iImgHeight,
		int *iPointPos, int iPointNum, int iPointColor/*=0*/);

	/*******************************************************************************************
	功	能：	在输入的RGB图像上在指定位置绘制文字
	参	数：	pRgbImgBuf  		- 输入	RGB图像缓冲区
				iImgWidth			- 输入	图像宽度
				iImgHeight			- 输入	图像高度
				iPointX				- 输入	指定位置的X坐标
				iPointY				- 输入  指定位置的Y坐标
				szText				- 输入  显示文字
				ifontSize			- 输入  字体大小
				ifontColor          - 输入  字体颜色，0:R,1:G,2:B
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageDrawText(unsigned char *pRgbImgBuf, int iImgWidth, int iImgHeight,
		int iPointX, int iPointY, char* szText, int ifontSize/*=25*/, int ifontColor/*=0*/);

	/*******************************************************************************************
	功	能：	图像文件数据转换为RGB24内存数据
	参	数：	pFileDataBuf	- 输入	图像文件数据
				iFileDataLen	- 输入  图像文件数据长度
				pRGB24Buf		- 输出	RGB24内存数据
				iWidth			- 输出	图像宽度
				iHeight			- 输出	图像高度
	返	回：	>=0成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageDecode(unsigned char* pFileDataBuf, int iFileDataLen,
		unsigned char* pRGB24Buf, int* iWidth, int* iHeight);

	/*******************************************************************************************
	功	能：	RGB内存数据转换为图像文件数据
	参	数：	pRGBBuf		     - 输入	RGB内存数据
				iWidth			 - 输入	图像宽度
				iHeight			 - 输入	图像高度
				szTpye           - 输入 文件后缀,比如.jpg;.bmp
				pFileDataBuf	 - 输出	图像文件数据
				iFileDataLen	 - 输出  图像文件数据长度
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageEncode(unsigned char* pRGBBuf, int iWidth, int iHeight,
		char* szTpye, unsigned char* pFileDataBuf, int* iFileDataLen);

	/*******************************************************************************************
	功	能：	图像内存数据转换为图像文件数据
	参	数：	pImgBuf		     - 输入	内存数据
				iWidth			 - 输入	图像宽度
				iHeight			 - 输入	图像高度
				iChannel         - 输入 图像通道数
				szTpye           - 输入 文件后缀,比如.jpg;.bmp;.webp
				iCompressQuality - 输入 压缩质量，从0到100（越大越好），100为没有压缩。
										该参数只对.jpg和.webp有效
				pFileDataBuf	 - 输出	图像文件数据
				iFileDataLen	 - 输出  图像文件数据长度
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageEncode2(unsigned char* pImgBuf, int iWidth, int iHeight, int iChannel,
		char* szTpye, int iCompressQuality, unsigned char* pFileDataBuf, int* iFileDataLen);

	/*******************************************************************************************
	功	能：	输入的RGB图像，进行顺时针90/180/270角度旋转
	参	数：	pRgbImgBuf  		- 输入	RGB图像缓冲区
				iImgWidth			- 输入	图像宽度
				iImgHeight			- 输入	图像高度
				iAngle              - 输入，角度90/180/270
				pDstImgBuf  		- 输出	RGB图像缓冲区
				iDstWidth			- 输出	图像宽度
				iDstHeight			- 输出	图像高度
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageRotate(unsigned char *pRgbImgBuf, int iImgWidth, int iImgHeight, int iAngle,
		unsigned char *pDstImgBuf, int* iDstWidth, int* iDstHeight);

	/*******************************************************************************************
	功	能：	输入的RGB图像，进行镜像
	参	数：	pRgbImgBuf  		- 输入	RGB图像缓冲区
				iImgWidth			- 输入	图像宽度
				iImgHeight			- 输入	图像高度
				iIndex              - 输入， >0: 沿y-轴翻转, 0: 沿x-轴翻转, <0: x、y轴同时翻转
				pDstImgBuf  		- 输出	RGB图像缓冲区
	返	回：	1-成功，其他-失败
	*******************************************************************************************/
	int WINAPI ImageFlip(unsigned char *pRgbImgBuf, int iImgWidth, int iImgHeight, int iIndex,
		unsigned char *pDstImgBuf);

#if defined(__linux__) 
#ifdef __cplusplus
}
#endif
#endif

