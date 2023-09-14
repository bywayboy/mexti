//---------------------------------------------------------------------------

#ifndef __MX_FACE_INFO_EX_H__
#define __MX_FACE_INFO_EX_H__

#define MAX_KEY_POINT_NUM 120

typedef struct MXFACEINFOEX // 人脸信息
{
	//face rect人脸框
	int x;        // 左上角x坐标
	int y;        // 坐上角y坐标
	int width;    // 人脸宽
	int height;   // 人脸高

	//face_point关键点
	int keypt_num;						 // 关键点个数
	int keypt_x[MAX_KEY_POINT_NUM];      // 关键点x坐标
	int keypt_y[MAX_KEY_POINT_NUM];      // 关键点y坐标

	//人脸检测概率
	//int faceprobability;

	// 人脸属性
	int age;       	//年龄
	int gender;    	//性别
	int expression; //表情

	//人脸质量分
	int quality;     // 总分，0~100之间，越大则人脸质量越好.
	
	// 瞳距
	int eyeDistance; // 瞳距
  
	// 活体
	int liveness;  //0~100
  
	// 人脸ID
	int	detected;			// 1: 检测到的人脸,.0：跟踪到的人脸
							// 注： 跟踪到的仅ID和人脸框数据有效

	int	trackId;			// 人脸ID（ID<0表示没有进入跟踪）
	int idmax;              //获取交并比最大的人脸下标
	int reCog;              //判断该人脸是否被识别-识别标识
	int reCogId;            //数据库中的识别ID
	int reCogScore;			//数据库中识别ID的分数
	
	
	int mask;             //口罩0~100

	int stranger;          //陌生人标识位
	

	// head_pose头部姿态
	int pitch;         // 抬头、低头,范围-90到90，越大表示越抬头
	int yaw;           // 左右转头
	int roll;          // 平面内偏头
}MXFaceInfoEx;

#endif