#include <streams.h>
#include <initguid.h>
#include "AudioCaptureSource.h"

#define AudioSource		L"Audio Capture Source"
// Note: It is better to register no media types than to register a partial 
// media type (subtype == GUID_NULL) because that can slow down intelligent connect 
// for everyone else.

// For a specialized source filter like this, it is best to leave out the 
// AMOVIESETUP_FILTER altogether, so that the filter is not available for 
// intelligent connect. Instead, use the CLSID to create the filter or just 
// use 'new' in your application.


// Filter setup data
const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
	&MEDIATYPE_Video,       // Major type
	&MEDIASUBTYPE_NULL      // Minor type
};


const AMOVIESETUP_PIN sudOutputPinBitmap = 
{
	L"Output",      // Obsolete, not used.
	FALSE,          // Is this pin rendered?
	TRUE,           // Is it an output pin?
	FALSE,          // Can the filter create zero instances?
	FALSE,          // Does the filter create multiple instances?
	&CLSID_NULL,    // Obsolete.
	NULL,           // Obsolete.
	1,              // Number of media types.
	&sudOpPinTypes  // Pointer to media types.
};

const AMOVIESETUP_PIN sudOutputPin = 
{
	L"Output",      // Obsolete, not used.
	FALSE,          // Is this pin rendered?
	TRUE,           // Is it an output pin?
	FALSE,          // Can the filter create zero instances?
	FALSE,          // Does the filter create multiple instances?
	&CLSID_NULL,    // Obsolete.
	NULL,           // Obsolete.
	1,              // Number of media types.
	&sudOpPinTypes  // Pointer to media types.
};

const AMOVIESETUP_FILTER sudPushSource =
{
	&CLSID_AUDIO_CAPTURE_SOURCE,// Filter CLSID
	AudioSource,       // String name
	MERIT_DO_NOT_USE,       // Filter merit
	1,                      // Number pins
	&sudOutputPin    // Pin details
};


// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance.
// We provide a set of filters in this one DLL.

CFactoryTemplate g_Templates[1] = 
{
	{ 
		AudioSource,               // Name
			&CLSID_AUDIO_CAPTURE_SOURCE,       // CLSID
			CAudioCaptureSource::CreateInstance, // Method to create an instance of MyComponent
			NULL,                           // Initialization function
			&sudPushSource           // Set-up information (for filters)
	},
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);    


#define CreateComObject(clsid, iid, var) CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, iid, (void **)&var);

STDAPI AMovieSetupRegisterServer( CLSID   clsServer, LPCWSTR szDescription, LPCWSTR szFileName, LPCWSTR szThreadingModel = L"Both", LPCWSTR szServerType     = L"InprocServer32" );
STDAPI AMovieSetupUnregisterServer( CLSID clsServer );

STDAPI RegisterFilters( BOOL bRegister )
{
	HRESULT hr = NOERROR;
	WCHAR achFileName[MAX_PATH];
	char achTemp[MAX_PATH];
	ASSERT(g_hInst != 0);

	if( 0 == GetModuleFileNameA(g_hInst, achTemp, sizeof(achTemp))) 
		return AmHresultFromWin32(GetLastError());

	MultiByteToWideChar(CP_ACP, 0L, achTemp, lstrlenA(achTemp) + 1, 
		achFileName, NUMELMS(achFileName));

	hr = CoInitialize(0);
	if(bRegister)
	{ 
		hr = AMovieSetupRegisterServer(CLSID_AUDIO_CAPTURE_SOURCE, L"screen-capture-recorder", achFileName, L"Both", L"InprocServer32");
	}

	if( SUCCEEDED(hr) )
	{
		IFilterMapper2 *fm = 0;
		hr = CreateComObject( CLSID_FilterMapper2, IID_IFilterMapper2, fm );
		if( SUCCEEDED(hr) )
		{
			if(bRegister)
			{
				IMoniker *pMoniker = 0;
				REGFILTER2 rf2;
				rf2.dwVersion = 1;
				rf2.dwMerit = MERIT_DO_NOT_USE;
				rf2.cPins = 1;
				rf2.rgPins = &sudOutputPin;
				// this is the name that actually shows up in VLC et al. weird
				hr = fm->RegisterFilter(CLSID_AUDIO_CAPTURE_SOURCE, L"Audio Hook Source", &pMoniker, &CLSID_AudioInputDeviceCategory, NULL, &rf2);
			}
			else
			{
				hr = fm->UnregisterFilter(&CLSID_AudioInputDeviceCategory, 0, CLSID_AUDIO_CAPTURE_SOURCE);
			}
		}

		// release interface
		//
		if(fm)
			fm->Release();
	}

	if( SUCCEEDED(hr) && !bRegister )
		hr = AMovieSetupUnregisterServer( CLSID_AUDIO_CAPTURE_SOURCE );

	CoFreeUnusedLibraries();
	CoUninitialize();
	return hr;
}

STDAPI DllRegisterServer()
{
	return RegisterFilters(TRUE); // && AMovieDllRegisterServer2( TRUE );
}

STDAPI DllUnregisterServer()
{
	return RegisterFilters(FALSE); // && AMovieDllRegisterServer2( FALSE );
}


//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
	DWORD  dwReason, 
	LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}