#pragma once
// {69DFBACA-1BEE-4bd2-B429-06A87838F679}
IMPLEMENT_OLECREATE(<<class>>, <<external_name>>, 
0x69dfbaca, 0x1bee, 0x4bd2, 0xb4, 0x29, 0x6, 0xa8, 0x78, 0x38, 0xf6, 0x79);

DECLARE_INTERFACE_(ICaptureConfig, IUnknown)
{
	//设置波形音频格式信息
	STDMETHOD (SetWAVEFORMAT) (WAVEFORMATEX* pwfe) = 0;

	//获取波形音频格式信息
	STDMETHOD (GetWAVEFORMAT) (WAVEFORMATEX* pwfe) = 0;

	//添加音频sample到音频队列
	STDMETHOD (AddAudioSample) (WAVEFORMATEX* pwfe) = 0;
};