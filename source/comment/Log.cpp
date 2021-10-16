#include "stdafx.h"
#include "Log.h"
#include <windows.h>
#include <stdio.h>

CLog::CLog(void)
{
	m_pcs = new RTL_CRITICAL_SECTION;
	//��ʼ���ٽ�����Դ
	::InitializeCriticalSection(m_pcs);
	m_pFile = NULL;
	m_nMaxLogLen = MAX_PATH;
	m_pmbInfo = NULL;
	m_pwbInfo = NULL;
	m_pwLogName = NULL;
	m_pLogName = NULL;
	SetMaxLogLen(MAX_PATH);
}

CLog::~CLog(void)
{
	//�����ٽ���
	if(m_pcs)
	{
		//�뿪�ٽ���
		LeaveCriticalSection(m_pcs);
		//�����ٽ�����Դ
		::DeleteCriticalSection(m_pcs);
		delete m_pcs;
		m_pcs = NULL;
	}

	//�ر��ļ�
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	//ɾ��buffer�ڴ�
	if(m_pmbInfo)
	{
		delete[] m_pmbInfo;
		m_pmbInfo = NULL;
	}
	
	if(m_pwbInfo)
	{
		delete[] m_pwbInfo;
		m_pwbInfo = NULL;
	}

	//�����ļ���ָ��
	if(m_pwLogName)
	{
		delete[] m_pwLogName;
		m_pwLogName = NULL;
	}

	if(m_pLogName)
	{
		delete[] m_pLogName;
		m_pLogName = NULL;
	}
	
}

/*****************************************************************************************
Function:       CreateLocalLog
Description:    ��ָ��·���������ش�����־
Param1:         wchar_t* pwpath��Unicode�������־·��
Return:         bool�ͣ��ɹ�����true��ʧ�ܷ���false
Remark:         ��
******************************************************************************************/
bool CLog::CreateLocalLog(wchar_t* pwpath, wchar_t* mode)
{
	if(pwpath == NULL|| ENABLE_FILE_LOG_LEVEL == DISABLE_ALL_INFO)
	{
		return false;
	}

	if(mode == NULL)
	{
		mode = L"w+";
	}

	DWORD ret = GetFileAttributes(pwpath);
	if((FILE_ATTRIBUTE_DIRECTORY & ret) && ret != 0xFFFFFFFF)//��·��Ϊ�ļ���·�������Լ�������־�ļ���
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		wchar_t Logpath[MAX_PATH];

		swprintf(Logpath, L"\\MediaLog%02u%02u%02u%02u%02u%02u.txt",  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		m_pwLogName = new wchar_t[wcslen(pwpath) + wcslen(Logpath) + 1];
		wcscpy(m_pwLogName, pwpath);
		wcscat(m_pwLogName, Logpath);
	}
	else//���ļ�·��Ϊ���ļ���·��
	{
		m_pwLogName = new wchar_t[wcslen(pwpath) + 1];
		wcscpy(m_pwLogName,pwpath);
	}

	if(m_pLogName)
	{
		delete[] m_pLogName;
		m_pLogName = NULL;
	}

	int namelen = (wcslen(pwpath) + 1)*sizeof(wchar_t);
	m_pLogName = new char[namelen];

	WideCharToMultiByte(CP_ACP, 0, m_pwLogName, wcslen(m_pwLogName) + 1, (LPSTR)m_pLogName,  namelen*sizeof(wchar_t), NULL, NULL);

	if(_wfopen_s(&m_pFile, m_pwLogName, mode) == 0 &&  m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
		return true;
	}

	return false;
}

