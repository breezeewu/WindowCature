#pragma once
#include "windows.h"
#ifdef QDBASE_EXPORTS /*happyhuang x*/
#define QDSTRINGAPI __declspec(dllexport)
#else
#define QDSTRINGAPI __declspec(dllimport)
#endif

//��Ƶ¼�ƽӿ�
extern "C"
{
/***************************************************************
	������:     CreateVideoRecordTask_c
	��������:   ���ô�����Ƶ¼������
	����1��     [in] long* pid
	����2��     [out] IVideoCapture** ppIVideoCapture�� ������Ƶ¼�ƿ��ƽӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
 QDSTRINGAPI HRESULT CreateVideoRecordTask_c(long* pid);
/***************************************************************
	������:     SetVideoHwnd_c
	��������:   ������Ƶ��Ⱦ���ھ��
	����1��     [in] long pid,taskid
	����2��     [in] HWND hwd�� ��Ƶ���Ŵ��ھ��
	����3��     [in] RECT* prect,��ȡ���򣬿���Ϊ�գ�Ĭ��Ϊ��������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
QDSTRINGAPI HRESULT SetVideoHwnd_c(long pid, HWND hwd, RECT* prect);

/***************************************************************
	������:     GetVideoHwnd_c
	��������:   ��ȡ��Ƶ��Ⱦ���ھ��
	����1��     [in] long pid,taskid
	����2��     [out] HWND& hwd�� ��Ƶ���Ŵ��ھ��
	����3��     [out] RECT* prect,��ȡ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
QDSTRINGAPI HRESULT GetVideoHwnd_c(long pid, HWND& hwd, RECT* prect);

/***************************************************************
	������:     SetVideoSolutionRatio_c  
	��������:   ����¼����Ƶ�ֱ���
	����1��     [in] long pid,taskid
	����2��     [in] int nWidth�� Ŀ����Ƶ��
	����3��     [in] int nHeight��Ŀ����Ƶ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

QDSTRINGAPI HRESULT SetDstVideoSolutionRatio_c(long pid, int nWidth, int nHeigh);

/***************************************************************
	������:     GetVideoSolutionRatio_c  
	��������:  ��ȡ¼����Ƶ�ֱ���
	����1��     [in] long pid,taskid
	����2��     [out] int& nWidth�� Ŀ����Ƶ��
	����3��     [out] int& nHeight��Ŀ����Ƶ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

QDSTRINGAPI HRESULT GetDstVideoSolutionRatio_c(long pid, int& nWidth, int& nHeigh);

/******************************************************************
	������:     SetDstVideoFrameRate_c
	��������:   ����¼����Ƶ֡��
	����1��     [in] long pid,taskid
	����2��     [in] double dFramerate�� ֡��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT SetDstVideoFrameRate_c(long pid, double dFramerate);

/******************************************************************
	������:     GetDstVideoFrameRate_c
	��������:   ��ȡ¼����Ƶ֡��
	����1��     [in] long pid,taskid
	����2��     [out] double& dFramerate�� ֡��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT GetDstVideoFrameRate_c(long pid, double& dFramerate);



/******************************************************************
	������:     SetAudioSampleRate_c
	��������:   ����¼����Ƶ������
	����1��     [in] long pid,taskid
	����2��     [in] e_SampleRate samplerate�� Ŀ����Ƶ�����ʣ�Ĭ��Ϊ44.1kHZ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT SetAudioSampleRate_c(long pid, e_SampleRate samplerate);

/******************************************************************
	������:     GetAudioSampleRate_c
	��������:   ��ȡ¼����Ƶ������
	����1��     [in] long pid,taskid
	����2��     [out] e_SampleRate& samplerate�� Ŀ����Ƶ�����ʣ�Ĭ��Ϊ44.1kHZ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT GetAudioSampleRate_c(long pid, (e_SampleRate& samplerate);

/******************************************************************
	������:     SetAudioChannel_c
	��������:   ����¼����Ƶ������
	����1��     [in]long pid,taskid
	����2��     [in] int channel�� Ŀ����Ƶ��������Ĭ��Ϊ2������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT SetAudioChannel_c(long pid, int channel);

/******************************************************************
	������:     GetAudioChannel_c
	��������:   ��ȡ¼����Ƶ������
	����1��     [in] long pid,taskid
	����2��     [out] int channel�� Ŀ����Ƶ��������Ĭ��Ϊ2������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT GetAudioChannel_c(long pid, int& channel);

/******************************************************************
	������:     SetAudioVolume_c
	��������:   ���������Ŵ�
	����1��     [in] long pid,taskid
	����2��     [in] long AudioVolume��//�����Ŵ�С��100��ʾ������С������100��ʾ�����Ŵ󣬰�����/100�ı�������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT SetAudioVolume_c(long pid, long AudioVolume);

/******************************************************************
	������:     GetAudioVolume_c
	��������:   ��ȡ�����Ŵ�
	����1��     [in] long pid,taskid
	����2��     [out] long& AudioVolume��//�����Ŵ�С��100��ʾ������С������100��ʾ�����Ŵ󣬰�����/100�ı�������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT GetAudioVolume_c(long pid, long& AudioVolume);


/******************************************************************
	������:     SetFileContainer_c                                
	��������:   ����¼����Ƶ������ʽ
	����1��     [in] long pid,taskid
	����2��     [in] e_RECORD_CONTAINER container, Ŀ���ļ���������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT SetDstFileContainer_c(long pid, e_RECORD_CONTAINER container);

/******************************************************************
	������:     GetDstFileContainer_c                                
	��������:   ��ȡ¼����Ƶ������ʽ
	����1��     [in] long pid,taskid
	����2��     [out] e_RECORD_CONTAINER& container, Ŀ���ļ���������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT GetDstFileContainer_c(long pid, e_RECORD_CONTAINER& container);

/******************************************************************
	������:     SetDstFilePath_c                                
	��������:   ����¼����Ƶ����·��������
	����1��     [in] long pid,taskid
	����2��     [in] wchar_t* pPath, Ŀ���ļ�����·��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT SetDstFilePath_c(long pid, wchar_t* pPath);

/******************************************************************
	������:     GetDstFilePath_c                                
	��������:   ��ȡ¼����Ƶ����·��������
	����1��     [in] long pid,taskid
	����2��     [in] wchar_t* pPath, Ŀ���ļ�����·��
	����3��     [in,out] int& len, pPath��buffer���ȣ���С�ڵ�ǰ·��buffer�ĳ��ȣ��򷵻ص�ǰ·�����ȶ������ص�ǰ·��(pPath)
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
	��ע��      pPath �ڴ泤�Ȳ�������S_FALSE�������ص�ǰ·���ڴ��С
*******************************************************************/

QDSTRINGAPI HRESULT GetDstFilePath_c(long pid, wchar_t* pPath, int& len);


/******************************************************************
	������:     StartRecord_c
	��������:   ��ʼ¼����Ƶ
	����1��     [in] long pid,taskid
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT StartRecord_c(long pid);

/******************************************************************
	������:     PauseRecord_c
	��������:   ��ͣ¼����Ƶ
	����1��     [in] long pid,taskid
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT PauseRecord_c(long pid);

/******************************************************************
	������:     StopRecord_c
	��������:    ֹͣ¼����Ƶ
	����1��     [in] long pid,taskid
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT StopRecord_c(long pid);

/******************************************************************
	������:     GetFirstBitmapImage_c
	��������:   ��ȡ��һ֡��Ƶͼ��
	����1��     [in] long pid,taskid
	����2:      PBYTE *ppbuf�������BITMAPINFOHEADER+λͼ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
	��ע��		�ú����ڿ�ʼ¼��֮����ܵ���
*******************************************************************/

QDSTRINGAPI HRESULT GetFirstBitmapImage_c(long pid, BYTE** ppData);

/******************************************************************
	������:     SetCallBack_c
	��������:   ������Ϣ�ص��ӿ�
	����1��     [in] long pid,taskid
	����2:      [in] interface IQvodCallBack* pcallback���ص���Ϣ�ӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT SetCallBack_c(long pid, struct IQvodCallBack* pcallback);

/******************************************************************
	������:     GetCallBack_c
	��������:   ������Ϣ�ص��ӿ�
	����1��     [in] long pid,taskid
	����2:      [out] interface IQvodCallBack** ppcallback���ص���Ϣ�ӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT GetCallBack_c(long pid, struct IQvodCallBack** ppcallback);

/******************************************************************
	������:     VideoResizeNotify_c
	��������:   ��Ƶ���ڴ�С����֪ͨ
	����1��     [in] long pid,taskid
	����2:      [in] RECT* prect����Ƶ���ڴ�С����֪ͨ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

QDSTRINGAPI HRESULT VideoResizeNotify_c(long pid, RECT* prect);

/******************************************************************
	������:     DestroyInstance_c
	��������:   ����ʵ��
	����1��     [in] long pid,taskid
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/
QDSTRINGAPI void DestroyInstance_c(long pid);
};

//��Ƶ��ǿ�ӿ�
extern "C"
{
}