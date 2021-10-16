#include "stdafx.h"
#include "BitmapHandle.h"

#ifndef __WXDEBUG__
#define		CheckPointer(p, ret) if(0==p){return ret;}
#endif

#define RGB2YUV(b, g, r, y, u, v) \
	y=(BYTE)(((int)299*r +(int)587*g +(int)114*b)/1000); \
	u=(BYTE)(((int)-147*r -(int)287*g +(int)436*b+128000)/1000); \
	v=(BYTE)(((int)615*r -(int)515*g -(int)100*b+128000)/1000)

#define YUV2RGB(b, g, r, y, u, v) \
	r = ((int)((y) + 1.4075 * ((v) - 128)));\
	g = ((int)((y) - 0.3455 * ((u) - 128) - 0.7169 * ((v) - 128)));\
	b = ((int)((y) + 1.779 * ((u) - 128)))

CBitmapHandle::CBitmapHandle(void)
{
	m_pbih				= NULL;
	m_pRGBBuffer		= NULL;
	m_pYV12Buffer		= NULL;
	m_nRGBBufferSize	= 0;
	m_nYV12BufferSize	= 0;
}


CBitmapHandle::~CBitmapHandle(void)
{
	if(m_pbih)
	{
		delete m_pbih;
		m_pbih = NULL;
	}

	if(m_pRGBBuffer)
	{
		delete[] m_pRGBBuffer;
		m_pRGBBuffer = NULL;
		m_nRGBBufferSize = 0;
	}

	if(m_pYV12Buffer)
	{
		delete[] m_pYV12Buffer;
		m_pYV12Buffer = NULL;
		m_nYV12BufferSize = NULL;
	}
}

HRESULT CBitmapHandle::WriteBitmap(wchar_t* pPath, LPBITMAPINFOHEADER pbih, BYTE* pbuffer)
{
	CheckPointer(pPath,		E_POINTER);
	CheckPointer(pbih,		E_POINTER);
	CheckPointer(pbuffer,	E_POINTER);
#define BFT_BITMAP 0x4d42  

	// Macros
#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
	? (int)(1 << (int)(lpbi)->biBitCount)          \
	: (int)(lpbi)->biClrUsed)
#define DibSize(lpbi)           ((lpbi)->biSize + (lpbi)->biSizeImage + (int)(lpbi)->biClrUsed * sizeof(RGBQUAD))
#define DibPaletteSize(lpbi)    (DibNumColors(lpbi) * sizeof(RGBQUAD))

	BITMAPFILEHEADER    hdr;
	DWORD               dwSize, dwWritten;
	LPBITMAPINFOHEADER  pdib = (LPBITMAPINFOHEADER) pbih;//pbuffer;

	// Create a new file to store the bitmap data
	HANDLE hFile = CreateFile(pPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
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
	WriteFile(hFile, (LPCVOID) pdib, sizeof(BITMAPINFOHEADER), &dwWritten, 0);
	WriteFile(hFile, (LPCVOID) pbuffer, dwSize - sizeof(BITMAPINFOHEADER), &dwWritten, 0);

	// Close the file
	CloseHandle(hFile);

	return true;
}

HRESULT CBitmapHandle::ReadBitmap(wchar_t* pPath, LPBITMAPINFOHEADER pbih, BYTE** ppbuffer)
{
	CheckPointer(pPath,		E_POINTER);
	//CheckPointer(pbih,		E_POINTER);
	//CheckPointer(pbuffer,	E_POINTER);

	HANDLE hFile = CreateFile(pPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	BITMAPFILEHEADER    hdr;
	DWORD               dwSize, dwRead;
	if(m_pbih == NULL)
	{
		m_pbih = new BITMAPINFOHEADER;
	}

	memset(m_pbih, 0, sizeof(BITMAPINFOHEADER));

	// Write the bitmap header and bitmap bits to the file
	ReadFile(hFile, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), &dwRead, 0);
	ReadFile(hFile, (LPVOID) m_pbih, sizeof(BITMAPINFOHEADER), &dwRead, 0);
	
	if(m_nRGBBufferSize <= m_pbih->biSizeImage)
	{
		if(m_pRGBBuffer)
		{
			delete[] m_pRGBBuffer;
			m_pRGBBuffer = NULL;
		}

		m_nRGBBufferSize = m_pbih->biSizeImage;
		m_pRGBBuffer = new BYTE[m_nRGBBufferSize];

	}

	//读BMP数据
	if(m_nRGBBufferSize)
	{
		ReadFile(hFile, (LPVOID) m_pRGBBuffer, m_pbih->biSizeImage, &dwRead, 0);
	}

	GetRGBData(pbih, ppbuffer);

	// Close the file
	CloseHandle(hFile);

	return true;
}

