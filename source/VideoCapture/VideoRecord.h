#pragma once
#include "..\include\IVideoCapture.h"
#include "Utility.h"
#include "Struct.h"
#include "ITaskManager.h"

//enum e_RecordState
//{
//	e_CreateDS,
//	e_Running,
//	e_Stop
//};
class CVideoRecord: public IVideoCapture
#ifdef COM_INTERFACE
							, public CUnknown
#endif
{
public:
	CVideoRecord(void);
	~CVideoRecord(void);
#ifdef COM_INTERFACE
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
#endif
	/***************************************************************
	������:     SetVideoHwnd
	��������:   ������Ƶ��Ⱦ���ھ��
	����1��     [in] HWND hwd�� ��Ƶ���Ŵ��ھ��
	����2��     [in] RECT* prect,��ȡ���򣬿���Ϊ�գ�Ĭ��Ϊ��������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
	*****************************************************************/

	HRESULT SetVideoHwnd(HWND hwd, RECT* prect = NULL);

/***************************************************************
	������:     GetVideoHwnd
	��������:   ��ȡ��Ƶ��Ⱦ���ھ��
	����1��     [out] HWND& hwd�� ��Ƶ���Ŵ��ھ��
	����2��     [out] RECT* prect,��ȡ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	HRESULT GetVideoHwnd(HWND& hwd, RECT* prect);



/***************************************************************
	������:     SetVideoSolutionRatio  
	��������:   ����¼����Ƶ�ֱ���
	����1��     int nWidth�� Ŀ����Ƶ��
	����2��     int nHeight��Ŀ����Ƶ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	HRESULT SetDstVideoSolutionRatio(int nWidth, int nHeigh);

/***************************************************************
	������:     GetVideoSolutionRatio  
	��������:  ��ȡ¼����Ƶ�ֱ���
	����1��     [out] int& nWidth�� Ŀ����Ƶ��
	����2��     [out] int& nHeight��Ŀ����Ƶ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	HRESULT GetDstVideoSolutionRatio(int& nWidth, int& nHeigh);

/******************************************************************
	������:     SetDstVideoFrameRate
	��������:   ����¼����Ƶ֡��
	����1��     [in] double dFramerate�� ֡��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT SetDstVideoFrameRate(double dFramerate);

/******************************************************************
	������:     GetDstVideoFrameRate
	��������:   ��ȡ¼����Ƶ֡��
	����1��     [out] double& dFramerate�� ֡��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT GetDstVideoFrameRate(double& dFramerate);



/******************************************************************
	������:     SetAudioSampleRate
	��������:   ����¼����Ƶ������
	����1��     [in] e_SampleRate samplerate�� Ŀ����Ƶ�����ʣ�Ĭ��Ϊ44.1kHZ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT SetAudioSampleRate(e_SampleRate samplerate);

/******************************************************************
	������:     GetAudioSampleRate
	��������:   ��ȡ¼����Ƶ������
	����1��     [out] e_SampleRate& samplerate�� Ŀ����Ƶ�����ʣ�Ĭ��Ϊ44.1kHZ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT GetAudioSampleRate(e_SampleRate& samplerate);

/******************************************************************
	������:     SetAudioChannel
	��������:   ����¼����Ƶ������
	����1��     [in] int channel�� Ŀ����Ƶ��������Ĭ��Ϊ2������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT SetAudioChannel(int channel);

/******************************************************************
	������:     GetAudioChannel
	��������:   ��ȡ¼����Ƶ������
	����1��     [out] int channel�� Ŀ����Ƶ��������Ĭ��Ϊ2������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT GetAudioChannel(int& channel);

/******************************************************************
	������:     SetAudioVolume
	��������:   ���������Ŵ�
	����1��     [in] long AudioVolume��//�����Ŵ�С��100��ʾ������С������100��ʾ�����Ŵ󣬰�����/100�ı�������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT SetAudioVolume(long AudioVolume);

/******************************************************************
	������:     GetAudioVolume
	��������:   ��ȡ�����Ŵ�
	����1��     [out] long& AudioVolume��//�����Ŵ�С��100��ʾ������С������100��ʾ�����Ŵ󣬰�����/100�ı�������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT GetAudioVolume(long& AudioVolume);


/******************************************************************
	������:     SetFileContainer                                
	��������:   ����¼����Ƶ������ʽ
	����1��     [in] e_RECORD_CONTAINER container, Ŀ���ļ���������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT SetDstFileContainer(e_RECORD_CONTAINER container);

/******************************************************************
	������:     GetDstFileContainer                                
	��������:   ��ȡ¼����Ƶ������ʽ
	����1��     [out] e_RECORD_CONTAINER& container, Ŀ���ļ���������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT GetDstFileContainer(e_RECORD_CONTAINER& container);

/******************************************************************
	������:     SetDstFilePath                                
	��������:   ����¼����Ƶ����·��������
	����1��     [in] wchar_t* pPath, Ŀ���ļ�����·��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT SetDstFilePath(wchar_t* pPath);

/******************************************************************
	������:     GetDstFilePath                                
	��������:   ��ȡ¼����Ƶ����·��������
	����1��     [in] wchar_t* pPath, Ŀ���ļ�����·��
	����2��     [in,out] int& len, pPath��buffer���ȣ���С�ڵ�ǰ·��buffer�ĳ��ȣ��򷵻ص�ǰ·�����ȶ������ص�ǰ·��(pPath)
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
	��ע��      pPath �ڴ泤�Ȳ�������S_FALSE�������ص�ǰ·���ڴ��С
*******************************************************************/

	HRESULT GetDstFilePath(wchar_t* pPath, int& len);


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

	HRESULT SetCallBack(struct IQvodCallBack* pcallback);

/******************************************************************
	������:     GetCallBack
	��������:   ������Ϣ�ص��ӿ�
	����1:      [out]interface IQvodCallBack** ppcallback���ص���Ϣ�ӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT GetCallBack(struct IQvodCallBack** ppcallback);

/******************************************************************
	������:     VideoResizeNotify
	��������:   ��Ƶ���ڴ�С����֪ͨ
	����1:      [in]RECT* prect����Ƶ���ڴ�С����֪ͨ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT VideoResizeNotify(RECT* prect);

/******************************************************************
	������:     SetTailLogo
	��������:   ������Ƶβ��logo
	����1:      [in,out]BITMAPINFOHEADER* pbih��24λ��32λ��bmpλͼͷ�ṹ��
	����2:		[in,out]BYTE* pBmpData,λͼ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT SetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData);

/******************************************************************
	������:     SetTailLogo
	��������:   ��ȡ��Ƶβ��logo
	����1:      [in,out]BITMAPINFOHEADER* pbih��24λ��32λ��bmpλͼͷ�ṹ��
	����2:		[in,out]BYTE* pBmpData,λͼ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	HRESULT GetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData);

/******************************************************************
	������:     DestroyInstance
	��������:   ����ʵ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/
	void DestroyInstance();

protected:
	//����ת��
	e_CONTAINER CointerConvert(e_RECORD_CONTAINER rc);

	e_RECORD_CONTAINER CointerConvert(e_CONTAINER container);

protected:

	HWND					m_hwnd;//��Ƶ¼�ƴ���
	int						m_nWidth;//Ŀ����Ƶ��
	int						m_nHeigh;//Ŀ����Ƶ��
	int						m_nChannel;//������
	int						m_nSampleRate;//��Ƶ������
	double					m_dFrameRate;//֡��
	e_RECORD_CONTAINER		m_eContainer;//��������
	e_State					m_RecordState;//¼��״̬
	Record_Param			m_RecordParam;//¼�Ʋ���
	ITaskManager*			m_pITaskManager;//�������ӿ�
	CCritSec				m_CritSecLock;//�ٽ���
};
