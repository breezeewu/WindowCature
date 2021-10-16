#pragma once

#ifndef QVODSTRUCTURE_H_
#define QVODSTRUCTURE_H_
#include <streams.h>
#include <atlbase.h>
#include <atlstr.h>
#include "CommonStructure.h"
#include <vector>
#ifdef UNICODE
#define ZeroTChar wmemset
#else
#define ZeroTChar memset
#endif

//#define SafeDelete(pObject)		 if(pObject) {delete pObject; pObject = NULL;}
//#define SafeDeleteArray(p)		 if(p){delete []p; p = NULL;}
//
//#ifdef _WIN32
//#define SafeRelease(pInterface)  if(pInterface) {pInterface->Release(); pInterface = NULL;}
//#define SafeDelObj(pObject)		 if(pObject){::DeleteObject(pObject); pObject = 0;}	
//#define SafeDelHandle(pHandle)	 if(pHandle){::CloseHandle(pHandle);pHandle = 0;}
//#endif

#define COPYCHAR(psrc, pdest)		 if(pdest){int nlen = _tcslen(pdest) + 1; psrc = new wchar_t[nlen]; _tcscpy(psrc, pdest); psrc[nlen-1] = 0; }



enum e_FilterType
{
	_EFT_SRC = 0,
	_EFT_SPL,
	_EFT_SUB,
	_EFT_A_DEC,
	_EFT_V_DEC,
	_EFT_A_TRANS,
	_EFT_V_TRANS,
	_EFT_AUDIO_RENDER,
	_EFT_VIDEO_RENDER,
	_EFT_SPY,
	_EFT_DEMUL,
	_EFT_V_ENCODER,
	_EFT_A_ENCODER,
	_EFT_MUX,
	_EFT_PSI,
	_EFT_WRIT
};
enum e_ACCELERATE_TYPE
{
	E_HA_NOSUPPORTED,
	E_HA_SUPPORTED,
	E_HA_SUCCESSED,
	E_HA_FAILED

};

enum e_GRAPH_TYPE
{
	E_DXVA_GRAPH,
	E_BEST_GRAPH,	
	E_CHECKBYTE_GRAPH,
	E_SUFFIX_GRAPH,
	E_FFMPEG_GRAPH,
	E_DVD_GRAPH,
	E_NULL_GRAPH
};

//#ifndef E_CONTAINER_DEFINE
//#define E_CONTAINER_DEFINE
//enum e_CONTAINER
//{
//	_E_CON_UNKNOW = 0,
//	_E_CON_RMVB = 1,
//	_E_CON_AVI,
//	_E_CON_MKV,
//	_E_CON_MP4,
//	_E_CON_MOV,
//	_E_CON_FLV,
//	_E_CON_AMR,
//	_E_CON_DC,
//	_E_CON_DAT,
//	_E_CON_DSM,
//	_E_CON_MP3,
//	_E_CON_OGG,
//	_E_CON_PMP,
//	_E_CON_WM,
//	_E_CON_TS,
//	_E_CON_BIK,
//	_E_CON_CSF,
//	_E_CON_MPEG,
//	_E_CON_HKMPG,
//	_E_CON_DTS,
//	_E_CON_QMV,
//	_E_CON_MIDI,
//	_E_CON_MPEGTS,
//	_E_CON_VP5,
//	_E_CON_WVP,
//	_E_CON_OTHER = 0x100
//};
//#endif


// Filter�����������Ϣ
typedef struct _ComponentInfo
{
	_ComponentInfo():bReg(false)
	{

	}
	CString path;	// ���·��
	bool	bReg;	// �Ƿ���Ҫע��
	CLSID	clsid;	// ���ID
} COMPONENT_INFO;

// ý������
typedef struct _MediaType
{
	CLSID major;
	CLSID sub;
	CLSID format;
	DWORD fcc;
	int   priority;//���ȼ�
} QVOD_MEDIA_TYPE;

// typedef struct _Decoder_Priority
// {
// 	e_V_ENCODER encode;
// 	DWORD		priority;
// 	_Decoder_Priority()
// 	{
// 		encode = _V_OTHER;
// 		priority = 5;
// 	}
// } QVOD_FILTER_DECODER_PRIORITY;

// Filter����
enum e_Filter_System_Apply
{
	e_Apply_All,
	e_Apply_XP_Only,
	e_Apply_Vista_Only,//Vista+
};

// �������� -> Source & Splitter
typedef struct  _container_to_src
{
	e_CONTAINER			container;
	std::vector<CLSID>	vSource;
	std::vector<CLSID>	vSplitters;
} QVOD_CONTAINER_TO_SRC;

// �������� -> Decoder 
typedef struct _v_encoder_to_decoder
{
	e_V_ENCODER			encoder;
	std::vector<CLSID>	vDecoders;
} QVOD_VIDEO_DECODER;

typedef struct _a_encoder_to_decoder
{
	e_A_ENCODER			encoder;
	std::vector<CLSID>	vDecoders;
} QVOD_AUDIO_DECODER;

