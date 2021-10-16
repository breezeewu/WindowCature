#include "StdAfx.h"
#include "VideoRecord.h"
#include "TaskManager.h"

CVideoRecord::CVideoRecord(void)
#ifdef COM_INTERFACE
				:CUnknown(L"VideoRecord", NULL)
#endif
{
	m_hwnd				= NULL;
	m_pITaskManager		= NULL;
	m_nWidth			= 640;		//Ŀ����Ƶ��
	m_nHeigh			= 480;		//Ŀ����Ƶ��
	m_nSampleRate		= _E_SR_44K;//��Ƶ������
	m_nChannel			= 2;		//������
	m_dFrameRate		= 15;		//֡��
	m_eContainer		= E_CON_FLV;//��������
	m_RecordState		= e_Stop;	//¼��״̬

	m_pITaskManager = static_cast<ITaskManager*>(new CTaskManager());
}

CVideoRecord::~CVideoRecord(void)
{
	//����taskmanager
	CTaskManager* ptm = static_cast<CTaskManager*>(m_pITaskManager);

	if(ptm)
	{
		delete ptm;
	}

	m_pITaskManager = NULL;
}
#ifdef COM_INTERFACE
STDMETHODIMP CVideoRecord::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	//�������ָ��ĺϷ���
	CheckPointer(ppv, E_POINTER);

	if(__uuidof(IUnknown) == riid)
	{
		*ppv = static_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}
	else if(__uuidof(IVideoCapture) == riid)
	{
		*ppv = static_cast<IVideoCapture*>(this);
		AddRef();
		return S_OK;
	}

	return __super::NonDelegatingQueryInterface(riid, ppv);
}
#endif
//������Ƶ��Ⱦ���ھ��
HRESULT CVideoRecord::SetVideoHwnd(HWND hwd, RECT* prect)
{
	//������ĺϷ���
	CheckPointer(hwd, E_HANDLE);
	CLog::GetInstance()->trace(3, L"SetVideoHwnd:%0x\n", hwd);
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);
	//prect = NULL;
	m_RecordParam.hwnd = hwd;

	if(prect)
	{
		//prect->bottom -= 45;
		m_RecordParam.WinRect = *prect;
	}
	CLog::GetInstance()->trace(3, L"SetVideoHwnd:%u end\n", hwd);
	return S_OK;
}

//��ȡ��Ƶ��Ⱦ���ھ��
HRESULT CVideoRecord::GetVideoHwnd(HWND& hwd, RECT* prect)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//�����ֵ
	hwd = m_RecordParam.hwnd;

	return S_OK;
}

// ����¼����Ƶ�ֱ���
HRESULT CVideoRecord::SetDstVideoSolutionRatio(int nWidth, int nHeigh)
{
	//���ֱ���ȡֵ��Χ�ĺϷ���
	CheckRange(nWidth, 100, 1280);
	CheckRange(nHeigh, 100, 720);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	m_RecordParam.nWidth = nWidth;
	m_RecordParam.nHeigh = nHeigh;

	return S_OK;
}

HRESULT CVideoRecord::GetDstVideoSolutionRatio(int& nWidth, int& nHeigh)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	nWidth = m_RecordParam.nWidth;
	nHeigh = m_RecordParam.nHeigh;

	return S_OK;
}

// ����¼����Ƶ�ֱ���
HRESULT CVideoRecord::SetDstVideoFrameRate(double dFramerate)
{
	//����֡��ȡֵ��Χ�ĺϷ���
	CheckRange(dFramerate, 0, 25)

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//���õ���֡��
	m_RecordParam.dframerate = dFramerate;

	return S_OK;
}

// ����¼����Ƶ�ֱ���
HRESULT CVideoRecord::GetDstVideoFrameRate(double& dFramerate)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//��ȡ��ǰ֡��
	dFramerate = m_RecordParam.dframerate;

	return S_OK;
}

// ����¼����Ƶ������
HRESULT CVideoRecord::SetAudioSampleRate(e_SampleRate samplerate)
{
	//�����Ƶ������ȡֵ��Χ�ĺϷ���
	CheckRange(samplerate, _E_SR_8K, _E_SR_48K);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//������Ƶ������
	m_RecordParam.nSampleRate = samplerate;

	return S_OK;
}

