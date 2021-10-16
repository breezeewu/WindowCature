#pragma once

//��Ƶ¼�Ʋ����ṹ��
struct Record_Param
{
	HWND			hwnd;
	RECT			WinRect;
	int				nWidth;
	int				nHeigh;
	double			dframerate;
	long			AudioVolume;//�����Ŵ�С��100��ʾ������С������100��ʾ�����Ŵ󣬰�����/100�ı�������
	int				nChannel;
	CString			RecordPath;
	e_SampleRate	nSampleRate;	
	e_CONTAINER		eContainer;

	Record_Param()
	{
		init();
	}

	void init()
	{
		hwnd			= NULL;
		WinRect.top		= 0;
		WinRect.left	= 0;
		WinRect.right	= 0;
		WinRect.bottom	= 0;
		nWidth			= 0;
		nHeigh			= 0;
		dframerate		= 20;
		AudioVolume		= 100;
		nChannel		= 2;
		nSampleRate		= _E_SR_44K;	
		eContainer		= _E_CON_MP4;
	}
};

class ITaskManager
{
public:
/******************************************************************
	������:     CreateRecordTask
	��������:   ������Ƶ¼������
	����1:		Record_Param* param, ��Ƶ¼�Ʋ������ã���Ϊ�գ����Ϊ����ʹ��Ĭ��ֵ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/
	virtual HRESULT CreateRecordTask(Record_Param* param) = 0;

/******************************************************************
	������:     StartRecord
	��������:   ��ʼ¼����Ƶ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT StartRecord() = 0;


/******************************************************************
	������:     PauseRecord
	��������:   ��ͣ¼����Ƶ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT PauseRecord() = 0;

/******************************************************************
	������:     StopRecord
	��������:    ֹͣ¼����Ƶ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT StopRecord() = 0;

/******************************************************************
	������:     GetFirstBitmapImage
	��������:   ��ȡ��һ֡��Ƶͼ��
	����1:      PBYTE *ppbuf�������BITMAPINFOHEADER+λͼ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
	��ע��		�ú����ڿ�ʼ¼��֮����ܵ���
*******************************************************************/

	virtual HRESULT GetFirstBitmapImage(BYTE** pData) = 0;

/******************************************************************
	������:     SetCallBack
	��������:   ������Ϣ�ص��ӿ�
	����1:      [in]interface IQvodCallBack* pcallback���ص���Ϣ�ӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT SetCallBack(interface IQvodCallBack* pcallback) = 0;

/******************************************************************
	������:     GetCallBack
	��������:   ������Ϣ�ص��ӿ�
	����1:      [out]interface IQvodCallBack** ppcallback���ص���Ϣ�ӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT GetCallBack(interface IQvodCallBack** ppcallback) = 0;

/******************************************************************
	������:     VideoResizeNotify
	��������:   ��Ƶ���ڴ�С����֪ͨ
	����1:      [in]RECT* prect����Ƶ���ڴ�С����֪ͨ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT VideoResizeNotify(RECT* prect) = 0;

/******************************************************************
	������:     SetTailLogo
	��������:   ������Ƶβ��logo
	����1:      [in,out]BITMAPINFOHEADER* pbih��24λ��32λ��bmpλͼͷ�ṹ��
	����2:		[in,out]BYTE* pBmpData,λͼ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	//virtual HRESULT SetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData) = 0;

/******************************************************************
	������:     SetTailLogo
	��������:   ��ȡ��Ƶβ��logo
	����1:      [in,out]BITMAPINFOHEADER* pbih��24λ��32λ��bmpλͼͷ�ṹ��
	����2:		[in,out]BYTE* pBmpData,λͼ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	//virtual HRESULT GetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData) = 0;
};