#include "stdafx.h"
#include "QvodGraphBuilder.h"
#include "Dmodshow.h"
#include "Reg.h"
#include "GUID.h"
#include "dvdmedia.h"
#include <algorithm>
#include "SystemInfo.h"
//#include "..\inc\MuxControl.h"
extern HINSTANCE g_hInstance;
CGraphBuilderEx::CGraphBuilderEx(void)
{
	mdwRegister = 0;
	wchar_t dllpath[MAX_PATH];
	
	memset(dllpath, 0, MAX_PATH*sizeof(wchar_t));
	::GetModuleFileName((HMODULE)g_hInstance, dllpath, MAX_PATH*2);
	CString dp = dllpath;
	int tmp = dp.ReverseFind('\\') + 1;
	m_strInsertpath = dp.Left(tmp);
	QvodFilterManager::GetInstance().LoadXMLs(m_strInsertpath);
	mpUrl = NULL;
	mpSubUrl = NULL;
	mConType = _E_CON_UNKNOW;		// 容器类型
	meVCodec = _V_OTHER;
	mnVCount = 0;
	mnACount = 0;
	mnSCount = 0;
	mbNeedVideo = false;
	mbNeedAudio = false;
	mbNeedSubtitle = false;

	mlWidth = 0;
	mlHeight = 0;
	mFileBitrate = 0;
	mnVBitrate = 0;
	mdwRegister = 0;
}

CGraphBuilderEx::~CGraphBuilderEx(void)
{
}

CComQIPtr<IBaseFilter> CGraphBuilderEx::LoadFilter(GUID clsid, QVOD_FILTER_INFO_EX* pFilterInfo)
{
	CComQIPtr<IBaseFilter> pbasefilter;

	QVOD_FILTER_INFO_EX filter_info;
	// 从XML中获取Filter
 	if(!QvodFilterManager::GetInstance().GetFilterByCLSID(clsid, filter_info))
	{
		CLog::GetInstance()->trace(ENABLE_ERROR_INFO, "Filter not found in xml %s\n", CLSID2String(clsid));
		return CComQIPtr<IBaseFilter>();
	}
	
	// 平台系统检查
	/*if (filter_info.eSystem != e_Apply_All)
	{
		DWORD version = mPlayMedia->GetSystemVersion();
		if (filter_info.eSystem == e_Apply_Vista_Only && version < 6)
			return CComQIPtr<IBaseFilter>();
		if (filter_info.eSystem == e_Apply_XP_Only && version >= 6)
			return CComQIPtr<IBaseFilter>();
	}*/

	// 设置路径等
	PrepareFilter(filter_info);

	// 装载依赖的组件并注册

	//std::vector<HMODULE> modules;
	if(!LoadComponents(filter_info, filter_info.mModuleList))
	{
		CLog::GetInstance()->trace(ENABLE_ERROR_INFO, "LoadComponents Failed\n");
		return CComQIPtr<IBaseFilter>();
	}
	CLog::GetInstance()->trace(ENABLE_MAIN_INFO, L"Load filter:%s\n", filter_info.path);

	// 装载DLL
	HMODULE hmodule = LoadLibrary(filter_info.path);

	if(hmodule == NULL)
	{
		hmodule = LoadModuleSpecy(filter_info.path);
	}
	if(!hmodule)
	{	
		//DWORD e= GetLastError();
		BYTE* perrmsg = NULL;
		GetLastErrorMsg(&perrmsg);
		CLog::GetInstance()->trace(ENABLE_ERROR_INFO, L"Load filter:%s failed\n", filter_info.path);
		CLog::GetInstance()->trace(ENABLE_MAIN_INFO, "Error msg:%s\n", perrmsg);
		return CComQIPtr<IBaseFilter>();
	}

	// 创建IBaseFilter接口
	pfnDllGetClassObject	fnDllGetClassObject = (pfnDllGetClassObject) GetProcAddress(hmodule,	"DllGetClassObject");
	if(!fnDllGetClassObject)
	{
		::FreeLibrary(hmodule);
	}
	/*else
	{
		modules.push_back(hmodule);
	}*/
	
	// Create IBaseFilter
	CLSID _clsid = filter_info.bDMO ? filter_info.wrapid : filter_info.clsid;
	CComPtr<IClassFactory> pClassFactory;
	HRESULT hr = fnDllGetClassObject(_clsid,IID_IClassFactory, (void **)&pClassFactory);
	if(SUCCEEDED(hr) && pClassFactory)
	{
		hr = pClassFactory->CreateInstance(NULL, IID_IBaseFilter, (void**)(&pbasefilter));
		if(SUCCEEDED(hr) && pbasefilter)
		{
			if (filter_info.bDMO)
			{
				CComPtr<IDMOWrapperFilter>   pDmoWrapper = NULL;   
				hr = pbasefilter->QueryInterface(__uuidof(IDMOWrapperFilter),  reinterpret_cast<void**>(&pDmoWrapper));  
				if(SUCCEEDED(hr))     
				{
					hr = pDmoWrapper->Init(filter_info.clsid, filter_info.catid);
					if (FAILED(hr))
					{
						pbasefilter.Release();
						return CComQIPtr<IBaseFilter>();
					}
				} 
			}
		}
		else
		{
		}
		//SafeRelease(pClassFactory);
	}

 	if (pbasefilter)
 	{
 		if(FAILED(AddFiltertoGraph(pbasefilter, filter_info.strName)))
 		{
			wchar_t* pwerrmsg = NULL;
			GetLastErrorMsg(&pwerrmsg);
			CLog::GetInstance()->trace(ENABLE_ERROR_INFO, L"Load filter:%s failed Error msg:%s\n", filter_info.path, pwerrmsg);
			//SafeRelease(pbasefilter);
			pbasefilter.Release();
 			return CComQIPtr<IBaseFilter>();
 		}
 	}

	/*pFilterInfo->pFilter = pbasefilter;
	if(FAILED(AddFiltertoGraph(pbasefilter, filter_info.strName)))
	{
		return CComQIPtr<IBaseFilter>();
	}*/
	filter_info.mhFilterModule = hmodule;
	if(pFilterInfo)
	{
		*pFilterInfo = filter_info;
	}
	filter_info.pFilter = pbasefilter;
	if(pbasefilter)
	{
		mvFilterList.push_back(filter_info);
	}
	return pbasefilter;
}

