#include "stdafx.h"
#include "HookAPIFunc.h"
#include "..\include\HookControl.h"
#include <wxdebug.h>
#ifdef HOOK_WAVE_OUT_WRITE
MMRESULT WINAPI Hook_waveOutWrite(
                      HWAVEOUT hwo,
                      LPWAVEHDR pwh,
                      UINT cbwh
                      )
{
	
	MessageBox(0,0,L"Hook_waveOutWrite hook succeeded\n",0);
	if(pwh)
	{
		wchar_t str[100];
		swprintf(str, L"********Audio hwo:%d, Len:%d\n",hwo, pwh->dwBufferLength);
		OutputDebugString(str);
		//OutputDebugString(L"test");
	}
	if(CHookFuncInfo::GetInstance()->m_Hook_waveOutWrite)
	{
		CHookFuncInfo::GetInstance()->DispatchAudioSample(hwo, (BYTE*)pwh->lpData, pwh->dwBufferLength);
		return CHookFuncInfo::GetInstance()->m_Hook_waveOutWrite(hwo, pwh, cbwh);
	}

	return E_FAIL;
}

MMRESULT WINAPI Hook_waveOutOpen(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen)
{
	/*if(pwfx)
	{
		wchar_t str[100];
		swprintf(str, L"********Hook_waveOutOpen Audio phwo %d, nChannels:%d, nSamplesPerSec:%d, wBitsPerSample:%d\n", phwo, pwfx->nChannels, pwfx->nSamplesPerSec, pwfx->wBitsPerSample);
		OutputDebugString(str);
	}*/

	MMRESULT hr = E_FAIL;
	if(CHookFuncInfo::GetInstance()->m_Hook_waveOutOpen)
	{
		hr = CHookFuncInfo::GetInstance()->m_Hook_waveOutOpen(phwo,uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
		CHookFuncInfo::GetInstance()->AddWaveStream(*phwo, pwfx);
	}

	return hr;
}

HMODULE WINAPI Hook_LoadLibraryW( /*_In_*/  LPCWSTR lpFileName)
{
	MessageBox(0,0,L"LoadLibrary Hook Succeeded*************************\n",0);
	OutputDebugString(L"LoadLibrary Hook Succeeded*************************\n");
	if(CHookFuncInfo::GetInstance()->m_Hook_LoadLibraryW)
	{
		HMODULE hdm = CHookFuncInfo::GetInstance()->m_Hook_LoadLibraryW(lpFileName);
		return hdm;
	}
	//if(wcsstr((wchar_t*)lpFileName, L"MMDevAPI.dll"))
	//{
	//	OutputDebugString(L"load MMDevAPI.dll succeeded******************\n");
	//	//MessageBox(NULL, L"load MMDevAPI.dll!", 0, 0);
	//	g_hHdm = GetModuleHandleW(lpFileName);;
	//	g_Hook_DllGetClassObject = (pfnDllGetClassObject)GetProcAddress(g_hHdm, "DllGetClassObject");
	//    if(g_Hook_DllGetClassObject)//g_Hook_waveOutWrite)
	//    {
	//		OutputDebugString(L"load MMDevAPI.dll Hook succeeded******************\n");
	//		BOOL ret = Mhook_SetHook((PVOID*)&g_Hook_DllGetClassObject, Hook_DllGetClassObject);
	//		 //return 
	//		// TRUE;
	//	}
	//	else
	//	{
	//		OutputDebugString(L"load MMDevAPI.dll Hook failed******************\n");
	//		//return NULL;
	//	}
	//}

	return NULL;
}
#endif

CHookFuncInfo*	CHookFuncInfo::m_pHookFuncInfo(NULL);

CHookFuncInfo::CHookFuncInfo()
{
	//MessageBox(0,0,0,0);
	HMODULE hmd = GetModuleHandleA("winmm.dll");//GetModuleHandleA("winmm.lib");
	ASSERT(hmd);
	m_Hook_waveOutOpen		= (PFN_Hook_waveOutOpen)GetProcAddress(hmd, "waveOutOpen");
	ASSERT(m_Hook_waveOutOpen);
	m_Hook_waveOutWrite		= (PFN_Hook_waveOutWrite)GetProcAddress(hmd, "waveOutWrite");
	ASSERT(m_Hook_waveOutWrite);
	hmd = GetModuleHandleA("Kernel32.dll");
	m_Hook_LoadLibraryW		= (PFN_Hook_LoadLibraryW)GetProcAddress(hmd, "LoadLibraryW");
	//m_Hook_LoadLibraryA		= (PFN_Hook_LoadLibraryA)GetProcAddress(hmd, "LoadLibraryA");
}

CHookFuncInfo::~CHookFuncInfo()
{

}

CHookFuncInfo*	CHookFuncInfo::GetInstance()
{
	if(NULL == m_pHookFuncInfo)
	{
		m_pHookFuncInfo = new CHookFuncInfo();
	}

	return m_pHookFuncInfo;
}

HRESULT CHookFuncInfo::AddWaveStream(HWAVEOUT hwo, LPWAVEFORMATEX pwfx)
{
	if(hwo == NULL || pwfx == NULL)
	{
		return E_POINTER;
	}

	std::map<HWAVEOUT, LPWAVEFORMATEX>::iterator it = m_WaveInfoList.find(hwo);
	if(it == m_WaveInfoList.end())
	{
		LPWAVEFORMATEX pwf = new WAVEFORMATEX;
		*pwf = *pwfx;
		m_WaveInfoList[hwo] = pwf;
		if(m_Hwo != NULL && pwfx->nChannels >= 2)
		{
			m_Hwo	= hwo;
		}


		return S_OK;
	}
	
	return S_FALSE;
}

HRESULT CHookFuncInfo::DispatchAudioSample(HWAVEOUT hwo, BYTE* pdata, DWORD len)
{
	CheckPointer(m_pICaptureConfig, E_FAIL);
	CheckPointer(pdata, E_POINTER);

	if(m_Hwo != hwo)
	{
		return E_FAIL;
	}

	AudioSample* psample = NULL;
	m_pICaptureConfig->GetFreeSample(&psample);

	if(psample)
	{
		if(psample->MaxDataLen < len)
		{
			if(psample->pData)
			{
				delete[] psample->pData;
			}

			psample->pData = new BYTE[len];
			psample->MaxDataLen = len;
		}

		return m_pICaptureConfig->AddAudioSample(psample);
	}

	return S_OK;
}


HRESULT CHookFuncInfo::SetupAudioSource(ICaptureConfig* pCaptureConfig)
{
	if(pCaptureConfig == NULL)
	{
		m_pICaptureConfig.Release();

		return S_OK;
	}

	m_pICaptureConfig = pCaptureConfig;

	return S_OK;
}

HRESULT CHookFuncInfo::InstallHookFunc(IHookControl* pIHookControl)
{
	CheckPointer(pIHookControl, E_POINTER);
	//pIHookControl->InitHook(0);//GetCurrentThreadId()
	//pIHookControl->InstallWindowsHookFunc(Hook_waveOutOpen, m_Hook_waveOutOpen);
	//pIHookControl->InstallWindowsHookFunc(Hook_waveOutWrite, m_Hook_waveOutWrite);
	//pIHookControl->InstallWindowsHookFunc(Hook_LoadLibraryW, m_Hook_LoadLibraryW);

	return S_OK;	
}

HRESULT CHookFuncInfo::UnInstallHookFunc(class IHookControl* pIHookControl)
{
	CheckPointer(pIHookControl, E_POINTER);

	pIHookControl->DeInitHook();
	pIHookControl->UnInstallWindowsHookFunc(Hook_waveOutOpen);
	pIHookControl->UnInstallWindowsHookFunc(Hook_waveOutWrite);

	return S_OK;
}