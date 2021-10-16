#include "GraphBuilder.h"
#include "commonfunction.h"
#include "SystemInfo.h"
#include <dmodshow.h>
#include <algorithm>

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

CGraphBuilder::CGraphBuilder(void)
{
	//程序根目录路径
	TCHAR dllpath[MAX_PATH*2];
	TCHAR exepath[MAX_PATH*2];
	ZeroTChar(dllpath, 0, MAX_PATH*2);
	ZeroTChar(exepath, 0, MAX_PATH*2);
	::GetModuleFileName((HMODULE)g_hInstance, dllpath, MAX_PATH*2);
	::GetModuleFileName(NULL, exepath, MAX_PATH*2);
	
	CString dp = dllpath;
	CString ep = exepath;
	int tmp = dp.ReverseFind('\\') + 1;
	int tmp2 = ep.ReverseFind('\\') + 1;
	m_DllPath = dp.Left(tmp);
	m_ExePath = ep.Right(tmp2);
	//m_pMainModuleName = new TCHAR[MAX_PATH];
	//memset(m_pMainModuleName, 0, sizeof(TCHAR) * MAX_PATH);
	//wcscpy(m_pMainModuleName, exepath + /*ep.GetLength() -*/ tmp2);
}

CGraphBuilder::~CGraphBuilder(void)
{
}


void CGraphBuilder::ResetFlags()
{
	// 重置所有标识
	m_ConType			= _E_CON_UNKNOW;
	m_VideoType			= _V_OTHER;

	m_VideoWidth		= 0;
	m_VideoHeight		= 0;
	m_VideoHeight		= 0;
	m_FileBitrate		= 0;
	m_VideoBitrate		= 0;
	m_CurVideoBitCout	= 0;
	m_AudioTypes.clear();
}

void CGraphBuilder::ResetInterface()
{
	//释放所有接口指针
	/*m_pQSCallBack.Release();
	m_pIColorTrans.Release();
	m_pISubTrans.Release();
	m_pISoundTrans.Release();
	m_pIPostVideo.Release();
	m_pDirectVobSub.Release();
	m_pVideoTagQuery.Release();	
	m_pSubtitleControl.Release();
	m_pSubPicPresent.Release();
	m_pPostProcess.Release();
	m_pEpisodeSubtitle.Release();
	m_pIEVRPresenterControl.Release();*/
	m_pDvdControl.Release();
	m_pIDvdInfo2.Release();
	m_pIMediaEvent.Release();
	m_pKeyFrameInfo.Release();
	m_pKeyFrameInfoEx.Release();
	
}

// 重置所有filter
void CGraphBuilder::ResetFilters()
{
	//Release Filter
	ReleaseFilter(m_pAudioCapture);
	ReleaseFilter(m_pVideoCapture);
	ReleaseFilter(m_pFilterFileReader);
	ReleaseFilter(m_pFilterSource);
	ReleaseFilter(m_pFilterSplitter);
	ReleaseFilter(m_pFilterVideoDecoder);

	for (int i = 0 ; i < m_pFilterAudioDecoders.size(); i++)
	{
		ReleaseFilter(m_pFilterAudioDecoders[i]);
	}
	m_pFilterAudioDecoders.clear();

	ReleaseFilter(m_pFilterColorSpaceTrans);
	
	ReleaseFilter(m_pFilterVobSubTitle);

	ReleaseFilter(m_pFilterQvodSubTitle);

	ReleaseFilter(m_pFilterQvodPostVideo);

	ReleaseFilter(m_pFilterAudioSwitch);	

	ReleaseFilter(m_pFilterQvodSound);

	ReleaseFilter(m_pFilterAudioRender);

	ReleaseFilter(m_pFilterVideoRender);

	ReleaseFilter(m_pFilterVideoEncoder);

	ReleaseFilter(m_pFilterAudioEncoder);

	ReleaseFilter(m_pFilterMuxer);

	ReleaseFilter(m_pFilterSink);
}

//销毁Graph链路
void CGraphBuilder::DestroyGraph()
{
	//重置所有标识
	ResetFlags();
	
	// RemoveFilter及它接口
	ResetFilters();

	// 释放IGraphBuilder接口
	int ret = 0;
	HRESULT hr = E_FAIL;
	if(m_pGraph)
	{
 		IEnumFilters *pEnum = NULL;
 		IBaseFilter *pBF[20]= {0};
 		ULONG fetched = 20;
 		hr = m_pGraph->EnumFilters(&pEnum);
 		if(SUCCEEDED(hr) && pEnum)
 		{
 			pEnum->Next(20, pBF, &fetched);
			for(int i =0; i < fetched; i++)
			{
				hr = m_pGraph->RemoveFilter(pBF[i]);
				ret = pBF[i]->Release();
				CLog::GetInstance()->trace(1, L"ret = %d\n*****", &ret);
			}
 			pEnum->Release();
 		} 
	
		m_pGraph.Release();
	}

	//销毁dvdgraph指针
	m_pDvdGraph.Release();

	// 释放dlls
	std::map<IBaseFilter*, std::vector<HMODULE> >::iterator it = m_Module.begin();
	for (; it != m_Module.end(); it++)
	{
		for (int i = 0; i < it->second.size(); i++)
		{
			::FreeLibrary(it->second[i]);
		}
	}
	m_Module.clear();

	if (m_EVRPresenterHMD)
	{
		::FreeLibrary(m_EVRPresenterHMD);
		m_EVRPresenterHMD = 0;
	}

	//m_pPresent = NULL;

	//	if (m_EVRPresenterHMD)
	//{
	//	::FreeLibrary(m_EVRPresenterHMD);
	//	m_EVRPresenterHMD = 0;
	//}

	//if(m_D3DX9_32HMD)
	//{
	//	::FreeLibrary(m_D3DX9_32HMD);
	//	m_D3DX9_32HMD = 0;
	//}

	//if(m_pIdxSubPath)
	//{
	//	delete[] m_pIdxSubPath;
	//	m_pIdxSubPath = NULL;
	//}
}