/*****************************************************************************************
Function:       CreateLocalLog
Description:    ��ָ��·���������ش�����־
Param1:         wchar_t* ppath�����ַ��������־·��
Return:         bool�ͣ��ɹ�����true��ʧ�ܷ���false
Remark:         ��
******************************************************************************************/
bool CLog::CreateLocalLog(char* ppath, char* mode)
{
	if(ppath == NULL || ENABLE_FILE_LOG_LEVEL == DISABLE_ALL_INFO)
	{
		return false;
	}

	if(mode == NULL)
	{
		mode = "w+";
	}

	DWORD ret = GetFileAttributesA(ppath);
	if(FILE_ATTRIBUTE_DIRECTORY & ret)//��·��Ϊ�ļ���·�������Լ�������־�ļ���
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		char Logpath[MAX_PATH];

		sprintf(Logpath, "\\Media Log%02u/%02u/%02u %02u:%02u:%02u.txt",  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		m_pLogName = new char[strlen(ppath) + strlen(Logpath) + 1];
		strcpy(m_pLogName, ppath);
		strcat(m_pLogName, Logpath);
	}
	else//���ļ�·��Ϊ���ļ���·��
	{
		m_pLogName = new char[strlen(ppath) + 1];
		strcpy(m_pLogName,ppath);
	}

	if(fopen_s(&m_pFile, m_pLogName, "w+") == 0 &&  m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
		return true;
	}

	return false;
}

/*****************************************************************************************
Function:       SetMaxLogLen
Description:   ���õ�ǰ��־������������
Param1:         int len,��ǰ֧�������־����󳤶�(��λΪwchar_t)
Return:         ǰ��־������������
Remark:         ���������ȵ��ֽ���Ϊ������ֵ*sizeof(wcahr_t)
******************************************************************************************/
void CLog::SetMaxLogLen(int len)
{
	m_nMaxLogLen = len;
	if(m_pwbInfo)
	{
		delete[] m_pwbInfo;		
	}
	m_pwbInfo = new wchar_t[m_nMaxLogLen];

	if(m_pmbInfo)
	{
		delete[] m_pmbInfo;		
	}	
	m_pmbInfo = new char[m_nMaxLogLen*sizeof(wchar_t)];
}

/*****************************************************************************************
Function:       GetMaxLogLen
Description:    ��õ�ǰ��־������������
Return:         ǰ��־������������
Remark:         ���������ȵ��ֽ���Ϊ������ֵ*sizeof(wcahr_t)
******************************************************************************************/
int  CLog::GetMaxLogLen()
{
	return m_nMaxLogLen;
}

