#include "HookInterfaceFunc.h"
#include "detours.h"
#pragma comment("lib", "detours.lib")

HOOKDAPIINFO		HookDApiInfo;
HOOKDAPIINFO		HookApiInfo;
//////////////////////////////////////////////////////////////////////////
//�滻API����
VOID TrampolineWith(VOID)	
{
	UINT i;
	for(i=0;i<HookDApiInfo.Count;i++)
	{
		//�ػ�Ŀ�꺯��
		DetourFunctionWithTrampoline(HookDApiInfo.RealObjectApiName[i],HookDApiInfo.MineObjectApiName[i]);
	}


	for(i=0;i<HookApiInfo.Count;i++)
	{
		//�ػ�Ŀ�꺯��
		DetourFunctionWithTrampoline(HookApiInfo.RealObjectApiName[i],HookApiInfo.MineObjectApiName[i]);
	}
}

//�ָ�API����
VOID UnTrampolineWith(VOID)
{
	UINT i;
	//��ʼ����̬API����
	for(i=0;i<HookApiInfo.Count;i++)
	{
		//ȥ����һ��Ŀ�꺯���Ľػ�
		DetourRemoveWithTrampoline(HookApiInfo.RealObjectApiName[i],HookApiInfo.MineObjectApiName[i]);
	}

	//��ʼ����̬API����
	for(i=0;i<HookDApiInfo.Count;i++)
	{
		//ȥ����һ��Ŀ�꺯���Ľػ�
		DetourRemoveWithTrampoline(HookDApiInfo.RealObjectApiName[i],HookDApiInfo.MineObjectApiName[i]);
	}
}

//��Ӿ�̬HOOK����
VOID AddHookApi(PBYTE RealObjectApiName,PBYTE MineObjectApiName)
{
	UINT i;
	BOOL found=FALSE;
	//��龲̬HOOK��Ϣ���Ƿ��Ѿ����������
	for(i=0;i<HookApiInfo.Count;i++)
	{
		if(HookApiInfo.RealObjectApiName[i]==RealObjectApiName)
			found=TRUE;
	}

	//���û�о����
	if(!found)
	{
		HookApiInfo.Count++;
		HookApiInfo.RealObjectApiName[i]=RealObjectApiName;
		HookApiInfo.MineObjectApiName[i]=MineObjectApiName;
	}
}

//��Ӷ�̬HOOK����
VOID AddHookDApi(PBYTE RealObjectApiName,PBYTE MineObjectApiName)
{
	UINT i;
	BOOL found=FALSE;
	//��鶯̬HOOK��Ϣ���Ƿ��Ѿ����������
	for(i=0;i<HookDApiInfo.Count;i++)
	{
		if(HookDApiInfo.RealObjectApiName[i]==RealObjectApiName)
			found=TRUE;
	}

	//���û�о����
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