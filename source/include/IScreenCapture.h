//#include <objbase.h>

DEFINE_GUID(IID_IScreenCapture, 
			0x8E4D4121, 0xbc23, 0x49fb, 0xa4, 0x42, 0xc7, 0x38, 0x85, 0xa3, 0x20, 0xca);

[uuid("8E4D4121-BC23-49fb-A442-C73885A320CA")]
DECLARE_INTERFACE_(IScreenCapture, IUnknown)
{
	STDMETHOD (SetVideoHwnd) (HWND hwnd, RECT* pRect = NULL) = 0;
	STDMETHOD (SetVideoSolution)(int nwidth, int nheight) = 0;
	STDMETHOD (GetVideoSolution)(int* pnwidth, int* pnheight) = 0;
	STDMETHOD (GetFirstBitmapImage) (BYTE* pData, int len) = 0;	
	STDMETHOD (GetFirstBitmapinfo) (BITMAPINFOHEADER * pBitmapInfo,int *pLen) = 0;
};
	//----------------------------------------------------------------------------

