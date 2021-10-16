#pragma once
#include <windows.h>
class CLazyException
{
public:
	CLazyException(void);
public:
	~CLazyException(void);

	//异常处理函数
	static LONG WINAPI ExceptionFilter(__in struct _EXCEPTION_POINTERS* pepinfo);

	//获取错误信息
	int GetLastErrorMsg(BYTE** perrormsg);

protected:
	char*		m_pErrMsg;
	int			m_nErrMsgSize;
};
