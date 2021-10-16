#pragma once
#include <atlbase.h>
#include <windows.h>
#include <MMSystem.h>
#include <map>
#include "..\include\CaptureConfig.h"

#define HOOK_WAVE_OUT_WRITE
#ifdef HOOK_WAVE_OUT_WRITE

#pragma comment(lib, "winmm.lib")
typedef MMRESULT (WINAPI *PFN_Hook_waveOutOpen)(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);
typedef MMRESULT (WINAPI *PFN_Hook_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI *PFN_Hook_waveOutClose)(HWAVEOUT hwo); 
typedef HMODULE (WINAPI *PFN_Hook_LoadLibraryW)( LPCTSTR lpFileName);
typedef HMODULE (WINAPI *PFN_Hook_LoadLibraryA)( LPCTSTR lpFileName); 

//typedef MMRESULT (/*WINAPI*/*PFN_Hook_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
//typedef MMRESULT (WINAPI *PFN_Hook_waveOutOpen)(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);



MMRESULT WINAPI Hook_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI Hook_waveOutOpen(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);
#endif

class CHookFuncInfo
{
public:
	PFN_Hook_waveOutOpen			m_Hook_waveOutOpen;
	PFN_Hook_waveOutWrite			m_Hook_waveOutWrite;
	PFN_Hook_LoadLibraryW			m_Hook_LoadLibraryW;
	//PFN_Hook_LoadLibraryA
	
	~CHookFuncInfo();
	static CHookFuncInfo*	GetInstance();

	HRESULT AddWaveStream(HWAVEOUT hwo, LPWAVEFORMATEX pwfx);

	HRESULT DispatchAudioSample(HWAVEOUT hwo, BYTE* pdata, DWORD len);

	HRESULT GetFreeAudioSample(AudioSample** ppSample);

	HRESULT SetupAudioSource(ICaptureConfig* pCaptureConfig);

	HRESULT InstallHookFunc(class IHookControl* pIHookControl);

	HRESULT UnInstallHookFunc(class IHookControl* pIHookControl);
	
protected:
	CHookFuncInfo();

protected:
	static CHookFuncInfo*					m_pHookFuncInfo;

	std::map<HWAVEOUT, LPWAVEFORMATEX>			m_WaveInfoList;
	CComQIPtr<ICaptureConfig>					m_pICaptureConfig;

	HWAVEOUT								m_Hwo;
};