// ���ٽ����������
typedef struct QVOD_ACCEL_SOLUTION
{
	CString						solution_name;		// ��������
	CString						solution_comment;	// ��������

	std::vector<QVOD_CONTAINER_TO_SRC>	vContainers;		// ��������
	std::vector<QVOD_VIDEO_DECODER>		vVDecoders;		// ���ٽ�����
	std::vector<CLSID>					vRenders;		// ��Ⱦ��
} QVOD_ACCEL_SOLUTION;

// CheckByte��Suffix��Filters��ӳ��
typedef struct _FilterMap
{
	e_CONTAINER					container;
	std::vector<CString>		vCheckBytes;	// CheckByte
	std::vector<CString>		vSuffix;		// ��׺��
	std::vector<CLSID>			vectFilters;	// ʹ�õ�Filters
} QVOD_FILTER_MAP;


struct S_TEMMEMORY
{
	BYTE *pTemMemory;
	//int MemoryOffset;
	int FileOffset;
	int DataLen;
	int MemorySize;
	S_TEMMEMORY():pTemMemory(NULL), FileOffset(0), DataLen(0), MemorySize(0){}
};
//
struct VORBISFORMAT  
{
	unsigned int vorbisVersion;
	unsigned int samplesPerSec;
	unsigned int minBitsPerSec;
	unsigned int avgBitsPerSec;
	unsigned int maxBitsPerSec;
	char numChannels;
};

struct S_FILTER 
{
	bool			IsSupportDXVA;
	LPCTSTR			lpfiltername;
	long			lorder;
	e_FilterType	filtertype;
	LPCTSTR			lpdllpath;
	LPCTSTR			lpclsid;
	long			lreload;				//һ��GPֻ��������Ĵ���	0 ������
};

struct S_OTHERDLL
{
	S_FILTER		*ps_filter;
	LPCTSTR			lpdllpath;
	LPCTSTR         lpSubKey;
	bool            bloadDur;
};

struct S_CHKBYTE
{
	S_FILTER		*ps_filter;
	enum e_CONTAINER		e_container;
	LPCTSTR			lpstrckbyte;
};

struct S_FILEEXT
{
	enum e_CONTAINER		e_container;
	LPCTSTR			lpstrfileext;
};

struct S_PINTYPE
{
	S_FILTER		*ps_filter;
	GUID			major;
	GUID			sub;
	bool			IsSupportDXVA;
};

struct S_E_CON_SPLITER
{
	e_CONTAINER		e_container;
	S_FILTER		*ps_filter;
};

typedef LONG    (WINAPI   *pfnDllGetClassObject)(REFCLSID , REFIID, LPVOID *);

struct S_LOADDLL
{
	HMODULE					hmodule;
	pfnDllGetClassObject	fnDllGetClassObject;
	CString					strdllpath;
};

struct S_REGDLL
{
	S_LOADDLL *ploaddll;
	bool RegFlag;
	bool bloadDur;
	S_REGDLL():ploaddll(NULL),RegFlag(false), bloadDur(false){};
};

struct S_ADDFILTER
{
	e_FilterType	filtertype;
	long			filterorder;
	IBaseFilter		*pbasefilter;
	S_FILTER		*ps_filter;
	bool            connflag;
	S_ADDFILTER():connflag(false)
	{
		ps_filter = NULL;
		pbasefilter = NULL;
	}
};
struct S_ADDPIN
{
	S_ADDFILTER		s_addfilter;
	int				nfind;//0 find 1 add 2 conn
	IPin			*ppin;
	S_ADDPIN():nfind(0), ppin(NULL){}
};

struct S_SubLang
{
	int index;
	LCID Lcid;
	TCHAR szLang[30];
};

typedef struct _QvodFilterInfo
{
	_QvodFilterInfo()
	{
		bEnable	= true;
		capacity = 0;	
		clsid = wrapid = catid = CLSID_NULL;
		bDMO = false;
	}
	DWORD		capacity;		//Filter Capacity
	bool		bEnable;		//Filter�ɲ�����
	CString		strName;		//Filter����
	CString		path;			//Filter·��
	CString		wrapperpath;	//Wrapper path
	bool		bDMO;			//DMO Flag
	CLSID		clsid;			// CLSID
	CLSID		wrapid;			// wrap ID
	CLSID		catid;			// ����ID
	//DWORD		version;		// ϵͳ�汾��
	e_Filter_System_Apply eSystem;// ϵͳ����
	std::vector<COMPONENT_INFO>		vComponents;	//Filter�������
	std::vector<QVOD_MEDIA_TYPE>	vInTypes;	// ����ý������
	//std::vector<QVOD_FILTER_DECODER_PRIORITY>	vDecoderPriority;
} QVOD_FILTER_INFO;

#define SafeDelete(pObject)		 if(pObject) {delete pObject; pObject = NULL;}
#define SafeDeleteArray(p)		 if(p){delete []p; p = NULL;}

#ifdef _WIN32
#define SafeRelease(pInterface)  if(pInterface) {pInterface->Release(); pInterface = NULL;}
#define SafeDelObj(pObject)		 if(pObject){::DeleteObject(pObject); pObject = 0;}	
#define SafeDelHandle(pHandle)	 if(pHandle){::CloseHandle(pHandle);pHandle = 0;}
#endif

#endif