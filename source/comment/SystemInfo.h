#pragma once
/*******************************************************************************
  Copyright (C), 2010-2012, QVOD Technology Co.,Ltd.
  File name:    SystemInfo.h
  Author:       zwu
  Version:      0.0.1
  Date:			2012-10-11
  Description:   �ṩ�����Ӳ��������ϵͳ�汾��Ϣ
  Modification:
********************************************************************************/

class CSystemInfo
{
protected:
	CSystemInfo(void);
public:
	~CSystemInfo(void);

	static CSystemInfo& GetInstance();

	//���ϵͳ�����汾��
	DWORD GetMajorVersion();

	//���ϵͳ�˰汾��
	DWORD GetMinorVersion();

	//���CPU�ں���
	DWORD GetProcessNum();

	//�Ƿ�64λϵͳ
	bool  IsWow64();

	//�Ƿ���win8
	bool  IsWin8();

	//��ô������汾��
	WORD GetProcessRevision();

	//��ȡCPU��Ƶ
	DWORD GetCPUFrequency();

	//��ȡCPU������Ϣ
	void GetCPUInfo(int cpuinfo[4], int tp);

	//��ȡCPU����
	bool GetCPUName(char *pbuf, int& len);

protected:
	static CSystemInfo m_SystemInfo;

	//�����Ӳ����Ϣ��������ϵ�ṹ�����봦���������͡�ϵͳ�����봦������������ҳ��Ĵ�С�Լ�������Ϣ
	_SYSTEM_INFO m_sSystemInfo;

	//����ϵͳ�İ汾��Ϣ����������ϵͳ�����汾�š����汾�š������š��Լ�����ϵͳƽ̨ID�ź͹��ڲ���ϵͳ������������Ϣ
	_OSVERSIONINFOW m_sVersionInfo;
};
