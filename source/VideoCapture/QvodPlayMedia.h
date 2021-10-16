#pragma once
/*******************************************************************************
  Copyright (C), 1988-1999, QVOD Technology Co.,Ltd.
  File name:      QvodPlayMedia.h
  Author:       kiven, zwu
  Version:      4.0.5
  Date:2010-3-20 // 作者、版本及完成日期
  Description:   本类实现网络、本地媒体及DVD的回放及播放时的调节控制功能
  Others:        
  Function List:  play(), pause(), stop()等
  History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
    1. Date:
       Author:
       Modification:
********************************************************************************/
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 QVODPLAYMEDIA_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// QVODPLAYMEDIA_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef QVODPLAYMEDIA_EXPORTS
#define QVODPLAYMEDIA_API __declspec(dllexport)
#else
#define QVODPLAYMEDIA_API __declspec(dllimport)
#endif
#include <Il21dec.h>
#include <olectl.h>
#include <ddraw.h>
#include <mmsystem.h>
#include "..\include\IQvodPlayMedia.h"
#include "..\include\IQvodNet.h"
#include "..\include\IKeyFrameInfo.h"
#include "../include/IEVRPresenterControl.h"
#include <vector>
#import "..\..\bin\Flash10b.ocx" rename_namespace("FlashLib1")
#include "..\share\rmoc3260.tlh"
#include "..\share\wmp.tlh"
#include "QvodStructure.h"
#include "..\include\IMediaPost.h"
#include "Guid.h"
#include <d3d9.h>
#include <vmr9.h>
#include <evr.h>
#include <Evr9.h>
#include "Dshowasf.h"
#include "MediaInfo.h"
#include "Msdrmdefs.h"//DRM
#include "InterfaceExtend.h"

#include "..\tools\Log.h"

//#include <atlstr.h>
#define _WININET_
#define INTERNET_MAX_URL_LENGTH 512
#include "Shlobj.h"


#define  PREREADDATA     10240 * 10

