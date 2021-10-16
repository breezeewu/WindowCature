#pragma once
#include <streams.h>

enum Enhance_Effect
{
	Effect_None = 0,	//无特效
    Effect_Line,        //线性扫描效果
    Effect_Rolling      //滚动扫描效果（目前默认特效）
};

#ifdef COM_INTERFACE
[uuid("C8E1EB91-569C-4BD4-B57C-0ABA992901E7")]
DECLARE_INTERFACE_(IVideoEnhance, IUnknown)
{
#else
class IVideoEnhance
{
public:
#endif
/***************************************************************
	函数名:     SetEnhaceEffect
	函数描述:   设置视频增强特效
	参数1：     [in] Enhance_Effect effect， 视频增强特效
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT SetEnhaceEffect(Enhance_Effect effect) = 0;

/***************************************************************
	函数名:     GetEnhaceEffect
	函数描述:   获取视频增强特效
	参数1：     [out] Enhance_Effect* peffect， 视频增强特效
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT GetEnhaceEffect(Enhance_Effect* peffect) = 0;

/***************************************************************
	函数名:     EnhaceVideo
	函数描述:   视频增强
	参数1：     [in, out] BYTE* pdata， 视频位图数据
	参数2：     [in] BITMAPINFOHEADER* pbmp,视频位图描述结构体信息
	参数2：     [in] int pixel,取值范围(-width, width)，大于0时视频增强特效从左至右扫描（一般在开启视频增强时使用），小于0时从右至左扫描（一般在关闭视频增强时使用）
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
	virtual HRESULT EnhaceVideo(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel) = 0;

/***************************************************************
	函数名:     Reset
	函数描述:   视频增强复位
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
	virtual HRESULT Reset() = 0;

/***************************************************************
	函数名:     SetEffectSize
	函数描述:   视频增强雨刷效果初始宽高
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
	virtual HRESULT SetEffectSize(int width,int height) = 0;


/******************************************************************
	函数名:     DestroyInstance
	函数描述:   销毁实例
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual void DestroyInstance() = 0;

};