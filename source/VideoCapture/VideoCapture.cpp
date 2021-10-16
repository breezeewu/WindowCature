// VideoCapture.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
//#include "VideoRecord.h"
#include "MediaPost.h"
//#include "IVideoPost.h"
#include "VideoRecord.h"
#include "LazyException.h"

#ifdef _MANAGED
#pragma managed(push, off)//编译托管（推，关闭）

#endif

HINSTANCE	g_hInstance;
#define QDBASE_EXPORTS
#ifdef QDBASE_EXPORTS /*happyhuang x*/
#define QDSTRINGAPI __declspec(dllexport)
#else
#define QDSTRINGAPI __declspec(dllimport)
#endif
EXTERN_C __declspec(dllexport) HRESULT InitMediaPost(void** ppMediaPost)
{
	//检测指针的合法性
	CheckPointer(ppMediaPost, E_POINTER);

	//给指针赋初值
	*ppMediaPost = NULL;

	//创建实例对象
	*ppMediaPost = static_cast<IMediaPost*>(new CMediaPost());

	return S_OK;
}

EXTERN_C __declspec(dllexport) HRESULT DestoryMediaPost(void** ppMediaPost)
{
	//检测指针的合法性
	CheckPointer(ppMediaPost, E_POINTER);

	//还原实例指针对象
	CMediaPost* pMediaPost = static_cast<CMediaPost*>(*ppMediaPost);

	//销毁实例
	delete pMediaPost;

	//置空
	*ppMediaPost = NULL;

	return S_OK;
	
}

 EXTERN_C QDSTRINGAPI HRESULT CreateVideoRecordTask_c(long* pid)
 {
	 //检测指针的合法性
	 CheckPointer(pid, E_POINTER);

	 //给指针赋初值
	 IVideoCapture* pvc = NULL;

	 //创建实例对象
	 pvc = static_cast<IVideoCapture*>(new CVideoRecord());
	 *pid = (long)pvc;

	 return S_OK;
 }

//设置视频渲染窗口句柄
EXTERN_C QDSTRINGAPI HRESULT SetVideoHwnd_c(long pid, HWND hwd, RECT* prect)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetVideoHwnd(hwd, prect);
	}

	return E_FAIL;
}

/***************************************************************
	函数名:     GetVideoHwnd_c
	函数描述:   获取视频渲染窗口句柄
	参数1：     [in] long pid,taskid
	参数2：     [out] HWND& hwd， 视频播放窗口句柄
	参数3：     [out] RECT* prect,截取区域
	返回值：    成功返回S_OK，否则为失败
*****************************************************************/
EXTERN_C QDSTRINGAPI HRESULT GetVideoHwnd_c(long pid, HWND& hwd, RECT* prect)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetVideoHwnd(hwd, prect);
	}

	return E_FAIL;
}

//设置录制视频分辨率
EXTERN_C QDSTRINGAPI HRESULT SetDstVideoSolutionRatio_c(long pid, int nWidth, int nHeigh)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetDstVideoSolutionRatio(nWidth, nHeigh);
	}

	return E_FAIL;
}

//获取录制视频分辨率
EXTERN_C QDSTRINGAPI HRESULT GetDstVideoSolutionRatio_c(long pid, int& nWidth, int& nHeigh)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetDstVideoSolutionRatio(nWidth, nHeigh);
	}

	return E_FAIL;
}

//设置录制视频帧率
EXTERN_C QDSTRINGAPI HRESULT SetDstVideoFrameRate_c(long pid, double dFramerate)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetDstVideoFrameRate(dFramerate);
	}

	return E_FAIL;
}

// 获取录制视频帧率
//EXTERN_C QDSTRINGAPI HRESULT GetDstVideoFrameRate_c(long pid, double& dFramerate)
//{
//	//检测指针的合法性
//	CheckPointer(pid, E_POINTER);
//
//	//给指针赋初值
//	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);
//
//	if(pvc)
//	{
//		return pvc->GetDstVideoFrameRate(dFramerate);
//	}
//
//	return E_FAIL;
//}



//设置录制音频采样率
EXTERN_C QDSTRINGAPI HRESULT SetAudioSampleRate_c(long pid, e_SampleRate samplerate)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetAudioSampleRate(samplerate);
	}

	return E_FAIL;
}

