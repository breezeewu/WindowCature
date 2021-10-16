#include <atlstr.h>
#include <streams.h>
#include "GraphBaseBuilder.h"
#include "Reg.h"


CGraphBaseBuilder::CGraphBaseBuilder(void)
{
}

CGraphBaseBuilder::~CGraphBaseBuilder(void)
{
}
// 加载FilterGraph
CComQIPtr<IGraphBuilder> CGraphBaseBuilder::LoadFilterGraph()
{
	//加锁
	CLazyLock Lock(m_CriSec);

	//加载Quartz.dll
	HMODULE hmd = LoadLibrarySpecify(L"Quartz.dll");

	if(hmd)
	{
		HRESULT hr = S_OK;
		if(FAILED(hr = LoadComCompent(hmd, CLSID_FilterGraph, IID_IFilterGraph, (void**)&m_pGraph)))
		{
			pfnDllRegisterServer pfnReg = NULL;
			pfnReg = (pfnDllRegisterServer) GetProcAddress(hmd, "DllRegisterServer");
			hr = pfnReg();
			hr = LoadComCompent(hmd, CLSID_FilterGraph, IID_IFilterGraph, (void**)&m_pGraph);
			return m_pGraph;
		}
	}

	return m_pGraph;
}

#ifdef SHOW_GRAPH_IN_GRAPHEDIT
//添加GraphBuilder到运行时对象表
HRESULT	CGraphBaseBuilder::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
	//检测输入参数的合法性
	CheckPointer(pUnkGraph, E_POINTER);
	CheckPointer(pdwRegister, E_POINTER);

	//加锁
	CLazyLock Lock(m_CriSec);

	IMoniker * pMoniker;
	IRunningObjectTable *pROT;

	//获取运行时对象表ROT
	if(FAILED(GetRunningObjectTable(0, &pROT)))
	{
		return E_FAIL;
	}

	WCHAR wsz[256];
	wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());

	//创建监视器
	HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if(SUCCEEDED(hr))
	{
		//注册Graph
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,	pMoniker, pdwRegister);
		pMoniker->Release();
	}

	//销毁ROT
	pROT->Release();

	return hr;
}

//从运行时对象表中移除GraphBuilder
void CGraphBaseBuilder::RemoveFromRot(DWORD pdwRegister)
{
	//加锁
	CLazyLock Lock(m_CriSec);

	IRunningObjectTable *pROT;

	//获取运行时对象表ROT
	if(SUCCEEDED(GetRunningObjectTable(0, &pROT)))
	{
		//从运行时对象表中移除
		HRESULT hr = pROT->Revoke(pdwRegister);
		if(FAILED(hr))
		{
			//mPlayMedia->OutPutInfo(2, L"Revoke ROT FAILED!\n");
		}
		pROT->Release();
	}
}
#endif
                             
//释放filter
int	CGraphBaseBuilder::ReleaseFilter(CComQIPtr<IBaseFilter>& pbasefilter)
{
	//检测输入参数的合法性
	CheckPointer(pbasefilter, -1);

	//加锁
	CLazyLock Lock(m_CriSec);

	//释放filter
	if (pbasefilter)
	{
		IBaseFilter* filter = pbasefilter;
		HRESULT hr = m_pGraph->RemoveFilter(pbasefilter);
		ASSERT(pbasefilter != NULL);

		IBaseFilter* pfilter = pbasefilter.p;
		pfilter->AddRef();
		int ret = pfilter->Release();
		pbasefilter.Release();
		//返回释放后的引用数
		return --ret;
	}

	return -1;
}

//添加filter到graph
bool CGraphBaseBuilder::AddFiltertoGraph(CComQIPtr<IBaseFilter>& pbasefilter,  LPCWSTR pName)
{
	//检测输入参数的合法性
	CheckPointer(pbasefilter, false);

	//加锁
	CLazyLock Lock(m_CriSec);

	HRESULT hr = m_pGraph->AddFilter(pbasefilter, pName);
	if(FAILED(hr))
	{
		//失败，打印警告信息
		CLog::GetInstance()->trace(ENABLE_WARNING_INFO, L"AddFiltertoGraph failed, erroecode:%0x\n", hr);
	}

	return SUCCEEDED(hr);
}

