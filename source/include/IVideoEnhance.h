#pragma once
#include <streams.h>

enum Enhance_Effect
{
	Effect_None = 0,	//����Ч
    Effect_Line,        //����ɨ��Ч��
    Effect_Rolling      //����ɨ��Ч����ĿǰĬ����Ч��
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
	������:     SetEnhaceEffect
	��������:   ������Ƶ��ǿ��Ч
	����1��     [in] Enhance_Effect effect�� ��Ƶ��ǿ��Ч
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	virtual HRESULT SetEnhaceEffect(Enhance_Effect effect) = 0;

/***************************************************************
	������:     GetEnhaceEffect
	��������:   ��ȡ��Ƶ��ǿ��Ч
	����1��     [out] Enhance_Effect* peffect�� ��Ƶ��ǿ��Ч
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	virtual HRESULT GetEnhaceEffect(Enhance_Effect* peffect) = 0;

/***************************************************************
	������:     EnhaceVideo
	��������:   ��Ƶ��ǿ
	����1��     [in, out] BYTE* pdata�� ��Ƶλͼ����
	����2��     [in] BITMAPINFOHEADER* pbmp,��Ƶλͼ�����ṹ����Ϣ
	����2��     [in] int pixel,ȡֵ��Χ(-width, width)������0ʱ��Ƶ��ǿ��Ч��������ɨ�裨һ���ڿ�����Ƶ��ǿʱʹ�ã���С��0ʱ��������ɨ�裨һ���ڹر���Ƶ��ǿʱʹ�ã�
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
	virtual HRESULT EnhaceVideo(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel) = 0;

/***************************************************************
	������:     Reset
	��������:   ��Ƶ��ǿ��λ
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
	virtual HRESULT Reset() = 0;

/***************************************************************
	������:     SetEffectSize
	��������:   ��Ƶ��ǿ��ˢЧ����ʼ���
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
	virtual HRESULT SetEffectSize(int width,int height) = 0;


/******************************************************************
	������:     DestroyInstance
	��������:   ����ʵ��
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual void DestroyInstance() = 0;

};