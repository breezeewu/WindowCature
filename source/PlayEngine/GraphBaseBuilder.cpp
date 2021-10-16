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
// ����FilterGraph
CComQIPtr<IGraphBuilder> CGraphBaseBuilder::LoadFilterGraph()
{
	//����
	CLazyLock Lock(m_CriSec);

	//����Quartz.dll
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
//���GraphBuilder������ʱ�����
HRESULT	CGraphBaseBuilder::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
	//�����������ĺϷ���
	CheckPointer(pUnkGraph, E_POINTER);
	CheckPointer(pdwRegister, E_POINTER);

	//����
	CLazyLock Lock(m_CriSec);

	IMoniker * pMoniker;
	IRunningObjectTable *pROT;

	//��ȡ����ʱ�����ROT
	if(FAILED(GetRunningObjectTable(0, &pROT)))
	{
		return E_FAIL;
	}

	WCHAR wsz[256];
	wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());

	//����������
	HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if(SUCCEEDED(hr))
	{
		//ע��Graph
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,	pMoniker, pdwRegister);
		pMoniker->Release();
	}

	//����ROT
	pROT->Release();

	return hr;
}

//������ʱ��������Ƴ�GraphBuilder
void CGraphBaseBuilder::RemoveFromRot(DWORD pdwRegister)
{
	//����
	CLazyLock Lock(m_CriSec);

	IRunningObjectTable *pROT;

	//��ȡ����ʱ�����ROT
	if(SUCCEEDED(GetRunningObjectTable(0, &pROT)))
	{
		//������ʱ��������Ƴ�
		HRESULT hr = pROT->Revoke(pdwRegister);
		if(FAILED(hr))
		{
			//mPlayMedia->OutPutInfo(2, L"Revoke ROT FAILED!\n");
		}
		pROT->Release();
	}
}
#endif
                             
//�ͷ�filter
int	CGraphBaseBuilder::ReleaseFilter(CComQIPtr<IBaseFilter>& pbasefilter)
{
	//�����������ĺϷ���
	CheckPointer(pbasefilter, -1);

	//����
	CLazyLock Lock(m_CriSec);

	//�ͷ�filter
	if (pbasefilter)
	{
		IBaseFilter* filter = pbasefilter;
		HRESULT hr = m_pGraph->RemoveFilter(pbasefilter);
		ASSERT(pbasefilter != NULL);

		IBaseFilter* pfilter = pbasefilter.p;
		pfilter->AddRef();
		int ret = pfilter->Release();
		pbasefilter.Release();
		//�����ͷź��������
		return --ret;
	}

	return -1;
}

//���filter��graph
bool CGraphBaseBuilder::AddFiltertoGraph(CComQIPtr<IBaseFilter>& pbasefilter,  LPCWSTR pName)
{
	//�����������ĺϷ���
	CheckPointer(pbasefilter, false);

	//����
	CLazyLock Lock(m_CriSec);

	HRESULT hr = m_pGraph->AddFilter(pbasefilter, pName);
	if(FAILED(hr))
	{
		//ʧ�ܣ���ӡ������Ϣ
		CLog::GetInstance()->trace(ENABLE_WARNING_INFO, L"AddFiltertoGraph failed, erroecode:%0x\n", hr);
	}

	return SUCCEEDED(hr);
}