CComQIPtr<IBaseFilter>	CGraphBuilder::GetPreFilterOnPath(e_FilterType t, int* idx_audio_decoder)
{
	if (t == _EFT_SPL)
	{
		//获取Splitter之前的filter
		return m_pFilterFileReader;//m_pFilterFileReader?m_pFilterFileReader:m_pFilterQvodReader;
	}
	else if(t == _EFT_V_DEC)
	{
		//获取ideo decoder之前的Filter
		return m_pFilterSplitter?m_pFilterSplitter:m_pFilterSource;
	}
	else if(t == _EFT_A_DEC)
	{
		//获取audio decoder之前的filter
		return m_pFilterSplitter?m_pFilterSplitter:m_pFilterSource;
	}
	else if(t == _EFT_V_TRANS)
	{
		if(m_pFilterColorSpaceTrans)
		{
			return m_pFilterColorSpaceTrans;
		}

		if (m_pFilterQvodPostVideo)
		{
			return m_pFilterQvodPostVideo;
		}

		if (m_pFilterQvodSubTitle)
		{
			return m_pFilterQvodSubTitle;
		}

		if (m_pFilterVobSubTitle)
		{
			return m_pFilterVobSubTitle;
		}

		if (m_pFilterVideoDecoder)
		{
			return m_pFilterVideoDecoder;
		}

		if (m_pFilterSplitter)
		{
			return m_pFilterSplitter;
		}
		
		return m_pFilterSource;
		
	}
	else if(t == _EFT_A_TRANS)
	{
		//获取音频后处理之前的filter
		if (m_pFilterQvodSound)
		{
			return m_pFilterQvodSound;
		}
		if (m_pFilterAudioSwitch)
		{
			return m_pFilterAudioSwitch;
		}

		int idx = idx_audio_decoder ? *idx_audio_decoder : 0;
		CComQIPtr<IBaseFilter> filter = m_pFilterAudioDecoders.size()> idx ? m_pFilterAudioDecoders[idx] : m_pFilterSplitter;
		if (filter)
		{
			return filter;
		}
	
		return m_pFilterSource;
	}
	else if(t == _EFT_VIDEO_RENDER)
	{
		if(m_pFilterColorSpaceTrans)
		{
			return m_pFilterColorSpaceTrans;
		}

		return m_pFilterQvodPostVideo?m_pFilterQvodPostVideo : GetPreFilterOnPath(_EFT_V_TRANS);
	}
	else if(t == _EFT_AUDIO_RENDER)
	{
		//获取audio render之前的filter
		return m_pFilterQvodSound ? m_pFilterQvodSound : GetPreFilterOnPath(_EFT_A_TRANS);
	}
	else if(_EFT_V_ENCODER == t)
	{
		return m_pFilterQvodPostVideo ? 
	}
	else if(_EFT_MUX == t)
	{

	}

	return CComQIPtr<IBaseFilter>();
}


void CGraphBuilder::RepalcePath(CString& path)
{
	if (path.IsEmpty())
	{
		return;
	}

	TCHAR output[MAX_PATH];
	ZeroTChar(output, 0, MAX_PATH);

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
		wsprintf(output, _T("%sCodecs\\%s"), m_DllPath, path);
		path = output;
	}

	// 精简版？
	/*if(mPlayMedia->GetProductVersion() == 0|| CSystemInfo::GetInstance().IsWin8())
	{
		int pos = path.Find(L"WM\\");
		if(pos != -1)
		{
			GetWindowsDirectory(output, MAX_PATH);
			wsprintf(output, _T("%s\\SysWOW64\\%s"), output, ((LPCTSTR)path)+pos+3);
			if (!_taccess(output,0))
			{
				path = output;
			}
			else
			{
				wsprintf(output, _T("%s\\SYSTEM32\\%s"), output, ((LPCTSTR)path)+pos+3);
				path = output;
			}
		}
	}*/
}

