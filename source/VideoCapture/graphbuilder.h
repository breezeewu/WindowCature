#pragma once
/*******************************************************************************
  Copyright (C), 2010-2012, QVOD Technology Co.,Ltd.
  File name:    GraphBuilder.h
  Author:       zwu
  Version:      0.0.1
  Date:			2012-10-11
  Description:   �ṩ��graph����ĸ��ֹ��ܺ���
  Modification:
********************************************************************************/

#include <atlbase.h>
#include <atlstr.h>
#include <streams.h>
#include <windows.h>
#include <vector>
#include <map>
#include "LazyException.h"

//#define SHOW_GRAPH_IN_GRAPHEDIT
typedef LONG    (WINAPI   *fnDllGetClassObject)(REFCLSID , REFIID, LPVOID *);
typedef long (*pfnRegister)(void);

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

class CGraphBuilder:public CLazyException
{
public:
	CGraphBuilder(void);
	~CGraphBuilder(void);

	//����Filter graph
	CComQIPtr<IGraphBuilder> LoadFilterGraph();

	//����ָ��������Filter
	HRESULT ConnectFilter(CComQIPtr<IBaseFilter> pUp, CComQIPtr<IBaseFilter> pDown, bool bconnectall = false);

	//��ָ��filter��������filter
	HRESULT ConnectFilterThrough(CComQIPtr<IBaseFilter> pUp, CComQIPtr<IBaseFilter> pDown,  CComQIPtr<IBaseFilter> pthrough);

	//ʹ��ָ��pin����Ŀ��filter
	HRESULT ConnectPinToFilter(CComQIPtr<IPin> pPin, CComQIPtr<IBaseFilter> pDst);

	//������Ļpin��vobsubtitle filter
	HRESULT ConnectSubTitlePins(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pVobSubFilter);
	
	//���ָ��filter�ض������δ����pin
	CComQIPtr<IPin> GetUnconnectedPin(CComQIPtr<IBaseFilter>& pFilter,PIN_DIRECTION pinDir);

	//���ָ��filter�ض������ض�ý�������͵�δ����pin
	CComQIPtr<IPin>	GetSpecyPin(CComPtr<IBaseFilter> pFilter, PIN_DIRECTION pinDir, GUID Majoytype);

	//��ȡý���ļ�Checkbyte
	bool CheckByte(const TCHAR * pchkbytes, const BYTE* pDatas, int dataSize, int& needMoreData);

	//
	void CStringtoBin(CString str, BYTE *pdata);

protected:
	virtual void DestroyGraph();
	virtual int ReleaseFilter(CComQIPtr<IBaseFilter>& pbasefilter);
	virtual void ResetFilters() = 0;
	HRESULT			AddToFilterGraph(CComQIPtr<IBaseFilter>& pbasefilter, TCHAR* pName = NULL);
	HRESULT			RemoveFilterFromGraph(CComQIPtr<IBaseFilter>& pbasefilter);
	HRESULT			AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	HRESULT			RemoveFromRot(DWORD dwRegister);

protected:
	//��·Graph
	CComQIPtr<IGraphBuilder> m_pGraph;
	DWORD m_dwRegister;

};
