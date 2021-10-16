// CaptureDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CaptureDemo.h"
#include "CaptureDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCaptureDemoDlg 对话框




CCaptureDemoDlg::CCaptureDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCaptureDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_phdm				= NULL;
}

CCaptureDemoDlg::~CCaptureDemoDlg()
{
	if(m_pIVideoCapture)
	{
		m_pIVideoCapture->DestroyInstance();
	}

	if(m_pfnDestoryMediaPost)
	{
		m_pfnDestoryMediaPost((void**)&m_pIMediaPost);
	}
	
	if(m_phdm)
	{
		FreeLibrary(m_phdm);
		m_phdm = NULL;
	}
}

void CCaptureDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCaptureDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_BeginCapture, &CCaptureDemoDlg::OnBnClickedBegincapture)
	ON_BN_CLICKED(ID_StopCapture, &CCaptureDemoDlg::OnBnClickedStopcapture)
	ON_BN_CLICKED(IDC_BUTTON1, &CCaptureDemoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCaptureDemoDlg 消息处理程序

BOOL CCaptureDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_pfnInitMediaPost		= NULL;
	m_pfnDestoryMediaPost	= NULL;
	m_pIMediaPost			= NULL;

#ifndef COM_INTERFACE
	m_pIVideoCapture = NULL;
#endif
	//MessageBox(0,0,0);
	m_phdm = LoadLibrary(L"VideoCapture.dll");

	if(m_phdm)
	{
		m_pfnInitMediaPost = (InitMediaPost)GetProcAddress(m_phdm, "InitMediaPost");
		ASSERT(m_pfnInitMediaPost);

		m_pfnDestoryMediaPost = (DestoryMediaPost)GetProcAddress(m_phdm, "DestoryMediaPost");
		ASSERT(m_pfnDestoryMediaPost);
	}

	if(m_pfnInitMediaPost)
	{
		m_pfnInitMediaPost((void**)&m_pIMediaPost);

		if(m_pIMediaPost)
		{
			m_pIMediaPost->CreateVideoRecordTask(&m_pIVideoCapture);
		}
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCaptureDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCaptureDemoDlg::OnPaint()
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
HCURSOR CCaptureDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCaptureDemoDlg::OnBnClickedBegincapture()
{
	// TODO: 在此添加控件通知处理程序代码
	ASSERT(m_pIVideoCapture);
	CWnd* pwnd = GetActiveWindow();
	RECT rect = {0};
	GetClientRect(&rect);
	rect.left = rect.top = 100;
	rect.bottom  = rect.bottom - 100;
	rect.right   = rect.right - 100;
	if(pwnd && m_pIVideoCapture)
	{
		m_pIVideoCapture->SetVideoHwnd((HWND)pwnd->m_hWnd, NULL);//pwnd->m_hWnd//(HWND)0x000B0968
		m_pIVideoCapture->StartRecord();
	}
}

void CCaptureDemoDlg::OnBnClickedStopcapture()
{
	// TODO: 在此添加控件通知处理程序代码
	ASSERT(m_pIVideoCapture);

	if(m_pIVideoCapture)
	{
		m_pIVideoCapture->StopRecord();
	}
#ifdef COM_INTERFACE
	m_pIVideoCapture.Release();
#endif
	/*
	BYTE* pImage = NULL;
	if(SUCCEEDED(m_pIVideoCapture->GetFirstBitmapImage(&pImage)))
	{
//#define WRITEIMAGE
#ifdef  WRITEIMAGE
		typedef LPBITMAPINFOHEADER PDIB;

		// Constants
#define BFT_BITMAP 0x4d42  

		// Macros
#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
	? (int)(1 << (int)(lpbi)->biBitCount)          \
	: (int)(lpbi)->biClrUsed)
#define DibSize(lpbi)           ((lpbi)->biSize + (lpbi)->biSizeImage + (int)(lpbi)->biClrUsed * sizeof(RGBQUAD))
#define DibPaletteSize(lpbi)    (DibNumColors(lpbi) * sizeof(RGBQUAD))
		//BYTE *pbyte = new BYTE[nbufsize];
		//memset(pbyte, 0, nbufsize);
		//m_pBasicVideo->GetCurrentImage(&nbufsize, (long *)pbuffer);
		BYTE* pbuffer = pImage;
		BITMAPFILEHEADER    hdr;
		DWORD               dwSize, dwWritten;
		LPBITMAPINFOHEADER  pdib = (LPBITMAPINFOHEADER) pbuffer;

		// Create a new file to store the bitmap data
		//con<<psavefile<<"\n";
		//MessageBox(NULL, psavefile,L"ERROR", MB_TOPMOST);
		//LPCWSTR psavefile = L"C:\1.bmp";
		static int testParm = 0;
		WCHAR path[100];
		swprintf(path, L"C:\\%d.bmp", testParm++);
		HANDLE hFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			//SafeDeleteArray(pbuffer);
			return ;
		}

		// Initialize the bitmap header
		dwSize = DibSize(pdib);
		hdr.bfType          = BFT_BITMAP;
		hdr.bfSize          = dwSize + sizeof(BITMAPFILEHEADER);
		hdr.bfReserved1     = 0;
		hdr.bfReserved2     = 0;
		hdr.bfOffBits       = (DWORD)sizeof(BITMAPFILEHEADER) + pdib->biSize +
			DibPaletteSize(pdib);

		// Write the bitmap header and bitmap bits to the file
		WriteFile(hFile, (LPCVOID) &hdr, sizeof(BITMAPFILEHEADER), &dwWritten, 0);
		WriteFile(hFile, (LPCVOID) pdib, dwSize, &dwWritten, 0);

		// Close the file
		CloseHandle(hFile);
#endif
	}*/
}


void CCaptureDemoDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	/*ASSERT(m_pIVideoCapture);

	if(m_pIVideoCapture)
	{
		m_pIVideoCapture->PauseRecord();
	}*/
	IVideoCapture*		pIVideoCapture = NULL;
	if(m_pfnInitMediaPost)
	{
		m_pfnInitMediaPost((void**)&m_pIMediaPost);

		if(m_pIMediaPost)
		{
			m_pIMediaPost->CreateVideoRecordTask(&pIVideoCapture);
		}
	}

	if(pIVideoCapture)
	{
		CWnd* pwnd = GetActiveWindow();
		RECT rect = {0};
		pIVideoCapture->SetVideoHwnd(pwnd->m_hWnd);
		pIVideoCapture->SetDstFilePath(L"d:\\test2.mp4");
		pIVideoCapture->StartRecord();
	}
}
