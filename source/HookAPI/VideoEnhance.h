#pragma once
#include <xmmintrin.h>
#include "..\include\IVideoEnhance.h"
#define Clip1(x) if((x) < 0){x = 0;} if((x) > 255){x = 255;}
class CVideoEnhance:public IVideoEnhance
{
public:
	CVideoEnhance(void);
public:
	~CVideoEnhance(void);

/***************************************************************
	函数名:     SetEnhaceEffect
	函数描述:   设置视频增强特效
	参数1：     [in] Enhance_Effect effect， 视频增强特效
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT SetEnhaceEffect(Enhance_Effect effect);

/***************************************************************
	函数名:     GetEnhaceEffect
	函数描述:   获取视频增强特效
	参数1：     [out] Enhance_Effect* peffect， 视频增强特效
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	virtual HRESULT GetEnhaceEffect(Enhance_Effect* peffect);

/***************************************************************
	函数名:     EnhaceVideo
	函数描述:   视频增强
	参数1：     [in, out] BYTE* pdata， 视频位图数据
	参数2：     [in] BITMAPINFOHEADER* pbmp,视频位图描述结构体信息
	参数2：     [in] int pixel,取值范围(-width, width)，大于0时视频增强特效从左至右扫描（一般在开启视频增强时使用），小于0时从右至左扫描（一般在关闭视频增强时使用）
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
	virtual HRESULT EnhaceVideo(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel);

protected:
	//视频增强//////

void RGB32toYV12SSE2(BYTE *src,   BYTE *dst, int srcrowsize,int destrowsize, int width, int height) ;
void YV12toRGB32MMX(BYTE *src,BYTE *dst,int width,int height);
void EnhanceYV12(BYTE *src,int pitchY,int pitchUV,int width,int height,int len);
void Gamma();
double GetGammaAdjustedValue(double Input, double Gamma);

protected:
	unsigned char			GammaTable[256];
	Enhance_Effect			m_eEnhanceEffect;
	BYTE*					m_pBuffer;
	int						m_nBMPSize;
};
