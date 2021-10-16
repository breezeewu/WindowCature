#include "VideoDeviceChecker.h"
#include "stdio.h"
#include "Reg.h"
#include "Guid.h"
#include "dxva.h"
#include "SystemInfo.h"

static SizeLimit_t  NVIDIA_Feature_Set_B[] = 
{
	{769, 784},
	{849, 864},
	{929,944},
	{1009,1024},
	{1793, 1808},
	{1873, 1888},
	{1953, 1968},
	{2033, 2048},
};
#ifndef SAMPLE_VERSION
CVideoDeviceChecker CVideoDeviceChecker::m_CVideoDeviceChecker;
//CVideoDeviceChecker* CVideoDeviceChecker::m_pCVideoDeviceChecker(NULL);
//int CVideoDeviceChecker::m_nRef(0);
CVideoDeviceChecker::CVideoDeviceChecker(void):
                     m_pDirect3D9(NULL), 
					 m_pDirect3DDeviceManager9(NULL), 
					 m_pDirect3DDevice9(NULL), 
					 m_pDirectXVideoDecoderService(NULL),
					 m_nDXVAGuidNum(0),
					 m_dDXVAFlag(0),
					 m_pVideoCardInfo(NULL),
					 m_bInit(false),
					 m_bDxvaOpen(true),
					 m_eVideoCardType(ET_UNKNOW),
					 m_dwVidMemSize(0)
 {
#ifdef LOAD_DXVA_LIBRARY
	m_hd3d9 = NULL;
	m_hdxva2 = NULL;
	f_Direct3DCreate9 = NULL;
	f_DXVA2CreateVideoService = NULL;
	m_hd3d9 = LoadLibrary(L"d3d9.dll");
	m_hdxva2 = LoadLibrary(L"dxva2.dll");
	if(m_hd3d9)
	{
		f_Direct3DCreate9 = (Qvod_Direct3DCreate9)GetProcAddress(m_hd3d9,	"Direct3DCreate9");
	}

	if(m_hdxva2)
	{
		f_DXVA2CreateVideoService = (Qvod_DXVA2CreateVideoService)GetProcAddress(m_hdxva2,	"DXVA2CreateVideoService");
	}
#endif
	//MessageBox(NULL, NULL, NULL, MB_TOPMOST);
	//获取操作系统的版本信息
	/*OSVERSIONINFO versioninfo;
	memset(&versioninfo, 0, sizeof(versioninfo));
	versioninfo.dwOSVersionInfoSize = sizeof(versioninfo);
	BOOL ret = GetVersionEx(&versioninfo);
	if(ret)
	{
		m_nSystemVersion = versioninfo.dwMajorVersion;
	}*/
	//MessageBox(NULL, NULL, NULL, MB_TOPMOST);
}

CVideoDeviceChecker::~CVideoDeviceChecker(void)
{
	SafeDelete(m_pVideoCardInfo);

#ifdef LOAD_DXVA_LIBRARY	
	f_Direct3DCreate9 = NULL;
	f_DXVA2CreateVideoService = NULL;
	if(m_hd3d9)
	{
		FreeLibrary(m_hd3d9);
	}
	if(m_hdxva2)
	{
		FreeLibrary(m_hdxva2);
	}
#endif
}

QVOD_ERROR CVideoDeviceChecker::GetVideoCardInfo(VideoCardInfo *pVideoCardInfo)
{
	CheckPointer(pVideoCardInfo, E_QVOD_F_INVALIDPARAM);
	if(!m_bInit || !m_pVideoCardInfo)
	{
		return E_QVOD_E_FAIL;
	}
	*pVideoCardInfo = *m_pVideoCardInfo;
	return E_QVOD_S_OK;
}

