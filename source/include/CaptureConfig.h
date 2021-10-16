#pragma once
#include <windows.h>
//音频sample描述结构体
struct AudioSample
{
	LONGLONG		rtStart;	//开始时间戳
	LONGLONG		rtStop;		//停止时间戳
	BYTE*			pData;		//音频数据
	DWORD			Len;		//音频数据长度
	DWORD			MaxDataLen; //databuffer长度
	BOOL			bContinue;	//连续性标记

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
	//设置波形音频格式信息
	STDMETHOD (SetWAVEFORMAT) (WAVEFORMATEX* pwfe) = 0;

	//获取波形音频格式信息
	STDMETHOD (GetWAVEFORMAT) (WAVEFORMATEX* pwfe) = 0;

	//添加音频sample到音频队列
	STDMETHOD (AddAudioSample) (AudioSample* pAudioSmaple) = 0;

	//获取空闲音频sample
	STDMETHOD (GetFreeSample) (AudioSample** ppAudioSmaple) = 0;
};