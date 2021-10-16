#include "HookFunc.h"
#define NAMEPIPE_COMMUNICATION_TEST

#pragma comment(lib, "winmm.lib")

HMODULE g_MyModule;
//#define HOOK_WAVE_OUT_WRITE
#ifdef NAMEPIPE_COMMUNICATION_TEST
typedef MMRESULT (WINAPI *PFN_Hook_waveOutOpen)(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);
typedef MMRESULT (WINAPI *PFN_Hook_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI *PFN_Hook_waveOutClose)(HWAVEOUT hwo); 

//typedef MMRESULT (/*WINAPI*/*PFN_Hook_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
//typedef MMRESULT (WINAPI *PFN_Hook_waveOutOpen)(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);


PFN_Hook_waveOutOpen			g_Org_waveOutOpen  = NULL;
PFN_Hook_waveOutWrite			g_Org_waveOutWrite = NULL;
HANDLE		g_hPipeHnd = NULL;
MMRESULT WINAPI Hook_waveOutWrite(
                      HWAVEOUT hwo,
                      LPWAVEHDR pwh,
                      UINT cbwh
                      )
{
	//static bool writeb = false;
	//if(!writeb)
	//{
	//	if(pwh)
	//	{
	//		wchar_t str[100];
	//		swprintf(str, L"********Audio hwo:%d, Len:%d\n",hwo, pwh->dwBufferLength);
	//		OutputDebugString(str);
	//		//OutputDebugString(L"test");
	//	}
	//	writeb = true;
	//}

	if(NULL == g_hPipeHnd)
	{
		if(WaitNamedPipe(TEXT("\\\\.\\Pipe\\AudioPipe"), NMPWAIT_WAIT_FOREVER) != FALSE)
		{
			g_hPipeHnd = CreateFile(TEXT("\\\\.\\Pipe\\AudioPipe"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		}

		MessageBox(0,0,L"WaitNamedPipe 成功",0);
	}

	DWORD wlen = 0;
	if(g_hPipeHnd)
	{
		static HWAVEOUT orgpwh = NULL;
		if(orgpwh == NULL)
		{
			orgpwh = hwo;
		}
		DWORD id = GetCurrentProcessId();
		if(orgpwh == hwo)
		{
			WriteFile(g_hPipeHnd, pwh->lpData, pwh->dwBufferLength, &wlen,0);
			wchar_t str[100];
			swprintf(str, L"********Audio hwo:%d, Len:%d, GetCurrentProcessId:%d\n",hwo, wlen, id);
			OutputDebugString(str);
		}
	}

	//wchar_t str[100];
	//swprintf(str, L"********Audio hwo:%d, Len:%d\n",hwo, pwh->dwBufferLength);
	//OutputDebugString(str);
	if(g_Org_waveOutWrite)
	{
		//CHookFuncInfo::GetInstance()->DispatchAudioSample(hwo, (BYTE*)pwh->lpData, pwh->dwBufferLength);
		return g_Org_waveOutWrite(hwo, pwh, cbwh);
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

	//MessageBox(0,0,0,0);
	MMRESULT hr = E_FAIL;
	if(g_Org_waveOutOpen)
	{
		hr = g_Org_waveOutOpen(phwo,uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
		//hr = CHookFuncInfo::GetInstance()->m_Hook_waveOutOpen(phwo,uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
		//CHookFuncInfo::GetInstance()->AddWaveStream(*phwo, pwfx);
	}

	return hr;
}
#endif
//LRESULT CALLBACK HookCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
//{ 
//	OutputDebugString(L"HookCBTProc***************\n");
//	return CallNextHookEx(HooKCotrol::GetInstance()->GetHook(), nCode, wParam, lParam);
//}
//
//#ifdef HOOK_CREATE_D3D9_DEVICE
//HRESULT WINAPI Hook_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex **ppD3D)
//{
//	//Direct3DCreate9Ex API加载成功
//	if(g_Hook_Direct3DCreate9Ex == NULL)
//	{
//		return E_FAIL;
//	}
//	OutputDebugString(_T("hook Direct3DCreate9Ex**********************\n"));
//
//	*ppD3D = NULL;
//
//	HRESULT ret = g_Hook_Direct3DCreate9Ex(SDKVersion,  ppD3D);
//	if(*ppD3D)
//	{
//		g_pD3D9ex = *ppD3D;
//		g_pD3D9ex->AddRef();
//		OutputDebugString(_T("iexplorer Get g_pD3D9ex pointer**********************\n"));
//	}
//	return ret;
//}
//#endif

//静态变量声明
HooKCotrol*					HooKCotrol::m_pHooKCotrol(NULL);
DWORD						HooKCotrol::m_dwThreadId(0);
HHOOK						HooKCotrol::m_hHook(NULL);
std::map<PVOID, PVOID>		HooKCotrol::m_mHookFuncList;


HooKCotrol::HooKCotrol()
{
	m_dwThreadId		= 0;
	m_hHook				= NULL;
}

HooKCotrol::~HooKCotrol()
{
	DeInitHook();
}

HooKCotrol*	HooKCotrol::GetInstance()
{
	if(NULL == m_pHooKCotrol)
	{
		m_pHooKCotrol = new HooKCotrol();
	}
	return /*(IHookControl*)*/m_pHooKCotrol;
}

long HooKCotrol::EventNotify(UINT uMsg, long wparam, long lparam)
{
	return S_OK;
}

HRESULT HooKCotrol::InitHook(DWORD ThreadID)
{
	if(m_hHook == NULL)
	{
		m_dwThreadId = ThreadID;
		//MessageBox(0,0,0,0);
		m_hHook =  SetWindowsHookEx(WH_CBT, HooKCotrol::HookCBTProc, g_MyModule, m_dwThreadId);
		/*
		HMODULE hmd = GetModuleHandleA("winmm.dll");
		g_Org_waveOutOpen		= (PFN_Hook_waveOutOpen)GetProcAddress(hmd, "waveOutOpen");
		g_Org_waveOutWrite		= (PFN_Hook_waveOutWrite)GetProcAddress(hmd, "waveOutWrite");

		BOOL ret = 0;
		if(g_Org_waveOutOpen)
		{
			ret = Mhook_SetHook((PVOID *)&g_Org_waveOutOpen, Hook_waveOutOpen);
		}

		if(!ret)
		{
			return E_FAIL;
		}

		if(g_Org_waveOutWrite)
		{
			ret = Mhook_SetHook((PVOID *)&g_Org_waveOutWrite, Hook_waveOutWrite);
		}*/
		return m_hHook ? S_OK:E_FAIL;
	}

	return S_FALSE;
}

HRESULT HooKCotrol::DeInitHook()
{
	//卸载所有钩子
	for(std::map<PVOID, PVOID>::iterator it = m_mHookFuncList.begin(); it != m_mHookFuncList.end(); it++)
	{
		PVOID pfunc = it->first;
		BOOL ret = Mhook_Unhook(&pfunc);
	}


	//清空list
	m_mHookFuncList.clear();
	
	//销毁钩子
	if (m_hHook != NULL)
	{
		UnhookWindowsHookEx(m_hHook);
		m_hHook = NULL;
	}
	return S_OK;
}

HRESULT HooKCotrol::InstallWindowsHookFunc(PVOID* ppSystemFuncProc, PVOID pFuncProc)
 {
	 //检查输入参数的合法性
	CheckPointer(pFuncProc, E_POINTER);
	CheckPointer(ppSystemFuncProc, E_POINTER);

	//查找钩子map是否已经存在该函数钩子
	std::map<PVOID, PVOID>::iterator it = m_mHookFuncList.find(pFuncProc);
	if(it != m_mHookFuncList.end())
	{
		//已存在，返回
		return S_FALSE;
	}

	//设置函数钩子
	BOOL ret = Mhook_SetHook(ppSystemFuncProc, pFuncProc);
	if(ret)
	{
		//添加入钩子map链表
		m_mHookFuncList[pFuncProc] = ppSystemFuncProc;
	}

	return ret ? S_OK : E_FAIL;

 }
/*
HRESULT HooKCotrol::InstallWindowsHookFunc(PVOID pOrgFuncProc, PVOID pFuncProc)
{
	Mhook_SetHook(&pOrgFuncProc, pFuncProc);
	return S_OK;
	//检查输入参数的合法性
	CheckPointer(pFuncProc, E_POINTER);
	CheckPointer(pOrgFuncProc, E_POINTER);

	//查找钩子map是否已经存在该函数钩子
	std::map<PVOID, PVOID>::iterator it = m_mHookFuncList.find(pFuncProc);
	if(it != m_mHookFuncList.end())
	{
		//已存在，返回
		return S_FALSE;
	}

	//设置函数钩子
	BOOL ret = Mhook_SetHook(&pOrgFuncProc, pFuncProc);
	if(ret)
	{
		//添加入钩子map链表
		m_mHookFuncList[pFuncProc] = pOrgFuncProc;
	}

	return ret ? S_OK : E_FAIL;

}*/

//HRESULT HooKCotrol::InstallWindowsHookFunc(PVOID pFuncProc, char* pDllName, char* FuncName)
//{
//	//检查输入参数的合法性
//	CheckPointer(pFuncProc, E_POINTER);
//	CheckPointer(pDllName, E_POINTER);
//	CheckPointer(FuncName, E_POINTER);
//
//	//HMODULE hdm = LoadLibraryA(pDllName);
//	//获取原函数地址
//	PVOID pOrgFuncProc = DetourFindFunction(pDllName, FuncName);
//
//	//查找钩子map是否已经存在该函数钩子
//	std::map<PVOID, PVOID>::iterator it = m_mHookFuncList.find(pFuncProc);
//	if(it == m_mHookFuncList.end())
//	{
//		//已存在，返回
//		return S_FALSE;
//	}
//
//	//设置函数钩子
//	BOOL ret = Mhook_SetHook(&pFuncProc, pOrgFuncProc);
//	if(ret)
//	{
//		//添加入钩子map链表
//		m_mHookFuncList[pFuncProc] = pOrgFuncProc;
//	}
//
//	return ret ? S_OK : E_FAIL;
//
//}

HRESULT HooKCotrol::UnInstallWindowsHookFunc(PVOID pFuncProc)
{
	//检查输入参数的合法性
	CheckPointer(pFuncProc, E_POINTER);

	std::map<PVOID, PVOID>::iterator it = m_mHookFuncList.find(pFuncProc);
	if(it == m_mHookFuncList.end())
	{
		//map链表中不存在，返回
		return S_FALSE;
	}

	//删除钩子
	BOOL ret = Mhook_Unhook(&pFuncProc);

	if(ret)
	{
		//从钩子map链表中删除
		m_mHookFuncList.erase(it);
		return S_OK;
	}

	return E_FAIL;
}
#ifdef NAMEPIPE_COMMUNICATION_TEST
HRESULT HooKCotrol::HookWaveOut()
{
	if(g_Org_waveOutOpen == NULL)
	{
		HMODULE hmd = GetModuleHandleA("winmm.dll");
		g_Org_waveOutOpen		= (PFN_Hook_waveOutOpen)GetProcAddress(hmd, "waveOutOpen");
		g_Org_waveOutWrite		= (PFN_Hook_waveOutWrite)GetProcAddress(hmd, "waveOutWrite");


		InstallWindowsHookFunc((PVOID *)&g_Org_waveOutOpen, Hook_waveOutOpen);
		InstallWindowsHookFunc((PVOID *)&g_Org_waveOutWrite, Hook_waveOutWrite);
		//InstallHookFunc((PVOID )g_Org_waveOutOpen, Hook_waveOutOpen);
		//InstallHookFunc((PVOID )g_Org_waveOutWrite, Hook_waveOutWrite);
		//Mhook_SetHook((PVOID *)&g_Org_waveOutWrite, Hook_waveOutWrite);
		/*
		BOOL ret = 0;
		if(g_Org_waveOutOpen)
		{
			ret = Mhook_SetHook((PVOID *)&g_Org_waveOutOpen, Hook_waveOutOpen);
		}

		if(!ret)
		{
			return E_FAIL;
		}

		if(g_Org_waveOutWrite)
		{
			ret = Mhook_SetHook((PVOID *)&g_Org_waveOutWrite, Hook_waveOutWrite);
		}

		if(!ret)
		{
			return E_FAIL;
		}*/
	}
	/*DWORD id = GetCurrentProcessId();
	wchar_t str[100];
	swprintf(str, L"GetCurrentProcessId%d\n", id);
	MessageBox(0,0,str,0);*/
	return S_OK;
}
#endif

HRESULT HooKCotrol::InstallHookFunc(PVOID ppOrgFuncProc, PVOID pFuncProc)
{
	/*Mhook_SetHook(ppOrgFuncProc, pFuncProc);

	return S_OK;*/

	//检查输入参数的合法性
	CheckPointer(pFuncProc, E_POINTER);
	CheckPointer(ppOrgFuncProc, E_POINTER);

	//查找钩子map是否已经存在该函数钩子
	std::map<PVOID, PVOID>::iterator it = m_mHookFuncList.find(pFuncProc);
	if(it != m_mHookFuncList.end())
	{
		//已存在，返回
		return S_FALSE;
	}

	//设置函数钩子
	BOOL ret = Mhook_SetHook(&ppOrgFuncProc, pFuncProc);
	if(ret)
	{
		//添加入钩子map链表
		m_mHookFuncList[pFuncProc] = ppOrgFuncProc;
	}

	return ret ? S_OK : E_FAIL;

}

LRESULT CALLBACK HooKCotrol::HookCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{ 
	static bool CBTProcb = false;

	if(!CBTProcb)
	{
		OutputDebugString(L"HookCBTProc***************\n");
		CBTProcb = true;
	}
	return CallNextHookEx(m_hHook, nCode, wParam, lParam);
}