void CGraphBuilder::PrepareFilter(QVOD_FILTER_INFO& filter_info)
{
	// 获取全路径
	RepalcePath(filter_info.path);
	for (int i = 0; i < filter_info.vComponents.size(); i++)
	{
		RepalcePath(filter_info.vComponents[i].path);
	}
}

bool CGraphBuilder::LoadComponents(QVOD_FILTER_INFO& filter_info, std::vector<HMODULE>&  modules)
{
	for (int i = 0; i < filter_info.vComponents.size(); i++)
	{
		if (!filter_info.vComponents[i].bReg)
		{
			HMODULE hmodule = LoadLibrarySpecify(filter_info.vComponents[i].path);
			if (hmodule)
			{
				modules.push_back(hmodule);
			}
			else
			{
				DWORD e = GetLastError();
				CLog::GetInstance()->trace(2, L"LoadComponents FAILED!\n");
				continue;
			}
		}
		else
		{
			CString lpSubKey = "CLSID\\";
			lpSubKey += (CLSID2String(filter_info.vComponents[i].clsid));

			bool awalysreg = false;
			if (filter_info.clsid == CLSID_Haali_Splitter_AR || filter_info.clsid == CLSID_Haali_Source || CLSID_WMV_Decoder_DMO == filter_info.wrapid)
				awalysreg = true;
			
			// 检查是否注册且注册的文件是否存在
			HKEY key;
			wchar_t path[512];
			memset(path, 0, 512*sizeof(wchar_t));
			LONG len = 512;
			LONG ret = RegOpenKeyEx(HKEY_CLASSES_ROOT, lpSubKey, 0, KEY_READ, &key);
			if(ret == ERROR_SUCCESS)
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
					
					CLog::GetInstance()->trace(2, path);
				}
				RegCloseKey(key);
			}
			
			// Load & Register
			HMODULE	hmodule = LoadLibrarySpecify(filter_info.vComponents[i].path);
			if (!hmodule)
			{
				
				CLog::GetInstance()->trace(3, L"*********load Dll Failed:"+ filter_info.vComponents[i].path);
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
					CLog::GetInstance()->trace(2, L"Reg Components FAILED!\n");
					if(m_ConType == _E_CON_CSF)
					{
						return false;
					}
					
					//FreeLibrary(hmodule);
					continue;
				}
				else
				{
					CLog::GetInstance()->trace(2, filter_info.vComponents[i].path + L"注册成功\n");
				}
			}
		}
	}
	return true;
}

CComQIPtr<IBaseFilter>	CGraphBuilder::LoadFilter(const CLSID& clsid, QVOD_FILTER_INFO& filter_info)
{
	CComQIPtr<IBaseFilter> pbasefilter;

	// 从XML中获取Filter
	if(!QvodFilterManager::GetInstance().GetFilterByCLSID(clsid, filter_info))
	{
		//mPlayMedia->OutPutInfo(2, L"GetFilterByCLSID failed\n");
		return CComQIPtr<IBaseFilter>();
	}
	
	// 平台系统检查
	if (filter_info.eSystem != e_Apply_All)
	{
		DWORD version = CSystemInfo::GetInstance().GetMajorVersion();
		if (filter_info.eSystem == e_Apply_Vista_Only && version < 6)
			return CComQIPtr<IBaseFilter>();
		if (filter_info.eSystem == e_Apply_XP_Only && version >= 6)
			return CComQIPtr<IBaseFilter>();
	}

	// 设置路径等
	PrepareFilter(filter_info);

	// 装载依赖的组件并注册

	std::vector<HMODULE> modules;
	if(!LoadComponents(filter_info, modules))
	{
		return CComQIPtr<IBaseFilter>();
	}

	// 装载DLL
	HMODULE hmodule = LoadLibrarySpecify(filter_info.path);


	if(!hmodule)
	{	
		DWORD e= GetLastError();
		CLog::GetInstance()->trace(ENABLE_ERROR_INFO, L"load filter failed:");
		CLog::GetInstance()->trace(ENABLE_ERROR_INFO, filter_info.path.GetBuffer(filter_info.path.GetLength()));
		/*if(CLSID_QvodFFmpegSource != clsid)
			m_LossFilters[filter_info.path] = filter_info;*/
		return CComQIPtr<IBaseFilter>();
	}
	else
	{
		CLog::GetInstance()->trace(3, L"load filter Success:");
		CLog::GetInstance()->trace(3, filter_info.path.GetBuffer(filter_info.path.GetLength()));
		CLog::GetInstance()->trace(3, filter_info.strName.GetBuffer());
	}

	HRESULT hr = LoadComCompent(hmodule, clsid, IID_IBaseFilter, (void**)(&pbasefilter));
	
	/*// 创建IBaseFilter接口
	pfnDllGetClassObject	fnDllGetClassObject = (pfnDllGetClassObject) GetProcAddress(hmodule,	"DllGetClassObject");
	if(!fnDllGetClassObject)
	{
		::FreeLibrary(hmodule);
	}
	else
	{
		modules.push_back(hmodule);
		mPlayMedia->OutPutInfo(3, filter_info.path + L"loadfilter success\n");
	}
	
	// Create IBaseFilter
	CLSID _clsid = filter_info.bDMO ? filter_info.wrapid : filter_info.clsid;
	CComPtr<IClassFactory> pClassFactory;
	HRESULT hr = fnDllGetClassObject(_clsid,IID_IClassFactory, (void **)&pClassFactory);
	if(SUCCEEDED(hr) && pClassFactory)
	{
		hr = pClassFactory->CreateInstance(NULL, IID_IBaseFilter, (void**)(&pbasefilter));*/
		if(SUCCEEDED(hr) && pbasefilter)
		{
			if (filter_info.bDMO)
			{
				CComPtr<IDMOWrapperFilter>   pDmoWrapper = NULL;   
				hr = pbasefilter->QueryInterface(IID_IDMOWrapperFilter,  reinterpret_cast<void**>(&pDmoWrapper));  
				if(SUCCEEDED(hr))     
				{
					hr = pDmoWrapper->Init(filter_info.clsid, filter_info.catid);
					if (FAILED(hr))
					{
						CLog::GetInstance()->trace(2, L"Init DMO Wrapper failed\n");
						//SafeRelease(pClassFactory);
						//SafeRelease(pbasefilter);
						pbasefilter.Release();
						return CComQIPtr<IBaseFilter>();
					}
				} 
			}
		}
		else
		{
			CLog::GetInstance()->trace(2, L"Load IBaseFilter filed\n");
		}
		//SafeRelease(pClassFactory);
	//}

 	if (pbasefilter)
 	{
 		if(!AddFiltertoGraph(pbasefilter, filter_info.strName))
 		{
			CLog::GetInstance()->trace(4, filter_info.strName + CString("AddFiltertoGraph failed\n"));
			//SafeRelease(pbasefilter);
			pbasefilter.Release();
 			return CComQIPtr<IBaseFilter>();
 		}
		else
		{
			// 记录加载的组件
			m_Module[pbasefilter] = modules;
		}
 	}

	return pbasefilter;
}


