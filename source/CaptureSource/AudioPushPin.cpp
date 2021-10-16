
#include "AudioPushPin.h"
#include "AudioCaptureSource.h"

CAudioPushPin::CAudioPushPin(HRESULT *phr, CAudioCaptureSource *pFilter)
					: CSourceStream(NAME("Push Source pin"), phr, (CSource*)pFilter, L"Capture")
{
	m_nCurrentTs  = 0;
	m_tWAVEFORMATEX.nBlockAlign			= 4;
	m_tWAVEFORMATEX.nChannels			= 2;
	m_tWAVEFORMATEX.nSamplesPerSec		= 44100;
	m_tWAVEFORMATEX.wBitsPerSample		= 16;
	m_tWAVEFORMATEX.wFormatTag			= WAVE_FORMAT_PCM;
	m_tWAVEFORMATEX.cbSize				= sizeof(m_tWAVEFORMATEX);
	m_tWAVEFORMATEX.nAvgBytesPerSec	= m_tWAVEFORMATEX.nChannels * m_tWAVEFORMATEX.nSamplesPerSec * m_tWAVEFORMATEX.wBitsPerSample/8;
	
	*phr = S_OK;

	m_hPipeHnd = NULL;

	m_bAlive		= false;
	m_nCurrentTs	= 0;
	m_rtStartTime	= -1;
	CreateAudioDevice();
	//分配sample
	for(int i = 0; i < 50; i++)
	{
		m_qFreeSampleList.push(new AudioSample);
	}
}

CAudioPushPin::~CAudioPushPin(void)
{
	ResetSamples();

	if(m_hPipeHnd)
	{
		CloseHandle(m_hPipeHnd);
	}

	while(m_qFreeSampleList.size() > 0)
	{
		delete m_qFreeSampleList.front();
		m_qFreeSampleList.pop();
	}
}

HRESULT CAudioPushPin::InitMediaType(WAVEFORMATEX* pwfe)
{

	m_tWAVEFORMATEX = *pwfe;

	return MediaTypeFromWaveFormat(pwfe, &m_mt);
}

//CSourceStream
HRESULT CAudioPushPin::OnThreadCreate(void)
{
	////重置所有sample
	ResetSamples();

	//重置开始时间戳
	m_nCurrentTs = 0;

	if(m_pAudioClient)
	{
		m_pAudioClient->Start();
	}

	//开始标记
	m_bAlive = true;

	return S_OK;
}

HRESULT CAudioPushPin::OnThreadDestroy(void)
{
	if(m_pAudioClient)
	{
		return m_pAudioClient->Stop();
	}

	return S_OK;
}
//STDMETHODIMP CAudioPushPin::QueryInterface(REFIID riid, void **ppv)
//{
//	//return E_FAIL;
//return __super::QueryInterface(riid, ppv);
//}

STDMETHODIMP CAudioPushPin::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);

	if(__uuidof(ICaptureConfig) == riid)
	{
		/**ppv = static_cast<ICaptureConfig*>(this);
		AddRef();
		return *ppv ? S_OK : E_NOINTERFACE;*/
		return GetInterface((ICaptureConfig*)this, ppv);
	}
	return __super::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP CAudioPushPin::SetWAVEFORMAT(WAVEFORMATEX* pwfe)
{
	CheckPointer(pwfe, E_POINTER);

	m_tWAVEFORMATEX = *pwfe;

	return S_OK;
}

//获取波形音频格式信息
STDMETHODIMP CAudioPushPin::GetWAVEFORMAT(WAVEFORMATEX* pwfe)
{
	//检查输入指针的合法性
	CheckPointer(pwfe, E_POINTER);

	*pwfe = m_tWAVEFORMATEX;

	return S_OK;
}

//添加音频sample到音频队列
STDMETHODIMP CAudioPushPin::AddAudioSample(AudioSample* pAudioSmaple)
{
	//检查输入指针的合法性
	CheckPointer(pAudioSmaple, E_POINTER);

	//加锁
	CAutoLock cAutoLock(&m_CritSecLock);

	//计算当前sample时间戳
	pAudioSmaple->rtStart	= m_nCurrentTs ;
	pAudioSmaple->rtStop	= (REFERENCE_TIME)pAudioSmaple->Len*10000000/m_tWAVEFORMATEX.nAvgBytesPerSec;

	m_nCurrentTs = pAudioSmaple->rtStop;
	
	//入队列
	m_qSampleList.push(pAudioSmaple);

	return S_OK;
}

