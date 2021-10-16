#pragma once
#include "..\include\QMediaInfo.h"

class CMediaInfo
{
public:
	static CMediaInfo* GetInstance();
	BOOL init(wchar_t *pFileUrl);
	BOOL GetMediaMainInfo(wchar_t* strURL,Qvod_MainMediaInfo *pMainInfo);
	BOOL GetStreamInfo(IAsyncReader *pReader,Qvod_MainMediaInfo *pMainInfo);
	BOOL GetDetailMediaInfo(wchar_t* strURL,Qvod_DetailMediaInfo *pDetailInfo);
	BOOL GetFileInfo(wchar_t* strURL,s_FileInfo *pLabelInfo);
	BOOL GetQMVInfoInterface(wchar_t* strURL, interface IQMVDemuxFilter** ppqmvdemux, interface IVideoTagQuery** ppvideotgquery);
public:
	~CMediaInfo(void);
protected:
		CMediaInfo(void);
		static CMediaInfo m_MediaInfo;
		HMODULE					m_hMediaInfo_dll;
		GETFILEINFO				m_Get_FileInfo;
		GETSTREAMINFO			m_Get_StreamInfo;
		GETFILEDETAILINFO		m_Get_DetailInfo;
		GETFILELABEL			m_Get_FileLabel;
		GETQMVINFOINTERFACE		m_Get_QMVInfo;
};
