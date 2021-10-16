#include "stdafx.h"
#include <windows.h>
#include <atlbase.h>
#include "SystemInfo.h"


CSystemInfo CSystemInfo::m_SystemInfo;

CSystemInfo::CSystemInfo(void)
{
	//结构体初始化
	memset((BYTE*)&m_sVersionInfo, 0, sizeof(m_sVersionInfo));
	memset((BYTE*)&m_sSystemInfo, 0, sizeof(m_sSystemInfo));
	m_sVersionInfo.dwOSVersionInfoSize = sizeof(m_sVersionInfo);

	//获得计算机硬件信息
	BOOL ret = GetVersionEx(&m_sVersionInfo);

	//获得计算机系统版本号信息
	GetSystemInfo(&m_sSystemInfo);
}

CSystemInfo::~CSystemInfo(void)
{
}

/*****************************************************************************************
Function:       GetInstance
Description:    获得类实例对象
Return:         CSystemInfo&，类实例对象引用
Remark:         无
******************************************************************************************/
CSystemInfo& CSystemInfo::GetInstance()
{
	return m_SystemInfo;
}

/*****************************************************************************************
Function:       GetMajorVersion
Description:    获得当前操作系统主版本号
Return:			DWORD，当前操作系统主版本号
Remark:         无
******************************************************************************************/
DWORD CSystemInfo::GetMajorVersion()
{
	return m_sVersionInfo.dwMajorVersion;//
}

/*****************************************************************************************
Function:       GetMinorVersion
Description:    获得当前操作系统次版本号
Return:			DWORD，当前操作系统次版本号
Remark:         无
******************************************************************************************/
DWORD CSystemInfo::GetMinorVersion()
{
	return m_sVersionInfo.dwMinorVersion;
}

/*****************************************************************************************
Function:       GetProcessNum
Description:    获得当前计算机逻辑处理数
Return:			DWORD，当前计算机逻辑处理数
Remark:         如果cpu为双线程cpu，则核数为物理核数*2
******************************************************************************************/
DWORD CSystemInfo::GetProcessNum()
{
	return m_sSystemInfo.dwNumberOfProcessors;
}

/*****************************************************************************************
Function:       IsWow64
Description:    当前操作系统是否为64位操作系统
Return:			bool，返回true为64位系统，否则返回false
Remark:         无
******************************************************************************************/
bool CSystemInfo::IsWow64()
{
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = false;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"),"IsWow64Process");
	if(NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			return false;
		} 
	}
	return bIsWow64;
}

/*****************************************************************************************
Function:       IsWin8
Description:    当前操作系统是否为win8
Return:			bool，返回true为win8，否则返回false
Remark:         无
******************************************************************************************/
bool  CSystemInfo::IsWin8()
{
	return (m_sVersionInfo.dwMajorVersion == 6 && m_sVersionInfo.dwMinorVersion >= 2) ? true : false;
}

/*****************************************************************************************
Function:       GetProcessRevision
Description:    获得当前cpu版本号
Return:			WORD，返回cpu版本号
Remark:         无
******************************************************************************************/
WORD CSystemInfo::GetProcessRevision()
{
	return m_sSystemInfo.wProcessorRevision;
}

/*****************************************************************************************
Function:       GetCPUFrequency
Description:    获得当前cpu主频
Return:			DWORD，返回cpu主频
Remark:         无
******************************************************************************************/
DWORD CSystemInfo::GetCPUFrequency()
{
	// 检查机器
	DWORD dwClockOfProcessors;
	HKEY hKey = NULL;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey);
	if(hKey)
	{
		DWORD dwSize = sizeof(DWORD);
		DWORD dwType = REG_DWORD;
		RegQueryValueEx(hKey, _T("~MHz"), NULL, &dwType, (BYTE*)&dwClockOfProcessors, &dwSize);
		RegCloseKey(hKey);

		return dwClockOfProcessors;
	}
	return 0;

}

/*****************************************************************************************
Function:       GetCPUInfo
Description:    获得当前cpu描述信息
Param1:         cpuinfo[out],cpu描述信息
Param2:         要拷贝数据的内存地址
Return:			无
Remark:         无
******************************************************************************************/
void CSystemInfo::GetCPUInfo(int cpuinfo[4], int tp)
{
	__asm
	{
		mov eax, tp
			cpuid
			mov edi, cpuinfo
			mov dword ptr [edi], eax
			mov dword ptr [edi+4], ebx
			mov dword ptr [edi+8], ecx
			mov dword ptr [edi+12], edx
	}
}

/*****************************************************************************************
Function:       GetCPUName
Description:    获得当前cpu名称
Param1:         pbuf[out],cpu名称
Param2:         int& len[in,out]，pbuf的内存大小
Return:			成功返回true，否则为失败
Remark:         如果失败，参数2将返回当前参数1所需要的内存大小
******************************************************************************************/
bool CSystemInfo::GetCPUName(char *pbuf, int& len)
{
	if(pbuf == NULL || len < 16*3)
	{
		len = 16*3;
		return false;
	}
	int (*tmp)[4] = (int(*)[4])pbuf;
	GetCPUInfo(*tmp++, 0x80000002);
	GetCPUInfo(*tmp++, 0x80000003);
	GetCPUInfo(*tmp++, 0x80000004);

	return true;
}
