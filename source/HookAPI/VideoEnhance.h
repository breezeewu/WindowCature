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
	������:     SetEnhaceEffect
	��������:   ������Ƶ��ǿ��Ч
	����1��     [in] Enhance_Effect effect�� ��Ƶ��ǿ��Ч
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	virtual HRESULT SetEnhaceEffect(Enhance_Effect effect);

/***************************************************************
	������:     GetEnhaceEffect
	��������:   ��ȡ��Ƶ��ǿ��Ч
	����1��     [out] Enhance_Effect* peffect�� ��Ƶ��ǿ��Ч
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	virtual HRESULT GetEnhaceEffect(Enhance_Effect* peffect);

/***************************************************************
	������:     EnhaceVideo
	��������:   ��Ƶ��ǿ
	����1��     [in, out] BYTE* pdata�� ��Ƶλͼ����
	����2��     [in] BITMAPINFOHEADER* pbmp,��Ƶλͼ�����ṹ����Ϣ
	����2��     [in] int pixel,ȡֵ��Χ(-width, width)������0ʱ��Ƶ��ǿ��Ч��������ɨ�裨һ���ڿ�����Ƶ��ǿʱʹ�ã���С��0ʱ��������ɨ�裨һ���ڹر���Ƶ��ǿʱʹ�ã�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
	virtual HRESULT EnhaceVideo(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel);

protected:
	//��Ƶ��ǿ//////

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
