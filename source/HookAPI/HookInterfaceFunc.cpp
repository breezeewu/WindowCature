#include "HookInterfaceFunc.h"
#include "detours.h"
#pragma comment("lib", "detours.lib")

HOOKDAPIINFO		HookDApiInfo;
HOOKDAPIINFO		HookApiInfo;
//////////////////////////////////////////////////////////////////////////
//替换API函数
VOID TrampolineWith(VOID)	
{
	UINT i;
	for(i=0;i<HookDApiInfo.Count;i++)
	{
		//截获目标函数
		DetourFunctionWithTrampoline(HookDApiInfo.RealObjectApiName[i],HookDApiInfo.MineObjectApiName[i]);
	}


	for(i=0;i<HookApiInfo.Count;i++)
	{
		//截获目标函数
		DetourFunctionWithTrampoline(HookApiInfo.RealObjectApiName[i],HookApiInfo.MineObjectApiName[i]);
	}
}

//恢复API函数
VOID UnTrampolineWith(VOID)
{
	UINT i;
	//初始化静态API函数
	for(i=0;i<HookApiInfo.Count;i++)
	{
		//去掉对一个目标函数的截获
		DetourRemoveWithTrampoline(HookApiInfo.RealObjectApiName[i],HookApiInfo.MineObjectApiName[i]);
	}

	//初始化动态API函数
	for(i=0;i<HookDApiInfo.Count;i++)
	{
		//去掉对一个目标函数的截获
		DetourRemoveWithTrampoline(HookDApiInfo.RealObjectApiName[i],HookDApiInfo.MineObjectApiName[i]);
	}
}

//添加静态HOOK函数
VOID AddHookApi(PBYTE RealObjectApiName,PBYTE MineObjectApiName)
{
	UINT i;
	BOOL found=FALSE;
	//检查静态HOOK信息里是否已经有这个函数
	for(i=0;i<HookApiInfo.Count;i++)
	{
		if(HookApiInfo.RealObjectApiName[i]==RealObjectApiName)
			found=TRUE;
	}

	//如果没有就添加
	if(!found)
	{
		HookApiInfo.Count++;
		HookApiInfo.RealObjectApiName[i]=RealObjectApiName;
		HookApiInfo.MineObjectApiName[i]=MineObjectApiName;
	}
}

//添加动态HOOK函数
VOID AddHookDApi(PBYTE RealObjectApiName,PBYTE MineObjectApiName)
{
	UINT i;
	BOOL found=FALSE;
	//检查动态HOOK信息里是否已经有这个函数
	for(i=0;i<HookDApiInfo.Count;i++)
	{
		if(HookDApiInfo.RealObjectApiName[i]==RealObjectApiName)
			found=TRUE;
	}

	//如果没有就添加
	if(!found)
	{
		HookDApiInfo.Count++;
		HookDApiInfo.RealObjectApiName[i]=RealObjectApiName;
		HookDApiInfo.MineObjectApiName[i]=MineObjectApiName;
	}
}

VOID ReMoveAllApi(VOID)
{
	HookApiInfo.Count = 0 ;
}

VOID ReMoveAllDApi(VOID)
{
	HookDApiInfo.Count = 0 ;
}