#include "StdAfx.h"
#include "MediaPost.h"
#include "VideoRecord.h"
#include "VideoEnhance.h"
#include "Log.h"

CMediaPost::CMediaPost(void)
{
	m_pVideoEnhance = NULL;
	m_pVideoRecord  = NULL;

	//��bufferָ����������²���Ҫʹ�ã�ֻ���ڳ����쳣����ʱ��־����
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
	������:     CreateVideoRecordTask
	��������:   ���ô�����Ƶ¼������
	����1��     [out] IVideoCapture** ppIVideoCapture�� ������Ƶ¼�ƿ��ƽӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
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
	//	//����ʵ��
	//	m_pVideoRecord = new CVideoRecord();
	//}
	//MessageBox(0,0,0,0);
	CVideoRecord* pVideoRecord = new CVideoRecord();
	//��ֵ
	*ppIVideoCapture = static_cast<IVideoCapture*>(pVideoRecord);
	return *ppIVideoCapture ? S_OK: E_OUTOFMEMORY;
#endif
}

/***************************************************************
	������:     CreateVideoEnhanceTask
	��������:   ���ô�����Ƶ��ǿ����
	����1��     [out] IVideoEnhance** ppIVideoEnhance�� ������Ƶ��ǿ���ƽӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
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