//调试宏定义
//#define SHOW_GRAPH_IN_GRAPHEDIT
//#define SHOW_DEBUG_INFO
//#define QVOD_MEDIA_LOG
//#define NEW_POST_INTERFACE
//#define VIDEO_ZOOM 
//#define DRM
//#define BUILD_GRAPH_BY_XML
//模块初始化和销毁接口
extern "C" QVODPLAYMEDIA_API long	_InitQvodMedia(void **ppvoid);//
extern "C" QVODPLAYMEDIA_API void	_ClearQvodMedia(long nid);
class CQvodPlayMedia;
class QvodRenderCtrl;
typedef IDispEventImpl<1, CQvodPlayMedia, &DIID_IREALPLAYEvents,&LIBID_REALPLAYLib,1,0> CRealEventSink;
typedef IDispEventImpl<2, CQvodPlayMedia, &DIID_IMEDIAPLAYEvents,&LIBID_MEDIAPLAYLib,1,0> CMediaEventSink;
#define CoreAVC
class CQvodPlayMedia : public IQvodPlayMedia,/* IAMWMBufferPassCallback,*/ IVideoPost, ISubtitle, ISoundPost
{
	friend class QvodRenderCtrl;
public:
	CQvodPlayMedia();
	~CQvodPlayMedia();
public:
//播放文件源
#ifdef ADD_SUBTITLE_BY_BUILDGRAPH
	QVOD_ERROR PlaySource(wchar_t *pFileUrl, wchar_t* pSubPath = NULL, E_MEDIA_TYPE mt = E_MT_VIDEO, LONGLONG lStart = 0);
#else
	QVOD_ERROR PlaySource(wchar_t *pFileUrl,/* wchar_t* pSubPath = NULL,*/ E_MEDIA_TYPE mt = E_MT_VIDEO, LONGLONG lStart = 0);
#endif
	//播放广告
	QVOD_ERROR PlayPauseAd(bool bIsPlayAd, wchar_t *pFileUrl = NULL);
	QVOD_ERROR PlayLocalAd(bool bIsPlayAd, wchar_t *pAdUrl = NULL);
	QVOD_ERROR PlayAd(wchar_t *pAdUrl, bool bfixsize, RECT *prect);
	QVOD_ERROR StopAd();
	void SetPauseAdEnable(bool bAdEnable);
	//播放
	QVOD_ERROR Play();
	//停止
	QVOD_ERROR Stop();
	//暂停
	QVOD_ERROR Pause();
	//销毁Graph
	void DestroyGraph();
	//获取播放状态
	QVOD_ERROR GetPlayState(e_PlayState *pplaystate);
	//获取当前文件读取位置
	QVOD_ERROR GetCurrentPos(REFERENCE_TIME *pvalue);
	//设置当前文件读取位置
	QVOD_ERROR SetCurrentPos(REFERENCE_TIME  lvalue);
	//当前媒体是否可拖拽
	bool CurrentMediaCanSeek();
	//识别媒体文件类型
	e_CONTAINER GetMediaFileType(wchar_t *pFileUrl);
	//获取播放时长
	QVOD_ERROR GetDuration(REFERENCE_TIME *pvalue);
	//获取全屏标识
	QVOD_ERROR GetFull(bool *pvalue);
	//设置全屏标识
	QVOD_ERROR SetFull(bool bvalue) ;
	//获取静音标识
	QVOD_ERROR GetMute(bool *pvalue);
	//设置静音标识
	QVOD_ERROR SetMute(bool bvalue);
	//获取音量
	QVOD_ERROR GetVolume(long *pvalue);
	//设置音量
	QVOD_ERROR SetVolume(long lvalue);
	//void SetFlashVolume(UINT lvalue);
	//设置播放速率
	QVOD_ERROR SetPlaySpeed(double lSpeed);
	//获取播放速率
	QVOD_ERROR GetPlaySpeed(double *plSpeed);
	//获取视频渲染的总帧数
	QVOD_ERROR GetVideoFrameNum(int *pframenum);
	//获取视频框架
	QVOD_ERROR GetViewFrame(e_Viewframe *pviewframe, e_VFULLFRAME *pFFrame);
	//设置视频框架
	QVOD_ERROR SetViewFrame(e_Viewframe eViewframe, e_VFULLFRAME eFFrame);
	//区域放大
	QVOD_ERROR SetVideoZoom(POINT pt, float zoomparam);
	//移动放大区域
	QVOD_ERROR SetZoomMove(int x, int y);
	//动态加载视频调节filter
	QVOD_ERROR DynamicAddVideoPostFilter(bool bAddsub = true, bool bAddPostVideo = true);
	//获取声道
	QVOD_ERROR GetSoundChannel(e_SoundChannel *pSoundChannel);
	//设置声道
	QVOD_ERROR SetSoundChannel(e_SoundChannel eSoundChannel);
	//设置视频Render
	QVOD_ERROR SetVideoRender(e_VideoRender eVideoRender);
	//获取视频Render
	QVOD_ERROR GetVideoRender(e_VideoRender *pVideoRender);
	//获取音频Render数
	QVOD_ERROR GetAudioRenderCount(long *pAudioCount, long *pbufsize, wchar_t *pAudioBuf);
	//获取音频Render
	QVOD_ERROR GetCurAudioRender(long *pAudioRender);
	//设置音频Render
	QVOD_ERROR SetCurAudioRender(long lAudioOrder);
	//获取截图尺寸
	QVOD_ERROR GetClipSize(long *pwidht, long *pheight);
	//获取文件播放码率
	QVOD_ERROR GetRate(long *prate);
	//截取当前图像
	QVOD_ERROR GetCurrentImage(PBYTE *ppbuf,  e_CAPTUREMODE eCaptrueMode = _E_CM_CURRENT, REFERENCE_TIME* psampletime = NULL);
	//获取缓冲百分率
	QVOD_ERROR GetBufferPer(long *pbufferper);
	//获取指定路径文件的文件信息
	QVOD_ERROR GetFileInfo(wchar_t *pFileUrl, s_FileInfo *ps_fileinfo, E_MEDIA_TYPE& mt);
	//获取媒体文件的mediainfo
	QVOD_ERROR GetMediaInfo(wchar_t *pFileUrl, struct Qvod_DetailMediaInfo	*ps_MediaInfo);
	//获取媒体文件的mediainfo
	//QVOD_ERROR GetLabelInfo(wchar_t *pFileUrl, struct Qvod_Label *pLabelInfo);
	//获取文件类型
	QVOD_ERROR GetFileType(e_FileType *pfiletype);
	//获取文件协议
	QVOD_ERROR GetFileProtocol(e_FileProtocol *pfileprotocol);
	//设置父窗口句柄
	QVOD_ERROR SetParentWnd(long lparentwnd);		//HWND
	//设置网络模块接口指针
	QVOD_ERROR SetQvodNet(void **pIQvodNet);
	//是否启动硬件加速
	void IsOpenDxva(bool isopen);
	//设置播放选项（启动硬件加速、启动视频调节、不启动硬件加速和视频调节）
	HRESULT SetPlayOption(e_Play_Option eOption, PlayOption_Param* pParam = NULL);
	//获取播放选项设置
	void GetPlayOption(e_Play_Option &eOption, PlayOption_Param* pParam = NULL);
	//设置视频增强
	//QVOD_ERROR SetVideoEnhance(bool IsEnhance);
	////获取视频增强状态
	//QVOD_ERROR GetVideoEnhance(bool &IsEnhance);
	//设置回调函数
	QVOD_ERROR SetCallBack(IQvodCallBack *pQvodCallBack);
	//移动视频窗口
	QVOD_ERROR MoveWindow(CRect *prect = NULL);
	//重绘视频窗口
	QVOD_ERROR RepaintVideo(CRect *prect = NULL);
    //获取流信息
	QVOD_ERROR GetStreamInfo(S_StreamInfo *pStreamInfo);	
	//设置播放时的数据缓冲时间
	QVOD_ERROR SetBufferTime(int nTime);
	//枚举Filter
	QVOD_ERROR EnumFilters(_pfnUserFilter fnUserFileter, void *pvoid);
	QVOD_ERROR ShowFilter(wchar_t *pfiltername);

#ifndef BUILD_GRAPH_BY_XML
	//获取当前下载长度，已屏蔽
	QVOD_ERROR GetFileDownloadLen(INT64& iDownLoadLen);
	//查找graph中的Render Fitler
	QVOD_ERROR FindRender();
	//Add NULL Render
	QVOD_ERROR ConnectNULLRender(IPin *pPin);
	//QVOD_ERROR rotate(int x);
	QVOD_ERROR flip(E_FlipType eFlipType);
#endif
	////音视频调节设置接口
	//QVOD_ERROR SetAddMediaFilter(bool isadd);
	////获取是否添加音频频调节标识
	//QVOD_ERROR GetAddMediaFilter(bool* pisadd);
	//QVOD_ERROR SetVideoZoom(POINT pt, float zoomparam);
	QVOD_ERROR MoveZoom(POINT pt);
#ifdef VIDEO_ZOOM
	//
	QVOD_ERROR MoveVideoRect(POINT pt);
#endif

	/*STDMETHODIMP_(ULONG) AddRef() {
	return S_OK;
	};
	STDMETHODIMP_(ULONG) Release() {
	return S_OK;                          
	};*/
//NOTIFY
	long EventNotify_(UINT uMsg, long wparam, long lparam);

