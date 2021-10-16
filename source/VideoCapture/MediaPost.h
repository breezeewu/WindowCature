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
	������:     CreateVideoRecordTask
	��������:   ���ô�����Ƶ¼������
	����1��     [out] IVideoCapture** ppIVideoCapture�� ������Ƶ¼�ƿ��ƽӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	virtual HRESULT CreateVideoRecordTask(IVideoCapture** ppIVideoCapture);

/***************************************************************
	������:     CreateVideoEnhanceTask
	��������:   ���ô�����Ƶ��ǿ����
	����1��     [out] IVideoEnhance** ppIVideoEnhance�� ������Ƶ��ǿ���ƽӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	virtual HRESULT CreateVideoEnhanceTask(IVideoEnhance** ppIVideoEnhance);

/***************************************************************
	������:     GetCrashLog                               
	��������:   ��ȡ����ʱ��������ģ���������־
	����1��     [out]BYTE** ppbyte, �����Log bufferָ��
	����2��     [out]int& len,	   �����Logbuffer��С
	����ֵ��    ��
	��ע��		�ڴ�����Ƶ����ģ���ṩ�����ò����������ڴ�
*****************************************************************/
	virtual void GetCrashLog(BYTE** ppbyte, int& len);

protected:
	class CVideoEnhance*	m_pVideoEnhance;
	class CVideoRecord*		m_pVideoRecord;

	BYTE*					m_pLogbuffer;

};
