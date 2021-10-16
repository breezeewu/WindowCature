// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <tchar.h>
#include "mhook-lib/mhook.h"
#include <Mmdeviceapi.h>
#include <D3D9.h>
#include "detours.h"
#include <math.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "Winmm.lib")
//#pragma comment(lib, "Ole32.lib")
//#define HOOK_CREATE_D3D9_DEVICE
//#define HOOK_LIBRARY_LIBRARY
//#define HOOK_WAVE_OUT_WRITE
//#define HOOK_CREATE_D3D9_DEVICE
#define HOOK_GDI_BitBlt

#ifdef HOOK_CREATE_D3D9_DEVICE
#include "HookInterfaceFunc.h"
#endif
//#define HOOK_Dll_GET_CLASS_OBJECT
/* wave data block header */
typedef struct wavehdr_tag {
  LPSTR       lpData;                 /* pointer to locked data buffer */
  DWORD       dwBufferLength;         /* length of data buffer */
  DWORD       dwBytesRecorded;        /* used for input only */
  DWORD_PTR   dwUser;                 /* for client's use */
  DWORD       dwFlags;                /* assorted flags (see defines) */
  DWORD       dwLoops;                /* loop control counter */
  struct wavehdr_tag FAR *lpNext;     /* reserved for driver */
  DWORD_PTR   reserved;               /* reserved for driver */
} WAVEHDR, *PWAVEHDR, NEAR *NPWAVEHDR, FAR *LPWAVEHDR;

typedef struct {
  WORD wFormatTag; 
  WORD nChannels; 
  DWORD nSamplesPerSec; 
  DWORD nAvgBytesPerSec; 
  WORD nBlockAlign; 
  WORD wBitsPerSample; 
  WORD cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX; 

typedef __success( return == 0) UINT MMRESULT;
typedef struct HWAVEOUT__ *HWAVEOUT, *LPHWAVEOUT;
typedef MMRESULT (WINAPI *PFN_Hook_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI *PFN_Hook_waveOutOpen)(LPHWAVEOUT phwo,  UINT uDeviceID,  LPWAVEFORMATEX pwfx,  DWORD dwCallback, DWORD dwInstance,  DWORD fdwOpen);
typedef LONG    (WINAPI   *pfnDllGetClassObject)(REFCLSID , REFIID, LPVOID *);
typedef HRESULT (WINAPI *PENHook_Direct3DCreate9Ex)( /*_In_*/   UINT SDKVersion,  /*_Out_*/  IDirect3D9Ex **ppD3D);
typedef HRESULT (WINAPI *PDirect3DCreateDevice)(IDirect3D9Ex * pthis, THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface);

typedef IDirect3D9*   (WINAPI* PENHook_Direct3DCreate9)(UINT SDKVersion);

typedef HMODULE (WINAPI *pfnLoadLibraryA)(LPCSTR lpFileName);
typedef HMODULE (WINAPI *pfnLoadLibraryW)(LPCWSTR lpFileName);
typedef BOOL (WINAPI *pfnBitBlt)(HDC hdcDest,int nXDest,int nYDest,int nWidth,int nHeight,HDC hdcSrc,int nXSrc,int nYSrc,DWORD dwRop);
HHOOK g_hHook;
HMODULE g_MyModule;
PFN_Hook_waveOutWrite g_Hook_waveOutWrite = NULL;
PFN_Hook_waveOutOpen g_Hook_waveOutOpen	  = NULL;
pfnDllGetClassObject  g_Hook_DllGetClassObject = NULL;
PENHook_Direct3DCreate9Ex g_Hook_Direct3DCreate9Ex = NULL;
PENHook_Direct3DCreate9	  g_Hook_Direct3DCreate9   = NULL;
PDirect3DCreateDevice	  g_Hook_Direct3DCreateDevice	= NULL;
pfnLoadLibraryA	  g_HookLoadLibraryA = NULL;
pfnLoadLibraryW   g_HookLoadLibraryW = NULL;
HMODULE	g_hHdm = NULL;
IDirect3D9Ex*	g_pD3D9ex = NULL;
IDirect3D9*		g_pD3D9 = NULL;

pfnBitBlt			g_Real_BitBlt	= NULL;
IMMDeviceEnumerator* g_pIMMDeviceEnumerator = NULL;
//视频增强//////
unsigned char GammaTable[256];
void RGB32toYV12SSE2(BYTE *src,   BYTE *dst, int srcrowsize,int destrowsize, int width, int height) ;
void YV12toRGB32MMX(BYTE *src,BYTE *dst,int width,int height);
void EnhanceYV12(BYTE *src,int pitchY,int pitchUV,int width,int height,int len);
void Gamma();
double GetGammaAdjustedValue(double Input, double Gamma);

#ifdef HOOK_Dll_GET_CLASS_OBJECT
LONG Hook_DllGetClassObject(REFCLSID clsid, REFIID iid, LPVOID * pvoid)
{
	//MessageBox(0,L"哈哈，Hook_DllGetClassObject hook成功了",0,0);
	//
	if(g_Hook_DllGetClassObject)
	{
		LONG ret =  g_Hook_DllGetClassObject(clsid, iid, pvoid);
		if(iid == __uuidof(IMMDeviceEnumerator) && *pvoid)
		{
			MessageBox(0,L"哈哈，Hook_DllGetClassObject hook成功了",0,0);
			/*OutputDebugString(L"clsid == __uuidof(IMMDeviceEnumerator)*******************\n");
			g_pIMMDeviceEnumerator = static_cast<IMMDeviceEnumerator*>(*pvoid);
			g_pIMMDeviceEnumerator->AddRef();*/
		}
		return ret;
	}
	return -1;
}
#endif

#ifdef HOOK_WAVE_OUT_WRITE
MMRESULT WINAPI Hook_waveOutWrite(
                      HWAVEOUT hwo,
                      LPWAVEHDR pwh,
                      UINT cbwh
                      )
{
	DWORD id = GetCurrentProcessId();
	MessageBox(NULL,L"Hook_waveOutWrite hook succeeded\n",0,0);
	if(pwh)
	{
		wchar_t str[100];
		swprintf(str, L"********Audio hwo:%d, Len:%d, processid:%d\n",hwo, pwh->dwBufferLength, id);
		OutputDebugString(str);
	}
	if(g_Hook_waveOutWrite)
	{
		return g_Hook_waveOutWrite(hwo, pwh, cbwh);
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

	if(g_Hook_waveOutOpen)
	{
		return g_Hook_waveOutOpen(phwo,uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
	}
}
#endif
#include <gdiplus.h>  
using namespace Gdiplus;  
#pragma comment(lib, "gdiplus.lib")  
#ifdef HOOK_GDI_BitBlt
#include "..\VideoCapture\VideoEnhance.h"
#include "..\include\IMediaPost.h"

IVideoEnhance* g_pIVideoEnhance		= NULL;//new CVideoEnhance();
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)  
{  
  UINT  num = 0;          // number of image encoders  
  UINT  size = 0;         // size of the image encoder array in bytes  

  ImageCodecInfo* pImageCodecInfo = NULL;  
  
  Gdiplus::GetImageEncodersSize(&num, &size);  
  if(size == 0)  
    return -1;  // Failure  
  
  pImageCodecInfo = (ImageCodecInfo*)(malloc(size));  
  if(pImageCodecInfo == NULL)  
    return -1;  // Failure  
 
  Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);  
  
  for(UINT j = 0; j < num; ++j)  
  {  
    if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )  
    {  
      *pClsid = pImageCodecInfo[j].Clsid;  
      free(pImageCodecInfo);  
      return j;  // Success  
    }      
  }  
  
  free(pImageCodecInfo);  
  return -1;  // Failure  
}

