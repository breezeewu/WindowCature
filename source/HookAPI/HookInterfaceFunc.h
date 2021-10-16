#pragma once
#include <windows.h>
#include <map>

//静态替换方法
//用法
//DefHookAPI(源API函数名,源API函数类型,源API函数参数)　//参数格式:(参数1,参数2...)
#define DefHookApi(SourceFunctionName,FunctionType,FunctionParameter) \
	DETOUR_TRAMPOLINE(FunctionType WINAPI Real_##SourceFunctionName##FunctionParameter,SourceFunctionName); \
	FunctionType WINAPI Mine_##SourceFunctionName##FunctionParameter

//添加静态方法到链表中
//用法
//HookAPI(源API函数名);
#define HookApi(SourceFunctionName) \
	AddHookApi((PBYTE)Real_##SourceFunctionName,(PBYTE)Mine_##SourceFunctionName); 

//动态替换方法
//用法
//DefHookDApi(源API函数名,源API函数类型,源API函数参数)　//参数格式:(参数1,参数2...)
#define DefHookDApi(SourceFunctionName,FunctionType,FunctionParameter) \
	typedef FunctionType (WINAPI *P##SourceFunctionName)##FunctionParameter; \
	P##SourceFunctionName Real_##SourceFunctionName = NULL; \
	BOOL Hook##SourceFunctionName##Tag=FALSE; \
	FunctionType WINAPI Mine_##SourceFunctionName##FunctionParameter

//添加动态方法到链表中
//用法
//HookDAPI(源API函数名,源API函数名地址)
#define HookDApi(SourceFunctionName,SourceFunctionAddress) \
	if(!Hook##SourceFunctionName##Tag){ \
	Hook##SourceFunctionName##Tag=TRUE; \
	Real_##SourceFunctionName = (P##SourceFunctionName)DetourFunction((PBYTE)SourceFunctionAddress,(PBYTE)Mine_##SourceFunctionName); \
	AddHookDApi((PBYTE)Real_##SourceFunctionName,(PBYTE)Mine_##SourceFunctionName); }

//定义结构
//静态方法
typedef struct tagHOOKAPIINFO	//HOOK结构
{
	tagHOOKAPIINFO()
	{
		Count = 0 ;
	};

	UINT Count;	//个数
	PBYTE RealObjectApiName[250];	//源API名
	PBYTE MineObjectApiName[250];	//目标API名
}HOOKAPIINFO,*PHOOKAPIINFO;

//动态方法
typedef struct tagHOOKDAPIINFO	//HOOK结构
{
	tagHOOKDAPIINFO()
	{
		Count = 0 ;
	};

	UINT Count;	//个数
	PBYTE RealObjectApiName[250];	//源API名
	PBYTE MineObjectApiName[250];	//目标API名
}HOOKDAPIINFO,*PHOOKDAPIINFO;

//extern HOOKDAPIINFO		HookDApiInfo;
//extern HOOKDAPIINFO		HookApiInfo;
//替换API函数
VOID TrampolineWith(VOID);
//恢复API函数
VOID UnTrampolineWith(VOID);
//删除所有静态函数
VOID ReMoveAllApi(VOID);
//删除所有动态函数
VOID ReMoveAllDApi(VOID);	
//添加静态HOOK信息
VOID AddHookApi(PBYTE ObjectApiName,PBYTE SourceApiName); 
//添加动态HOOK信息
VOID AddHookDApi(PBYTE RealObjectApiName,PBYTE MineObjectApiName);