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
//��ӡ�Կ�Ӳ����Ϣ
#define OUTPUT_VIDEO_SUPPORT_INFO
#endif
#define  MAX_GUID_NUM 50
struct DXVA_GUID
{
	GUID guid;//DXVA ������
	DWORD flag;//�������ͱ�ʶ
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

    //��ȡ�Կ���Ϣ
	QVOD_ERROR GetVideoCardInfo(VideoCardInfo *pVideoCardInfo);

    //��ȡ�Կ�֧�ֵ�Ӳ����������
	bool GetDXVASupport(HD_CODEC encode, //[in],��Ƶ��������
		                DWORD *pCount = NULL, //[in],Guid�����п����ɵ����guid��(����ֵ��10),[out]��������Guid�����е���Чguid��
		                GUID* pGUIDArray = NULL);//[in],�Ѿ�������ڴ��guid����ָ��,[out],��������ָ�����������Կ���֧�ֵ�Ӳ��������

	//��ȡ֧��Ӳ�����ٵı�������
	bool GetDXVASupport(DWORD &Dxvaflag);

	//��DirectDrawӲ������
	bool OpenDirectDrawAcceleration();

	//�Ƿ��Ӳ������
	void SetDXVAOpen(bool bIsOpen);

	//���¼��
	bool ReCheck();

	//��ȡ�Կ����ʵ��
	static CVideoDeviceChecker* GetInstance();

	//���DXVASubType
	bool CheckDXVAGUID(GUID* pSubGuid, DWORD fourcc);

	//H264Ӳ��֧�ֲο�֡�ĸ���
	DWORD GetH264RefFrameNum();

	//��ʼ��ʵ��
	bool DXVACheck(IAMVideoAccelerator *pAMVideoAccelerator, HWND hwnd);

	//��ȡ�Կ�����
	VIDEO_CARD_TYPE GetVideoCardType();

#ifdef QVOD_DXVA_TEST
	//֧��Qvod Ӳ������
	bool SupportQvodHDAccel(HD_CODEC hdcodec = HD_ALL);	
#endif
	
	//�Կ��ߴ�����
	bool VideoCardSizeLimit(HD_CODEC ct, int width, int height);

	//���ʺ�cuda����
	bool NotSuitableForCUDA();

	//���ʺ�vmr9+yv12���Կ�����
	bool NotSuitableForYV12(GUID* pguid = NULL);

protected:
    //DXVA2���
	bool DXVA2Check(void);

    //DXVA1���
	bool DXVA1Check(IAMVideoAccelerator *pAMVideoAccel);

	//���ݲ�ͬ�ı������ͷ���
	void DXVASort();

	//��ȡϵͳ�汾��
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
	//��ȡ�Կ�����
	VIDEO_CARD_TYPE m_eVideoCardType;

#ifdef LOAD_DXVA_LIBRARY
	HMODULE m_hd3d9, m_hdxva2;
	Qvod_Direct3DCreate9 f_Direct3DCreate9;
	Qvod_DXVA2CreateVideoService f_DXVA2CreateVideoService;
#endif
};
#endif