HRESULT	CGraphBuilder::GetContainerByCheckBytes(wchar_t* pPath, e_CONTAINER& container)
{
	CheckPointer(pPath, E_POINTER);

	QVOD_FILTER_MAP filters;
	container = filters.container = _E_CON_UNKNOW;
	filters.vCheckBytes.clear();
	filters.vSuffix.clear();
	filters.vectFilters.clear();

	const std::vector<QVOD_FILTER_MAP>& filterMaps = QvodFilterManager::GetInstance().GetFilterMaps();
	if (filterMaps.empty())
	{
 		return E_FAIL;
 	}

	// 读取一段字节
	int size_data = 32;
	BYTE* datas = new BYTE[size_data];
	memset(datas,0,size_data);
  	/*if(!ReadCheckBytes(datas, size_data))
  	{
  		return false;
  	}*/
	if(!ReadByte(pPath, datas, size_data, 0))
	{
		return E_FAIL;
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
				//if (!ReadCheckBytes(datas, size_data))
				if(!ReadByte(pPath, datas, size_data, 0))
				{
					delete [] datas;
					return E_FAIL;
				}
			}
			if(bOK)
			{
				filters = filterMaps[i];
				delete [] datas;
				container = filters.container;
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

CComQIPtr<IBaseFilter> 	CGraphBuilder::Load_ReaderFilter(CString path, const CLSID& clsid)
{
	CLSID id = (clsid == CLSID_NULL) ? CLSID_AsyncReader : clsid;
	return Load_SourceFilter(path, id);
}

CComQIPtr<IBaseFilter> 	CGraphBuilder::Load_SourceFilter(CString path, const CLSID& clsid)
{
	QVOD_FILTER_INFO info;
	CComQIPtr<IBaseFilter> pibasefilter = LoadFilter(clsid, info);
	if(pibasefilter)
	{
		//加载媒体文件
		CComQIPtr<IFileSourceFilter> pfilesoruce;
		HRESULT hr = pibasefilter->QueryInterface(IID_IFileSourceFilter, (void **)&pfilesoruce);
		if(FAILED(hr))
		{
			ReleaseFilter(pibasefilter);
			//SafeRelease(pibasefilter);
			pibasefilter.Release();
			CLog::GetInstance()->trace(2, L"Load IID_IFileSourceFilter filed\n");
			return CComQIPtr<IBaseFilter>();
		}

		bool bOK = false;
		if(FAILED(pfilesoruce->Load(path, NULL)))
		{
			ReleaseFilter(pibasefilter);
			CLog::GetInstance()->trace(2, L"Load m_Url filed\n");
			//SafeRelease(pfilesoruce);
			//SafeRelease(pibasefilter);
			pfilesoruce.Release();
			pibasefilter.Release();
			return CComQIPtr<IBaseFilter>();
		}
		//SafeRelease(pfilesoruce);
		pfilesoruce.Release();
		// 获取接口
		//QueryDemuxInterface(pibasefilter);
	
		return pibasefilter;
	}
	else
	{
		CLog::GetInstance()->trace(4, L"Load Url filed\n");
		//m_Error = e_BG_Splitter_Loss;
	}
	return CComQIPtr<IBaseFilter>();
}

/*********************************************************************************************
	函数名:     Load_SplitterFilter                                
	函数描述:   根据clsid加载解析filter
	参数1:		const CLSID& clsid，filter clsid
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
CComQIPtr<IBaseFilter> 	CGraphBuilder::Load_SplitterFilter(const CLSID& clsid)
{
	ASSERT(m_pFilterSplitter == NULL);
	ASSERT(m_pFilterFileReader);

	QVOD_FILTER_INFO info;
	CComQIPtr<IBaseFilter> pbasefilter = LoadFilter(clsid, info);
	if(!pbasefilter)
	{
		//m_Error = e_BG_Splitter_Loss;
		return pbasefilter;
	}

	// 连接Splitter与前一个Filter
	CComQIPtr<IBaseFilter> pSrcFilter = GetPreFilterOnPath(_EFT_SPL);
	CComQIPtr<IPin> pOutPin = GetUnconnectedPin(pSrcFilter, PINDIR_OUTPUT);
	if(pOutPin)
	{
		if(SUCCEEDED(ConnectPinToFilers(pOutPin, pbasefilter)))
		{
			//SafeRelease(pOutPin);
			pOutPin.Release();
/*
			//RMVB特殊处理
			if(GetFileProtocol() == _FP_P2PFile)
			{
				if(mpKeyFrameInfoEx)
				{
					LONGLONG IndexPos = 0;
					mpKeyFrameInfoEx->GetIndexPos(&IndexPos);
					
					unsigned __int64 nCurFileLen = IndexPos;
					if(mpQSCallBack)
					{
						nCurFileLen = *(unsigned __int64*)(void*)mpQSCallBack->EventNotify_(QM_MS_GETCURFILELEN, 0, 0);
						mpQSCallBack->EventNotify_(QM_MS_SETINDEXPOS, 0, (long)&IndexPos);
					}
		
					mPlayMedia->CallBackMessage(QM_MEDIA_FILE_INDEX, IndexPos, nCurFileLen - IndexPos);
				}
				
				//qmvb标签下载设置
				if(mpQmvbDemux)
				{
					ULONGLONG tagPos = 0, size = 0;
					if(SUCCEEDED(mpQmvbDemux->GetTagPosition(&tagPos, &size)) && mpQSCallBack)
					{
						CLog::GetInstance()->trace(1, L"QM_MS_QMVB_TAG_POSITION:%I64d\n", tagPos);
						mpQSCallBack->EventNotify_(QM_MS_QMVB_TAG_POSITION, size, (long)&tagPos);
					}
				}
			}*/

			return pbasefilter;
		}
		//SafeRelease(pOutPin);
		pOutPin.Release();
	}

	ReleaseFilter(pbasefilter);
	//SafeRelease(pbasefilter);
	pbasefilter.Release();
	return CComQIPtr<IBaseFilter>();
}

