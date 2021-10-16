#include "StdAfx.h"
#include "MediaPost.h"
#include "VideoRecord.h"
#include "VideoEnhance.h"
#include "Log.h"

CMediaPost::CMediaPost(void)
{
	m_pVideoEnhance = NULL;
	m_pVideoRecord  = NULL;

	//此buffer指针正常情况下不需要使用，只用于程序异常崩溃时日志拷贝
	m_pLogbuffer = NULL;
}

CMediaPost::~CMediaPost(void)
{
	/*if(m_pVideoEnhance)
	{
	delete m_pVideoEnhance;
	}

	if(m_pVideoRecord)
	{
	delete m_pVideoRecord;
	}*/

	if(m_pLogbuffer)
	{
		delete[] m_pLogbuffer;
	}
}


/***************************************************************
	函数名:     CreateVideoRecordTask
	函数描述:   设置创建视频录制任务
	参数1：     [out] IVideoCapture** ppIVideoCapture， 创建视频录制控制接口
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
HRESULT CMediaPost::CreateVideoRecordTask(IVideoCapture** ppIVideoCapture)
{
	CheckPointer(ppIVideoCapture, E_POINTER);

#ifdef COM_INTERFACE
	CVideoRecord* pVideoRecord = new CVideoRecord();
	if(pVideoRecord)
	{
		return pVideoRecord->QueryInterface(__uuidof(IVideoCapture), (void**)ppIVideoCapture);
	}
	return E_FAIL;
#else
	//if(m_pVideoEnhance == NULL)
	//{
	//	//创建实例
	//	m_pVideoRecord = new CVideoRecord();
	//}
	//MessageBox(0,0,0,0);
	CVideoRecord* pVideoRecord = new CVideoRecord();
	//赋值
	*ppIVideoCapture = static_cast<IVideoCapture*>(pVideoRecord);
	return *ppIVideoCapture ? S_OK: E_OUTOFMEMORY;
#endif
}

/***************************************************************
	函数名:     CreateVideoEnhanceTask
	函数描述:   设置创建视频增强任务
	参数1：     [out] IVideoEnhance** ppIVideoEnhance， 创建视频增强控制接口
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

HRESULT CMediaPost::CreateVideoEnhanceTask(IVideoEnhance** ppIVideoEnhance)
{
	CheckPointer(ppIVideoEnhance, E_POINTER);

#ifdef COM_INTERFACE
	CVideoEnhance* pVideoEnhance = new CVideoEnhance();
	if(pVideoEnhance)
	{
		return pVideoEnhance->QueryInterface(__uuidof(IVideoEnhance), (void**)ppIVideoEnhance);
	}

	return E_FAIL;
#else

	/*if(m_pVideoEnhance == NULL)
	{
		m_pVideoEnhance = new CVideoEnhance();
	}*/
	CVideoEnhance* pVideoEnhance = new CVideoEnhance();
	*ppIVideoEnhance = static_cast<IVideoEnhance*>(pVideoEnhance);
	//MessageBox(0,0,0,0);
	return *ppIVideoEnhance ? S_OK: E_OUTOFMEMORY;
#endif
}

void CMediaPost::GetCrashLog(BYTE** ppbyte, int& len)
{
	CLog::GetInstance()->GetLogInfo(NULL,len);
	if(m_pLogbuffer)
	{
		delete m_pLogbuffer;
	}
	
	m_pLogbuffer = new BYTE[len];

	CLog::GetInstance()->GetLogInfo(m_pLogbuffer, len);
}