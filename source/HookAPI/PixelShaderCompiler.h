#pragma once
#include <windows.h>
#include <d3dx9shader.h>
#include <atlstr.h>

class CPixelShaderCompiler
{
protected:
	CPixelShaderCompiler(void);

public:
	~CPixelShaderCompiler(void);

	//��ȡCPixelShaderCompilerʵ������
	static CPixelShaderCompiler* GetInstance();

	//����pixelshader
	HRESULT CompileShader(
				IDirect3DDevice9* pD3DDev,
				LPCSTR pSrcData,
				LPCSTR pFunctionName,
				LPCSTR pProfile,
				DWORD Flags,
				IDirect3DPixelShader9** ppPixelShader,
				CString* disasm = NULL,
				CString* errmsg = NULL);

	//����PixelShader
	HRESULT LoadPixelShader(wchar_t* pPath);

protected:
	//��ʼ�������ر��뺯����ַ
	HRESULT InitPSC();

protected:
	typedef HRESULT (WINAPI * D3DXCompileShaderPtr) (
		LPCSTR		pSrcData,
		UINT		SrcDataLen,
		CONST D3DXMACRO* pDefines,
		LPD3DXINCLUDE	pInclude,
		LPCSTR		pFunctionName,
		LPCSTR		pProfile,
		DWORD		Flags,
		LPD3DXBUFFER*	ppShader,
		LPD3DXBUFFER*	ppErrorMsgs,
		LPD3DXCONSTANTTABLE* ppConstantTable);

	typedef HRESULT (WINAPI * D3DXDisassembleShaderPtr) (
		CONST DWORD*	pShader,
		bool		EnableColorCode,
		LPCSTR		pComments,
		LPD3DXBUFFER*	ppDisassembly);

	D3DXCompileShaderPtr					m_pD3DXCompileShader;
	D3DXDisassembleShaderPtr				m_pD3DXDisassembleShader;

protected:

	static CPixelShaderCompiler*			m_pPixelShaderCompiler;
	BYTE*									m_pBuffer;
	DWORD									m_nBufferSize;
};