static BOOL WINAPI BitmapToFile(HBITMAP hBitmap,  LPCTSTR lpFile)
	{
		HDC                hDC;
		int                iBits;
		WORD               wBitCount;
		BITMAP             Bitmap;
		BITMAPFILEHEADER   bmfHdr;
		BITMAPINFOHEADER   bi;
		LPBITMAPINFOHEADER lpbi;
		HPALETTE           hOldPal = NULL;
		HANDLE             fh, hDib, hPal;
		DWORD              dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;

		hDC = CreateDC(_T("DISPLAY"), 0, 0, 0);
		iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
		DeleteDC(hDC);

		if(iBits <= 1)       wBitCount =  1;
		else if(iBits <=  4) wBitCount =  4;
		else if(iBits <=  8) wBitCount =  8;
		else if(iBits <= 24) wBitCount = 24;
		else                 wBitCount = 24;

		if(wBitCount <= 8) dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
		bi.biSize        = sizeof(BITMAPINFOHEADER);
		bi.biWidth       = Bitmap.bmWidth;
		bi.biHeight      = Bitmap.bmHeight;
		bi.biPlanes      = 1;
		bi.biBitCount    = wBitCount;
		bi.biCompression = BI_RGB;
		bi.biSizeImage   = bi.biXPelsPerMeter = bi.biYPelsPerMeter = 0;
		bi.biClrUsed     = bi.biClrImportant = 0;
		dwBmBitsSize     = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

		if(!(hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER))))
			return FALSE;

		*(lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib)) = bi;

		if((hPal = GetStockObject(DEFAULT_PALETTE)))
		{
			hDC = GetDC(0);
			hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
			RealizePalette(hDC);
		}
		GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

		if(hOldPal)
		{
			SelectPalette(hDC, hOldPal, TRUE);
			RealizePalette(hDC);
			ReleaseDC(0, hDC);
		}

		fh = CreateFile(lpFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
		if(fh != INVALID_HANDLE_VALUE)
		{
			bmfHdr.bfType = 0x4D42;
			dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
			bmfHdr.bfSize = dwDIBSize;
			bmfHdr.bfReserved1 = bmfHdr.bfReserved2 = 0;
			bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
			WriteFile(fh, &bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
			WriteFile(fh, lpbi, dwDIBSize, &dwWritten, NULL);
			memset(lpbi+0x100,100,  Bitmap.bmWidth *Bitmap.bmHeight*2);
			FlushFileBuffers(fh);
			CloseHandle(fh);
		}

		GlobalUnlock(hDib);
		GlobalFree(hDib);
		return TRUE;
	}
//#define WRITE_BITMAP_FILE
#define MODIFY_BITMAP_DATA
BYTE *pYV12 = NULL;
int len = 2;
int m_bmpW = 0;
int m_bmpP = 0;
#define   WIDTHBYTES_LIB(bits)         (((bits)   +   31)   /   32   *   4) 
BOOL WINAPI MyBitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
	/*if(g_Real_BitBlt)
	{
		return g_Real_BitBlt(hdcDest,  nXDest,  nYDest,  nWidth,  nHeight,  hdcSrc,  nXSrc,  nYSrc,  SRCCOPY);
	}*/
	
	HWND hwnd = WindowFromDC(hdcDest);
	wchar_t* pclassname = new wchar_t[MAX_PATH];
	if(hwnd)
	{
		
		memset(pclassname, 0, sizeof(wchar_t) * MAX_PATH);
		GetClassName(hwnd, pclassname, MAX_PATH);
		OutputDebugString(pclassname);
		if(wcscmp(L"NativeWindowClass", pclassname) != 0 && wcscmp(L"Chrome_RenderWidgetHostHWND", pclassname) != 0
 &&wcscmp(L"ShockwaveFlashFullScreen", pclassname) != 0 &&  g_Real_BitBlt)
		{
			return g_Real_BitBlt(hdcDest,  nXDest,  nYDest,  nWidth,  nHeight,  hdcSrc,  nXSrc,  nYSrc,  SRCCOPY);
		}
	}
	else
	{
		return g_Real_BitBlt(hdcDest,  nXDest,  nYDest,  nWidth,  nHeight,  hdcSrc,  nXSrc,  nYSrc,  SRCCOPY);
	}
	
	static int n = 0;
	if(hwnd && nWidth > 500 && nHeight > 300/* && n++%10 == 0*/)
	{
		wchar_t str[100];
		swprintf(str, L"MyBitBlt:nWidth%d, nHeight%d, hwnd:%u, nXDest%d, nYDest:%d\n", nWidth, nHeight, hwnd, nXDest, nYDest);
		OutputDebugString(str);

	//MessageBox(0,L"MyBitBlt",0,0);
	/*BITMAPINFO bmpInfo = {0};  
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);  
	bmpInfo.bmiHeader.biWidth = nWidth;  
	bmpInfo.bmiHeader.biHeight = nHeight;  
	bmpInfo.bmiHeader.biPlanes = 1;  
	bmpInfo.bmiHeader.biBitCount = 24;*/
#ifdef WRITE_BITMAP_FILE
	//HBITMAP hbmp2 =(HBITMAP)LoadImage(NULL, _T("d:\\5.bmp"), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
	////HDC bmpdc    = ::CreateCompatibleDC(hdcSrc);
	//::SelectObject(hdcSrc, hbmp2);
	//::DrawState(hdcSrc,NULL,NULL,(LPARAM)hbmp2,0,0,0,0,0,DST_BITMAP);
	HDC memDC    = ::CreateCompatibleDC(hdcSrc);
	HBITMAP hbmp = ::CreateCompatibleBitmap(hdcSrc, nWidth, nHeight);
	::SelectObject(memDC, hbmp);
	//BOOL ret = g_Real_BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
	//BOOL ret = g_Real_BitBlt(memDC, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
	BOOL ret = g_Real_BitBlt(memDC,  0,  0,  nWidth,  nHeight,  hdcSrc,  0,  0,  SRCCOPY);
	static int i= 0;
	swprintf(str, L"C:\\test\\test%d.bmp", i++);
	BitmapToFile(hbmp,  str);
	DeleteDC(memDC);	
	//return 1;
	DeleteObject(hbmp);
	//DeleteObject(hbmp2);
	//DeleteDC(bmpdc);
#endif

	//BITMAPINFO BitmapHeader;
	//memset(&bmi, 0, sizeof(BITMAPINFO));
#ifdef MODIFY_BITMAP_DATA
	BYTE              *pData = NULL ; 
	BITMAPINFOHEADER  BitmapHeader = {0}; 
	BitmapHeader.biSize       = sizeof (BITMAPINFOHEADER) ; 
	BitmapHeader.biWidth      = nWidth ; 
	BitmapHeader.biHeight     = nHeight ; 
	BitmapHeader.biPlanes     = 1 ; 
	BitmapHeader.biBitCount   = 32;        
	BitmapHeader.biCompression= BI_RGB;
	BitmapHeader.biSizeImage  = nWidth * nHeight * BitmapHeader.biBitCount/8;
	/*bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = nWidth;
	bmi.bmiHeader.biHeight = nHeight;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biBitCount=24;
	bmi.bmiHeader.biCompression=BI_RGB;
	bmi.bmiHeader.biSizeImage=nWidth*nHeight*3;
	bmi.bmiHeader.biXPelsPerMeter=10000;
	bmi.bmiHeader.biYPelsPerMeter=10000;
	bmi.bmiHeader.biClrUsed=0;
	bmi.bmiHeader.biClrImportant=0;*/
 
	BYTE* ppv01;
	//HDC copyhdc = CreateDC(NULL, NULL, NULL, NULL);//GetDC(NULL);
	HDC bmpdc = ::CreateCompatibleDC(NULL);
	HBITMAP hbmp3 = CreateDIBSection(NULL, (BITMAPINFO *)&BitmapHeader, DIB_RGB_COLORS,(void **)&ppv01, NULL, 0);
	SelectObject(bmpdc, hbmp3);
	BOOL ret = g_Real_BitBlt(bmpdc,  nXDest,  nYDest,  nWidth,  nHeight,  hdcSrc,  nXSrc,  nYSrc,  SRCCOPY);//g_Real_BitBlt(bmpdc,  0,  0,  nWidth,  nHeight,  hdcSrc,  0,  0,  SRCCOPY);//
	/*static int i= 0;
	swprintf(str, L"C:\\test\\test%d.bmp", i++);
	BitmapToFile(hbmp3,  str);*/
	//BITMAPINFOHEADER bmInfoHeader = {0};  
	//bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);  
	//bmInfoHeader.biWidth = nWidth;  
	//bmInfoHeader.biHeight = nHeight;  
	//bmInfoHeader.biPlanes = 1;  
	//bmInfoHeader.biBitCount = 24;  
 // 
	////Bimap file header in order to write bmp file  
	//BITMAPFILEHEADER bmFileHeader = {0};  
	//bmFileHeader.bfType = 0x4d42;  //bmp    
	//bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  
	//bmFileHeader.bfSize = bmFileHeader.bfOffBits + ((bmInfoHeader.biWidth * bmInfoHeader.biHeight) * 3);

	//if(pYV12 == NULL)
	//{
	//	pYV12 = (BYTE*)malloc((nWidth*1000*3)>>1);
	//}
	//m_bmpW = (nWidth>>3)<<3;
	//nHeight = (nHeight>>1) << 1;
	//m_bmpP = WIDTHBYTES_LIB(m_bmpW*32);
	//if(pYV12)
	//{
	//	RGB32toYV12SSE2(ppv01,pYV12, nWidth*4, nWidth, nWidth,nHeight) ;	
	//	EnhanceYV12(pYV12,nWidth,nWidth>>1,nWidth,nHeight,nWidth>>1);
	//	YV12toRGB32MMX(pYV12,ppv01,nWidth,nHeight);
	//	len+=4;
	//	if(len >= nWidth)
	//	{
	//		len = 4;
	//	}

	//	//RGB32toYV12SSE2(ppv01,pYV12, nWidth*4, nWidth, nWidth,nHeight) ;	
	//	//EnhanceYV12(pYV12,nWidth/2,nWidth>>2,nWidth,nHeight);
	//	//YV12toRGB32MMX(pYV12,ppv01,nWidth,nHeight);
	//}

	//MessageBox(NULL,0,0,0);
	//g_pIVideoEnhance = new CVideoEnhance();
	static int i = 0;
	
	if(g_pIVideoEnhance == NULL)
	{
		HMODULE hmd = LoadLibrary(L"VideoCapture1.dll");
		if(hmd)
		{
			typedef HRESULT (*pInitMediaPost)(void** ppMediaPost);
			pInitMediaPost pfunc = (pInitMediaPost)GetProcAddress(hmd, "InitMediaPost");
			IMediaPost* pIMediaPost = NULL;
			if(pfunc)
			{
				pfunc((void**)&pIMediaPost);
			}
			if(pIMediaPost)
			{
				pIMediaPost->CreateVideoEnhanceTask(&g_pIVideoEnhance);
			}
		}
	}

	if(g_pIVideoEnhance)
	{
		g_pIVideoEnhance->EnhaceVideo(ppv01, &BitmapHeader, BitmapHeader.biWidth);
	}
	
	//swprintf(str, L"C:\\test\\test%d.bmp", i++);
	//HANDLE hFile = CreateFile(str,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	//DWORD dwWrite = 0;
	//BITMAPFILEHEADER bmFileHeader = {0};
	//bmFileHeader.bfType = 0x4d42;  //bmp    
	//bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  
	//bmFileHeader.bfSize = bmFileHeader.bfOffBits + ((BitmapHeader.biWidth * BitmapHeader.biHeight) * BitmapHeader.biBitCount/8); ///3=(24 / 8)  
	//WriteFile(hFile,&bmFileHeader,sizeof(BITMAPFILEHEADER),&dwWrite,NULL);  
	//WriteFile(hFile,&BitmapHeader, sizeof(BITMAPINFOHEADER),&dwWrite,NULL);  
	//WriteFile(hFile,ppv01, (BitmapHeader.biWidth * BitmapHeader.biHeight) * BitmapHeader.biBitCount/8,&dwWrite,NULL);  
	//CloseHandle(hFile);
	//memset(ppv01+5000,50 , 80);
/*	HANDLE hd = CreateFile(L"D://test191.bmp",GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD dwRead = 0; 
	BITMAPINFOHEADER bmInfoHeader2;
	BITMAPFILEHEADER bmFileHeader2;
	ReadFile(hd,&bmFileHeader2,sizeof(bmFileHeader2),&dwRead,NULL);
	ReadFile(hd,&bmInfoHeader2,sizeof(bmInfoHeader2),&dwRead,NULL);
	ReadFile(hd,ppv01,(bmInfoHeader.biWidth * bmInfoHeader.biHeight) * 3,&dwRead,NULL);
	CloseHandle(hd);

	static int i = 0;
	
	swprintf(str, L"C:\\test\\test%d.bmp", i++);
	HANDLE hFile = CreateFile(str,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);  
	DWORD dwWrite = 0;  
	WriteFile(hFile,&bmFileHeader,sizeof(BITMAPFILEHEADER),&dwWrite,NULL);  
	WriteFile(hFile,&bmInfoHeader, sizeof(BITMAPINFOHEADER),&dwWrite,NULL);  
	WriteFile(hFile,ppv01, (bmInfoHeader.biWidth * bmInfoHeader.biHeight) * 3,&dwWrite,NULL);  
	CloseHandle(hFile);*/
	
	ret = g_Real_BitBlt(hdcDest,  nXDest,  nYDest,  nWidth,  nHeight,  bmpdc,  nXSrc,  nYSrc,  SRCCOPY);
	DeleteDC(bmpdc);
	DeleteObject(hbmp3);

	return ret;
#endif
	//HDC bmpdc    = ::CreateCompatibleDC(hdcDest);
	/*HBITMAP hbmp3 = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS,(void **)&ppv01, NULL, 0);
	SelectObject(bmpdc, hbmp3);

	g_Real_BitBlt(hdcDest,  0,  0,  nWidth,  nHeight,  hdcSrc,  0,  0,  SRCCOPY);

	
	memset(ppv01+ 100, 100, nWidth*nHeight*2); 
	DeleteDC(bmpdc);    
	DeleteObject(hbmp3);

	/*if(g_Real_BitBlt)
	{
		return g_Real_BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, copyhdc, nXSrc, nYSrc, dwRop);
	}*/
	//HDC bmpdc    = ::CreateCompatibleDC(memDC);
	//::SelectObject(bmpdc, hbmp2);
	//::DrawState(hdcSrc,NULL,NULL,(LPARAM)hbmp2,0,0,0,0,0,DST_BITMAP);
	//HDC memDC    = ::CreateCompatibleDC(hdcSrc);
	//HBITMAP hbmp = ::CreateCompatibleBitmap(hdcSrc, nWidth,nHeight);
	//::SelectObject(hdcSrc, hbmp);

	
	/*HDC hdcMem = CreateCompatibleDC(hdcSrc);

	BYTE *pData = NULL;  
	HBITMAP hBmp = CreateDIBSection(hdcMem,&bmpInfo,DIB_RGB_COLORS,reinterpret_cast<VOID **>(&pData),NULL,0);

	HGDIOBJ hOldObj = SelectObject(hdcMem, hBmp);*/

	//BOOL ret =g_Real_BitBlt(hdcDest,  0,  0,  nWidth,  nHeight,  bmpdc,  0,  0,  SRCCOPY);


/*
	BITMAPINFOHEADER bmInfoHeader = {0};  
	bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);  
	bmInfoHeader.biWidth = nWidth;  
	bmInfoHeader.biHeight = nHeight;  
	bmInfoHeader.biPlanes = 1;  
	bmInfoHeader.biBitCount = 24;  
  
	//Bimap file header in order to write bmp file  
	BITMAPFILEHEADER bmFileHeader = {0};  
	bmFileHeader.bfType = 0x4d42;  //bmp    
	bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  
	bmFileHeader.bfSize = bmFileHeader.bfOffBits + ((bmInfoHeader.biWidth * bmInfoHeader.biHeight) * 3); ///3=(24 / 8)  


	
	static int i= 0;
  swprintf(str, L"C:\\test\\test%d.bmp", i++);
	HANDLE hFile = CreateFile(str,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);  
	DWORD dwWrite = 0;  
	WriteFile(hFile,&bmFileHeader,sizeof(BITMAPFILEHEADER),&dwWrite,NULL);  
	WriteFile(hFile,&bmInfoHeader, sizeof(BITMAPINFOHEADER),&dwWrite,NULL);  
	WriteFile(hFile,pData, (bmInfoHeader.biWidth * bmInfoHeader.biHeight) * 3,&dwWrite,NULL);  
	CloseHandle(hFile);*/
	}
	int iWidth = GetDeviceCaps(hdcDest,HORZRES);  
	int iHeight = GetDeviceCaps(hdcDest,VERTRES);  
	int iWidth2 = GetDeviceCaps(hdcSrc,HORZRES);  
	int iHeight2 = GetDeviceCaps(hdcSrc,VERTRES);  

	RECT rect;
	if(hwnd)
	{
		GetWindowRect(hwnd,&rect);
	}
	/* HDC hMemDC = CreateCompatibleDC(hdcSrc);
	 SelectObject(hdcSrc, hBmp);
	 HBITMAP hRawBitmap = CreateCompatibleBitmap(hdcSrc, nWidth, nHeight);
	 HBITMAP hRawBitmap2 = (HBITMAP) SelectObject(hMemDC, hOldBitmap);
		GetDIBits()*/
	/*BOOL bRet = FALSE;
	//将目标区域贴图到内存BITMAP  
	HDC memDC = CreateCompatibleDC(hdcSrc);
	HBITMAP hBmp = CreateCompatibleBitmap(hdcSrc, nWidth, nHeight);  
	SelectObject(hdcSrc, hBmp);  
	BitBlt(memDC, 0, 0, nWidth, nHeight,  hdcSrc, 0, 0, SRCCOPY);  
  
  //保存成文件  
  {  
   //L"image/bmp" L"image/jpeg"  L"image/gif" L"image/tiff" L"image/png"  
   CLSID pngClsid;  
    GetEncoderClsid(L"image/bmp", &pngClsid);//此处以BMP为例，其它格式选择对应的类型，如JPG用L"image/jpeg"   
  static int i= 0;
  swprintf(str, L"C:\\test\\test%d.bmp", i);
    Gdiplus::Bitmap *pbmSrc = Gdiplus::Bitmap::FromHBITMAP(hBmp, NULL);  
    if( pbmSrc->Save(str, &pngClsid) == Ok)  
    {  
      bRet = TRUE;  
    }  
   delete pbmSrc;  
  }  
  
  //清理工作  
  SelectObject(memDC, (HBITMAP)NULL);  
  DeleteDC(memDC);    
  DeleteObject(hBmp);  */

	if(g_Real_BitBlt)
	{
		return g_Real_BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
	}

	return FALSE;
}
#endif
#ifdef HOOK_CREATE_D3D9_DEVICE
#include "PixelShaderCompiler.h"
DefHookDApi(D3D9DevicePresent,
			HRESULT,
			(void *pThis,THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion))
{  
	//OUTPUTSTRING( "Hook call D3DDevicePresent" ) ;

	/*if ( !m_bIsHooking && m_hThreadAutoSave && m_bOpenOverlay )
	{
		m_bIsHooking = TRUE ;
		m_bNotifyShot = FALSE ;
	}

	if ( m_bNotifyShot && m_bOpenOverlay )
	{
		m_nBackBufferHeight = pDestRect->bottom - pDestRect->top ;
		m_nBackBufferWidth = pDestRect->right - pDestRect->left ;

		ScreenShot( (IDirect3DDevice9*)pThis ) ;

		m_bNotifyShot = FALSE ;
	}*/

	//MessageBox(NULL,L"D3D9DevicePresent hook succeeded\n",0,0);
	OutputDebugString(L"D3D9DevicePresent************************************\n");
	return Real_D3D9DevicePresent(pThis, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion) ;
}

