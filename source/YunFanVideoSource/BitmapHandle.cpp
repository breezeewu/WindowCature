#include "BitmapHandle.h"

#define YUV2RGB(b, g, r, y, u, v) \
	r = ((int)((y) + 1.4075 * ((v) - 128)));\
	g = ((int)((y) - 0.3455 * ((u) - 128) - 0.7169 * ((v) - 128)));\
	b = ((int)((y) + 1.779 * ((u) - 128)))

CBitmapHandle::CBitmapHandle(void)
{
	m_pYV12Data			= NULL;
	m_pRGBData			= NULL;
	m_nRGBBufferSize	= 0;
	m_nYV12DataSize		= 0;
}


CBitmapHandle::~CBitmapHandle(void)
{
}

HRESULT CBitmapHandle::WriteBitmap(wchar_t* pPath, LPBITMAPINFOHEADER pbih, BYTE* pbuffer)
{

#define BFT_BITMAP 0x4d42
	// Macros
#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
	? (int)(1 << (int)(lpbi)->biBitCount)          \
	: (int)(lpbi)->biClrUsed)
#define DibSize(lpbi)           ((lpbi)->biSize + (lpbi)->biSizeImage + (int)(lpbi)->biClrUsed * sizeof(RGBQUAD))
#define DibPaletteSize(lpbi)    (DibNumColors(lpbi) * sizeof(RGBQUAD))

	BITMAPFILEHEADER    hdr;
	DWORD               dwSize, dwWritten;
	LPBITMAPINFOHEADER  pdib = (LPBITMAPINFOHEADER) pbih;

	HANDLE hFile = CreateFile(pPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
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

	return S_OK;
}

HRESULT CBitmapHandle::LoadBitmap(wchar_t* pPath, LPBITMAPINFOHEADER pbih, BYTE* pbuffer, int& len)
{
	//CheckPointer(pPath,   E_POINTER);
	//CheckPointer(pbih,    E_POINTER);
	//CheckPointer(pbuffer, E_POINTER);

	HANDLE hFile = CreateFile(pPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		//SafeDeleteArray(pbuffer);
		return E_FAIL;
	}

	BITMAPFILEHEADER    hdr;
	DWORD               dwSize, dwRead;
	BITMAPINFOHEADER	bih;

	memset(&hdr, 0, sizeof(hdr));
	memset(&bih, 0, sizeof(bih));

	// Write the bitmap header and bitmap bits to the file
	ReadFile(hFile, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), &dwRead, 0);
	ReadFile(hFile, (LPVOID) &bih, sizeof(BITMAPINFOHEADER), &dwRead, 0);

	if(pbuffer && len >= bih.biSizeImage)
	{
		ReadFile(hFile, (LPVOID) pbuffer, bih.biSizeImage, &dwRead, 0);
	}
	
	// Close the file
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CBitmapHandle::WriteYV12Data(wchar_t* pPath, BYTE* pbuffer, int width, int height, int npitch)
{
	int size = npitch * height * 4;
	if(m_nRGBBufferSize < size)
	{
		if(m_pRGBData)
		{
			delete[] m_pRGBData;
			m_pRGBData = NULL;
		}

		m_pRGBData = new BYTE[size];
		//CheckPointer(m_pRGBData, E_OUTOFMEMORY);
		m_nRGBBufferSize = size;
	}

	YV12toRGB32C(pbuffer, m_pRGBData, width, height);

	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof(bih));
	bih.biBitCount = 32;
	bih.biSize = sizeof(bih);
	bih.biCompression = BI_RGB;
	bih.biWidth			= width;
	bih.biHeight		= height;
	bih.biSizeImage		= width*height*4;
	return WriteBitmap(pPath, &bih, m_pRGBData);
}

void CBitmapHandle::YV12toRGB32C(BYTE *pSrc,BYTE *pDst,int width,int height)
{
	int h2 = height/2;
	int w2 = width/2;
	int p2 = width/2;

	BYTE *y = pSrc;
	BYTE *v = y +width * height;
	BYTE *u = v +width/2 * height/2;
	BYTE *pBmpTemp = pDst;

	for (int i = 0; i < height;++i)
	{
		BYTE *pSrcY = y + i * width;
		BYTE *pSrcU = u + (i/2) * p2;
		BYTE *pSrcV = v + (i/2) * p2;
		BYTE *pDst = pBmpTemp + width * 4 * i;

		BYTE *pEnd = pSrcY + width;
		for (;pSrcY < pEnd; pSrcY+=2)
		{
			int nr,ng,nb;
			YUV2RGB(nr,ng,nb,pSrcY[0],pSrcV[0],pSrcU[0]);
			if (nr > 255)
			{
				pDst[0] = 0xff;
			}
			else if (nr < 0)
			{
				pDst[0] = 0;
			}
			else
			{
				pDst[0] = (BYTE)nr;
			}

			if (ng > 255)
			{
				pDst[1] = 0xff;
			}
			else if (ng < 0)
			{
				pDst[1] = 0;
			}
			else
			{
				pDst[1] = (BYTE)ng;
			}

			if (nb > 255)
			{
				pDst[2] = 0xff;
			}
			else if (nb < 0)
			{
				pDst[2] = 0;
			}
			else
			{
				pDst[2] = (BYTE)nb;
			}

			pDst[3] = 0xff;
			YUV2RGB(nr,ng,nb,pSrcY[1],pSrcV[0],pSrcU[0]);
			if (nr > 0xff)
			{
				pDst[4] = 0xff;
			}
			else if (nr < 0)
			{
				pDst[4] = 0;
			}
			else
			{
				pDst[4] = (BYTE)nr;
			}

			if (ng > 0xff)
			{
				pDst[5] = 0xff;
			}
			else if (ng < 0)
			{
				pDst[5] = 0;
			}
			else
			{
				pDst[5] = (BYTE)ng;
			}

			if (nb > 0xff)
			{
				pDst[6] = 0xff;
			}
			else if (nb < 0)
			{
				pDst[6] = 0;
			}
			else
			{
				pDst[6] = (BYTE)nb;
			}

			pDst[7] = 0xff;
			pSrcU ++;
			pSrcV ++;
			pDst += 8;
		}
	}
}