/*********************************************************************************************
	函数名:     Load_VideoDecoderByCLSID
	函数描述:   根据clsid加载视频解码filter
	参数1:		const CLSID& clsid，filter clsid
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
CComQIPtr<IBaseFilter> 	CGraphBuilder::Load_VideoDecoderByCLSID(CComQIPtr<IPin>& pVideoPin, const CLSID& clsid)
{
	CheckPointer(pVideoPin, CComQIPtr<IBaseFilter>());

	QVOD_FILTER_INFO info;
	CComQIPtr<IBaseFilter> pbasefilter = LoadFilter(clsid, info);
	if(!pbasefilter)
	{
		CLog::GetInstance()->trace(2, info.path);
		return CComQIPtr<IBaseFilter>();
	}

	////mpeg2dec，非加速链路限制加速
	//if(clsid == CLSID_QVODMPEG2Decoder && m_eGraphType != E_DXVA_GRAPH)
	//{
	//	CComPtr<IMpeg2Setting> pIMpeg2Setting;
	//	if(SUCCEEDED(pbasefilter->QueryInterface(__uuidof(IMpeg2Setting), (void**)&pIMpeg2Setting)) && pIMpeg2Setting)
	//	{
	//		pIMpeg2Setting->SetDecodeLimit(DCSoft);
	//	}
	//}

	ASSERT(pVideoPin);
	// 获取当前使用的视频流
	if (pVideoPin && SUCCEEDED(ConnectPinToFilers(pVideoPin, pbasefilter)))
	{
		return pbasefilter;
	}
	else
	{
		CLog::GetInstance()->trace(2, L"Connect to decoder failed!");
	}
	ReleaseFilter(pbasefilter);
	//SafeRelease(pbasefilter);
	pbasefilter.Release();
	return CComQIPtr<IBaseFilter>();
}

/*********************************************************************************************
	函数名:     Load_AudioDecoderByCLSID                                
	函数描述:   根据clsid加载音频解码filter
	参数1:		const CLSID& clsid，filter clsid
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
CComQIPtr<IBaseFilter> 	CGraphBuilder::Load_AudioDecoderByCLSID(const CLSID& clsid)
{
	ASSERT(m_pFilterSplitter || m_pFilterSource);

	QVOD_FILTER_INFO info;
	CComQIPtr<IBaseFilter>pbasefilter = LoadFilter(clsid,info);
	if(!pbasefilter)
	{
		return CComQIPtr<IBaseFilter>();
	}

	CComQIPtr<IBaseFilter> pSrc = GetPreFilterOnPath(_EFT_A_DEC);
	bool bOK = ConnectFilter(pSrc, pbasefilter/*, &info*/);
	if(!bOK)
	{
		CLog::GetInstance()->trace(4, L"Load_AudioDecoderByCLSID - ConnectFilter failed\n");
		ReleaseFilter(pbasefilter);
		//SafeRelease(pbasefilter);
		pbasefilter.Release();
		return CComQIPtr<IBaseFilter>();
	}
	return pbasefilter;
}