CComQIPtr<IBaseFilter> CGraphBuilderEx::LoadDeviceByClsid(GUID clsid, QVOD_FILTER_INFO_EX* pFilterInfo)
{
	CComQIPtr<IBaseFilter> pbasefilter;

	QVOD_FILTER_INFO_EX filter_info;
	// 从XML中获取Filter
	if(!QvodFilterManager::GetInstance().GetFilterByCLSID(clsid, filter_info))
	{
		return CComQIPtr<IBaseFilter>();
	}
	
	if(filter_info.capacity != 4096)
	{
		return CComQIPtr<IBaseFilter>();
	}
	// 平台系统检查
	/*if (filter_info.eSystem != e_Apply_All)
	{
		DWORD version = mPlayMedia->GetSystemVersion();
		if (filter_info.eSystem == e_Apply_Vista_Only && version < 6)
			return CComQIPtr<IBaseFilter>();
		if (filter_info.eSystem == e_Apply_XP_Only && version >= 6)
			return CComQIPtr<IBaseFilter>();
	}*/

	// 设置路径等
	PrepareFilter(filter_info);

	// 装载依赖的组件并注册

	//std::vector<HMODULE> modules;
	if(!LoadComponents(filter_info, filter_info.mModuleList))
	{
		return CComQIPtr<IBaseFilter>();
	}

	CComQIPtr<IBaseFilter> pFilter;
	CString fstr;
	CComQIPtr<ICreateDevEnum> pSysDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr) || pSysDevEnum == NULL)
	{
		return CComQIPtr<IBaseFilter>();
	}
	CComQIPtr<IEnumMoniker> pEnumMoniker;
	hr = pSysDevEnum->CreateClassEnumerator(filter_info.catid, &pEnumMoniker, 0);
	if(FAILED(hr) || pEnumMoniker == NULL)
	{
		return CComQIPtr<IBaseFilter>();
	}
	pEnumMoniker->Reset();
	CComQIPtr<IMoniker> pMoniker;
	for(pEnumMoniker; pEnumMoniker->Next(1, &pMoniker, 0) == S_OK;)
	{
		LPOLESTR olestr = NULL;
		if(FAILED(pMoniker->GetDisplayName(0, 0, &olestr)))
			continue;
		CComQIPtr<IPropertyBag> pPB;
		if(SUCCEEDED(pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPB)) && pPB != NULL)
		{
			CComVariant var;
			if(SUCCEEDED(pPB->Read(CComBSTR(_T("FriendlyName")), &var, NULL)))
			{
				fstr = var.bstrVal;
				if(filter_info.strName == fstr)
				{

					hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);//生成一个filter绑定到设备上。
					if(SUCCEEDED(hr) && pFilter)
					{
						if(pFilter)
						{
							hr = AddFiltertoGraph(pFilter, fstr.GetBuffer(fstr.GetLength()+1));
						}
						return pFilter;
					}
				}
			}
		}
		pMoniker.Release();
	}

	
	return pFilter;

}

//HRESULT CGraphBuilder::ParserMediaInfo(Qvod_DetailMediaInfo* pMediaInfo)
//{
//	// 初始化
//	mConType = _E_CON_UNKNOW;
//	meVCodec = _V_OTHER;
//	meACodecs.clear();
//	//meACodec = _A_OTHER;
//	mnVCount = 0;
//	mnVCount = 0;
//	mnSCount = 0;
//	mbNeedVideo = false;
//	mbNeedAudio = false;
//	mbNeedSubtitle = false;
//	mlWidth = 0;
//	mlHeight = 0;
//
//	// 容器类型
//	CString con = pMediaInfo->container;
//	mConType = ContainerFromString(con);
//
//	if (con != L"" && mConType == _E_CON_OTHER)
//	{
//		OutPutInfo(2, L"******** 容器类型辨别失败！");
//	}
//
//	if (mConType == _E_CON_UNKNOW || mConType == _E_CON_OTHER)
//	{
//		return false;
//	}
//	//检测nOverAllBitrate的合理性
//	mFileBitrate = pMediaInfo->nOverAllBitrate;
//	if(pMediaInfo->nDuration > 0 && pMediaInfo->fileSize > 0)
//	{
//		if(pMediaInfo->fileSize * 1000 * 8/ pMediaInfo->nDuration < mFileBitrate)
//		{
//			mFileBitrate = pMediaInfo->fileSize * 1000 * 8 / pMediaInfo->nDuration;
//		}
//	}
//	// 视频编码类型
//	if (pMediaInfo->nVideoCount > 0)
//	{
//		CString video = pMediaInfo->videoInfo[0]->Codec;
//		meVCodec = VideoEncoderFromString(video);
//		if(pMediaInfo->videoInfo[0])
//		{
//			mlWidth = pMediaInfo->videoInfo[0]->nWidth;
//			mlHeight = pMediaInfo->videoInfo[0]->nHeigth;
//			mnVBitrate = pMediaInfo->videoInfo[0]->nBitrate ? pMediaInfo->videoInfo[0]->nBitrate : pMediaInfo->nOverAllBitrate;
//		}
//	}
//	mnVCount = (meVCodec == _V_OTHER) ? 1 : 0;
//
//	if (meVCodec == _V_OTHER)
//	{	
//		OutPutInfo(3, L"******** 视频编码类型没有或辨别失败！");
//	}
//
//	// 音频编码类型
//	for (int i = 0; i < pMediaInfo->nAudioCount; i++)
//	{
//		CString audio = pMediaInfo->audioInfo[i]->Codec;
//		e_A_ENCODER audioType = AudioEncoderFromString(audio);
//		if (audioType == _A_OTHER)
//		{	
//			OutPutInfo(3, L"******** 音频编码类型辨别失败！");
//			continue;
//		}
//		else
//		{
//			meACodecs.push_back(audioType);
//			//break; 多音频流功能
//		}
//	}
//	//mHasAudio = !mAudioTypes.empty();
//	mnACount = meACodecs.size();
//
//	// 字幕
//	mnSCount = pMediaInfo->nSubtitleCount;
//
//	// 码率和高度
//// 	mFileBitrate = info->videoInfo[0] ? info->videoInfo[0]->nBitrate : 0;
//// 	mFileBitrate += info->audioInfo[0] ? info->audioInfo[0]->nBitrate : 0;
//	return S_OK;
//}


