#include "StdAfx.h"
#include "TaskManager.h"
#include "SystemInfo.h"
#include "GUID.h"
#include "HookAPIFunc.h"

//��Ļץȡsource clsid
const GUID CLSID_ScreenSource = {0x4EA6930A, 0x2C8A, 0x4ae6, {0xA5, 0x61, 0x56, 0xE4, 0xB5, 0x04, 0x44, 0x39}};

//��Ƶץȡsource clsid
const GUID CLSID_AudioSource = {0x8E14549B, 0xDB61, 0x4309, {0xAF, 0xA1, 0x35, 0x78, 0xE9, 0x27, 0xE9, 0x35}};
//xp��Ƶץȡsource clsid
const GUID CLSID_AUDIO_HOOK_SOURCE = {0xc9960314, 0xeccd, 0x4b58, {0x8d, 0xf8, 0x26, 0x4f, 0x4, 0x9e, 0x1b, 0xf}};

CTaskManager::CTaskManager(void)
{
	m_nImageSize	= 0;
	m_pImageBuffer	= NULL;
	m_pIHookControl = NULL;
	m_hHookHandle   = NULL;
	m_pCallBack		= NULL;
}

CTaskManager::~CTaskManager(void)
{
	if(m_hHookHandle)
	{
		DestoryMediaHook pfunc = (DestoryMediaHook)GetProcAddress(m_hHookHandle, "DestoryMediaHook");
		pfunc((void**)&m_pIHookControl);
	}

	if(m_pImageBuffer)
	{
		delete[] m_pImageBuffer;
		m_pImageBuffer = NULL;
	}
}

HRESULT CTaskManager::CreateRecordTask(Record_Param* param)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	m_RecordParam.init();
	if(param)
	{
		m_RecordParam = *param;
	}

	if(CSystemInfo::GetInstance().GetMajorVersion() <= 5)
	{
		return E_FAIL;
	}

	HRESULT hr = BuildGraph();

	//���filtergraph�Ƿ���سɹ�
	CheckPointer(m_pGraph, E_FAIL);

	//������Ļ¼��Filter
	m_pScreenSource = LoadFilter(CLSID_ScreenSource);

	ASSERT(m_pScreenSource);

	if(m_pScreenSource && SUCCEEDED(m_pScreenSource->QueryInterface(__uuidof(IScreenCapture), (void**)&m_pIScreenCapture))&& m_pIScreenCapture)
	{
		m_pIScreenCapture->SetVideoHwnd(m_RecordParam.hwnd, &m_RecordParam.WinRect);

		if(m_RecordParam.nWidth > 0 &&  m_RecordParam.nHeigh > 0)
		{
			m_pIScreenCapture->SetVideoSolution(m_RecordParam.nWidth, m_RecordParam.nHeigh);
		}
	}

	if(CSystemInfo::GetInstance().GetMajorVersion() > 5)
	{
		//������Ƶ����ץȡFilter
		m_pAudioSource = LoadFilter(CLSID_AudioSource);
	}
	else
	{
		//LoadAPIHook();

		////LoadMediaHook(&m_pIHookControl);
		//return S_OK;
		//������Ƶ����ץȡFilter
		m_pAudioSource = LoadFilter(CLSID_AUDIO_HOOK_SOURCE);
		if(m_pAudioSource && SUCCEEDED(m_pAudioSource->QueryInterface(__uuidof(ICaptureConfig), (void**)&m_pICaptureConfig))&& m_pICaptureConfig)
		{
			CHookFuncInfo::GetInstance()->SetupAudioSource(m_pICaptureConfig);
		}

		//MessageBox(0,0,0,0);
		if(SUCCEEDED(LoadMediaHook(&m_pIHookControl)) && m_pIHookControl)
		{
			CHookFuncInfo::GetInstance()->InstallHookFunc(m_pIHookControl);
		}
	}

	ASSERT(m_pAudioSource);

	//����ffmpeg mux filter
	m_pffmpegMux = LoadFilter(CLSID_QvodFFmpegMux);

	ASSERT(m_pffmpegMux);

	//��Ƶ¼��Source��ffmpegmux����
	hr = ConnectFilter(m_pScreenSource, m_pffmpegMux);
	//MessageBox(0,0,0,0);
	if(FAILED(hr))
	{
		//������Ƶsourceʧ�ܣ����ش���
		DestroyGraph();

		return hr;
	}

	//��Ƶ¼��Source��ffmpegmux����
	hr = ConnectFilter(m_pAudioSource, m_pffmpegMux);
	
	if(FAILED(hr))
	{
		//������Ƶsourceʧ�ܣ����ش���
		DestroyGraph();

		return hr;
	}

	//CComQIPtr<IFFmpegMux>			pIFFmpegMux;
	CComQIPtr<IFileSinkFilter>		pIFileSinkFilter;

	//ffmpegmux���ýӿڲ�ѯ
	if(SUCCEEDED(m_pffmpegMux->QueryInterface(__uuidof(IFFmpegMux), (void**)&m_pIFFmpegMux))&& m_pIFFmpegMux)
	{
		MuxConfigure(m_pIFFmpegMux);
	}

	if(SUCCEEDED(m_pffmpegMux->QueryInterface(IID_IFileSinkFilter, (void**)&pIFileSinkFilter))&& pIFileSinkFilter)
	{
		if(m_RecordParam.RecordPath.IsEmpty())
		{
			m_RecordParam.RecordPath = L"D:\\test.mp4";
		}
		pIFileSinkFilter->SetFileName(m_RecordParam.RecordPath.GetBuffer(), NULL);
	}
	return hr;
}

