#include <windows.h>

#ifdef _MANAGED
#pragma managed(push, off)//编译托管（推，关闭）

#endif

HINSTANCE	g_hInstance;
/*
EXTERN_C __declspec(dllexport) HRESULT InitPlayEngine(void** ppPlayEngine)
{
	//检测指针的合法性
	CheckPointer(ppPlayEngine, E_POINTER);

	//给指针赋初值
	*ppPlayEngine = NULL;

	//创建实例对象
	*ppPlayEngine = static_cast<IMediaPost*>(new CPlayEngine());

	return S_OK;
}

EXTERN_C __declspec(dllexport) HRESULT DestoryPlayEngine(void** ppPlayEngine)
{
	//检测指针的合法性
	CheckPointer(ppPlayEngine, E_POINTER);

	//还原实例指针对象
	CPlayEngine* pPlayEngine = static_cast<CPlayEngine*>(*ppPlayEngine);

	//销毁实例
	delete pPlayEngine;

	//置空
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
