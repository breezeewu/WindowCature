#pragma once
/*******************************************************************************
  Copyright (C), 2010-2012, QVOD Technology Co.,Ltd.
  File name:    SystemInfo.h
  Author:       zwu
  Version:      0.0.1
  Date:			2012-10-11
  Description:   提供计算机硬件及操作系统版本信息
  Modification:
********************************************************************************/

class CSystemInfo
{
protected:
	CSystemInfo(void);
public:
	~CSystemInfo(void);

	static CSystemInfo& GetInstance();

	//获得系统的主版本号
	DWORD GetMajorVersion();

	//获得系统此版本号
	DWORD GetMinorVersion();

	//获得CPU内核数
	DWORD GetProcessNum();

	//是否64位系统
	bool  IsWow64();

	//是否是win8
	bool  IsWin8();

	//获得处理器版本号
	WORD GetProcessRevision();

	//获取CPU主频
	DWORD GetCPUFrequency();

	//获取CPU描述信息
	void GetCPUInfo(int cpuinfo[4], int tp);

	//获取CPU名称
	bool GetCPUName(char *pbuf, int& len);

protected:
	static CSystemInfo m_SystemInfo;

	//计算机硬件信息，包括体系结构、中央处理器的类型、系统中中央处理器的数量、页面的大小以及其他信息
	_SYSTEM_INFO m_sSystemInfo;

	//操作系统的版本信息，包括操作系统的主版本号、副版本号、创建号、以及操作系统平台ID号和关于操作系统的其他描述信息
	_OSVERSIONINFOW m_sVersionInfo;
};
