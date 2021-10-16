#pragma once

//视频录制参数结构体
struct Record_Param
{
	HWND			hwnd;
	RECT			WinRect;
	int				nWidth;
	int				nHeigh;
	double			dframerate;
	long			AudioVolume;//音量放大，小于100表示音量减小，大于100表示音量放大，按音量/100的比例进行
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
	函数名:     CreateRecordTask
	函数描述:   创建视频录制任务
	参数1:		Record_Param* param, 视频录制参数配置，可为空，如果为空则使用默认值
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/
	virtual HRESULT CreateRecordTask(Record_Param* param) = 0;

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
	备注：		该函数在开始录制之后才能调用
*******************************************************************/

	virtual HRESULT GetFirstBitmapImage(BYTE** pData) = 0;

/******************************************************************
	函数名:     SetCallBack
	函数描述:   设置消息回调接口
	参数1:      [in]interface IQvodCallBack* pcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT SetCallBack(interface IQvodCallBack* pcallback) = 0;

/******************************************************************
	函数名:     GetCallBack
	函数描述:   设置消息回调接口
	参数1:      [out]interface IQvodCallBack** ppcallback，回调消息接口
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT GetCallBack(interface IQvodCallBack** ppcallback) = 0;

/******************************************************************
	函数名:     VideoResizeNotify
	函数描述:   视频窗口大小调整通知
	参数1:      [in]RECT* prect，视频窗口大小调整通知
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT VideoResizeNotify(RECT* prect) = 0;

/******************************************************************
	函数名:     SetTailLogo
	函数描述:   设置视频尾部logo
	参数1:      [in,out]BITMAPINFOHEADER* pbih，24位或32位的bmp位图头结构体
	参数2:		[in,out]BYTE* pBmpData,位图数据
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	//virtual HRESULT SetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData) = 0;

/******************************************************************
	函数名:     SetTailLogo
	函数描述:   获取视频尾部logo
	参数1:      [in,out]BITMAPINFOHEADER* pbih，24位或32位的bmp位图头结构体
	参数2:		[in,out]BYTE* pBmpData,位图数据
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	//virtual HRESULT GetTailLogo(BITMAPINFOHEADER* pbih, BYTE* pBmpData) = 0;
};