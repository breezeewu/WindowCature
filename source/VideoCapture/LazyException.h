#pragma once
#include <windows.h>
class CLazyException
{
public:
	CLazyException(void);
public:
	~CLazyException(void);

	//�쳣������
	static LONG WINAPI ExceptionFilter(__in struct _EXCEPTION_POINTERS* pepinfo);

	//��ȡ������Ϣ
	int GetLastErrorMsg(BYTE** perrmsg);

	//��ȡ������Ϣ
	int GetLastErrorMsg(wchar_t** pwerrmsg);

protected:
	char*			m_pErrMsg;			//������Ϣ��MultiByte��
	wchar_t*		m_pwErrMsg;			//������Ϣ��Unicode��
	int				m_nErrMsgSize;
};
