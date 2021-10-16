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
	int GetLastErrorMsg(BYTE** perrmsg);

	//获取错误信息
	int GetLastErrorMsg(wchar_t** pwerrmsg);

protected:
	char*			m_pErrMsg;			//错误信息（MultiByte）
	wchar_t*		m_pwErrMsg;			//错误信息（Unicode）
	int				m_nErrMsgSize;
};
