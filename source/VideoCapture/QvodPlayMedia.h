#pragma once
/*******************************************************************************
  Copyright (C), 1988-1999, QVOD Technology Co.,Ltd.
  File name:      QvodPlayMedia.h
  Author:       kiven, zwu
  Version:      4.0.5
  Date:2010-3-20 // ���ߡ��汾���������
  Description:   ����ʵ�����硢����ý�弰DVD�Ļطż�����ʱ�ĵ��ڿ��ƹ���
  Others:        
  Function List:  play(), pause(), stop()��
  History:        // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸�
    1. Date:
       Author:
       Modification:
********************************************************************************/
// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� QVODPLAYMEDIA_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// QVODPLAYMEDIA_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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

//���Ժ궨��
//#define SHOW_GRAPH_IN_GRAPHEDIT
//#define SHOW_DEBUG_INFO
//#define QVOD_MEDIA_LOG
//#define NEW_POST_INTERFACE
//#define VIDEO_ZOOM 
//#define DRM
//#define BUILD_GRAPH_BY_XML
//ģ���ʼ�������ٽӿ�
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
//�����ļ�Դ
#ifdef ADD_SUBTITLE_BY_BUILDGRAPH
	QVOD_ERROR PlaySource(wchar_t *pFileUrl, wchar_t* pSubPath = NULL, E_MEDIA_TYPE mt = E_MT_VIDEO, LONGLONG lStart = 0);
#else
	QVOD_ERROR PlaySource(wchar_t *pFileUrl,/* wchar_t* pSubPath = NULL,*/ E_MEDIA_TYPE mt = E_MT_VIDEO, LONGLONG lStart = 0);