/*****************************************************************************************
Function:       trace
Description:    ��ָ����־���ȼ�����ʽ��ӡUnicode�������־
Param1:         const int level����־���ȼ�
Param2:         const char *pstring��Unicode�������־��ʽ
Param2:         ...����־�ַ�����ʽ����
Return:         bool�ͣ��ɹ�����true��ʧ�ܷ���false
Remark:         ��
******************************************************************************************/
void CLog::trace(const int level, const wchar_t *pwstring, ...)
{
	//�ж���־�ȼ������Ƿ��ӡ
	if(level < ENABLE_OUTPUT_LOG_LEVEL && level < ENABLE_FILE_LOG_LEVEL)
	{
		return;
	}

	if(pwstring == NULL)//��Ч����������ӡ
	{
		return;
	}



	va_list _Arglist;
	int _Ret, _Count = 0;
	if(m_pwbInfo == NULL || m_pmbInfo == NULL)
	{
		SetMaxLogLen(MAX_PATH);
	}
	memset(m_pwbInfo, 0, m_nMaxLogLen * sizeof(wchar_t));

	//�����ӡ����
	va_start(_Arglist, pwstring);
	_Ret = _vswprintf_c_l(m_pwbInfo, m_nMaxLogLen, pwstring, NULL, _Arglist);
	va_end(_Arglist);

	if(level >= ENABLE_OUTPUT_LOG_LEVEL)
	{
		//��ӡ������Ϣ
		OutputDebugStringW(m_pwbInfo);
	}

	return ;
	//�����ٽ�����д��־�����õ�
	EnterCriticalSection(m_pcs);

	//���ش����ļ�д��־
	if(level >= ENABLE_FILE_LOG_LEVEL && OpenLogFile() && m_pFile)//��־��Ҫ��ӡ�������־�ļ�
	{
		char str[50];
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf(str, "%2u/%2u/%2u %2u:%2u:%2u:", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		//д�뵱ǰʱ��
		fwrite(str, 1, strlen(str) + 1, m_pFile);

		//��wchar_t���ַ�ת��Ϊ���ֽ�(char)�ַ�
		memset(m_pmbInfo, 0, m_nMaxLogLen*sizeof(wchar_t));
		WideCharToMultiByte(CP_ACP, 0, m_pwbInfo, wcslen(m_pwbInfo) + 1, (LPSTR)m_pmbInfo,  m_nMaxLogLen*sizeof(wchar_t), NULL, NULL);		
		strcat(m_pmbInfo,"\n");

		//д���ӡ��Ϣ
		fwrite(m_pmbInfo, 1, strlen(m_pmbInfo) + 1, m_pFile);

		//�ر���־�ļ�
		CloseLogFile();
	}
	//�˳��ٽ���
	LeaveCriticalSection(m_pcs);
}

/*****************************************************************************************
Function:       trace
Description:    ��ָ����־���ȼ�����ʽ��ӡ���ַ��������־
Param1:         const int level����־���ȼ�
Param2:         const char *pstring�����ַ��������־��ʽ
Param2:         ...����־�ַ�����ʽ����
Return:         bool�ͣ��ɹ�����true��ʧ�ܷ���false
Remark:         ��
******************************************************************************************/
void CLog::trace(const int level, const char *pstring, ...)
{
	//�ж���־�ȼ������Ƿ��ӡ
	if(level < ENABLE_OUTPUT_LOG_LEVEL && level < ENABLE_FILE_LOG_LEVEL)
	{
		return;
	}

	if(pstring == NULL)//��Ч����������ӡ
	{
		return;
	}

	va_list _Arglist;
	int _Ret, _Count = 0;

	if(m_pwbInfo == NULL || m_pmbInfo == NULL)
	{
		SetMaxLogLen(MAX_PATH);
	}
	memset(m_pmbInfo, 0, m_nMaxLogLen*sizeof(wchar_t));

	//�����ӡ����
	va_start(_Arglist, pstring);
	_Ret = /*_sprintf_s_l*/_vsprintf_s_l(m_pmbInfo, m_nMaxLogLen, pstring, NULL, _Arglist);
	va_end(_Arglist);

	if(level >= ENABLE_OUTPUT_LOG_LEVEL)
	{
		//��ӡ������Ϣ
		OutputDebugStringA(m_pmbInfo);
	}

	//�����ٽ�����д��־�����õ�
	EnterCriticalSection(m_pcs);

	//���ش����ļ�д��־
	if(level >= ENABLE_FILE_LOG_LEVEL && OpenLogFile() && m_pFile)//����־�ļ�
	{
		char str[50];
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf(str, "%2u/%2u/%2u %2u:%2u:%2u:", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		//д�뵱ǰʱ��
		fwrite(str, strlen(str) + 1, 1, m_pFile);

		//�ڴ�ӡ��Ϣβ�����뻻�з�
		strcat(m_pmbInfo,"\n");

		//д���ӡ��Ϣ
		fwrite(m_pmbInfo, strlen(m_pmbInfo) + 1, 1, m_pFile);

		//�ر���־�ļ�
		CloseLogFile();
	}

	//�˳��ٽ���
	LeaveCriticalSection(m_pcs);
}

bool CLog::OpenLogFile()
{
	if(m_pLogName && fopen_s(&m_pFile, m_pLogName, "a+") == 0 &&  m_pFile)
	{
		return true;
	}

	return false;
}

void CLog::CloseLogFile()
{
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}