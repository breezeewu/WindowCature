#pragma once
#include "streams.h"
#include "atlbase.h"
#include "Struct.h"
#include <map>
#include "..\inc\IvideoEffect.h"

#ifdef _DEBUG
#define SHOW_GRAPH_IN_GRAPHEDIT
#endif
//debug info add by zwu
#define      OUTPUT_DETAIL_INFO          2
#define      OUTPUT_MAIN_INFO            1

#define      OUTPUT_WARNING_INFO         -1
#define      OUTPUT_ERROR_INFO           -2
#ifndef _DEBUG
#define ENABLE_LOG_LEVEL  2
#else
#define ENABLE_LOG_LEVEL   -2
#endif


typedef LONG    (WINAPI   *pfnDllGetClassObject)(REFCLSID , REFIID, LPVOID *);

#define BeginEnumPins(pBaseFilter, pEnumPins, pPin) \
{CComPtr<IEnumPins> pEnumPins; \
	if(pBaseFilter && SUCCEEDED(pBaseFilter->EnumPins(&pEnumPins))) \
{ \
	for(CComPtr<IPin> pPin; S_OK == pEnumPins->Next(1, &pPin, 0); pPin = NULL) \
{ \

#define EndEnumPins }}}

#define BeginEnumMediaTypes(pPin, pEnumMediaTypes, pMediaType) \
{CComPtr<IEnumMediaTypes> pEnumMediaTypes; \
	if(pPin && SUCCEEDED(pPin->EnumMediaTypes(&pEnumMediaTypes))) \
{ \
	AM_MEDIA_TYPE* pMediaType = NULL; \
	for(; S_OK == pEnumMediaTypes->Next(1, &pMediaType, NULL); DeleteMediaType(pMediaType), pMediaType = NULL) \
{ \

#define EndEnumMediaTypes(pMediaType) } if(pMediaType) DeleteMediaType(pMediaType); }}

enum e_FilterType
{
	_EFT_SRC = 0,
	_EFT_SPL,
	_EFT_SUB,
	_EFT_A_DEC,
	_EFT_V_DEC,
	_EFT_A_TRANS,
	_EFT_V_TRANS,
	_EFT_AUDIO,
	_EFT_VIDEO,
	_EFT_SPY,
	_EFT_DEMUL,
	_EFT_MUX,
	_EFT_PSI,
	_EFT_WRIT
};
struct TUNER_INFO
{
	bool bTunerEnable;
	long lChannel;
	long lCountryCode;
	AMTunerModeType eTunerMode;
};

class xMainGraph
{
public:
	xMainGraph();
	~xMainGraph(void);

	void	SetPlayWnd(HWND);
public:
	virtual CComPtr<IGraphBuilder>		BuildGraph(TUNER_INFO* pTunerInfo);
	virtual void						DestroyGraph();
	static void Trace(int loglevel, const wchar_t* pstr, ...);


protected:	
	virtual	void	init();
	//virtual bool	LoadVideoRender();
	//virtual bool	LoadAudioRender();


	virtual HRESULT				LoadTuner();

	virtual	CComPtr<IBaseFilter>		LoadXbarFilter();

	CComPtr<IBaseFilter>		LoadAnalogCaptureFilter();

	CComPtr<IBaseFilter>		LoadAnalogAudioCaptureFilter();

	CComPtr<IBaseFilter>		LoadNextFilter(CComPtr<IBaseFilter> pupfilter, QVOD_FILTER_INFO nextinfo);

	CComPtr<IBaseFilter>		LoadTSMuxer();
	
	//CComPtr<IBaseFilter>		LoadVideoPostFilter();

	CComPtr<IPin>				FindPin(CComPtr<IBaseFilter> pfilter, GUID majoytype, PIN_DIRECTION dir);

	CComPtr<IBaseFilter>		LoadFilter(const CLSID& clsid,  QVOD_FILTER_INFO* pfilter_info);
	HMODULE						LoadModuleSpecy(CString path);
	CComPtr<IGraphBuilder>		LoadFilterGraph();

	CComPtr<IBaseFilter>		LoadMultiOutputFilter();
	bool						AddFiltertoGraph(CComPtr<IBaseFilter>& pbasefilter,  LPCWSTR pName);
	bool						ConnectFilers(CComPtr<IBaseFilter>& pSrc, CComPtr<IBaseFilter>& pDest);			// connect all output pin to in pin
	HRESULT						ConnectFilterDirect(CComPtr<IBaseFilter>& pSrc, CComPtr<IBaseFilter>& pDest);	// connect one free output pin to in pin
	HRESULT						ConnectPinToFilers(CComPtr<IPin>& pOutPin,CComPtr<IBaseFilter>& pDest);
	void						ReleaseFilter(CComPtr<IBaseFilter>& pbasefilter);
	

	/*bool						CreateAudioRender();
	bool						ConnectAudioRenderAuto();*/
	void						PrepareFilter(QVOD_FILTER_INFO& filter_info);
	void						RepalcePath(CString& path);
	//bool						LoadComponents(QVOD_FILTER_INFO& filter_info, std::vector<HMODULE>&  modules);

	
	  
#ifdef SHOW_GRAPH_IN_GRAPHEDIT
	HRESULT		AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void		RemoveFromRot(DWORD pdwRegister);
#endif

protected:
	std::map<IBaseFilter*, std::vector<HMODULE>>	mModule;
	CComPtr<IGraphBuilder>			mpGraph;
	DWORD							mdwRegister;
	HWND							mhPlayWnd;
	CString							mSoundDevice;
	std::vector<_QvodFilterInfo>			m_vFilters;

	// BASE FILTER
	CComPtr<IBaseFilter>			mpTVTunerFilter;
	CComPtr<IBaseFilter>			mpTVAudioFilter;
	CComPtr<IBaseFilter>			mpAnalogXbar;
	CComPtr<IBaseFilter>			mpAnalog_AudioCapture;
	CComPtr<IBaseFilter>			mpVideoCapture;
	CComPtr<IBaseFilter>			mpPostVideo;

	CComPtr<IBaseFilter>			mpH264Encoder;
	CComPtr<IBaseFilter>			mpAACEncoder;
	CComPtr<IBaseFilter>			mpMPEG2_Muxer;
	CComPtr<IBaseFilter>			mpNetRender;

	CComPtr<IAMTVTuner>				mpIAMTVTuner;
	TUNER_INFO						mTunerInfo;
};
