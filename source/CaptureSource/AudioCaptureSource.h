#pragma once
#include <streams.h>
#include "AudioPushPin.h"

const GUID CLSID_AUDIO_CAPTURE_SOURCE = {0xDE0E4074, 0x02C1, 0x4128, {0xAE, 0xCB, 0x8B, 0x24, 0x5E, 0x1E, 0xA7, 0x99}};
[uuid("DE0E4074-02C1-4128-AECB-8B245E1EA799")]
class CAudioCaptureSource :
	public CSource
{
public:
	CAudioCaptureSource(IUnknown *pUnk, HRESULT *phr);
public:
	~CAudioCaptureSource(void);

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

	DECLARE_IUNKNOWN;
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	STDMETHODIMP Stop();

protected:
	CAudioPushPin* m_pPin;
};
