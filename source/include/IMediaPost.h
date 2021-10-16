#pragma once
#include "IVideoEnhance.h"
#include "IVideoCapture.h"


//[uuid("E783FE4B-7459-4722-940F-4CCB60DD5972")]
//DECLARE_INTERFACE_(IMediaPost, IUnknown)
class IMediaPost
{
public:

/***************************************************************
	������:     CreateVideoRecordTask
	��������:   ���ô�����Ƶ¼������
	����1��     [out] IVideoCapture** ppIVideoCapture�� ������Ƶ¼�ƿ��ƽӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	virtual HRESULT CreateVideoRecordTask(IVideoCapture** ppIVideoCapture) = 0;

/***************************************************************
	������:     CreateVideoEnhanceTask
	��������:   ���ô�����Ƶ��ǿ����
	����1��     [out] IVideoEnhance** ppIVideoEnhance�� ������Ƶ��ǿ���ƽӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	virtual HRESULT CreateVideoEnhanceTask(IVideoEnhance** ppIVideoEnhance) = 0;

/***************************************************************
	������:     GetCrashLog                               
	��������:   ��ȡ����ʱ��������ģ���������־
	����1��     [out]BYTE** ppbyte, �����Log bufferָ��
	����2��     [out]int& len,	   �����Logbuffer��С
	����ֵ��    ��
	��ע��		�ڴ�����Ƶ����ģ���ṩ�����ò����������ڴ�
*****************************************************************/
	virtual void GetCrashLog(BYTE** ppbyte, int& len) = 0;
};
