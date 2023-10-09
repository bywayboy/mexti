#ifdef __cplusplus
	extern "C" {
#endif
#if defined(_WIN32) || defined(_WIN64)
	#ifndef MIAXIS_API
		#define MIAXIS_API __stdcall
	#endif
#else
	#define MIAXIS_API 
#endif

						   
/****************************************************************************
	函  数：int  mxGetVersion(char * version)
	功  能：获取算法版本号
	参  数：version  - 长度为100字节的字符串指针
	返  回：   0     - 成功
              其他   - 失败
*****************************************************************************/
int MIAXIS_API mxGetVersion(char* version);

/***************************************************************************
	函	数:	int mxFingerMatchBase64(unsigned char *fingerdata1,
	                                unsigned char *fingerdata2,
									int level)
	功	能:	对输入的两个指纹特征值进行比对
	参	数:	fingerdata1 -  输入，指纹特征值1,344字节base64码
			    fingerdata2 -  输入，指纹特征值2,344字节base64码
			     level      -  输入,匹配等级
	返回值:	   0        -  匹配成功
			   1        -  匹配失败
			   -2        -  数据非法
****************************************************************************/
int MIAXIS_API mxFingerMatchBase64(unsigned char *fingerdata1, 
							   unsigned char *fingerdata2,
							   int level);  

#ifdef __cplusplus
}
#endif