//��������filter
bool CGraphBaseBuilder::ConnectFilter(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pDest)
{
	
	//�����������ĺϷ���
	CheckPointer(pSrc, false);
	CheckPointer(pDest, false);

	//����
	CLazyLock Lock(m_CriSec);

	// ѭ��ÿ�����PIN
	bool ret = false;
	BeginEnumPins(pSrc, pEnumPins, pOutPin)
	{	
		PIN_DIRECTION out_direction;

		//��ѯ���pin�ķ���
		HRESULT hr = pOutPin->QueryDirection(&out_direction);
		if(SUCCEEDED(hr) && out_direction == PINDIR_OUTPUT)
		{
			// ѭ��ö�����PIN��ý������
			CComPtr<IEnumMediaTypes> pEnumMediaTypes; 
			if(pOutPin && SUCCEEDED(pOutPin->EnumMediaTypes(&pEnumMediaTypes))) 
			{ 
				pEnumMediaTypes->Reset();
				AM_MEDIA_TYPE* pMediaType = NULL; 
				HRESULT rr = E_FAIL;
				//���ö�����pin֧�ֵ�ý������
				for(; S_OK == (rr = pEnumMediaTypes->Next(1, &pMediaType, NULL)); DeleteMediaType(pMediaType), pMediaType = NULL) 
				{ 
					// �ҵ�����PIN
					bool bFind = false;
					BeginEnumPins(pDest, pDecoderEnumPins, pInPin)
					{
						bFind = true;
						// �ҵ�
						if (SUCCEEDED(pInPin->QueryAccept(pMediaType)))
						{
							CComQIPtr<IPin> pConnectto;
							//����Ƿ�������
							pInPin->ConnectedTo(&pConnectto);
							if(pConnectto)
							{
								//�����ӣ���������
								pConnectto.Release();
								continue;
							}

							// ��������PIN
							hr =  m_pGraph->ConnectDirect(pOutPin,pInPin,pMediaType);
							if(SUCCEEDED(hr))
							{
								//���ӳɹ�
								ret = true;
							}
							else
							{
								//ʧ�ܣ���ӡ������Ϣ
								CLog::GetInstance()->trace(ENABLE_WARNING_INFO, L"ConnectFilter ConnectDirect failed, erroecode:%0x\n", hr);
							}
						}
					}
					EndEnumPins
					ASSERT(bFind);	
					
				}

				//ɾ��ý������
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

//ͨ��ָ����ý����������filter
HRESULT	CGraphBaseBuilder::ConnectFilterDirect(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pDest,AM_MEDIA_TYPE* pConnMediaType)
{
	//�����������ĺϷ���
	CheckPointer(pSrc, E_POINTER);
	CheckPointer(pDest, E_POINTER);
	CheckPointer(pConnMediaType, E_POINTER);

	//����
	CLazyLock Lock(m_CriSec);

	// ���Բ�ָ����������
	BeginEnumPins(pSrc, pEnumPins, pOutPin)
	{
		PIN_DIRECTION thisPinDir;
		//��ѯ����
		pOutPin->QueryDirection(&thisPinDir);
		if (thisPinDir == PINDIR_OUTPUT)
		{
			//���Pin
			CComQIPtr<IPin> pTmp;
			pOutPin->ConnectedTo(&pTmp);
			if (!pTmp)
			{
				HRESULT hr = E_FAIL;

				//����Filter
				if(SUCCEEDED(hr = ConnectPinToFilers(pOutPin, pDest)))
				{
					if (pConnMediaType)
					{
						//��ȡ����ý������
						if(SUCCEEDED(hr = pOutPin->ConnectionMediaType(pConnMediaType)))
						{
							
						}
						else
						{
							//ʧ�ܣ���ӡ������Ϣ
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

//����pin��filter
HRESULT	CGraphBaseBuilder::ConnectPinToFilers(CComPtr<IPin>& pOutPin, CComQIPtr<IBaseFilter>& pDest)
{
	//�����������ĺϷ���
	CheckPointer(pOutPin, E_POINTER);
	CheckPointer(pDest, E_POINTER);

	//����
	CLazyLock Lock(m_CriSec);
	
	//��ʵ���������Ҫ��
	CComQIPtr<IEnumPins> pEnum;
	CComQIPtr<IPin> pPin;

	//��ȡFilter Pinö����
	HRESULT hr = pDest->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		//ʧ�ܣ���ӡ������Ϣ
		CLog::GetInstance()->trace(ENABLE_WARNING_INFO, L"ConnectPinToFilers EnumPins failed, erroecode:%0x\n", hr);
		return hr;
	}

	//���ö��Filter pin
	while(pPin.Release(), pEnum->Next(1,&pPin, NULL) == S_OK)
	{
		PIN_DIRECTION thisPinDir;
		pPin->QueryDirection(&thisPinDir);
		if (thisPinDir == PINDIR_INPUT)
		{
			//��Filter pinΪ����pin
			CComQIPtr<IPin> pTmp;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))
			{
				//�����ӣ�����Filter pin������Ѱ��
				pTmp.Release();
				continue;
			}
			else
			{
				//�ҵ�δ����pin������
				hr = m_pGraph->ConnectDirect(pOutPin, pPin, NULL);
				if (SUCCEEDED(hr))
				{
					//���ӳɹ�������
					return hr;
				}
				else
				{
					//ʧ�ܣ���ӡ������Ϣ
					CLog::GetInstance()->trace(ENABLE_WARNING_INFO, L"ConnectPinToFilers ConnectionMediaType failed, erroecode:%0x\n", hr);
				}
			}
		}
		pPin.Release();
	}

	//�޷����ӣ�����ʧ��
	return E_FAIL;

}

//������Ļpin��vobsub
bool CGraphBaseBuilder::ConnectSubTitilePins(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pSubFilter)
{
	//�����������ĺϷ���
	CheckPointer(pSrc, false);
	CheckPointer(pSubFilter, false);

	//����
	CLazyLock Lock(m_CriSec);

	bool ret = false;
	// ��ȡDest Filter������ý������
	// ѭ��ÿ�����PIN
	BeginEnumPins(pSrc, pEnumPins, pOutPin)
	{	
		PIN_DIRECTION out_direction;
		HRESULT hr = pOutPin->QueryDirection(&out_direction);
		if(SUCCEEDED(hr) && out_direction == PINDIR_OUTPUT)
		{
			// ѭ�����PIN��ý������
			BeginEnumMediaTypes(pOutPin, pEnumMediaTypes, pOutMediaType)
			{
				if (pOutMediaType->majortype == MEDIATYPE_Subtitle 
					|| (pOutMediaType->majortype == MEDIATYPE_DVD_ENCRYPTED_PACK && pOutMediaType->subtype == MEDIASUBTYPE_DVD_SUBPICTURE))
				{			
					// �ҵ�����PIN
					BeginEnumPins(pSubFilter, pDecoderEnumPins, pInPin)
					{
						hr = pInPin->QueryDirection(&out_direction);
						// �ҵ�
						if (SUCCEEDED(hr) && out_direction == PINDIR_INPUT && SUCCEEDED(pInPin->QueryAccept(pOutMediaType)))
						{
							// δ���ӣ�
							CComQIPtr<IPin> pConnectto;
							pInPin->ConnectedTo(&pConnectto);
							if(pConnectto)
							{
								pConnectto.Release();
								continue;
							}

							// ��������PIN
							hr =  m_pGraph->ConnectDirect(pOutPin,pInPin,pOutMediaType);
							if(SUCCEEDED(hr))
							{
// 								//���ӳɹ�
								ret = true;
							}
						}
					}
					EndEnumPins
				}
			}
			//��ɴ˴�ö�٣�ɾ��ý������
			EndEnumMediaTypes(pOutMediaType)
		}
	}
	EndEnumPins
	return ret;
}


//��ȡδ���ӵ�pin
CComQIPtr<IPin>	CGraphBaseBuilder::GetUnconnectedPin(CComQIPtr<IBaseFilter>& pFilter, PIN_DIRECTION pinDir)
{
	//�����������ĺϷ���
	CheckPointer(pFilter, CComQIPtr<IPin>());

	//����
	CLazyLock Lock(m_CriSec);

	CComQIPtr<IEnumPins> pEnum;
	CComQIPtr<IPin> pPin;

	//��ȡFilter Pinö����
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return CComQIPtr<IPin>();
	}

	//ö��filter pin
	while(pPin.Release(), pEnum->Next(1,&pPin, NULL) == S_OK)
	{
		PIN_DIRECTION thisPinDir;
		pPin->QueryDirection(&thisPinDir);

		//�ж�pin�ķ����Ƿ�������
		if (thisPinDir == pinDir)
		{
			CComQIPtr<IPin> pTmp;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))
			{
				//Filter pin�����ӣ���������pin��������Ѱ��
				pTmp.Release();
			}
			else
			{
				//����δ���ӵ�pin
				return pPin;
			}
		}
		pPin.Release();
	}

	//δ�ҵ�����Ҫ���filter pin�����ؿ�ָ��
	return CComQIPtr<IPin>();
}

//���ָ��filter�ض������ض�ý�������͵�δ����pin
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
//��ȡδ���ӵ�pin
HRESULT	CGraphBaseBuilder::LoadComCompent(HMODULE hmodule, const CLSID& clsid, const IID& iid, void**ppComInterface)
{
	CheckPointer(hmodule, E_POINTER);
	CheckPointer(ppComInterface, E_POINTER);

	//����
	CLazyLock Lock(m_CriSec);

	//��ȡ�����ָ��
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
	
	//����ʵ������ָ��
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

//���ض���ʽ����dll
HMODULE	CGraphBaseBuilder::LoadLibrarySpecify(const CString  path)
{
	//����
	CLazyLock Lock(m_CriSec);

	//��·������dll
	HMODULE hmd = LoadLibrary(path);

	if(hmd == NULL)
	{
		char* pbyte = NULL;
		if(GetLastErrorMsg((BYTE**)&pbyte)> 0)
		{
			CLog::GetInstance()->trace(ENABLE_ERROR_INFO, pbyte);
		}

		//ֱ�Ӽ���dll
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
	//����dll���
	return hmd;
}