HRESULT CGraphBuilderEx::LoadSourceFilter()
{
	QVOD_FILTER_MAP FilterMap;
	if(!GetFilterMapByCheckByte(FilterMap))
	{
		return E_FAIL;
	}

	QVOD_FILTER_INFO_EX info;
	CComPtr<IBaseFilter> pSrc;
	FilterMap.vectFilters.push_back(CLSID_AsyncReader);
	for(int i = 0; i < FilterMap.vectFilters.size(); i++)
	{
		if(QvodFilterManager::GetInstance().GetFilterCapacity(FilterMap.vectFilters[i]) & E_FILTER_CAPACITY_READ)
		{
			CComPtr<IBaseFilter> pSrc = LoadFilter(FilterMap.vectFilters[i], &info);
			if(pSrc)
			{
				CComPtr<IFileSourceFilter> pFileSrc;
				if(SUCCEEDED(pSrc->QueryInterface(IID_IFileSourceFilter, (void**)&pFileSrc)) && pFileSrc)
				{
					if(FAILED(pFileSrc->Load(mpUrl, NULL)))
					{
						return E_FAIL;
					}
					mpFileReader = pSrc;
					break;
				}
			}
		}
	}

	if(mpFileReader == NULL)
	{
		return E_FAIL;
	}

	if(!(info.capacity & E_FILTER_CAPACITY_SPLIT))
	{
		CLSID splitterID = CLSID_NULL;
		BEGIN_FOR_EACH_FILTER_BY_CAPACITY(FilterMap.vectFilters, E_FILTER_CAPACITY_SPLIT, splitterID)
		{
			// 创建Splitter
			CComQIPtr<IBaseFilter> pSplt = LoadFilter(splitterID, &info);
			if (pSplt && SUCCEEDED(ConnectFilter(mpFileReader, pSplt)))
			{
				mpSplitterFilter = pSplt;
				break;
			}
		}
		END_FOR_EACH
	}
	return S_OK;
}

CComQIPtr<IBaseFilter> CGraphBuilderEx::LoadVideoDecoder()
{
	ASSERT(mCurVideoOutputPin);
	if (!mCurVideoOutputPin)
	{
		return CComQIPtr<IBaseFilter>();
	}
	BeginEnumMediaTypes(mCurVideoOutputPin, pEnumMediaTypes, pOutMediaType)
	{
		QVOD_MEDIA_TYPE type = {pOutMediaType->majortype , pOutMediaType->subtype};

		// 获取合适的Filter
		std::vector<CLSID> video_filters;
		QvodFilterManager::GetInstance().GetFilterByMediaType(E_FILTER_CAPACITY_VIDEO_DEC, type, video_filters);

		if(video_filters.size() == 0)
		{
			OutPutInfo(1, L"video_filters.size() == 0\n");
		}
		// 按解码优先级排序
		SortFilterByPriority(video_filters, type);

		for (int j = 0; j < video_filters.size(); j++)
		{
			QVOD_FILTER_INFO_EX info;
			CComQIPtr<IBaseFilter> pbasefilter = LoadFilter(video_filters[j], &info);
			if(!pbasefilter)
			{
				continue;
			}
			//MessageBox(NULL, NULL, L"test", MB_TOPMOST);
			if(SUCCEEDED(ConnectPinToFilter(mCurVideoOutputPin, pbasefilter)))
			{
				DeleteMediaType(pOutMediaType);
				return pbasefilter;
			}
			else
			{
				ReleaseFilter(pbasefilter);
				//SafeRelease(pbasefilter);
				pbasefilter.Release();
				continue;
			}
		}

	}
	EndEnumMediaTypes(pOutMediaType)
	
	return CComQIPtr<IBaseFilter>();


}

CComQIPtr<IPin>	CGraphBuilderEx::GetSpecyPin(CComPtr<IBaseFilter> pFilter, PIN_DIRECTION pinDir, GUID Majoytype)
{
	if(pFilter == NULL)
	{
		return CComQIPtr<IPin>();
	}

	CComQIPtr<IEnumPins> pEnum;
	CComQIPtr<IPin> pPin;
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return CComQIPtr<IPin>();
	}

	while(pPin.Release(), pEnum->Next(1,&pPin, NULL) == S_OK)
	{
		PIN_DIRECTION thisPinDir;
		pPin->QueryDirection(&thisPinDir);
		if (thisPinDir == pinDir)
		{
			CComQIPtr<IPin> pTmp;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))
			{
				pTmp.Release();
			}
			else
			{
				BeginEnumMediaTypes(pPin, pEnumMediaTypes, pOutMediaType)
				{
					if(pOutMediaType && pOutMediaType->majortype == Majoytype)
					{
						DeleteMediaType(pOutMediaType);
						return pPin;
					}
				}
				EndEnumMediaTypes(pOutMediaType)
			}
		}
		pPin.Release();
	}
	//pEnum->Release();
	return CComQIPtr<IPin>();
}

