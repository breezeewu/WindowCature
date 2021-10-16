#pragma once
#include <streams.h>
#include <queue>
#include "..\include\CaptureConfig.h"
#define NAMEPIPE_COMMUNICATION_TEST

class CAudioHookSource;

class CAudioPushPin:public CSourceStream, public ICaptureConfig
{
public:
	CAudioPushPin(HRESULT *phr, CAudioHookSource *pFilter);
public:
	~CAudioPushPin(void);

	HRESULT InitMediaType(WAVEFORMATEX* pwfe);
	//CSourceStream
	HRESULT OnThreadCreate(void);

    //////////////////////////////////////////////////////////////////////////
    //  IUnknown
    //////////////////////////////////////////////////////////////////////////
    //STDMETHODIMP QueryInterface(REFIID riid, void **ppv); 
    //STDMETHODIMP_(ULONG) AddRef(){ return GetOwner()->AddRef(); } // gets called often...
    //STDMETHODIMP_(ULONG) Release(){ return GetOwner()->Release(); }

	DECLARE_IUNKNOWN;
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
	//////////////////////////////////////////////////////////////////////////
    //  Interface ICaptureConfig
    //////////////////////////////////////////////////////////////////////////
	//���ò�����Ƶ��ʽ��Ϣ
	STDMETHODIMP SetWAVEFORMAT(WAVEFORMATEX* pwfe);

	//��ȡ������Ƶ��ʽ��Ϣ
	STDMETHODIMP GetWAVEFORMAT(WAVEFORMATEX* pwfe);

	//�����Ƶsample����Ƶ����
	STDMETHODIMP AddAudioSample(AudioSample* pAudioSmaple);

	//��ȡ����sample
	STDMETHODIMP GetFreeSample(AudioSample** ppAudioSmaple);

    // Override the version that offers exactly one media type
    HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest);

    HRESULT FillBuffer(IMediaSample *pSample);
    
    // Set the agreed media type and set up the necessary parameters
    HRESULT SetMediaType(const CMediaType *pMediaType);

    // Support multiple display formats (CBasePin)
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    HRESULT GetMediaType(int iPosition, CMediaType *pmt);

	HRESULT Stop();

protected:
	HRESULT MediaTypeFromWaveFormat(WAVEFORMATEX* pwfe, CMediaType *pmt);

	void ResetSamples();
	HRESULT SetSampleTime(IMediaSample *pSample);

protected:
	WAVEFORMATEX						m_tWAVEFORMATEX;
	std::queue<AudioSample*>			m_qSampleList;
	std::queue<AudioSample*>			m_qFreeSampleList;

	bool								m_bAlive;

	CCritSec							m_CritSecLock;//�ٽ���
	REFERENCE_TIME						m_nCurrentTs;
	REFERENCE_TIME						m_rtFrameLength;

#ifdef			NAMEPIPE_COMMUNICATION_TEST
	HANDLE								m_hPipeHnd;
#endif

};
