#pragma once
#include <windows.h>
interface IMediaCallBack
{
public:
	//��Ϣ�ص��ӿ�
	virtual long EventNotify(UINT uMsg, long wparam, long lparam) = 0;
};

class IHookControl:public IMediaCallBack
{
public:
/******************************************************************
	������:     InitHook
	��������:   ��ʼ������ģ��
	����1:		[in] DWORD ThreadID���߳�id������Ϊ�����ڹ��ӣ�0Ϊϵͳ����
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT InitHook(DWORD ThreadID) = 0;

/******************************************************************
	������:     DeInitHook
	��������:   �������й���
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT DeInitHook() = 0;

/******************************************************************
	������:     InstallWindowsHookFunc
	��������:   ��װһ�����Ӻ���
	����1:		[in] PVOID* pFuncProc
	����2:		[in] char* pDllName
	����3:		[in] char* FuncName
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/
	virtual HRESULT InstallWindowsHookFunc(PVOID* ppSystemFuncProc, PVOID pFuncProc) = 0;


/******************************************************************
	������:     UnInstallWindowsHookFunc
	��������:   ж��һ�����Ӻ���
	����1:		[in] PVOID* pFuncProc
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*******************************************************************/

	virtual HRESULT UnInstallWindowsHookFunc(PVOID pFuncProc) = 0;
};