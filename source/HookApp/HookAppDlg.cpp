// HookAppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HookApp.h"
#include "HookAppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHookAppDlg 对话框

#pragma comment(lib, "winmm.lib")
#ifdef MEDIA_HOOK_DLL
PFN_Hook_waveOutOpen			g_pOrg_waveOutOpen  = NULL;
PFN_Hook_waveOutWrite			g_pOrg_waveOutWrite = NULL;
PFN_Hook_waveOutClose			g_pOrg_waveOutClose = NULL;

InitMediaHook					g_InitMediaHook		= NULL;
DestoryMediaHook				g_DestoryMediaHook	= NULL;
MMRESULT WINAPI Hook_waveOutWrite(
                      HWAVEOUT hwo,
                      LPWAVEHDR pwh,
                      UINT cbwh
                      )
{
	if(pwh)
	{
		wchar_t str[100];
		swprintf(str, L"********Audio hwo:%d, Len:%d\n",hwo, pwh->dwBufferLength);
		OutputDebugString(str);
		//OutputDebugString(L"test");
	}
	if(g_pOrg_waveOutWrite)
	{
		return g_pOrg_waveOutWrite(hwo, pwh, cbwh);
	}

	return E_FAIL;
}

MMRESULT WINAPI Hook_waveOutOpen(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen)
{
	if(pwfx)
	{
		wchar_t str[100];
		swprintf(str, L"********Hook_waveOutOpen Audio phwo %d, nChannels:%d, nSamplesPerSec:%d, wBitsPerSample:%d\n", phwo, pwfx->nChannels, pwfx->nSamplesPerSec, pwfx->wBitsPerSample);
		OutputDebugString(str);
	}

	MMRESULT hr = E_FAIL;
	if(g_pOrg_waveOutOpen)
	{
		g_pOrg_waveOutOpen(phwo,  uDeviceID,  pwfx,  dwCallback, dwInstance,  fdwOpen);
	}

	return hr;
}
#endif

CHookAppDlg::CHookAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHookAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHookAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHookAppDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CHookAppDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CHookAppDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CHookAppDlg 消息处理程序

BOOL CHookAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHookAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CHookAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHookAppDlg::OnBnClickedOk()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_HookHmd = NULL;

#ifdef MEDIA_HOOK_DLL
	m_pIHookControl = NULL;
	m_HookHmd = LoadLibrary(_T("MediaHook.dll"));
	g_InitMediaHook		= (InitMediaHook)GetProcAddress(m_HookHmd,"InitMediaHook");
	g_DestoryMediaHook	= (DestoryMediaHook)GetProcAddress(m_HookHmd,"DestoryMediaHook");

	if(g_InitMediaHook)
	{
		g_InitMediaHook((void**)&m_pIHookControl);
	}

	if(m_pIHookControl == NULL)
	{
		return;
	}

	return ;
	m_pIHookControl->InitHook(/*GetCurrentThreadId()*/0);
	
	HMODULE hdm = GetModuleHandleA("winmm.dll");
	if(hdm)
	{
		g_pOrg_waveOutOpen		= (PFN_Hook_waveOutOpen)GetProcAddress(hdm, "waveOutOpen");
		g_pOrg_waveOutWrite		= (PFN_Hook_waveOutWrite)GetProcAddress(hdm, "waveOutWrite");
		g_pOrg_waveOutClose		= (PFN_Hook_waveOutClose)GetProcAddress(hdm, "waveOutClose");
	}
	
	if(m_pIHookControl)
	{
		if(Hook_waveOutOpen && g_pOrg_waveOutOpen)
		{
			m_pIHookControl->InstallWindowsHookFunc(Hook_waveOutOpen, g_pOrg_waveOutOpen);
		}

		if(Hook_waveOutWrite && g_pOrg_waveOutOpen)
		{
			m_pIHookControl->InstallWindowsHookFunc(Hook_waveOutWrite, g_pOrg_waveOutOpen);
		}
	}
#else
	m_HookHmd = LoadLibrary(_T("HookAPI.dll"));

  FARPROC lpFun = GetProcAddress(m_HookHmd,"InstallWindowsHook");

  if ( lpFun != NULL  )
  {
    (HHOOK)lpFun(); 
  }
  else
  { 
    GetDlgItem(IDC_STATIC)->SetWindowText(_T("install error")); 
    return;
  }
  GetDlgItem(IDC_STATIC)->SetWindowText(_T("install success.")); 

#endif
  GetDlgItem(IDOK)->EnableWindow(FALSE);
  GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
}

void CHookAppDlg::OnBnClickedCancel()
{
#ifdef MEDIA_HOOK_DLL

	if(m_pIHookControl)
	{
		if(Hook_waveOutOpen)
		{
			m_pIHookControl->UnInstallWindowsHookFunc(Hook_waveOutOpen);
		}

		if(Hook_waveOutWrite)
		{
			m_pIHookControl->UnInstallWindowsHookFunc(Hook_waveOutWrite);
		}
	}
	
#else
	// TODO: 在此添加控件通知处理程序代码
	HMODULE hdll = LoadLibrary(_T("HookAPI.dll"));

	FARPROC lpFun = GetProcAddress(hdll,"UnInstallWindowsHook");

	if ( lpFun != NULL  )
	{
		(HHOOK)lpFun(); 
	}else
	{ 
		GetDlgItem(IDC_STATIC)->SetWindowText(_T("install error")); 
		exit(1);
		return;
	}
	GetDlgItem(IDC_STATIC)->SetWindowText(_T("install success.")); 
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	FARPROC lpUnInstallWindowsHook = GetProcAddress(m_HookHmd,"UnInstallWindowsHook");
	if(lpUnInstallWindowsHook)
	{
	   lpUnInstallWindowsHook();
	}
#endif
  /*if(m_HookHmd)
  {
	  FreeLibrary(m_HookHmd);
  }*/
  exit(1);
}
