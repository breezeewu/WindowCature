// yv12Viewer.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "yv12Viewer.h"
/*
#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_YV12VIEWER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_YV12VIEWER));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_YV12VIEWER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_YV12VIEWER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
*/

// yv12.cpp : 
#define WIN32_LEAN_AND_MEAN  // �� Windows ͷ���ų�����ʹ�õ�����
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "ddraw.h"
#pragma comment(lib,"ddraw.lib")
// �˴���ģ���а����ĺ�����ǰ������:
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM); // ������Ϣ������
BOOL     DrawYV12(HWND hWnd);    // Draw YV12 ͼ��
// Ӧ�����
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	WNDCLASSEX wcex;
	TCHAR szWindowClass[] = L"Draw YV12";   // ����������
	HWND hWnd;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style   = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra  = 0;
	wcex.cbWndExtra  = 0;
	wcex.hInstance  = hInstance;
	wcex.hIcon   = NULL;
	wcex.hCursor  = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm  = NULL;
	RegisterClassEx(&wcex);
	hWnd = CreateWindow(szWindowClass, szWindowClass, 
		WS_OVERLAPPEDWINDOW,//WS_POPUP, 
		0, 0, 
		800,//GetSystemMetrics(SM_CXSCREEN), 
		600,//GetSystemMetrics(SM_CYSCREEN), 
		NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}
//
//  ����: WndProc(HWND, unsigned, WORD, LONG)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND - ����Ӧ�ó���˵�
//  WM_PAINT - ����������
//  WM_DESTROY - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_KEYDOWN: 
		switch( wParam ) 
		{ 
		case VK_ESCAPE: 
		case VK_F12: 
			PostMessage(hWnd, WM_CLOSE, 0, 0); 
			break;
		case VK_TAB:
			if(!DrawYV12(hWnd))
				MessageBox(hWnd,L"Draw yv12 image failed",L"DDraw error", MB_OK);
			break; 
		} 
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
#define FILE_HEIGHT   288
#define FILE_WIDTH   352
#define DRAW_TOP   0
#define DRAW_LEFT   0
#define DRAW_HEIGHT   774
#define DRAW_WIDHT   1190
BOOL DrawYV12(HWND hWnd)
{
	LPDIRECTDRAW            lpDD;    // DirectDraw ����ָ��
	LPDIRECTDRAWSURFACE     lpDDSPrimary;  // DirectDraw ������ָ��
	LPDIRECTDRAWSURFACE     lpDDSOffScr;  // DirectDraw ��������ָ��
	DDSURFACEDESC   ddsd;    // DirectDraw ��������
	RECT     rctDest;   // Ŀ������
	RECT     rctSour;   // Դ����
	HRESULT     ddRval;    // DirectDraw ��������ֵ
	// ����DirectCraw����
	if (DirectDrawCreate(NULL, &lpDD, NULL) != DD_OK) 
		return FALSE;

	// ����Э����
	if (lpDD->SetCooperativeLevel(hWnd,
		DDSCL_NORMAL | DDSCL_NOWINDOWCHANGES) != DD_OK)
		return FALSE;

	// ����������
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL) != DD_OK)
		return FALSE;

	// ���������������
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY; //DDSCAPS_OVERLAY DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	ddsd.dwWidth = DRAW_WIDHT;
	ddsd.dwHeight = DRAW_HEIGHT;
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags  = DDPF_FOURCC | DDPF_YUV ;
	ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','V','1','2');
	ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
	if (lpDD->CreateSurface(&ddsd, &lpDDSOffScr, NULL) != DD_OK)
		return FALSE;
	LPBYTE lpYV12 = new BYTE[FILE_WIDTH * FILE_HEIGHT * 3 / 2];
	// ����yv12ͼ���ļ�
	FILE * f = fopen("test.yv12","rb");
	UINT iLen = FILE_WIDTH * FILE_HEIGHT * 3 / 2;
	if(f)
	{
		iLen = fread(lpYV12, 1, FILE_WIDTH * FILE_HEIGHT * 3 / 2, f);
		fclose(f);
	}
	
	LPBYTE lpY = lpYV12;
	LPBYTE lpV = lpYV12 + FILE_WIDTH * FILE_HEIGHT;
	LPBYTE lpU = lpYV12 + FILE_WIDTH * FILE_HEIGHT * 5 / 4;

	ddRval = lpDDSOffScr->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
	while(ddRval == DDERR_WASSTILLDRAWING);
	if(ddRval != DD_OK)
		return FALSE;
	LPBYTE lpSurf = (LPBYTE)ddsd.lpSurface;
	LPBYTE lpY1 = lpSurf;
	LPBYTE lpV1 = lpSurf + ddsd.lPitch * FILE_HEIGHT;
	LPBYTE lpU1 = lpV1 + ddsd.lPitch  * FILE_HEIGHT / 4;
	int nOffset = DRAW_TOP*FILE_WIDTH+DRAW_LEFT;

	// �����������
	if(lpSurf)
	{
		int i = 0;

		// fill Y data
		lpY += nOffset;
		for(i=0; i<ddsd.dwHeight; i++)
		{
			memcpy(lpSurf, lpY, ddsd.dwWidth);
			lpY += FILE_WIDTH;
			lpSurf += ddsd.lPitch;
		}
		// fill V data
		lpY += DRAW_TOP * FILE_WIDTH / 4 + DRAW_LEFT / 2;
		for(i=0; i<ddsd.dwHeight/2; i++)
		{
			memcpy(lpSurf, lpV, ddsd.dwWidth / 2);
			lpV += FILE_WIDTH / 2;
			lpSurf += ddsd.lPitch / 2;
		}
		// fill U data
		lpU += DRAW_TOP * FILE_WIDTH / 4 + DRAW_LEFT / 2;
		for(i=0; i<ddsd.dwHeight/2; i++)
		{
			memcpy(lpSurf, lpU, ddsd.dwWidth / 2);
			lpU += FILE_WIDTH / 2;
			lpSurf += ddsd.lPitch / 2;
		}
	}

	lpDDSOffScr->Unlock(NULL);

	delete lpYV12;
	// Blt����������
	rctSour.left = 0;
	rctSour.top = 0;
	rctSour.right = ddsd.dwWidth;
	rctSour.bottom = ddsd.dwHeight;
	GetClientRect(hWnd,&rctDest);
	ClientToScreen(hWnd, (LPPOINT)&rctDest.left);
	ClientToScreen(hWnd, (LPPOINT)&rctDest.right);
	ddRval = lpDDSPrimary->Blt(&rctDest, lpDDSOffScr, &rctSour, DDBLT_WAIT, NULL);
	while(ddRval == DDERR_WASSTILLDRAWING);
	if(ddRval != DD_OK)
		return FALSE;

	// �ͷ�DirectDraw����
	if(lpDD != NULL)
	{
		if(lpDDSPrimary != NULL)
		{
			lpDDSPrimary->Release();
			lpDDSPrimary = NULL;
		}
		if(lpDDSOffScr != NULL)
		{
			lpDDSOffScr->Release();
			lpDDSOffScr = NULL;
		}
		lpDD->Release();
		lpDD = NULL;
	}
	return TRUE;
}