	QVOD_ERROR ProductVersion(long version); //0, sample（简易）, 1,enhance(增强版)
	//获取文件媒体类型(音频、视频，swf，csf，其他)
	QVOD_ERROR GetCurrentMediaType(E_MEDIA_TYPE& mt);
	//获取帧率
	QVOD_ERROR GetCurrentavgFramerate(int* pavgframerate);
	//获取视频编码类型
	QVOD_ERROR GetCurrenVideoCodecType(e_V_ENCODER *pvideocodectype);

	//设置字幕
	QVOD_ERROR SetSubTitle(int iIndex);
#ifdef VMR_EVR_PRESENT_TEST
	//设置双字幕 
	QVOD_ERROR SetMultiSub(int majorsub, int minorsub);
#endif
	//显示隐藏字幕
	QVOD_ERROR ShowSubTitle(bool showflag);
	//设置声道
	QVOD_ERROR SetAudioChannel(int iIndex);

	//帧操作：上一帧，下一帧
	QVOD_ERROR SetPlayFrame(long lFrameNums, bool bsync = false);
	//接口查询
	QVOD_ERROR __stdcall QueryInterface(REFIID riid, void **ppVIF);
    //水平移动窗口
	QVOD_ERROR Move(int x, int y );
    //放大，缩小
	QVOD_ERROR Zoom(int iParmer);
    //调整
	QVOD_ERROR Scale(int x);   
	//QVOD_ERROR SelectScale(e_Viewframe eViewframe);
    //还原视频窗口
	QVOD_ERROR Restore();
	
    //桌面播放
	QVOD_ERROR DestopPlay(bool bDestopplay);
	//获得上一次桌面播放标志
	QVOD_ERROR GetLastDestopPlay(bool& bflag);
	//设置桌面播放（注册表操作）
	QVOD_ERROR SetDestopRecover(bool bflag);

	//分配色键
	void AllocateColokey(DWORD dwColorkey);

	//是否应该打开画质增强
	BOOL ShallVideoEnhanceOpen();

                            
	//设置播放缓冲数据长度
	QVOD_ERROR SetBufferingTime(int nPreBufferTime, int nPlayBufferTime);
                            
	//获取播放缓冲数据长度
	void GetBufferingTime(int* pnPreBufferTime, int* pnPlayBufferTime);

#ifdef PROVIDE_CRASH_INFO
	void GetCrashLog(BYTE** ppbyte, int& len);
#endif

	//检测3D视频
	void Detect3DVideo();
#ifdef DETECT_3D_VIDEO
	bool IsSuitableFor3D();
#endif

	//DVD菜单中鼠标MOVE事件响应
	QVOD_ERROR DvdSelectAtPosition(POINT Pt);
	//DVD菜单中鼠标点击事件响应
	QVOD_ERROR DvdActivateAtPosition(POINT Pt);
	//获取DVD的标题数
	QVOD_ERROR	DvdGetTitles(ULONG *pTitleNum = NULL, int *pCurTitle = NULL);
	//设置标题
	QVOD_ERROR DvdSetCurtitle(int nCurtitle, BOOL bEvent);
	//获取指定DVD标题的章节数
	QVOD_ERROR	DvdGetChapters(ULONG uTitle, ULONG *pChapterNum = NULL, int *pCurChapter = NULL);
	//设置DVD播放的章节号
	QVOD_ERROR DvdSetCurChapter(int uCurchapters, BOOL bEvent);
	//获取视角信息
	ULONG DvdGetAngle(ULONG* pAngNum = NULL, int* pCurAngle = NULL);
	//设置当前视角
	QVOD_ERROR DvdSetAngle(int nCurangle, BOOL bEvent);
	//获取音频流数
	ULONG DvdGetAudios();
	//获取音频信息
	QVOD_ERROR DvdGetAudios(S_DVDINFO* pAudio);
	//设置当前音频
	QVOD_ERROR	DvdSetAudio(int nCuraudio, BOOL bEvent);
	//获取字幕信息
	QVOD_ERROR DvdGetSubTitle(S_DVDINFO* pSubTitle = NULL);
	//设置当前字幕
	QVOD_ERROR DvdSetSubTitle(int nCurWords, BOOL bEvent);
	//上一章
	QVOD_ERROR PlayPrevChapter();
	//下一章
	QVOD_ERROR PlayNextChapter();
	//设置DVD菜单
	QVOD_ERROR DvdSetMenu(DVD_MENU_ID eMenu);
#ifdef INTEL_CUSTOM_DXVA_VERSION
	//重置监视器
	QVOD_ERROR ResetMonitor();

	//视频墙是否可用
	bool IsVideoWallAvailable();

	//设置视频墙模式
	QVOD_ERROR SetVideoWallMode(bool flag);

	//获取视频墙模式
	QVOD_ERROR GetVideoWallMode(bool* flag);
#endif
#ifdef NEW_POST_INTERFACE
	//PostVideo接口

	//视频增强
#ifdef VIDEO_ENHANCE_WIPER
	QVOD_ERROR SetVideoEnhance(bool enable,  OPTIONENHANCE effect);
#else
	QVOD_ERROR SetVideoEnhance(bool enable);
#endif
	QVOD_ERROR GetVideoEnhance(bool &enable);

	//去噪
	QVOD_ERROR ReduceNoise(bool enable,		//开启或关闭
		int ReduceNum);//降噪数量（范围20-150,35）
	QVOD_ERROR GetReduceNoiseFlag(bool &enable,
		int &ReduceNum);

	//GAMMA校正
	QVOD_ERROR Gamma(bool enable,		//开启或关闭
		int nGamma);//降噪数量（范围20-150,35）
	QVOD_ERROR GetGammaFlag(bool &enable, 
		int &nGamma);

	//锐化
	QVOD_ERROR Sharpen(bool enable,			//开启或关闭
		int nSharpenCount,	//锐化数量(范围0-100，默认值50)
		int nSharpenThreshold);//锐化阈值（范围0-20，默认值5）
	QVOD_ERROR GetSharpenFlag(bool &enable, //开启或关闭
		int &nSharpenCount, //开启或关闭
		int &nSharpenThreshold);

