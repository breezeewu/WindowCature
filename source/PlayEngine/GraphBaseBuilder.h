#pragma once
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <streams.h>
#include "LazyException.h"
#include "LazyLock.h"
#include "Log.h"
#include "Guid.h"


#define SHOW_GRAPH_IN_GRAPHEDIT

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

#define BeginEnumSysDev(clsid, pMoniker) \
{CComPtr<ICreateDevEnum> pDevEnum4$##clsid; \
	pDevEnum4$##clsid.CoCreateInstance(CLSID_SystemDeviceEnum); \
	CComPtr<IEnumMoniker> pClassEnum4$##clsid; \
	if(SUCCEEDED(pDevEnum4$##clsid->CreateClassEnumerator(clsid, &pClassEnum4$##clsid, 0)) \
	&& pClassEnum4$##clsid) \
{ \
	pClassEnum4$##clsid->Reset(); \
	for(CComPtr<IMoniker> pMoniker; pClassEnum4$##clsid->Next(1, &pMoniker, 0) == S_OK; pMoniker = NULL) \
{ \

#define EndEnumSysDev }}}

typedef LONG    (WINAPI   *pfnDllGetClassObject)(REFCLSID , REFIID, LPVOID *);
typedef long (*pfnDllRegisterServer)(void);
typedef long (*pfnRegister)(void);

class CGraphBaseBuilder:CLazyException
{
public:
	CGraphBaseBuilder(void);
public:
	~CGraphBaseBuilder(void);

/*********************************************************************************************
	������:     ResetFlags                                
	��������:   �������б�ʶ      
	����ֵ��    ��
*********************************************************************************************/

	virtual void ResetFlags()   = 0;

/*********************************************************************************************
	������:     ResetInterface                                
	��������:   �������нӿ�      
	����ֵ��    ��
*********************************************************************************************/

	virtual void ResetInterface() = 0;

/*********************************************************************************************
	������:     ResetFilters                                
	��������:   ��������filter      
	����ֵ��    ��
*********************************************************************************************/

	virtual void ResetFilters() = 0;

/*********************************************************************************************
	������:     DestroyGraph                                
	��������:   ������·     
	����ֵ��    ��
*********************************************************************************************/

	virtual void DestroyGraph() = 0;

protected:
/*********************************************************************************************
	������:     LoadFilterGraph                                
	��������:   ����FilterGraph      
	����ֵ��    �ɹ�����CComQIPtr<IGraphBuilder>���ص�ǰgraphָ�룬ʧ�ܷ���NULL
*********************************************************************************************/
	CComQIPtr<IGraphBuilder> LoadFilterGraph();

#ifdef SHOW_GRAPH_IN_GRAPHEDIT
/*********************************************************************************************
	������:     AddToRot                                
	��������:   ���GraphBuilder������ʱ�����
	����1:      [in]IUnknown *pUnkGraph����ӵ����ж����ĵ�ǰgraphָ��
	����2:		[out]DWORD *pdwRegister��rotע��id
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*********************************************************************************************/
	HRESULT	AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);

/*********************************************************************************************
	������:     RemoveFromRot                                
	��������:   ������ʱ��������Ƴ�GraphBuilder
	����1:      [in]DWORD pdwRegister��GraphBuilder������ʱ������е�ע��id
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*********************************************************************************************/
	void RemoveFromRot(DWORD pdwRegister);
#endif
/*********************************************************************************************
	������:     ReleaseFilter                                
	��������:   �ͷ�filter
	����1:      [in]CComQIPtr<IBaseFilter>& piasefilter����ǰ�ͷŵ�filterָ������
	����ֵ��    ����filter�ͷź��������
*********************************************************************************************/
	int	ReleaseFilter(CComQIPtr<IBaseFilter>& pbasefilter);

/*********************************************************************************************
	������:     AddFiltertoGraph                                
	��������:   ���filter��graph
	����1:      [in]CComQIPtr<IBaseFilter>& piasefilter����ǰfilterָ��
	����2:      [in]LPCWSTR pName= NULL��filter����
	����ֵ��    �ɹ�����true��ʧ��false
*********************************************************************************************/
	bool AddFiltertoGraph(CComQIPtr<IBaseFilter>& pbasefilter,  LPCWSTR pName= NULL);

/*********************************************************************************************
	������:     ConnectFilter                                
	��������:   ö������pin������pin����������
	����1:      [in]CComQIPtr<IBaseFilter>& pSrc����������filter
	����2:      [in]CComQIPtr<IBaseFilter>& pDest����������filter
	����ֵ��    �ɹ�����true��ʧ�ܷ���false
*********************************************************************************************/
	bool ConnectFilter(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pDest);

/*********************************************************************************************
	������:     ConnectFilterDirect                                
	��������:   ����������Filter���ʺ�����Filterֻ��һ�����PIN�ĳ���
	����1:      [in]CComQIPtr<IBaseFilter>& pSrc����������filterָ������
	����2:      [in]CComQIPtr<IBaseFilter>& pDst����������filterָ������
	����3:      AM_MEDIA_TYPE* pConnType = NULL��filter����ý������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*********************************************************************************************/
	HRESULT	 ConnectFilterDirect(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pDest,AM_MEDIA_TYPE* pConnMediaType = NULL);

/*********************************************************************************************
	������:     ConnectPinToFilers                                
	��������:   ����pin��filter
	����1:      [in]CComQIPtr<IPin>& pPin������filter����pin
	����2:		[in]CComQIPtr<IBaseFilter>& pDes����������filter
	����ֵ��    �ɹ�����S_OK,����Ϊʧ��
*********************************************************************************************/
	HRESULT	ConnectPinToFilers(CComPtr<IPin>& pOutPin, CComQIPtr<IBaseFilter>& pDest);

/*********************************************************************************************
	������:     ConnectSubTitilePins                                
	��������:   ������Ļpin��vobsub
	����1:      [in]CComQIPtr<IBaseFilter>& pSrc����������filterָ������
	����2:		[in]CComQIPtr<IBaseFilter>& pSubFilter����Ļ����filterָ������
	����ֵ��    �ɹ�����true,ʧ�ܷ���false
*********************************************************************************************/
	bool ConnectSubTitilePins(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pSubFilter);


/*********************************************************************************************
	������:     GetUnconnectedPin                                
	��������:   ��ȡδ���ӵ�pin
	����1:      [in]CComQIPtr<IBaseFilter>& pFilter��δ����pin������filter
	����2:      [in]PIN_DIRECTION pinDir��pin���ӷ���
	����ֵ��    �ɹ�����filterΪ����pin
*********************************************************************************************/
	CComQIPtr<IPin>	GetUnconnectedPin(CComQIPtr<IBaseFilter>& pFilter, PIN_DIRECTION pinDir);

	//���ָ��filter�ض������ض�ý�������͵�δ����pin
/*********************************************************************************************
	������:     GetSpecyPin                                
	��������:   ���ָ��filter�ض������ض�ý�������͵�δ����pin
	����1:      [in]CComQIPtr<IBaseFilter>& pFilter��δ����pin������filter
	����2:      [in]PIN_DIRECTION pinDir��pin���ӷ���
	����2:      [in]GUID Majoytype, pin��ý��������
	����ֵ��    �ɹ�����filterΪ����pin
*********************************************************************************************/
CComQIPtr<IPin>	GetSpecyPin(CComPtr<IBaseFilter>& pFilter, PIN_DIRECTION pinDir, GUID Majoytype);

/*********************************************************************************************
	������:     LoadComCompent                                
	��������:   ��ȡδ���ӵ�pin
	����1:      [in]HMODULE hmodule��dll���
	����2:      [in]CLSID& clsid��COM���clsid
	����3:      [in]IID& iid���ӿ�id
	����4:      [out]void** ppComInterface���ӿ�ָ���ָ��
	����ֵ��    �ɹ�����S_OK����Ϊʧ��
*********************************************************************************************/
	HRESULT	LoadComCompent(HMODULE hmodule, const CLSID& clsid, const IID& iid, void** ppComInterface);

/*********************************************************************************************
	������:     LoadLibrarySpecify                                
	��������:   ���ض���ʽ����dll
	����1:      [in]const CString  path��dll·����dll����
	����ֵ��    �ɹ�����DLL�����ʧ�ܷ���NULL
*********************************************************************************************/
	HMODULE	LoadLibrarySpecify(const CString  path);

protected:
	CComQIPtr<IGraphBuilder>			m_pGraph;

	CCriSec								m_CriSec;//�ٽ���

};