bool CGraphBuilderEx::LoadAudioDecoder()
{
	CComQIPtr<IEnumPins> pEnum;
	CComQIPtr<IPin> pPin;
	CComQIPtr<IBaseFilter> pSplt = mpSplitterFilter ? mpSplitterFilter : mpFileReader;;
	HRESULT hr = pSplt->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return false;
	}

	bool ret = false;
	while(pPin.Release(), pEnum->Next(1,&pPin, NULL) == S_OK)
	{
		PIN_DIRECTION thisPinDir;
		pPin->QueryDirection(&thisPinDir);
		if (thisPinDir == PINDIR_OUTPUT)
		{
			CComQIPtr<IPin> pTmp;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))
			{
				pTmp.Release();
			}
			else
			{
				//BeginEnumMediaTypes(pPin, pEnumMediaTypes, pOutMediaType)
				CComPtr<IEnumMediaTypes> pEnumMediaTypes; 
				if(pPin && SUCCEEDED(pPin->EnumMediaTypes(&pEnumMediaTypes)))
				{
					AM_MEDIA_TYPE* pOutMediaType = NULL;
					for(; S_OK == pEnumMediaTypes->Next(1, &pOutMediaType, NULL); /*DeleteMediaType(pOutMediaType), pOutMediaType = NULL*/)
				{
				{
					QVOD_MEDIA_TYPE type = {pOutMediaType->majortype , pOutMediaType->subtype};
					if(pOutMediaType->majortype != MEDIATYPE_Video && pOutMediaType->majortype != MEDIATYPE_Audio)
					{
						break;
					}

					// 获取合适的Filter
					std::vector<CLSID> audio_filters;
					QvodFilterManager::GetInstance().GetFilterByMediaType(E_FILTER_CAPACITY_AUDIO_DEC, type, audio_filters);

					// 按解码优先级排序
					SortFilterByPriority(audio_filters, type);

					for (int j = 0; j < audio_filters.size(); j++)
					{
						QVOD_FILTER_INFO_EX info;
						CComQIPtr<IBaseFilter> pbasefilter = LoadFilter(audio_filters[j], &info);
						if(!pbasefilter)
						{
							continue;
						}
						
						if(/*ConnectFilers(pSrc, pbasefilter)*/
							SUCCEEDED(ConnectPinToFilter(pPin, pbasefilter)))
						{	
							mpFilterAudioDecoders.push_back(pbasefilter);
							ret = true;
							DeleteMediaType(pOutMediaType);
							pOutMediaType = NULL;
							pPin.Release();
							break;
						}
						else
						{
							ReleaseFilter(pbasefilter);
							//SafeRelease(pbasefilter);
							pbasefilter.Release();
							continue;
						}
					}

				}
				if(pOutMediaType) DeleteMediaType(pOutMediaType); }}
			//	EndEnumMediaTypes(pOutMediaType)
			}
		}
		pPin.Release();
	}

	return ret;
}

bool CGraphBuilderEx::LoadAudioSwitch()
{
	if(mnACount <= 1 || mpFilterAudioDecoders.size() <= 1)
	{
		return true;
	}

	QVOD_FILTER_INFO_EX info;
	mpAudioSwtich = LoadFilter(CLSID_AudioSwtich, &info);

	for(int i = 0; i < mpFilterAudioDecoders.size(); i++)
	{
		if(FAILED(ConnectFilter(mpFilterAudioDecoders[i], mpAudioSwtich)))
		{
			return false;
		}
	}
	return true;
}
//HRESULT CGraphBuilder::LoadDecoder()
//{
//	CComQIPtr<IEnumPins> pEnum;
//	CComQIPtr<IPin> pPin;
//	CComQIPtr<IBaseFilter> pSplt = mpSplitterFilter ? mpSplitterFilter : mpFileReader;;
//	HRESULT hr = pSplt->EnumPins(&pEnum);
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//
//	while(pPin.Release(), pEnum->Next(1,&pPin, NULL) == S_OK)
//	{
//		PIN_DIRECTION thisPinDir;
//		pPin->QueryDirection(&thisPinDir);
//		if (thisPinDir == pinDir)
//		{
//			CComQIPtr<IPin> pTmp;
//			hr = pPin->ConnectedTo(&pTmp);
//			if (SUCCEEDED(hr))
//			{
//				pTmp.Release();
//			}
//			else
//			{
//				BeginEnumMediaTypes(pPin, pEnumMediaTypes, pOutMediaType)
//				{
//					QVOD_MEDIA_TYPE type = {pOutMediaType->majortype , pOutMediaType->subtype};
//					if(pOutMediaType->majortype != MEDIATYPE_Video && pOutMediaType->majortype != MEDIATYPE_Audio)
//					{
//						break;
//					}
//
//					// 获取合适的Filter
//					std::vector<CLSID> video_filters;
//					QvodFilterManager::GetInstance().GetFilterByMediaType(E_FILTER_CAPACITY_VIDEO_DEC, type, video_filters);
//
//					// 按解码优先级排序
//					SortFilterByPriority(video_filters, type);
//
//					for (int j = 0; j < video_filters.size(); j++)
//					{
//						QVOD_FILTER_INFO info;
//						CComQIPtr<IBaseFilter> pbasefilter = LoadFilter(video_filters[j], info);
//						if(!pbasefilter)
//						{
//							continue;
//						}
//						
//						if(/*ConnectFilers(pSrc, pbasefilter)*/
//							SUCCEEDED(ConnectPinToFilter(pPin, pbasefilter)))
//						{	
//							DeleteMediaType(pOutMediaType);
//							break;
//						}
//						else
//						{
//							ReleaseFilter(pbasefilter);
//							//SafeRelease(pbasefilter);
//							pbasefilter.Release();
//							continue;
//						}
//					}
//
//				}
//				EndEnumMediaTypes(pOutMediaType)
//
//			}
//		}
//		pPin.Release();
//	}
//	
//}

HRESULT CGraphBuilderEx::LoadSubFilter()
{
	if(mnSCount <= 0)
	{
		return S_OK;
	}

	QVOD_FILTER_INFO_EX info;
	CComQIPtr<IBaseFilter> pVobSubFilter = LoadFilter(CLSID_DirectVobSub, &info);
	if(pVobSubFilter == NULL)
	{
		return E_FAIL;
	}

	if(FAILED(ConnectFilter(mpVideoDecFilter, pVobSubFilter)))
	{
		return E_FAIL;
	}

	CComQIPtr<IBaseFilter> pSrc = mpSplitterFilter ? mpSplitterFilter : mpFileReader;
	HRESULT hr = E_FAIL;
	// 循环每个输出PIN
	BeginEnumPins(pSrc, pEnumPins, pOutPin)
	{	
		PIN_DIRECTION out_direction;
		hr = pOutPin->QueryDirection(&out_direction);
		if(SUCCEEDED(hr) && out_direction == PINDIR_OUTPUT)
		{
			// 循环输出PIN的媒体类型
			BeginEnumMediaTypes(pOutPin, pEnumMediaTypes, pOutMediaType)
			{
				if (pOutMediaType->majortype == MEDIATYPE_Subtitle 
					|| (pOutMediaType->majortype == MEDIATYPE_DVD_ENCRYPTED_PACK && pOutMediaType->subtype == MEDIASUBTYPE_DVD_SUBPICTURE))
				{			
					// 找到输入PIN
					BeginEnumPins(pVobSubFilter, pDecoderEnumPins, pInPin)
					{
						hr = pInPin->QueryDirection(&out_direction);
						// 找到
						if (SUCCEEDED(hr) && out_direction == PINDIR_INPUT && SUCCEEDED(pInPin->QueryAccept(pOutMediaType)))
						{
							// 未连接？
							CComQIPtr<IPin> pConnectto;
							pInPin->ConnectedTo(&pConnectto);
							if(pConnectto)
							{
								pConnectto.Release();
								continue;
							}

							// 连接两个PIN
							hr =  m_pGraph->ConnectDirect(pOutPin,pInPin,pOutMediaType);
							if(SUCCEEDED(hr))
							{
								break;
							}
						}
					}
					EndEnumPins
				}
			}
			EndEnumMediaTypes(pOutMediaType)
		}
	}
	EndEnumPins
	if(pVobSubFilter == NULL)
	{
		mpVobSubFilter = pVobSubFilter;
	}
	return hr;
}