#endif
	//���Ź��
	QVOD_ERROR PlayPauseAd(bool bIsPlayAd, wchar_t *pFileUrl = NULL);
	QVOD_ERROR PlayLocalAd(bool bIsPlayAd, wchar_t *pAdUrl = NULL);
	QVOD_ERROR PlayAd(wchar_t *pAdUrl, bool bfixsize, RECT *prect);
	QVOD_ERROR StopAd();
	void SetPauseAdEnable(bool bAdEnable);
	//����
	QVOD_ERROR Play();
	//ֹͣ
	QVOD_ERROR Stop();
	//��ͣ
	QVOD_ERROR Pause();
	//����Graph
	void DestroyGraph();
	//��ȡ����״̬
	QVOD_ERROR GetPlayState(e_PlayState *pplaystate);
	//��ȡ��ǰ�ļ���ȡλ��
	QVOD_ERROR GetCurrentPos(REFERENCE_TIME *pvalue);
	//���õ�ǰ�ļ���ȡλ��
	QVOD_ERROR SetCurrentPos(REFERENCE_TIME  lvalue);
	//��ǰý���Ƿ����ק
	bool CurrentMediaCanSeek();
	//ʶ��ý���ļ�����
	e_CONTAINER GetMediaFileType(wchar_t *pFileUrl);
	//��ȡ����ʱ��
	QVOD_ERROR GetDuration(REFERENCE_TIME *pvalue);
	//��ȡȫ����ʶ
	QVOD_ERROR GetFull(bool *pvalue);
	//����ȫ����ʶ
	QVOD_ERROR SetFull(bool bvalue) ;
	//��ȡ������ʶ
	QVOD_ERROR GetMute(bool *pvalue);
	//���þ�����ʶ
	QVOD_ERROR SetMute(bool bvalue);
	//��ȡ����
	QVOD_ERROR GetVolume(long *pvalue);
	//��������
	QVOD_ERROR SetVolume(long lvalue);
	//void SetFlashVolume(UINT lvalue);
	//���ò�������
	QVOD_ERROR SetPlaySpeed(double lSpeed);
	//��ȡ��������
	QVOD_ERROR GetPlaySpeed(double *plSpeed);
	//��ȡ��Ƶ��Ⱦ����֡��
	QVOD_ERROR GetVideoFrameNum(int *pframenum);
	//��ȡ��Ƶ���
	QVOD_ERROR GetViewFrame(e_Viewframe *pviewframe, e_VFULLFRAME *pFFrame);
	//������Ƶ���
	QVOD_ERROR SetViewFrame(e_Viewframe eViewframe, e_VFULLFRAME eFFrame);
	//����Ŵ�
	QVOD_ERROR SetVideoZoom(POINT pt, float zoomparam);
	//�ƶ��Ŵ�����
	QVOD_ERROR SetZoomMove(int x, int y);
	//��̬������Ƶ����filter
	QVOD_ERROR DynamicAddVideoPostFilter(bool bAddsub = true, bool bAddPostVideo = true);
	//��ȡ����
	QVOD_ERROR GetSoundChannel(e_SoundChannel *pSoundChannel);
	//��������
	QVOD_ERROR SetSoundChannel(e_SoundChannel eSoundChannel);
	//������ƵRender
	QVOD_ERROR SetVideoRender(e_VideoRender eVideoRender);
	//��ȡ��ƵRender
	QVOD_ERROR GetVideoRender(e_VideoRender *pVideoRender);
	//��ȡ��ƵRender��
	QVOD_ERROR GetAudioRenderCount(long *pAudioCount, long *pbufsize, wchar_t *pAudioBuf);
	//��ȡ��ƵRender
	QVOD_ERROR GetCurAudioRender(long *pAudioRender);
	//������ƵRender
	QVOD_ERROR SetCurAudioRender(long lAudioOrder);
	//��ȡ��ͼ�ߴ�
	QVOD_ERROR GetClipSize(long *pwidht, long *pheight);
	//��ȡ�ļ���������
	QVOD_ERROR GetRate(long *prate);
	//��ȡ��ǰͼ��
	QVOD_ERROR GetCurrentImage(PBYTE *ppbuf,  e_CAPTUREMODE eCaptrueMode = _E_CM_CURRENT, REFERENCE_TIME* psampletime = NULL);
	//��ȡ����ٷ���
	QVOD_ERROR GetBufferPer(long *pbufferper);
	//��ȡָ��·���ļ����ļ���Ϣ
	QVOD_ERROR GetFileInfo(wchar_t *pFileUrl, s_FileInfo *ps_fileinfo, E_MEDIA_TYPE& mt);
	//��ȡý���ļ���mediainfo
	QVOD_ERROR GetMediaInfo(wchar_t *pFileUrl, struct Qvod_DetailMediaInfo	*ps_MediaInfo);
	//��ȡý���ļ���mediainfo
	//QVOD_ERROR GetLabelInfo(wchar_t *pFileUrl, struct Qvod_Label *pLabelInfo);
	//��ȡ�ļ�����
	QVOD_ERROR GetFileType(e_FileType *pfiletype);
	//��ȡ�ļ�Э��
	QVOD_ERROR GetFileProtocol(e_FileProtocol *pfileprotocol);
	//���ø����ھ��
	QVOD_ERROR SetParentWnd(long lparentwnd);		//HWND
	//��������ģ��ӿ�ָ��
	QVOD_ERROR SetQvodNet(void **pIQvodNet);
	//�Ƿ�����Ӳ������
	void IsOpenDxva(bool isopen);
	//���ò���ѡ�����Ӳ�����١�������Ƶ���ڡ�������Ӳ�����ٺ���Ƶ���ڣ�
	HRESULT SetPlayOption(e_Play_Option eOption, PlayOption_Param* pParam = NULL);
	//��ȡ����ѡ������
	void GetPlayOption(e_Play_Option &eOption, PlayOption_Param* pParam = NULL);
	//������Ƶ��ǿ
	//QVOD_ERROR SetVideoEnhance(bool IsEnhance);
	////��ȡ��Ƶ��ǿ״̬
	//QVOD_ERROR GetVideoEnhance(bool &IsEnhance);
	//���ûص�����
	QVOD_ERROR SetCallBack(IQvodCallBack *pQvodCallBack);
	//�ƶ���Ƶ����
	QVOD_ERROR MoveWindow(CRect *prect = NULL);
	//�ػ���Ƶ����
	QVOD_ERROR RepaintVideo(CRect *prect = NULL);
    //��ȡ����Ϣ
	QVOD_ERROR GetStreamInfo(S_StreamInfo *pStreamInfo);	
	//���ò���ʱ�����ݻ���ʱ��
	QVOD_ERROR SetBufferTime(int nTime);
	//ö��Filter
	QVOD_ERROR EnumFilters(_pfnUserFilter fnUserFileter, void *pvoid);
	QVOD_ERROR ShowFilter(wchar_t *pfiltername);

#ifndef BUILD_GRAPH_BY_XML
	//��ȡ��ǰ���س��ȣ�������
	QVOD_ERROR GetFileDownloadLen(INT64& iDownLoadLen);
	//����graph�е�Render Fitler
	QVOD_ERROR FindRender();
	//Add NULL Render
	QVOD_ERROR ConnectNULLRender(IPin *pPin);
	//QVOD_ERROR rotate(int x);
	QVOD_ERROR flip(E_FlipType eFlipType);
