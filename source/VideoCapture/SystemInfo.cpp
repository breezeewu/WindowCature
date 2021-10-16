#include "stdafx.h"
#include <windows.h>
#include <atlbase.h>
#include "SystemInfo.h"


CSystemInfo CSystemInfo::m_SystemInfo;

CSystemInfo::CSystemInfo(void)
{
	//�ṹ���ʼ��
	memset((BYTE*)&m_sVersionInfo, 0, sizeof(m_sVersionInfo));
	memset((BYTE*)&m_sSystemInfo, 0, sizeof(m_sSystemInfo));
	m_sVersionInfo.dwOSVersionInfoSize = sizeof(m_sVersionInfo);

	//��ü����Ӳ����Ϣ
	BOOL ret = GetVersionEx(&m_sVersionInfo);

	//��ü����ϵͳ�汾����Ϣ
	GetSystemInfo(&m_sSystemInfo);
}

CSystemInfo::~CSystemInfo(void)
{
}

/*****************************************************************************************
Function:       GetInstance
Description:    �����ʵ������
Return:         CSystemInfo&����ʵ����������
Remark:         ��
******************************************************************************************/
CSystemInfo& CSystemInfo::GetInstance()
{
	return m_SystemInfo;
}

/*****************************************************************************************
Function:       GetMajorVersion
Description:    ��õ�ǰ����ϵͳ���汾��
Return:			DWORD����ǰ����ϵͳ���汾��
Remark:         ��
******************************************************************************************/
DWORD CSystemInfo::GetMajorVersion()
{
	return m_sVersionInfo.dwMajorVersion;//
}

/*****************************************************************************************
Function:       GetMinorVersion
Description:    ��õ�ǰ����ϵͳ�ΰ汾��
Return:			DWORD����ǰ����ϵͳ�ΰ汾��
Remark:         ��
******************************************************************************************/
DWORD CSystemInfo::GetMinorVersion()
{
	return m_sVersionInfo.dwMinorVersion;
}

/*****************************************************************************************
Function:       GetProcessNum
Description:    ��õ�ǰ������߼�������
Return:			DWORD����ǰ������߼�������
Remark:         ���cpuΪ˫�߳�cpu�������Ϊ�������*2
******************************************************************************************/
DWORD CSystemInfo::GetProcessNum()
{
	return m_sSystemInfo.dwNumberOfProcessors;
}

/*****************************************************************************************
Function:       IsWow64
Description:    ��ǰ����ϵͳ�Ƿ�Ϊ64λ����ϵͳ
Return:			bool������trueΪ64λϵͳ�����򷵻�false
Remark:         ��
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
Description:    ��ǰ����ϵͳ�Ƿ�Ϊwin8
Return:			bool������trueΪwin8�����򷵻�false
Remark:         ��
******************************************************************************************/
bool  CSystemInfo::IsWin8()
{
	return (m_sVersionInfo.dwMajorVersion == 6 && m_sVersionInfo.dwMinorVersion >= 2) ? true : false;
}

/*****************************************************************************************
Function:       GetProcessRevision
Description:    ��õ�ǰcpu�汾��
Return:			WORD������cpu�汾��
Remark:         ��
******************************************************************************************/
WORD CSystemInfo::GetProcessRevision()
{
	return m_sSystemInfo.wProcessorRevision;
}

/*****************************************************************************************
Function:       GetCPUFrequency
Description:    ��õ�ǰcpu��Ƶ
Return:			DWORD������cpu��Ƶ
Remark:         ��
******************************************************************************************/
DWORD CSystemInfo::GetCPUFrequency()
{
	// ������
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
Description:    ��õ�ǰcpu������Ϣ
Param1:         cpuinfo[out],cpu������Ϣ
Param2:         Ҫ�������ݵ��ڴ��ַ
Return:			��
Remark:         ��
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
Description:    ��õ�ǰcpu����
Param1:         pbuf[out],cpu����
Param2:         int& len[in,out]��pbuf���ڴ��С
Return:			�ɹ�����true������Ϊʧ��
Remark:         ���ʧ�ܣ�����2�����ص�ǰ����1����Ҫ���ڴ��С
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
