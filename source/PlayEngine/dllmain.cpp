#include <windows.h>

#ifdef _MANAGED
#pragma managed(push, off)//�����йܣ��ƣ��رգ�

#endif

HINSTANCE	g_hInstance;
/*
EXTERN_C __declspec(dllexport) HRESULT InitPlayEngine(void** ppPlayEngine)
{
	//���ָ��ĺϷ���
	CheckPointer(ppPlayEngine, E_POINTER);

	//��ָ�븳��ֵ
	*ppPlayEngine = NULL;

	//����ʵ������
	*ppPlayEngine = static_cast<IMediaPost*>(new CPlayEngine());

	return S_OK;
}

EXTERN_C __declspec(dllexport) HRESULT DestoryPlayEngine(void** ppPlayEngine)
{
	//���ָ��ĺϷ���
	CheckPointer(ppPlayEngine, E_POINTER);

	//��ԭʵ��ָ�����
	CPlayEngine* pPlayEngine = static_cast<CPlayEngine*>(*ppPlayEngine);

	//����ʵ��
	delete pPlayEngine;

	//�ÿ�
	*ppPlayEngine = NULL;

	return S_OK;
	
}*/


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hInstance = hModule;
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