#endif
	////����Ƶ�������ýӿ�
	//QVOD_ERROR SetAddMediaFilter(bool isadd);
	////��ȡ�Ƿ������ƵƵ���ڱ�ʶ
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

	QVOD_ERROR ProductVersion(long version); //0, sample�����ף�, 1,enhance(��ǿ��)
	//��ȡ�ļ�ý������(��Ƶ����Ƶ��swf��csf������)
	QVOD_ERROR GetCurrentMediaType(E_MEDIA_TYPE& mt);
	//��ȡ֡��
	QVOD_ERROR GetCurrentavgFramerate(int* pavgframerate);
	//��ȡ��Ƶ��������
	QVOD_ERROR GetCurrenVideoCodecType(e_V_ENCODER *pvideocodectype);

	//������Ļ
	QVOD_ERROR SetSubTitle(int iIndex);
#ifdef VMR_EVR_PRESENT_TEST
	//����˫��Ļ 
	QVOD_ERROR SetMultiSub(int majorsub, int minorsub);
#endif
	//��ʾ������Ļ
	QVOD_ERROR ShowSubTitle(bool showflag);
	//��������
	QVOD_ERROR SetAudioChannel(int iIndex);

	//֡��������һ֡����һ֡
	QVOD_ERROR SetPlayFrame(long lFrameNums, bool bsync = false);
	//�ӿڲ�ѯ
	QVOD_ERROR __stdcall QueryInterface(REFIID riid, void **ppVIF);
    //ˮƽ�ƶ�����
	QVOD_ERROR Move(int x, int y );
    //�Ŵ���С
	QVOD_ERROR Zoom(int iParmer);
    //����
	QVOD_ERROR Scale(int x);   
	//QVOD_ERROR SelectScale(e_Viewframe eViewframe);
    //��ԭ��Ƶ����
	QVOD_ERROR Restore();
	
    //���沥��
	QVOD_ERROR DestopPlay(bool bDestopplay);
	//�����һ�����沥�ű�־
	QVOD_ERROR GetLastDestopPlay(bool& bflag);
	//�������沥�ţ�ע��������
	QVOD_ERROR SetDestopRecover(bool bflag);

	//����ɫ��
	void AllocateColokey(DWORD dwColorkey);

	//�Ƿ�Ӧ�ô򿪻�����ǿ
	BOOL ShallVideoEnhanceOpen();

                            
	//���ò��Ż������ݳ���
	QVOD_ERROR SetBufferingTime(int nPreBufferTime, int nPlayBufferTime);
                            
	//��ȡ���Ż������ݳ���
	void GetBufferingTime(int* pnPreBufferTime, int* pnPlayBufferTime);

#ifdef PROVIDE_CRASH_INFO
	void GetCrashLog(BYTE** ppbyte, int& len);
#endif

	//���3D��Ƶ
	void Detect3DVideo();
#ifdef DETECT_3D_VIDEO
	bool IsSuitableFor3D();
#endif

	//DVD�˵������MOVE�¼���Ӧ
	QVOD_ERROR DvdSelectAtPosition(POINT Pt);
	//DVD�˵���������¼���Ӧ
	QVOD_ERROR DvdActivateAtPosition(POINT Pt);
	//��ȡDVD�ı�����
	QVOD_ERROR	DvdGetTitles(ULONG *pTitleNum = NULL, int *pCurTitle = NULL);
	//���ñ���
	QVOD_ERROR DvdSetCurtitle(int nCurtitle, BOOL bEvent);
	//��ȡָ��DVD������½���
	QVOD_ERROR	DvdGetChapters(ULONG uTitle, ULONG *pChapterNum = NULL, int *pCurChapter = NULL);
	//����DVD���ŵ��½ں�
	QVOD_ERROR DvdSetCurChapter(int uCurchapters, BOOL bEvent);
	//��ȡ�ӽ���Ϣ
	ULONG DvdGetAngle(ULONG* pAngNum = NULL, int* pCurAngle = NULL);
	//���õ�ǰ�ӽ�
	QVOD_ERROR DvdSetAngle(int nCurangle, BOOL bEvent);
	//��ȡ��Ƶ����
	ULONG DvdGetAudios();
	//��ȡ��Ƶ��Ϣ
	QVOD_ERROR DvdGetAudios(S_DVDINFO* pAudio);
	//���õ�ǰ��Ƶ
	QVOD_ERROR	DvdSetAudio(int nCuraudio, BOOL bEvent);
	//��ȡ��Ļ��Ϣ
	QVOD_ERROR DvdGetSubTitle(S_DVDINFO* pSubTitle = NULL);
	//���õ�ǰ��Ļ
	QVOD_ERROR DvdSetSubTitle(int nCurWords, BOOL bEvent);
	//��һ��
	QVOD_ERROR PlayPrevChapter();
	//��һ��
	QVOD_ERROR PlayNextChapter();
	//����DVD�˵�
	QVOD_ERROR DvdSetMenu(DVD_MENU_ID eMenu);
