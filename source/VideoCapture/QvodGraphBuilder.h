#pragma once
#include "QvodFilterManager.h"
#include "GraphBuilder.h"
//#include "MediaInfo.h"
#include "commonfunction.h"
#include "evr.h"
//#include "..\inc\guid.h"
#include <dvdmedia.h>
#include "windows.h"
#include <D3d9.h>
#include <vmr9.h>

#define BEGIN_FOR_EACH_FILTER_BY_CAPACITY(VECT,CAPACITY,NODE) \
	for (int i = 0; i < VECT.size(); i++) \
	{ \
		NODE= VECT[i];\
		DWORD C = QvodFilterManager::GetInstance().GetFilterCapacity(NODE) << 1;\
		DWORD T = 1; \
		bool conti = false;\
		while((T = T << 1) <= CAPACITY){if (C & T) { conti=true; break;} };\
		if (conti || !(CAPACITY&(C>>1))) {continue;}
		


#define END_FOR_EACH }

#define BeginEnumMediaTypes(pPin, pEnumMediaTypes, pMediaType) \
{CComPtr<IEnumMediaTypes> pEnumMediaTypes; \
	if(pPin && SUCCEEDED(pPin->EnumMediaTypes(&pEnumMediaTypes))) \
{ \
	AM_MEDIA_TYPE* pMediaType = NULL; \
	for(; S_OK == pEnumMediaTypes->Next(1, &pMediaType, NULL); DeleteMediaType(pMediaType), pMediaType = NULL) \
{ \

#define EndEnumMediaTypes(pMediaType) } if(pMediaType) DeleteMediaType(pMediaType); }}


//struct QVOD_FILTER_INFO_EX:public QVOD_FILTER_INFO
//{
//	QVOD_FILTER_INFO_EX()
//	{
//		__super::_QvodFilterInfo();
//		mhFilterModule = NULL;
//	}
//	CComQIPtr<IBaseFilter>	pFilter;
//	std::vector<HMODULE>	mModuleList;
//	HMODULE					mhFilterModule;
//
//	QVOD_FILTER_INFO_EX& operator=(const QVOD_FILTER_INFO& filter_info)
//	{
//		this->capacity = filter_info.capacity;
//		this->bEnable = filter_info.bEnable;
//		this->strName = filter_info.strName;
//		this->path = filter_info.path;
//		this->wrapperpath = filter_info.wrapperpath;
//		this->bDMO = filter_info.bDMO;
//		this->clsid = filter_info.clsid;
//		this->wrapid = filter_info.wrapid;
//		this->catid = filter_info.catid;
//		this->eSystem = filter_info.eSystem;
//		this->vComponents = filter_info.vComponents;
//		this->vInTypes = filter_info.vInTypes;
//		return *this;
//	}
//};

class CGraphBuilderEx:public CGraphBuilder
{
public:
	CGraphBuilderEx(void);
public:
	~CGraphBuilderEx(void);

	CComQIPtr<IBaseFilter> LoadFilter(GUID clsid, QVOD_FILTER_INFO_EX* pFilterInfo = NULL);
	CComQIPtr<IBaseFilter> LoadDeviceByClsid(GUID clsid, QVOD_FILTER_INFO_EX* pFilterInfo = NULL);
	//CComQIPtr<IGraphBuilder> LoadFilterGraph();

	////HRESULT ParserMediaInfo(Qvod_DetailMediaInfo* pMediaInfo);
	//HRESULT ConnectFilter(CComPtr<IBaseFilter> pUp, CComPtr<IBaseFilter> pDown, bool bconnectall = false);
	//HRESULT ConnectFilterThrough(CComQIPtr<IBaseFilter> pUp, CComQIPtr<IBaseFilter> pDown,  CComQIPtr<IBaseFilter> pthrough);
	//HRESULT ConnectPinToFilter(CComPtr<IPin> pPin, CComPtr<IBaseFilter> pDst);
	//HRESULT ConnectSubTitlePins(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pVobSubFilter);
	//CComQIPtr<IPin> GetUnconnectedPin(CComQIPtr<IBaseFilter>& pFilter,PIN_DIRECTION pinDir);
	HRESULT LoadSourceFilter();
	CComQIPtr<IBaseFilter> LoadVideoDecoder();
	bool LoadAudioDecoder();
	CComQIPtr<IPin>	GetSpecyPin(CComPtr<IBaseFilter> pFilter, PIN_DIRECTION pinDir, GUID Majoytype);
	bool LoadAudioSwitch();
	HRESULT LoadSubFilter();

	//播放控制接口
	virtual HRESULT Run();

	virtual HRESULT Pause();

	virtual HRESULT Stop();

protected:
	void	SortFilterByPriority(std::vector<CLSID>& filters, QVOD_MEDIA_TYPE& mt);
	bool	CheckChannelInfo();
	virtual void	DestroyGraph();
	//virtual void ReleaseFilter(CComQIPtr<IBaseFilter>& pbasefilter);
	virtual void ResetFilters();
	virtual void ResetFlags();
	virtual HRESULT BuildGraph();

	CComQIPtr<IBaseFilter>	LoadAudioRender(QVOD_FILTER_INFO_EX* pinfo = NULL);
	CComQIPtr<IBaseFilter>	LoadVideoRender(QVOD_FILTER_INFO_EX* pinfo = NULL);


//#ifdef SHOW_GRAPH_IN_GRAPHEDIT
//	HRESULT			AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
//	void			RemoveFromRot(DWORD pdwRegister);
//#endif
	bool			LoadComponents(QVOD_FILTER_INFO& filter_info, std::vector<HMODULE>&  modules);
	void			RepalcePath(CString& path);
	void			PrepareFilter(QVOD_FILTER_INFO_EX& filter_info);
	HMODULE			LoadModuleSpecy(const CString  path);
	HRESULT			AddFiltertoGraph(CComQIPtr<IBaseFilter>& pbasefilter,  LPCWSTR pName= NULL);
	HRESULT			RemoveFilterFromGraph(CComQIPtr<IBaseFilter>& pbasefilter);
	bool			GetFilterMapByCheckByte(QVOD_FILTER_MAP& filters);
	bool			ReadCheckBytes(BYTE* buffer, int size_to_read);

protected:
	CComQIPtr<IBaseFilter>				mpFileReader;
	CComQIPtr<IBaseFilter>				mpSplitterFilter;
	CComQIPtr<IBaseFilter>				mpVideoDecFilter;
	CComQIPtr<IBaseFilter>				mpAudioSwtich;
	CComQIPtr<IBaseFilter>				mpVobSubFilter;
	CComQIPtr<IBaseFilter>				mpVideoRenderFilter;
	CComQIPtr<IBaseFilter>				mpAudioRenderFilter;
	CComQIPtr<IPin>						mCurVideoOutputPin;
std::vector<CComQIPtr<IBaseFilter>>		mpFilterAudioDecoders;// 选用的Audio Decoder Filter
	CComQIPtr<IMFVideoDisplayControl>	mpMFVideoDisplayControl;
	CComQIPtr<IVMRWindowlessControl>	mpVMRWindowless;
	CComQIPtr<IVMRWindowlessControl9>	mpVMRWindowless9;

	std::vector<QVOD_FILTER_INFO_EX>	mvFilterList;
	e_CONTAINER							mConType;		// 容器类型
	e_V_ENCODER							meVCodec;
	std::vector<e_A_ENCODER>			meACodecs;	// 音频编码类型

	int									mnVCount;
	int									mnACount;
	int									mnSCount;
	bool								mbNeedVideo;
	bool								mbNeedAudio;
	bool								mbNeedSubtitle;

	long								mlWidth;
	long								mlHeight;
	DWORD								mFileBitrate;
	DWORD								mnVBitrate;
	DWORD								mdwRegister;

	wchar_t*							mpUrl;
	wchar_t*							mpSubUrl;
	CString								m_strInsertpath;
	//CAutoPtr<Qvod_DetailMediaInfo>		mpMediaInfo;
};
