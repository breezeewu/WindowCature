#pragma once
#include <streams.h>
#include "AudioPushPin.h"

const GUID CLSID_AUDIO_HOOK_SOURCE = {0xc9960314, 0xeccd, 0x4b58, {0x8d, 0xf8, 0x26, 0x4f, 0x4, 0x9e, 0x1b, 0xf}};
class CAudioHookSource :
	public CSource
{
public:
	CAudioHookSource(IUnknown *pUnk, HRESULT *phr);
public:
	~CAudioHookSource(void);

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

	DECLARE_IUNKNOWN;
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	STDMETHODIMP Stop();

protected:
	CAudioPushPin* m_pPin;
};
