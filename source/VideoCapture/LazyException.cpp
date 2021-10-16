#include "stdafx.h"
#include "LazyException.h"
#include <DbgHelp.h>
#include <Psapi.h>
#include <string.h>
#include <tchar.h>
#include "Log.h"

#pragma comment(lib, "dbghelp.lib")

CLazyException::CLazyException(void)
{
	m_pErrMsg		= new char[MAX_PATH];
	m_pwErrMsg		= new wchar_t[MAX_PATH];
	m_nErrMsgSize	= MAX_PATH;
}

CLazyException::~CLazyException(void)
{
	//���ٶ��ַ���������Ϣbuffer
	if(m_pErrMsg)
	{
		delete[] m_pErrMsg;
		m_pErrMsg = NULL;
	}

	//����Unicode������Ϣbuffer
	if(m_pwErrMsg)
	{
		delete[] m_pwErrMsg;
		m_pwErrMsg = NULL;
	}
}

LONG WINAPI CLazyException::ExceptionFilter(__in struct _EXCEPTION_POINTERS* pepinfo)
{
	//MessageBox(NULL, NULL, _T("Ҫ���Բ���"), MB_TOPMOST);
	//��������ջ��Ƭ
	STACKFRAME	sf;
	//������ָ���ļĴ����������
	CONTEXT		context;

	//����ģ���·��
	TCHAR modulepath[MAX_PATH] = {0};
	TCHAR info[MAX_PATH] = {0};
	TCHAR exinfo[10240] = {0};

	//�ڴ��ַ��Ϣ�ṹ��
	MEMORY_BASIC_INFORMATION mbi = {0};
	int len = MAX_PATH;

	_stprintf_s(exinfo, _T("��������δ֪�쳣��������Ϣ���£��쳣���룺0x%x ��־��0x%x ָ���ַ��0x%x"), pepinfo->ExceptionRecord->ExceptionCode, pepinfo->ExceptionRecord->ExceptionFlags, pepinfo->ExceptionRecord->ExceptionAddress);
	//��ѯ��ַ�ռ����ڴ��ַ����Ϣ
	if(FALSE == ::VirtualQuery(pepinfo->ExceptionRecord->ExceptionAddress, &mbi, sizeof(mbi)))
	{
		//failed, do something...
	}
	
	_stprintf(info, _T(" ģ���ַ��0x%x\n"), mbi.AllocationBase);
	_tcscat(exinfo,info);

	//����ģ��������ַ����HMODULE(HINSTANCE)Ϊͬһֵ
	HMODULE hmodule = (HMODULE)mbi.AllocationBase;

	//��ñ���ģ��·��
	if(hmodule == NULL || ::GetModuleFileName(hmodule, modulepath, len) <= 0)
	{
		//failed, do something...
	}

	_stprintf(info, _T("���������ģ�飺%s\n"), modulepath);
	_tcscat(exinfo,info);

	memset(&sf, 0, sizeof(sf));
	memset(&context, 0, sizeof(context));
	memcpy(&context, pepinfo->ContextRecord, sizeof(CONTEXT));

	sf.AddrPC.Offset		= context.Eip;
    sf.AddrPC.Mode			= AddrModeFlat;
    sf.AddrStack.Offset		= context.Esp;
    sf.AddrStack.Mode		= AddrModeFlat;
    sf.AddrFrame.Offset	    = context.Ebp;
    sf.AddrFrame.Mode		= AddrModeFlat;

    DWORD machineType    = IMAGE_FILE_MACHINE_I386;
	//��ǰ���̵�α���
    HANDLE hProcess        = GetCurrentProcess();
	//��õ�ǰ�̵߳��߳�id
    HANDLE hThread        = GetCurrentThread();
    SymInitialize(hProcess,NULL,TRUE);

	//дdump
	HANDLE hFile = CreateFile(  _T("project.dmp"),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);  
    MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;
    stExceptionParam.ThreadId    = GetCurrentThreadId();
	stExceptionParam.ExceptionPointers = pepinfo;
	stExceptionParam.ClientPointers    = FALSE;
	MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile,MiniDumpWithFullMemory,&stExceptionParam,NULL,NULL);  
	CloseHandle(hFile);  


	while(1)
	{
		//��õ���ջ������Ƭ(stack frame)
		if(!StackWalk(machineType, hProcess, hThread, &sf, &context, 0, SymFunctionTableAccess, SymGetModuleBase, 0))
		{
			break;
		}

		if(sf.AddrFrame.Offset == 0)
		{
			break;
		}

		BYTE* buffer = new BYTE[sizeof(SYMBOL_INFO) + 255*sizeof(TCHAR)];
		memset(buffer, 0, sizeof(SYMBOL_INFO) + 255*sizeof(TCHAR));
		SYMBOL_INFO* pSymbol = (SYMBOL_INFO*)buffer;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = 256;
		_stprintf(info, _T("sf.AddrPC.Offset:0x%x"), sf.AddrPC.Offset);
		_tcscat(exinfo, info);

		
		//��ó������ĺ�������
		if(SymFromAddr(hProcess, sf.AddrPC.Offset, 0, pSymbol))
		{
			_stprintf(info, _T("Function:%S\n"), pSymbol->Name);
			_tcscat(exinfo, info);
		}
		else
		{
			DWORD hr = GetLastError();
			_stprintf(info, _T("get Function name failed, error code:0x%x\n"), hr);
			_tcscat(exinfo, info);
		}

		DWORD dwLineDisplacement = 0;
		IMAGEHLP_LINE lineinfo = { sizeof(IMAGEHLP_LINE) };
		memset(info, 0, sizeof(TCHAR)*MAX_PATH);
		//�õ�����Դ�����λ��
		if(SymGetLineFromAddr(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineinfo))
		{
			_stprintf(info, _T("Source File:%S, Source Line:%u\n"), lineinfo.FileName, lineinfo.LineNumber);
			_tcscat(exinfo, info);
		}
		else
		{
			_stprintf(info, _T("�޷����Դ�ļ�λ��\n"));
			_tcscat(exinfo, info);
		}
	}

	//MessageBox(NULL, exinfo, _T("����������"), MB_TOPMOST);
	//CLog::GetInstance()->CreateLocalLog(L"errorlog.txt",L"a+");
	//CLog::GetInstance()->SetMaxLogLen(10240);
	CLog::GetInstance()->trace(4,L"Error Info");
	CLog::GetInstance()->trace(4,exinfo);
	//_stprintf(exinfo, _T("��������δ֪�쳣��������Ϣ���£��쳣���룺0x%x  ģ���ַ��0x%x ��־��0x%x ָ���ַ��0x%x\n���������ģ�飺%s\n%s��ջ��Ϣ���£�%s")
	return EXCEPTION_CONTINUE_SEARCH;
}

int CLazyException::GetLastErrorMsg(BYTE** pErrMsg)
{
	memset(m_pErrMsg, 0, m_nErrMsgSize);
	LPVOID lpMsgBuf; 
     FormatMessageA(
     FORMAT_MESSAGE_FROM_SYSTEM | 
     FORMAT_MESSAGE_IGNORE_INSERTS, 
     NULL, 
     GetLastError(), 
     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language 
     (LPSTR) &m_pErrMsg, 
     m_nErrMsgSize, 
     NULL 
     ); 

	 *pErrMsg = (BYTE*)m_pErrMsg;

	 return strlen(m_pErrMsg);
}

int CLazyException::GetLastErrorMsg(wchar_t** pwerrmsg)
{
	memset(m_pErrMsg, 0, m_nErrMsgSize);
	LPVOID lpMsgBuf; 
	FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL, 
		GetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language 
		(LPWSTR) &m_pwErrMsg, 
		m_nErrMsgSize, 
		NULL 
		); 

	*pwerrmsg = m_pwErrMsg;

	return wcslen(m_pwErrMsg);
}