#include "Log.h"
#include <windows.h>
#include <stdio.h>

//静态变量声明
CLog CLog::m_Log;

CLog::CLog(void)
{
	m_pFile				= NULL;
	m_nMaxLogLen		= MAX_PATH;
	m_pmbInfo			= NULL;
	m_pwbInfo			= NULL;
	m_pwLogName			= NULL;
	m_pLogName			= NULL;	

	//日志buffer最大长度
	m_nMaxLogBufferSize = 65536;
	m_nLogOffset		= 0;
	m_pLogBuffer		= new char[m_nMaxLogBufferSize];
	memset(m_pLogBuffer, 0, m_nMaxLogBufferSize);
	SetMaxLogLen(MAX_PATH);
}

CLog::~CLog(void)
{
	//关闭文件
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	//删除buffer内存
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

	//消毁文件名指针
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

	//销毁日志buffer
	if(m_pLogBuffer)
	{
		delete[] m_pLogBuffer;
		m_pLogBuffer = NULL;
	}
	
}

/*****************************************************************************************
Function:       GetInstance
Description:    获取日志对象实例指针
Return:         日志对象指针
Remark:         无
******************************************************************************************/
CLog* CLog::GetInstance()
{
	return &m_Log;
}

/*****************************************************************************************
Function:       CreateLocalLog
Description:    按指定路径创建本地磁盘日志
Param1:         wchar_t* pwpath，Unicode编码的日志路径
Param2:         wchar_t* mode，打开模式
Return:         bool型，成功返回true，失败返回false
Remark:         无
******************************************************************************************/
bool CLog::CreateLocalLog(wchar_t* pwpath, wchar_t* mode)
{
	if(pwpath == NULL|| ENABLE_FILE_LOG_LEVEL == DISABLE_ALL_INFO)
	{
		return false;
	}

	//加锁
	CLazyLock Lock(m_cCriSec);

	if(mode == NULL)
	{
		mode = L"w+";
	}

	DWORD ret = GetFileAttributes(pwpath);
	if((FILE_ATTRIBUTE_DIRECTORY & ret) && ret != 0xFFFFFFFF)//该路径为文件夹路径，则自己生成日志文件名
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		wchar_t Logpath[MAX_PATH];

		swprintf(Logpath, L"\\MediaLog%02u%02u%02u%02u%02u%02u.txt",  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		m_pwLogName = new wchar_t[wcslen(pwpath) + wcslen(Logpath) + 1];
		wcscpy(m_pwLogName, pwpath);
		wcscat(m_pwLogName, Logpath);
	}
	else//该文件路径为非文件夹路径
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
Description:    按指定路径创建本地磁盘日志
Param1:         char* ppath，宽字符编码的日志路径
Param2:         char* mode，打开模式
Return:         bool型，成功返回true，失败返回false
Remark:         无
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

	//加锁
	CLazyLock Lock(m_cCriSec);

	DWORD ret = GetFileAttributesA(ppath);
	if(FILE_ATTRIBUTE_DIRECTORY & ret)//该路径为文件夹路径，则自己生成日志文件名
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		char Logpath[MAX_PATH];

		sprintf(Logpath, "\\Media Log%02u/%02u/%02u %02u:%02u:%02u.txt",  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		m_pLogName = new char[strlen(ppath) + strlen(Logpath) + 1];
		strcpy(m_pLogName, ppath);
		strcat(m_pLogName, Logpath);
	}
	else//该文件路径为非文件夹路径
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
Description:   设置当前日志的最大输出长度
Param1:         int len,当前支持输出日志的最大长度(单位为wchar_t)
Return:         前日志的最大输出长度
Remark:         最大输出长度的字节数为：返回值*sizeof(wcahr_t)
******************************************************************************************/
void CLog::SetMaxLogLen(int len)
{
	//加锁
	CLazyLock Lock(m_cCriSec);

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
Description:    获得当前日志的最大输出长度
Return:         前日志的最大输出长度
Remark:         最大输出长度的字节数为：返回值*sizeof(wcahr_t)
******************************************************************************************/
int  CLog::GetMaxLogLen()
{
	return m_nMaxLogLen;
}

/*****************************************************************************************
Function:       trace
Description:    按指定日志优先级及格式打印Unicode编码的日志
Param1:         const int level，日志优先级
Param2:         const char *pstring，Unicode编码的日志格式
Param2:         ...，日志字符串格式参数
Return:         bool型，成功返回true，失败返回false
Remark:         无
******************************************************************************************/
void CLog::trace(const int level, const wchar_t *pwstring, ...)
{
	if(pwstring == NULL)//无效参数，不打印
	{
		return;
	}

	//加锁
	CLazyLock Lock(m_cCriSec);

	va_list _Arglist;
	int _Ret, _Count = 0;
	if(m_pwbInfo == NULL || m_pmbInfo == NULL)
	{
		SetMaxLogLen(MAX_PATH);
	}
	memset(m_pwbInfo, 0, m_nMaxLogLen * sizeof(wchar_t));

	//输入打印参数
	va_start(_Arglist, pwstring);
	_Ret = _vswprintf_c_l(m_pwbInfo, m_nMaxLogLen, pwstring, NULL, _Arglist);
	va_end(_Arglist);

	//Unicode转宽字符
	int len = Unicode2char(m_pwbInfo, m_pmbInfo, m_nMaxLogLen * sizeof(wchar_t));
	
	//输出日志信息
	DumpInfo(level, m_pmbInfo);
}

/*****************************************************************************************
Function:       trace
Description:    按指定日志优先级及格式打印宽字符编码的日志
Param1:         const int level，日志优先级
Param2:         const char *pstring，宽字符编码的日志格式
Param2:         ...，日志字符串格式参数
Return:         bool型，成功返回true，失败返回false
Remark:         无
******************************************************************************************/
void CLog::trace(const int level, const char *pstring, ...)
{
	if(pstring == NULL)//无效参数，不打印
	{
		return;
	}

	//加锁
	CLazyLock Lock(m_cCriSec);

	va_list _Arglist;
	int _Ret, _Count = 0;

	if(m_pwbInfo == NULL || m_pmbInfo == NULL)
	{
		SetMaxLogLen(MAX_PATH);
	}
	memset(m_pmbInfo, 0, m_nMaxLogLen*sizeof(wchar_t));

	//输入打印参数
	va_start(_Arglist, pstring);
	_Ret = /*_sprintf_s_l*/_vsprintf_s_l(m_pmbInfo, m_nMaxLogLen, pstring, NULL, _Arglist);
	va_end(_Arglist);

	//输出日志信息
	DumpInfo(level, m_pmbInfo);
}

/*****************************************************************************************
	Function:       ResetLogBuffer
	Description:    清空日志buffer信息
	Remark:         无
******************************************************************************************/

void CLog::ResetLogBuffer()
{
	//加锁
	CLazyLock Lock(m_cCriSec);

	//清空日志buffer
	memset(m_pLogBuffer, 0, m_nMaxLogBufferSize);
	m_nLogOffset = 0;
}

/*****************************************************************************************
	Function:       GetLogInfo
	Description:    获取日志信息
	Param1:         [in,out]BYTE* pbuffer，日志存放buffer指针
	Param2:         [in,out]int& len，日志存放buffer长度
	Return:         bool型，成功返回true，失败返回false
	Remark:         pbuffer为空时，仅返回日志长度，返回值为true，当pbuffer不为空时，成功返回true，失败返回当前日志长度（Param2）
******************************************************************************************/

bool CLog::GetLogInfo(BYTE* pbuffer, int& len)
{
	//加锁
	CLazyLock Lock(m_cCriSec);

	if(pbuffer == NULL)
	{
		//返回当前日志长度
		len = m_nLogOffset;
		return true;
	}

	if(len >= m_nLogOffset)
	{
		//拷贝数据
		memcpy(pbuffer, m_pLogBuffer, m_nLogOffset);

		//日志buffer长度
		len = m_nLogOffset;

		return true;
	}
	return false;
}

bool CLog::OpenLogFile()
{
	//加锁
	CLazyLock Lock(m_cCriSec);

	if(m_pLogName && fopen_s(&m_pFile, m_pLogName, "a+") == 0 &&  m_pFile)
	{
		return true;
	}

	return false;
}

void CLog::CloseLogFile()
{
	//加锁
	CLazyLock Lock(m_cCriSec);

	//关闭文件句柄
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

int CLog::Unicode2char(const wchar_t *pwstring, char* pstring, int len)
{
	//加锁
	CLazyLock Lock(m_cCriSec);

	//宽字节字符串buffer置零
	memset(pstring, 0, len);

	//Unicode转宽字符
	int mblen = WideCharToMultiByte(CP_ACP, 0, pwstring, wcslen(pwstring) + 1, (LPSTR)pstring,  len, NULL, NULL);

	return mblen;
}

void CLog::DumpInfo(const int level, char *pstring)
{
	//加锁
	CLazyLock Lock(m_cCriSec);


	if(!strstr(pstring, "\n"))
	{
		strcat(pstring, "\n");
	}

	//写入日志buffer
	WriteLogBuffer(pstring);

	if(level >= ENABLE_OUTPUT_LOG_LEVEL)
	{
		//打印调试信息
		OutputDebugStringA(pstring);
	}

	//写日志文件
	WriteLogFile(level, pstring);
}

bool CLog::WriteLogBuffer(const char *pstring)
{
	//加锁
	CLazyLock Lock(m_cCriSec);

	bool ret = false;
	int len = strlen(pstring);
	if(m_pLogBuffer)
	{
		sprintf(&m_pLogBuffer[m_nLogOffset], "%s:", __TIME__);
		m_nLogOffset += strlen(&m_pLogBuffer[m_nLogOffset]);

		//检测是否溢出
		len = m_nLogOffset + len > m_nMaxLogBufferSize ? (m_nMaxLogBufferSize - m_nLogOffset) : len;
		if(len > 0)
		{
			
			memcpy((void*)&m_pLogBuffer[m_nLogOffset], pstring, len);
			ret = true;
		}
	}
	
	return ret;
}

//向日志文件写日志
bool CLog::WriteLogFile(const int level, const char *pstring)
{
	//加锁
	CLazyLock Lock(m_cCriSec);

	//本地磁盘文件写日志
	if(level >= ENABLE_FILE_LOG_LEVEL && OpenLogFile() && m_pFile)//打开日志文件
	{
		char str[50];
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf(str, "%2u/%2u/%2u %2u:%2u:%2u:", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		//写入当前时间
		fwrite(str, strlen(str) + 1, 1, m_pFile);

		//写入打印信息
		fwrite(m_pmbInfo, strlen(m_pmbInfo) + 1, 1, m_pFile);

		//关闭日志文件
		CloseLogFile();

		return true;
	}

	return false;
}