bool CVideoDeviceChecker::GetDXVASupport(HD_CODEC encode, DWORD *pCount, GUID* pGUIDArray)
{
	if(!m_bInit)
	{
		*pCount = 0;
		return false;
	}
	DWORD flag = 0;
	switch(encode)
	{
	case HD_H261:
		flag = flag|MARK_HD_H261;
		break;
	case HD_H263:
		flag = flag|MARK_HD_H263;
		break;
	case HD_MPEG1:
		flag = flag|MARK_HD_MPEG1;
		break;
	case HD_MPEG2:
		flag = flag|MARK_HD_MPEG2;
		break;
	case HD_H264:
		flag = flag|MARK_HD_H264;
		break;
	case HD_WMV8:
		flag = flag|MARK_HD_WMV8;
		break;
	case HD_WMV9:
		flag = flag|MARK_HD_WMV9;
		break;
	case HD_VC_1:
		flag = flag|MARK_HD_VC_1;
		break;
	case HD_ALL:
		flag = flag|MARK_HD_H261|MARK_HD_H263|MARK_HD_MPEG1|MARK_HD_MPEG2|MARK_HD_H264|MARK_HD_WMV8|MARK_HD_WMV9|MARK_HD_VC_1;
		break;
	}
	DWORD k = 0;
	for(DWORD i = 0; i < m_nDXVAGuidNum; i++)
	{
		if((m_ADXVAGuid[i].flag&flag) && k < *pCount)
		{			
			pGUIDArray[k] = m_ADXVAGuid[i].guid;
			k++;
		}
	}
	*pCount = k;
	return true;
}

bool CVideoDeviceChecker::GetDXVASupport(DWORD &Dxvaflag)
{
	Dxvaflag = 0;
	if(!m_bInit || !m_bDxvaOpen)
	{
		return false;
	}
	else if(m_pVideoCardInfo)
	{
		if(m_pVideoCardInfo->bHD_Acceleration && !m_pVideoCardInfo->bDD_Acceleration)
		{
			OpenDirectDrawAcceleration();
		}
#ifdef INTEL_CUSTOM_DXVA_VERSION
		Dxvaflag = m_dDXVAFlag;
#else
		Dxvaflag = m_dDXVAFlag&(~MARK_HD_IDCTVC1);
#endif
		return true;
	}
	return false;
}