HRESULT CGraphBuilderEx::Run()
{
	BuildGraph();
	if(m_pGraph == NULL)
	{
		return E_FAIL;
	}
	CComQIPtr<IMediaControl> pMediaControl;
	HRESULT hr = m_pGraph->QueryInterface(__uuidof(IMediaControl), (void**)&pMediaControl);
	if(FAILED(hr) || pMediaControl == NULL)
	{
		return hr;
	}
	return pMediaControl->Run();
}

HRESULT CGraphBuilderEx::Pause()
{
	if(m_pGraph == NULL)
	{
		return E_FAIL;
	}
	CComQIPtr<IMediaControl> pMediaControl;
	HRESULT hr = m_pGraph->QueryInterface(__uuidof(IMediaControl), (void**)&pMediaControl);
	if(FAILED(hr) || pMediaControl == NULL)
	{
		return hr;
	}
	return pMediaControl->Pause();
}

HRESULT CGraphBuilderEx::Stop()
{
	if(m_pGraph == NULL)
	{
		return E_FAIL;
	}
	CComQIPtr<IMediaControl> pMediaControl;
	HRESULT hr = m_pGraph->QueryInterface(__uuidof(IMediaControl), (void**)&pMediaControl);
	if(FAILED(hr) || pMediaControl == NULL)
	{
		return hr;
	}
	return pMediaControl->Stop();

	DestroyGraph();
	ResetFilters();
	ResetFlags();
}

// 排序仿函数
struct PRIORITY_SORT
{
	bool operator()(const CLSID& filter1, const CLSID& filter2) const
	{
		QVOD_FILTER_INFO node1, node2;
		DWORD p1 = QvodFilterManager::GetInstance().GetFilterPriority(filter1, mt);
		DWORD p2 = QvodFilterManager::GetInstance().GetFilterPriority(filter2, mt);
		return p1 > p2;
	}
	static QVOD_MEDIA_TYPE mt;
};
QVOD_MEDIA_TYPE PRIORITY_SORT::mt;
// 排序函数
void CGraphBuilderEx::SortFilterByPriority(std::vector<CLSID>& filters, QVOD_MEDIA_TYPE& mt)
{
	PRIORITY_SORT::mt = mt;
	std::sort(filters.begin(), filters.end(), PRIORITY_SORT());
}

