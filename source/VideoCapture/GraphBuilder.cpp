#include "stdafx.h"
#include "GraphBuilder.h"

const GUID MEDIATYPE_Subtitle  =   {0xe487eb08, 0x6b26, 0x4be9, {0x9d, 0xd3, 0x99, 0x34, 0x34, 0xd3, 0x13, 0xfd}};
CGraphBuilder::CGraphBuilder(void)
{
	m_dwRegister = 0;
}

CGraphBuilder::~CGraphBuilder(void)
{
}

CComQIPtr<IGraphBuilder> CGraphBuilder::LoadFilterGraph()
{
	HRESULT hr = E_FAIL;
	CComQIPtr<IGraphBuilder> pGraph;
	//加载quartz.dll
	HMODULE hmd = LoadLibrary(L"Quartz.dll");
	
	if(hmd == NULL)
	{
		return CComQIPtr<IGraphBuilder>();
	}
	//获得DllGetClassObject函数地址
	fnDllGetClassObject pfnDllGetClassObject = (fnDllGetClassObject) GetProcAddress(hmd,	"DllGetClassObject");
	if(!pfnDllGetClassObject)
	{
		::FreeLibrary(hmd);
		return CComQIPtr<IGraphBuilder>();
	}

	if(pfnDllGetClassObject)
	{
		CComPtr<IClassFactory> pClassFactory;
		//获得类工厂接口指针
		hr = pfnDllGetClassObject(CLSID_FilterGraph, IID_IClassFactory, (void **)&pClassFactory);
		if(SUCCEEDED(hr) && pClassFactory)
		{
			bool reg = false;
			while(true)
			{
				pGraph.Release();
				//创建Graph对象实例
				hr = pClassFactory->CreateInstance(NULL, IID_IFilterGraph, (void **)&pGraph);
				if(FAILED(hr) && !reg)//如果失败则先注册该dll（Quartz.dll）
				{
					pfnRegister pfnReg = NULL;
					pfnReg = (pfnRegister) GetProcAddress(hmd, "DllRegisterServer");
					hr = pfnReg();
					reg = true;
				}
				else//注册后仍无法加载，则失败退出
				{
					break;
				}
			}
		}
	}
	::FreeLibrary(hmd);

	if(FAILED(hr))
	{
		pGraph.Release();
	}

#ifdef SHOW_GRAPH_IN_GRAPHEDIT
	//将Graph对象添加到rot中
	AddToRot(pGraph, &m_dwRegister);
#endif

	return pGraph;
}

HRESULT CGraphBuilder::ConnectFilter(CComQIPtr<IBaseFilter> pSrc, CComQIPtr<IBaseFilter> pDst, bool bconnectall)
{
	// 获取Dest Filter的输入媒体类型
	CheckPointer(pSrc, E_POINTER);
	CheckPointer(pDst, E_POINTER);

	HRESULT hr = E_FAIL;
	CComPtr<IEnumPins> pEnumPins;

	// 循环枚举上游filter的每个输出PIN
	BeginEnumPins(pSrc, pEnumPins, pOutPin)
	{
		PIN_DIRECTION out_direction;
		hr = pOutPin->QueryDirection(&out_direction);
		if(SUCCEEDED(hr) && out_direction == PINDIR_OUTPUT)
		{
			// 循环枚举输出PIN的媒体类型
			CComPtr<IEnumMediaTypes> pEnumMediaTypes; 
			if(pOutPin && SUCCEEDED(pOutPin->EnumMediaTypes(&pEnumMediaTypes))) 
			{ 
				pEnumMediaTypes->Reset();
				AM_MEDIA_TYPE* pMediaType = NULL;
				pEnumMediaTypes->Next(1, &pMediaType, NULL); 
				do
				{
					hr = E_FAIL;
					// 枚举输出下游filter的输入pin
					BeginEnumPins(pDst, pDecoderEnumPins, pInPin)
					{
						// 找到
						if (pMediaType == NULL || SUCCEEDED(pInPin->QueryAccept(pMediaType)))
						{
							CComQIPtr<IPin> pConnectto;
							pInPin->ConnectedTo(&pConnectto);
							if(pConnectto)
							{
								pConnectto.Release();
								continue;
							}
							// 连接两个PIN
							hr =  m_pGraph->ConnectDirect(pOutPin,pInPin,pMediaType);
							
						}
					}
					EndEnumPins

					if(pMediaType) 
					DeleteMediaType(pMediaType);

					if(SUCCEEDED(hr))
					{
						if(!bconnectall)
						{
							return hr;
						}
						else
						{
							break;
						}
					}
				}while(S_OK ==  pEnumMediaTypes->Next(1, &pMediaType, NULL));
			}
		}
	}
	EndEnumPins
	return hr;
}