DefHookDApi(D3D9DevicePresentEx,
			HRESULT,
			(void *pThis,CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags))
{  
	//OUTPUTSTRING( "Hook call D3DDevicePresent" ) ;

	/*if ( !m_bIsHooking && m_hThreadAutoSave && m_bOpenOverlay )
	{
		m_bIsHooking = TRUE ;
		m_bNotifyShot = FALSE ;
	}

	if ( m_bNotifyShot && m_bOpenOverlay )
	{
		m_nBackBufferHeight = pDestRect->bottom - pDestRect->top ;
		m_nBackBufferWidth = pDestRect->right - pDestRect->left ;

		ScreenShot( (IDirect3DDevice9*)pThis ) ;

		m_bNotifyShot = FALSE ;
	}*/

	//MessageBox(NULL,L"D3D9DevicePresent hook succeeded\n",0,0);
	OutputDebugString(L"D3D9DevicePresentEx************************************\n");
	return Real_D3D9DevicePresentEx(pThis, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags) ;
}
CComPtr<IDirect3DPixelShader9> m_pPixelShader;
DefHookDApi(D3D9DeviceSetPixelShader,			
			HRESULT,
			(void *pThis, THIS_ IDirect3DPixelShader9* pShader))
{
	//MessageBox(NULL,L"D3D9DeviceSetPixelShader hook succeeded\n",0,0);
	//OUTPUTSTRING( "Hook call D3DDevicePresent" ) ;

	/*if ( !m_bIsHooking && m_hThreadAutoSave && m_bOpenOverlay )
	{
		m_bIsHooking = TRUE ;
		m_bNotifyShot = FALSE ;
	}

	if ( m_bNotifyShot && m_bOpenOverlay )
	{
		m_nBackBufferHeight = pDestRect->bottom - pDestRect->top ;
		m_nBackBufferWidth = pDestRect->right - pDestRect->left ;

		ScreenShot( (IDirect3DDevice9*)pThis ) ;

		m_bNotifyShot = FALSE ;
	}*/
	

	if(m_pPixelShader == NULL)
	{
		//MessageBox(NULL,L"D3D9DeviceSetPixelShader hook succeeded\n",0,0);
		CPixelShaderCompiler::GetInstance()->LoadPixelShader(L"Enhance.txt");
		CPixelShaderCompiler::GetInstance()->CompileShader((IDirect3DDevice9*)pThis, NULL, "main", "ps_3_0",0, &m_pPixelShader);
	}

	IDirect3DDevice9* pd3ddev = (IDirect3DDevice9*)pThis;
	static float fEnhancePos = 0.0, offset = 0.01;
	float fConstData[] = {0.0, 0.0, 0.0, 0.0};
	if(fEnhancePos < 1)
	{
		fEnhancePos += offset;
		
	}
	else
	{	
		fEnhancePos = 0;
	}
	
	fConstData[0] = fEnhancePos;
	HRESULT hr = pd3ddev->SetPixelShaderConstantF(0, (float*)fConstData, sizeof(fConstData)/sizeof(fConstData[0]));
	OutputDebugString(L"Real_D3D9DeviceSetPixelShader************************************\n");
	if(pShader == NULL)
	{
		//Real_D3D9DeviceSetPixelShader(pThis, NULL) ;
		pShader = m_pPixelShader;
	}
	return Real_D3D9DeviceSetPixelShader(pThis, pShader) ;
}  
/*
typedef HRESULT (*PD3D9CreateDevice)(void *pThis,UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface);
	PD3D9CreateDevice Real_D3D9CreateDevice		= NULL;
	BOOL HookD3D9CreateDeviceTag				= FALSE;
	HRESULT Mine_D3D9CreateDevice(void *pThis,UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
*/
DefHookDApi(D3D9CreateDevice,
			HRESULT,
			(void *pThis,UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface))
{ 
	HRESULT hr = Real_D3D9CreateDevice(pThis, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);  

	MessageBox(NULL,L"D3D9CreateDevice hook succeeded\n",0,0);
	if ( hr == D3D_OK && *ppReturnedDeviceInterface )  
	{
		//OUTPUTSTRING( "Hook call D3DCreateDevice" ) ;

		PROC p = NULL ;
		p=*(PROC*)(*((DWORD*)*ppReturnedDeviceInterface) + 107*4 ) ;
		HookDApi(D3D9DeviceSetPixelShader, p) ;

// 		p=*(PROC*)(*((DWORD*)*ppReturnedDeviceInterface) + 13*4 ) ;
// 		HookDApi(D3D9CreateAdditionalSwapChain,p) ;

		
		TrampolineWith();

		//创建线程，等待通知截图
		//StartupD3D9SaveThread() ;
	}

	return hr;
}

