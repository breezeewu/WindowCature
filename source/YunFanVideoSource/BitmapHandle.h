#pragma once
#include <windows.h>
class CBitmapHandle
{
public:
	CBitmapHandle(void);
	~CBitmapHandle(void);


	static HRESULT WriteBitmap(wchar_t* pPath, LPBITMAPINFOHEADER pbih, BYTE* pbuffer);
	static HRESULT LoadBitmap(wchar_t* pPath, LPBITMAPINFOHEADER pbih, BYTE* pbuffer, int& len);
	HRESULT WriteYV12Data(wchar_t* pPath, BYTE* pbuffer, int width, int height, int npitch);
private:
	void YV12toRGB32C(BYTE *pSrc,BYTE *pDst,int width,int height);
	//void YV12toRGB32(BYTE* pyv12, int width, int height, int nyv12pitch, BYTE* prgb32);

protected:
	BYTE*   m_pYV12Data;
	BYTE*	m_pRGBData;
	int		m_nRGBBufferSize;
	int		m_nYV12DataSize;
};