/*********************************************************************************************
	函数名:     Load_VideoDecoderAuto                                
	函数描述:   自动加载视频解码filter
	返回值：    成功返回IBaseFilter指针，否则返回空指针
*********************************************************************************************/
CComQIPtr<IBaseFilter> 	CGraphBuilder::Load_VideoDecoderAuto(CComQIPtr<IPin>& pVideoPin)
{
	if (!pVideoPin)
	{
		return CComQIPtr<IBaseFilter>();
	}

	BeginEnumMediaTypes(pVideoPin, pEnumMediaTypes, pOutMediaType)
	{
		QVOD_MEDIA_TYPE type = {pOutMediaType->majortype , pOutMediaType->subtype};

		// 获取合适的Filter
		std::vector<CLSID> video_filters;
		QvodFilterManager::GetInstance().GetFilterByMediaType(E_FILTER_CAPACITY_VIDEO_DEC, type, video_filters);

		// 按解码优先级排序
		SortFilterByPriority(video_filters, type);

		for (int j = 0; j < video_filters.size(); j++)
		{
			CComQIPtr<IBaseFilter> pbasefilter = Load_VideoDecoderByCLSID(pVideoPin, video_filters[j]);
			if(pbasefilter)
			{
				return pbasefilter;
			}
		}

	}
	EndEnumMediaTypes(pOutMediaType)
	
	return CComQIPtr<IBaseFilter>();
}

/*********************************************************************************************
	函数名:     Load_AudioDecoderAuto                                
	函数描述:   自动加载音频解码filter
	返回值：    成功返回true，否则返回false
*********************************************************************************************/
bool CGraphBuilder::Load_AudioDecoderAuto()
{
	ASSERT(m_pFilterSplitter || m_pFilterSource);
	CComQIPtr<IBaseFilter> pSrc = GetPreFilterOnPath(_EFT_A_DEC);
	CComQIPtr<IBaseFilter> pbasefilter;
	BeginEnumPins(pSrc, pEnumPins, pOutPin)
	{
		bool bBreakFor = false;
		PIN_DIRECTION pin_direction;
		HRESULT hr = pOutPin->QueryDirection(&pin_direction);
		if(SUCCEEDED(hr) && pin_direction == PINDIR_OUTPUT)
		{
			BeginEnumMediaTypes(pOutPin, pEnumMediaTypes, pOutMediaType)
			{
				if (pOutMediaType->majortype == MEDIATYPE_Video)
				{
					continue;
				}
				
				//// 不需要解码器
				if(pOutMediaType->subtype == MEDIASUBTYPE_PCM)
				{
					continue;
				}

				if (bBreakFor)
				{
					break;
				}
				QVOD_MEDIA_TYPE type = {pOutMediaType->majortype, pOutMediaType->subtype};

				// 获取合适的Filter
				std::vector<CLSID> audio_filters;
				QvodFilterManager::GetInstance().GetFilterByMediaType(E_FILTER_CAPACITY_AUDIO_DEC, type, audio_filters);
				// 按解码优先级排序
				SortFilterByPriority(audio_filters, type);
				for (int i = 0; i < audio_filters.size(); i++)
				{
					QVOD_FILTER_INFO info;
					pbasefilter = LoadFilter(audio_filters[i], info);
					if(pbasefilter)
					{
						if(ConnectFilter(pSrc, pbasefilter/*,&info*/))
						{	
							m_pFilterAudioDecoders.push_back(pbasefilter);
							//DeleteMediaType(pOutMediaType);
							//return true;//多音频流功能
							bBreakFor = true;
							break;
						}
						else
						{
							ReleaseFilter(pbasefilter);
							//SafeRelease(pbasefilter);
							pbasefilter.Release();
						}
					}
				}
			}
			EndEnumMediaTypes(pOutMediaType)
		}
	}
	EndEnumPins

	return !m_pFilterAudioDecoders.empty();
}

