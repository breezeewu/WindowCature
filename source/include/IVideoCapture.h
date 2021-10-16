#pragma once
#include <streams.h>

//this interface disigned for third-part video record
enum e_RECORD_CONTAINER
{
	E_CON_UNKNOW = 0,
	E_CON_FLV = 1,
	E_CON_MP4,
	E_CON_MPEG_TS,
	//_E_CON_AVI,
	//_E_CON_MKV,
	
};

enum e_SampleRate
{
	_E_SR_8K,	//电话采用的音频采样率
	_E_SR_11K,	
	_E_SR_22K,	//无线广播采用的音频采样率
	_E_SR_44K,	//音频 CD, 也常用于 MPEG-1 音频（VCD, SVCD, MP3）所用音频采样率，在此模块为音频采样率默认值
	_E_SR_48K	//miniDV、数字电视、DVD、DAT、电影和专业音频所用的数字声音所用采样率
};

#ifdef COM_INTERFACE
[uuid("99F68BE0-A46C-42BA-B90B-2EF5053F9430")]
DECLARE_INTERFACE_(IVideoCapture, IUnknown)
{
#else
class IVideoCapture
{
public:
#endif

/***************************************************************
	函数名:     SetVideoHwnd
	函数描述:   设置视频渲染窗口句柄
	参数1：     [in] HWND hwd， 视频播放窗口句柄
	参数2：     [in] RECT* prect,截取区域，可以为空，默认为整个窗口
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT SetVideoHwnd(HWND hwd, RECT* prect = NULL) = 0;

/***************************************************************
	函数名:     GetVideoHwnd
	函数描述:   获取视频渲染窗口句柄
	参数1：     [out] HWND& hwd， 视频播放窗口句柄
	参数2：     [out] RECT* prect,截取区域
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT GetVideoHwnd(HWND& hwd, RECT* prect) = 0;

/***************************************************************
	函数名:     SetVideoSolutionRatio  
	函数描述:   设置录制视频分辨率
	参数1：     int nWidth， 目标视频宽
	参数2：     int nHeight，目标视频高
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT SetDstVideoSolutionRatio(int nWidth, int nHeigh) = 0;

/***************************************************************
	函数名:     GetVideoSolutionRatio  
	函数描述:  获取录制视频分辨率
	参数1：     [out] int& nWidth， 目标视频宽
	参数2：     [out] int& nHeight，目标视频高
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT GetDstVideoSolutionRatio(int& nWidth, int& nHeigh) = 0;

/******************************************************************
	函数名:     SetDstVideoFrameRate
	函数描述:   设置录制视频帧率
	参数1：     int nWidth， 目标视频宽
	参数2：     int nHeight，目标视频高
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT SetDstVideoFrameRate(double dFramerate) = 0;

/******************************************************************
	函数名:     GetDstVideoFrameRate
	函数描述:   设置录制视频帧率
	参数1：     [out] int nWidth， 目标视频宽
	参数2：     [out] int nHeight，目标视频高
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT GetDstVideoFrameRate(double& dFramerate) = 0;

/******************************************************************
	函数名:     SetAudioSampleRate
	函数描述:   设置录制音频采样率
	参数1：     [in] e_SampleRate samplerate， 目标音频采样率（默认为44.1kHZ）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT SetAudioSampleRate(e_SampleRate samplerate) = 0;

/******************************************************************
	函数名:     GetAudioSampleRate
	函数描述:   获取录制音频采样率
	参数1：     [out] e_SampleRate& samplerate， 目标音频采样率（默认为44.1kHZ）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT GetAudioSampleRate(e_SampleRate& samplerate) = 0;

/******************************************************************
	函数名:     SetAudioChannel
	函数描述:   设置录制音频声道数
	参数1：     [in] int channel， 目标音频声道数（默认为2声道）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT SetAudioChannel(int channel) = 0;

/******************************************************************
	函数名:     GetAudioChannel
	函数描述:   获取录制音频声道数
	参数1：     [out] int channel， 目标音频声道数（默认为2声道）
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT GetAudioChannel(int& channel) = 0;

/******************************************************************
	函数名:     SetAudioVolume
	函数描述:   设置音量放大
	参数1：     [in] long AudioVolume，//音量放大，小于100表示音量减小，大于100表示音量放大，按音量/100的比例进行
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT SetAudioVolume(long AudioVolume) = 0;

/******************************************************************
	函数名:     GetAudioVolume
	函数描述:   获取音量放大
	参数1：     [out] long& AudioVolume，//音量放大，小于100表示音量减小，大于100表示音量放大，按音量/100的比例进行
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT GetAudioVolume(long& AudioVolume) = 0;


/******************************************************************
	函数名:     SetFileContainer                                
	函数描述:   设置录制视频容器格式
	参数1：     [in] e_RECORD_CONTAINER container, 目标文件容器类型
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT SetDstFileContainer(e_RECORD_CONTAINER container) = 0;

/******************************************************************
	函数名:     GetDstFileContainer                                
	函数描述:   获取录制视频容器格式
	参数1：     [out] e_RECORD_CONTAINER& container, 目标文件容器类型
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT GetDstFileContainer(e_RECORD_CONTAINER& container) = 0;

/******************************************************************
	函数名:     SetDstFilePath                                
	函数描述:   设置录制视频保存路径及名称
	参数1：     [in] wchar_t* pPath, 目标文件保存路径
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT SetDstFilePath(wchar_t* pPath) = 0;

/******************************************************************
	函数名:     GetDstFilePath                                
	函数描述:   获取录制视频保存路径及名称
	参数1：     [in] wchar_t* pPath, 目标文件保存路径
	参数2：     [in,out] int& len, pPath的buffer长度，如小于当前路径buffer的长度，则返回当前路径长度而不返回当前路径(pPath)
	返回值：    成功返回S_OK，否则为失败
	备注：      pPath 内存长度不够返回S_FALSE，并返回当前路径内存大小
*******************************************************************/

	virtual HRESULT GetDstFilePath(wchar_t* pPath, int& len) = 0;

/******************************************************************
	函数名:     StartRecord
	函数描述:   开始录制视频
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT StartRecord() = 0;

/******************************************************************
	函数名:     PauseRecord
	函数描述:   暂停录制视频
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT PauseRecord() = 0;

/******************************************************************
	函数名:     StopRecord
	函数描述:    停止录制视频
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT StopRecord() = 0;

/******************************************************************
	函数名:     GetFirstBitmapImage
	函数描述:   获取第一帧视频图像
	参数1:      PBYTE *ppbuf，输出，BITMAPINFOHEADER+位图数据
	返回值：    成功返回S_OK，否则为失败
	备注：		该函数在开始录制之后才能调用，且内存由视频录制模块管理，上层使用完后不需要释放
*******************************************************************/

	virtual HRESULT GetFirstBitmapImage(BYTE** pData) = 0;

/******************************************************************
	函数名:     SetCallBack
	函数描述:   设置消息回调接口
	参数1:      [in]interface IQvodCallBack* pcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT SetCallBack(struct IQvodCallBack* pcallback) = 0;

/******************************************************************
	函数名:     GetCallBack
	函数描述:   设置消息回调接口
	参数1:      [out]interface IQvodCallBack** ppcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
	*******************************************************************/

	virtual HRESULT GetCallBack(struct IQvodCallBack** ppcallback) = 0;

/******************************************************************
	函数名:     VideoResizeNotify
	函数描述:   视频窗口大小调整通知
	参数1:      [in]RECT* prect，视频窗口大小调整通知
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT VideoResizeNotify(RECT* prect) = 0;

/******************************************************************
	函数名:     SetTailLogoPath
	函数描述:   设置视频尾部logo
	参数1:      [in,out]BITMAPINFOHEADER* pbih，24位或32位的bmp位图头结构体
	参数2:		[in,out]BYTE* pBmpData,位图数据
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT SetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData) = 0;

/******************************************************************
	函数名:     SetTailLogoPath
	函数描述:   获取视频尾部logo
	参数1:      [in,out]BITMAPINFOHEADER* pbih，24位或32位的bmp位图头结构体
	参数2:		[in,out]BYTE* pBmpData,位图数据
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT GetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData) = 0;

/******************************************************************
	函数名:     DestroyInstance
	函数描述:   销毁实例
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual void DestroyInstance() = 0;
};