// 获取录制音频采样率
EXTERN_C QDSTRINGAPI HRESULT GetAudioSampleRate_c(long pid, e_SampleRate& samplerate)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetAudioSampleRate(samplerate);
	}

	return E_FAIL;
}

//设置录制音频声道数
EXTERN_C QDSTRINGAPI HRESULT SetAudioChannel_c(long pid, int channel)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetAudioChannel(channel);
	}

	return E_FAIL;
}

//获取录制音频声道数
EXTERN_C QDSTRINGAPI HRESULT GetAudioChannel_c(long pid, int& channel)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetAudioChannel(channel);
	}

	return E_FAIL;
}

//设置音量放大
EXTERN_C QDSTRINGAPI HRESULT SetAudioVolume_c(long pid, long AudioVolume)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetAudioVolume(AudioVolume);
	}

	return E_FAIL;
}

//获取音量放大
EXTERN_C QDSTRINGAPI HRESULT GetAudioVolume_c(long pid, long& AudioVolume)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetAudioVolume(AudioVolume);
	}

	return E_FAIL;
}


//设置录制视频容器格式
EXTERN_C QDSTRINGAPI HRESULT SetDstFileContainer_c(long pid, e_RECORD_CONTAINER container)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetDstFileContainer(container);
	}

	return E_FAIL;
}

// 获取录制视频容器格式
EXTERN_C QDSTRINGAPI HRESULT GetDstFileContainer_c(long pid, e_RECORD_CONTAINER& container)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetDstFileContainer(container);
	}

	return E_FAIL;
}

//设置录制视频保存路径及名称
EXTERN_C QDSTRINGAPI HRESULT SetDstFilePath_c(long pid, wchar_t* pPath)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetDstFilePath(pPath);
	}

	return E_FAIL;
}

// 获取录制视频保存路径及名称
EXTERN_C QDSTRINGAPI HRESULT GetDstFilePath_c(long pid, wchar_t* pPath, int& len)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetDstFilePath(pPath, len);
	}

	return E_FAIL;
}


//开始录制视频
EXTERN_C QDSTRINGAPI HRESULT StartRecord_c(long pid)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->StartRecord();
	}

	return E_FAIL;
}

//暂停录制视频
EXTERN_C QDSTRINGAPI HRESULT PauseRecord_c(long pid)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->PauseRecord();
	}

	return E_FAIL;
}

//停止录制视频
EXTERN_C QDSTRINGAPI HRESULT StopRecord_c(long pid)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->StopRecord();
	}

	return E_FAIL;
}

//获取第一帧视频图像
EXTERN_C QDSTRINGAPI HRESULT GetFirstBitmapImage_c(long pid, BYTE** ppData)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetFirstBitmapImage(ppData);
	}

	return E_FAIL;
}

//设置消息回调接口
EXTERN_C QDSTRINGAPI HRESULT SetCallBack_c(long pid, struct IQvodCallBack* pcallback)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetCallBack(pcallback);
	}

	return E_FAIL;
}

//设置消息回调接口
EXTERN_C QDSTRINGAPI HRESULT GetCallBack_c(long pid, struct IQvodCallBack** ppcallback)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetCallBack(ppcallback);
	}

	return E_FAIL;
}

/******************************************************************
	函数名:     VideoResizeNotify_c
	函数描述:   视频窗口大小调整通知
	参数1：     [in] long pid,taskid
	参数2:      [in] RECT* prect，视频窗口大小调整通知参数
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

EXTERN_C QDSTRINGAPI HRESULT VideoResizeNotify_c(long pid, RECT* prect)
{
	//检测指针的合法性
	CheckPointer(pid, E_POINTER);
	CheckPointer(prect, E_POINTER);

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->VideoResizeNotify(prect);
	}

	return E_FAIL;
}

//销毁实例
EXTERN_C QDSTRINGAPI void DestroyInstance_c(long pid)
{
	//检测指针的合法性
	//CheckPointer(pid, );

	//给指针赋初值
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->DestroyInstance();
	}

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	/*g_hInstance = hModule;
	return TRUE;*/
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			g_hInstance = hModule;
			//SetUnhandledExceptionFilter(CLazyException::ExceptionFilter);
		}
		break;
	case DLL_THREAD_ATTACH:
		{
			break;
		}
	case DLL_THREAD_DETACH:
		{
			CoUninitialize();
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			break;
		}
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

