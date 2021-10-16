// VideoCapture.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
//#include "VideoRecord.h"
#include "MediaPost.h"
//#include "IVideoPost.h"
#include "VideoRecord.h"
#include "LazyException.h"

#ifdef _MANAGED
#pragma managed(push, off)//�����йܣ��ƣ��رգ�

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
	//���ָ��ĺϷ���
	CheckPointer(ppMediaPost, E_POINTER);

	//��ָ�븳��ֵ
	*ppMediaPost = NULL;

	//����ʵ������
	*ppMediaPost = static_cast<IMediaPost*>(new CMediaPost());

	return S_OK;
}

EXTERN_C __declspec(dllexport) HRESULT DestoryMediaPost(void** ppMediaPost)
{
	//���ָ��ĺϷ���
	CheckPointer(ppMediaPost, E_POINTER);

	//��ԭʵ��ָ�����
	CMediaPost* pMediaPost = static_cast<CMediaPost*>(*ppMediaPost);

	//����ʵ��
	delete pMediaPost;

	//�ÿ�
	*ppMediaPost = NULL;

	return S_OK;
	
}

 EXTERN_C QDSTRINGAPI HRESULT CreateVideoRecordTask_c(long* pid)
 {
	 //���ָ��ĺϷ���
	 CheckPointer(pid, E_POINTER);

	 //��ָ�븳��ֵ
	 IVideoCapture* pvc = NULL;

	 //����ʵ������
	 pvc = static_cast<IVideoCapture*>(new CVideoRecord());
	 *pid = (long)pvc;

	 return S_OK;
 }

//������Ƶ��Ⱦ���ھ��
EXTERN_C QDSTRINGAPI HRESULT SetVideoHwnd_c(long pid, HWND hwd, RECT* prect)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetVideoHwnd(hwd, prect);
	}

	return E_FAIL;
}

