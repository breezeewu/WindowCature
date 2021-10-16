#include "AudioHookSource.h"

CAudioHookSource::CAudioHookSource(IUnknown *pUnk, HRESULT *phr)
					: CSource(NAME("PushSourceDesktop Parent"), pUnk, CLSID_AUDIO_HOOK_SOURCE)
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

CAudioHookSource::~CAudioHookSource(void)
{
	//delete m_pPin;
}


CUnknown * WINAPI CAudioHookSource::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
	// the first entry point
    CAudioHookSource *pNewFilter = new CAudioHookSource(lpunk, phr);

	if (phr)
	{
		if (pNewFilter == NULL) 
			*phr = E_OUTOFMEMORY;
		else
			*phr = S_OK;
	}
    return pNewFilter;
}

STDMETHODIMP CAudioHookSource::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	if(__uuidof(ICaptureConfig) == riid)
	{
		return m_pPin->QueryInterface(riid, ppv);
	}

	return __super::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP CAudioHookSource::Stop()
{
	if(m_pPin)
	{
		m_pPin->Stop();
	}
	return __super::Stop();
}