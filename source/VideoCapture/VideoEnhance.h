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
	//IUnknow�ӿ�
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
#endif
/***************************************************************
	������:     SetEnhaceEffect
	��������:   ������Ƶ��ǿ��Ч
	����1��     [in] Enhance_Effect effect�� ��Ƶ��ǿ��Ч
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	HRESULT SetEnhaceEffect(Enhance_Effect effect);

/***************************************************************
	������:     GetEnhaceEffect
	��������:   ��ȡ��Ƶ��ǿ��Ч
	����1��     [out] Enhance_Effect* peffect�� ��Ƶ��ǿ��Ч
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	HRESULT GetEnhaceEffect(Enhance_Effect* peffect);


/***************************************************************
	������:     EnhaceVideo
	��������:   ��Ƶ��ǿ
	����1��     [in, out] BYTE* pdata�� ��Ƶλͼ����
	����2��     [in] BITMAPINFOHEADER* pbmp,��Ƶλͼ�����ṹ����Ϣ
	����2��     [in] int pixel,ȡֵ��Χ(-width, width)������0ʱ��Ƶ��ǿ��Ч��������ɨ�裨һ���ڿ�����Ƶ��ǿʱʹ�ã���С��0ʱ��������ɨ�裨һ���ڹر���Ƶ��ǿʱʹ�ã�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	HRESULT EnhaceVideo(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel);

/***************************************************************
	������:     Reset
	��������:   ��Ƶ��ǿ��λ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
	HRESULT Reset();

	HRESULT SetEffectSize(int width,int height);

/******************************************************************
	������:     DestroyInstance
	��������:   ����ʵ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
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
