#pragma once
/*******************************************************************************
  Copyright (C), 2010-2012, QVOD Technology Co.,Ltd.
  File name:    Log.h
  Author:       zwu
  Version:      0.0.1
  Date:			2012-10-10
  Description:   ʵ����־�Ĵ�ӡ����
  Modification:
********************************************************************************/

#define		ENABLE_ALL_INFO				0 //��ӡ������Ϣ
#define		ENABLE_DETAIL_INFO			1 //��ӡ��ϸ��Ϣ
#define		ENABLE_MAIN_INFO			2 //��ӡ��Ҫ��Ϣ
#define		ENABLE_WARNING_INFO			3 //��ӡ�������ϼ�����Ϣ
#define		ENABLE_ERROR_INFO			4 //�����ʧ�ܣ����ϼ�����Ϣ
#define		ENABLE_SPECIAL_INFO			5 //��ӡĳЩ������Ϣ
//#define		ENABLE_WRITE_LOG_FILE		32 //д��־�ļ�
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

	//�������ش�����־
	bool CreateLocalLog(wchar_t* pwpath, wchar_t* mode = 0);
	bool CreateLocalLog(char* ppath, char* mode = 0);

	//����/������log����
	void SetMaxLogLen(int len);
	int  GetMaxLogLen();

	//��ӡUnicode�ַ��������־
	void trace(const int level, const wchar_t *pwstring, ...);

	//��ӡ���ֽ��ַ��������־
	void trace(const int level, const char *pstring, ...);

protected:
	//����־
	bool OpenLogFile();

	//�ر���־
	void CloseLogFile();

protected:
	//�ٽ���
	struct _RTL_CRITICAL_SECTION*			m_pcs;
	struct _iobuf*							m_pFile;
	int										m_nMaxLogLen;
	wchar_t*								m_pwLogName;
	char*									m_pLogName;
	char*									m_pmbInfo;
	wchar_t*								m_pwbInfo;
};