	//反交错
	QVOD_ERROR Deinterlace(DEINTERLACE_MODE Dtlmode,//反交错模式，见DEINTERLACE_MODE定义
		int Dtlparam);//反交错参数BOB模式（范围0-5，默认值0），TomsMoComp模式（范围0-25，默认值3）
	QVOD_ERROR GetDeinterlaceFlag(DEINTERLACE_MODE &Dtlmode,
		int &Dtlparam);

	//亮度
	QVOD_ERROR SetBrightness(int nBrightness);//范围-127-127,默认值0
	QVOD_ERROR GetBrightness(int &nBrightness);

	//饱和度
	QVOD_ERROR SetSaturation(int nSaturation);//范围-127-127,默认值0
	QVOD_ERROR GetSaturation(int &nSaturation);

	//对比度
	QVOD_ERROR SetContrast(int nContrast);//范围-127-127,默认值0
	QVOD_ERROR GetContrast(int &nContrast);

	//色调
	QVOD_ERROR SetHue(int nHue);//范围0-616,默认值0
	QVOD_ERROR GetHue(int &nHue);

	//旋转
	QVOD_ERROR Flip(int nAngel);//范围90的倍数,默认值0
	QVOD_ERROR GetFlip(int &nAngel);

	//水平翻转
	QVOD_ERROR HorizontalReverse(BOOL bHorizontal);
	QVOD_ERROR GetHorizontalReverse(BOOL &bHorizontal);

	//垂直翻转
	QVOD_ERROR VerticalReverse(BOOL bVertical);
	QVOD_ERROR GetVerticalReverse(BOOL &bVertical);

	QVOD_ERROR Set3DView(OPTION3DVIEW opt3DView, float kView);//OPTION3DVIEW opt3DView:左右眼位置，float kView：视差系数，详见OPTION3DVIEW注释
	QVOD_ERROR Get3DView(OPTION3DVIEW* opt3DView, float* kView);

	//截图
	QVOD_ERROR GetPostVideoImage(int WaitTime, BYTE** ppByte, int *pnBuffersize);

	//ISubtitle

	//设置（获取）字幕路径
#ifdef USE_D3D_PIXEL_SHADER
	QVOD_ERROR PutSubPath(WCHAR* wszPath/*, int& SubIndex*/);
	QVOD_ERROR GetSubPath(/*int SubIndex, */WCHAR* wszPath, int &buffersize);

