#pragma once
#include <windows.h>
//��Ƶsample�����ṹ��
struct AudioSample
{
	LONGLONG		rtStart;	//��ʼʱ���
	LONGLONG		rtStop;		//ֹͣʱ���
	BYTE*			pData;		//��Ƶ����
	DWORD			Len;		//��Ƶ���ݳ���
	DWORD			MaxDataLen; //databuffer����
	BOOL			bContinue;	//�����Ա��

	AudioSample()
	{
		rtStart		= -1;
		rtStop		= -1;
		pData		= NULL;
		MaxDataLen	= 0;
		Len			= 0;
		bContinue	= 0;
	}

	void ResetSample()
	{
		rtStart		= -1;
		rtStop		= -1;
		Len			= 0;
		bContinue	= 0;
	}
	~AudioSample()
	{
		if(pData)
		{
			delete[] pData;
			pData = NULL;
		}
	}
};

[uuid("69DFBACA-1BEE-4bd2-B429-06A87838F679")]
DECLARE_INTERFACE_(ICaptureConfig, IUnknown)
{
	//���ò�����Ƶ��ʽ��Ϣ
	STDMETHOD (SetWAVEFORMAT) (WAVEFORMATEX* pwfe) = 0;

	//��ȡ������Ƶ��ʽ��Ϣ
	STDMETHOD (GetWAVEFORMAT) (WAVEFORMATEX* pwfe) = 0;

	//�����Ƶsample����Ƶ����
	STDMETHOD (AddAudioSample) (AudioSample* pAudioSmaple) = 0;

	//��ȡ������Ƶsample
	STDMETHOD (GetFreeSample) (AudioSample** ppAudioSmaple) = 0;
};