DefHookDApi(D3D9CreateDeviceEx,
			HRESULT,
			(void *pThis, UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX* pFullscreenDisplayMode,IDirect3DDevice9Ex** ppReturnedDeviceInterface))
{ 
	HRESULT hr = Real_D3D9CreateDeviceEx(pThis, Adapter, DeviceType,hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);  

	//MessageBox(NULL,L"D3D9CreateDeviceEx hook succeeded\n",0,0);
	if ( hr == D3D_OK && *ppReturnedDeviceInterface )  
	{
		//OUTPUTSTRING( "Hook call D3DCreateDevice" ) ;

		if(!HookD3D9DeviceSetPixelShaderTag)
		{
			//Hook pixelshader
			PROC p = NULL ;
			p=*(PROC*)(*((DWORD*)*ppReturnedDeviceInterface) + 107*4 ) ;

			/*HookD3D9DeviceSetPixelShaderTag=TRUE;

			Real_D3D9DeviceSetPixelShader = (PD3D9DeviceSetPixelShader)DetourFunction((PBYTE)p,(PBYTE)Mine_D3D9DeviceSetPixelShader);
			AddHookDApi((PBYTE)Real_D3D9DeviceSetPixelShader,(PBYTE)Mine_D3D9DeviceSetPixelShader);*/

		
			HookDApi(D3D9DeviceSetPixelShader, p) ;

			//Hook D3D9DevicePresent
			p=*(PROC*)(*((DWORD*)*ppReturnedDeviceInterface) + 17*4 ) ;


		
			HookDApi(D3D9DevicePresent, p) ;

			IDirect3DDevice9Ex* pdvc = *ppReturnedDeviceInterface;

			pdvc->Present(0,0,0,0);
			//pdvc->SetPixelShader(NULL);
			//hook D3D9DevicePresent
			p=*(PROC*)(*((DWORD*)*ppReturnedDeviceInterface) + 121*4 ) ;
			
			Real_D3D9DevicePresentEx = (PD3D9DevicePresentEx)DetourFunction((PBYTE)p,(PBYTE)Mine_D3D9DevicePresentEx);
			AddHookDApi((PBYTE)Real_D3D9DevicePresentEx,(PBYTE)Mine_D3D9DevicePresentEx);

			pdvc->PresentEx(0,0,0,0,0);
			//HookDApi(D3D9DevicePresent, p) ;
		}

// 		p=*(PROC*)(*((DWORD*)*ppReturnedDeviceInterface) + 13*4 ) ;
// 		HookDApi(D3D9CreateAdditionalSwapChain,p) ;

		
		TrampolineWith();

		//创建线程，等待通知截图
		//StartupD3D9SaveThread() ;
	}

	return hr;
}

