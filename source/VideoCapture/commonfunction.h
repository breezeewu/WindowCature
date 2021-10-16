#pragma once
#include "Config.h"
enum e_CONTAINER;
enum e_V_ENCODER;
enum e_A_ENCODER;

CLSID	CStringToClsid(CString strClsid);
CString CLSID2String(const CLSID& clsid);
void	CStringtoBin(CString str, BYTE *pdata);

e_CONTAINER ContainerFromString(CString str);
e_V_ENCODER VideoEncoderFromString(CString str);
e_A_ENCODER AudioEncoderFromString(CString str);
bool CheckCB(const TCHAR * pchkbytes, const BYTE* pDatas, int dataSize, int& needMoreData);
void	OutPutInfo(const int level, const wchar_t *pstring, ...);