HRESULT CGraphBuilder::ConnectFilterThrough(CComQIPtr<IBaseFilter> pUp, CComQIPtr<IBaseFilter> pDown,  CComQIPtr<IBaseFilter> pthrough)
{
	CheckPointer(pUp, E_POINTER);
	CheckPointer(pDown, E_POINTER);
	CheckPointer(pthrough, E_POINTER);

	HRESULT hr = E_FAIL;
	if(FAILED(hr = ConnectFilter(pUp, pthrough)))
	{
		return hr;
	}

	if(FAILED(hr = ConnectFilter(pthrough, pDown)))
	{
		m_pGraph->RemoveFilter(pthrough);
		return hr;
	}

	return hr;
}

HRESULT CGraphBuilder::ConnectPinToFilter(CComQIPtr<IPin> pOutPin, CComQIPtr<IBaseFilter> pDst)
{
	CheckPointer(pOutPin, E_POINTER);
	CheckPointer(pDst, E_POINTER);
	
	CComQIPtr<IEnumPins> pEnum;
	CComQIPtr<IPin> pPin;

	// 获得目标filter pin枚举器
	HRESULT hr = pDst->EnumPins(&pEnum);
	if (FAILED(hr) || pEnum == NULL)
	{
		return hr;
	}

	// 循环枚举目标filter输入pin
	while(pEnum->Next(1,&pPin, NULL) == S_OK)
	{
		PIN_DIRECTION thisPinDir;
		pPin->QueryDirection(&thisPinDir);
		if(thisPinDir == PINDIR_INPUT)
		{
			CComQIPtr<IPin> pTmp;
			hr = pPin->ConnectedTo(&pTmp);
			if(SUCCEEDED(hr))
			{
				//已连接，继续寻找
				continue;
			}
			else
			{
				// 尝试连接这两个pin
				hr = m_pGraph->ConnectDirect(pOutPin,pPin, NULL);
				if(SUCCEEDED(hr))
				{
					return hr;
				}
			}
		}
		pPin.Release();
	}
	return E_FAIL;
}

HRESULT CGraphBuilder::ConnectSubTitlePins(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pVobSubFilter)
{
	// 获取Dest Filter的输入媒体类型
	if (!pSrc || !pVobSubFilter)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	int count = 0;
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
							// 是否已连接
							CComQIPtr<IPin> pConnectto;
							pInPin->ConnectedTo(&pConnectto);
							if(pConnectto)
							{
								// 已连接
								pConnectto.Release();
								continue;
							}

							// 未连接，连接两个PIN
							hr =  m_pGraph->ConnectDirect(pOutPin,pInPin,pOutMediaType);
							if(SUCCEEDED(hr))
							{
								count++;
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
	return count > 0 ? S_OK : E_FAIL;
}

CComQIPtr<IPin> CGraphBuilder::GetUnconnectedPin(CComQIPtr<IBaseFilter>& pFilter,PIN_DIRECTION pinDir)
{
	CheckPointer(pFilter, CComQIPtr<IPin>());

	CComQIPtr<IEnumPins> pEnum;
	CComQIPtr<IPin> pPin;

	//获目标Filter的pin枚举对象接口
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if(FAILED(hr) || pEnum == NULL)
	{
		return CComQIPtr<IPin>();
	}

	//枚举目标filter的pins对象
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
				//已连接，继续寻找
				pTmp.Release();
			}
			else
			{
				//找到，返回
				return pPin;
			}
		}
	}
	return CComQIPtr<IPin>();
}

