#include <windows.h>
#include <DbgHelp.h>
#include <Psapi.h>
#include <string.h>
#include <tchar.h>
#include "DebugTools.h"
#include "Log.h"
//#define		ENABLE_LOG_LEVEL	ENABLE_ERROR_INFO
#pragma   comment(lib,   "DbgHelp.lib")
//#pragma   comment(lib,   "Psapi.lib")
//SetUnhandledExceptionFilter(CDebugTools::ExceptionFilter);
CDebugTools::CDebugTools(void)
{
	m_pErrMsg		= new BYTE[MAX_PATH];
	m_nErrMsgSize	= MAX_PATH;
}

CDebugTools::~CDebugTools(void)
{

	//���ٴ�����Ϣbuffer
	if(m_pErrMsg)
	{
		delete[] m_pErrMsg;
		m_pErrMsg = NULL;
	}
}

LONG WINAPI CDebugTools::ExceptionFilter(__in struct _EXCEPTION_POINTERS* pepinfo)
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

	_stprintf(exinfo, _T("��������δ֪�쳣��������Ϣ���£��쳣���룺0x%x ��־��0x%x ָ���ַ��0x%x"), pepinfo->ExceptionRecord->ExceptionCode, pepinfo->ExceptionRecord->ExceptionFlags, pepinfo->ExceptionRecord->ExceptionAddress);
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

		
		//��ó��������ĺ�������
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
	CLog log;
	log.CreateLocalLog(L"errorlog.txt",L"a+");
	log.SetMaxLogLen(10240);
	log.trace(4,L"Error Info");
	log.trace(4,exinfo);
	//_stprintf(exinfo, _T("��������δ֪�쳣��������Ϣ���£��쳣���룺0x%x  ģ���ַ��0x%x ��־��0x%x ָ���ַ��0x%x\n���������ģ�飺%s\n%s��ջ��Ϣ���£�%s")
	return EXCEPTION_CONTINUE_SEARCH;
}

int CDebugTools::GetErrorMsg(BYTE** pErrMsg)
{
	LPVOID lpMsgBuf; 
     FormatMessage(
     FORMAT_MESSAGE_FROM_SYSTEM | 
     FORMAT_MESSAGE_IGNORE_INSERTS, 
     NULL, 
     GetLastError(), 
     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language 
     (LPTSTR) &m_pErrMsg, 
     m_nErrMsgSize, 
     NULL 
     ); 

	 *pErrMsg = m_pErrMsg;
}