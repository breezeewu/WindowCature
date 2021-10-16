#include "MediaInfo.h"
#include "streams.h"
CMediaInfo CMediaInfo::m_MediaInfo;
CMediaInfo::CMediaInfo(void)
{
	m_hMediaInfo_dll = NULL;
	m_Get_FileInfo = NULL;
	m_Get_StreamInfo = NULL;
	m_Get_DetailInfo = NULL;
	m_Get_FileLabel = NULL;
}

CMediaInfo::~CMediaInfo(void)
{
	if(m_hMediaInfo_dll)
	{
		FreeLibrary(m_hMediaInfo_dll);
	}
	m_hMediaInfo_dll = NULL;
}

CMediaInfo* CMediaInfo::GetInstance()
{
	return &m_MediaInfo;
}
BOOL CMediaInfo::init(wchar_t *pFileUrl)
{
	if(m_hMediaInfo_dll == NULL)
	{
		if(pFileUrl)
		{
			wchar_t str[MAX_PATH];
			memset(str, 0, MAX_PATH * sizeof(wchar_t));
			wcscpy(str, pFileUrl);
			wcscat(str, L"QMediaInfo.dll");
			m_hMediaInfo_dll = LoadLibrary(str);
		}
		else
		{
			m_hMediaInfo_dll = LoadLibrary(L"QMediaInfo.dll");
		}
		if(m_hMediaInfo_dll)
		{
			m_Get_FileInfo = (GETFILEINFO)GetProcAddress(m_hMediaInfo_dll,"GetFileMediaInfo");
			m_Get_StreamInfo = (GETSTREAMINFO)GetProcAddress(m_hMediaInfo_dll,"GetStreamMediaInfo");
			m_Get_DetailInfo = (GETFILEDETAILINFO)GetProcAddress(m_hMediaInfo_dll,"GetFileDetailMediaInfo");
			m_Get_FileLabel = (GETFILELABEL)GetProcAddress(m_hMediaInfo_dll,"GetFileLabel");
			m_Get_QMVInfo = (GETQMVINFOINTERFACE)GetProcAddress(m_hMediaInfo_dll,"GetQMVInfoInterface");
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}
BOOL CMediaInfo::GetMediaMainInfo(wchar_t* strURL,Qvod_MainMediaInfo *pMainInfo)
{
	CheckPointer(strURL, FALSE);
	CheckPointer(pMainInfo, FALSE);

	if(m_hMediaInfo_dll == NULL)
	{
		init(NULL);
	}

	if(m_Get_FileInfo)
	{
		return m_Get_FileInfo(strURL, pMainInfo);
	}
	return FALSE;
}
BOOL CMediaInfo::GetStreamInfo(IAsyncReader *pReader,Qvod_MainMediaInfo *pMainInfo)
{
	CheckPointer(pReader, FALSE);
	CheckPointer(pMainInfo, FALSE);

	if(m_hMediaInfo_dll == NULL)
	{
		init(NULL);
	}

	if(m_Get_StreamInfo)
	{
		return m_Get_StreamInfo(pReader, pMainInfo);
	}
	return FALSE;
}
BOOL CMediaInfo::GetDetailMediaInfo(wchar_t* strURL,Qvod_DetailMediaInfo *pDetailInfo)
{
	CheckPointer(strURL, FALSE);
	CheckPointer(pDetailInfo, FALSE);

	if(m_hMediaInfo_dll == NULL)
	{
		init(NULL);
	}

	if(m_Get_DetailInfo)
	{
		return m_Get_DetailInfo(strURL, pDetailInfo);
	}
	return FALSE;
}
BOOL CMediaInfo::GetFileInfo(wchar_t* strURL,s_FileInfo *pFileInfo)
{
	CheckPointer(strURL, FALSE);
	CheckPointer(pFileInfo, FALSE);

	if(m_hMediaInfo_dll == NULL)
	{
		init(NULL);
	}

	if(m_Get_FileLabel)
	{
		return m_Get_FileLabel(strURL, pFileInfo);
	}
	return FALSE;
}

BOOL CMediaInfo::GetQMVInfoInterface(wchar_t* strURL, interface IQMVDemuxFilter** ppqmvdemux, interface IVideoTagQuery** ppvideotgquery)
{
	CheckPointer(strURL, FALSE);

	if(m_Get_QMVInfo)
	{
		return m_Get_QMVInfo(strURL, ppqmvdemux, ppvideotgquery);
	}
	return FALSE;
}