/*********************************************************************************************
	函数名:     Load_VideoRenderByClsid                                
	函数描述:   根据clsid加载视频渲染器
	返回值：    成功返回true，否则返回false
*********************************************************************************************/
bool CGraphBuilder::Load_VideoRenderByClsid(/*const CLSID& clsid*/ CLSID clsid)
{
	QVOD_FILTER_INFO info;
	m_pFilterVideoRender = LoadFilter(clsid, info);
	if(!m_pFilterVideoRender)
	{
		CLog::GetInstance()->trace(4, L"VideoRender加载失败\n");
		return false;
	}

	CComQIPtr<IBaseFilter> pSrc = GetPreFilterOnPath(_EFT_VIDEO_RENDER);
	ASSERT(pSrc);
	AM_MEDIA_TYPE type;
	if(FAILED(ConnectFilterDirect(pSrc, m_pFilterVideoRender, &type)))
	{
		CLog::GetInstance()->trace(3, L"FAILED(Load_VideoRenderByClsid ConnectFilterDirect(pSrc, mpFilterVideoRender, &type) failed\n");
		return false;
	}

	return true;
}

/*********************************************************************************************
	函数名:     ConnectAudioRenderAuto                                
	函数描述:   自动连接音频渲染器
	返回值：    成功返回true，否则返回false
*********************************************************************************************/
bool CGraphBuilder::ConnectAudioRenderAuto()
{
	if (!m_pFilterAudioRender)
	{
		if(!CreateAudioRender())
		{
			QVOD_FILTER_INFO info;
			m_pFilterAudioRender = LoadFilter(CLSID_NULLRender, info);
		}
	}

	if(m_pFilterAudioRender)
	{
		CComQIPtr<IBaseFilter> pSrc = GetPreFilterOnPath(_EFT_AUDIO_RENDER);
		if (!pSrc)
		{
			ReleaseFilter(m_pFilterAudioRender);
			m_pFilterAudioRender.Release();
		}

		if (pSrc && FAILED(ConnectFilterDirect(pSrc, m_pFilterAudioRender)))
		{
			ReleaseFilter(m_pFilterAudioRender);
			m_pFilterAudioRender.Release();
		}
		
		if(pSrc && NULL == m_pFilterAudioRender)
		{
			QVOD_FILTER_INFO info;
			m_pFilterAudioRender = LoadFilter(CLSID_NULLRender, info);
			if(FAILED(ConnectFilterDirect(pSrc, m_pFilterAudioRender)))
			{
				ReleaseFilter(m_pFilterAudioRender);
				m_pFilterAudioRender.Release();
				return false;
			}
		}
	}

	
	return true;
}

/*********************************************************************************************
	函数名:     Load_VideoRenderAuto                                
	函数描述:   自动连接视频渲染器
	返回值：    成功返回true，否则返回false
*********************************************************************************************/
bool CGraphBuilder::Load_VideoRenderAuto()
{
	std::vector<GUID> renders;
	if (m_ConType == _E_CON_CSF)
	{
		renders.push_back(CLSID_CsfRenderer);
	}
	else
	{
 		DWORD version = CSystemInfo::GetInstance().GetMajorVersion();
  		if (version >= 6)
  		{
  			renders.push_back(CLSID_EnhancedVideoRenderer);
  		}
  		
  		renders.push_back(CLSID_VideoMixingRenderer);
  		
  		renders.push_back(CLSID_VideoMixingRenderer9);
  		
		renders.push_back(CLSID_VideoRenderer);
	}
	
	bool bRet =  false;
	QVOD_FILTER_INFO info;
	GUID idNow = GUID_NULL;
	for (int i = 0; i < renders.size(); i++)
	{
		if (m_pFilterVideoRender)
			ReleaseFilter(m_pFilterVideoRender);
		
		idNow = renders[i];
		if(bRet = Load_VideoRenderByClsid(idNow))
		{
			break;
		}
	}

	return bRet;
}

bool CGraphBuilder::Load_DVDNavigater(wchar_t* pPath)
{
	CheckPointer(pPath, E_POINTER);

	HRESULT hr = CoCreateInstance(CLSID_DVDNavigator, NULL,CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pFilterSource);
	if(!SUCCEEDED(hr))
	{
		CLog::GetInstance()->trace(2, L" CoCreateInstance CLSID_DVDNavigator Failed!");
		return false;
	}

	if(!AddFiltertoGraph(m_pFilterSource, L"DVD Navigator"))
	{
		//SafeRelease(m_pFilterSource);
		m_pFilterSource.Release();
		return false;
	}

	hr = m_pFilterSource->QueryInterface(IID_IDvdControl2, (void **)&m_pDvdControl);
	if(FAILED(hr))
	{
		//SafeRelease(m_pFilterSource);
		m_pFilterSource.Release();
		return false;
	}
	
	CString dvdurl(pPath);
 	if(FAILED(m_pDvdControl->SetDVDDirectory(pPath)) 
 		&& FAILED(m_pDvdControl->SetDVDDirectory(dvdurl + L"VIDEO_TS")) 
 		&& FAILED(m_pDvdControl->SetDVDDirectory(dvdurl + L"\\VIDEO_TS")))
	{
		//SafeRelease(m_pFilterSource);
		m_pFilterSource.Release();
		//SafeRelease(m_pDvdControl);
		m_pDvdControl.Release();
		CLog::GetInstance()->trace(2, L" SetDVDDirectory Failed!");
		return false;
	}

	hr = m_pFilterSource->QueryInterface(IID_IDvdInfo2, (void **)&m_pIDvdInfo2);
	if(FAILED(hr))
	{
		//SafeRelease(m_pFilterSource);
		m_pFilterSource.Release();
		//SafeRelease(m_pDvdControl);
		m_pDvdControl.Release();
		//SafeRelease(m_pIDvdInfo2);
		m_pIDvdInfo2.Release();
		return false;
	}

	if (m_pDvdControl)
	{
		m_pDvdControl->SetOption(DVD_ResetOnStop, FALSE);
		m_pDvdControl->SetOption(DVD_NotifyParentalLevelChange, FALSE);
		m_pDvdControl->SetOption(DVD_HMSF_TimeCodeEvents, TRUE);
	}

	CLog::GetInstance()->trace(2, L" Load_DVDNavigater succeeded!");
	return true;
}

