#pragma once
#include "Utility.h"
#include "..\include\IVideoEnhance.h"


#define ENHANCE_PARAM  0.18f
struct RBGA
{
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
};

class CVideoEnhance:public IVideoEnhance
#ifdef COM_INTERFACE
	, public CUnknown
#endif
{
private:
	unsigned char GammaTable[256];
	unsigned char darkGammaTable[256];
	int m_frameCount;
	int m_speed;
	LONGLONG m_start;
	LONGLONG m_duration;
public:
	CVideoEnhance(void);
	~CVideoEnhance(void);
#ifdef COM_INTERFACE
	//IUnknow接口
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
#endif
/***************************************************************
	函数名:     SetEnhaceEffect
	函数描述:   设置视频增强特效
	参数1：     [in] Enhance_Effect effect， 视频增强特效
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	HRESULT SetEnhaceEffect(Enhance_Effect effect);

/***************************************************************
	函数名:     GetEnhaceEffect
	函数描述:   获取视频增强特效
	参数1：     [out] Enhance_Effect* peffect， 视频增强特效
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	HRESULT GetEnhaceEffect(Enhance_Effect* peffect);


/***************************************************************
	函数名:     EnhaceVideo
	函数描述:   视频增强
	参数1：     [in, out] BYTE* pdata， 视频位图数据
	参数2：     [in] BITMAPINFOHEADER* pbmp,视频位图描述结构体信息
	参数2：     [in] int pixel,取值范围(-width, width)，大于0时视频增强特效从左至右扫描（一般在开启视频增强时使用），小于0时从右至左扫描（一般在关闭视频增强时使用）
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	HRESULT EnhaceVideo(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel);

/***************************************************************
	函数名:     Reset
	函数描述:   视频增强复位
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
	HRESULT Reset();

	HRESULT SetEffectSize(int width,int height);

/******************************************************************
	函数名:     DestroyInstance
	函数描述:   销毁实例
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	void DestroyInstance();

protected:
	void Enhance(BYTE *pBuffer,int pitchY,int pitchUV,int widthY,int heightY,int bAsm);
	void USM(BYTE *pBuffer,int const width,int const height,int const nPitch,int const sharpen ,int const nThreshold);

private:
	void Enhance_C(BYTE *y,BYTE*u,BYTE *v,int pitchY,int pitchUV,int width,int height,int len,int bValid);
	void Enhance_dark(BYTE *y,BYTE*u,BYTE *v,int pitchY,int pitchUV,int width,int height,int len );
	void Enhance_MMX(BYTE *y,BYTE*u,BYTE *v,int pitchY,int pitchUV,int width,int height);
	void Enhance_SSE2(BYTE *y,BYTE*u,BYTE *v,int pitchY,int pitchUV,int width,int height);
	
	double GetGammaAdjustedValue(double Input, double Gamma);
	void EnhanceRGB32(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel);
	
	bool USM_C(BYTE *pBuffer
		, int const nWidth
		, int const nHeight
		, int const nPitch
		, int const sharpen
		, int const nThreshold,int len);
	void USM_MMX(BYTE *pBuffer
		, int const nWidth
		, int const nHeight
		, int const nPitch
		, int const sharpen
		, int const nThreshold);
	void USM_SSE2(BYTE *pBuffer
		, int const nWidth
		, int const nHeight
		, int const nPitch
		, int const sharpen
		, int const nThreshold);


protected:
	BYTE*				m_pYV12Image;
	BYTE*				m_pEffectbuf;
	BYTE*				m_pRGBAlign;
	int					m_nImageSize;
	int					m_nRGBSize;

	int					m_nWidth;
	int					m_nHeight;
	int					m_brushWidth;
	int					m_brushHeight;
	Enhance_Effect		m_eEnhanceEffect;
};