bool CGraphBuilderEx::CheckChannelInfo()
{
	// 检查视频流，音频流，字幕流
	CComQIPtr<IBaseFilter> pCheckFilter = mpSplitterFilter ? mpSplitterFilter : mpFileReader;
	if (!pCheckFilter)
	{
		return false;
	}

	int maxVideoRate = 0;
	int maxAudioRate = 0;
	int idx = 0;
	BeginEnumPins(pCheckFilter, pEnumPins, pOutPin)
	{
		PIN_DIRECTION pin_direction;
		HRESULT hr = pOutPin->QueryDirection(&pin_direction);
		if(SUCCEEDED(hr) && pin_direction == PINDIR_OUTPUT)
		{
			bool bAddVideo = false;
			bool bAddAudio = false;
			bool bAddTitle = false;
			BeginEnumMediaTypes(pOutPin, pEnumMediaTypes, pOutMediaType)
			{
				if (pOutMediaType->majortype == MEDIATYPE_Video || 
					pOutMediaType->formattype == FORMAT_VideoInfo || 
					pOutMediaType->formattype == FORMAT_VideoInfo2 ||
					pOutMediaType->formattype == FORMAT_MPEGVideo)
				{
 					if(pOutMediaType->formattype == FORMAT_VideoInfo)
 					{
 						VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pOutMediaType->pbFormat;
 						mlWidth = pVih->bmiHeader.biWidth;
 						mlHeight = pVih->bmiHeader.biHeight;
						
						/*mdwAspectRatio_X = abs(pVih->bmiHeader.biWidth);
						mdwAspectRatio_Y = abs(pVih->bmiHeader.biHeight);*/

						if (!mCurVideoOutputPin || pVih->dwBitRate > maxVideoRate)
						{
							maxVideoRate = pVih->dwBitRate;
							mCurVideoOutputPin = pOutPin;// 最高码率视频流
							//mCurVideoBitCout = pVih->bmiHeader.biBitCount;
						}
 					}
 					else if(pOutMediaType->formattype == FORMAT_VideoInfo2)
 					{
 						VIDEOINFOHEADER2 *pvih2 = (VIDEOINFOHEADER2*)pOutMediaType->pbFormat;
 						mlWidth = pvih2->bmiHeader.biWidth;
 						mlHeight = pvih2->bmiHeader.biHeight;
						
						/*mdwAspectRatio_X = pvih2->dwPictAspectRatioX;
						mdwAspectRatio_Y = pvih2->dwPictAspectRatioY;*/

						if (!mCurVideoOutputPin || pvih2->dwBitRate > maxVideoRate)
						{
							maxVideoRate = pvih2->dwBitRate;
							mCurVideoOutputPin = pOutPin;// 最高码率视频流
							//mCurVideoBitCout = pvih2->bmiHeader.biBitCount;
						}
 					}
 					else if(pOutMediaType->formattype == FORMAT_MPEGVideo)
 					{
 						MPEG1VIDEOINFO *pMVI = (MPEG1VIDEOINFO*)pOutMediaType->pbFormat;
 						
 						mlWidth = pMVI->hdr.bmiHeader.biWidth;
 						mlHeight = pMVI->hdr.bmiHeader.biHeight;
 						
						/*mdwAspectRatio_X = abs(pMVI->hdr.bmiHeader.biWidth);
						mdwAspectRatio_Y = abs(pMVI->hdr.bmiHeader.biHeight);*/

						if (!mCurVideoOutputPin || pMVI->hdr.dwBitRate > maxVideoRate)
						{
							maxVideoRate = pMVI->hdr.dwBitRate;
							mCurVideoOutputPin = pOutPin;// 最高码率视频流
							//mCurVideoBitCout = pMVI->hdr.bmiHeader.biBitCount;
						}

 					}
 					else if(pOutMediaType->formattype == FORMAT_MPEG2_VIDEO)
 					{
 						MPEG2VIDEOINFO *pmvi2 = (MPEG2VIDEOINFO*)pOutMediaType->pbFormat;
 						mlWidth = pmvi2->hdr.bmiHeader.biWidth;
 						mlHeight = pmvi2->hdr.bmiHeader.biHeight;
 						
						/*mdwAspectRatio_X = pmvi2->hdr.dwPictAspectRatioX;
						mdwAspectRatio_Y = pmvi2->hdr.dwPictAspectRatioY;*/

						if (!mCurVideoOutputPin || pmvi2->hdr.dwBitRate > maxVideoRate)
						{
							maxVideoRate = pmvi2->hdr.dwBitRate;
							mCurVideoOutputPin = pOutPin;// 最高码率视频流
							//mCurVideoBitCout = pmvi2->hdr.bmiHeader.biBitCount;
						}
 					}

					bAddVideo = true;
				}
				else if(pOutMediaType->majortype == MEDIATYPE_Audio ||
						pOutMediaType->majortype == MEDIATYPE_OggPacketStream ||
						pOutMediaType->majortype == MEDIATYPE_Midi ||
						pOutMediaType->formattype == FORMAT_WaveFormatEx)
				{
					bAddAudio = true;
					
					//WAVEFORMATEX *pwav = (WAVEFORMATEX*)pOutMediaType->pbFormat;
				}
				else if(pOutMediaType->majortype == MEDIATYPE_Stream)
				{
					// 初值
					if (!mCurVideoOutputPin)
					{
						mCurVideoOutputPin = pOutPin;
					}

					bAddVideo = true;
					
				}
				else if(pOutMediaType->majortype == MEDIATYPE_Subtitle ||
						pOutMediaType->majortype == MEDIATYPE_Text )
				{	
					bAddTitle = true;
					
				}
				else if(pOutMediaType->majortype == MEDIATYPE_MPEG2_PES ||
						pOutMediaType->majortype == MEDIATYPE_DVD_ENCRYPTED_PACK ||
						pOutMediaType->majortype == MEDIATYPE_MPEG2_PACK)
				{
					if (pOutMediaType->subtype == MEDIASUBTYPE_MPEG2_VIDEO )
					{
						
						// 初值
						if (!mCurVideoOutputPin)
						{
							mCurVideoOutputPin = pOutPin;
						}

						bAddVideo = true;
					
 						MPEG2VIDEOINFO *pmvi2 = (MPEG2VIDEOINFO*)pOutMediaType->pbFormat;
 						mlWidth = pmvi2->hdr.bmiHeader.biWidth;
 						mlHeight = pmvi2->hdr.bmiHeader.biHeight;
						
						if (pmvi2->hdr.dwBitRate > maxVideoRate)
							maxVideoRate = pmvi2->hdr.dwBitRate;
					}
					else
					{
						bAddAudio = true;
						
					}
				}
				else if(pOutMediaType->majortype == MEDIATYPE_HKSTREAM)
				{
					if (pOutMediaType->subtype == MEDIASUBTYPE_HKH264)
					{
						// 初值
						if (!mCurVideoOutputPin)
						{
							mCurVideoOutputPin = pOutPin;
						}

						bAddVideo = true;
					}
					else
					{
						bAddAudio = true;
					}
				}
			}
			EndEnumMediaTypes(pOutMediaType)

			// 增加流数目
			if (bAddVideo)
				mnVCount++;
			else if(bAddAudio)
				mnACount++;
			else if(bAddTitle)
				mnSCount++;

		}
		idx++;
	}
	EndEnumPins

	if(mnVCount <= 0 && mnACount <= 0)
	{
		return false;
	}
	return true;
}

void	CGraphBuilderEx::DestroyGraph()
{
#ifdef SHOW_GRAPH_IN_GRAPHEDIT
	RemoveFromRot(mdwRegister);
#endif
	ResetFilters();
	//m_pGraph.Release();
	__super::DestroyGraph();
}

void CGraphBuilderEx::ResetFilters()
{
	for(int i = 0; i < mvFilterList.size(); i++)
	{
		ReleaseFilter(mvFilterList[i].pFilter);
		for(int j = 0; j < mvFilterList[i].mModuleList.size(); j++)
		{
			FreeLibrary(mvFilterList[i].mModuleList[j]);
		}
		mvFilterList[i].pFilter.Release();
	}
	mvFilterList.clear();
	mpFileReader.Release();
	mpSplitterFilter.Release();
	mpVideoDecFilter.Release();
	mpAudioSwtich.Release();
	mpVobSubFilter.Release();
	mpAudioRenderFilter.Release();
	mpFilterAudioDecoders.clear();
	mCurVideoOutputPin.Release();
	mpMFVideoDisplayControl.Release();
	mpVMRWindowless.Release();
	mpVMRWindowless9.Release();
	for(int i = 0; i < mpFilterAudioDecoders.size(); i++)
	{
		mpFilterAudioDecoders[i].Release();
	}
	mpFilterAudioDecoders.clear();
}

void CGraphBuilderEx::ResetFlags()
{
	mnVCount = 0;
	mnACount = 0;
	mnSCount = 0;
	mConType = _E_CON_UNKNOW;
	meVCodec = _V_OTHER;
	meACodecs.clear();
	mlWidth = 0;
	mlHeight = 0;
	mFileBitrate = 0;
	mnVBitrate = 0;
}