HRESULT CTaskManager::StartRecord()
{
	//���filtergraph�Ƿ���سɹ�
	CheckPointer(m_pGraph, E_FAIL);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	CComQIPtr<IMediaControl> pMediaControl;

	//��ѯgraph���ƽӿ�
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&pMediaControl);

	//����graph
	hr = pMediaControl->Run();

	return hr;
}

HRESULT CTaskManager::PauseRecord()
{
	//���filtergraph�Ƿ���سɹ�
	CheckPointer(m_pGraph, E_FAIL);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	CComQIPtr<IMediaControl> pMediaControl;

	//��ѯgraph���ƽӿ�
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&pMediaControl);

	//����graph
	hr = pMediaControl->Pause();

	return hr;
}

HRESULT CTaskManager::StopRecord()
{
	//���filtergraph�Ƿ���سɹ�
	CheckPointer(m_pGraph, E_FAIL);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	CComQIPtr<IMediaControl> pMediaControl;

	//��ѯgraph���ƽӿ�
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&pMediaControl);

	//ֹͣ¼��
	hr = pMediaControl->Stop();

	//��ȡ��һ�Ž�ͼ
	GetFirstBitmapImage(NULL);

	//����Audio���ýӿ�
	CHookFuncInfo::GetInstance()->SetupAudioSource(NULL);

	//������·
	DestroyGraph();	

	if(m_pIHookControl)
	{
		CHookFuncInfo::GetInstance()->UnInstallHookFunc(m_pIHookControl);
	}

	return hr;
}

HRESULT CTaskManager::GetFirstBitmapImage(BYTE** ppData)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	//return S_OK;
	int len = 0;
	HRESULT hr = E_FAIL;
	BITMAPINFOHEADER bmi;

	//Screen Captureδ�����٣���Screen Capture��ȡ��ͼ
	if(m_pIScreenCapture)
	{
		//��ȡλͼͷ��Ϣ
		if(SUCCEEDED(m_pIScreenCapture->GetFirstBitmapinfo(&bmi, &len)) && len >= sizeof(bmi))
		{
			if(m_nImageSize < len)
			{
				//���·����ڴ�
				if(m_pImageBuffer)
				{
					delete[] m_pImageBuffer;
				}
				m_pImageBuffer = new BYTE[len + sizeof(bmi)];
				m_nImageSize = len + sizeof(bmi);
			}

			//����λͼͷ��Ϣ
			memcpy(m_pImageBuffer, &bmi, sizeof(bmi));

			//����λͼ������Ϣ
			hr = m_pIScreenCapture->GetFirstBitmapImage(m_pImageBuffer+sizeof(bmi), len);
			ASSERT(hr == S_OK);
		}
	}
	
	if(ppData && m_nImageSize > sizeof(bmi))
	{
		//����λͼͷ��Ϣ+λͼ������Ϣ
		*ppData = m_pImageBuffer;
		hr = S_OK;
	}
	return hr;
}