/***************************************************************
	������:     GetVideoHwnd_c
	��������:   ��ȡ��Ƶ��Ⱦ���ھ��
	����1��     [in] long pid,taskid
	����2��     [out] HWND& hwd�� ��Ƶ���Ŵ��ھ��
	����3��     [out] RECT* prect,��ȡ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
EXTERN_C QDSTRINGAPI HRESULT GetVideoHwnd_c(long pid, HWND& hwd, RECT* prect)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetVideoHwnd(hwd, prect);
	}

	return E_FAIL;
}

//����¼����Ƶ�ֱ���
EXTERN_C QDSTRINGAPI HRESULT SetDstVideoSolutionRatio_c(long pid, int nWidth, int nHeigh)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetDstVideoSolutionRatio(nWidth, nHeigh);
	}

	return E_FAIL;
}

//��ȡ¼����Ƶ�ֱ���
EXTERN_C QDSTRINGAPI HRESULT GetDstVideoSolutionRatio_c(long pid, int& nWidth, int& nHeigh)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetDstVideoSolutionRatio(nWidth, nHeigh);
	}

	return E_FAIL;
}

//����¼����Ƶ֡��
EXTERN_C QDSTRINGAPI HRESULT SetDstVideoFrameRate_c(long pid, double dFramerate)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetDstVideoFrameRate(dFramerate);
	}

	return E_FAIL;
}

// ��ȡ¼����Ƶ֡��
//EXTERN_C QDSTRINGAPI HRESULT GetDstVideoFrameRate_c(long pid, double& dFramerate)
//{
//	//���ָ��ĺϷ���
//	CheckPointer(pid, E_POINTER);
//
//	//��ָ�븳��ֵ
//	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);
//
//	if(pvc)
//	{
//		return pvc->GetDstVideoFrameRate(dFramerate);
//	}
//
//	return E_FAIL;
//}



//����¼����Ƶ������
EXTERN_C QDSTRINGAPI HRESULT SetAudioSampleRate_c(long pid, e_SampleRate samplerate)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetAudioSampleRate(samplerate);
	}

	return E_FAIL;
}

// ��ȡ¼����Ƶ������
EXTERN_C QDSTRINGAPI HRESULT GetAudioSampleRate_c(long pid, e_SampleRate& samplerate)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetAudioSampleRate(samplerate);
	}

	return E_FAIL;
}

//����¼����Ƶ������
EXTERN_C QDSTRINGAPI HRESULT SetAudioChannel_c(long pid, int channel)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetAudioChannel(channel);
	}

	return E_FAIL;
}

//��ȡ¼����Ƶ������
EXTERN_C QDSTRINGAPI HRESULT GetAudioChannel_c(long pid, int& channel)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetAudioChannel(channel);
	}

	return E_FAIL;
}

//���������Ŵ�
EXTERN_C QDSTRINGAPI HRESULT SetAudioVolume_c(long pid, long AudioVolume)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetAudioVolume(AudioVolume);
	}

	return E_FAIL;
}

//��ȡ�����Ŵ�
EXTERN_C QDSTRINGAPI HRESULT GetAudioVolume_c(long pid, long& AudioVolume)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetAudioVolume(AudioVolume);
	}

	return E_FAIL;
}


//����¼����Ƶ������ʽ
EXTERN_C QDSTRINGAPI HRESULT SetDstFileContainer_c(long pid, e_RECORD_CONTAINER container)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetDstFileContainer(container);
	}

	return E_FAIL;
}

// ��ȡ¼����Ƶ������ʽ
EXTERN_C QDSTRINGAPI HRESULT GetDstFileContainer_c(long pid, e_RECORD_CONTAINER& container)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetDstFileContainer(container);
	}

	return E_FAIL;
}

//����¼����Ƶ����·��������
EXTERN_C QDSTRINGAPI HRESULT SetDstFilePath_c(long pid, wchar_t* pPath)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetDstFilePath(pPath);
	}

	return E_FAIL;
}

// ��ȡ¼����Ƶ����·��������
EXTERN_C QDSTRINGAPI HRESULT GetDstFilePath_c(long pid, wchar_t* pPath, int& len)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetDstFilePath(pPath, len);
	}

	return E_FAIL;
}


//��ʼ¼����Ƶ
EXTERN_C QDSTRINGAPI HRESULT StartRecord_c(long pid)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->StartRecord();
	}

	return E_FAIL;
}

//��ͣ¼����Ƶ
EXTERN_C QDSTRINGAPI HRESULT PauseRecord_c(long pid)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->PauseRecord();
	}

	return E_FAIL;
}

//ֹͣ¼����Ƶ
EXTERN_C QDSTRINGAPI HRESULT StopRecord_c(long pid)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->StopRecord();
	}

	return E_FAIL;
}

//��ȡ��һ֡��Ƶͼ��
EXTERN_C QDSTRINGAPI HRESULT GetFirstBitmapImage_c(long pid, BYTE** ppData)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetFirstBitmapImage(ppData);
	}

	return E_FAIL;
}

//������Ϣ�ص��ӿ�
EXTERN_C QDSTRINGAPI HRESULT SetCallBack_c(long pid, struct IQvodCallBack* pcallback)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->SetCallBack(pcallback);
	}

	return E_FAIL;
}

//������Ϣ�ص��ӿ�
EXTERN_C QDSTRINGAPI HRESULT GetCallBack_c(long pid, struct IQvodCallBack** ppcallback)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	//CheckPointer(hwd, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->GetCallBack(ppcallback);
	}

	return E_FAIL;
}

/******************************************************************
	������:     VideoResizeNotify_c
	��������:   ��Ƶ���ڴ�С����֪ͨ
	����1��     [in] long pid,taskid
	����2:      [in] RECT* prect����Ƶ���ڴ�С����֪ͨ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

EXTERN_C QDSTRINGAPI HRESULT VideoResizeNotify_c(long pid, RECT* prect)
{
	//���ָ��ĺϷ���
	CheckPointer(pid, E_POINTER);
	CheckPointer(prect, E_POINTER);

	//��ָ�븳��ֵ
	IVideoCapture* pvc = static_cast<IVideoCapture*>((IVideoCapture*)pid);

	if(pvc)
	{
		return pvc->VideoResizeNotify(prect);
	}

	return E_FAIL;
}

//����ʵ��
EXTERN_C QDSTRINGAPI void DestroyInstance_c(long pid)
{
	//���ָ��ĺϷ���
	//CheckPointer(pid, );

	//��ָ�븳��ֵ
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

