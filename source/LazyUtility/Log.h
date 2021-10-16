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
#include "LazyLock.h"

#define		ENABLE_ALL_INFO				0 //打印所有信息
#define		ENABLE_DETAIL_INFO			1 //打印详细信息
#define		ENABLE_MAIN_INFO			2 //打印主要信息
#define		ENABLE_WARNING_INFO			3 //打印警告以上级别信息
#define		ENABLE_ERROR_INFO			4 //打错误（失败）以上级别信息
#define		ENABLE_SPECIAL_INFO			5 //打印某些特殊信息
//#define		ENABLE_WRITE_LOG_FILE		32 //写日志文件
#define		DISABLE_ALL_INFO			_I32_MAX//MAXINT32

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
	~CLog(void);

/*****************************************************************************************
	Function:       GetInstance
	Description:    获取日志对象实例指针
	Return:         日志对象指针
	Remark:         无
******************************************************************************************/

	static CLog* GetInstance();

/*****************************************************************************************
	Function:       CreateLocalLog
	Description:    按指定路径创建本地磁盘日志
	Param1:         wchar_t* pwpath，Unicode编码的日志路径
	Param2:         wchar_t* mode，打开模式
	Return:         bool型，成功返回true，失败返回false
	Remark:         无
******************************************************************************************/

	bool CreateLocalLog(wchar_t* pwpath, wchar_t* mode = 0);

/*****************************************************************************************
	Function:       CreateLocalLog
	Description:    按指定路径创建本地磁盘日志
	Param1:         char* ppath，宽字符编码的日志路径
	Param2:         char* mode，打开模式
	Return:         bool型，成功返回true，失败返回false
	Remark:         无
******************************************************************************************/

	bool CreateLocalLog(char* ppath, char* mode = 0);

/*****************************************************************************************
	Function:       SetMaxLogLen
	Description:   设置当前日志的最大输出长度
	Param1:         int len,当前支持输出日志的最大长度(单位为wchar_t)
	Return:         无
	Remark:         最大输出长度的字节数为：返回值*sizeof(wcahr_t)
******************************************************************************************/

	void SetMaxLogLen(int len);

/*****************************************************************************************
	Function:       GetMaxLogLen
	Description:    获得当前日志的最大输出长度
	Return:         前日志的最大输出长度
	Remark:         最大输出长度的字节数为：返回值*sizeof(wcahr_t)
******************************************************************************************/

	int  GetMaxLogLen();

/*****************************************************************************************
	Function:       trace
	Description:    按指定日志优先级及格式打印Unicode编码的日志
	Param1:         const int level，日志优先级
	Param2:         const char *pstring，Unicode编码的日志格式
	Param3:         ...，日志字符串格式参数
	Return:         bool型，成功返回true，失败返回false
	Remark:         无
******************************************************************************************/

	void trace(const int level, const wchar_t *pwstring, ...);

/*****************************************************************************************
	Function:       trace
	Description:    按指定日志优先级及格式打印宽字符编码的日志
	Param1:         const int level，日志优先级
	Param2:         const char *pstring，宽字符编码的日志格式
	Param3:         ...，日志字符串格式参数
	Return:         bool型，成功返回true，失败返回false
	Remark:         无
******************************************************************************************/

	void trace(const int level, const char *pstring, ...);

/*****************************************************************************************
	Function:       ResetLogBuffer
	Description:    清空日志buffer信息
	Remark:         无
******************************************************************************************/

	void ResetLogBuffer();

/*****************************************************************************************
	Function:       GetLogInfo
	Description:    获取日志信息
	Param1:         [in,out]BYTE* pbuffer，日志存放buffer指针
	Param2:         [in,out]int& len，日志存放buffer长度
	Return:         bool型，成功返回true，失败返回false
	Remark:         pbuffer为空时，仅返回日志长度，返回值为true，当pbuffer不为空时，成功返回true，失败返回当前日志长度（Param2）
******************************************************************************************/
	bool GetLogInfo(BYTE* pbuffer, int& len);

protected:
	//打开日志
	bool OpenLogFile();

	//关闭日志
	void CloseLogFile();

	//Unicode转宽字符
	int Unicode2char(const wchar_t *pwstring, char* pstring, int len);

	void DumpInfo(const int level, char *pstring);

	//向日志buffer写日志
	bool WriteLogBuffer(const char *pstring);

	//向日志文件写日志
	bool WriteLogFile(const int level, const char *pstring);

	CLog(void);



protected:
	//临界锁
	CCriSec									m_cCriSec;
	struct _iobuf*							m_pFile;
	int										m_nMaxLogLen;
	wchar_t*								m_pwLogName;
	char*									m_pLogName;
	char*									m_pmbInfo;				//宽字节字符串buffer
	wchar_t*								m_pwbInfo;				//Unicode字符串buffer

	char*									m_pLogBuffer;			//日志buffer指针
	int										m_nMaxLogBufferSize;	//日志buffer最大长度
	int										m_nLogOffset;			//日志buffer偏移
	static	CLog							m_Log;
};
