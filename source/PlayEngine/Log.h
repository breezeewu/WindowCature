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
#include "LazyLock.h"

#define		ENABLE_ALL_INFO				0 //��ӡ������Ϣ
#define		ENABLE_DETAIL_INFO			1 //��ӡ��ϸ��Ϣ
#define		ENABLE_MAIN_INFO			2 //��ӡ��Ҫ��Ϣ
#define		ENABLE_WARNING_INFO			3 //��ӡ�������ϼ�����Ϣ
#define		ENABLE_ERROR_INFO			4 //�����ʧ�ܣ����ϼ�����Ϣ
#define		ENABLE_SPECIAL_INFO			5 //��ӡĳЩ������Ϣ
//#define		ENABLE_WRITE_LOG_FILE		32 //д��־�ļ�
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
	Description:    ��ȡ��־����ʵ��ָ��
	Return:         ��־����ָ��
	Remark:         ��
******************************************************************************************/

	static CLog* GetInstance();

/*****************************************************************************************
	Function:       CreateLocalLog
	Description:    ��ָ��·���������ش�����־
	Param1:         wchar_t* pwpath��Unicode�������־·��
	Param2:         wchar_t* mode����ģʽ
	Return:         bool�ͣ��ɹ�����true��ʧ�ܷ���false
	Remark:         ��
******************************************************************************************/

	bool CreateLocalLog(wchar_t* pwpath, wchar_t* mode = 0);

/*****************************************************************************************
	Function:       CreateLocalLog
	Description:    ��ָ��·���������ش�����־
	Param1:         char* ppath�����ַ��������־·��
	Param2:         char* mode����ģʽ
	Return:         bool�ͣ��ɹ�����true��ʧ�ܷ���false
	Remark:         ��
******************************************************************************************/

	bool CreateLocalLog(char* ppath, char* mode = 0);

/*****************************************************************************************
	Function:       SetMaxLogLen
	Description:   ���õ�ǰ��־������������
	Param1:         int len,��ǰ֧�������־����󳤶�(��λΪwchar_t)
	Return:         ��
	Remark:         ���������ȵ��ֽ���Ϊ������ֵ*sizeof(wcahr_t)
******************************************************************************************/

	void SetMaxLogLen(int len);

/*****************************************************************************************
	Function:       GetMaxLogLen
	Description:    ��õ�ǰ��־������������
	Return:         ǰ��־������������
	Remark:         ���������ȵ��ֽ���Ϊ������ֵ*sizeof(wcahr_t)
******************************************************************************************/

	int  GetMaxLogLen();

/*****************************************************************************************
	Function:       trace
	Description:    ��ָ����־���ȼ�����ʽ��ӡUnicode�������־
	Param1:         const int level����־���ȼ�
	Param2:         const char *pstring��Unicode�������־��ʽ
	Param3:         ...����־�ַ�����ʽ����
	Return:         bool�ͣ��ɹ�����true��ʧ�ܷ���false
	Remark:         ��
******************************************************************************************/

	void trace(const int level, const wchar_t *pwstring, ...);

/*****************************************************************************************
	Function:       trace
	Description:    ��ָ����־���ȼ�����ʽ��ӡ���ַ��������־
	Param1:         const int level����־���ȼ�
	Param2:         const char *pstring�����ַ��������־��ʽ
	Param3:         ...����־�ַ�����ʽ����
	Return:         bool�ͣ��ɹ�����true��ʧ�ܷ���false
	Remark:         ��
******************************************************************************************/

	void trace(const int level, const char *pstring, ...);

/*****************************************************************************************
	Function:       ResetLogBuffer
	Description:    �����־buffer��Ϣ
	Remark:         ��
******************************************************************************************/

	void ResetLogBuffer();

/*****************************************************************************************
	Function:       GetLogInfo
	Description:    ��ȡ��־��Ϣ
	Param1:         [in,out]BYTE* pbuffer����־���bufferָ��
	Param2:         [in,out]int& len����־���buffer����
	Return:         bool�ͣ��ɹ�����true��ʧ�ܷ���false
	Remark:         pbufferΪ��ʱ����������־���ȣ�����ֵΪtrue����pbuffer��Ϊ��ʱ���ɹ�����true��ʧ�ܷ��ص�ǰ��־���ȣ�Param2��
******************************************************************************************/
	bool GetLogInfo(BYTE* pbuffer, int& len);

protected:
	//����־
	bool OpenLogFile();

	//�ر���־
	void CloseLogFile();

	//Unicodeת���ַ�
	int Unicode2char(const wchar_t *pwstring, char* pstring, int len);

	void DumpInfo(const int level, char *pstring);

	//����־bufferд��־
	bool WriteLogBuffer(const char *pstring);

	//����־�ļ�д��־
	bool WriteLogFile(const int level, const char *pstring);

	CLog(void);



protected:
	//�ٽ���
	CCriSec									m_cCriSec;
	struct _iobuf*							m_pFile;
	int										m_nMaxLogLen;
	wchar_t*								m_pwLogName;
	char*									m_pLogName;
	char*									m_pmbInfo;				//���ֽ��ַ���buffer
	wchar_t*								m_pwbInfo;				//Unicode�ַ���buffer

	char*									m_pLogBuffer;			//��־bufferָ��
	int										m_nMaxLogBufferSize;	//��־buffer��󳤶�
	int										m_nLogOffset;			//��־bufferƫ��
	static	CLog							m_Log;
};
