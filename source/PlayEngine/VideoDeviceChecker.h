#pragma once

#include <windows.h>
#include <d3d9.h>
//#include <d3dx9.h>
#include <Dxva2api.h>
#include <streams.h>
#include <Videoacc.h>
#include "..\include\IDXVAChecker.h"

#ifndef SAMPLE_VERSION
#ifdef SHOW_DEBUG_INFO
//打印显卡硬解信息
#define OUTPUT_VIDEO_SUPPORT_INFO
#endif
#define  MAX_GUID_NUM 50
struct DXVA_GUID
{
	GUID guid;//DXVA 子类型
	DWORD flag;//编码类型标识
	DXVA_TYPE DxvaType;//DXVA1 or DXVA2
	DXVA_GUID():guid(GUID_NULL), flag(0), DxvaType(DXVA_TYPE_NULL){}

};
#ifdef LOAD_DXVA_LIBRARY
typedef IDirect3D9* (*Qvod_Direct3DCreate9)(UINT);
typedef HRESULT (*Qvod_DXVA2CreateVideoService) (IDirect3DDevice9*,REFIID,void**);
//typedef HRESULT (*Qvod_DXVA2CreateVideoService)(__in IDirect3DDevice9*,__in REFIID,__deref_out void**);
#endif

class CVideoDeviceChecker:public IDXVAChecker
{
protected:
	CVideoDeviceChecker(void);
public:
	~CVideoDeviceChecker(void);

    //获取显卡信息
	QVOD_ERROR GetVideoCardInfo(VideoCardInfo *pVideoCardInfo);

    //获取显卡支持的硬件加速类型
	bool GetDXVASupport(HD_CODEC encode, //[in],视频编码类型
		                DWORD *pCount = NULL, //[in],Guid数组中可容纳的最大guid数(建议值：10),[out]函数返回Guid数组中的有效guid数
		                GUID* pGUIDArray = NULL);//[in],已经分配好内存的guid数组指针,[out],函数返回指定编码类型显卡所支持的硬解子类型

	//获取支持硬件加速的编码类型
	bool GetDXVASupport(DWORD &Dxvaflag);

	//打开DirectDraw硬件加速
	bool OpenDirectDrawAcceleration();

	//是否打开硬件加速
	void SetDXVAOpen(bool bIsOpen);

	//重新检测
	bool ReCheck();

	//获取显卡检测实例
	static CVideoDeviceChecker* GetInstance();

	//检测DXVASubType
	bool CheckDXVAGUID(GUID* pSubGuid, DWORD fourcc);

	//H264硬解支持参考帧的个数
	DWORD GetH264RefFrameNum();

	//初始化实例
	bool DXVACheck(IAMVideoAccelerator *pAMVideoAccelerator, HWND hwnd);

	//获取显卡类型
	VIDEO_CARD_TYPE GetVideoCardType();

#ifdef QVOD_DXVA_TEST
	//支持Qvod 硬件加速
	bool SupportQvodHDAccel(HD_CODEC hdcodec = HD_ALL);	
#endif
	
	//显卡尺寸限制
	bool VideoCardSizeLimit(HD_CODEC ct, int width, int height);

	//不适合cuda加速
	bool NotSuitableForCUDA();

	//不适合vmr9+yv12的显卡环境
	bool NotSuitableForYV12(GUID* pguid = NULL);

protected:
    //DXVA2检测
	bool DXVA2Check(void);

    //DXVA1检测
	bool DXVA1Check(IAMVideoAccelerator *pAMVideoAccel);

	//根据不同的编码类型分类
	void DXVASort();

	//获取系统版本号
	DWORD GetSystemVersion();

protected:
	IDirect3D9* m_pDirect3D9;
	IDirect3DDeviceManager9*  m_pDirect3DDeviceManager9;
	IDirect3DDevice9* m_pDirect3DDevice9;
	IDirectXVideoDecoderService* m_pDirectXVideoDecoderService;
	DXVA_GUID m_ADXVAGuid[MAX_GUID_NUM];
	DWORD m_nDXVAGuidNum;
	DWORD m_dDXVAFlag;
	DWORD m_dwVidMemSize;
	VideoCardInfo *m_pVideoCardInfo;
	bool m_bInit;
	bool m_bDxvaOpen;
	//static int m_nRef;
	static CVideoDeviceChecker m_CVideoDeviceChecker;
	//获取显卡类型
	VIDEO_CARD_TYPE m_eVideoCardType;

#ifdef LOAD_DXVA_LIBRARY
	HMODULE m_hd3d9, m_hdxva2;
	Qvod_Direct3DCreate9 f_Direct3DCreate9;
	Qvod_DXVA2CreateVideoService f_DXVA2CreateVideoService;
#endif
};
#endif