CComQIPtr<IPin>	CGraphBuilder::GetSpecyPin(CComPtr<IBaseFilter> pFilter, PIN_DIRECTION pinDir, GUID Majoytype)
{
	CheckPointer(pFilter, CComQIPtr<IPin>());

	CComQIPtr<IEnumPins> pEnum;
	CComQIPtr<IPin> pPin;

	HRESULT hr = pFilter->EnumPins(&pEnum);
	if(FAILED(hr) || pEnum)
	{
		return CComQIPtr<IPin>();
	}

	while(pEnum->Next(1,&pPin, NULL) == S_OK)
	{
		PIN_DIRECTION thisPinDir;
		pPin->QueryDirection(&thisPinDir);
		if(thisPinDir == pinDir)
		{
			CComQIPtr<IPin> pTmp;
			hr = pPin->ConnectedTo(&pTmp);
			if(SUCCEEDED(hr))
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
	return CComQIPtr<IPin>();
}

void CGraphBuilder::DestroyGraph()
{
	if(m_pGraph)
	{
		//销毁Graph里面的残余filter
 		IEnumFilters *pEnum = NULL;
 		IBaseFilter *pBF[20]= {0};
 		ULONG fetched = 20;
 		HRESULT hr = m_pGraph->EnumFilters(&pEnum);
 		if(SUCCEEDED(hr) && pEnum)
 		{
 			pEnum->Next(20, pBF, &fetched);
			for(int i =0; i < fetched; i++)
			{
				hr = m_pGraph->RemoveFilter(pBF[i]);
				int ret = pBF[i]->Release();
			}
 			pEnum->Release();
 		} 
	
		m_pGraph.Release();
	}
}

int CGraphBuilder::ReleaseFilter(CComQIPtr<IBaseFilter>& pbasefilter)
{
	if (pbasefilter)
	{
		if(m_pGraph)
		{
			HRESULT hr = m_pGraph->RemoveFilter(pbasefilter);
			ASSERT(hr == S_OK);
		}
		ASSERT(pbasefilter != NULL);

		IBaseFilter* pbf = pbasefilter;
		pbf->AddRef();
		int ret = pbf->Release();
		//ASSERT(ret == 1);
		pbasefilter.Release();
		return --ret;
	}

	return -1;
}


HRESULT CGraphBuilder::AddToFilterGraph(CComQIPtr<IBaseFilter>& pbasefilter,  TCHAR* pName)
{
	CheckPointer(pbasefilter, E_POINTER);
	CheckPointer(m_pGraph, E_FAIL);

	HRESULT hr = m_pGraph->AddFilter(pbasefilter, pName);
	if(FAILED(hr))
	{
		//output debug info
	}

	return hr;
}

HRESULT	CGraphBuilder::RemoveFilterFromGraph(CComQIPtr<IBaseFilter>& pbasefilter)
{
	if(!pbasefilter || !m_pGraph)
		return false;

	return m_pGraph->RemoveFilter(pbasefilter);
}

HRESULT	CGraphBuilder::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
	CComQIPtr<IMoniker> pMoniker;
	CComQIPtr<IRunningObjectTable> pROT;

	//获得操作系统运行时对象表
	if(FAILED(GetRunningObjectTable(0, (LPRUNNINGOBJECTTABLE *)&pROT)) || pROT == NULL)
	{
		return E_FAIL;
	}

	//创建监视器
	TCHAR wsz[256];
	wsprintf(wsz,_T("FilterGraph %08x pid %08x"), (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
	HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if(SUCCEEDED(hr))
	{
		//在运行时对象表中注册Graph链路
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,	pMoniker, pdwRegister);
	}

	return hr;
}

HRESULT CGraphBuilder::RemoveFromRot(DWORD dwRegister)
{
	HRESULT hr = E_FAIL;
	CComQIPtr<IRunningObjectTable> pROT;

	//获得操作系统运行时对象表
	if(SUCCEEDED(hr = GetRunningObjectTable(0, &pROT)))
	{
		//从操作系统运行时对象表中移除
		hr = pROT->Revoke(dwRegister);
	}

	return hr;
}

bool CGraphBuilder::CheckByte(const TCHAR * pchkbytes, const BYTE* pDatas, int dataSize, int& needMoreData)
{
	if (!pchkbytes || !pDatas)
	{
		return false;
	}

	needMoreData = 0;

	std::vector<CString> vcheckbytes;
	int nlen = _tcslen(pchkbytes);
	const TCHAR *pc = pchkbytes;
	CString strtemp;
	while(nlen-- > 0)
	{
		if(*pc == ',')
		{
			if(strtemp.CompareNoCase(L",") == 0)
			{
				strtemp.Empty();
			}
			vcheckbytes.push_back(strtemp);
			strtemp.Empty();
		}
		else
		{
			strtemp += (*pc);
		}
		pc++;
	}
	vcheckbytes.push_back(strtemp);
	if(vcheckbytes.size() < 4)
	{	
		return false;
	}
	for (int i = 0; i < vcheckbytes.size(); i += 4)
	{
		CString offsetstr = vcheckbytes[i];
		CString cbstr = vcheckbytes[i+1];
		CString maskstr = vcheckbytes[i+2];
		CString valstr = vcheckbytes[i+3];
		long cb = _ttol(cbstr);

		if(offsetstr.IsEmpty() || cbstr.IsEmpty() 
			|| valstr.IsEmpty() || (valstr.GetLength() & 1)
			|| cb*2 != valstr.GetLength())
			return false;

		int pos = int(pDatas) + (int)_ttoi64(offsetstr);
		if (pos >  (int)pDatas + dataSize - valstr.GetLength()/2)
		{
			// 数据不够，要求更多数据
			needMoreData = (int)_ttoi64(offsetstr) + 64;
			return false;
		}

		// LAME
		while(maskstr.GetLength() < valstr.GetLength())
			maskstr += 'F';
		valstr.TrimLeft();
		valstr.TrimRight();
		BYTE *pmask = new BYTE[maskstr.GetLength()/2];
		BYTE *pval  = new BYTE[valstr.GetLength()/2];
		int nvallen = valstr.GetLength()/2;
		CStringtoBin(maskstr, pmask);
		CStringtoBin(valstr, pval);

		for(size_t i = 0; i < nvallen; i++, pos++)
		{
			BYTE b = *((BYTE*)pos);
			if( (b & pmask[i]) != pval[i])
			{
				SafeDeleteArray(pmask);
				SafeDeleteArray(pval);
				return false;
			}
		}
		delete[] pmask;
		delete[] pval;
	}
	return true;
}

void CGraphBuilder::CStringtoBin(CString str, BYTE *pdata)
{
	ASSERT(pdata);
	BYTE b = 0;
	str.Trim();
	str.MakeUpper();

	for(int i = 0, j = str.GetLength(); i < j; i++)
	{
		TCHAR c = str[i];
		if(c >= '0' && c <= '9') 
		{
			if(!(i&1)) b = ((char(c-'0')<<4)&0xf0)|(b&0x0f);
			else b = (char(c-'0')&0x0f)|(b&0xf0);
		}
		else if(c >= 'A' && c <= 'F')
		{
			if(!(i&1)) b = ((char(c-'A'+10)<<4)&0xf0)|(b&0x0f);
			else b = (char(c-'A'+10)&0x0f)|(b&0xf0);
		}
		else break;

		if(i&1)
		{
			//data[i>>1] = b;
			pdata[i>>1] = b;
			b = 0;
		}
	}
}