HRESULT CGraphBuilderEx::BuildGraph()
{
	if(m_pGraph)
	{
		//销毁之前创建的链路
		DestroyGraph();
	}

	ASSERT(m_pGraph == NULL);

	//加载filtergraph
	m_pGraph = LoadFilterGraph();

	ASSERT(m_pGraph);

	return m_pGraph ? S_OK : E_FAIL;
}

bool CGraphBuilderEx::LoadComponents(QVOD_FILTER_INFO& filter_info, std::vector<HMODULE>&  modules)
{
	for (int i = 0; i < filter_info.vComponents.size(); i++)
	{
		if (!filter_info.vComponents[i].bReg)
		{
			HMODULE hmodule = ::LoadLibrary(filter_info.vComponents[i].path);
			if (hmodule)
			{
				modules.push_back(hmodule);
			}
			else
			{
				DWORD e = GetLastError();
				continue;
			}
		}
		else
		{
			CString lpSubKey = "CLSID\\";
			lpSubKey += (CLSID2String(filter_info.vComponents[i].clsid));

			bool awalysreg = false;
			/*if (filter_info.vComponents[i].clsid == CLSID_Haali_Splitter_AR || filter_info.vComponents[i].clsid == CLSID_Haali_Source)
				awalysreg = true;*/
			
			// 检查是否注册且注册的文件是否存在
			HKEY key;
			wchar_t path[512];
			memset(path, 0, 512*sizeof(wchar_t));
			LONG len = 512;
			LONG ret = RegOpenKeyEx(HKEY_CLASSES_ROOT, lpSubKey, 0, KEY_READ, &key);
			if(ret ==ERROR_SUCCESS)
			{
				ret = RegQueryValue(key, _T("InprocServer32"), path, &len);
				if(ret == ERROR_SUCCESS && len > 0)
				{
					// 查看文件是否存在
					int ret = _waccess(path, 0);
					if(ret == 0 && !awalysreg)
					{
						RegCloseKey(key);
						continue;
					}
				}
				RegCloseKey(key);
			}
			
			// Load & Register
			HMODULE	hmodule = ::LoadLibrary(filter_info.vComponents[i].path);
			if (!hmodule)
			{
				continue;
			}

			modules.push_back(hmodule);

			pfnRegister pfnReg = NULL;
			
			pfnReg = (pfnRegister) GetProcAddress(hmodule, "DllRegisterServer");
			if(pfnReg)
			{
				HRESULT hr = pfnReg();
				if(FAILED(hr))
				{
					if(mConType == _E_CON_CSF)
					{
						return false;
					}
					//FreeLibrary(hmodule);
					continue;
				}
			}
		}
	}
	return true;
}

void	CGraphBuilderEx::RepalcePath(CString& path)
{
	if (path.IsEmpty())
	{
		return;
	}

	TCHAR output[MAX_PATH];
	memset(output, 0, MAX_PATH*sizeof(wchar_t));

	path.MakeUpper();
	int pos = path.Find(L"%WINDOWS%");
	if(pos != -1)
	{		
		GetWindowsDirectory(output, MAX_PATH);
		wsprintf(output, _T("%s\\%s"), output, ((LPCTSTR)path)+10);
		path = output;
	}
	else
	{
		wsprintf(output, _T("%s%s"),m_strInsertpath, path);
		path = output;
	}
}

void	CGraphBuilderEx::PrepareFilter(QVOD_FILTER_INFO_EX& filter_info)
{
	// 获取全路径
	RepalcePath(filter_info.path);
	for (int i = 0; i < filter_info.vComponents.size(); i++)
	{
		RepalcePath(filter_info.vComponents[i].path);
	}
}

HMODULE	CGraphBuilderEx::LoadModuleSpecy(const CString  path)
{
	int index = path.ReverseFind(L'\\');
	CString newpath = path;
	if(index >= 0)
	{
		newpath = path.Right(path.GetLength() - index - 1);
	}

	if(newpath.Find(L"COREAVC.AX") >= 0)
	{
		//CReg::GetInstance()->CoreAVCReg();
		return LoadLibrary(path);
	}

	return LoadLibrary(newpath);
}

HRESULT	CGraphBuilderEx::AddFiltertoGraph(CComQIPtr<IBaseFilter>& pbasefilter,  LPCWSTR pName)
{
	if(!pbasefilter || !m_pGraph)
		return false;

	HRESULT hr = m_pGraph->AddFilter(pbasefilter, pName);

	return hr;
}

HRESULT	CGraphBuilderEx::RemoveFilterFromGraph(CComQIPtr<IBaseFilter>& pbasefilter)
{
	if(!pbasefilter || !m_pGraph)
		return false;

	return m_pGraph->RemoveFilter(pbasefilter);
}