IDirect3D9*   WINAPI Hook_Direct3DCreate9(UINT SDKVersion)
{
	OutputDebugString(_T("hook Direct3DCreate9**********************\n"));
	//Direct3DCreate9Ex API加载成功
	if(g_Hook_Direct3DCreate9 == NULL)
	{
		return NULL;
	}

	IDirect3D9* pD3D = NULL;
	pD3D = g_Hook_Direct3DCreate9(SDKVersion);

	PROC p = NULL ;
	p=*(PROC*)(*((DWORD*)pD3D) + 16*4 ) ;
	HookDApi(D3D9CreateDevice,p) ;

	TrampolineWith();
	/*
	if(pD3D)
	{
		g_pD3D9 = pD3D;
		g_pD3D9->AddRef();
		OutputDebugString(_T("Get g_pD3D9 pointer**********************\n"));
	}*/
	return pD3D;
}
HRESULT WINAPI Mine_Direct3DCreateDevice(IDirect3D9Ex * pthis, THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
{
	return g_Hook_Direct3DCreateDevice(pthis, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
}

BYTE D3D9CreateDevice_begin[5];
void * pCreateDevice = NULL;//IDirect3D9::CreateDevice函数地址指针
void * pCreateDeviceEx = NULL;//IDirect3D9::CreateDeviceEx函数地址指针
HRESULT WINAPI Hook_Direct3DCreate9Ex( /*_In_*/   UINT SDKVersion,  /*_Out_*/  IDirect3D9Ex **ppD3D)
{
	//MessageBox(NULL,L"Hook_Direct3DCreate9Ex hook succeeded\n",0,0);
	
	DWORD pid = GetCurrentProcessId();
	wchar_t str[100];
	swprintf(str, L"hook Direct3DCreate9Ex, pid:%d **********************\n", pid);
	OutputDebugString(str);
	//Direct3DCreate9Ex API加载成功
	if(g_Hook_Direct3DCreate9Ex == NULL)
	{
		return E_FAIL;
	}
	*ppD3D = NULL;
	HRESULT ret = g_Hook_Direct3DCreate9Ex(SDKVersion,  ppD3D);
	//return ret;
	PROC p = NULL ;
	IDirect3D9Ex* pd3dex = *ppD3D;
	IDirect3D9* pd3d = *ppD3D;

	pCreateDevice =(void*)*(DWORD*)(*((DWORD*)pd3dex)+16*4);////(PD3D9CreateDevice)*(PROC*)(*((DWORD*)ppD3D) + 15*4 ) ;//
	pCreateDeviceEx = (void*)*(DWORD*)(*((DWORD*)pd3dex)+20*4);
/*
		LPDIRECT3DDEVICE9   pd3dDevice = NULL;
	D3DPRESENT_PARAMETERS d3dpp;  
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = TRUE;    //不是全屏   
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT hr = pd3dex->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);
	if(*ppD3D)
	{
		g_pD3D9ex = *ppD3D;
		//g_pD3D9ex->AddRef();
		OutputDebugString(_T("iexplorer Get g_pD3D9ex pointer**********************\n"));
	}*/

	DWORD dwOldFlag = 0;
	//HookDApi(D3D9CreateDevice, pCreateDevice);
	/*VirtualProtect( pCreateDevice, 5, PAGE_EXECUTE_READWRITE, &dwOldFlag);
	memcpy(D3D9CreateDevice_begin, pCreateDevice, 5);
	*(BYTE*)pCreateDevice = 0xe9;
	*(DWORD*)((BYTE*)pCreateDevice+1)=(DWORD)Mine_D3D9CreateDevice-(DWORD)pCreateDevice-5;
	*(PROC*)(*((DWORD*)pCreateDevice) + 15*4 ) = (PROC)Mine_D3D9CreateDevice;
	 VirtualProtect( pCreateDevice, 5, dwOldFlag, &dwOldFlag);*/

	//BOOL rt = Mhook_SetHook((PVOID*)&Real_D3D9CreateDevice, Mine_D3D9CreateDevice);
	//if(rt)
	//{
	//	OutputDebugString(L"g_Hook_Direct3DCreate9Ex get SUCCEEDED********\n");
	//	//MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
	//}
	//return S_OK;
	int i = 0;
	//p=*(PROC*)((*((DWORD*)ppD3D)+64)) ;
	//HookDApi(D3D9CreateDevice,p) ;

	if(!HookD3D9CreateDeviceTag)
	{

		HookD3D9CreateDeviceTag=TRUE;

		Real_D3D9CreateDevice = (PD3D9CreateDevice)DetourFunction((PBYTE)pCreateDevice,(PBYTE)Mine_D3D9CreateDevice);
		AddHookDApi((PBYTE)Real_D3D9CreateDevice,(PBYTE)Mine_D3D9CreateDevice);

		HookDApi(D3D9CreateDeviceEx,pCreateDeviceEx) ;
		/*Real_D3D9CreateDeviceEx = (PD3D9CreateDeviceEx)DetourFunction((PBYTE)pCreateDeviceEx,(PBYTE)Mine_D3D9CreateDeviceEx);
		AddHookDApi((PBYTE)Real_D3D9CreateDeviceEx,(PBYTE)Mine_D3D9CreateDeviceEx);*/
	}

	TrampolineWith();
	return ret;
}
#endif

#ifdef HOOK_LIBRARY_LIBRARY
HMODULE WINAPI Hook_LoadLibraryA( /*_In_*/  LPCSTR lpFileName)
{
	//MessageBox(0,0,0,0);
	HMODULE hdm = g_HookLoadLibraryA(lpFileName);
	if(strstr(lpFileName, "MMDevAPI.dll"))
	{
		OutputDebugString(L"load MMDevAPI.dll succeeded******************\n");
		//MessageBox(NULL, L"load MMDevAPI.dll!", 0, 0);
		g_hHdm = GetModuleHandleA(lpFileName);;
		g_Hook_DllGetClassObject = (pfnDllGetClassObject)GetProcAddress(g_hHdm, "DllGetClassObject");
	    if(g_Hook_DllGetClassObject)//g_Hook_waveOutWrite)
	    {
			OutputDebugString(L"load MMDevAPI.dll Hook succeeded******************\n");
			 Mhook_SetHook((PVOID*)&g_Hook_DllGetClassObject, Hook_DllGetClassObject);
			 //return 
			// TRUE;
		}
		else
		{
			OutputDebugString(L"load MMDevAPI.dll Hook failed******************\n");
			//return NULL;
		}
	}

	return hdm;
}

HMODULE WINAPI Hook_LoadLibraryW( /*_In_*/  LPCWSTR lpFileName)
{
	//MessageBox(0,0,0,0);
	HMODULE hdm = g_HookLoadLibraryW(lpFileName);
	if(wcsstr((wchar_t*)lpFileName, L"MMDevAPI.dll"))
	{
		OutputDebugString(L"load MMDevAPI.dll succeeded******************\n");
		//MessageBox(NULL, L"load MMDevAPI.dll!", 0, 0);
		g_hHdm = GetModuleHandleW(lpFileName);;
		g_Hook_DllGetClassObject = (pfnDllGetClassObject)GetProcAddress(g_hHdm, "DllGetClassObject");
	    if(g_Hook_DllGetClassObject)//g_Hook_waveOutWrite)
	    {
			OutputDebugString(L"load MMDevAPI.dll Hook succeeded******************\n");
			BOOL ret = Mhook_SetHook((PVOID*)&g_Hook_DllGetClassObject, Hook_DllGetClassObject);
			 //return 
			// TRUE;
		}
		else
		{
			OutputDebugString(L"load MMDevAPI.dll Hook failed******************\n");
			//return NULL;
		}
	}

	return hdm;
}
#endif
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

    if(_tcsnicmp(lpFileVName, _T("yfe.exe"), _tcslen(lpFileVName)) == 0)//KWExplorer.exe
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

BOOL HookFunction()
{

#ifdef HOOK_Dll_GET_CLASS_OBJECT
	g_Hook_DllGetClassObject = (pfnDllGetClassObject)GetProcAddress(GetModuleHandleA("Ole32.dll"), "DllGetClassObject");
	if(g_Hook_DllGetClassObject)
	{
		   BOOL ret = Mhook_SetHook((PVOID*)&g_Hook_DllGetClassObject, Hook_DllGetClassObject);
			  if(ret)
			  {
				  OutputDebugString(L"g_Hook_DllGetClassObject get SUCCEEDED********\n");
				  //MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
			  }
	}
	else
	{
		OutputDebugString(L"g_Hook_DllGetClassObject get failed********\n");
		return FALSE;
	}
#endif

#ifdef HOOK_CREATE_D3D9_DEVICE
	//MessageBox(NULL,L"Real_D3D9CreateDevice set hook\n",0,0);
	Real_D3D9CreateDevice = (PD3D9CreateDevice)DetourFindFunction("D3d9.dll", "CreateDevice");
	g_Hook_Direct3DCreate9Ex = (PENHook_Direct3DCreate9Ex)DetourFindFunction("D3d9.dll", "Direct3DCreate9Ex");

	if(Real_D3D9CreateDevice)
	{
		BOOL ret = Mhook_SetHook((PVOID*)&Real_D3D9CreateDevice, Mine_D3D9CreateDevice);
		if(ret)
		{
			OutputDebugString(L"g_Hook_Direct3DCreate9Ex get SUCCEEDED********\n");
			//MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
		}
	}

	if(g_Hook_Direct3DCreate9Ex)
	{
		   BOOL ret = Mhook_SetHook((PVOID*)&g_Hook_Direct3DCreate9Ex, Hook_Direct3DCreate9Ex);
			  if(ret)
			  {
				  OutputDebugString(L"g_Hook_Direct3DCreate9Ex get SUCCEEDED********\n");
				  //MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
			  }
	}
	else
	{
		OutputDebugString(L"g_Hook_Direct3DCreate9Ex get failed********\n");
		return FALSE;
	}
#endif
#ifdef HOOK_GDI_BitBlt
g_Real_BitBlt = (pfnBitBlt)DetourFindFunction("GDI32.dll", "BitBlt");
if(g_Real_BitBlt)
{
	  BOOL ret = Mhook_SetHook((PVOID*)&g_Real_BitBlt, MyBitBlt);
	  if(ret)
	  {
		 // MessageBox(0,0,0,0);
		  OutputDebugString(L"g_Real_BitBlt get SUCCEEDED********\n");
		  //MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
	  }
}
#endif
#ifdef HOOK_WAVE_OUT_WRITE

	g_Hook_waveOutOpen = (PFN_Hook_waveOutOpen)DetourFindFunction("Winmm.dll", "waveOutOpen");
	g_Hook_waveOutWrite = (PFN_Hook_waveOutWrite)DetourFindFunction("Winmm.dll", "waveOutWrite");
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
#endif

#ifdef HOOK_LIBRARY_LIBRARY
	g_HookLoadLibraryA = (pfnLoadLibraryA)GetProcAddress(g_hHdm, "LoadLibraryA");
	g_HookLoadLibraryW = (pfnLoadLibraryW)GetProcAddress(g_hHdm, "LoadLibraryW");
	if(g_HookLoadLibraryA && g_HookLoadLibraryW)
	{
		  if(g_HookLoadLibraryA)//g_Hook_waveOutWrite)
		  {
			  OutputDebugString(L"g_Hook_Direct3DCreate9Ex get succeeded***\n");
			  BOOL ret = Mhook_SetHook((PVOID*)&g_HookLoadLibraryA, Hook_LoadLibraryA);
			  if(ret)
			  {
				  //MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
			  }
			// TRUE;
		  }
		  if(g_HookLoadLibraryW)//g_Hook_waveOutWrite)
		  {
			  OutputDebugString(L"g_Hook_Direct3DCreate9Ex get succeeded***\n");
			  BOOL ret = Mhook_SetHook((PVOID*)&g_HookLoadLibraryW, Hook_LoadLibraryW);
			  if(ret)
			  {
				  //MessageBox(NULL,L"LoadLibraryW set hook succeeded\n",0,0);
			  }
			// TRUE;
		  }
		  return TRUE;
	}
	else
	{
		OutputDebugString(L"g_Hook_Direct3DCreate9Ex get failed********\n");
		return FALSE;
	}
#endif
	return FALSE;
}

BOOL UnHookFunction()
{

#ifdef  HOOK_CREATE_D3D9_DEVICE
	UnTrampolineWith();
#endif

  if (g_Hook_waveOutWrite != NULL)
  {
		Mhook_Unhook((PVOID*)&g_Hook_waveOutWrite);
  }

  if (g_Hook_waveOutOpen != NULL)
  {
		Mhook_Unhook((PVOID*)&g_Hook_waveOutOpen);
  }

  if (g_Hook_DllGetClassObject != NULL)
  {
		Mhook_Unhook((PVOID*)&g_Hook_DllGetClassObject);
  }

  if (g_Hook_Direct3DCreate9 != NULL)
  {
		Mhook_Unhook((PVOID*)&g_Hook_Direct3DCreate9);
  }

  if (g_HookLoadLibraryA != NULL)
  {
		Mhook_Unhook((PVOID*)&g_HookLoadLibraryA);
  }

  if (g_HookLoadLibraryW != NULL)
  {
		Mhook_Unhook((PVOID*)&g_HookLoadLibraryW);
  }

  if (g_Real_BitBlt != NULL)
  {
		Mhook_Unhook((PVOID*)&g_Real_BitBlt);
  }

  /*if(g_hHdm)
  {
	  FreeLibrary(g_hHdm);
	  g_hHdm = NULL;
  }*/
  return TRUE;
}

LRESULT CALLBACK HookCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{ 
	//OutputDebugString(L"HookCBTProc*****************************\n");
	//if(g_hHook)
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

EXTERN_C __declspec(dllexport) void InstallWindowsHook()
{
  if (g_hHook != NULL)
  {
    return;
  }

	//MessageBox(0,0,L"InstallWindowsHook",0);
	//g_Hook_waveOutOpen = (PFN_Hook_waveOutOpen)DetourFindFunction("Winmm.dll", "waveOutOpen");
	//g_Hook_waveOutWrite = (PFN_Hook_waveOutWrite)DetourFindFunction("Winmm.dll", "waveOutWrite");
	//if(g_Hook_waveOutOpen && g_Hook_waveOutWrite)
	//{
	//      BOOL ret = Mhook_SetHook((PVOID*)&g_Hook_waveOutWrite, Hook_waveOutWrite);
	//      if(ret)
	//	  {
	//		  OutputDebugString(L"g_Hook_waveOutWrite get SUCCEEDED********\n");
	//		  //MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
	//	  }
	//	  ret = Mhook_SetHook((PVOID*)&g_Hook_waveOutOpen, Hook_waveOutOpen);
	//	  if(ret)
	//	  {
	//		  OutputDebugString(L"g_Hook_waveOutOpen get SUCCEEDED********\n");
	//		  //MessageBox(NULL,L"LoadLibraryA set hook succeeded\n",0,0);
	//	  }
	//}
	//else
	//{
	//	OutputDebugString(L"g_Hook_waveOutOpen && g_Hook_waveOutWrite get failed********\n");
	//	return ;
	//}
  
  DWORD id = GetCurrentProcessId();
  wchar_t str[100];
  swprintf(str, L"GetCurrentProcessId:%d*****************\n", id);
	g_hHook =  SetWindowsHookEx(WH_CBT, HookCBTProc, g_MyModule, 0/*GetCurrentThreadId()*/);
#ifdef HOOK_WAVE_OUT_WRITE
	
	g_Hook_waveOutOpen = (PFN_Hook_waveOutOpen)DetourFindFunction("Winmm.dll", "waveOutOpen");
	g_Hook_waveOutWrite = (PFN_Hook_waveOutWrite)DetourFindFunction("Winmm.dll", "waveOutWrite");
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
		return ;
	}
#endif
}

EXTERN_C __declspec(dllexport) void UnInstallWindowsHook()
{
	UnHookFunction();

  if (g_hHook != NULL)
  {
    UnhookWindowsHookEx(g_hHook);
    g_hHook = NULL;
  }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{

	case DLL_PROCESS_ATTACH:
    g_MyModule = hModule;

    if (IsProcess())
	{
		OutputDebugString(L"IsProcess()->HookFunction********\n");
		HookFunction();
    }
	else
	{
		OutputDebugString(L"!IsProcess()->HookFunction failed********\n");
	}
    break;
	case DLL_THREAD_ATTACH:

    break;
	case DLL_THREAD_DETACH:

    break;
	case DLL_PROCESS_DETACH:
    if (IsProcess())
    {
      UnHookFunction();
    }
		break;
	}
	return TRUE;
}

#ifdef HOOK_GDI_BitBlt_
void RGB32toYV12SSE2(BYTE *src,   BYTE *dst, int srcrowsize,int destrowsize, int width, int height) 
{ 
	BYTE   *yp,   *up,   *vp; 
	BYTE   *prow; 
	int   i,   j ; 

	BYTE *y = dst;
	BYTE *u = y +destrowsize * height;
	BYTE *v = u +destrowsize/2 * height/2;

	__declspec(align(16))
		static short int ycoefs[8] = {2851,   22970,   6947,   0, 2851,   22970,   6947,   0 };
	__declspec(align(16))
		static short int ucoefs[8] =   {16384,   -12583,   -3801,   0, 16384,   -12583,   -3801,   0 };
	__declspec(align(16))
		static short int vcoefs[8] = {-1802,   -14582,   16384,   0, -1802,   -14582,   16384,   0 };

	_asm   
	{ 
		push eax
		push ebx
		push ecx
		push edx
		push edi
		push esi

		xor    edx,   edx 
		mov    eax,   width 
		sar    eax,2 
		cmp    edx,   eax 
		jge    yuvexit 

		mov    j,   eax 
		mov    eax,   height 

		mov    i,   eax 
		cmp    edx,   eax 
		jge    yuvexit 

		mov    eax,   y 
		mov    yp,   eax 
		mov    eax,   u 
		mov    up,   eax 
		mov    eax,   v 
		mov    vp,   eax 
		mov    eax,   src 
		mov    prow,   eax 
		pxor    xmm7,   xmm7 
		mov    eax,   i 

heighttop:

		mov    i,   eax 
		mov    edi,   j 
		mov    ebx,   prow 
		mov    ecx,   yp 
		mov    edx,   up 
		mov    esi,   vp 

widthtop: 
		movdqu   xmm0,   [ebx]     //   MM5   has   0   r2   g2   b2   0   r1   g1   b1,   4   pixels  
		movq      xmm5,   xmm0 // xmm1  = 00..00 g3 b3 r2 g2 b2 r1 g1 b1
		pshufd     xmm0 ,xmm0 ,0x4e
		movq       xmm4,xmm0
		add           ebx,   16 

		punpcklbw   xmm5,   xmm7   //   MM5   has   0   r2   g2   b2 0   r1   g1   b1 
		punpcklbw   xmm4,   xmm7   //   MM4   has  0   r4   g4   b4 0   r3   g3   b3

		movdqa      xmm0,  xmm5 
		movdqa      xmm1,   xmm4 

		pmaddwd   xmm0,   ycoefs   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movdqa      xmm2,   xmm0 
		PSRLQ     xmm2,   32 
		paddd     xmm0,   xmm2       //   MM0   has   y1 y2  in   lower   32   bits 
		pmaddwd   xmm1,   ycoefs   //   MM1   has   r2*cr   and   g2*cg+b2*cb 
		movdqa      xmm2,   xmm1 
		PSRLQ    xmm2,   32 
		paddd     xmm1,   xmm2       //   MM1   has   y3 y4   in   lower   32   bits 

		movd      eax,  xmm0 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 

		pshufd     xmm0 ,xmm0 ,0x4e
		movd      eax,  xmm0 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 

		movd      eax,   xmm1 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 

		pshufd     xmm1 ,xmm1 ,0x4e
		movd      eax,  xmm1 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 

		test  i, 0x00000001
		jnz  L1


		movdqa      xmm0,   xmm5 
		movdqa      xmm1,   xmm4 

		pmaddwd   xmm0,   ucoefs   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movdqa      xmm2,  xmm0 
		PSRLQ     xmm2,   32 
		paddd     xmm0,   xmm2       //   MM0   has   u1   in   lower   32   bits 


		movd      eax,   xmm0 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [edx],   al 
		inc       edx 
        
		movdqa      xmm0,   xmm5 
		pshufd     xmm0 ,xmm0 ,0x4e

		pmaddwd   xmm0,   vcoefs   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movdqa      xmm2,  xmm0 
		PSRLQ     xmm2,   32 
		paddd     xmm0,   xmm2       //   MM0   has   u1   in   lower   32   bits 

		movd      eax,   xmm0 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [esi],   al 
		inc       esi 

		pmaddwd   xmm1,   ucoefs   //   MM5   has   r1*cr   and   g1*cg+b1*cb 
		movdqa     xmm2,   xmm1 
		PSRLQ     xmm2,   32 
		paddd     xmm1,   xmm2       //   MM5   has   v1   in   lower   32   bits 

		movd      eax,   xmm1 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [edx],   al 
		inc       edx 
        
		movdqa      xmm1,   xmm4 
		pmaddwd   xmm1,   vcoefs   //   MM5   has   r1*cr   and   g1*cg+b1*cb 

		movdqa     xmm2,   xmm1 
		PSRLQ     xmm2,   32 
		paddd     xmm1,   xmm2       //   MM5   has   v1   in   lower   32   bits 

       
		movd      eax,   xmm1 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [esi],   al 
		inc       esi 


L1:
		dec       edi 	
		jnz       widthtop 

		mov       eax,   destrowsize 
		add       yp,   eax 
		sar        eax,1

		test  i, 0x00000001
		jnz  L2

		add       up,   eax 
		add       vp,   eax 
L2:
		mov       eax,   srcrowsize 
		add       prow,   eax 
		mov       eax,   i 
		dec       eax 
		jnz       heighttop 

		pop esi
		pop edi
		pop edx
		pop ecx
		pop ebx
		pop eax
yuvexit:
		emms 
	} 
}

void YV12toRGB32MMX(BYTE *src,BYTE *dst,int width,int height)
{
	typedef unsigned __int64  UInt64;
	BYTE *pBmpTemp = dst;
	BYTE *y = src;
	BYTE *v = y +width * height;
	BYTE *u = v +width/2 * height/2;


	//B= 1.164 * (Y - 16) + 2.018 * (U - 128)
	//G= 1.164 * (Y - 16) -   0.38 * (U - 128) - 0.813 * (V - 128)
	//R= 1.164 * (Y - 16)                             + 1.159 * (V - 128)

	//	Y’ = 0.257*R' + 0.504*G' + 0.098*B' + 16
	//	Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128
	//	Cr' = 0.439*R' - 0.368*G' - 0.071*B' + 128
 
	//	R' = 1.164*(Y’-16) + 1.596*(Cr'-128)
	//	G' = 1.164*(Y’-16) - 0.813*(Cr'-128) - 0.392*(Cb'-128)
	//	B' = 1.164*(Y’-16) + 2.017*(Cb'-128)

	//	Y = 0.257*R + 0.504*G + 0.098*B + 16
	//	V'= -0.148*R - 0.291*G + 0.439*B + 128
	//	U = 0.439*R - 0.368*G - 0.071*B + 128

	//	R = 1.164*(Y-16) + 1.596*(U-128)
	//	G = 1.164*(Y-16) - 0.813*(U-128) - 0.392*(V-128)
	//	B = 1.164*(Y-16) + 2.017*(V-128)


	const  UInt64   csMMX_16_b      = 0x1010101010101010; // byte{16,16,16,16,16,16,16,16}
	const  UInt64   csMMX_128_w     = 0x0080008000800080; //short{  128,  128,  128,  128}
	const  UInt64   csMMX_0x00FF_w  = 0x00FF00FF00FF00FF; //掩码
	const  UInt64   csMMX_Y_coeff_w = 0x2543254325432543; //short{ 9539, 9539, 9539, 9539} =1.164383*(1<<13)
	const  UInt64   csMMX_U_blue_w  = 0x408D408D408D408D; //short{16525,16525,16525,16525} =2.017232*(1<<13)
	const  UInt64   csMMX_U_green_w = 0xF377F377F377F377; //short{-3209,-3209,-3209,-3209} =(-0.391762)*(1<<13)
	const  UInt64   csMMX_V_green_w = 0xE5FCE5FCE5FCE5FC; //short{-6660,-6660,-6660,-6660} =(-0.812968)*(1<<13)
	const  UInt64   csMMX_V_red_w   = 0x3313331333133313; //short{13075,13075,13075,13075} =1.596027*(1<<13)
	//const  unsigned __int64   csMMX_V_red_w   = 0x2516251625162516; //short{9494,9494,9494,9494} =1.1596027*(1<<13)

	int nLoop = width / 8;
	int p2 = width / 2; 

	//算法核心  把参数放大（1<<13倍）  Y  U  V 放大（1<3)倍。在两字节内运行完成。再通过交换，得到所要求的顺序

	for (int i = 0; i < height; ++i)
	{
		BYTE *pSrcY = y + i * width;
		BYTE *pSrcU = u + (i/2) * p2;
		BYTE *pSrcV = v + (i/2) * p2;
		BYTE *pDst = pBmpTemp + width*4 * i;

	__asm{
		mov ecx,nLoop;
		mov esi,pSrcY;
		mov edi,pDst;
		mov eax,pSrcV;
		mov edx,pSrcU;

LOOP_begin:
		movq        mm0,[esi];
		movd		mm1,[eax];
		movd		mm2,[edx];
		pxor		mm4,mm4;
		punpcklbw	mm1, mm4;
		punpcklbw	mm2, mm4;

		psubusb     mm0,csMMX_16_b		//y-16      
		psubsw      mm1,csMMX_128_w     //u-128 
		movq        mm7,mm0             
		psubsw      mm2,csMMX_128_w     //v-128  
		pand        mm0,csMMX_0x00FF_w  //y6   y4  y2  y0
		psllw       mm1,3                //放大u
		psllw       mm2,3                //放大v
		psrlw       mm7,8               //0 y7  0 y5 0 y3 0 y1 
		movq        mm3,mm1              
		movq        mm4,mm2              

		pmulhw      mm1,csMMX_U_green_w  //g_u
		psllw       mm0,3                //放大y
		pmulhw      mm2,csMMX_V_green_w  //g_v
		psllw       mm7,3                //放大y
		pmulhw      mm3,csMMX_U_blue_w   //b_u
		paddsw      mm1,mm2              //g_uv
		pmulhw      mm4,csMMX_V_red_w    //r_v
		movq        mm2,mm3              //b_u
		pmulhw      mm0,csMMX_Y_coeff_w  //pre_y  1
		movq        mm6,mm4              //r_v
		pmulhw      mm7,csMMX_Y_coeff_w  //pre-y  2
		movq        mm5,mm1				 //g_uv 
		paddsw      mm3,mm0              //mm3:B1 -ok
		paddsw      mm2,mm7              //mm2:B2 -ok
		paddsw      mm4,mm0              //mm4:R1 -ok
		paddsw      mm6,mm7              //mm6:R2 -ok
		paddsw      mm1,mm0              //mm1:g1-ok
		paddsw      mm5,mm7              //mm5:g2-ok
		
		packuswb    mm3,mm4              //以下合起来部分
		packuswb    mm2,mm6              
		packuswb    mm5,mm1              
		movq        mm4,mm3              
		punpcklbw   mm3,mm2              
		punpckldq   mm1,mm5              
		punpckhbw   mm4,mm2              
		punpckhbw   mm5,mm1              


		pcmpeqb     mm2,mm2              

		movq        mm0,mm3              
		movq        mm7,mm4              
		punpcklbw   mm0,mm5              
		punpcklbw   mm7,mm2              
		movq        mm1,mm0              
		movq        mm6,mm3              
		punpcklwd   mm0,mm7              
		punpckhwd   mm1,mm7              
		movq		[edi],mm0                 
		movq        mm7,mm4              
		punpckhbw   mm6,mm5              
		movq		[edi+8],mm1               
		punpckhbw   mm7,mm2              
		movq        mm0,mm6              
		punpcklwd   mm6,mm7              
		punpckhwd   mm0,mm7              
		movq		[edi+16],mm6              
		movq		[edi+24],mm0 

		add esi,8;
		add edi,32;
		add eax,4;
		add edx,4;

		dec  ecx;
		jnz  LOOP_begin;
	}
	}
	__asm emms

//	边界处理

}

void EnhanceYV12(BYTE *src,int pitchY,int pitchUV,int width,int height,int len)
{
	BYTE *y = src;
	BYTE *v = y +width * height;
	BYTE *u = v +width/2 * height/2;
	for (int i = 0; i < height; i+=2)
	{
		BYTE *pY = NULL;
		BYTE *pY2 = NULL;
		BYTE *pU = NULL;
		BYTE *pV = NULL;

		pY = y +  pitchY * i;
		pY2 = pY + pitchY;
		pU = u + pitchUV * (i/2);
		pV = v +  pitchUV * (i/2);

		for (int j = 0;j <len; j += 2)
		{
			
			int D1,D2,D3,D4;
			int E,F;


			D1 = *pY ; 
			D2 = *(pY + 1) ;
			D3 = *pY2 ; 
			D4 = *(pY2 + 1) ;
			E = *pU - 128;
			F = *pV - 128;

			///测试视频增强
			*(pY + len ) = D1;
			*(pY + len + 1 ) = D2;
			*(pY2 + len ) = D3;
			*(pY2 + len + 1) = D4;
			*(pU + len/2 ) = E + 128;
			*(pV + len/2 ) = F + 128;
			///

			int diffY =  (int)(0.08 *E + 0.126 *F);
			//int diffY =  (int)(0.06 *E + 0.124 *F);

			D1 = D1  - diffY;
			D2 = D2  - diffY;
			D3 = D3  - diffY;
			D4 = D4  - diffY;

			//E = (int)(1.267 * E + 128);
			//F = (int)(1.267 * F + 128);
			E = (int)(1.287 * E + 128);
			F = (int)(1.287 * F + 128);

			Clip1(E);
			Clip1(F);

			Clip1(D1);
			Clip1(D2);
			Clip1(D3);
			Clip1(D4);

			if(j == (len - 1) || j == (len - 2)
				 || j == (len - 3) || j == (len - 4 ))
			{
				D1 = 0;
				D2 = 0;
				D3 = 0;
				D4 = 0;
				E = 128;
				F = 128;
			}

			D1 = GammaTable[D1];
			D2 = GammaTable[D2];
			D3 = GammaTable[D3];
			D4 = GammaTable[D4];


			*pY = D1;
			*(pY + 1) = D2;
			*pY2 = D3;
			*(pY2 + 1) = D4;

			*pU = E;
			*pV = F;

			pY += 2;
			pY2 += 2;
			++ pU;
			++ pV;
		}
	}
}

void Gamma()
{
	int i;
	double AdjustedValue;
	for (i = 0;  i < 256; i++)
	{
		AdjustedValue = 255.0 * GetGammaAdjustedValue((double)(i-2) / (double)(255-2), 1.1/*(double)((70>>1)+1000) / 1000.0*/);
		//AdjustedValue = 255.0 * GetGammaAdjustedValue((double)(i-2) / (double)(255-2), (double)((50)+1000) / 1000.0);
		GammaTable[i] = (unsigned char)AdjustedValue;
	}
}

double GetGammaAdjustedValue(double Input, double Gamma)
{
	if(Input <= 0.0)
	{
		return 0.0;
	}
	else if(Input >= 1.0)
	{
		return 1.0;
	}
	else
	{
		return pow(Input, 1.0 / Gamma);
	}
}

#endif