	//显示（隐藏）字幕
	QVOD_ERROR PutSubVisableFlag(bool bVisable, MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetSubVisableFlag(bool &bVisable,  MULTIP_SUB msub = MAJOR_SUB);

	//清除字幕
	QVOD_ERROR ClearSubtitle();

	//设置（获取）字幕字体
	QVOD_ERROR PutSubFont(LOGFONT sf,  MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetSubFont(LOGFONT &sf,  MULTIP_SUB msub = MAJOR_SUB);

	//设置（获取）字体颜色
	QVOD_ERROR PutFontColor(DWORD fcolor, MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetFontColor(DWORD &fcolor, MULTIP_SUB msub = MAJOR_SUB);

	//设置（获取）字体轮廓颜色
	QVOD_ERROR PutOutLineColor(DWORD fcolor,  MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetOutlineColor(DWORD &fcolor,  MULTIP_SUB msub = MAJOR_SUB);

	//设置（获取）字幕位置
	QVOD_ERROR PutSubPlace(ESubPlace place, int cx, int cy,  MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetSubPlace(ESubPlace &place, int &cx, int &cy,  MULTIP_SUB msub = MAJOR_SUB);

	//设置（获取）字幕延迟(超前)
	QVOD_ERROR PutSubDelay(int delaytime,  MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetSubDelay(int &delaytime,  MULTIP_SUB msub = MAJOR_SUB);

	//显示当前（剩余、系统）时间
	QVOD_ERROR PutSubTime(SubTimeType subTimeType);
	QVOD_ERROR GetSubTime(SubTimeType &subTimeType);

	//设置（获取）字幕延迟
	QVOD_ERROR PutTimeFont(LOGFONT lf);
	QVOD_ERROR GetTimeFont(LOGFONT &lf);
#else
	QVOD_ERROR PutSubPath(WCHAR* wszPath);
	QVOD_ERROR GetSubPath(WCHAR* wszPath, int &buffersize);

	//显示（隐藏）字幕
	QVOD_ERROR PutSubVisableFlag(bool bVisable);
	QVOD_ERROR GetSubVisableFlag(bool &bVisable);

	//清除字幕
	QVOD_ERROR ClearSubtitle();

	//设置（获取）字幕字体
	QVOD_ERROR PutSubFont(LOGFONT sf);
	QVOD_ERROR GetSubFont(LOGFONT &sf);

	//设置（获取）字体颜色
	QVOD_ERROR PutFontColor(DWORD fcolor);
	QVOD_ERROR GetFontColor(DWORD &fcolor);

	//设置（获取）字体轮廓颜色
	QVOD_ERROR PutOutLineColor(DWORD fcolor);
	QVOD_ERROR GetOutlineColor(DWORD &fcolor);

	//设置（获取）字幕位置
	QVOD_ERROR PutSubPlace(ESubPlace place, int cx, int cy);
	QVOD_ERROR GetSubPlace(ESubPlace &place, int &cx, int &cy);

	//设置（获取）字幕延迟(超前)
	QVOD_ERROR PutSubDelay(int delaytime);
	QVOD_ERROR GetSubDelay(int &delaytime);

	//显示当前（剩余、系统）时间
	QVOD_ERROR PutSubTime(SubTimeType subTimeType);
	QVOD_ERROR GetSubTime(SubTimeType &subTimeType);

	//设置（获取）字幕延迟
	QVOD_ERROR PutTimeFont(LOGFONT lf);
	QVOD_ERROR GetTimeFont(LOGFONT &lf);
#endif
	//设置播放时长
	QVOD_ERROR SetDuration(__int64 ltime);

	//ISound
	//设置（获取）音频声道
	QVOD_ERROR PutChannel(Channel channel);
	QVOD_ERROR GetChannel(Channel &channel);

	//设置（获取）静音标识
	QVOD_ERROR PutMute(bool bmute);
	QVOD_ERROR GetMute(bool &bmute);

	//设置（获取）声音延迟
	QVOD_ERROR PutSoundDelay(LONGLONG delaytime);//单位ms
	QVOD_ERROR GetSoundDelay(LONGLONG &delaytime);

	//设置（获取）声音延迟
	QVOD_ERROR PutSoundEnlarge(long lenlarge);
	QVOD_ERROR GetSoundEnlarge(long &lenlarge);

	//设置（获取）音效均衡
	QVOD_ERROR PutEqualization(int *plenlarge);
	QVOD_ERROR GetEqualization(int *plenlarge);

	//开启（关闭）音效均衡
	QVOD_ERROR PutEqualizationEnable(bool enable);
	QVOD_ERROR GetEqualizationEnable(bool &enable);

	//设置（获取）音效平衡
	QVOD_ERROR PutVolumeBalance(LONG balance);
	QVOD_ERROR GetVolumeBalance(LONG &balance);

#ifdef CHANNEAL_TRANSLATION
	//设置（获取）当前声道数
	QVOD_ERROR PutChannelNum(float channel);
	QVOD_ERROR GetChannelNum(float* pchannel);
#endif

#endif
protected:
	//读媒体文件
	//QVOD_ERROR ReadBit(PBYTE pbBuffer, DWORD uPosition, DWORD uTotalLen, LPDWORD puReaded);
	
	//*************************ZWY BEGIN
	void	OutPutInfo(const wchar_t *pstring, ...);
	void	OutPutInfo(const int level, const wchar_t *pstring, ...);
	CString	GetCurrentPath() const;
	bool	IsGrabFrame() const;
	bool	PrepareQvodSource(interface IQSourceCallBack* pQvodSource);
	DWORD	GetSystemVersion() const;
	void	SetVideoPostParamter(interface IPostVideoEffect *pPostVideoEffect);
	HRESULT	SetVideoRenderParamter(IBaseFilter* pVideoRender);

#ifdef PROVIDE_CRASH_INFO
	//将调试信息写入Crash buffer
	bool	WriteCrashLog(const wchar_t *pstring);
#endif
	// 建链路的线程
	static void	ThreadBuildGraph(void*);
	bool	NeedToDirectCreateGraph();//判断是用主线程建链路还是工作线程建链路
	DWORD   GetVideoBitrate();
	bool	LowVideoBitrate();
	
	HANDLE	m_hBuildGraph;
	HANDLE  m_hPlayThread;
	//**************************ZWY END
protected:
	//暂停，供内部调用
	QVOD_ERROR InnerPause(bool IsInnerPlay = true);
	QVOD_ERROR InnerStop();
	QVOD_ERROR InnerPlay(bool IsInnerPlay = true);
	//播放SWF文件
	QVOD_ERROR PlaySWF(CString str, bool bIsAd = false);
	
	//停止swf文件播放并销毁Active控件
	QVOD_ERROR StopSWF();
	//播放MMS协议的网络文件
	QVOD_ERROR PlayMMsFile(wchar_t *pFileUrl);
	//停止MMS网络文件的播放并销毁Active控件
	QVOD_ERROR StopMMSFile();
#ifdef DRM
	//DRM Active 
	QVOD_ERROR DRMActive();
#endif
	//QVOD_ERROR MapMediaType(BYTE bmt, AM_MEDIA_TYPE* pmt);
	//SWF消息处理函数
	QVOD_ERROR SwfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	//注册表选项检测
	void Play_RegOperater();
#ifdef VIDEO_ZOOM
	//窗口缩放
	QVOD_ERROR Video_Rect_Zoom(CRect *prect, float time);
#endif
	//SWF消息钩子
	static LRESULT CALLBACK HookSwfWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifdef DRM
	//DRM状态回调
	static HRESULT __stdcall StatusCallback(DRM_STATUS_MSG msg,
											HRESULT hr,VOID *pvParam,
											VOID *pvContext);
#endif
private:
	//根据指定的filter配置结构体加载相应的Filter
	bool		LoadFilter(S_FILTER *ps_filter, IBaseFilter **ppbasefilter);
	//查找指定Filter指定方向的输出pin
	bool		FindPin(S_ADDFILTER *ps_addfilter, IPin **ppin, _PinDirection ePinType);
#ifndef BUILD_GRAPH_BY_XML
	//检查本地文件的文件类型
	bool		CheckFileHeader(HANDLE	hFile, const TCHAR *pChkbytes, FILE* pFile);//byte *pbuf, int buflen,
	bool		CheckFileHeader(const TCHAR *pchkbytes, IAsyncReader* pAsyncReader);
	

	//播放ts流
	QVOD_ERROR PlayTS(wchar_t *purl = NULL);
	//add ColorFilter
	QVOD_ERROR AddColorFilter(IPin **ppColorInPin, IPin **ppPostVideoInPin);
	//加载FilterGraph
	bool        LoadFilterGraph();
	//加载与该Filter配置结构体相关的其他Filter
	void		LoadOtherDll(S_FILTER *ps_filter);
	//注册或反注册com组件
    HRESULT        RegeditDLL(S_FILTER *ps_filter, bool isreg = true);
	//反注册com组件
	void        UnRegeditDLL(bool bUnRegall);
	//加载SourceFilter
	bool		LoadSourceFilter(HANDLE hfile, S_ADDFILTER *ps_addfilter, bool *bsysread);
	//查找Filter
	bool		FindFilter(IPin *pOutPin, IBaseFilter **ppbasefilter, S_FILTER **pps_filter, int nfind);
	//连接pin
	bool		ConnPin(IPin *pOutPin, IPin	*pInPin, AM_MEDIA_TYPE *pamt = NULL);
	
	//检查网络文件的文件类型
	//bool		CheckNetFile(const TCHAR *pChkbytes);
	//将字符串值中十六进制的数字转换为ASCII值
	void		CStringtoBin(CString str, BYTE *pdata);
	//创建音频Render Filter
	HRESULT		CreateAudioFilter();
	//创建视频Render Filter
	void        CreateVideoRender();
	//枚举Fitler pin
	void		EnumFilterPin(S_ADDFILTER *ps_addfilter);
	//枚举Filter输出pin
	void		EnumFilterOutPin(S_ADDFILTER *ps_addfilter);
	//枚举filter输入pin
	void        EnumFilterInPutPin(S_ADDFILTER *ps_addfilter);
	//添加Fitler到Graph
	bool		AddFiltertoGraph(S_ADDFILTER *ps_addfilter, LPCTSTR lpfiltername = NULL);
	//给指定的文件类型添加相应的Splitter
	bool		AddSplitter(const S_ADDFILTER *ps_addfilter, e_CONTAINER e_container);
	//添加字幕Filter
	bool		AddVobSubFilter(const TCHAR *pstrfile);//??
	//add Audio switch
	bool        AddAudioSwitch();
	//播放文件
	QVOD_ERROR	PlayFile();
	//视频录制
	QVOD_ERROR RecordVideo(wchar_t *pPath, 
		E_RVQuality quality, 
		E_MEDIA_TYPE mt,
		long rtStrat, 
		long rtStop, 
		long lwidth, 
		long lheight);
	//停止录制视频
	QVOD_ERROR StopRecord(bool bContinuePlay = true);
	//硬件加速是否启动
	bool HardWaveAccelerate();
	//尝试下一个解码器
	bool TryNextDec();
	//断开下游链接
	QVOD_ERROR DisconnectLowerPin(IPin *pPin, IPin **pIn, IPin **pOut, int PinNum, int &UseNum);
	//播放Dvd
	QVOD_ERROR	PlayDvd();
#endif
	//安全删除graph
	void		GraphSafeRelease();

	void		InterfaceSafeRelease();
	//回调消息
	void		CallBackMessage(long lMessage, long wparam, long lparam);

	////视频增强
	//QVOD_ERROR VideoEnhance(bool IsEnhance);
	//播放网络文件
	QVOD_ERROR	PlayNetFile();

	//Render Graph
	QVOD_ERROR  PlayRender(BOOL bRun = TRUE);//??
	//DVD消息回调函数
	HRESULT		OnDvdNotify(LONG lMsg, LONG wParam, LONG lParam);
	//设置缓冲百分率
	//void		SetBufferPer(long lBufferper);
	QVOD_ERROR FindStreamSelect();
	
	//视频窗口获取函数
	QVOD_ERROR GetAdjustRect(long *lLeft, long *lTop, long *lWidth, long *lHeight);
	//视频窗口设置函数
	QVOD_ERROR SetAdjustRect(long  lLeft, long  lTop, long  lWidht, long lHeight);
	//获取链接的hash值
	bool GetHash(wchar_t * pFileUrl, char* pHash);
	//设置桌面播放
	QVOD_ERROR SetDestopPlay(bool bflag);
	//获取视频宽高
	QVOD_ERROR GetNativeVideoSize(LPRECT pSRect, LPRECT pDRect,SIZE* pARate);
	//设置视频位置
	QVOD_ERROR SetVideoPosition(CRect* pSRect, CRect* pDstRect);
	//恢复播放事件设置
	void SetPlayEvent();

	private:
		//Interface
		interface IColortrans *		GetIColorTrans();
		interface ISUBTITLE*			GetISubTitle();
		interface ISoundtrans*		GetISoundTrans();
		interface IPostVideoEffect*	GetIPostVideoEffect();
		interface IDvdControl2*		GetIDvdControl2();
		interface IDvdInfo2	*		GetIDvdInfo2();
		interface IEVRPresenterControl* GetIEVRPresenterControl();
#ifdef	VMR_EVR_PRESENT_TEST
		interface ISubtitleControl* GetSubtitleControl();
		interface ISubPicAllocatorPresenter* GetSubPicAllocatorPresenter();
#ifdef USE_D3D_PIXEL_SHADER
		interface IPostProcess*		GetPostProcess();
#endif
#endif
		//Filter
		IBaseFilter*        GetVideoRender();
		IBaseFilter*		GetSubTitleFilter();

		IMediaEvent*		GetIMediaEvent();
		IKeyFrameInfo *		GetIKeyFrameInfo();
		IKeyFrameInfoEx *	GetIKeyFrameInfoEx();

		//QvodSource
		interface IQSourceCallBack* GetQSourceCallBack();

		e_CONTAINER			GetConType();
		void LoadInterface();
		e_ACCELERATE_TYPE GetDxvaAccelSucc();
		bool WaitForNextFrame(long waittime, int* ptoframe = NULL);

		bool FirstFrameNotify(DWORD ms, REFERENCE_TIME startplay);
		DWORD				GetProductVersion();//0为简易版，1为增强版
#ifndef BUILD_GRAPH_BY_XML
private:
	//创建网络缓冲文件播放线程
	static	 void ProgressNet(void *pVoid);
	QVOD_ERROR GetPinMediaType(IPin *ppin, AM_MEDIA_TYPE* pamt);
	QVOD_ERROR CoreAVCReg();
	//static unsigned long WINAPI timer(void * pmedia);
#endif
private:
#ifdef SHOW_GRAPH_IN_GRAPHEDIT
	inline HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	inline void RemoveFromRot(DWORD dwRegister);
	DWORD		m_dwRegister;
#endif

private:
//基本变量
	DWORD           m_SystemVersion;
	DWORD           m_ProductVersion;
	bool            m_bPauseAdEnable;
	wchar_t			*m_pFileUrl;//文件路径
	wchar_t			*m_pSubPath;//字幕路径
	e_PlayState		m_ePlayState;//播放状态
	e_PlayState     m_eAdPlayState;
    e_PlayState     m_eOldState;
	e_PlayState     m_eInnerState;
	E_MEDIA_TYPE    m_eRecordMediaType;//录制媒体类型
	E_MEDIA_TYPE    m_ePlayMediaType;
	REFERENCE_TIME			m_nCurPos;//目前播放时间//当前文件读取位置
	//UINT			m_nDurPos;//目前播放时间
	REFERENCE_TIME            m_nDuration;
	bool			m_bFull;//全屏标识
	bool			m_bMute;//静音标识
	long			m_nVolume;//音量
	e_Viewframe		m_eViewFrame;//视频框架模式
	e_VFULLFRAME    m_eFullFrame;//全屏模式
	e_SoundChannel	m_eSoundChannel;//声道
	e_VideoRender	m_eVideoRender;//视频Render Filter
	e_FileType		m_eFileType;//文件类型
	e_FileProtocol	m_eFileProtocol;//文件协议
	e_Play_Option   m_ePlay_Option;//播放模式
	PlayOption_Param m_OptionParam;
	long			m_lAudioOrder;//声卡渲染
	long			m_lAudioRenderCount;
	HWND			m_hParentWnd;//父窗口(播放器主窗口)句柄
	HWND			m_hDestopWnd;//??
	CRect           m_Rect;//视频窗口大小
	CRect           m_SourceRect;
	CRect           m_Zoomrect;
	POINT           m_DstRectMove;
	BOOL			m_bSetNewPos;//拖曳标识
	bool            m_ZoomFlag;
	bool            m_ScaleFlag;
	bool            m_bIsAddfAudio;
	bool            m_bIsAddADec;
	bool            m_bIsAddVDec;
	bool            m_bIsAddColorFilter;
	bool            m_bIsCreateswfCtl;
	bool            m_bIsCreateMMsCtl;
	bool            m_bDvdFirstPlay;
	bool            m_bDvdMouseActive;
	bool            m_bIsDxvaOpen;
	bool            m_bVideoPlayEnable;
	bool            m_bAudioPlayEnable;
	bool            m_bInnerFlag;
	bool            m_bNoAudioRender;
	bool            m_bHDVideo;
	bool            m_bVideoPost;//m_bAddMediaFilter;//添加音视频调节filter
	bool            m_bSetSourceVideo;
	bool            m_bFrameView;
	bool			m_bDirectBuildGraph;
	//bool            m_bHD_Accelerate;
	e_ACCELERATE_TYPE m_e_Accelerate;
	int             m_Zoomparma;
	int             m_Scaleparma;
	float           m_ZoomHeight;
	float           m_fSrcRectZoom;
	int             m_nCurTitle;
	int             m_nCurChapter;
	ULONG           m_lTitleNum;
	ULONG           m_nPreBufferTime;
	ULONG           m_nBufferTime;
	INT64           m_nPreBufferLen;
	ULONG           m_nVideoNum;
	ULONG           m_nAudioNum;
	ULONG           m_nUnknowMediaNum;
	ULONG           m_nVideoBitRate;
	ULONG           m_nAudioBitRate;
	ULONG           m_nSubTitleNum;
	int             m_nflipAngle;
	double		    m_lSpeed;
	TCHAR			*m_pFileAdder;
	TCHAR           *m_pMainModuleName;
	int             m_lWidth;
	int             m_lHeight;
	//IBaseFilter     *m_pSystemClock;
	IReferenceClock *m_pReferenceClock;
	DWORD m_ComputerVolume;
	class  CReg     *m_pReg;
	CLog			m_Log;

    //swf相关变量
	CAxWindow *m_pAdWindow;
	CAxWindow *m_pAxWindow;
	IWebBrowser2 *m_pIWebBrowser;
	FlashLib1::IShockwaveFlash	*m_pShockFlash;
	WMPLib1::IWMPPlayer4		*m_pWMPlayer;
	WMPLib1::IWMPControls       *m_pWMPControl;
	WMPLib1::IWMPNetwork        *m_pWMPNetWork;
	WMPLib1 :: IWMPSettings     *m_pWMPSet;

	//WINDOWS DS变量
	IGraphBuilder			*m_pGraph;//Graph指针
	IBasicVideo				*m_pBasicVideo;
	IBasicVideo2			*m_pBasicVideo2;
	IBasicAudio				*m_pBasicAudio;
	IMediaControl			*m_pMediaControl;
	IVideoWindow			*m_pVideoWindow;
	IMFVideoMixerControl    *m_pMFVideoMixerControl;
	//IMFVideoMixerBitmap     *m_pIMFVideoMixerBitmap;
	//IMFVideoMixerControl2   *m_pIMFVideoMixerControl2;
	IMediaSeeking			*m_pMediaSeeking;//播放拖拽接口
	IMediaEventEx			*m_pMediaEvent;
	IVMRWindowlessControl	*m_pVMRWindowless;
	IVMRWindowlessControl9	*m_pVMRWindowless9;
	IMFVideoDisplayControl  *m_pMFVideoCtl;
//	interface IVideoRender            *m_pIVideoRender;
	//IMFVideoPresenter       *m_pMFVideoPresenter;
	IQualProp               *m_pQualProp;
	interface IColortrans   *m_pColorTrans;
	interface ISoundtrans   *m_pSoundTrans;
	interface ISUBTITLE     *m_pSubTrans;
	interface IDXVAChecker  *m_pDXVAChecker;

	IPostVideoEffect *m_pPostVideo;
	IKeyFrameInfo *m_pKeyFrameInfo;
    IKeyFrameInfoEx *m_pKeyFrameInfoEx;
	//IBaseFilter             *m_pCoreAVCFilter;
	IOverlay                *m_pOverlay;
    interface IActiveDesktop          *m_pActiveDesktop;
	unsigned char *m_pImage;
	int m_nImagesize;
	IAMResourceControl      *m_pIAMResourceControl;
	//AM_MEDIA_TYPE *m_pmt;
	TCHAR                   *m_pWallName;
	DWORD                   m_nSysColor;
	COLORREF				m_ColorKey;
	WALLPAPEROPT            *m_pWallPaperopt;
	bool                    m_bDeskTopPlay;
	//DVD
	IDvdGraphBuilder		*m_pDvdGraph;
	IDvdControl2			*m_pDvdControl;
	IDvdInfo2				*m_pDvdInfo;
	DVD_HMSF_TIMECODE		m_CurTimeDvd;
	IVideoFrameStep         *m_pVideoFrameStep;
	RECT					m_rectadjustframe;
	DVDTITLE                m_DvdTitle;
	//DLL
	std::vector<S_LOADDLL>		m_vs_LoadDll;//DLL vector
	std::vector<S_REGDLL>       m_v_RegDll;
	std::vector<S_ADDFILTER>	m_vs_AddFilter;//addfilter vector
	std::vector<S_ADDPIN>		m_v_InPin;//输入pinv vector
	std::vector<S_ADDPIN>		m_v_OutPin;//输出pin vector
	S_ADDFILTER					m_s_af_Video;//video Render
	S_ADDFILTER					m_s_af_Audio;//audio Render
	S_ADDFILTER					m_s_af_Color;
	S_ADDFILTER					m_s_af_VideoPost;
	S_ADDFILTER					m_s_af_ColorConverter;
	S_ADDFILTER					m_s_af_Sound;
	S_ADDFILTER                 m_s_af_Demultiplexer;
	S_ADDFILTER                 m_s_af_AsfWriter;
	S_FILTER                    *m_pLackFilter;
	REFERENCE_TIME              m_rtStartTime;
	REFERENCE_TIME              m_rtRecordStart;
	REFERENCE_TIME              m_rtRecordEnd;
	REFERENCE_TIME              m_rtStartPlay;
	CString						m_strInsertpath;
	CString						m_strAudioName;
	e_CONTAINER					m_e_Container;//文件类型
	IQvodCallBack				*m_pQvodCallBack;//回调函数指针
	interface IQSourceCallBack/*IQvodCallBack*/               *m_pQSCallBack;  //QvodSource 回调函数
	IQvodNet					*m_pQvodNet;//网络模块指针
	//IQLiveNet					*m_pQlive;//Qlive接口指针
	S_StreamInfo                *m_pStreamInfo;
	GUID                        m_gVideoSubType;
	GUID                        m_gAudioSubType;

//
//  当前播放的HASH值 -以及当前播放节目的索引值  网络文件
	char						m_strCurHash[41];//网络文件的hash值
	int							m_nCurIndex;//当前索引
	CString						m_strCurFile;//
	unsigned __int64			m_nCurFileLen;//当前文件长度
	unsigned __int64            m_DownLoadLen;//当前下载长度
	UINT						m_nCurDownRate;//当前下载速度
	unsigned __int64			m_nCurStepPos;	//当前播放的节目在几个FILEINFO中的偏移位置
	long						m_nBufPer;		//缓冲百分比
	int							m_nRebuildGraphCount;
#ifdef QVOD_MEDIA_LOG
    FILE                        *m_pLogFile;
	char                        m_LogName[256];
#endif
    //  锁
	CRITICAL_SECTION m_Lock;
//#ifdef QVOD_MEDIA_LOG
	CRITICAL_SECTION m_LogLock;
//#endif
	
	//消息函数指针
	WNDPROC					m_pSwfWindProc;
	HANDLE                  m_FileHandle;
	FILE                    *m_pFile;

	//Event
	HANDLE                      m_hDSCreateEvent; //媒体流创建事件
	HANDLE						m_hEvent_Buffer; //缓冲事件
	HANDLE						m_hStepEvent;
	HANDLE                      m_hCreateNetPlay;//创建网络播放的线程

	//int testParm;
	//ZWY
	QvodRenderCtrl*				m_pRenderCtrl;
	Qvod_DetailMediaInfo*		m_pCurMediaInfo;
	s_FileInfo*					m_pFileInfo;//影音信息

#ifdef PROVIDE_CRASH_INFO
	BYTE*						m_Logbuffer;	//崩溃日志buffer
	int							m_LogSize;		//崩溃日志大小
	int							m_LogMaxSize;	//崩溃日志最大长度
	DWORD						m_nRunTime;		//播放器开始运行时间
#endif
	//mediainfo
	CMediaInfo					*m_pMediaInfo;
#ifdef INTERFACE_PACKAGE
	CInterfaceExtend			m_cInterfaceExtend;
#endif

#ifdef	INTEL_CUSTOM_DXVA_VERSION
	bool						m_bEnterVideoWall;
#endif
	/*HMODULE						m_hMediaInfo_dll;
	GETFILEINFO				m_Get_FileInfo;
	GETSTREAMINFO			m_Get_StreamInfo;
	GETFILEDETAILINFO		m_Get_DetailInfo;
	GETFILELABEL			m_Get_FileLabel;*/
};
