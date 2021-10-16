#pragma once
#include "windows.h"
#ifdef QDBASE_EXPORTS /*happyhuang x*/
#define QDSTRINGAPI __declspec(dllexport)
#else
#define QDSTRINGAPI __declspec(dllimport)
#endif

//视频录制接口
extern "C"
{
/***************************************************************
	函数名:     CreateVideoRecordTask_c
	函数描述:   设置创建视频录制任务
	参数1：     [in] long* pid
	参数2：     [out] IVideoCapture** ppIVideoCapture， 创建视频录制控制接口
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
 QDSTRINGAPI HRESULT CreateVideoRecordTask_c(long* pid);
/***************************************************************
	函数名:     SetVideoHwnd_c
	函数描述:   设置视频渲染窗口句柄
	参数1：     [in] long pid,taskid
	参数2：     [in] HWND hwd， 视频播放窗口句柄
	参数3：     [in] RECT* prect,截取区域，可以为空，默认为整个窗口
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
QDSTRINGAPI HRESULT SetVideoHwnd_c(long pid, HWND hwd, RECT* prect);

/***************************************************************
	函数名:     GetVideoHwnd_c
	函数描述:   获取视频渲染窗口句柄
	参数1：     [in] long pid,taskid
	参数2：     [out] HWND& hwd， 视频播放窗口句柄
	参数3：     [out] RECT* prect,截取区域
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
QDSTRINGAPI HRESULT GetVideoHwnd_c(long pid, HWND& hwd, RECT* prect);

/***************************************************************
	函数名:     SetVideoSolutionRatio_c  
	函数描述:   设置录制视频分辨率
	参数1：     [in] long pid,taskid
	参数2：     [in] int nWidth， 目标视频宽
	参数3：     [in] int nHeight，目标视频高
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

QDSTRINGAPI HRESULT SetDstVideoSolutionRatio_c(long pid, int nWidth, int nHeigh);

/***************************************************************
	函数名:     GetVideoSolutionRatio_c  
	函数描述:  获取录制视频分辨率
	参数1：     [in] long pid,taskid
	参数2：     [out] int& nWidth， 目标视频宽
	参数3：     [out] int& nHeight，目标视频高
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

QDSTRINGAPI HRESULT GetDstVideoSolutionRatio_c(long pid, int& nWidth, int& nHeigh);

/******************************************************************
	函数名:     SetDstVideoFrameRate_c
	函数描述:   设置录制视频帧率
	参数1：     [in] long pid,taskid
	参数2：     [in] double dFramerate， 帧率
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT SetDstVideoFrameRate_c(long pid, double dFramerate);

/******************************************************************
	函数名:     GetDstVideoFrameRate_c
	函数描述:   获取录制视频帧率
	参数1：     [in] long pid,taskid
	参数2：     [out] double& dFramerate， 帧率
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT GetDstVideoFrameRate_c(long pid, double& dFramerate);



/******************************************************************
	函数名:     SetAudioSampleRate_c
	函数描述:   设置录制音频采样率
	参数1：     [in] long pid,taskid
	参数2：     [in] e_SampleRate samplerate， 目标音频采样率（默认为44.1kHZ）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT SetAudioSampleRate_c(long pid, e_SampleRate samplerate);

/******************************************************************
	函数名:     GetAudioSampleRate_c
	函数描述:   获取录制音频采样率
	参数1：     [in] long pid,taskid
	参数2：     [out] e_SampleRate& samplerate， 目标音频采样率（默认为44.1kHZ）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT GetAudioSampleRate_c(long pid, (e_SampleRate& samplerate);

/******************************************************************
	函数名:     SetAudioChannel_c
	函数描述:   设置录制音频声道数
	参数1：     [in]long pid,taskid
	参数2：     [in] int channel， 目标音频声道数（默认为2声道）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT SetAudioChannel_c(long pid, int channel);

/******************************************************************
	函数名:     GetAudioChannel_c
	函数描述:   获取录制音频声道数
	参数1：     [in] long pid,taskid
	参数2：     [out] int channel， 目标音频声道数（默认为2声道）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT GetAudioChannel_c(long pid, int& channel);

/******************************************************************
	函数名:     SetAudioVolume_c
	函数描述:   设置音量放大
	参数1：     [in] long pid,taskid
	参数2：     [in] long AudioVolume，//音量放大，小于100表示音量减小，大于100表示音量放大，按音量/100的比例进行
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT SetAudioVolume_c(long pid, long AudioVolume);

/******************************************************************
	函数名:     GetAudioVolume_c
	函数描述:   获取音量放大
	参数1：     [in] long pid,taskid
	参数2：     [out] long& AudioVolume，//音量放大，小于100表示音量减小，大于100表示音量放大，按音量/100的比例进行
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT GetAudioVolume_c(long pid, long& AudioVolume);


/******************************************************************
	函数名:     SetFileContainer_c                                
	函数描述:   设置录制视频容器格式
	参数1：     [in] long pid,taskid
	参数2：     [in] e_RECORD_CONTAINER container, 目标文件容器类型
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT SetDstFileContainer_c(long pid, e_RECORD_CONTAINER container);

/******************************************************************
	函数名:     GetDstFileContainer_c                                
	函数描述:   获取录制视频容器格式
	参数1：     [in] long pid,taskid
	参数2：     [out] e_RECORD_CONTAINER& container, 目标文件容器类型
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT GetDstFileContainer_c(long pid, e_RECORD_CONTAINER& container);

/******************************************************************
	函数名:     SetDstFilePath_c                                
	函数描述:   设置录制视频保存路径及名称
	参数1：     [in] long pid,taskid
	参数2：     [in] wchar_t* pPath, 目标文件保存路径
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT SetDstFilePath_c(long pid, wchar_t* pPath);

/******************************************************************
	函数名:     GetDstFilePath_c                                
	函数描述:   获取录制视频保存路径及名称
	参数1：     [in] long pid,taskid
	参数2：     [in] wchar_t* pPath, 目标文件保存路径
	参数3：     [in,out] int& len, pPath的buffer长度，如小于当前路径buffer的长度，则返回当前路径长度而不返回当前路径(pPath)
	返回值：    成功返回S_OK，否则为失败
	备注：      pPath 内存长度不够返回S_FALSE，并返回当前路径内存大小
*******************************************************************/

QDSTRINGAPI HRESULT GetDstFilePath_c(long pid, wchar_t* pPath, int& len);


/******************************************************************
	函数名:     StartRecord_c
	函数描述:   开始录制视频
	参数1：     [in] long pid,taskid
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT StartRecord_c(long pid);

/******************************************************************
	函数名:     PauseRecord_c
	函数描述:   暂停录制视频
	参数1：     [in] long pid,taskid
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT PauseRecord_c(long pid);

/******************************************************************
	函数名:     StopRecord_c
	函数描述:    停止录制视频
	参数1：     [in] long pid,taskid
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT StopRecord_c(long pid);

/******************************************************************
	函数名:     GetFirstBitmapImage_c
	函数描述:   获取第一帧视频图像
	参数1：     [in] long pid,taskid
	参数2:      PBYTE *ppbuf，输出，BITMAPINFOHEADER+位图数据
	返回值：    成功返回S_OK，否则为失败
	备注：		该函数在开始录制之后才能调用
*******************************************************************/

QDSTRINGAPI HRESULT GetFirstBitmapImage_c(long pid, BYTE** ppData);

/******************************************************************
	函数名:     SetCallBack_c
	函数描述:   设置消息回调接口
	参数1：     [in] long pid,taskid
	参数2:      [in] interface IQvodCallBack* pcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT SetCallBack_c(long pid, struct IQvodCallBack* pcallback);

/******************************************************************
	函数名:     GetCallBack_c
	函数描述:   设置消息回调接口
	参数1：     [in] long pid,taskid
	参数2:      [out] interface IQvodCallBack** ppcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT GetCallBack_c(long pid, struct IQvodCallBack** ppcallback);

/******************************************************************
	函数名:     VideoResizeNotify_c
	函数描述:   视频窗口大小调整通知
	参数1：     [in] long pid,taskid
	参数2:      [in] RECT* prect，视频窗口大小调整通知参数
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

QDSTRINGAPI HRESULT VideoResizeNotify_c(long pid, RECT* prect);

/******************************************************************
	函数名:     DestroyInstance_c
	函数描述:   销毁实例
	参数1：     [in] long pid,taskid
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/
QDSTRINGAPI void DestroyInstance_c(long pid);
};

//视频增强接口
extern "C"
{
}