HRESULT CBitmapHandle::GetRGBData(LPBITMAPINFOHEADER pbih, BYTE** ppbuffer)
{
	if(pbih && m_pbih)
	{
		*pbih = *m_pbih;
	}

	if(ppbuffer && m_pRGBBuffer)
	{
		*ppbuffer = m_pRGBBuffer;
	}

	return S_OK;
}

HRESULT CBitmapHandle::GetYV12Data(BYTE** ppYV12buffer)
{
	CheckPointer(m_pbih, E_FAIL);
	if(m_nYV12BufferSize < m_pbih->biWidth * abs(m_pbih->biHeight) * 3/2)
	{
		if(m_pYV12Buffer)
		{
			delete[] m_pYV12Buffer;
			m_pYV12Buffer = NULL;
		}

		m_nYV12BufferSize = m_pbih->biWidth * abs(m_pbih->biHeight) * 3/2;
		m_pYV12Buffer = new BYTE[m_nYV12BufferSize];
	}

	
	CheckPointer(m_pRGBBuffer, E_FAIL);
	CheckPointer(m_pYV12Buffer, E_OUTOFMEMORY);

	RGBtoYV12C(m_pRGBBuffer, m_pYV12Buffer, m_pbih->biWidth * m_pbih->biBitCount / 8, m_pbih->biBitCount, m_pbih->biWidth, m_pbih->biWidth, abs(m_pbih->biHeight));

	if(ppYV12buffer)
	{
		*ppYV12buffer = m_pYV12Buffer;
	}
	return S_OK;
}

void CBitmapHandle::RGBtoYV12C(BYTE *pRGBData,   BYTE *pYV12Data, int nSrcPitch, int bitcount, int nDstPitch, int width, int height)
{
	BYTE* py = pYV12Data;
	width = ((width + 1) >>1) << 1;
	BYTE* pu = py + nDstPitch * height;
	BYTE* pv = pu + nDstPitch * height/4;
	BYTE* ptmp = pRGBData;
	BYTE* ply = py;
	BYTE* plu = pu;
	BYTE* plv = pv;
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j+=2)
		{
			//两个像素共用一个uv
			if(i % 2 == 1)
			{
				*py = (BYTE)(((int)30*ptmp[2] +(int)59*ptmp[1] +(int)11*ptmp[0])/100);
			}
			else
			{
				RGB2YUV(ptmp[0], ptmp[1], ptmp[2], (*py), (*pu), (*pv));
				pu++;
				pv++;
			}
			//后移一个像素
			ptmp += bitcount/8;
			py++;

			*py = (BYTE)(((int)30*ptmp[2] +(int)59*ptmp[1] +(int)11*ptmp[0])/100);
			py++;
			//后移一个像素
			ptmp += bitcount/8;
		}


		if(nDstPitch >= width)
		{
			py = ply + nDstPitch;
			ply = py;
			//两行公用uv
			if(i % 2 == 1)
			{
				pu = plu + nDstPitch/2;
				pv = plv + nDstPitch/2;
				plu = pu;
				plv = pv;
			}
		}

		pRGBData += nSrcPitch;
		ptmp = pRGBData;
	}
	return ;

}