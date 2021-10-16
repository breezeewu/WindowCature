#include "AudioPushPin.h"
#include "AudioHookSource.h"

CAudioPushPin::CAudioPushPin(HRESULT *phr, CAudioHookSource *pFilter)
					: CSourceStream(NAME("Push Source pin"), phr, (CSource*)pFilter, L"Capture")
{
	m_nCurrentTs  = 0;
	m_tWAVEFORMATEX.nBlockAlign = 1;
	m_tWAVEFORMATEX.nChannels			= 2;
	m_tWAVEFORMATEX.nSamplesPerSec		= 44100;
	m_tWAVEFORMATEX.wBitsPerSample		= 16;
	m_tWAVEFORMATEX.wFormatTag			= WAVE_FORMAT_PCM;
	m_tWAVEFORMATEX.nAvgBytesPerSec	= m_tWAVEFORMATEX.nChannels * m_tWAVEFORMATEX.nSamplesPerSec * m_tWAVEFORMATEX.wBitsPerSample/8;
	*phr = S_OK;

	m_hPipeHnd = NULL;

	m_bAlive		= false;
	m_nCurrentTs	= 0;

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

	//开始标记
	m_bAlive = true;

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
#ifdef			NAMEPIPE_COMMUNICATION_TEST
	OutputDebugString(L"FillBuffer");
	if(m_hPipeHnd == NULL)
	{
		
		 m_hPipeHnd = CreateNamedPipe(TEXT("\\\\.\\Pipe\\AudioPipe"),PIPE_ACCESS_DUPLEX |      // read/write access         
																		FILE_FLAG_OVERLAPPED,     // overlapped mode         
																		PIPE_TYPE_MESSAGE |       // message-type pipe         
																		PIPE_READMODE_MESSAGE |   // message read mode         
																		PIPE_WAIT,                // blocking mode         
																		/*PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT*/
																		PIPE_UNLIMITED_INSTANCES, 819200,819200,NMPWAIT_WAIT_FOREVER,0);//创建了一个命名管道
		 OVERLAPPED		overlapped;
		 memset(&overlapped, 0, sizeof(OVERLAPPED));
		 overlapped.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		 if(ConnectNamedPipe(m_hPipeHnd, &overlapped) == NULL)
		 {
			 while(WaitForSingleObject(overlapped.hEvent, 10) == WAIT_TIMEOUT && m_bAlive)
			 {
				 //MessageBox(0,0,0,0);
				 Sleep(1);
				 OutputDebugString(L" Sleep(1)");
			 }

			 MessageBox(0,0,L"等到连接",0);
		 }
		 //if(ConnectNamedPipe(m_hPipeHnd, &overlapped)==NULL)//等待另一客户的链接。
		 //{
		 //  OutputDebugString(L"连接失败！");
		 //}
		 //else
		 //{
		 //  OutputDebugString(L"连接成功");
		 //}
	}

	BYTE* pdata = NULL;
	pSample->GetPointer(&pdata);
	DWORD readlen = 0;
	while(readlen <= 0 && m_bAlive)
	{
		OutputDebugString(L"ReadFile");
		ReadFile(m_hPipeHnd,pdata,pSample->GetSize(),&readlen,NULL);
		OutputDebugString(L"pSample->SetActualDataLength");
		pSample->SetActualDataLength(readlen);
		//__int64 len = readlen;
		SetSampleTime(pSample);
		/*REFERENCE_TIME rtstop = m_nCurrentTs + len * 10000000/44100/2/2;
		pSample->SetTime(&m_nCurrentTs, &rtstop);*/
		//m_nCurrentTs = rtstop;
	}

	OutputDebugString(L"return S_OK");
	return S_OK;
#endif
	//检查队列中是否有可提交的sample，没有则等待
	while(m_qSampleList.size() <= 0 && m_bAlive)
	{
		Sleep(1);
	}
	//加锁
	CAutoLock cAutoLock(&m_CritSecLock);

	//有可提交的sample，则提交sample
	if(m_qSampleList.size() > 0)
	{
		AudioSample* pasample = m_qSampleList.front();
		if(pasample && pasample->Len > 0)
		{
			BYTE* pdata = NULL;
			//获取sample数据指针
			if(SUCCEEDED(pSample->GetPointer(&pdata)) && pdata)
			{
				int copylen = pasample->Len <= pSample->GetSize() ? pasample->Len : pSample->GetSize();

				//拷贝数据
				memcpy(pdata, pasample->pData, copylen);

				//设置sample数据长度
				pSample->SetActualDataLength(copylen);
			}

			//打上时间戳
			pSample->SetTime(&pasample->rtStart, &pasample->rtStop);

			//设置联系性标记
			pSample->SetDiscontinuity(pasample->bContinue);

			return S_OK;
		}
	}
	
	if(m_bAlive)
	{
		ASSERT(0);
	}
	return E_FAIL;

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

HRESULT CAudioPushPin::Stop()
{
	m_bAlive = false;

	return __super::Stop();
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
