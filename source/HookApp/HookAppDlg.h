// HookAppDlg.h : 头文件
//

#pragma once

//#include <atlbase.h>
#include <windows.h>
#include <MMSystem.h>
#include "..\include\HookControl.h"

typedef MMRESULT (WINAPI *PFN_Hook_waveOutOpen)(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);
typedef MMRESULT (WINAPI *PFN_Hook_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI *PFN_Hook_waveOutClose)(HWAVEOUT hwo); 

typedef HRESULT (*InitMediaHook)(void** ppMediaHook);


typedef HRESULT (*DestoryMediaHook)(void** ppMediaHook);
//#define MEDIA_HOOK_DLL
#ifdef MEDIA_HOOK_DLL
extern PFN_Hook_waveOutOpen			g_pOrg_waveOutOpen;
extern PFN_Hook_waveOutWrite			g_pOrg_waveOutWrite;
extern PFN_Hook_waveOutClose			g_pOrg_waveOutClose;

extern InitMediaHook					g_InitMediaHook;
extern DestoryMediaHook				g_DestoryMediaHook;
#endif
// CHookAppDlg 对话框
class CHookAppDlg : public CDialog
{

// 构造
public:
	CHookAppDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HOOKAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	HMODULE m_HookHmd;

	IHookControl*		m_pIHookControl;


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
