#include "AudioCaptureSource.h"

CAudioCaptureSource::CAudioCaptureSource(IUnknown *pUnk, HRESULT *phr)
					: CSource(NAME("AudioCaptureSourceDesktop Parent"), pUnk, CLSID_AUDIO_CAPTURE_SOURCE)
{
	m_pPin = new CAudioPushPin(phr, this);

	if(phr)
	{
		if (m_pPin == NULL)
		{
			*phr = E_OUTOFMEMORY;
		}
		else
		{
			*phr = S_OK;
		}
	}
}

CAudioCaptureSource::~CAudioCaptureSource(void)
{
	//delete m_pPin;
}


CUnknown * WINAPI CAudioCaptureSource::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
	// the first entry point
    CAudioCaptureSource *pNewFilter = new CAudioCaptureSource(lpunk, phr);

	if (phr)
	{
		if (pNewFilter == NULL) 
			*phr = E_OUTOFMEMORY;
		else
			*phr = S_OK;
	}
    return pNewFilter;
}

STDMETHODIMP CAudioCaptureSource::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	if(__uuidof(ICaptureConfig) == riid)
	{
		return m_pPin->QueryInterface(riid, ppv);
	}

	return __super::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP CAudioCaptureSource::Stop()
{
	/*if(m_pPin)
	{
	m_pPin->Stop();
	}*/
	return __super::Stop();
}