#ifdef INTEL_CUSTOM_DXVA_VERSION
	//���ü�����
	QVOD_ERROR ResetMonitor();

	//��Ƶǽ�Ƿ����
	bool IsVideoWallAvailable();

	//������Ƶǽģʽ
	QVOD_ERROR SetVideoWallMode(bool flag);

	//��ȡ��Ƶǽģʽ
	QVOD_ERROR GetVideoWallMode(bool* flag);
#endif
#ifdef NEW_POST_INTERFACE
	//PostVideo�ӿ�

	//��Ƶ��ǿ
#ifdef VIDEO_ENHANCE_WIPER
	QVOD_ERROR SetVideoEnhance(bool enable,  OPTIONENHANCE effect);
#else
	QVOD_ERROR SetVideoEnhance(bool enable);
#endif
	QVOD_ERROR GetVideoEnhance(bool &enable);

	//ȥ��
	QVOD_ERROR ReduceNoise(bool enable,		//������ر�
		int ReduceNum);//������������Χ20-150,35��
	QVOD_ERROR GetReduceNoiseFlag(bool &enable,
		int &ReduceNum);

	//GAMMAУ��
	QVOD_ERROR Gamma(bool enable,		//������ر�
		int nGamma);//������������Χ20-150,35��
	QVOD_ERROR GetGammaFlag(bool &enable, 
		int &nGamma);

	//��
	QVOD_ERROR Sharpen(bool enable,			//������ر�
		int nSharpenCount,	//������(��Χ0-100��Ĭ��ֵ50)
		int nSharpenThreshold);//����ֵ����Χ0-20��Ĭ��ֵ5��
	QVOD_ERROR GetSharpenFlag(bool &enable, //������ر�
		int &nSharpenCount, //������ر�
		int &nSharpenThreshold);

	//������
	QVOD_ERROR Deinterlace(DEINTERLACE_MODE Dtlmode,//������ģʽ����DEINTERLACE_MODE����
		int Dtlparam);//���������BOBģʽ����Χ0-5��Ĭ��ֵ0����TomsMoCompģʽ����Χ0-25��Ĭ��ֵ3��
	QVOD_ERROR GetDeinterlaceFlag(DEINTERLACE_MODE &Dtlmode,
		int &Dtlparam);

	//����
	QVOD_ERROR SetBrightness(int nBrightness);//��Χ-127-127,Ĭ��ֵ0
	QVOD_ERROR GetBrightness(int &nBrightness);

	//���Ͷ�
	QVOD_ERROR SetSaturation(int nSaturation);//��Χ-127-127,Ĭ��ֵ0
	QVOD_ERROR GetSaturation(int &nSaturation);

	//�Աȶ�
	QVOD_ERROR SetContrast(int nContrast);//��Χ-127-127,Ĭ��ֵ0
	QVOD_ERROR GetContrast(int &nContrast);

	//ɫ��
	QVOD_ERROR SetHue(int nHue);//��Χ0-616,Ĭ��ֵ0
	QVOD_ERROR GetHue(int &nHue);

	//��ת
	QVOD_ERROR Flip(int nAngel);//��Χ90�ı���,Ĭ��ֵ0
	QVOD_ERROR GetFlip(int &nAngel);

	//ˮƽ��ת
	QVOD_ERROR HorizontalReverse(BOOL bHorizontal);
	QVOD_ERROR GetHorizontalReverse(BOOL &bHorizontal);

	//��ֱ��ת
	QVOD_ERROR VerticalReverse(BOOL bVertical);
	QVOD_ERROR GetVerticalReverse(BOOL &bVertical);

	QVOD_ERROR Set3DView(OPTION3DVIEW opt3DView, float kView);//OPTION3DVIEW opt3DView:������λ�ã�float kView���Ӳ�ϵ�������OPTION3DVIEWע��
	QVOD_ERROR Get3DView(OPTION3DVIEW* opt3DView, float* kView);

	//��ͼ
	QVOD_ERROR GetPostVideoImage(int WaitTime, BYTE** ppByte, int *pnBuffersize);

	//ISubtitle

	//���ã���ȡ����Ļ·��
#ifdef USE_D3D_PIXEL_SHADER
	QVOD_ERROR PutSubPath(WCHAR* wszPath/*, int& SubIndex*/);
	QVOD_ERROR GetSubPath(/*int SubIndex, */WCHAR* wszPath, int &buffersize);

	//��ʾ�����أ���Ļ
	QVOD_ERROR PutSubVisableFlag(bool bVisable, MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetSubVisableFlag(bool &bVisable,  MULTIP_SUB msub = MAJOR_SUB);

	//�����Ļ
	QVOD_ERROR ClearSubtitle();

	//���ã���ȡ����Ļ����
	QVOD_ERROR PutSubFont(LOGFONT sf,  MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetSubFont(LOGFONT &sf,  MULTIP_SUB msub = MAJOR_SUB);

	//���ã���ȡ��������ɫ
	QVOD_ERROR PutFontColor(DWORD fcolor, MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetFontColor(DWORD &fcolor, MULTIP_SUB msub = MAJOR_SUB);

	//���ã���ȡ������������ɫ
	QVOD_ERROR PutOutLineColor(DWORD fcolor,  MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetOutlineColor(DWORD &fcolor,  MULTIP_SUB msub = MAJOR_SUB);

	//���ã���ȡ����Ļλ��
	QVOD_ERROR PutSubPlace(ESubPlace place, int cx, int cy,  MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetSubPlace(ESubPlace &place, int &cx, int &cy,  MULTIP_SUB msub = MAJOR_SUB);

	//���ã���ȡ����Ļ�ӳ�(��ǰ)
	QVOD_ERROR PutSubDelay(int delaytime,  MULTIP_SUB msub = MAJOR_SUB);
	QVOD_ERROR GetSubDelay(int &delaytime,  MULTIP_SUB msub = MAJOR_SUB);

	//��ʾ��ǰ��ʣ�ࡢϵͳ��ʱ��
	QVOD_ERROR PutSubTime(SubTimeType subTimeType);
	QVOD_ERROR GetSubTime(SubTimeType &subTimeType);

	//���ã���ȡ����Ļ�ӳ�
	QVOD_ERROR PutTimeFont(LOGFONT lf);
	QVOD_ERROR GetTimeFont(LOGFONT &lf);
#else
	QVOD_ERROR PutSubPath(WCHAR* wszPath);
	QVOD_ERROR GetSubPath(WCHAR* wszPath, int &buffersize);

	//��ʾ�����أ���Ļ
	QVOD_ERROR PutSubVisableFlag(bool bVisable);
	QVOD_ERROR GetSubVisableFlag(bool &bVisable);

	//�����Ļ
	QVOD_ERROR ClearSubtitle();

	//���ã���ȡ����Ļ����
	QVOD_ERROR PutSubFont(LOGFONT sf);
	QVOD_ERROR GetSubFont(LOGFONT &sf);

	//���ã���ȡ��������ɫ
	QVOD_ERROR PutFontColor(DWORD fcolor);
	QVOD_ERROR GetFontColor(DWORD &fcolor);

	//���ã���ȡ������������ɫ
	QVOD_ERROR PutOutLineColor(DWORD fcolor);
	QVOD_ERROR GetOutlineColor(DWORD &fcolor);

	//���ã���ȡ����Ļλ��
	QVOD_ERROR PutSubPlace(ESubPlace place, int cx, int cy);
	QVOD_ERROR GetSubPlace(ESubPlace &place, int &cx, int &cy);

	//���ã���ȡ����Ļ�ӳ�(��ǰ)
	QVOD_ERROR PutSubDelay(int delaytime);
	QVOD_ERROR GetSubDelay(int &delaytime);

	//��ʾ��ǰ��ʣ�ࡢϵͳ��ʱ��
	QVOD_ERROR PutSubTime(SubTimeType subTimeType);
	QVOD_ERROR GetSubTime(SubTimeType &subTimeType);

	//���ã���ȡ����Ļ�ӳ�
	QVOD_ERROR PutTimeFont(LOGFONT lf);
	QVOD_ERROR GetTimeFont(LOGFONT &lf);
#endif
	//���ò���ʱ��
	QVOD_ERROR SetDuration(__int64 ltime);

	//ISound
	//���ã���ȡ����Ƶ����
	QVOD_ERROR PutChannel(Channel channel);
	QVOD_ERROR GetChannel(Channel &channel);

	//���ã���ȡ��������ʶ
	QVOD_ERROR PutMute(bool bmute);
	QVOD_ERROR GetMute(bool &bmute);

	//���ã���ȡ�������ӳ�
	QVOD_ERROR PutSoundDelay(LONGLONG delaytime);//��λms
	QVOD_ERROR GetSoundDelay(LONGLONG &delaytime);

	//���ã���ȡ�������ӳ�
	QVOD_ERROR PutSoundEnlarge(long lenlarge);
	QVOD_ERROR GetSoundEnlarge(long &lenlarge);

	//���ã���ȡ����Ч����
	QVOD_ERROR PutEqualization(int *plenlarge);
	QVOD_ERROR GetEqualization(int *plenlarge);

	//�������رգ���Ч����
	QVOD_ERROR PutEqualizationEnable(bool enable);
	QVOD_ERROR GetEqualizationEnable(bool &enable);

	//���ã���ȡ����Чƽ��
	QVOD_ERROR PutVolumeBalance(LONG balance);
	QVOD_ERROR GetVolumeBalance(LONG &balance);

#ifdef CHANNEAL_TRANSLATION
	//���ã���ȡ����ǰ������
	QVOD_ERROR PutChannelNum(float channel);
	QVOD_ERROR GetChannelNum(float* pchannel);
#endif

#endif
protected:
	//��ý���ļ�
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
	//��������Ϣд��Crash buffer
	bool	WriteCrashLog(const wchar_t *pstring);
#endif
	// ����·���߳�
	static void	ThreadBuildGraph(void*);
	bool	NeedToDirectCreateGraph();//�ж��������߳̽���·���ǹ����߳̽���·
	DWORD   GetVideoBitrate();
	bool	LowVideoBitrate();
	
	HANDLE	m_hBuildGraph;
	HANDLE  m_hPlayThread;
	//**************************ZWY END
protected:
	//��ͣ�����ڲ�����
	QVOD_ERROR InnerPause(bool IsInnerPlay = true);
	QVOD_ERROR InnerStop();
	QVOD_ERROR InnerPlay(bool IsInnerPlay = true);
	//����SWF�ļ�
	QVOD_ERROR PlaySWF(CString str, bool bIsAd = false);
	
	//ֹͣswf�ļ����Ų�����Active�ؼ�
	QVOD_ERROR StopSWF();
	//����MMSЭ��������ļ�
	QVOD_ERROR PlayMMsFile(wchar_t *pFileUrl);
	//ֹͣMMS�����ļ��Ĳ��Ų�����Active�ؼ�
	QVOD_ERROR StopMMSFile();
#ifdef DRM
	//DRM Active 
	QVOD_ERROR DRMActive();
#endif
	//QVOD_ERROR MapMediaType(BYTE bmt, AM_MEDIA_TYPE* pmt);
	//SWF��Ϣ������
	QVOD_ERROR SwfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	//ע���ѡ����
	void Play_RegOperater();
#ifdef VIDEO_ZOOM
	//��������
	QVOD_ERROR Video_Rect_Zoom(CRect *prect, float time);
#endif
	//SWF��Ϣ����
	static LRESULT CALLBACK HookSwfWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifdef DRM
	//DRM״̬�ص�
	static HRESULT __stdcall StatusCallback(DRM_STATUS_MSG msg,
											HRESULT hr,VOID *pvParam,
											VOID *pvContext);
#endif
private:
	//����ָ����filter���ýṹ�������Ӧ��Filter
	bool		LoadFilter(S_FILTER *ps_filter, IBaseFilter **ppbasefilter);
	//����ָ��Filterָ����������pin
	bool		FindPin(S_ADDFILTER *ps_addfilter, IPin **ppin, _PinDirection ePinType);
#ifndef BUILD_GRAPH_BY_XML
	//��鱾���ļ����ļ�����
	bool		CheckFileHeader(HANDLE	hFile, const TCHAR *pChkbytes, FILE* pFile);//byte *pbuf, int buflen,
	bool		CheckFileHeader(const TCHAR *pchkbytes, IAsyncReader* pAsyncReader);
	

	//����ts��
	QVOD_ERROR PlayTS(wchar_t *purl = NULL);
	//add ColorFilter
	QVOD_ERROR AddColorFilter(IPin **ppColorInPin, IPin **ppPostVideoInPin);
	//����FilterGraph
	bool        LoadFilterGraph();
	//�������Filter���ýṹ����ص�����Filter
	void		LoadOtherDll(S_FILTER *ps_filter);
	//ע���ע��com���
    HRESULT        RegeditDLL(S_FILTER *ps_filter, bool isreg = true);
	//��ע��com���
	void        UnRegeditDLL(bool bUnRegall);
	//����SourceFilter
	bool		LoadSourceFilter(HANDLE hfile, S_ADDFILTER *ps_addfilter, bool *bsysread);
	//����Filter
	bool		FindFilter(IPin *pOutPin, IBaseFilter **ppbasefilter, S_FILTER **pps_filter, int nfind);
	//����pin
	bool		ConnPin(IPin *pOutPin, IPin	*pInPin, AM_MEDIA_TYPE *pamt = NULL);
	
	//��������ļ����ļ�����
	//bool		CheckNetFile(const TCHAR *pChkbytes);
	//���ַ���ֵ��ʮ�����Ƶ�����ת��ΪASCIIֵ
	void		CStringtoBin(CString str, BYTE *pdata);
	//������ƵRender Filter
	HRESULT		CreateAudioFilter();
	//������ƵRender Filter
	void        CreateVideoRender();
	//ö��Fitler pin
	void		EnumFilterPin(S_ADDFILTER *ps_addfilter);
	//ö��Filter���pin
	void		EnumFilterOutPin(S_ADDFILTER *ps_addfilter);
	//ö��filter����pin
	void        EnumFilterInPutPin(S_ADDFILTER *ps_addfilter);
	//���Fitler��Graph
	bool		AddFiltertoGraph(S_ADDFILTER *ps_addfilter, LPCTSTR lpfiltername = NULL);
	//��ָ�����ļ����������Ӧ��Splitter
	bool		AddSplitter(const S_ADDFILTER *ps_addfilter, e_CONTAINER e_container);
	//�����ĻFilter
	bool		AddVobSubFilter(const TCHAR *pstrfile);//??
	//add Audio switch
	bool        AddAudioSwitch();
	//�����ļ�
	QVOD_ERROR	PlayFile();
	//��Ƶ¼��
	QVOD_ERROR RecordVideo(wchar_t *pPath, 
		E_RVQuality quality, 
		E_MEDIA_TYPE mt,
		long rtStrat, 
		long rtStop, 
		long lwidth, 
		long lheight);
	//ֹͣ¼����Ƶ
	QVOD_ERROR StopRecord(bool bContinuePlay = true);
	//Ӳ�������Ƿ�����
	bool HardWaveAccelerate();
	//������һ��������
	bool TryNextDec();
	//�Ͽ���������
	QVOD_ERROR DisconnectLowerPin(IPin *pPin, IPin **pIn, IPin **pOut, int PinNum, int &UseNum);
	//����Dvd
	QVOD_ERROR	PlayDvd();
#endif
	//��ȫɾ��graph
	void		GraphSafeRelease();

	void		InterfaceSafeRelease();
	//�ص���Ϣ
	void		CallBackMessage(long lMessage, long wparam, long lparam);

	////��Ƶ��ǿ
	//QVOD_ERROR VideoEnhance(bool IsEnhance);
	//���������ļ�
	QVOD_ERROR	PlayNetFile();

	//Render Graph
	QVOD_ERROR  PlayRender(BOOL bRun = TRUE);//??
	//DVD��Ϣ�ص�����
	HRESULT		OnDvdNotify(LONG lMsg, LONG wParam, LONG lParam);
	//���û���ٷ���
	//void		SetBufferPer(long lBufferper);
	QVOD_ERROR FindStreamSelect();
	
	//��Ƶ���ڻ�ȡ����
	QVOD_ERROR GetAdjustRect(long *lLeft, long *lTop, long *lWidth, long *lHeight);
	//��Ƶ�������ú���
	QVOD_ERROR SetAdjustRect(long  lLeft, long  lTop, long  lWidht, long lHeight);
	//��ȡ���ӵ�hashֵ
	bool GetHash(wchar_t * pFileUrl, char* pHash);
	//�������沥��
	QVOD_ERROR SetDestopPlay(bool bflag);
	//��ȡ��Ƶ���
	QVOD_ERROR GetNativeVideoSize(LPRECT pSRect, LPRECT pDRect,SIZE* pARate);
	//������Ƶλ��
	QVOD_ERROR SetVideoPosition(CRect* pSRect, CRect* pDstRect);
	//�ָ������¼�����
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
		DWORD				GetProductVersion();//0Ϊ���װ棬1Ϊ��ǿ��
#ifndef BUILD_GRAPH_BY_XML
private:
	//�������绺���ļ������߳�
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
//��������
	DWORD           m_SystemVersion;
	DWORD           m_ProductVersion;
	bool            m_bPauseAdEnable;
	wchar_t			*m_pFileUrl;//�ļ�·��
	wchar_t			*m_pSubPath;//��Ļ·��
	e_PlayState		m_ePlayState;//����״̬
	e_PlayState     m_eAdPlayState;
    e_PlayState     m_eOldState;
	e_PlayState     m_eInnerState;
	E_MEDIA_TYPE    m_eRecordMediaType;//¼��ý������
	E_MEDIA_TYPE    m_ePlayMediaType;
	REFERENCE_TIME			m_nCurPos;//Ŀǰ����ʱ��//��ǰ�ļ���ȡλ��
	//UINT			m_nDurPos;//Ŀǰ����ʱ��
	REFERENCE_TIME            m_nDuration;
	bool			m_bFull;//ȫ����ʶ
	bool			m_bMute;//������ʶ
	long			m_nVolume;//����
	e_Viewframe		m_eViewFrame;//��Ƶ���ģʽ
	e_VFULLFRAME    m_eFullFrame;//ȫ��ģʽ
	e_SoundChannel	m_eSoundChannel;//����
	e_VideoRender	m_eVideoRender;//��ƵRender Filter
	e_FileType		m_eFileType;//�ļ�����
	e_FileProtocol	m_eFileProtocol;//�ļ�Э��
	e_Play_Option   m_ePlay_Option;//����ģʽ
	PlayOption_Param m_OptionParam;
	long			m_lAudioOrder;//������Ⱦ
	long			m_lAudioRenderCount;
	HWND			m_hParentWnd;//������(������������)���
	HWND			m_hDestopWnd;//??
	CRect           m_Rect;//��Ƶ���ڴ�С
	CRect           m_SourceRect;
	CRect           m_Zoomrect;
	POINT           m_DstRectMove;
	BOOL			m_bSetNewPos;//��ҷ��ʶ
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
	bool            m_bVideoPost;//m_bAddMediaFilter;//�������Ƶ����filter
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

    //swf��ر���
	CAxWindow *m_pAdWindow;
	CAxWindow *m_pAxWindow;
	IWebBrowser2 *m_pIWebBrowser;
	FlashLib1::IShockwaveFlash	*m_pShockFlash;
	WMPLib1::IWMPPlayer4		*m_pWMPlayer;
	WMPLib1::IWMPControls       *m_pWMPControl;
	WMPLib1::IWMPNetwork        *m_pWMPNetWork;
	WMPLib1 :: IWMPSettings     *m_pWMPSet;

	//WINDOWS DS����
	IGraphBuilder			*m_pGraph;//Graphָ��
	IBasicVideo				*m_pBasicVideo;
	IBasicVideo2			*m_pBasicVideo2;
	IBasicAudio				*m_pBasicAudio;
	IMediaControl			*m_pMediaControl;
	IVideoWindow			*m_pVideoWindow;
	IMFVideoMixerControl    *m_pMFVideoMixerControl;
	//IMFVideoMixerBitmap     *m_pIMFVideoMixerBitmap;
	//IMFVideoMixerControl2   *m_pIMFVideoMixerControl2;
	IMediaSeeking			*m_pMediaSeeking;//������ק�ӿ�
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
	std::vector<S_ADDPIN>		m_v_InPin;//����pinv vector
	std::vector<S_ADDPIN>		m_v_OutPin;//���pin vector
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
	e_CONTAINER					m_e_Container;//�ļ�����
	IQvodCallBack				*m_pQvodCallBack;//�ص�����ָ��
	interface IQSourceCallBack/*IQvodCallBack*/               *m_pQSCallBack;  //QvodSource �ص�����
	IQvodNet					*m_pQvodNet;//����ģ��ָ��
	//IQLiveNet					*m_pQlive;//Qlive�ӿ�ָ��
	S_StreamInfo                *m_pStreamInfo;
	GUID                        m_gVideoSubType;
	GUID                        m_gAudioSubType;

//
//  ��ǰ���ŵ�HASHֵ -�Լ���ǰ���Ž�Ŀ������ֵ  �����ļ�
	char						m_strCurHash[41];//�����ļ���hashֵ
	int							m_nCurIndex;//��ǰ����
	CString						m_strCurFile;//
	unsigned __int64			m_nCurFileLen;//��ǰ�ļ�����
	unsigned __int64            m_DownLoadLen;//��ǰ���س���
	UINT						m_nCurDownRate;//��ǰ�����ٶ�
	unsigned __int64			m_nCurStepPos;	//��ǰ���ŵĽ�Ŀ�ڼ���FILEINFO�е�ƫ��λ��
	long						m_nBufPer;		//����ٷֱ�
	int							m_nRebuildGraphCount;
#ifdef QVOD_MEDIA_LOG
    FILE                        *m_pLogFile;
	char                        m_LogName[256];
#endif
    //  ��
	CRITICAL_SECTION m_Lock;
//#ifdef QVOD_MEDIA_LOG
	CRITICAL_SECTION m_LogLock;
//#endif
	
	//��Ϣ����ָ��
	WNDPROC					m_pSwfWindProc;
	HANDLE                  m_FileHandle;
	FILE                    *m_pFile;

	//Event
	HANDLE                      m_hDSCreateEvent; //ý���������¼�
	HANDLE						m_hEvent_Buffer; //�����¼�
	HANDLE						m_hStepEvent;
	HANDLE                      m_hCreateNetPlay;//�������粥�ŵ��߳�

	//int testParm;
	//ZWY
	QvodRenderCtrl*				m_pRenderCtrl;
	Qvod_DetailMediaInfo*		m_pCurMediaInfo;
	s_FileInfo*					m_pFileInfo;//Ӱ����Ϣ

#ifdef PROVIDE_CRASH_INFO
	BYTE*						m_Logbuffer;	//������־buffer
	int							m_LogSize;		//������־��С
	int							m_LogMaxSize;	//������־��󳤶�
	DWORD						m_nRunTime;		//��������ʼ����ʱ��
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