//获取空闲sample
STDMETHODIMP CAudioPushPin::GetFreeSample(AudioSample** ppAudioSmaple)
{
	//检查输入指针的合法性
	CheckPointer(ppAudioSmaple, E_POINTER);

	//加锁
	CAutoLock cAutoLock(&m_CritSecLock);

	//从空闲队列中取出sample
	*ppAudioSmaple = m_qFreeSampleList.front();

	//弹出队列
	m_qFreeSampleList.pop();

	return *ppAudioSmaple ? S_OK : E_FAIL;
}

HRESULT CAudioPushPin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest)
{
	//检查输入指针的合法性
	CheckPointer(pAlloc, E_POINTER);
	CheckPointer(pRequest, E_POINTER);

	//加锁
	CAutoLock cAutoLock(m_pFilter->pStateLock());

	HRESULT hr = NOERROR;

	//sample分配结构体属性
    pRequest->cBuffers	= 2;
    pRequest->cbAlign	= 1;
	pRequest->cbPrefix	= 0;
	pRequest->cbBuffer = m_tWAVEFORMATEX.nAvgBytesPerSec;

	ALLOCATOR_PROPERTIES Actual;
    if(FAILED(hr = pAlloc->SetProperties(pRequest,&Actual)))
	{
		return hr;
	}

    //ASSERT(Actual.cBuffers == 1);

	//检测可分配的sample大小是否符合需求
    if(pRequest->cBuffers > Actual.cBuffers
	|| pRequest->cbBuffer > Actual.cbBuffer)
	{
		return E_FAIL;
    }
    return NOERROR;
}

HRESULT CAudioPushPin::FillBuffer(IMediaSample *pSample)
{
	if(m_pAudioCaptureClient)
	{
		UINT32 size			= 0;		
		DWORD  flags		= 0;		
		UINT64 pcpos		= 0;
		UINT64 dvepos		= 0;
		UINT32 framesavailable	= 0;
		int i = 100;
		while(i--)
		{
			m_pAudioCaptureClient->GetNextPacketSize(&size);
			if(size > 0)
				break;
			Sleep(1);
		}
		
		size = size * m_tWAVEFORMATEX.wBitsPerSample / 8;
		BYTE* pbuffer = NULL;
		pSample->GetPointer(&pbuffer);
		HRESULT hr = E_FAIL;

		hr = m_pAudioCaptureClient->GetBuffer(&pbuffer, &framesavailable, &flags, &dvepos, &pcpos);

		m_pAudioCaptureClient->ReleaseBuffer(framesavailable);
		if(AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY == flags)
		{
			pSample->SetDiscontinuity(TRUE);
		}
		
		if(m_rtStartTime == -1)
		{
			m_rtStartTime = pcpos;
		}

		size = framesavailable;// * m_tWAVEFORMATEX.nChannels * m_tWAVEFORMATEX.wBitsPerSample/8;
		ASSERT(size);
		pSample->SetActualDataLength(size);

		REFERENCE_TIME rtstop = size;
		REFERENCE_TIME dur = rtstop * 10000000 /m_tWAVEFORMATEX.nAvgBytesPerSec;
		rtstop = m_nCurrentTs + dur;
		pSample->SetTime(&m_nCurrentTs, &rtstop);
		CRefTime reftime;
		while(SUCCEEDED(m_pFilter->StreamTime(reftime)) && reftime.m_time < m_nCurrentTs )
		{
			Sleep(1);
		}
		m_nCurrentTs += dur;		
	}
	return S_OK;
}

HRESULT CAudioPushPin::SetMediaType(const CMediaType *pMediaType)
{
	//加锁
	CAutoLock cAutoLock(m_pFilter->pStateLock());

    // Pass the call up to my base class
    HRESULT hr = CSourceStream::SetMediaType(pMediaType);
	
    return hr;
}