HRESULT CTaskManager::SetCallBack(interface IQvodCallBack* pcallback)
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	m_pCallBack = pcallback;

	if(m_pIFFmpegMux)
	{
		m_pIFFmpegMux->SetQvodCallBack(m_pCallBack);
	}

	return S_OK;
}

HRESULT CTaskManager::GetCallBack(interface IQvodCallBack** ppcallback)
{
	//����������
	CheckPointer(ppcallback, E_POINTER);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	*ppcallback = m_pCallBack;

	return S_OK;
}

HRESULT CTaskManager::VideoResizeNotify(RECT* prect)
{
	//����������
	CheckPointer(prect, E_POINTER);

	//���״̬��
	CAutoLock Lock(&m_CritSecLock);
	if(m_pIScreenCapture)
	{
		return m_pIScreenCapture->SetVideoHwnd(m_RecordParam.hwnd, prect);
	}

	return E_FAIL;
}

void CTaskManager::ResetFilters()
{
	//���״̬��
	CAutoLock Lock(&m_CritSecLock);

	__super::ResetFilters();

	m_pIScreenCapture.Release();
	m_pICaptureConfig.Release();
	m_pIFFmpegMux.Release();
	ASSERT(ReleaseFilter(m_pScreenSource) == 0);
	ASSERT(ReleaseFilter(m_pAudioSource) == 0);
	ASSERT(ReleaseFilter(m_pffmpegMux) == 0);
	
}

HRESULT CTaskManager::MuxConfigure(IFFmpegMux* pIFFmpegMux)
{
	CheckPointer(pIFFmpegMux, E_POINTER);

	MUX_CONFIG muxconfig;
	muxconfig.ContainerType			= m_RecordParam.eContainer;
	muxconfig.VideoCodec.CodecType	= _V_H264;
	muxconfig.VideoCodec.width		= m_RecordParam.nWidth;
	muxconfig.VideoCodec.height		= m_RecordParam.nHeigh;
	muxconfig.VideoCodec.bitrate	= 0;//m_RecordParam.nWidth * m_RecordParam.nHeigh * m_RecordParam.dframerate 3 / 2/20000;
	muxconfig.VideoCodec.framerate	= m_RecordParam.dframerate;
	muxconfig.AudioCodec.channel	= m_RecordParam.nChannel;
	muxconfig.AudioCodec.samplingrate = CalculateSampleRate(m_RecordParam.nSampleRate);
	muxconfig.AudioCodec.CodecType	= _A_AAC;
	muxconfig.AudioCodec.bitrate	= 0;//m_RecordParam.nChannel * CalculateSampleRate(m_RecordParam.nSampleRate) * 2/2500;
	muxconfig.AudioVolume			= 100;
	muxconfig.ContainerType			= m_RecordParam.eContainer;

	/*pIFFmpegMux->SetMuxConfig(&muxconfig);*/

	if(m_pCallBack)
	{
		pIFFmpegMux->SetQvodCallBack(m_pCallBack);
	}

	pIFFmpegMux->SetMuxThreadCount(CSystemInfo::GetInstance().GetProcessNum());
	return S_OK;
}

int CTaskManager::CalculateSampleRate(e_SampleRate eSampleRate)
{
	switch(eSampleRate)
	{
	case _E_SR_8K:
		return 8000;
	case _E_SR_11K:
		return 11025;
	case _E_SR_22K:
		return 22050;
	case _E_SR_44K://Ĭ�ϲ�����
		return 44100;
	case _E_SR_48K:
		return 48000;
	default:
		return 44100;
	}
}


HRESULT CTaskManager::LoadMediaHook(IHookControl** ppIHookControl)
{

	CheckPointer(ppIHookControl, E_FAIL);

	m_hHookHandle = LoadLibrary(L"MediaHook.dll");

	CheckPointer(m_hHookHandle, E_FAIL);

	InitMediaHook pfunc = (InitMediaHook)GetProcAddress(m_hHookHandle, "InitMediaHook");

	if(pfunc)
	{
		return pfunc((void**)ppIHookControl);
	}

	return E_FAIL;
}

HRESULT CTaskManager::LoadAPIHook()
{
  HMODULE hmd = LoadLibrary(_T("HookAPI.dll"));

  FARPROC lpFun = GetProcAddress(hmd,"InstallWindowsHook");

  if ( lpFun != NULL  )
  {
    (HHOOK)lpFun();
	return S_OK;
  }

  return E_FAIL;
}