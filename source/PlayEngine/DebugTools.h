#pragma once

class CDebugTools
{
public:
	CDebugTools(void);
	~CDebugTools(void);

	//异常处理函数
	static LONG WINAPI ExceptionFilter(__in struct _EXCEPTION_POINTERS* pepinfo);

	//获取错误信息
	int GetErrorMsg(BYTE** perrormsg);

protected:
	BYTE*		m_pErrMsg;
	int			m_nErrMsgSize;
};
