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
	int GetLastErrorMsg(BYTE** perrormsg);

protected:
	char*		m_pErrMsg;
	int			m_nErrMsgSize;
};
