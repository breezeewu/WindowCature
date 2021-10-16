#pragma once
#include "QvodGraphBuilder.h"
#include "QvodFilterManager.h"
#include "..\include\IVideoCapture.h"
#include "..\include\IScreenCapture.h"
#include "ITaskManager.h"
#include "..\include\CaptureConfig.h"
#include "..\include\HookControl.h"
#include "..\include\IFFmpegMux.h"

typedef HRESULT (*InitMediaHook)(void** ppMediaHook);


typedef HRESULT (*DestoryMediaHook)(void** ppMediaHook);

class CTaskManager :
	public CGraphBuilderEx, public ITaskManager
{
public:
	CTaskManager(void);
	~CTaskManager(void);

/******************************************************************
	������:     CreateRecordTask
	��������:   ������Ƶ¼������
	����1:		Record_Param* param, ��Ƶ¼�Ʋ������ã���Ϊ�գ����Ϊ����ʹ��Ĭ��ֵ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/
	HRESULT CreateRecordTask(Record_Param* param);

/******************************************************************
	������:     StartRecord
	��������:   ��ʼ¼����Ƶ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT StartRecord();

/******************************************************************
	������:     PauseRecord
	��������:   ��ͣ¼����Ƶ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT PauseRecord();

/******************************************************************
	������:     StopRecord
	��������:    ֹͣ¼����Ƶ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT StopRecord();

/******************************************************************
	������:     GetFirstBitmapImage
	��������:   ��ȡ��һ֡��Ƶͼ��
	����1:      PBYTE *ppbuf�������BITMAPINFOHEADER+λͼ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
	��ע��		�ú����ڿ�ʼ¼��֮����ܵ���
*******************************************************************/

	HRESULT GetFirstBitmapImage(BYTE** ppData);

/******************************************************************
	������:     SetCallBack
	��������:   ������Ϣ�ص��ӿ�
	����1:      [in]interface IQvodCallBack* pcallback���ص���Ϣ�ӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT SetCallBack(interface IQvodCallBack* pcallback);

/******************************************************************
	������:     GetCallBack
	��������:   ������Ϣ�ص��ӿ�
	����1:      [out]interface IQvodCallBack** ppcallback���ص���Ϣ�ӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT GetCallBack(interface IQvodCallBack** ppcallback);

/******************************************************************
	������:     VideoResizeNotify
	��������:   ��Ƶ���ڴ�С����֪ͨ
	����1:      [in]RECT* prect����Ƶ���ڴ�С����֪ͨ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT VideoResizeNotify(RECT* prect);

protected:

	//����filter
	void ResetFilters();

	//ffmpeg mux configure
	HRESULT MuxConfigure(interface IFFmpegMux* pIFFmpegMux);

	//������Ƶsamplerate
	int CalculateSampleRate(e_SampleRate eSampleRate);

	HRESULT LoadMediaHook(IHookControl** ppIHookControl);
	HRESULT LoadAPIHook();

protected:
	CComQIPtr<IBaseFilter>				m_pScreenSource;//��Ļץȡsource
	CComQIPtr<IBaseFilter>				m_pAudioSource;//��Ƶץȡsource
	CComQIPtr<IBaseFilter>				m_pffmpegMux;//ffmpeg mux
	CComQIPtr<IScreenCapture>			m_pIScreenCapture;
	CComQIPtr<ICaptureConfig>			m_pICaptureConfig;
	CComQIPtr<IFFmpegMux>				m_pIFFmpegMux;//ffmpegmux���ýӿ�

	IHookControl*						m_pIHookControl;
	interface IQvodCallBack*			m_pCallBack;	//�ص������ӿ�
	HMODULE								m_hHookHandle;
	Record_Param						m_RecordParam;//��Ƶ¼�Ʋ���
	BYTE*								m_pImageBuffer;//¼����Ƶ��һ֡BMPλͼ
	DWORD								m_nImageSize;	//ͼ���С
	CCritSec							m_CritSecLock;//�ٽ���

};
