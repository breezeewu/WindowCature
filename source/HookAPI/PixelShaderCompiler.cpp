#include "PixelShaderCompiler.h"
#include <stdio.h>
#include <wxdebug.h>
//#pragma comment(lib, "D3dx9.lib");
CPixelShaderCompiler* CPixelShaderCompiler::m_pPixelShaderCompiler(NULL);

CPixelShaderCompiler::CPixelShaderCompiler(void):m_pD3DXCompileShader(NULL), m_pD3DXDisassembleShader(NULL),m_pBuffer(NULL), m_nBufferSize(0)
{
	InitPSC();
}

CPixelShaderCompiler::~CPixelShaderCompiler(void)
{
}

HRESULT CPixelShaderCompiler::InitPSC()
{
	HMODULE hmd = NULL;
	wchar_t dllname[100];
	int n = 43;
	while(NULL == hmd && n > 0)
	{
		swprintf(dllname, L"d3dx9_%d.dll",n--);

		//加载d3dx9_xx.dll
		hmd = LoadLibrary(dllname);

		if(hmd)
		{
			//加载D3DXCompileShader函数地址
			m_pD3DXCompileShader		= (D3DXCompileShaderPtr)GetProcAddress(hmd, "D3DXCompileShader");
			//ASSERT(m_pD3DXCompileShader);

			//加载D3DXDisassembleShader函数地址
			m_pD3DXDisassembleShader	= (D3DXDisassembleShaderPtr)GetProcAddress(hmd, "D3DXDisassembleShader");
			//ASSERT(m_pD3DXDisassembleShader);
			return S_OK;
		}
	}

	return E_FAIL;
}

CPixelShaderCompiler* CPixelShaderCompiler::GetInstance()
{
	//为空则创建实例
	if(NULL == m_pPixelShaderCompiler)
	{
		m_pPixelShaderCompiler = new CPixelShaderCompiler();
	}
	
	//返回实例对象指针
	return m_pPixelShaderCompiler;
}

HRESULT CPixelShaderCompiler::CompileShader(
								IDirect3DDevice9* pD3DDev,
								LPCSTR pSrcData,
								LPCSTR pFunctionName,
								LPCSTR pProfile,
								DWORD Flags,
								IDirect3DPixelShader9** ppPixelShader,
								CString* disasm,
								CString* errmsg)
{
	CheckPointer(pD3DDev,		E_POINTER);
	//CheckPointer(pSrcData,		E_POINTER);
	CheckPointer(pFunctionName, E_POINTER);
	CheckPointer(pProfile,		E_POINTER);
	CheckPointer(ppPixelShader, E_POINTER);
	CheckPointer(m_pD3DXCompileShader, E_POINTER);
	CheckPointer(m_pD3DXDisassembleShader, E_POINTER);

	HRESULT hr;

	CComPtr<ID3DXBuffer> pShader, pDisAsm, pErrorMsgs;

	if(pSrcData == NULL && m_pBuffer)
	{
		pSrcData = (LPCSTR)m_pBuffer;
	}

	if(m_pD3DXCompileShader)
	{
	//编译shader
	hr = m_pD3DXCompileShader(pSrcData, strlen(pSrcData), NULL, NULL, pFunctionName, pProfile, Flags, &pShader, &pErrorMsgs, NULL);
	}
	else
	{
   // D3DXCompileShader(pSrcData, strlen(pSrcData), NULL, NULL, pFunctionName, pProfile, Flags, &pShader, &pErrorMsgs, NULL);
	}
	if(FAILED(hr))
	{
		if(errmsg) 
		{
			CStringA msg = "Unexpected compiler error";

			if(pErrorMsgs) 
			{
				int len = pErrorMsgs->GetBufferSize();
				memcpy(msg.GetBufferSetLength(len), pErrorMsgs->GetBufferPointer(), len);
			}

			*errmsg = msg;
		}

		return hr;
	}
	
	//创建pixelshader
	hr = pD3DDev->CreatePixelShader((DWORD*)pShader->GetBufferPointer(), ppPixelShader);
	if(FAILED(hr)) {
		return hr;
	}

	if(disasm) {
		hr = m_pD3DXDisassembleShader((DWORD*)pShader->GetBufferPointer(), FALSE, NULL, &pDisAsm);
		if(SUCCEEDED(hr) && pDisAsm) {
			*disasm = CStringA((const char*)pDisAsm->GetBufferPointer());
		}
	}

	return S_OK;
}

HRESULT CPixelShaderCompiler::LoadPixelShader(wchar_t* pPath)
{
	DWORD heightlen = 0, lowlen = 0, readlen = 0;
	
	
	HANDLE handle = CreateFile(pPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == handle)
	{
		return E_FAIL;
	}
	
	lowlen = GetFileSize(handle,&heightlen);

	/*FILE* pfile = _wfopen(pPath, L"r");
	if(pfile == NULL)
	{
		return E_FAIL;
	}
	
	fseek(pfile, 0, SEEK_END);
	lowlen = ftell(pfile);
	fseek(pfile, 0, SEEK_SET);*/

	m_nBufferSize = lowlen + 1;
	m_pBuffer = new BYTE[m_nBufferSize];
	memset(m_pBuffer, 0, m_nBufferSize);
	//readlen = fread(m_pBuffer, 1, lowlen, pfile);
	ReadFile(handle, m_pBuffer, m_nBufferSize, &readlen, NULL);
	CloseHandle(handle);
	if(readlen < lowlen)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
		m_nBufferSize = 0;
		return E_FAIL;
	}

	//fclose(pfile);

	return S_OK;
}