HRESULT CVideoRecord::GetAudioSampleRate(e_SampleRate& samplerate)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//��ȡ��Ƶ������
	samplerate = m_RecordParam.nSampleRate;

	return S_OK;
}

//����¼����Ƶ������
HRESULT CVideoRecord::SetAudioChannel(int channel)
{
	//�����Ƶ������ȡֵ��Χ�ĺϷ���
	CheckRange(channel, 1, 2);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//���õ�ǰ��Ƶ������
	m_RecordParam.nChannel = channel;

	return S_OK;
}

//��ȡ¼����Ƶ������
HRESULT CVideoRecord::GetAudioChannel(int& channel)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//��ȡ��ǰ��Ƶ������
	channel = m_RecordParam.nChannel;

	return S_OK;
}

// ���������Ŵ�
HRESULT CVideoRecord::SetAudioVolume(long AudioVolume)
{
	//����ļ������ĺϷ���
	CheckRange(AudioVolume, 0, 1000);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//���õ�ǰ��Ƶ����
	m_RecordParam.AudioVolume = AudioVolume;

	return S_OK;
}

/******************************************************************
	������:     GetAudioVolume
	��������:   ��ȡ�����Ŵ�
	����1��     [out] long& AudioVolume��//�����Ŵ�С��100��ʾ������С������100��ʾ�����Ŵ󣬰�����/100�ı�������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

HRESULT CVideoRecord::GetAudioVolume(long& AudioVolume)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//��ȡ��ǰ��Ƶ����
	AudioVolume = m_RecordParam.AudioVolume;

	return S_OK;
}


//����¼����Ƶ������ʽ
HRESULT CVideoRecord::SetDstFileContainer(e_RECORD_CONTAINER container)
{
	//����ļ������ĺϷ���
	CheckRange(container, E_CON_FLV, E_CON_MPEG_TS);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//����Ŀ���ļ���������
	m_RecordParam.eContainer = CointerConvert(container);

	return S_OK;
}

//��ȡ¼����Ƶ������ʽ
HRESULT CVideoRecord::GetDstFileContainer(e_RECORD_CONTAINER& container)
{
	//����ļ������ĺϷ���
	CheckRange(container, E_CON_FLV, E_CON_MPEG_TS);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//��ȡĿ���ļ���������
	container = CointerConvert(m_RecordParam.eContainer);

	return S_OK;
}

//����¼����Ƶ����·��������
HRESULT CVideoRecord::SetDstFilePath(wchar_t* pPath)
{

	CLog::GetInstance()->trace(3, L"SetDstFilePath:%s\n", pPath);
	//����ļ�·��ָ��ĺϷ���
	CheckPointer(pPath, E_POINTER);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//����Ŀ���ļ�·��
	m_RecordParam.RecordPath.Format(_T("%s"), pPath);
	CLog::GetInstance()->trace(3, L"SetDstFilePath:%s end\n", pPath);
	return S_OK;
}

//��ȡ¼����Ƶ����·��������
HRESULT CVideoRecord::GetDstFilePath(wchar_t* pPath, int& len)
{
	if(len < m_RecordParam.RecordPath.GetLength() + 1)
	{
		len = m_RecordParam.RecordPath.GetLength() + 1;
		return S_FALSE;
	}

	//����ļ�·��ָ��ĺϷ���
	CheckPointer(pPath, E_POINTER);

	wcscpy_s(pPath, len, m_RecordParam.RecordPath.GetBuffer());

	return S_OK;
}

//��ʼ¼����Ƶ
HRESULT CVideoRecord::StartRecord()
{
	CLog::GetInstance()->trace(3, L"StartRecord\n");
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//����������ӿ��Ƿ����
	CheckPointer(m_pITaskManager, E_FAIL);

	//����¼������
	HRESULT hr = S_OK;
	CLog::GetInstance()->trace(3, L"CreateRecordTask\n");
	if(e_Stop == m_RecordState)
	{
		hr = m_pITaskManager->CreateRecordTask(&m_RecordParam);
		if(FAILED(hr))
		{
			return hr;
		}
	}	

	CLog::GetInstance()->trace(3, L"StartRecord begin\n");

	//��ʼ¼����Ƶ
	hr = m_pITaskManager->StartRecord();

	if(FAILED(hr))
	{
		//ʧ�ܣ�ֹͣ���˳�
		m_pITaskManager->StopRecord();
		return hr;
	}

	m_RecordState = e_Running;


	CLog::GetInstance()->trace(3, L"StartRecord end\n");

	return S_OK;
}

HRESULT CVideoRecord::PauseRecord()
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	m_RecordState = e_Pause;

	//����������ӿ��Ƿ����
	CheckPointer(m_pITaskManager, E_FAIL);
	OutputDebugString(L"**********************m_pITaskManager->PauseRecord()\n");
	//ֹͣ¼����Ƶ
	HRESULT hr = m_pITaskManager->PauseRecord();
	OutputDebugString(L"**********************m_pITaskManager->PauseRecord() end\n");

	if(FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

//ֹͣ¼����Ƶ
HRESULT CVideoRecord::StopRecord()
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	m_RecordState = e_Stop;

	//����������ӿ��Ƿ����
	CheckPointer(m_pITaskManager, E_FAIL);

	//ֹͣ¼����Ƶ
	HRESULT hr = m_pITaskManager->StopRecord();

	if(FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

//��ȡ��һ֡��Ƶͼ��
HRESULT CVideoRecord::GetFirstBitmapImage(BYTE** ppData)
{
	CheckPointer(ppData, E_POINTER);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	CheckPointer(m_pITaskManager, E_FAIL);

	return m_pITaskManager->GetFirstBitmapImage(ppData);
}

HRESULT CVideoRecord::SetCallBack(IQvodCallBack* pcallback)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	if(m_pITaskManager)
	{
		return m_pITaskManager->SetCallBack(pcallback);
	}

	return E_FAIL;
}

/******************************************************************
	������:     GetCallBack
	��������:   ������Ϣ�ص��ӿ�
	����1:      [out]interface IQvodCallBack** ppcallback���ص���Ϣ�ӿ�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

HRESULT CVideoRecord::GetCallBack(interface IQvodCallBack** ppcallback)
{
	//��������Ϸ��Լ��
	CheckPointer(ppcallback, E_POINTER);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	if(m_pITaskManager)
	{
		return m_pITaskManager->GetCallBack(ppcallback);
	}

	return E_FAIL;
}

HRESULT CVideoRecord::VideoResizeNotify(RECT* prect)
{
	//��������Ϸ��Լ��
	CheckPointer(prect, E_POINTER);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	if(m_pITaskManager)
	{
		return m_pITaskManager->VideoResizeNotify(prect);
	}

	return E_FAIL;
}

HRESULT CVideoRecord::SetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData)
{
	CLog::GetInstance()->trace(3, L"SetTailLogo******************\n");

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);


	return E_FAIL;
}

HRESULT CVideoRecord::GetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData)
{
	CLog::GetInstance()->trace(3, L"GetTailLogo******************\n");
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);


	return E_FAIL;
}

// ����ʵ��
void CVideoRecord::DestroyInstance()
{
	delete this;
}

e_CONTAINER CVideoRecord::CointerConvert(e_RECORD_CONTAINER rc)
{
	switch(rc)
	{
	case E_CON_FLV:
		return _E_CON_FLV;
	case E_CON_MP4:
		return _E_CON_MP4;
	case E_CON_MPEG_TS:
		return _E_CON_TS;
	default:
		return _E_CON_UNKNOW;
	}
}

e_RECORD_CONTAINER CVideoRecord::CointerConvert(e_CONTAINER container)
{
	switch(container)
	{
	case _E_CON_FLV:
		return E_CON_FLV;
	case _E_CON_MP4:
		return E_CON_MP4;
	case _E_CON_TS:
		return E_CON_MPEG_TS;
	default:
		return E_CON_UNKNOW;
	}
}