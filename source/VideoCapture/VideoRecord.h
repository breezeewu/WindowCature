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
	函数名:     SetVideoHwnd
	函数描述:   设置视频渲染窗口句柄
	参数1：     [in] HWND hwd， 视频播放窗口句柄
	参数2：     [in] RECT* prect,截取区域，可以为空，默认为整个窗口
	返回值：    成功返回S_OK，否则为失败
	*****************************************************************/

	HRESULT SetVideoHwnd(HWND hwd, RECT* prect = NULL);

/***************************************************************
	函数名:     GetVideoHwnd
	函数描述:   获取视频渲染窗口句柄
	参数1：     [out] HWND& hwd， 视频播放窗口句柄
	参数2：     [out] RECT* prect,截取区域
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	HRESULT GetVideoHwnd(HWND& hwd, RECT* prect);



/***************************************************************
	函数名:     SetVideoSolutionRatio  
	函数描述:   设置录制视频分辨率
	参数1：     int nWidth， 目标视频宽
	参数2：     int nHeight，目标视频高
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	HRESULT SetDstVideoSolutionRatio(int nWidth, int nHeigh);

/***************************************************************
	函数名:     GetVideoSolutionRatio  
	函数描述:  获取录制视频分辨率
	参数1：     [out] int& nWidth， 目标视频宽
	参数2：     [out] int& nHeight，目标视频高
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	HRESULT GetDstVideoSolutionRatio(int& nWidth, int& nHeigh);

/******************************************************************
	函数名:     SetDstVideoFrameRate
	函数描述:   设置录制视频帧率
	参数1：     [in] double dFramerate， 帧率
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT SetDstVideoFrameRate(double dFramerate);

/******************************************************************
	函数名:     GetDstVideoFrameRate
	函数描述:   获取录制视频帧率
	参数1：     [out] double& dFramerate， 帧率
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT GetDstVideoFrameRate(double& dFramerate);



/******************************************************************
	函数名:     SetAudioSampleRate
	函数描述:   设置录制音频采样率
	参数1：     [in] e_SampleRate samplerate， 目标音频采样率（默认为44.1kHZ）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT SetAudioSampleRate(e_SampleRate samplerate);

/******************************************************************
	函数名:     GetAudioSampleRate
	函数描述:   获取录制音频采样率
	参数1：     [out] e_SampleRate& samplerate， 目标音频采样率（默认为44.1kHZ）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT GetAudioSampleRate(e_SampleRate& samplerate);

/******************************************************************
	函数名:     SetAudioChannel
	函数描述:   设置录制音频声道数
	参数1：     [in] int channel， 目标音频声道数（默认为2声道）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT SetAudioChannel(int channel);

/******************************************************************
	函数名:     GetAudioChannel
	函数描述:   获取录制音频声道数
	参数1：     [out] int channel， 目标音频声道数（默认为2声道）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT GetAudioChannel(int& channel);

/******************************************************************
	函数名:     SetAudioVolume
	函数描述:   设置音量放大
	参数1：     [in] long AudioVolume，//音量放大，小于100表示音量减小，大于100表示音量放大，按音量/100的比例进行
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT SetAudioVolume(long AudioVolume);

/******************************************************************
	函数名:     GetAudioVolume
	函数描述:   获取音量放大
	参数1：     [out] long& AudioVolume，//音量放大，小于100表示音量减小，大于100表示音量放大，按音量/100的比例进行
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT GetAudioVolume(long& AudioVolume);


/******************************************************************
	函数名:     SetFileContainer                                
	函数描述:   设置录制视频容器格式
	参数1：     [in] e_RECORD_CONTAINER container, 目标文件容器类型
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT SetDstFileContainer(e_RECORD_CONTAINER container);

/******************************************************************
	函数名:     GetDstFileContainer                                
	函数描述:   获取录制视频容器格式
	参数1：     [out] e_RECORD_CONTAINER& container, 目标文件容器类型
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT GetDstFileContainer(e_RECORD_CONTAINER& container);

/******************************************************************
	函数名:     SetDstFilePath                                
	函数描述:   设置录制视频保存路径及名称
	参数1：     [in] wchar_t* pPath, 目标文件保存路径
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT SetDstFilePath(wchar_t* pPath);

/******************************************************************
	函数名:     GetDstFilePath                                
	函数描述:   获取录制视频保存路径及名称
	参数1：     [in] wchar_t* pPath, 目标文件保存路径
	参数2：     [in,out] int& len, pPath的buffer长度，如小于当前路径buffer的长度，则返回当前路径长度而不返回当前路径(pPath)
	返回值：    成功返回S_OK，否则为失败
	备注：      pPath 内存长度不够返回S_FALSE，并返回当前路径内存大小
*******************************************************************/

	HRESULT GetDstFilePath(wchar_t* pPath, int& len);


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

	HRESULT SetCallBack(struct IQvodCallBack* pcallback);

/******************************************************************
	函数名:     GetCallBack
	函数描述:   设置消息回调接口
	参数1:      [out]interface IQvodCallBack** ppcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT GetCallBack(struct IQvodCallBack** ppcallback);

/******************************************************************
	函数名:     VideoResizeNotify
	函数描述:   视频窗口大小调整通知
	参数1:      [in]RECT* prect，视频窗口大小调整通知参数
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT VideoResizeNotify(RECT* prect);

/******************************************************************
	函数名:     SetTailLogo
	函数描述:   设置视频尾部logo
	参数1:      [in,out]BITMAPINFOHEADER* pbih，24位或32位的bmp位图头结构体
	参数2:		[in,out]BYTE* pBmpData,位图数据
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT SetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData);

/******************************************************************
	函数名:     SetTailLogo
	函数描述:   获取视频尾部logo
	参数1:      [in,out]BITMAPINFOHEADER* pbih，24位或32位的bmp位图头结构体
	参数2:		[in,out]BYTE* pBmpData,位图数据
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	HRESULT GetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData);

/******************************************************************
	函数名:     DestroyInstance
	函数描述:   销毁实例
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/
	void DestroyInstance();

protected:
	//容器转换
	e_CONTAINER CointerConvert(e_RECORD_CONTAINER rc);

	e_RECORD_CONTAINER CointerConvert(e_CONTAINER container);

protected:

	HWND					m_hwnd;//视频录制窗口
	int						m_nWidth;//目标视频宽
	int						m_nHeigh;//目标视频高
	int						m_nChannel;//声道数
	int						m_nSampleRate;//音频采样率
	double					m_dFrameRate;//帧率
	e_RECORD_CONTAINER		m_eContainer;//容器类型
	e_State					m_RecordState;//录制状态
	Record_Param			m_RecordParam;//录制参数
	ITaskManager*			m_pITaskManager;//任务管理接口
	CCritSec				m_CritSecLock;//临界锁
};