//连接两个filter
bool CGraphBaseBuilder::ConnectFilter(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pDest)
{
	
	//检测输入参数的合法性
	CheckPointer(pSrc, false);
	CheckPointer(pDest, false);

	//加锁
	CLazyLock Lock(m_CriSec);

	// 循环每个输出PIN
	bool ret = false;
	BeginEnumPins(pSrc, pEnumPins, pOutPin)
	{	
		PIN_DIRECTION out_direction;

		//查询输出pin的方向
		HRESULT hr = pOutPin->QueryDirection(&out_direction);
		if(SUCCEEDED(hr) && out_direction == PINDIR_OUTPUT)
		{
			// 循环枚举输出PIN的媒体类型
			CComPtr<IEnumMediaTypes> pEnumMediaTypes; 
			if(pOutPin && SUCCEEDED(pOutPin->EnumMediaTypes(&pEnumMediaTypes))) 
			{ 
				pEnumMediaTypes->Reset();
				AM_MEDIA_TYPE* pMediaType = NULL; 
				HRESULT rr = E_FAIL;
				//逐个枚举输出pin支持的媒体类型
				for(; S_OK == (rr = pEnumMediaTypes->Next(1, &pMediaType, NULL)); DeleteMediaType(pMediaType), pMediaType = NULL) 
				{ 
					// 找到输入PIN
					bool bFind = false;
					BeginEnumPins(pDest, pDecoderEnumPins, pInPin)
					{
						bFind = true;
						// 找到
						if (SUCCEEDED(pInPin->QueryAccept(pMediaType)))
						{
							CComQIPtr<IPin> pConnectto;
							//检测是否已连接
							pInPin->ConnectedTo(&pConnectto);
							if(pConnectto)
							{
								//已连接，继续查找
								pConnectto.Release();
								continue;
							}

							// 连接两个PIN
							hr =  m_pGraph->ConnectDirect(pOutPin,pInPin,pMediaType);
							if(SUCCEEDED(hr))
							{
								//连接成功
								ret = true;
							}
							else
							{
								//失败，打印警告信息
								CLog::GetInstance()->trace(ENABLE_WARNING_INFO, L"ConnectFilter ConnectDirect failed, erroecode:%0x\n", hr);
							}
						}
					}
					EndEnumPins
					ASSERT(bFind);	
					
				}

				//删除媒体类型
				if(pMediaType)
				{
					DeleteMediaType(pMediaType);
				}
			}
		}
	}
	EndEnumPins

	return ret;//false;
}

//通过指定的媒体类型连接filter
HRESULT	CGraphBaseBuilder::ConnectFilterDirect(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pDest,AM_MEDIA_TYPE* pConnMediaType)
{
	//检测输入参数的合法性
	CheckPointer(pSrc, E_POINTER);
	CheckPointer(pDest, E_POINTER);
	CheckPointer(pConnMediaType, E_POINTER);

	//加锁
	CLazyLock Lock(m_CriSec);

	// 尝试不指定类型连接
	BeginEnumPins(pSrc, pEnumPins, pOutPin)
	{
		PIN_DIRECTION thisPinDir;
		//查询方向
		pOutPin->QueryDirection(&thisPinDir);
		if (thisPinDir == PINDIR_OUTPUT)
		{
			//输出Pin
			CComQIPtr<IPin> pTmp;
			pOutPin->ConnectedTo(&pTmp);
			if (!pTmp)
			{
				HRESULT hr = E_FAIL;

				//连接Filter
				if(SUCCEEDED(hr = ConnectPinToFilers(pOutPin, pDest)))
				{
					if (pConnMediaType)
					{
						//获取连接媒体类型
						if(SUCCEEDED(hr = pOutPin->ConnectionMediaType(pConnMediaType)))
						{
							
						}
						else
						{
							//失败，打印警告信息
							CLog::GetInstance()->trace(ENABLE_WARNING_INFO, L"ConnectFilterDirect ConnectionMediaType failed, erroecode:%0x\n", hr);
						}
					}
					return S_OK;
				}
			}
		}
	}
	EndEnumPins
	return E_FAIL;
}

