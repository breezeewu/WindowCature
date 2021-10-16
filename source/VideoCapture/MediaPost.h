#pragma once
#include "..\include\IMediaPost.h"

EXTERN_C __declspec(dllexport) HRESULT InitMediaPost(void** ppMediaPost);

EXTERN_C __declspec(dllexport) HRESULT DestoryMediaPost(void** ppMediaPost);

class CMediaPost:public IMediaPost
{
public:
	CMediaPost(void);
public:
	~CMediaPost(void);

/***************************************************************
	函数名:     CreateVideoRecordTask
	函数描述:   设置创建视频录制任务
	参数1：     [out] IVideoCapture** ppIVideoCapture， 创建视频录制控制接口
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT CreateVideoRecordTask(IVideoCapture** ppIVideoCapture);

/***************************************************************
	函数名:     CreateVideoEnhanceTask
	函数描述:   设置创建视频增强任务
	参数1：     [out] IVideoEnhance** ppIVideoEnhance， 创建视频增强控制接口
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT CreateVideoEnhanceTask(IVideoEnhance** ppIVideoEnhance);

/***************************************************************
	函数名:     GetCrashLog                               
	函数描述:   获取崩溃时播放引擎模块的运行日志
	参数1：     [out]BYTE** ppbyte, 输出，Log buffer指针
	参数2：     [out]int& len,	   输出，Logbuffer大小
	返回值：    无
	备注：		内存如视频处理模块提供，调用层无需申请内存
*****************************************************************/
	virtual void GetCrashLog(BYTE** ppbyte, int& len);

protected:
	class CVideoEnhance*	m_pVideoEnhance;
	class CVideoRecord*		m_pVideoRecord;

	BYTE*					m_pLogbuffer;

};
