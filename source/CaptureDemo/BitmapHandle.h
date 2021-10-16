#pragma once
#include <windows.h>

class CBitmapHandle
{
public:
	CBitmapHandle(void);
	~CBitmapHandle(void);


	HRESULT WriteBitmap(wchar_t* pPath, LPBITMAPINFOHEADER pbih, BYTE* pbuffer);
	HRESULT ReadBitmap(wchar_t* pPath, LPBITMAPINFOHEADER pbih = NULL, BYTE** ppbuffer = NULL);

	HRESULT GetRGBData(LPBITMAPINFOHEADER pbih, BYTE** ppbuffer);

	HRESULT GetYV12Data(BYTE** ppYV12buffer);

	void	RGBtoYV12C(BYTE *pRGBData,   BYTE *pYV12Data, int nSrcPitch, int bitcount, int nDstPitch, int width, int height);

protected:
	BITMAPINFOHEADER*			m_pbih;				//BMP头结构体指针
	BYTE*						m_pRGBBuffer;		//BMP数据buffer指针
	BYTE*						m_pYV12Buffer;		//YV12数据buffer指针
	DWORD						m_nRGBBufferSize;	//RGB数据buffer大小
	DWORD						m_nYV12BufferSize;  //YV12数据buffer大小
};