//连接pin与filter
HRESULT	CGraphBaseBuilder::ConnectPinToFilers(CComPtr<IPin>& pOutPin, CComQIPtr<IBaseFilter>& pDest)
{
	//检测输入参数的合法性
	CheckPointer(pOutPin, E_POINTER);
	CheckPointer(pDest, E_POINTER);

	//加锁
	CLazyLock Lock(m_CriSec);
	
	//其实情况，不作要求
	CComQIPtr<IEnumPins> pEnum;
	CComQIPtr<IPin> pPin;

	//获取Filter Pin枚举器
	HRESULT hr = pDest->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		//失败，打印警告信息
		CLog::GetInstance()->trace(ENABLE_WARNING_INFO, L"ConnectPinToFilers EnumPins failed, erroecode:%0x\n", hr);
		return hr;
	}

	//逐个枚举Filter pin
	while(pPin.Release(), pEnum->Next(1,&pPin, NULL) == S_OK)
	{
		PIN_DIRECTION thisPinDir;
		pPin->QueryDirection(&thisPinDir);
		if (thisPinDir == PINDIR_INPUT)
		{
			//该Filter pin为输入pin
			CComQIPtr<IPin> pTmp;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))
			{
				//已连接，销毁Filter pin，继续寻找
				pTmp.Release();
				continue;
			}
			else
			{
				//找到未连接pin，连接
				hr = m_pGraph->ConnectDirect(pOutPin, pPin, NULL);
				if (SUCCEEDED(hr))
				{
					//连接成功，返回
					return hr;
				}
				else
				{
					//失败，打印警告信息
					CLog::GetInstance()->trace(ENABLE_WARNING_INFO, L"ConnectPinToFilers ConnectionMediaType failed, erroecode:%0x\n", hr);
				}
			}
		}
		pPin.Release();
	}

	//无法连接，返回失败
	return E_FAIL;

}

//连接字幕pin到vobsub
bool CGraphBaseBuilder::ConnectSubTitilePins(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pSubFilter)
{
	//检测输入参数的合法性
	CheckPointer(pSrc, false);
	CheckPointer(pSubFilter, false);

	//加锁
	CLazyLock Lock(m_CriSec);

	bool ret = false;
	// 获取Dest Filter的输入媒体类型
	// 循环每个输出PIN
	BeginEnumPins(pSrc, pEnumPins, pOutPin)
	{	
		PIN_DIRECTION out_direction;
		HRESULT hr = pOutPin->QueryDirection(&out_direction);
		if(SUCCEEDED(hr) && out_direction == PINDIR_OUTPUT)
		{
			// 循环输出PIN的媒体类型
			BeginEnumMediaTypes(pOutPin, pEnumMediaTypes, pOutMediaType)
			{
				if (pOutMediaType->majortype == MEDIATYPE_Subtitle 
					|| (pOutMediaType->majortype == MEDIATYPE_DVD_ENCRYPTED_PACK && pOutMediaType->subtype == MEDIASUBTYPE_DVD_SUBPICTURE))
				{			
					// 找到输入PIN
					BeginEnumPins(pSubFilter, pDecoderEnumPins, pInPin)
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
// 								//连接成功
								ret = true;
							}
						}
					}
					EndEnumPins
				}
			}
			//完成此次枚举，删除媒体类型
			EndEnumMediaTypes(pOutMediaType)
		}
	}
	EndEnumPins
	return ret;
}


