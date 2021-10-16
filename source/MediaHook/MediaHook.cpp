// MediaHook.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "HookFunc.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif
typedef MMRESULT (WINAPI *PFN_Hook_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI *PFN_Hook_waveOutOpen)(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);

PFN_Hook_waveOutWrite g_Hook_waveOutWrite = NULL;
PFN_Hook_waveOutOpen g_Hook_waveOutOpen	  = NULL;

EXTERN_C __declspec(dllexport) HRESULT InitMediaHook(void** ppMediaHook)
{
	//检测指针的合法性
	CheckPointer(ppMediaHook, E_POINTER);

	//给指针赋初值
	*ppMediaHook = NULL;

	//创建实例对象
	*ppMediaHook = (void*)HooKCotrol::GetInstance();

	//MessageBox(0,L"InitHook",0,0);
	HooKCotrol::GetInstance()->InitHook(0);//GetCurrentThreadId()
	return S_OK;
}

EXTERN_C __declspec(dllexport) HRESULT DestoryMediaHook(void** ppMediaHook)
{
	//检测指针的合法性
	CheckPointer(ppMediaHook, E_POINTER);

	//还原实例指针对象
	HooKCotrol* pMediaHook = static_cast<HooKCotrol*>(*ppMediaHook);

	HooKCotrol::GetInstance()->DeInitHook();
	//销毁实例
	delete pMediaHook;

	//置空
	*ppMediaHook = NULL;

	return S_OK;
	
}
/*
BOOL HookFunction()
{
//#ifdef HOOK_WAVE_OUT_WRITE
	HMODULE hdm = GetModuleHandleA("Winmm.dll");
	g_Hook_waveOutOpen = (PFN_Hook_waveOutOpen)GetProcAddress(hdm, "waveOutOpen");//DetourFindFunction("Winmm.dll", "waveOutOpen");
	g_Hook_waveOutWrite = (PFN_Hook_waveOutWrite)GetProcAddress(hdm, "waveOutWrite");//DetourFindFunction("Winmm.dll", "waveOutWrite");
	if(g_Hook_waveOutOpen && g_Hook_waveOutWrite)
	{
	      BOOL ret = Mhook_SetHook((PVOID*)&g_Hook_waveOutWrite, Hook_waveOutWrite);
	      if(ret)
		  {
			  OutputDebugString(L"g_Hook_waveOutWrite get SUCCEEDED********\n");
			  //MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
		  }
		  ret = Mhook_SetHook((PVOID*)&g_Hook_waveOutOpen, Hook_waveOutOpen);
		  if(ret)
		  {
			  OutputDebugString(L"g_Hook_waveOutOpen get SUCCEEDED********\n");
			  //MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
		  }
	}
	else
	{
		OutputDebugString(L"g_Hook_waveOutOpen && g_Hook_waveOutWrite get failed********\n");
		return FALSE;
	}
//#endif
}

BOOL UnHookFunction()
{
	BOOL ret = 0;
  if (g_Hook_waveOutWrite != NULL)
  {
		ret = Mhook_Unhook((PVOID*)&g_Hook_waveOutWrite);
  }

  if (g_Hook_waveOutOpen != NULL)
  {
		ret = Mhook_Unhook((PVOID*)&g_Hook_waveOutOpen);
  }

  return ret;
}
//LRESULT CALLBACK HookCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
//{ 
//	OutputDebugString(L"HookCBTProc*****************************\n");
//	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
//}

//void InstallWindowsHook()
//{
//  if (g_hHook != NULL)
//  {
//    return;
//  }
//
//  g_hHook =  SetWindowsHookEx(WH_CBT, HookCBTProc, g_MyModule, 0);
//}
//
//void UnInstallWindowsHook()
//{
//	UnHookFunction();
//
//  if (g_hHook != NULL)
//  {
//    UnhookWindowsHookEx(g_hHook);
//    g_hHook = NULL;
//  }
//}
*/

#ifdef NAMEPIPE_COMMUNICATION_TEST

BOOL IsProcess()
{
  LPWSTR lpProcessName = NULL; 
  DWORD dwModuleName = 0;
  LPWSTR lpFileName = NULL;
  LPWSTR lpFileExt = NULL;
  LPWSTR lpFileVName = NULL;
  BOOL bResult = FALSE; 

  do
  {
    lpProcessName = new WCHAR[0x200];

    if (lpProcessName == NULL)
    {
      break;
    }

    lpFileName = new WCHAR[0x200];

    if (lpFileName == NULL)
    {
      break;
    }

    lpFileExt = new WCHAR[0x20];

    if (lpFileExt == NULL)
    {
      break;
    }

    lpFileVName = new WCHAR[0x200];

    if (lpFileVName == NULL)
    {
      break;
    }

    dwModuleName = GetModuleFileNameW(NULL, lpProcessName, 0x200);
	OutputDebugString(lpProcessName);
    if (dwModuleName == 0)
    {
      break;
    }
    _wsplitpath_s(lpProcessName, NULL, 0, NULL, 0, lpFileName, 0x200, lpFileExt, 0x20);
    _stprintf_s(lpFileVName, 0x200, _T("%s%s"), lpFileName, lpFileExt);

    if(_tcsnicmp(lpFileVName, _T("KWExplorer.exe"), _tcslen(lpFileVName)) == 0)//KWExplorer.exe
    {
      bResult = TRUE;
      break;
    }

    break;
  } while (FALSE);

  if (lpProcessName != NULL)
  {
    delete lpProcessName;
    lpProcessName = NULL;
  }

  if (lpFileExt != NULL)
  {
    delete lpFileExt;
    lpFileExt = NULL;
  }

  if (lpFileName != NULL)
  {
    delete lpFileName;
    lpFileName = NULL;
  }

  if (lpFileVName != NULL)
  {
    delete lpFileVName;
    lpFileVName = NULL;
  }

  return bResult;
}
#endif
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_MyModule = hModule;
#ifdef NAMEPIPE_COMMUNICATION_TEST
		OutputDebugString(L"MediaHook DLL Load******************************\n");
		if (IsProcess())
		{
			//HooKCotrol::GetInstance()->InitHook(0);
			/*OutputDebugString(L"IsProcess()->HookFunction********\n");*/
			//HookFunction();
			HooKCotrol::GetInstance()->HookWaveOut();
		}
		else
		{
			OutputDebugString(L"!IsProcess()->HookFunction failed********\n");
		}
#endif
		break;
	case DLL_PROCESS_DETACH:
		HooKCotrol::GetInstance()->DeInitHook();
    /*if (IsProcess())
    {
      UnHookFunction();
    }*/
		break;
	}

    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