bool CGraphBuilderEx::GetFilterMapByCheckByte(QVOD_FILTER_MAP& filters)
{
	filters.container = _E_CON_UNKNOW;
	filters.vCheckBytes.clear();
	filters.vSuffix.clear();
	filters.vectFilters.clear();

	const std::vector<QVOD_FILTER_MAP>& filterMaps = QvodFilterManager::GetInstance().GetFilterMaps();
	if (filterMaps.empty())
	{
 		return false;
 	}

	// 读取一段字节
	int size_data = 32;
	BYTE* datas = new BYTE[size_data];
	memset(datas,0,size_data);
  	if(!ReadCheckBytes(datas, size_data))
  	{
  		return false;
  	}
	
	// 对比每个CheckByte,查找类型类型
	for (int i = 0; i < filterMaps.size(); i++)
	{
		for (int j = 0; j < filterMaps[i].vCheckBytes.size(); j++)
		{
			CString checkBytes = filterMaps[i].vCheckBytes[j];
			bool bOK = false;
			int needMoreData = 0;
			while (!(bOK = CheckCB(LPCTSTR(checkBytes), datas, size_data, needMoreData)) && needMoreData)
			{
				delete [] datas;
				datas = new BYTE[needMoreData];
				size_data = needMoreData;
				if (!ReadCheckBytes(datas, size_data))
				{
					delete [] datas;
					return false;
				}
			}
			if(bOK)
			{
				filters = filterMaps[i];
				delete [] datas;
				return true;
			}
		}
	}
	return false;
}
bool	CGraphBuilderEx::ReadCheckBytes(BYTE* buffer, int size_to_read)
{
	if(buffer == NULL || size_to_read <= 0)
	{
		return false;
	}

	FILE* pfile = _wfopen(mpUrl, L"rb");
	if(pfile)
	{
		int readlen = fread(buffer, 1, size_to_read, pfile);
		fclose(pfile);
		pfile = NULL;
		if(readlen >= size_to_read)
		{
			return true;
		}
	}
	return false;
	/*if (mpIAsyncReader)
	{
		BeginEnumPins(mpFilterQvodReader, pEnumPins, pOutPin)
		{	
			PIN_DIRECTION pin_direction;
			HRESULT hr = pOutPin->QueryDirection(&pin_direction);
			if(SUCCEEDED(hr) && pin_direction == PINDIR_OUTPUT)
			{
				CComQIPtr<IAsyncReader> pAsynReader;
				HRESULT hr = pOutPin->QueryInterface(IID_IAsyncReader, (void**)&pAsynReader);
				if(pAsynReader)
				{
					HRESULT hr = pAsynReader->SyncRead(0, size_to_read, buffer);	
					if (SUCCEEDED(hr))
					{
						return true;
					}
				}
			}
		}
		EndEnumPins
	}
	else if(mpFilterFileReader)
	{
		if (mConType == _E_CON_XLMV)
		{
			BeginEnumPins(mpFilterFileReader, pEnumPins, pPin)
			{
				PIN_DIRECTION pin_dir;
				HRESULT hr = pPin->QueryDirection(&pin_dir);
				if(SUCCEEDED(hr)&& pin_dir == PINDIR_OUTPUT)
				{
					CComQIPtr<IAsyncReader> pAsynReader;
					HRESULT hr = pPin->QueryInterface(IID_IAsyncReader, (void**)&pAsynReader);
					if(pAsynReader && SUCCEEDED(pAsynReader->SyncRead(0, size_to_read, buffer)))
					{
						return true;	
					}
				}
			}
			EndEnumPins
		}
		else
		{
			CComQIPtr<IAsyncReader> pAsynReader;
			HRESULT hr = mpFilterFileReader->QueryInterface(IID_IAsyncReader, (void**)&pAsynReader);
			if(pAsynReader && SUCCEEDED(pAsynReader->SyncRead(0, size_to_read, buffer)))
			{
				return true;
			}
		}
	}
	else
	{
		std::ifstream ifile((LPCTSTR)mUrl, std::ios::in | std::ios::binary);
		if (ifile.is_open())
		{
			ifile.read((char*)buffer, size_to_read);
			int read_size = ifile.gcount();
			if(read_size == size_to_read)
				return true;
		}
	}
	return false;*/
}

CComQIPtr<IBaseFilter>	CGraphBuilderEx::LoadAudioRender(QVOD_FILTER_INFO_EX* pinfo)
{
	CComQIPtr<IBaseFilter> pFilter;
	QVOD_FILTER_INFO_EX info;
	if(mConType == _E_CON_MIDI)
	{
		//HRESULT hr = CoCreateInstance(CLSID_AVIMIDIRender, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pFilter);
		pFilter = LoadFilter(CLSID_AVIMIDIRender, &info);
		/*if(SUCCEEDED(hr))
		{
			CString SoundDevice = L"MIDI Render";
			IAMResourceControl* pIAMResourceControl= NULL;
			hr = pFilter->QueryInterface(IID_IAMResourceControl, (void**)&pIAMResourceControl);
			ASSERT(hr == S_OK && pIAMResourceControl);
			hr = pIAMResourceControl->Reserve(AMRESCTL_RESERVEFLAGS_RESERVE, NULL);
			SafeRelease(pIAMResourceControl);
		}*/
	}
	else
	{
		pFilter = LoadFilter(CLSID_DSoundRender, &info);
		//HRESULT hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pFilter);
		//if(FAILED(hr))
		//{
		//	//SafeRelease(mpFilterAudioRender);
		//	pFilter.Release();
		//}
	}
	/*if(pFilter && FAILED(AddFiltertoGraph(pFilter, info.strName.GetBuffer(info.strName.GetLength()))))
	{
		ReleaseFilter(pFilter);
		return pFilter;
	}*/

	if(pinfo)
	{
		*pinfo = info;
	}
	return pFilter;
}

CComQIPtr<IBaseFilter>	CGraphBuilderEx::LoadVideoRender(QVOD_FILTER_INFO_EX* pinfo)
{
	CComQIPtr<IBaseFilter> pFilter;
	//GUID clsid = CLSID_VideoMixingRenderer9;//CLSID_VideoMixingRenderer;
	std::vector<GUID> clsid;
	/*if(CSystemInfo::GetInstance().GetMajorVersion() > 5)
	{
		clsid.push_back(CLSID_EnhancedVideoRenderer);
	}*/

	QVOD_FILTER_INFO_EX info;
	clsid.push_back(CLSID_VideoMixingRenderer9);
	clsid.push_back(CLSID_VideoMixingRenderer);
	for(int i = 0; i <= clsid.size(); i++)
	{
		
		pFilter = LoadFilter(clsid[i], &info);
		if(!pFilter)
		{
			OutPutInfo(3, L"LoadFilter Failed\n");
			continue;
		}

		if (pFilter)
		{
			if (clsid[i] == CLSID_EnhancedVideoRenderer)
			{
				pFilter->QueryInterface(__uuidof(IVMRWindowlessControl), (void**)&mpVMRWindowless);
			}
			else
			{
				pFilter->QueryInterface(__uuidof(IMFVideoDisplayControl), (void**)&mpMFVideoDisplayControl);
			}
			break;
		}
	}
	/*if(pFilter && FAILED(AddFiltertoGraph(pFilter, info.strName.GetBuffer(info.strName.GetLength()))))
	{
		ReleaseFilter(pFilter);
		return pFilter;
	}*/

	if(pFilter && pinfo)
	{
		*pinfo = info;
	}
	return pFilter;
}