HRESULT CAudioPushPin::CheckMediaType(const CMediaType *pMediaType)
{
	//检查输入指针的合法性
	CheckPointer(pMediaType, E_POINTER);

	//加锁
	CAutoLock cAutoLock(m_pFilter->pStateLock());

	//检查媒体类型
	if(MEDIATYPE_Audio != pMediaType->majortype || MEDIASUBTYPE_PCM != pMediaType->subtype || 	FORMAT_WaveFormatEx	!= pMediaType->formattype)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAudioPushPin::GetMediaType(int iPosition, CMediaType *pmt)
{
	//检查输入指针的合法性
	CheckPointer(pmt, E_POINTER);
	
	//加锁
	CAutoLock cAutoLock(m_pFilter->pStateLock());

	//只能输出一种媒体类型
	if(iPosition != 0)
	{
		return E_INVALIDARG;
	}


	//返回媒体类型
	return MediaTypeFromWaveFormat(&m_tWAVEFORMATEX, pmt);

}

HRESULT CAudioPushPin::MediaTypeFromWaveFormat(WAVEFORMATEX* pwfe, CMediaType *pmt)
{
	//检查输入指针的合法性
	CheckPointer(pmt, E_POINTER);
	CheckPointer(pwfe, E_POINTER);

	//加锁
	CAutoLock cAutoLock(m_pFilter->pStateLock());

	//媒体类型属性赋值
	pmt->majortype				= MEDIATYPE_Audio;
	pmt->subtype				= MEDIASUBTYPE_PCM;
	pmt->formattype				= FORMAT_WaveFormatEx;
	pmt->bTemporalCompression	= FALSE;
	pmt->bFixedSizeSamples		= TRUE;
	pmt->lSampleSize			= pwfe->nChannels * pwfe->nSamplesPerSec * pwfe->wBitsPerSample / 8;
	pwfe->wFormatTag			= WAVE_FORMAT_PCM;

	//分配formattype内存空间
	if(pmt->pbFormat == NULL)
	{
		pmt->AllocFormatBuffer(sizeof(WAVEFORMATEX));
	}
	else
	{
		pmt->ReallocFormatBuffer(sizeof(WAVEFORMATEX));
	}

	//内存分配是否成功
	if(pmt->pbFormat == NULL)
	{
		return E_OUTOFMEMORY;
	}

	//拷贝formattype
	memcpy(pmt->pbFormat, pwfe, sizeof(WAVEFORMATEX));
	pmt->cbFormat				= sizeof(WAVEFORMATEX);

	return S_OK;
}

//重置所有sample
void CAudioPushPin::ResetSamples()
{
	//加锁
	CAutoLock cAutoLock(&m_CritSecLock);

	//清空sample队列
	while(m_qSampleList.size() > 0)
	{
		AudioSample* psample = m_qSampleList.front();

		//重置sample
		psample->ResetSample();

		//推入空闲sample队列
		m_qFreeSampleList.push(psample);

		//从sample队列弹出
		m_qSampleList.pop();
	}
}

HRESULT CAudioPushPin::SetSampleTime(IMediaSample *pSample)
{
	//检查输入指针的合法性
	CheckPointer(pSample,  E_POINTER);

	if(m_tWAVEFORMATEX.nAvgBytesPerSec)
	{
		m_rtFrameLength = (REFERENCE_TIME)pSample->GetActualDataLength()*10000000/m_tWAVEFORMATEX.nAvgBytesPerSec;
	}

	CRefTime now;
	CRefTime endFrame;
	now = 0;
	CSourceStream::m_pFilter->StreamTime(now);
	
	while(now < m_nCurrentTs && m_bAlive) { // guarantees monotonicity too :P
		  //LocalOutput("sleeping because %llu < %llu", now, previousFrameEndTime);
		  Sleep(1);
          CSourceStream::m_pFilter->StreamTime(now);
		}
	
	if(now >= m_nCurrentTs)
	{
		m_nCurrentTs = max(now, m_nCurrentTs);
	}
	else
	{
		now = m_nCurrentTs;
	}

	endFrame = now + m_rtFrameLength;

	m_nCurrentTs = endFrame;
	// accomodate for 0 to avoid startup negatives, which would kill our math on the next loop...
	m_nCurrentTs = max(0, m_nCurrentTs); 

	wchar_t str[100];
	swprintf(str, L"startts:%d, endts:%d\n", (int)now.m_time/10000, (int)endFrame.m_time/10000);
	OutputDebugString(str);
    pSample->SetTime((REFERENCE_TIME *) &now, (REFERENCE_TIME *) &endFrame);

	return S_OK;
}
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
	if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
	if ((punk) != NULL)  \
{ (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

HRESULT CAudioPushPin::CreateAudioDevice()
{
	/*
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;
	IAudioClient *pAudioClient = NULL;
	IAudioCaptureClient *pCaptureClient = NULL;
	WAVEFORMATEX *pwfx = NULL;
	UINT32 packetLength = 0;
	BOOL bDone = FALSE;
	BYTE *pData;
	DWORD flags;

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

		hr = pEnumerator->GetDefaultAudioEndpoint(
		eCapture, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

		hr = pDevice->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)&pAudioClient);
	EXIT_ON_ERROR(hr)

		hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr)

		hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		0,
		hnsRequestedDuration,
		0,
		pwfx,
		NULL);
	EXIT_ON_ERROR(hr)

		// Get the size of the allocated buffer.
		hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr)

		hr = pAudioClient->GetService(
		IID_IAudioCaptureClient,
		(void**)&pCaptureClient);
	EXIT_ON_ERROR(hr)

		// Calculate the actual duration of the allocated buffer.
		hnsActualDuration = (double)REFTIMES_PER_SEC *
		bufferFrameCount / pwfx->nSamplesPerSec;

	hr = pAudioClient->Start();  // Start recording.
	EXIT_ON_ERROR(hr)

Exit:
	return hr;*/

	
	//HRESULT hr = CoInitialize(NULL);
	int i = 100;
	while(i-- > 0)
	{
		//创建设备枚举器
		CComQIPtr<IMMDeviceEnumerator> pEnumerator;
		HRESULT hr = CoCreateInstance( __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

		if(FAILED(hr) || pEnumerator == NULL)
		{
			return hr;
		}

		//获取默认音频输出设备
		CComQIPtr<IMMDevice>		pMMDevice;
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pMMDevice);

		if(FAILED(hr) || pMMDevice == NULL)
		{
			return hr;
		}

		LPWSTR pid = NULL;
		pMMDevice->GetId(&pid);
		IMMEndpoint* pmmep = NULL;
		pMMDevice->QueryInterface(__uuidof(IMMEndpoint), (void**)&pmmep);
		EDataFlow df;
		if(pmmep)
		{
			pmmep->GetDataFlow(&df);
		}
		DWORD dw = 0;
		pMMDevice->GetState(&dw);
		//创建IAudioClient
		hr = pMMDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_pAudioClient);

		if(FAILED(hr) || m_pAudioClient == NULL)
		{
			return hr;
		}

		WAVEFORMATEX* pwfx = NULL;
		WAVEFORMATEXTENSIBLE* pwfxe = NULL;
		hr = m_pAudioClient->GetMixFormat(&pwfx);
		if(pwfx && pwfx->wBitsPerSample == 16)
		{
			m_tWAVEFORMATEX = *pwfx;
			break;
		}
		m_pAudioClient->Stop();
		m_pAudioClient.Release();
		CoTaskMemFree(pwfx);
	}
	
	ASSERT(i > 0);

	//创建设备枚举器
	CComQIPtr<IMMDeviceEnumerator> pEnumerator;
	HRESULT hr = CoCreateInstance( __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

	if(FAILED(hr) || pEnumerator == NULL)
	{
		return hr;
	}

	//获取默认音频输出设备
	CComQIPtr<IMMDevice>		pMMDevice;
	hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pMMDevice);

	if(FAILED(hr) || pMMDevice == NULL)
	{
		return hr;
	}

	LPWSTR pid = NULL;
	pMMDevice->GetId(&pid);
	IMMEndpoint* pmmep = NULL;
	pMMDevice->QueryInterface(__uuidof(IMMEndpoint), (void**)&pmmep);
	EDataFlow df;
	if(pmmep)
	{
		pmmep->GetDataFlow(&df);
	}
	DWORD dw = 0;
	pMMDevice->GetState(&dw);
	//创建IAudioClient
	hr = pMMDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_pAudioClient);

	if(FAILED(hr) || m_pAudioClient == NULL)
	{
		return hr;
	}

	WAVEFORMATEX* pwfx = NULL;
	WAVEFORMATEXTENSIBLE* pwfxe = NULL;
	hr = m_pAudioClient->GetMixFormat(&pwfx);
	WAVEFORMATEX* pwfx2 = &m_tWAVEFORMATEX;
	//hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwfx, &pwfx2);
	hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,0,REFTIMES_PER_SEC,0,pwfx2,NULL);
	//CComQIPtr<IAudioCaptureClient>		pAudioCaptureClient;
	hr = m_pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pAudioCaptureClient);
	

	return hr;
}

HRESULT CAudioPushPin::DestroyAudioDevice()
{
	//m_pAudioClient->Release();
	return S_OK;
}