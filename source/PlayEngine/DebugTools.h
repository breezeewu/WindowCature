#pragma once

class CDebugTools
{
public:
	CDebugTools(void);
	~CDebugTools(void);

	//�쳣������
	static LONG WINAPI ExceptionFilter(__in struct _EXCEPTION_POINTERS* pepinfo);

	//��ȡ������Ϣ
	int GetErrorMsg(BYTE** perrormsg);

protected:
	BYTE*		m_pErrMsg;
	int			m_nErrMsgSize;
};
