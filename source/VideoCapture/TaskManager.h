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
	函数名:     CreateRecordTask
	函数描述:   创建视频录制任务
	参数1:		Record_Param* param, 视频录制参数配置，可为空，如果为空则使用默认值
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/
	HRESULT CreateRecordTask(Record_Param* param);

/******************************************************************
	函数名:     StartRecord
	函数描述:   开始录制视频
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT StartRecord();

/******************************************************************
	函数名:     PauseRecord
	函数描述:   暂停录制视频
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT PauseRecord();

/******************************************************************
	函数名:     StopRecord
	函数描述:    停止录制视频
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT StopRecord();

/******************************************************************
	函数名:     GetFirstBitmapImage
	函数描述:   获取第一帧视频图像
	参数1:      PBYTE *ppbuf，输出，BITMAPINFOHEADER+位图数据
	返回值：    成功返回S_OK，否则为失败
	备注：		该函数在开始录制之后才能调用
*******************************************************************/

	HRESULT GetFirstBitmapImage(BYTE** ppData);

/******************************************************************
	函数名:     SetCallBack
	函数描述:   设置消息回调接口
	参数1:      [in]interface IQvodCallBack* pcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT SetCallBack(interface IQvodCallBack* pcallback);

/******************************************************************
	函数名:     GetCallBack
	函数描述:   设置消息回调接口
	参数1:      [out]interface IQvodCallBack** ppcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT GetCallBack(interface IQvodCallBack** ppcallback);

/******************************************************************
	函数名:     VideoResizeNotify
	函数描述:   视频窗口大小调整通知
	参数1:      [in]RECT* prect，视频窗口大小调整通知
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT VideoResizeNotify(RECT* prect);

protected:

	//重置filter
	void ResetFilters();

	//ffmpeg mux configure
	HRESULT MuxConfigure(interface IFFmpegMux* pIFFmpegMux);

	//计算音频samplerate
	int CalculateSampleRate(e_SampleRate eSampleRate);

	HRESULT LoadMediaHook(IHookControl** ppIHookControl);
	HRESULT LoadAPIHook();

protected:
	CComQIPtr<IBaseFilter>				m_pScreenSource;//屏幕抓取source
	CComQIPtr<IBaseFilter>				m_pAudioSource;//音频抓取source
	CComQIPtr<IBaseFilter>				m_pffmpegMux;//ffmpeg mux
	CComQIPtr<IScreenCapture>			m_pIScreenCapture;
	CComQIPtr<ICaptureConfig>			m_pICaptureConfig;
	CComQIPtr<IFFmpegMux>				m_pIFFmpegMux;//ffmpegmux配置接口

	IHookControl*						m_pIHookControl;
	interface IQvodCallBack*			m_pCallBack;	//回调函数接口
	HMODULE								m_hHookHandle;
	Record_Param						m_RecordParam;//视频录制参数
	BYTE*								m_pImageBuffer;//录制视频第一帧BMP位图
	DWORD								m_nImageSize;	//图像大小
	CCritSec							m_CritSecLock;//临界锁

};