//获取未连接的pin
CComQIPtr<IPin>	CGraphBaseBuilder::GetUnconnectedPin(CComQIPtr<IBaseFilter>& pFilter, PIN_DIRECTION pinDir)
{
	//检测输入参数的合法性
	CheckPointer(pFilter, CComQIPtr<IPin>());

	//加锁
	CLazyLock Lock(m_CriSec);

	CComQIPtr<IEnumPins> pEnum;
	CComQIPtr<IPin> pPin;

	//获取Filter Pin枚举器
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return CComQIPtr<IPin>();
	}

	//枚举filter pin
	while(pPin.Release(), pEnum->Next(1,&pPin, NULL) == S_OK)
	{
		PIN_DIRECTION thisPinDir;
		pPin->QueryDirection(&thisPinDir);

		//判断pin的方向及是否已连接
		if (thisPinDir == pinDir)
		{
			CComQIPtr<IPin> pTmp;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))
			{
				//Filter pin已连接，销毁连接pin，并继续寻找
				pTmp.Release();
			}
			else
			{
				//返回未连接的pin
				return pPin;
			}
		}
		pPin.Release();
	}

	//未找到符合要求的filter pin，返回空指针
	return CComQIPtr<IPin>();
}

//获得指定filter特定方向特定媒体主类型的未连接pin
CComQIPtr<IPin>	CGraphBaseBuilder::GetSpecyPin(CComPtr<IBaseFilter>& pFilter, PIN_DIRECTION pinDir, GUID Majoytype)
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
//获取未连接的pin
HRESULT	CGraphBaseBuilder::LoadComCompent(HMODULE hmodule, const CLSID& clsid, const IID& iid, void**ppComInterface)
{
	CheckPointer(hmodule, E_POINTER);
	CheckPointer(ppComInterface, E_POINTER);

	//加锁
	CLazyLock Lock(m_CriSec);

	//获取类对象指针
	pfnDllGetClassObject	fnDllGetClassObject = (pfnDllGetClassObject) GetProcAddress(hmodule,	"DllGetClassObject");
	if(!fnDllGetClassObject)
	{
		::FreeLibrary(hmodule);
	}
	else
	{
		//modules.push_back(hmodule);
		CLog::GetInstance()->trace(ENABLE_MAIN_INFO, "LoadComCompent GetProcAddress DllGetClassObject failed");
	}
	
	//创建实例对象指针
	CComPtr<IClassFactory> pClassFactory;
	HRESULT hr = fnDllGetClassObject(clsid,IID_IClassFactory, (void **)&pClassFactory);
	if(SUCCEEDED(hr) && pClassFactory)
	{
		hr = pClassFactory->CreateInstance(NULL, iid, (void**)ppComInterface);
		if(SUCCEEDED(hr) && *ppComInterface)
		{
			
			return S_OK;
		}
		else
		{
			CLog::GetInstance()->trace(ENABLE_MAIN_INFO, "LoadComCompent pClassFactory->CreateInstance failed");
		}
	}

	if(FAILED(hr))
	{
		//error...
		CLog::GetInstance()->trace(ENABLE_MAIN_INFO, "LoadComCompent DllGetClassObject failed");
	}

	return hr;
}

//以特定方式加载dll
HMODULE	CGraphBaseBuilder::LoadLibrarySpecify(const CString  path)
{
	//加锁
	CLazyLock Lock(m_CriSec);

	//按路径加载dll
	HMODULE hmd = LoadLibrary(path);

	if(hmd == NULL)
	{
		char* pbyte = NULL;
		if(GetLastErrorMsg((BYTE**)&pbyte)> 0)
		{
			CLog::GetInstance()->trace(ENABLE_ERROR_INFO, pbyte);
		}

		//直接加载dll
		int index = path.ReverseFind(L'\\');
		CString newpath = path;
		if(index >= 0)
		{
			newpath = path.Right(path.GetLength() - index - 1);
		}

		if(newpath.Find(L"COREAVC.AX") >= 0)
		{
			CReg::GetInstance()->CoreAVCReg();
			hmd = LoadLibrary(path);
		}

		hmd = LoadLibrary(newpath);
	}

	if(hmd)
	{
		CLog::GetInstance()->trace(ENABLE_MAIN_INFO, CString(L"Load hmodule")+ path + CString(L"Succeeded\n"));
	}
	else
	{
		CString str = CString(L"Load hmodule")+ path + CString(L"Failed\n");
		CLog::GetInstance()->trace(ENABLE_MAIN_INFO, str);
	}
	//返回dll句柄
	return hmd;
}