// 排序函数
void CGraphBuilder::SortFilterByPriority(std::vector<CLSID>& filters, QVOD_MEDIA_TYPE& mt)
{
	PRIORITY_SORT::mt = mt;
	std::sort(filters.begin(), filters.end(), PRIORITY_SORT());
}

bool CGraphBuilder::CreateAudioRender()
{
	if(m_ConType == _E_CON_MIDI)
	{
		HRESULT hr = CoCreateInstance(CLSID_AVIMIDIRender, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&m_pFilterAudioRender);
		if(SUCCEEDED(hr))
		{
			m_SoundDevice = L"MIDI Render";
			IAMResourceControl* pIAMResourceControl= NULL;
			hr = m_pFilterAudioRender->QueryInterface(IID_IAMResourceControl, (void**)&pIAMResourceControl);
			ASSERT(hr == S_OK && pIAMResourceControl);
			hr = pIAMResourceControl->Reserve(AMRESCTL_RESERVEFLAGS_RESERVE, NULL);
			if (FAILED(hr))
			{
				CLog::GetInstance()->trace(3, L"MIDI IAMResourceControl::Reserve FAILED!\n");
			}
			SafeRelease(pIAMResourceControl);
		}
	}
	else
	{
		long audioDevice = 0;
		//mPlayMedia->GetCurAudioRender(&audioDevice);

		if (!audioDevice)
		{
			HRESULT hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&m_pFilterAudioRender);
			if(FAILED(hr))
			{
				//SafeRelease(mpFilterAudioRender);
				m_pFilterAudioRender.Release();
			}
			m_SoundDevice = L"Default DirectSound Device";
		}
		else
		{
			// 遍历，找出
			ICreateDevEnum *pSysDevEnum = NULL;
			HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
			if (FAILED(hr))
			{
				return NULL;
			}

			IEnumMoniker *pEnumMoniker = NULL;
			hr = pSysDevEnum->CreateClassEnumerator(CLSID_AudioRendererCategory, &pEnumMoniker, 0);
			if(FAILED(hr))
			{	
				SafeRelease(pSysDevEnum);
				return NULL;
			}
			pEnumMoniker->Reset();
			IMoniker *pMoniker = NULL;
			long norder = 0;
			for (pEnumMoniker; pEnumMoniker->Next(1, &pMoniker, 0) == S_OK;)
			{
				LPOLESTR olestr = NULL;
				if(FAILED(pMoniker->GetDisplayName(0, 0, &olestr)))
				{
					SafeRelease(pMoniker);
					continue;
				}

				IPropertyBag *pPB = NULL;
				if(SUCCEEDED(pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPB)))
				{
					CComVariant var;
					GUID gaudio;
					CString fstr;
					if(SUCCEEDED(pPB->Read(CComBSTR(_T("FriendlyName")), &var, NULL)))
					{
						m_SoundDevice  = var.bstrVal;
						var.Clear();
					}
					if(SUCCEEDED(pPB->Read(CComBSTR(_T("CLSID")), &var, NULL)))
					{
						CLSIDFromString(var.bstrVal, &gaudio);
						var.Clear();
					}
					/*if(gaudio != CLSID_DSoundRender && gaudio != CLSID_AudioRender)
					{
						SafeRelease(pPB);
						SafeRelease(pMoniker);
						continue;
					}*/

					if(audioDevice == 0 && CLSID_DSoundRender == gaudio)
					{//default
						pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pFilterAudioRender);
						break;				
					}
					if(norder == audioDevice-1)
					{
						pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pFilterAudioRender);
						break;	
					}
					SafeRelease(pPB);
				}
				norder++;
				SafeRelease(pMoniker);
			}
			SafeRelease(pEnumMoniker);
			SafeRelease(pSysDevEnum);
		}
	}

	if (m_pFilterAudioRender)
	{
		if(!AddFiltertoGraph(m_pFilterAudioRender,m_SoundDevice))
		{
			return false;
		}
	}

	return m_pFilterAudioRender?true:false;
}