#pragma once
#include "GraphBaseBuilder.h"
#include "GraphStructure.h"
#include "QVodFilterManager.h"
#include "QMediaInfo.h"
#include "..\include\IKeyFrameInfo.h"

extern HINSTANCE	g_hInstance;//dll实例句柄

class CGraphBuilder :
	public CGraphBaseBuilder
{
public:
	CGraphBuilder(void);
public:
	~CGraphBuilder(void);

public:
/*********************************************************************************************
	函数名:     ResetFlags                                
	函数描述:   重置所有标识      
	返回值：    无
*********************************************************************************************/
	virtual void ResetFlags();

/*********************************************************************************************
	函数名:     ResetInterface                                
	函数描述:   重置所有接口      
	返回值：    无
*********************************************************************************************/

	virtual void ResetInterface();

/*********************************************************************************************
	函数名:     ResetFilters                                
	函数描述:   重置所有filter      
	返回值：    无
*********************************************************************************************/
	virtual void ResetFilters();

/*********************************************************************************************
	函数名:     DestroyGraph                                
	函数描述:   销毁链路     
	返回值：    无
*********************************************************************************************/
	virtual void DestroyGraph();

/*********************************************************************************************
	函数名:     GetPreFilterOnPath
	函数描述:   获取指定filter类型的前一个filter
	参数1:		e_FilterType t，filter类型
	参数2:		int* idx_audio_decoder，filter类型
	返回值：    无
*********************************************************************************************/
CComQIPtr<IBaseFilter>	GetPreFilterOnPath(e_FilterType t, int* idx_audio_decoder = NULL);

/*********************************************************************************************
	函数名:     RepalcePath                                
	函数描述:   重定位dll路径
	参数1:		CString& path，路径
	返回值：    无
*********************************************************************************************/
	void	RepalcePath(CString& path);

/*********************************************************************************************
	函数名:     PrepareFilter                                
	函数描述:   重定位Filter及所依赖的dll路径
	参数1:		QVOD_FILTER_INFO& filter_info，filter信息结构体
	返回值：    无
*********************************************************************************************/
	void	PrepareFilter(QVOD_FILTER_INFO& filter_info);

/*********************************************************************************************
	函数名:     LoadComponents                                
	函数描述:   加载组件
	参数1:		QVOD_FILTER_INFO& filter_info，filter信息结构体
	参数2:		std::vector<HMODULE>&  modules，文件句柄列表引用
	返回值：    成功返回true，否则为false
*********************************************************************************************/
	bool	LoadComponents(QVOD_FILTER_INFO& filter_info, std::vector<HMODULE>&  modules);

/*********************************************************************************************
	函数名:     LoadFilter                                
	函数描述:   加载filter
	参数1:		const CLSID& clsid，filter clsid
	参数2:		QVOD_FILTER_INFO& filter_info，filter信息结构体
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
	CComQIPtr<IBaseFilter>		LoadFilter(const CLSID& clsid, QVOD_FILTER_INFO& filter_info);

/*********************************************************************************************
	函数名:     ReadCheckBytes                                
	函数描述:   读取checkbyte
	参数1:		BYTE* buffer，数据buffer指针
	参数2:		int size_to_read，filter信息结构体
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
bool			ReadCheckBytes(BYTE* buffer, int size_to_read);

/*********************************************************************************************
	函数名:     GetContainerByCheckBytes                                
	函数描述:   根据CheckByte获取容器类型
	参数1:		wchar_t* pPath，文件路径
	参数2:		e_CONTAINER& container，容器类型
	返回值：    成功返回S_OK，否则为失败
*********************************************************************************************/
	HRESULT			GetContainerByCheckBytes(wchar_t* pPath, e_CONTAINER& container);
	
/*********************************************************************************************
	函数名:     GetContainerBySuffix                                
	函数描述:   根据clsid根据后缀名获取容器类型
	参数1:		wchar_t* pPath，文件路径
	参数2:		e_CONTAINER& container，容器类型
	返回值：    成功返回S_OK，否则为失败
*********************************************************************************************/
	HRESULT			GetContainerBySuffix(wchar_t* pPath, e_CONTAINER& container);

/*********************************************************************************************
	函数名:     Load_ReaderFilter                                
	函数描述:   根据clsid加载file reader
	参数1:		const CLSID& clsid，filter clsid
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_ReaderFilter(CString path, const CLSID& clsid = CLSID_NULL);

/*********************************************************************************************
	函数名:     Load_SourceFilter                                
	函数描述:   根据clsid加载源解析filter
	参数1:		const CLSID& clsid，filter clsid
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_SourceFilter(CString path, const CLSID& clsid);

/*********************************************************************************************
	函数名:     Load_SplitterFilter                                
	函数描述:   根据clsid加载解析filter
	参数1:		const CLSID& clsid，filter clsid
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_SplitterFilter(const CLSID& clsid);

/*********************************************************************************************
	函数名:     Load_VideoDecoderByCLSID                                
	函数描述:   根据clsid加载视频解码filter
	参数1:		const CLSID& clsid，filter clsid
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_VideoDecoderByCLSID(CComQIPtr<IPin>& pVideoPin, const CLSID& clsid);

/*********************************************************************************************
	函数名:     Load_AudioDecoderByCLSID                                
	函数描述:   根据clsid加载音频解码filter
	参数1:		const CLSID& clsid，filter clsid
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_AudioDecoderByCLSID(const CLSID& clsid);

/*********************************************************************************************
	函数名:     Load_VideoDecoderAuto                                
	函数描述:   自动加载视频解码filter
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_VideoDecoderAuto(CComQIPtr<IPin>& pVideoPin);

/*********************************************************************************************
	函数名:     Load_AudioDecoderAuto                                
	函数描述:   自动加载音频解码filter
	返回值：    成功返回true，否则返回false
*********************************************************************************************/
	bool			Load_AudioDecoderAuto();

/*********************************************************************************************
	函数名:     Load_VideoRenderByClsid                                
	函数描述:   根据clsid加载视频渲染器
	返回值：    成功返回true，否则返回false
*********************************************************************************************/
	bool			Load_VideoRenderByClsid(/*const CLSID& clsid*/ CLSID clsid);

/*********************************************************************************************
	函数名:     ConnectAudioRenderAuto                                
	函数描述:   自动连接音频渲染器
	返回值：    成功返回true，否则返回false
*********************************************************************************************/
	bool			ConnectAudioRenderAuto();

/*********************************************************************************************
	函数名:     Load_VideoRenderAuto                                
	函数描述:   自动连接视频渲染器
	返回值：    成功返回true，否则返回false
*********************************************************************************************/
	bool			Load_VideoRenderAuto();

/*********************************************************************************************
	函数名:     Load_DVDNavigater                                
	函数描述:   加载dvd导航
	返回值：    成功返回true，否则返回false
*********************************************************************************************/
	bool			Load_DVDNavigater(wchar_t* pPath);

protected:
	void SortFilterByPriority(std::vector<CLSID>& filters, QVOD_MEDIA_TYPE& mt);

	bool CreateAudioRender();

protected:

	e_CONTAINER						m_ConType;					// 容器类型
	e_V_ENCODER						m_VideoType;				// 视频编码类型
	std::vector<e_A_ENCODER>		m_AudioTypes;				// 音频编码类型
	CComQIPtr<IDvdGraphBuilder>		m_pDvdGraph;				// DVD

	// Filters
	CComQIPtr<IBaseFilter>			m_pAudioCapture;			// 音频采集filter
	CComQIPtr<IBaseFilter>			m_pVideoCapture;			// 视频采集filter
	CComQIPtr<IBaseFilter>			m_pFilterFileReader;		// SYS FileReader/XLMV Reader/...
	CComQIPtr<IBaseFilter>			m_pFilterSource;			// 本地播放的Source
	CComQIPtr<IBaseFilter>			m_pFilterSplitter;			// 本地、网络播放的Splitter
	CComQIPtr<IBaseFilter>			m_pFilterVideoDecoder;		// 选用的Video Decoder Filter
	CComQIPtr<IBaseFilter>			m_pFilterVideoRender;		// 视频Render
	CComQIPtr<IBaseFilter>			m_pFilterAudioRender;		// 音频Render
	CComQIPtr<IBaseFilter>			m_pFilterVobSubTitle;		// 字幕解析器
	CComQIPtr<IBaseFilter>			m_pFilterQvodSubTitle;		// QVOD SubTitle
	CComQIPtr<IBaseFilter>			m_pFilterQvodPostVideo;		// QVOD PostVideo
	CComQIPtr<IBaseFilter>			m_pFilterQvodSound;			// QVOD Sound filter
	CComQIPtr<IBaseFilter>			m_pFilterAudioSwitch;		// 音频流切换器
	CComQIPtr<IBaseFilter>			m_pFilterColorSpaceTrans;	// Color Space trans
	CComQIPtr<IBaseFilter>			m_pFilterSubTitle;			// Color Space trans 2 （POST VIDEO->Render间）
	CComQIPtr<IBaseFilter>			m_pFilterVideoEncoder;		//视频编码器
	CComQIPtr<IBaseFilter>			m_pFilterAudioEncoder;		//音频编码器
	CComQIPtr<IBaseFilter>			m_pFilterMuxer;				//muxer filter
	CComQIPtr<IBaseFilter>			m_pFilterSink;				//Sink filter
	std::vector<CComQIPtr<IBaseFilter>>	
									m_pFilterAudioDecoders;			// 选用的Audio Decoder Filter


	// DVD
	CComQIPtr<IDvdControl2>			m_pDvdControl;
	CComQIPtr<IDvdInfo2>			m_pIDvdInfo2;

	// 媒体控制
	CComQIPtr<IMediaEvent>			m_pIMediaEvent;					//事件通知接口
	CComQIPtr<IKeyFrameInfo>		m_pKeyFrameInfo;				//关键帧信息接口
	CComPtr<IKeyFrameInfoEx>		m_pKeyFrameInfoEx;				//关键帧扩展信息接口


	// filter -> dlls
	std::map<IBaseFilter*, std::vector<HMODULE> > 
									m_Module;						// 已加载的Dll


	QVOD_MEDIA_TYPE					m_MTToVRender;

	int								m_CurVideoBitCout;				//视频位深

	CComQIPtr<IPin>					m_CurVideoOutputPin;//选用的最高码率视频流（PIN INDEX)

	CString							m_SoundDevice;				//音频输出设备
	CString							m_DllPath;					//dll文件路径
	CString							m_ExePath;					//可执行文件路径
	HMODULE							m_D3DX9_32HMD;
	HMODULE							m_EVRPresenterHMD;

	// 缺少的Filter
	std::map<CString, QVOD_FILTER_INFO>	m_LossFilters;

	// 视频码率
	int								m_FileBitrate;					//文件码率
	int								m_VideoBitrate;					//视频码率	
	int								m_VideoWidth;					//视频宽度
	int								m_VideoHeight;					//视频高度

};