bool CVideoDeviceChecker::OpenDirectDrawAcceleration()
{
	//OSVERSIONINFO versioninfo;  //获取操作系统的版本信息
	//memset(&versioninfo, 0, sizeof(versioninfo));
	//versioninfo.dwOSVersionInfoSize = sizeof(versioninfo);	
	//BOOL rt = GetVersionEx(&versioninfo);
	//if(!rt)
	//{
	//	return false;
	//}
	DWORD value = 0;
	DWORD len = sizeof(DWORD);
	bool ret = false;
	if(GetSystemVersion() < 6)
	{
		CReg* pReg = CReg::GetInstance();
		if(pReg)
		{
			if(pReg->RegValueQuery(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\DirectDraw", L"EmulationOnly", (BYTE*)&value, len))
			{
				if(0 == value)
				{
					ret = true;
				}
				else
				{
					value = 0;
					if(pReg->RegValueSet(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\DirectDraw", L"EmulationOnly", REG_DWORD, (BYTE*)&value, len))
					{
						m_pVideoCardInfo->bDD_Acceleration = true;
						ret = true;
					}
				}
			}
		}
	}
	return ret;
}

//是否打开硬件加速
void CVideoDeviceChecker::SetDXVAOpen(bool bIsOpen)
{
	m_bDxvaOpen = bIsOpen;
}

bool CVideoDeviceChecker::ReCheck()
{
	return false;
}

CVideoDeviceChecker* CVideoDeviceChecker::GetInstance()
{
	/*if(m_pCVideoDeviceChecker == NULL)
	{
		m_pCVideoDeviceChecker = new CVideoDeviceChecker();
	}
	m_nRef++;*/
	return &m_CVideoDeviceChecker;
}

//void CVideoDeviceChecker::ReleaseInstance()
//{
//	m_nRef--;
//	if(0 == m_nRef)
//	{
//		delete m_pCVideoDeviceChecker;
//		m_pCVideoDeviceChecker = NULL;
//	}
//}

bool CVideoDeviceChecker::DXVACheck(IAMVideoAccelerator *pAMVideoAccelerator, HWND hwnd)
{
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			 OutputDebugString(L"DXVACheck\n");
#endif
	 if(CSystemInfo::GetInstance().GetProcessNum() == 6 && CSystemInfo::GetInstance().GetMajorVersion() < 6)//AMD六核配置的xp系统，不检测dxva2
	 {
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			 OutputDebugString(L"AMD 六核，xp系统，不检测dxva\n");
#endif
		 m_dDXVAFlag = 0;//MARK_HD_MPEG2 | MARK_HD_H264 | MARK_HD_VC_1;
		 return true;
	 }

	/*SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	//获取操作系统的版本信息
	OSVERSIONINFO versioninfo;
	memset(&versioninfo, 0, sizeof(versioninfo));
	versioninfo.dwOSVersionInfoSize = sizeof(versioninfo);
	BOOL ret = GetVersionEx(&versioninfo);*/
#ifndef SAMPLE_VERSION
#ifdef LOAD_DXVA_LIBRARY
	if(f_Direct3DCreate9)//d3d9.dll加载成功
	{
#endif
		//Create D3D interface
		if(m_nDXVAGuidNum > 0)
		{
			return true;
		}
		 m_nDXVAGuidNum = 0;
#ifdef LOAD_DXVA_LIBRARY
		 m_pDirect3D9 = f_Direct3DCreate9( D3D_SDK_VERSION);
#else
		m_pDirect3D9 = Direct3DCreate9( D3D_SDK_VERSION);
#endif
		 if(m_pDirect3D9 == NULL)
		 {
			 return false;
		 }

		 if(m_pVideoCardInfo == NULL)
		 {
			 m_pVideoCardInfo = new VideoCardInfo;
		 }

		 DWORD value = 0;
		 DWORD len = sizeof(DWORD);
		 if(GetSystemVersion() < 6)
		 {
			 CReg* pReg = CReg::GetInstance();
			 if(pReg)
			 {
				 if(pReg->RegValueQuery(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\DirectDraw", L"EmulationOnly", (BYTE*)&value, len))
				 {
					 if(0 == value)
					 {
						 m_pVideoCardInfo->bDD_Acceleration = true;
					 }
					 else
					 {
						 m_pVideoCardInfo->bDD_Acceleration = false;
					 }
				 }
			 }
		 }

		

		 //Get Adapter info
		 D3DADAPTER_IDENTIFIER9 D3Dadapter_identifier9;
		// D3DCAPS9 caps;
		 memset(&D3Dadapter_identifier9, 0, sizeof(D3DADAPTER_IDENTIFIER9));
		 HRESULT hr = m_pDirect3D9->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0,&D3Dadapter_identifier9);
		 //hr =  m_pDirect3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,&caps);
		 if(D3D_OK != hr)
		 {
			 return false;
		 }
		 size_t dislen = strlen(D3Dadapter_identifier9.Description);
		 //wchar_t str[256];
 #ifdef OUTPUT_VIDEO_SUPPORT_INFO
		 LPOLESTR str = NULL;//new wchar_t[256];
		 StringFromCLSID(D3Dadapter_identifier9.DeviceIdentifier, (LPOLESTR*)&str);
		 OutputDebugString(str);
#endif
		 MultiByteToWideChar(CP_ACP, 0, D3Dadapter_identifier9.Description, dislen, m_pVideoCardInfo->VideoCardDisp, 512);
 #ifdef OUTPUT_VIDEO_SUPPORT_INFO
		 OutputDebugString(m_pVideoCardInfo->VideoCardDisp);
#endif
		 m_pVideoCardInfo->DeviceID = D3Dadapter_identifier9.DeviceIdentifier;

		 wchar_t vcinfo[512];
		 wcscpy(vcinfo, m_pVideoCardInfo->VideoCardDisp);
		 wchar_t* pvcinfo = _wcsupr(vcinfo);

		 /*if(wcsstr(pvcinfo, L"INTER") || D3Dadapter_identifier9.VendorId == 0x8086)
		 {
			 m_eVideoCardType = ET_INTER;
		 }*/
		 if(wcsstr(pvcinfo, L"NVDIA") || D3Dadapter_identifier9.VendorId == 0x10DE)
		 {
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			 OutputDebugString(L"NVDIA 显卡\n");
#endif
			 m_eVideoCardType = ET_NVDIA;
		 }
		 else if(wcsstr(pvcinfo, L"ATI") || D3Dadapter_identifier9.VendorId == 0x1002)
		 {
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			 OutputDebugString(L"ATI 显卡\n");
#endif
			 m_eVideoCardType = ET_ATI;
		 }
		 else if(wcsstr(pvcinfo, L"S3GRAPH") || D3Dadapter_identifier9.VendorId == 0x5333)
		 {
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			 OutputDebugString(L"S3Graph 显卡\n");
#endif
			m_eVideoCardType = ET_S3Graph;
		 }
		 else if(wcsstr(pvcinfo, L"INTEL") || D3Dadapter_identifier9.VendorId == 0x8086)
		 {
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			 wchar_t str[100];
			 swprintf(str, L"显卡参数,VendorId:%d\n", D3Dadapter_identifier9.VendorId);
			 OutputDebugString(str);
			 OutputDebugString(L"intel 显卡\n");
#endif
			 //m_eVideoCardType = ET_UNKNOW;
			 m_eVideoCardType = ET_INTEL;
		 }
		 else
		 {
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			 wchar_t str[100];
			 swprintf(str, L"显卡参数,VendorId:%d\n", D3Dadapter_identifier9.VendorId);
			 OutputDebugString(str);
			 OutputDebugString(L"其他显卡\n");
			 m_eVideoCardType = ET_UNKNOW;
#endif
		 }

		 //检测显卡是否支持硬件加速
		 D3DCAPS9 d3dcap;
		 memset(&d3dcap, 0, sizeof(D3DCAPS9));
		 HRESULT rt = m_pDirect3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcap);
		 if(SUCCEEDED(rt))
		 {
			 m_pVideoCardInfo->bHD_Acceleration = true;
		 }
		 else if(rt == D3DERR_NOTAVAILABLE)
		 {
			 m_pVideoCardInfo->bHD_Acceleration = false;
		 }
#ifdef LOAD_DXVA_LIBRARY
	}
#endif
	 //DXVA1检测
	 DXVA1Check(pAMVideoAccelerator);

	 //Create D3DAdapter Device
	 D3DPRESENT_PARAMETERS D3dPresent_Parameter;
	 memset(&D3dPresent_Parameter, 0, sizeof(D3DPRESENT_PARAMETERS));
	 D3dPresent_Parameter.hDeviceWindow = hwnd;//GetDesktopWindow();//hWnd;
	 D3dPresent_Parameter.Windowed = TRUE;
	 D3dPresent_Parameter.SwapEffect = D3DSWAPEFFECT_DISCARD;
	 D3dPresent_Parameter.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	 DWORD dwFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	 hr = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, 
		 D3DDEVTYPE_HAL, 
		 NULL,
		 dwFlags,
		 &D3dPresent_Parameter,
		 &m_pDirect3DDevice9);
	 if(FAILED(hr) || m_pDirect3DDevice9 == NULL)
	 {
		 return false;
	 }

	 //if(CSystemInfo::GetInstance().GetProcessNum() == 6 && CSystemInfo::GetInstance().GetMajorVersion() < 6)//AMD六核配置的xp系统，不检测dxva2
	 //{
		// m_dDXVAFlag = MARK_HD_MPEG2 | MARK_HD_H264 | MARK_HD_VC_1;
		// return true;
	 //}
	 m_dwVidMemSize = m_pDirect3DDevice9->GetAvailableTextureMem();
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			 //wchar_t str[100];
			 swprintf(str, L"可用显存:%u\n", m_dwVidMemSize);
			 OutputDebugString(str);
#endif

	 //DXVA2检测
	 DXVA2Check();

	 //DXVA编码类型分类
	 DXVASort();

	 m_bInit = true;
	 SafeRelease(m_pDirect3D9);
	 SafeRelease(m_pDirect3DDeviceManager9);
	 SafeRelease(m_pDirect3DDevice9);
	 SafeRelease(m_pDirectXVideoDecoderService);
	 return true;
#else if
return false;
#endif
}

bool CVideoDeviceChecker::DXVA2Check(void)
{
	//OutputDebugString(L"DXVA2Check\n");
#ifndef SAMPLE_VERSION
#ifdef LOAD_DXVA_LIBRARY
	if(f_DXVA2CreateVideoService == NULL || m_pDirect3DDevice9 == NULL)
	{
		return false;
	}
#endif
	if(!m_pVideoCardInfo->bHD_Acceleration)
	{
		return false;
	}
	GUID *pGuidArry = NULL;
	UINT nGuidCount = 0;
#ifdef LOAD_DXVA_LIBRARY
	HRESULT hr = f_DXVA2CreateVideoService(m_pDirect3DDevice9, IID_PPV_ARGS(&m_pDirectXVideoDecoderService));
#else
	HRESULT hr = DXVA2CreateVideoService(m_pDirect3DDevice9, IID_PPV_ARGS(&m_pDirectXVideoDecoderService));
#endif
	if(FAILED(hr) || m_pDirectXVideoDecoderService == NULL)
	{
		return false;
	}

	hr = m_pDirectXVideoDecoderService->GetDecoderDeviceGuids(&nGuidCount, &pGuidArry);
	/*BYTE *pBuf = new BYTE[sizeof(GUID)*nGuidCount];
	memcpy(pBuf, pGuidArry, sizeof(GUID)* nGuidCount);
	pGuidArry = (GUID*)pBuf;
	GUID guid;*/
	for(DWORD i = 0; i < nGuidCount; i++)
	{
		//guid = pGuidArry[i];
		if(CheckDXVAGUID(&pGuidArry[i], 0))
		{
			bool exist = false;
			for(DWORD j = 0; j < m_nDXVAGuidNum; j++)
			{
				if(m_ADXVAGuid[j].guid == pGuidArry[i])
				{
					exist = true;
					if(m_ADXVAGuid[j].DxvaType == DXVA_TYPE1)
					{
						m_ADXVAGuid[j].DxvaType = DXVA_TYPE1_2;
					}
					else
					{
						m_ADXVAGuid[j].DxvaType = DXVA_TYPE2;
					}
					break;
				}
			}
			if(!exist)
			{
			   m_ADXVAGuid[m_nDXVAGuidNum].DxvaType = DXVA_TYPE2;
			   m_ADXVAGuid[m_nDXVAGuidNum].guid = pGuidArry[i];
			   m_nDXVAGuidNum++;
			}
		}
	}
	if(SUCCEEDED(hr) && nGuidCount > 0)
	{
		CoTaskMemFree(pGuidArry);
		return true;
	}
#endif
	return false;
}

bool CVideoDeviceChecker::DXVA1Check(IAMVideoAccelerator *pAMVideoAccel/*IPin *pPin*/)
{
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
	OutputDebugString(L"DXVA1Check\n");
#endif

	if(!m_pVideoCardInfo->bHD_Acceleration)
	{
		return false;
	}
	if(pAMVideoAccel == NULL)
	{
		return false;
	}
	DWORD DXVA1GuidNum = MAX_GUID_NUM;
	GUID pADXVA1Guid[MAX_GUID_NUM];
	HRESULT hr = pAMVideoAccel->GetVideoAcceleratorGUIDs(&DXVA1GuidNum, pADXVA1Guid);
	if(SUCCEEDED(hr))
	{
		for(DWORD i = 0; i < DXVA1GuidNum; i++)
		{
			if(CheckDXVAGUID(&pADXVA1Guid[i], 0))
			{
				m_ADXVAGuid[m_nDXVAGuidNum].guid = pADXVA1Guid[i];
				m_ADXVAGuid[m_nDXVAGuidNum].DxvaType = DXVA_TYPE1;
				m_nDXVAGuidNum++;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CVideoDeviceChecker::CheckDXVAGUID(GUID* pSubGuid, DWORD fourcc)
{
	if(NULL == pSubGuid)
	{
		return false;
	}
	bool ret = false;
	if(*pSubGuid == MEDIASUBTYPE_NV12
		|| *pSubGuid == MEDIASUBTYPE_NV24
		|| *pSubGuid == MEDIASUBTYPE_IMC1
		|| *pSubGuid == MEDIASUBTYPE_IMC2
		|| *pSubGuid == MEDIASUBTYPE_IMC3
		|| *pSubGuid == MEDIASUBTYPE_IMC4
		|| *pSubGuid == MEDIASUBTYPE_IMC3
		|| *pSubGuid == MEDIASUBTYPE_AYUV
		|| *pSubGuid == MEDIASUBTYPE_AI44
		|| *pSubGuid == MEDIASUBTYPE_IA44
		|| *pSubGuid == MEDIASUBTYPE_S340
		|| *pSubGuid == MEDIASUBTYPE_S342
		)//evr 硬解成功
	{
		if(fourcc == 'AVXD' || fourcc == 'avxd')
		{
			return true;
		}
		else
		{
			return false;
		}

	}

	//xp下硬解成功
	if(memcmp(&(pSubGuid->Data2), &(DXVA_ModeNone.Data2), sizeof(GUID) - 4) == 0)
	{
		
		if(pSubGuid->Data1 > DXVA_ModeNone.Data1 && pSubGuid->Data1 < DXVA_NoEncrypt.Data1)
		{
			return true;
		}
	}
	if(*pSubGuid == DXVA2_ModeMPEG2_MoComp)
	{
		return true;
	}
	else if (*pSubGuid == DXVA2_ModeMPEG2_IDCT)
	{
		return true;
	}
	else if(*pSubGuid == DXVA2_ModeMPEG2_VLD)
	{
		return true;
	}

	return ret;
}

void CVideoDeviceChecker::DXVASort()
{
	/*wchar_t str[100];
	swprintf(str, L"DXVA Num:%d\n", m_nDXVAGuidNum);
	OutputDebugString(str);*/
	for(DWORD i = 0; i < m_nDXVAGuidNum; i++)
	{
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
		if(m_ADXVAGuid[i].DxvaType == DXVA_TYPE1)
		{
			OutputDebugString(L"DXVA1:");
		}
		else if(m_ADXVAGuid[i].DxvaType == DXVA_TYPE2)
		{
			OutputDebugString(L"DXVA2:");
		}
		else if(m_ADXVAGuid[i].DxvaType == DXVA_TYPE1_2)
		{
			OutputDebugString(L"DXVA1/2:");
		}
#endif
		if(m_ADXVAGuid[i].guid.Data1 > DXVA_ModeNone.Data1 && m_ADXVAGuid[i].guid.Data1 <= DXVA_ModeH261_B.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_H261;
			m_ADXVAGuid[i].flag = MARK_HD_H261;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			if(DXVA_ModeH261_A == m_ADXVAGuid[i].guid)
			{
               OutputDebugString(L"DXVA_ModeH261_A\n");
			}
			else if(DXVA_ModeH261_B == m_ADXVAGuid[i].guid)
			{
				OutputDebugString(L"DXVA_ModeH261_B\n");
			}
#endif
			
		}
		else if(/*m_ADXVAGuid[i].guid.Data1 >= DXVA_ModeH263_A.Data1 &&*/ m_ADXVAGuid[i].guid.Data1 <= DXVA_ModeH263_F.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_H263;
			m_ADXVAGuid[i].flag = MARK_HD_H263;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			wchar_t str[30];
			swprintf(str, L"DXVA_ModeH263_%c\n", 'A'+ m_ADXVAGuid[i].guid.Data1 - DXVA_ModeH263_A.Data1);
			OutputDebugString(str);
#endif
		}
		else if(m_ADXVAGuid[i].guid.Data1 == DXVA_ModeMPEG1_A.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_MPEG1;
			m_ADXVAGuid[i].flag = MARK_HD_MPEG1;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			OutputDebugString(L"DXVA_ModeMPEG1_A\n");
#endif
		}
		else if(m_ADXVAGuid[i].guid.Data1 <= DXVA_ModeMPEG2_D.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_MPEG2_IDCT;
			m_ADXVAGuid[i].flag = MARK_HD_MPEG2_IDCT;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			wchar_t str[30];
			swprintf(str, L"DXVA_ModeMPEG2_%c\n", 'A'+ m_ADXVAGuid[i].guid.Data1 - DXVA_ModeMPEG2_A.Data1);
			OutputDebugString(str);
#endif
		}
		else if(m_ADXVAGuid[i].guid == DXVA2_ModeMPEG2_MoComp)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_MPEG2_IDCT;
			m_ADXVAGuid[i].flag = MARK_HD_MPEG2_IDCT;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			OutputDebugString(L"DXVA2_ModeMPEG2_MoComp\n");
#endif
		}
		else if(m_ADXVAGuid[i].guid == DXVA2_ModeMPEG2_IDCT)
		{
			
			//m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_MPEG2;
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_MPEG2_IDCT;
			m_ADXVAGuid[i].flag = MARK_HD_MPEG2_IDCT;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			OutputDebugString(L"DXVA2_ModeMPEG2_IDCT\n");
#endif
		}
		else if(m_ADXVAGuid[i].guid == DXVA2_ModeMPEG2_VLD)
		{
			
			//m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_MPEG2;
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_MPEG2;
			m_ADXVAGuid[i].flag = MARK_HD_MPEG2;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			OutputDebugString(L"DXVA2_ModeMPEG2_VLD\n");
#endif
		}
		else if(m_ADXVAGuid[i].guid.Data1 <= DXVA_ModeH264_D.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_IDCTH264;
			m_ADXVAGuid[i].flag = MARK_HD_IDCTH264;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			wchar_t str[30];
			swprintf(str, L"DXVA_ModeH264_%c\n", 'A'+ m_ADXVAGuid[i].guid.Data1 - DXVA_ModeH264_A.Data1);
			OutputDebugString(str);
#endif
		}
		else if(m_ADXVAGuid[i].guid.Data1 <= DXVA_ModeH264_F.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_H264;
			m_ADXVAGuid[i].flag = MARK_HD_H264;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			wchar_t str[30];
			swprintf(str, L"DXVA_ModeH264_%c\n", 'A'+ m_ADXVAGuid[i].guid.Data1 - DXVA_ModeH264_A.Data1);
			OutputDebugString(str);
#endif
		}
		else if(m_ADXVAGuid[i].guid.Data1 <= DXVA_ModeWMV8_B.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_WMV8;
			m_ADXVAGuid[i].flag = MARK_HD_WMV8;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			wchar_t str[30];
			swprintf(str, L"DXVA_ModeWMV8_%c\n", 'A'+ m_ADXVAGuid[i].guid.Data1 - DXVA_ModeWMV8_A.Data1);
			OutputDebugString(str);
#endif
		}
		else if(m_ADXVAGuid[i].guid.Data1 <= DXVA_ModeWMV9_C.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_WMV9;
			m_ADXVAGuid[i].flag = MARK_HD_WMV9;
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			wchar_t str[30];
			swprintf(str, L"DXVA_ModeWMV9_%c\n", 'A'+ m_ADXVAGuid[i].guid.Data1 - DXVA_ModeWMV9_A.Data1);
			OutputDebugString(str);
#endif
		}
		else if(m_ADXVAGuid[i].guid.Data1 <= DXVA_ModeVC1_D.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag| MARK_HD_IDCTVC1;
			m_ADXVAGuid[i].flag = MARK_HD_IDCTVC1;
			if(m_ADXVAGuid[i].guid == DXVA_ModeVC1_D)
			{
				m_dDXVAFlag =  m_dDXVAFlag| MARK_HD_VC_1;
				m_ADXVAGuid[i].flag = MARK_HD_VC_1;
			}
#ifdef OUTPUT_VIDEO_SUPPORT_INFO
			wchar_t str[30];
			swprintf(str, L"DXVA_ModeVC1_%c\n", 'A'+ m_ADXVAGuid[i].guid.Data1 - DXVA_ModeVC1_A.Data1);
			OutputDebugString(str);
#endif
		}
		else
		{
			//MessageBox(NULL, NULL, NULL, MB_TOPMOST);
			//OutputDebugString(L"发现新的硬解CLSID\n");
		}
		/*else if(m_ADXVAGuid[i].guid.Data1 == DXVA_ModeVC1_D.Data1)
		{
			m_dDXVAFlag =  m_dDXVAFlag|MARK_HD_VC_1;
			m_ADXVAGuid[i].flag = MARK_HD_VC_1;
			wchar_t str[30];
			swprintf(str, L"DXVA_ModeVC1_%c\n", 'A'+ m_ADXVAGuid[i].guid.Data1 - DXVA_ModeVC1_A.Data1);
			OutputDebugString(str);
		}*/

	}
}

DWORD CVideoDeviceChecker::GetH264RefFrameNum()
{
	if(m_pVideoCardInfo == NULL)
	{
		return 6;
	}
	//CString videocardInfo = m_pVideoCardInfo->VideoCardInfo.VideoCardDisp;
	//videocardInfo.MakeUpper();
	if(GetSystemVersion() > 5)
	{
		return INT_MAX;
	}

	/*CString vcinfo = m_pVideoCardInfo->VideoCardDisp;
	vcinfo.MakeUpper();*/
	//wcsstr(vcinfo.)
	wchar_t vcinfo[512];
	wcscpy(vcinfo, m_pVideoCardInfo->VideoCardDisp);
	wchar_t* pvcinfo = _wcsupr(vcinfo);

	/*if(wcsstr(pvcinfo, L"INTER"))
	{
		return 0;
	}
	else */if(m_eVideoCardType == ET_NVDIA)
	{
		return 13;
	}
	else if(m_eVideoCardType == ET_INTEL)
	{
		return INT_MAX;//inter暂定为无限制
	}
	else
	{
		return 6;
	}
}

DWORD CVideoDeviceChecker::GetSystemVersion()
{
	return CSystemInfo::GetInstance().GetMajorVersion();
}

VIDEO_CARD_TYPE CVideoDeviceChecker::GetVideoCardType()
{
	return m_eVideoCardType;
}

#ifdef QVOD_DXVA_TEST
bool CVideoDeviceChecker::SupportQvodHDAccel(HD_CODEC hdcodec)
{
	if(m_pVideoCardInfo == NULL)
	{
		return false;
	}

	if(m_eVideoCardType == ET_NVDIA)
	{
		return true;
	}
	else if(m_eVideoCardType == ET_INTEL)
	{
		if(hdcodec == HD_MPEG2)
		{
			return true;
		}
		else if(hdcodec == HD_VC_1)
		{
			return true;
		}
	}
	else if(m_eVideoCardType == ET_ATI)
	{
		if(hdcodec == HD_MPEG2)
		{
			return true;
		}
		else if(hdcodec == HD_VC_1)
		{
			return true;
		}
	}

	//if(m_pVideoCardInfo->DeviceID == VID_NVIDIA_GT440 ||  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"NVIDIA GeForce GT 440") != NULL)
	//{
	//	return true;
	//}
	//else if(VID_NVIDIA_GT7300 == m_pVideoCardInfo->DeviceID ||  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"NVIDIA GeForce 7300 GT") != NULL)
	//{
	//	return true;
	//}
	//else if(VID_NVIDIA_GT9400 == m_pVideoCardInfo->DeviceID ||  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"NVIDIA GeForce 9400 GT") != NULL)
	//{
	//	return true;
	//}
	////else if(VID_AMD_Radeon_HD6870 == m_pVideoCardInfo->DeviceID  ||  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"AMD Radeon HD 6800 Series") != NULL)
	////{
	////	return true;
	////}
	//else if(VID_NVIDIA_GT460 == m_pVideoCardInfo->DeviceID  ||  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"NVIDIA GeForce GTX 460") != NULL)
	//{
	//	return true;
	//}
	///*else if(VID_ATI_Mobility_Radeon_HD_545V  == m_pVideoCardInfo->DeviceID  ||  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"ATI Mobility Radeon HD 545v") != NULL)
	//{
	//	return true;
	//}*/
	//else if(VID_NVIDIA_GE9300  == m_pVideoCardInfo->DeviceID  ||  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"NVIDIA GeForce 9300 GE") != NULL)
	//{
	//	return true;
	//}
	return false;
}
#endif

bool CVideoDeviceChecker::VideoCardSizeLimit(HD_CODEC ct, int width, int height)
{
	if(ct == HD_H264)
	{
		if(m_eVideoCardType == ET_NVDIA)
		{
			if(wcsstr(m_pVideoCardInfo->VideoCardDisp, L"NVIDIA GeForce 9300") != NULL)//9300不走硬解
			{
				return true;
			}
			//Feature Set B
			if(wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 8400 GS") != NULL ||
				 wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 8200") != NULL ||
				  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 8300") != NULL ||
				   wcsstr(m_pVideoCardInfo->VideoCardDisp, L"ION") != NULL)
			{
				int size = sizeof(NVIDIA_Feature_Set_B)/sizeof(SizeLimit_t);
				for(int i = 0; i < size; i++)
				{
					if(width >= NVIDIA_Feature_Set_B[i].left && width <= NVIDIA_Feature_Set_B[i].right)
					{
						return true;
					}
				}
			}
		}
		else if(m_eVideoCardType == ET_ATI)
		{
		}
		else if(m_eVideoCardType == ET_INTEL)
		{
		}
	}
	return false;
}

bool CVideoDeviceChecker::NotSuitableForCUDA()
{
	if(m_eVideoCardType == ET_NVDIA)
		{
			//Feature Set B
			if(wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 9300") != NULL ||//目前已发现cuda加速存在问题（coreavc报错）//NVIDIA GeForce 9300 GE
				wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 9400") != NULL ||//目前已发现cuda加速存在问题（coreavc seek画面卡主）//NVIDIA GeForce 9400 GT
				 wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 8400 GS") != NULL ||
				  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 8200") != NULL ||
				   wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 8300") != NULL ||
				    wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 205") != NULL ||
					 wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 210") != NULL ||
					  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce G210") != NULL ||
					   wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 310") != NULL ||
					    wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 305") != NULL ||
					     wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 410M") != NULL ||
						  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce GT 520MX") != NULL ||
						   wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce 510") != NULL ||
						    wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce GT 520") != NULL ||
						     wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce GT 610") != NULL ||
							  wcsstr(m_pVideoCardInfo->VideoCardDisp, L"GeForce GT 620") != NULL)
			{
				return true;
			}
		}
		else if(m_eVideoCardType == ET_ATI)
		{
		}
		else if(m_eVideoCardType == ET_INTEL)
		{
		}
		return false;
}

bool CVideoDeviceChecker::NotSuitableForYV12(GUID* pguid)
{
	int ret = false;

	//GUID 为yv12
	if(pguid && *pguid == MEDIASUBTYPE_YV12)
	{
		ret = true;
	}

	return m_dwVidMemSize < 256*1024*1024 ? ret : false;

	//return true;
}
#endif
