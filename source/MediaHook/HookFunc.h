#pragma once
//#define HOOK_CREATE_D3D9_DEVICE
////#define HOOK_LIBRARY_LIBRARY
//#define HOOK_WAVE_OUT_WRITE
////#define HOOK_Dll_GET_CLASS_OBJECT
#include "..\comment\Utility.h"
#include <windows.h>
#include <tchar.h>
#include "mhook-lib/mhook.h"
#include <Mmdeviceapi.h>
#include <D3D9.h>
#include "detours.h"
 #include "mmsystem.h"
#include "..\include\HookControl.h"
#include <map>
#define NAMEPIPE_COMMUNICATION_TEST
//#ifdef HOOK_CREATE_D3D9_DEVICE
//#pragma comment(lib, "d3d9.lib")
//typedef HRESULT (WINAPI *PENHook_Direct3DCreate9Ex)( /*_In_*/   UINT SDKVersion,  /*_Out_*/  IDirect3D9Ex **ppD3D);
//
//IDirect3D9Ex					*g_pD3D9ex						= NULL;
//PENHook_Direct3DCreate9Ex		g_Hook_Direct3DCreate9Ex		= NULL;
//
//#endif
//#pragma comment(lib, "Ole32.lib")
//#ifdef HOOK_WAVE_OUT_WRITE
//#pragma comment(lib, "Winmm.lib")
///* wave data block header */
////typedef struct wavehdr_tag {
////  LPSTR       lpData;                 /* pointer to locked data buffer */
////  DWORD       dwBufferLength;         /* length of data buffer */
////  DWORD       dwBytesRecorded;        /* used for input only */
////  DWORD_PTR   dwUser;                 /* for client's use */
////  DWORD       dwFlags;                /* assorted flags (see defines) */
////  DWORD       dwLoops;                /* loop control counter */
////  struct wavehdr_tag FAR *lpNext;     /* reserved for driver */
////  DWORD_PTR   reserved;               /* reserved for driver */
////} WAVEHDR, *PWAVEHDR, NEAR *NPWAVEHDR, FAR *LPWAVEHDR;
////
////typedef struct {
////  WORD wFormatTag; 
////  WORD nChannels; 
////  DWORD nSamplesPerSec; 
////  DWORD nAvgBytesPerSec; 
////  WORD nBlockAlign; 
////  WORD wBitsPerSample; 
////  WORD cbSize;
////} WAVEFORMATEX, *LPWAVEFORMATEX; 
////
////
////typedef __success( return == 0) UINT MMRESULT;
////typedef struct HWAVEOUT__ *HWAVEOUT, *LPHWAVEOUT;
//typedef MMRESULT (WINAPI *PFN_Hook_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
//typedef MMRESULT (WINAPI *PFN_Hook_waveOutOpen)(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);
//
//PFN_Hook_waveOutWrite g_Hook_waveOutWrite = NULL;
//PFN_Hook_waveOutOpen g_Hook_waveOutOpen	  = NULL;
//#endif
//
//typedef LONG    (WINAPI   *pfnDllGetClassObject)(REFCLSID , REFIID, LPVOID *);
//IMMDeviceEnumerator* g_pIMMDeviceEnumerator = NULL;
//
//HHOOK g_hHook;
extern HMODULE g_MyModule;
//
//pfnDllGetClassObject  g_Hook_DllGetClassObject = NULL;
//
//HMODULE	g_hHdm = NULL;


/* wave data block header */
//typedef struct wavehdr_tag {
//  LPSTR       lpData;                 /* pointer to locked data buffer */
//  DWORD       dwBufferLength;         /* length of data buffer */
//  DWORD       dwBytesRecorded;        /* used for input only */
//  DWORD_PTR   dwUser;                 /* for client's use */
//  DWORD       dwFlags;                /* assorted flags (see defines) */
//  DWORD       dwLoops;                /* loop control counter */
//  struct wavehdr_tag FAR *lpNext;     /* reserved for driver */
//  DWORD_PTR   reserved;               /* reserved for driver */
//} WAVEHDR, *PWAVEHDR, NEAR *NPWAVEHDR, FAR *LPWAVEHDR;

//typedef struct {
//  WORD wFormatTag; 
//  WORD nChannels; 
//  DWORD nSamplesPerSec; 
//  DWORD nAvgBytesPerSec; 
//  WORD nBlockAlign; 
//  WORD wBitsPerSample; 
//  WORD cbSize;
//} WAVEFORMATEX, *LPWAVEFORMATEX; 

//typedef __success( return == 0) UINT MMRESULT;
//typedef struct HWAVEOUT__ *HWAVEOUT, *LPHWAVEOUT;
//MMRESULT WINAPI Hook_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
//
//MMRESULT WINAPI Hook_waveOutOpen(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);

class  HooKCotrol:public IHookControl
{
public:
	~HooKCotrol();

/******************************************************************
	������:     GetInstance
	��������:   ��ȡ����ģ��ʵ��
	����ֵ��    �ɹ�����ģ��ʵ��ָ�룬ʧ�ܷ���NULL
*******************************************************************/
	static HooKCotrol* GetInstance();

/******************************************************************
	������:     EventNotify
	��������:   ��ʼ������ģ��
	����1:		[in] UINT uMsg����Ϣ����
	����2:		[in] long wparam������˵������
	����3:		[in] long lparam����Ϣ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/
	virtual long EventNotify(UINT uMsg, long wparam, long lparam);

/******************************************************************
	������:     InitHook
	��������:   ��ʼ������ģ��
	����1:		[in] DWORD ThreadID���߳�id������Ϊ�����ڹ��ӣ�0Ϊϵͳ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT InitHook(DWORD ThreadID);

/******************************************************************
	������:     DeInitHook
	��������:   �������й���
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT DeInitHook();

/******************************************************************
	������:     InstallWindowsHookFunc
	��������:   ��װһ�����Ӻ���
	����1:		[in] PVOID* ppSystemFuncProc(ϵͳapi����ֱ�Ӵ�����ָ��)
	����2:		[in] PVOID pFuncProc
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/
	//virtual HRESULT InstallWindowsHookFunc(PVOID pFuncProc, char* pDllName, char* FuncName);
	virtual HRESULT InstallWindowsHookFunc(PVOID* ppSystemFuncProc, PVOID pFuncProc);


/******************************************************************
	������:     UnInstallWindowsHookFunc
	��������:   ж��һ�����Ӻ���
	����1:		[in] PVOID* pFuncProc
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT UnInstallWindowsHookFunc(PVOID pFuncProc);

#ifdef NAMEPIPE_COMMUNICATION_TEST
	HRESULT HookWaveOut();
#endif

	HHOOK	GetHook();
protected:
	HooKCotrol();

	HRESULT InstallHookFunc(PVOID ppOrgFuncProc, PVOID pFuncProc);

	static LRESULT CALLBACK HookCBTProc(int nCode, WPARAM wParam, LPARAM lParam);

protected:
	static HooKCotrol*						m_pHooKCotrol;
	static DWORD							m_dwThreadId;
	static HHOOK							m_hHook;
	static std::map<PVOID, PVOID>			m_mHookFuncList;
};