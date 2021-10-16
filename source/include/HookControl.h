#pragma once
#include <windows.h>
interface IMediaCallBack
{
public:
	//消息回调接口
	virtual long EventNotify(UINT uMsg, long wparam, long lparam) = 0;
};

class IHookControl:public IMediaCallBack
{
public:
/******************************************************************
	函数名:     InitHook
	函数描述:   初始化钩子模块
	参数1:		[in] DWORD ThreadID，线程id，非零为进程内钩子，0为系统钩子
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT InitHook(DWORD ThreadID) = 0;

/******************************************************************
	函数名:     DeInitHook
	函数描述:   销毁所有钩子
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT DeInitHook() = 0;

/******************************************************************
	函数名:     InstallWindowsHookFunc
	函数描述:   安装一个钩子函数
	参数1:		[in] PVOID* pFuncProc
	参数2:		[in] char* pDllName
	参数3:		[in] char* FuncName
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/
	virtual HRESULT InstallWindowsHookFunc(PVOID* ppSystemFuncProc, PVOID pFuncProc) = 0;


/******************************************************************
	函数名:     UnInstallWindowsHookFunc
	函数描述:   卸载一个钩子函数
	参数1:		[in] PVOID* pFuncProc
	返回值：    成功返回S_OK，否则为失败
*******************************************************************/

	virtual HRESULT UnInstallWindowsHookFunc(PVOID pFuncProc) = 0;
};