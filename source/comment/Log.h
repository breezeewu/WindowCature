#pragma once
/*******************************************************************************
  Copyright (C), 2010-2012, QVOD Technology Co.,Ltd.
  File name:    Log.h
  Author:       zwu
  Version:      0.0.1
  Date:			2012-10-10
  Description:   实现日志的打印功能
  Modification:
********************************************************************************/

#define		ENABLE_ALL_INFO				0 //打印所有信息
#define		ENABLE_DETAIL_INFO			1 //打印详细信息
#define		ENABLE_MAIN_INFO			2 //打印主要信息
#define		ENABLE_WARNING_INFO			3 //打印警告以上级别信息
#define		ENABLE_ERROR_INFO			4 //打错误（失败）以上级别信息
#define		ENABLE_SPECIAL_INFO			5 //打印某些特殊信息
//#define		ENABLE_WRITE_LOG_FILE		32 //写日志文件
#define		DISABLE_ALL_INFO			0xFFFFFFFF//MAXINT32

#ifdef _DEBUG
#define		ENABLE_OUTPUT_LOG_LEVEL			ENABLE_DETAIL_INFO//ENABLE_MAIN_INFO//ENABLE_DETAIL_INFO//ENABLE_DETAIL_INFO//ENABLE_ERROR_INFO//
#define		ENABLE_FILE_LOG_LEVEL			ENABLE_MAIN_INFO
#else
#define		ENABLE_OUTPUT_LOG_LEVEL		ENABLE_DETAIL_INFO//ENABLE_DETAIL_INFO//ENABLE_DETAIL_INFO//ENABLE_SPECIAL_INFO//ENABLE_MAIN_INFO//ENABLE_ERROR_INFO//ENABLE_ERROR_INFO//
#define		ENABLE_FILE_LOG_LEVEL		DISABLE_ALL_INFO
#endif
class CLog
{
public:
	CLog(void);
	~CLog(void);

	//创建本地磁盘日志
	bool CreateLocalLog(wchar_t* pwpath, wchar_t* mode = 0);
	bool CreateLocalLog(char* ppath, char* mode = 0);

	//设置/获得最大log长度
	void SetMaxLogLen(int len);
	int  GetMaxLogLen();

	//打印Unicode字符编码的日志
	void trace(const int level, const wchar_t *pwstring, ...);

	//打印宽字节字符编码的日志
	void trace(const int level, const char *pstring, ...);

protected:
	//打开日志
	bool OpenLogFile();

	//关闭日志
	void CloseLogFile();

protected:
	//临界锁
	struct _RTL_CRITICAL_SECTION*			m_pcs;
	struct _iobuf*							m_pFile;
	int										m_nMaxLogLen;
	wchar_t*								m_pwLogName;
	char*									m_pLogName;
	char*									m_pmbInfo;
	wchar_t*								m_pwbInfo;
};
