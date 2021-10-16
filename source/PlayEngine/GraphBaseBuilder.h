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
	函数名:     ResetFlags                                
	函数描述:   重置所有标识      
	返回值：    无
*********************************************************************************************/

	virtual void ResetFlags()   = 0;

/*********************************************************************************************
	函数名:     ResetInterface                                
	函数描述:   重置所有接口      
	返回值：    无
*********************************************************************************************/

	virtual void ResetInterface() = 0;

/*********************************************************************************************
	函数名:     ResetFilters                                
	函数描述:   重置所有filter      
	返回值：    无
*********************************************************************************************/

	virtual void ResetFilters() = 0;

/*********************************************************************************************
	函数名:     DestroyGraph                                
	函数描述:   销毁链路     
	返回值：    无
*********************************************************************************************/

	virtual void DestroyGraph() = 0;

protected:
/*********************************************************************************************
	函数名:     LoadFilterGraph                                
	函数描述:   加载FilterGraph      
	返回值：    成功返回CComQIPtr<IGraphBuilder>返回当前graph指针，失败返回NULL
*********************************************************************************************/
	CComQIPtr<IGraphBuilder> LoadFilterGraph();

#ifdef SHOW_GRAPH_IN_GRAPHEDIT
/*********************************************************************************************
	函数名:     AddToRot                                
	函数描述:   添加GraphBuilder到运行时对象表
	参数1:      [in]IUnknown *pUnkGraph，添加到运行对象表的当前graph指针
	参数2:		[out]DWORD *pdwRegister，rot注册id
	返回值：    成功返回S_OK，否则为失败
*********************************************************************************************/
	HRESULT	AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);

/*********************************************************************************************
	函数名:     RemoveFromRot                                
	函数描述:   从运行时对象表中移除GraphBuilder
	参数1:      [in]DWORD pdwRegister，GraphBuilder在运行时对象表中的注册id
	返回值：    成功返回S_OK，否则为失败
*********************************************************************************************/
	void RemoveFromRot(DWORD pdwRegister);
#endif
/*********************************************************************************************
	函数名:     ReleaseFilter                                
	函数描述:   释放filter
	参数1:      [in]CComQIPtr<IBaseFilter>& piasefilter，当前释放的filter指针引用
	返回值：    返回filter释放后的引用数
*********************************************************************************************/
	int	ReleaseFilter(CComQIPtr<IBaseFilter>& pbasefilter);

/*********************************************************************************************
	函数名:     AddFiltertoGraph                                
	函数描述:   添加filter到graph
	参数1:      [in]CComQIPtr<IBaseFilter>& piasefilter，当前filter指针
	参数2:      [in]LPCWSTR pName= NULL，filter名称
	返回值：    成功返回true，失败false
*********************************************************************************************/
	bool AddFiltertoGraph(CComQIPtr<IBaseFilter>& pbasefilter,  LPCWSTR pName= NULL);

/*********************************************************************************************
	函数名:     ConnectFilter                                
	函数描述:   枚举输入pin与输入pin，尝试连接
	参数1:      [in]CComQIPtr<IBaseFilter>& pSrc，上游连接filter
	参数2:      [in]CComQIPtr<IBaseFilter>& pDest，下游连接filter
	返回值：    成功返回true，失败返回false
*********************************************************************************************/
	bool ConnectFilter(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pDest);

/*********************************************************************************************
	函数名:     ConnectFilterDirect                                
	函数描述:   连接里两个Filter，适合上游Filter只有一个输出PIN的场合
	参数1:      [in]CComQIPtr<IBaseFilter>& pSrc，上游连接filter指针引用
	参数2:      [in]CComQIPtr<IBaseFilter>& pDst，下游连接filter指针引用
	参数3:      AM_MEDIA_TYPE* pConnType = NULL，filter连接媒体类型
	返回值：    成功返回S_OK，否则为失败
*********************************************************************************************/
	HRESULT	 ConnectFilterDirect(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pDest,AM_MEDIA_TYPE* pConnMediaType = NULL);

/*********************************************************************************************
	函数名:     ConnectPinToFilers                                
	函数描述:   连接pin与filter
	参数1:      [in]CComQIPtr<IPin>& pPin，上游filter连接pin
	参数2:		[in]CComQIPtr<IBaseFilter>& pDes，下游连接filter
	返回值：    成功返回S_OK,否则为失败
*********************************************************************************************/
	HRESULT	ConnectPinToFilers(CComPtr<IPin>& pOutPin, CComQIPtr<IBaseFilter>& pDest);

/*********************************************************************************************
	函数名:     ConnectSubTitilePins                                
	函数描述:   连接字幕pin到vobsub
	参数1:      [in]CComQIPtr<IBaseFilter>& pSrc，上游连接filter指针引用
	参数2:		[in]CComQIPtr<IBaseFilter>& pSubFilter，字幕处理filter指针引用
	返回值：    成功返回true,失败返回false
*********************************************************************************************/
	bool ConnectSubTitilePins(CComQIPtr<IBaseFilter>& pSrc, CComQIPtr<IBaseFilter>& pSubFilter);


/*********************************************************************************************
	函数名:     GetUnconnectedPin                                
	函数描述:   获取未连接的pin
	参数1:      [in]CComQIPtr<IBaseFilter>& pFilter，未连接pin的载体filter
	参数2:      [in]PIN_DIRECTION pinDir，pin连接方向
	返回值：    成功返回filter为连接pin
*********************************************************************************************/
	CComQIPtr<IPin>	GetUnconnectedPin(CComQIPtr<IBaseFilter>& pFilter, PIN_DIRECTION pinDir);

	//获得指定filter特定方向特定媒体主类型的未连接pin
/*********************************************************************************************
	函数名:     GetSpecyPin                                
	函数描述:   获得指定filter特定方向特定媒体主类型的未连接pin
	参数1:      [in]CComQIPtr<IBaseFilter>& pFilter，未连接pin的载体filter
	参数2:      [in]PIN_DIRECTION pinDir，pin连接方向
	参数2:      [in]GUID Majoytype, pin的媒体主类型
	返回值：    成功返回filter为连接pin
*********************************************************************************************/
CComQIPtr<IPin>	GetSpecyPin(CComPtr<IBaseFilter>& pFilter, PIN_DIRECTION pinDir, GUID Majoytype);

/*********************************************************************************************
	函数名:     LoadComCompent                                
	函数描述:   获取未连接的pin
	参数1:      [in]HMODULE hmodule，dll句柄
	参数2:      [in]CLSID& clsid，COM组件clsid
	参数3:      [in]IID& iid，接口id
	参数4:      [out]void** ppComInterface，接口指针的指针
	返回值：    成功返回S_OK否则为失败
*********************************************************************************************/
	HRESULT	LoadComCompent(HMODULE hmodule, const CLSID& clsid, const IID& iid, void** ppComInterface);

/*********************************************************************************************
	函数名:     LoadLibrarySpecify                                
	函数描述:   以特定方式加载dll
	参数1:      [in]const CString  path，dll路径或dll名称
	返回值：    成功返回DLL句柄，失败返回NULL
*********************************************************************************************/
	HMODULE	LoadLibrarySpecify(const CString  path);

protected:
	CComQIPtr<IGraphBuilder>			m_pGraph;

	CCriSec								m_CriSec;//临界锁

};
