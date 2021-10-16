#pragma once
#include <windows.h>
#include <map>

//��̬�滻����
//�÷�
//DefHookAPI(ԴAPI������,ԴAPI��������,ԴAPI��������)��//������ʽ:(����1,����2...)
#define DefHookApi(SourceFunctionName,FunctionType,FunctionParameter) \
	DETOUR_TRAMPOLINE(FunctionType WINAPI Real_##SourceFunctionName##FunctionParameter,SourceFunctionName); \
	FunctionType WINAPI Mine_##SourceFunctionName##FunctionParameter

//��Ӿ�̬������������
//�÷�
//HookAPI(ԴAPI������);
#define HookApi(SourceFunctionName) \
	AddHookApi((PBYTE)Real_##SourceFunctionName,(PBYTE)Mine_##SourceFunctionName); 

//��̬�滻����
//�÷�
//DefHookDApi(ԴAPI������,ԴAPI��������,ԴAPI��������)��//������ʽ:(����1,����2...)
#define DefHookDApi(SourceFunctionName,FunctionType,FunctionParameter) \
	typedef FunctionType (WINAPI *P##SourceFunctionName)##FunctionParameter; \
	P##SourceFunctionName Real_##SourceFunctionName = NULL; \
	BOOL Hook##SourceFunctionName##Tag=FALSE; \
	FunctionType WINAPI Mine_##SourceFunctionName##FunctionParameter

//��Ӷ�̬������������
//�÷�
//HookDAPI(ԴAPI������,ԴAPI��������ַ)
#define HookDApi(SourceFunctionName,SourceFunctionAddress) \
	if(!Hook##SourceFunctionName##Tag){ \
	Hook##SourceFunctionName##Tag=TRUE; \
	Real_##SourceFunctionName = (P##SourceFunctionName)DetourFunction((PBYTE)SourceFunctionAddress,(PBYTE)Mine_##SourceFunctionName); \
	AddHookDApi((PBYTE)Real_##SourceFunctionName,(PBYTE)Mine_##SourceFunctionName); }

//����ṹ
//��̬����
typedef struct tagHOOKAPIINFO	//HOOK�ṹ
{
	tagHOOKAPIINFO()
	{
		Count = 0 ;
	};

	UINT Count;	//����
	PBYTE RealObjectApiName[250];	//ԴAPI��
	PBYTE MineObjectApiName[250];	//Ŀ��API��
}HOOKAPIINFO,*PHOOKAPIINFO;

//��̬����
typedef struct tagHOOKDAPIINFO	//HOOK�ṹ
{
	tagHOOKDAPIINFO()
	{
		Count = 0 ;
	};

	UINT Count;	//����
	PBYTE RealObjectApiName[250];	//ԴAPI��
	PBYTE MineObjectApiName[250];	//Ŀ��API��
}HOOKDAPIINFO,*PHOOKDAPIINFO;

//extern HOOKDAPIINFO		HookDApiInfo;
//extern HOOKDAPIINFO		HookApiInfo;
//�滻API����
VOID TrampolineWith(VOID);
//�ָ�API����
VOID UnTrampolineWith(VOID);
//ɾ�����о�̬����
VOID ReMoveAllApi(VOID);
//ɾ�����ж�̬����
VOID ReMoveAllDApi(VOID);	
//��Ӿ�̬HOOK��Ϣ
VOID AddHookApi(PBYTE ObjectApiName,PBYTE SourceApiName); 
//��Ӷ�̬HOOK��Ϣ
VOID AddHookDApi(PBYTE RealObjectApiName,PBYTE MineObjectApiName);