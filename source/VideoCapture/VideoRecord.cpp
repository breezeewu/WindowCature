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
	m_nWidth			= 640;		//目标视频宽
	m_nHeigh			= 480;		//目标视频高
	m_nSampleRate		= _E_SR_44K;//音频采样率
	m_nChannel			= 2;		//声道数
	m_dFrameRate		= 15;		//帧率
	m_eContainer		= E_CON_FLV;//容器类型
	m_RecordState		= e_Stop;	//录制状态

	m_pITaskManager = static_cast<ITaskManager*>(new CTaskManager());
}

CVideoRecord::~CVideoRecord(void)
{
	//销毁taskmanager
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
	//检测输入指针的合法性
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
//设置视频渲染窗口句柄
HRESULT CVideoRecord::SetVideoHwnd(HWND hwd, RECT* prect)
{
	//检测句柄的合法性
	CheckPointer(hwd, E_HANDLE);
	CLog::GetInstance()->trace(3, L"SetVideoHwnd:%0x\n", hwd);
	//添加状态锁
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

//获取视频渲染窗口句柄
HRESULT CVideoRecord::GetVideoHwnd(HWND& hwd, RECT* prect)
{
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//句柄赋值
	hwd = m_RecordParam.hwnd;

	return S_OK;
}

// 设置录制视频分辨率
HRESULT CVideoRecord::SetDstVideoSolutionRatio(int nWidth, int nHeigh)
{
	//检测分辨率取值范围的合法性
	CheckRange(nWidth, 100, 1280);
	CheckRange(nHeigh, 100, 720);

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	m_RecordParam.nWidth = nWidth;
	m_RecordParam.nHeigh = nHeigh;

	return S_OK;
}

HRESULT CVideoRecord::GetDstVideoSolutionRatio(int& nWidth, int& nHeigh)
{
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	nWidth = m_RecordParam.nWidth;
	nHeigh = m_RecordParam.nHeigh;

	return S_OK;
}

// 设置录制视频分辨率
HRESULT CVideoRecord::SetDstVideoFrameRate(double dFramerate)
{
	//检测分帧率取值范围的合法性
	CheckRange(dFramerate, 0, 25)

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//设置当期帧率
	m_RecordParam.dframerate = dFramerate;

	return S_OK;
}

// 设置录制视频分辨率
HRESULT CVideoRecord::GetDstVideoFrameRate(double& dFramerate)
{
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//获取当前帧率
	dFramerate = m_RecordParam.dframerate;

	return S_OK;
}

// 设置录制音频采样率
HRESULT CVideoRecord::SetAudioSampleRate(e_SampleRate samplerate)
{
	//检测音频采样率取值范围的合法性
	CheckRange(samplerate, _E_SR_8K, _E_SR_48K);

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//设置音频采样率
	m_RecordParam.nSampleRate = samplerate;

	return S_OK;
}

HRESULT CVideoRecord::GetAudioSampleRate(e_SampleRate& samplerate)
{
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//获取音频采样率
	samplerate = m_RecordParam.nSampleRate;

	return S_OK;
}

//设置录制音频声道数
HRESULT CVideoRecord::SetAudioChannel(int channel)
{
	//检测音频声道数取值范围的合法性
	CheckRange(channel, 1, 2);

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//设置当前音频声道数
	m_RecordParam.nChannel = channel;

	return S_OK;
}

//获取录制音频声道数
HRESULT CVideoRecord::GetAudioChannel(int& channel)
{
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//获取当前音频声道数
	channel = m_RecordParam.nChannel;

	return S_OK;
}

// 设置音量放大
HRESULT CVideoRecord::SetAudioVolume(long AudioVolume)
{
	//检测文件容器的合法性
	CheckRange(AudioVolume, 0, 1000);

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//设置当前音频音量
	m_RecordParam.AudioVolume = AudioVolume;

	return S_OK;
}

/******************************************************************
	函数名:     GetAudioVolume
	函数描述:   获取音量放大
	参数1：     [out] long& AudioVolume，//音量放大，小于100表示音量减小，大于100表示音量放大，按音量/100的比例进行
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

HRESULT CVideoRecord::GetAudioVolume(long& AudioVolume)
{
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//获取当前音频音量
	AudioVolume = m_RecordParam.AudioVolume;

	return S_OK;
}


//设置录制视频容器格式
HRESULT CVideoRecord::SetDstFileContainer(e_RECORD_CONTAINER container)
{
	//检测文件容器的合法性
	CheckRange(container, E_CON_FLV, E_CON_MPEG_TS);

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//设置目标文件容器类型
	m_RecordParam.eContainer = CointerConvert(container);

	return S_OK;
}

//获取录制视频容器格式
HRESULT CVideoRecord::GetDstFileContainer(e_RECORD_CONTAINER& container)
{
	//检测文件容器的合法性
	CheckRange(container, E_CON_FLV, E_CON_MPEG_TS);

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//获取目标文件容器类型
	container = CointerConvert(m_RecordParam.eContainer);

	return S_OK;
}

//设置录制视频保存路径及名称
HRESULT CVideoRecord::SetDstFilePath(wchar_t* pPath)
{

	CLog::GetInstance()->trace(3, L"SetDstFilePath:%s\n", pPath);
	//检测文件路径指针的合法性
	CheckPointer(pPath, E_POINTER);

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//设置目标文件路径
	m_RecordParam.RecordPath.Format(_T("%s"), pPath);
	CLog::GetInstance()->trace(3, L"SetDstFilePath:%s end\n", pPath);
	return S_OK;
}

//获取录制视频保存路径及名称
HRESULT CVideoRecord::GetDstFilePath(wchar_t* pPath, int& len)
{
	if(len < m_RecordParam.RecordPath.GetLength() + 1)
	{
		len = m_RecordParam.RecordPath.GetLength() + 1;
		return S_FALSE;
	}

	//检测文件路径指针的合法性
	CheckPointer(pPath, E_POINTER);

	wcscpy_s(pPath, len, m_RecordParam.RecordPath.GetBuffer());

	return S_OK;
}

//开始录制视频
HRESULT CVideoRecord::StartRecord()
{
	CLog::GetInstance()->trace(3, L"StartRecord\n");
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	//检测任务管理接口是否可用
	CheckPointer(m_pITaskManager, E_FAIL);

	//创建录制任务
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

	//开始录制视频
	hr = m_pITaskManager->StartRecord();

	if(FAILED(hr))
	{
		//失败，停止并退出
		m_pITaskManager->StopRecord();
		return hr;
	}

	m_RecordState = e_Running;


	CLog::GetInstance()->trace(3, L"StartRecord end\n");

	return S_OK;
}

HRESULT CVideoRecord::PauseRecord()
{
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	m_RecordState = e_Pause;

	//检测任务管理接口是否可用
	CheckPointer(m_pITaskManager, E_FAIL);
	OutputDebugString(L"**********************m_pITaskManager->PauseRecord()\n");
	//停止录制视频
	HRESULT hr = m_pITaskManager->PauseRecord();
	OutputDebugString(L"**********************m_pITaskManager->PauseRecord() end\n");

	if(FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

//停止录制视频
HRESULT CVideoRecord::StopRecord()
{
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	m_RecordState = e_Stop;

	//检测任务管理接口是否可用
	CheckPointer(m_pITaskManager, E_FAIL);

	//停止录制视频
	HRESULT hr = m_pITaskManager->StopRecord();

	if(FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

//获取第一帧视频图像
HRESULT CVideoRecord::GetFirstBitmapImage(BYTE** ppData)
{
	CheckPointer(ppData, E_POINTER);

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	CheckPointer(m_pITaskManager, E_FAIL);

	return m_pITaskManager->GetFirstBitmapImage(ppData);
}

HRESULT CVideoRecord::SetCallBack(IQvodCallBack* pcallback)
{
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	if(m_pITaskManager)
	{
		return m_pITaskManager->SetCallBack(pcallback);
	}

	return E_FAIL;
}

/******************************************************************
	函数名:     GetCallBack
	函数描述:   设置消息回调接口
	参数1:      [out]interface IQvodCallBack** ppcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

HRESULT CVideoRecord::GetCallBack(interface IQvodCallBack** ppcallback)
{
	//输入参数合法性检测
	CheckPointer(ppcallback, E_POINTER);

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);

	if(m_pITaskManager)
	{
		return m_pITaskManager->GetCallBack(ppcallback);
	}

	return E_FAIL;
}

HRESULT CVideoRecord::VideoResizeNotify(RECT* prect)
{
	//输入参数合法性检测
	CheckPointer(prect, E_POINTER);

	//添加状态锁
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

	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);


	return E_FAIL;
}

HRESULT CVideoRecord::GetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData)
{
	CLog::GetInstance()->trace(3, L"GetTailLogo******************\n");
	//添加状态锁
	CAutoLock Lock(&m_CritSecLock);


	return E_FAIL;
}

// 销毁实例
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