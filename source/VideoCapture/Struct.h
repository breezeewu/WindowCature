#pragma once
#include "Config.h"
#include <vector>
#include <atlstr.h>
#include "windows.h"
//#include "GUID.h"
#include "streams.h"


//<!-- Filter Capacity Bit位说明 -->
//<!-- 第一位 该Filter具备读取数据能力，如SYS FileReader，QVodSource,DCBass Source, MIDI Parser -->
//<!-- 第二位 该Filter具备解析数据能力（Splitter)-->
//<!-- 第三位 该Filter具备视频解码能力（Video Decoder） -->
//<!-- 第四位 该Filter具备音频解码能力（Audio Decoder） -->
//<!-- 第五位 该Filter具备Video Trans能力，如QvodPostVideo，QvodSubTitle,其他字幕Filter） -->
//<!-- 第六位 该Filter具备Audio Trans能力，如Qvod Sound Filter -->
//<!-- 第七位 该Filter具备Video Render能力，如CSF Render, FFMPEG Filter -->
//<!-- 第八位 该Filter具备Audio Render能力，如CSF Render, FFMPEG Filter -->
#define E_FILTER_CAPACITY_READ			0x01
#define E_FILTER_CAPACITY_SPLIT			0x02
#define E_FILTER_CAPACITY_VIDEO_DEC		0x04
#define E_FILTER_CAPACITY_AUDIO_DEC		0x08
#define E_FILTER_CAPACITY_VIDEO_TRANS	0x10
#define E_FILTER_CAPACITY_AUDIO_TRANS	0x20
#define E_FILTER_CAPACITY_VIDEO_RENDER  0x40
#define E_FILTER_CAPACITY_AUDIO_RENDER	0x80
#define E_FILTER_CAPACITY_VIDEO_ENC		0x100
#define E_FILTER_CAPACITY_AUDIO_ENC		0x200
#define E_FILTER_CAPACITY_MUX			0x400
#define E_FILTER_CAPACITY_REND			0x800

#ifndef MEDIA_ENUM_TYPE
#define MEDIA_ENUM_TYPE
//媒体文件类型
enum e_CONTAINER
{
	_E_CON_UNKNOW = 0,

	_E_CON_RMVB = 1,
	_E_CON_AVI,
	_E_CON_MKV,
	_E_CON_MP4,		//MPEG-4 Part 14,3GP系
	_E_CON_3GP,		//3GP

	_E_CON_MOV,		//Apple系 包含MOV，QT
	//_E_CON_QT,
	
	_E_CON_MPEG,	//MPEG1/2/4 标准容器格式
	_E_CON_DAT,
	_E_CON_MPEG4ES,	//MPEG4基本流
	_E_CON_MPEGTS,	//TS
	_E_CON_MPEGPS,	//VOB EVO HD-DVD
	_E_CON_HKMPG,	//Hikvision mp4
	_E_CON_DIVX,	//Divx Media Format(DMF)
	_E_CON_WM,		//ASF/WMV/WMA/WMP
	_E_CON_TS,		//TS
	_E_CON_M2TS,	//M2TS/MTS,支持DBAV 与 AVCHD
	_E_CON_DVD,		//DVD盘，特殊处理（DVD导航）

	_E_CON_QMV,
	_E_CON_VP5,
	_E_CON_VP6,
	_E_CON_VP7,
	_E_CON_WEBM,//VP8

	_E_CON_FLV,
	_E_CON_PMP,
	_E_CON_BIK,
	_E_CON_CSF,
	_E_CON_DV,	

	_E_CON_XLMV,//迅雷格式

	_E_CON_M2V,
	_E_CON_SWF,
	_E_CON_AMV, //Anime Music Video File
	_E_CON_IVF,//Indeo Video Format file
	_E_CON_IVM,

	_E_CON_SCM,
	_E_CON_AOB,

	_E_CON_AMR,
	_E_CON_DC,
	_E_CON_DSM,
	_E_CON_MP2,
	_E_CON_MP3,
	_E_CON_OGG,
	_E_CON_DTS,
	_E_CON_MIDI,
	_E_CON_WVP,
	_E_CON_AAC,
	_E_CON_AC3,
	_E_CON_AIFF,
	_E_CON_ALAW,
	_E_CON_APE,
	_E_CON_AU,
	_E_CON_AWB,//AMR-WB
	_E_CON_CDA,
	_E_CON_FLAC,
	_E_CON_MOD,
	_E_CON_MPA,//MPEG-2 Audio File
	_E_CON_MPC,//Musepack Audio
	_E_CON_RA,
	_E_CON_TTA,
	_E_CON_WAV,
	_E_CON_CDXA,  

	_E_CON_SKM,
	_E_CON_MTV,
	_E_CON_MXF,
	_E_CON_OTHER = 0x100
};

//视频编码类型
enum e_V_ENCODER
{
	/*MPEG 系列*/
	_V_MPEG_1,
	_V_MPEG_2,

	_V_MPEG_4_Visual,//Simple
	_V_MPEG_4_Advanced_Simple,//Advanced Simple

	_V_XVID,
	_V_DIVX,
	//_V_AVC,//AVC1/H264/X264

	/*ITU 系列*/
	_V_H261,
	_V_H263,
	_V_H263_PLUS,
	_V_H264,

	/*Real Media系列*/
	_V_RV10,
	_V_RV20,
	_V_RV30,
	_V_RV40,			

/*Windows 系列*/
// 	Windows Media Video v7	WMV1	0	
// 	Microsoft MPEG-4 Video Codec v3	MP43	1	
// 	Windows Media Video v8	WMV2	2	
// 	Microsoft MPEG-4 Video Codec v2	MP42	3	
// 	Microsoft ISO MPEG-4 Video Codec v1	MP4S	4	
// 	Windows Media Video v9	WMV3	5	
// 	Windows Media Video v9 Advanced Profile	WMVA	6	deprecated as not VC-1不完全兼容。
// 	Windows Media Video v9 Advanced Profile	WVC1	7[来源请求]	VC-1完整支持

	_V_WM_MPEG4_V1,//Microsoft ISO MPEG-4 Video Codec
	_V_WM_MPEG4_V2,//Microsoft MPEG-4 Video Codec v2	MP42
	_V_WM_MPEG4_V3,//Microsoft MPEG-4 Video Codec v3	MP43

	_V_WMV7,//Windows Media Video v7	WMV1
	_V_WMV8,//Windows Media Video v8	WMV2
	_V_WMV9,//Windows Media Video v9	WMV3 
	_V_WVC1,//Windows Media Video v9 Advanced Profile	WVC1
	_V_MSVIDEO1,//MS VIDEO 1
	/*Google(ON2)*/
	_V_VP3,
	_V_VP4,
	_V_VP5,
	_V_VP6,
	_V_VP60,  
	_V_VP61,
	_V_VP62,
	_V_VP7,   
	_V_VP8,	

	/*其他*/
	_V_HFYU,		//Huffyuv
	_V_YUV,			//YUV
	_V_YV12,		//YV12
	_V_UYVY,		//UYVY
	_V_JPEG,        //JPEG11
	_V_RGB,
	_V_PNG,

	_V_IV31,		//IndeoV31
	_V_IV32,

	//_V_FLV,			//FLV
	_V_MJPG,		//MOTION JPEG
	_V_LJPG,		//LossLess JPEG

	_V_ZLIB,		//AVIZlib
	_V_TSCC,		//tscc
	_V_CVID,		//Cinepak
	_V_RAW,			//uncompressed Video
	_V_SMC,			//Graphics
	_V_RLE,			//Animation
	_V_APPV,		//Apple Video
	_V_KPCD,		//Kodak Photo CD
	_V_SORV,		//Sorenson video
	_V_SCRN,		//Screen video
	_V_THEORA,		//Ogg Theora         
	_V_ZMP5,
	_V_CSF,
	_V_OTHER	= 0x100	//未知
};

enum e_A_ENCODER
{
	_A_AAC	= 0,		//LC-AAC/HE-AAC
	_A_AC3,				//AC3
	_A_E_AC3,			//E-AC-3
	_A_MP2,				//MPEG1 Layer 2
	_A_MP3,				//MPEG1 Layer 3
	_A_MP4,             //MPEG4 Audio
	_A_AMR_NB,			//AMR
	_A_AMR_WB,
	_A_DTS,
	_A_MIDI,
	_A_PCM,				//脉冲编码调制
	_A_DPCM,			//差分脉冲编码调制
	_A_APCM,			//自适应脉冲编码调制
	_A_ADPCM,			//自适应差分脉冲编码调制
	_A_PCM_LE,			//PCM Little Endian
	_A_PCM_BE,			//PCM Big	 Endian

	_A_COOK,			//rmvb AUDIO
	_A_SIPR,			//rmvb AUDIO
	_A_TSCC,			//tscc

	_A_QCELP,			//Qualcomm公司于1993年提出了可变速率的CELP，常称为QCELP,QCELP 语音压缩编码算法包括8K和13K两种
	
	_A_WMA1,
	_A_WMA2,
	_A_WMA3,
	
	_A_WMA4,
	_A_WMA5,
	_A_WMA6,
	
	_A_WMA7,
	_A_WMA8,
	_A_WMA9,            //windows media audio 9.0

	_A_RAW,             //uncompressed audio
	_A_MAC,             //kMACE3Compression
	_A_IMA,             //kIMACompression, Samples have been compressed using IMA 4:1.
	_A_LAW,             //kULawCompression,kALawCompression, uLaw2:1
	_A_ACM,             //kMicrosoftADPCMFormat: Microsoft ADPCM-ACM code 2;kDVIIntelIMAFormat:DVI/Intel IMAADPCM-ACM code 17
	_A_DVA,             //kDVAudioFormat:DV Audio
	_A_QDM,             //kQDesignCompression:QDesign music
	_A_QCA,             //kQUALCOMMCompression:QUALCOMM PureVoice
	_A_G711,            //G.711 A-law logarithmic PCM and G.711 mu-law logarithmic PCM
	_A_NYM,             //Nellymoser
	_A_SPX,             //Speex
	_A_VORBIS,          //Ogg vorbis
	_A_APE,				//Monkey's audio
	_A_FLAC,
	_A_ALAC,
	_A_MLP,				//DVD-Audio MLP
	_A_CSF,
	_A_AIFF,			//AIFF
	_A_TTA,				//True Type Audio
	_A_VP7,
	_A_MPC,				//Musepack audio
	_A_TRUEHD,			//Dolby True HD
	_A_WVP,				//Wavpack
	_A_OTHER   = 0x100	//未知
};
#endif

enum CAPTURE_MODE
{
	E_VIDEOCARD,
	E_STREAMINGMEDIA,
	E_LOCALFILE
};

enum TUNER_MODE
{
	E_MODE_TV,
	E_MODE_FM
};

enum e_State
{
	e_Running,
	e_Pause,
	e_Stop
};

// Filter属性
enum e_Filter_System_Apply
{
	e_Apply_All,
	e_Apply_XP_Only,
	e_Apply_Vista_Only,//Vista+
};

// Filter所依赖组件信息
typedef struct _ComponentInfo
{
	_ComponentInfo():bReg(false)
	{

	}
	CString path;	// 组件路径
	bool	bReg;	// 是否需要注册
	CLSID	clsid;	// 组件ID
} COMPONENT_INFO;

// 媒体类型
typedef struct _MediaType
{
	CLSID major;
	CLSID sub;
	CLSID format;
	DWORD fcc;
	int   priority;//优先级
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




// 容器类型 -> Source & Splitter
typedef struct  _container_to_src
{
	e_CONTAINER			container;
	std::vector<CLSID>	vSource;
	std::vector<CLSID>	vSplitters;
} QVOD_CONTAINER_TO_SRC;

// 编码类型 -> Decoder 
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

// 加速解决方案属性
typedef struct QVOD_ACCEL_SOLUTION
{
	CString						solution_name;		// 方案名称
	CString						solution_comment;	// 方案描述

	std::vector<QVOD_CONTAINER_TO_SRC>	vContainers;		// 容器类型
	std::vector<QVOD_VIDEO_DECODER>		vVDecoders;		// 加速解码器
	std::vector<CLSID>					vRenders;		// 渲染器
} QVOD_ACCEL_SOLUTION;

// CheckByte、Suffix到Filters的映射
typedef struct _FilterMap
{
	e_CONTAINER					container;
	std::vector<CString>		vCheckBytes;	// CheckByte
	std::vector<CString>		vSuffix;		// 后缀名
	std::vector<CLSID>			vectFilters;	// 使用的Filters
} QVOD_FILTER_MAP;

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
	bool		bEnable;		//Filter可不可用
	CString		strName;		//Filter名称
	CString		path;			//Filter路径
	CString		wrapperpath;	//Wrapper path
	bool		bDMO;			//DMO Flag
	CLSID		clsid;			// CLSID
	CLSID		wrapid;			// wrap ID
	CLSID		catid;			// 分类ID
	//DWORD		version;		// 系统版本号
	e_Filter_System_Apply eSystem;// 系统限制
	std::vector<COMPONENT_INFO>		vComponents;	//Filter依赖组件
	std::vector<QVOD_MEDIA_TYPE>	vInTypes;	// 输入媒体类型
	//std::vector<QVOD_FILTER_DECODER_PRIORITY>	vDecoderPriority;
} QVOD_FILTER_INFO;

struct QVOD_FILTER_INFO_EX:public QVOD_FILTER_INFO
{
	QVOD_FILTER_INFO_EX()
	{
		__super::_QvodFilterInfo();
		mhFilterModule = NULL;
	}

	CComQIPtr<IBaseFilter>	pFilter;
	std::vector<HMODULE>	mModuleList;
	HMODULE					mhFilterModule;

	QVOD_FILTER_INFO_EX& operator=(const QVOD_FILTER_INFO& filter_info)
	{
		this->capacity = filter_info.capacity;
		this->bEnable = filter_info.bEnable;
		this->strName = filter_info.strName;
		this->path = filter_info.path;
		this->wrapperpath = filter_info.wrapperpath;
		this->bDMO = filter_info.bDMO;
		this->clsid = filter_info.clsid;
		this->wrapid = filter_info.wrapid;
		this->catid = filter_info.catid;
		this->eSystem = filter_info.eSystem;
		this->vComponents = filter_info.vComponents;
		this->vInTypes = filter_info.vInTypes;
		return *this;
	}
};

//#define SafeRelease(p) if(p){p->Release();}
//#define SafeDelete(pObject)		 if(pObject) {delete pObject; pObject = NULL;}
//#define SafeDeleteArray(p)		 if(p){delete []p; p = NULL;}

//const _QvodFilterInfo Filter_info[] = 
//{
//	{//0 xBar
//		true,
//		L"Analog Xbar",
//		L"%Windows%\\System32\\ksxbar.ax",
//		CLSID_Analog_Xbar,
//		NULL
//	},
//
//	{//1 
//		true,
//		L"Analog Video Capture",
//		L"%Windows%\\System32\\ksproxy.ax",
//		CLSID_Analog_VideoCapture,
//		NULL
//	},
//
//	{//2
//		true,
//		L"Analog Audio Capture",
//		L"%Windows%\\System32\\ksproxy.ax",
//		CLSID_Analog_AudioCapture,
//		NULL
//	},
//
//	{//3, Video Tuner
//		true,
//		L"Analog TV Tuner",
//		L"%Windows%\\System32\\kstvtune.ax",
//		CLSID_TVTunerFilter,
//		NULL
//	},
//
//	{//4 Audio tuner
//		true,
//		L"Analog TV Audio Tuner",
//		L"%Windows%\\System32\\ksxbar.ax",
//		CLSID_TVAudioFilter,
//		NULL
//	},
//
//	{//5
//		true,
//		L"H264 Encoder",
//		L"QVODH264EncoderFilter.ax",
//		CLSID_H264Encoder,
//		NULL
//	},
//
//	{//6
//		true,
//		L"AAC Encoder",
//		L"mmaaced.ax",
//		CLSID_AAC_Encoder,
//		NULL
//	},
//
//	{//7
//		true,
//		L"Qvod Post Video",
//		L"QvodPostVideo.ax",
//		CLSID_Post_Video,
//		NULL
//	},
//
//	{//8
//		true,
//		L"MPEG-2 Muxer",
//		L"MPEG-2 Muxer.ax",
//		CLSID_MPEG2_MUXER,
//		NULL
//	},
//
//	{//9
//		true,
//		L"Dump",
//		L"dump.ax",
//		CLSID_DUMP,
//		NULL
//	}
//};
enum e_MEDIA_TYPE
{
	e_Video,
	e_Audio,
	e_Subtitle,
	e_Unknown
};

enum e_DEVICE_TYPE
{
	e_VideoCapture,
	e_AudioCapture,
	e_CrossBar,
	e_VTuner,
	e_ATuner
};
struct VideoCodec
{
	DWORD nfourcc;
	DWORD nBitrate;
	DWORD nFrameRate;
	DWORD nWidth;
	DWORD nHeight;
	//DWORD nGop;
	RECT Cutrect;
};

struct AudioCodec
{
	DWORD nfourcc;
	DWORD nBitrate;
	DWORD nSampleRate;
	DWORD nchannel;
};

struct PLAY_LIST_ITEM
{
	CString Url;
	CString SubUrl;
};

struct PLAY_LIST_ITEM2
{
	wchar_t Url[MAX_PATH];
	wchar_t SubUrl[MAX_PATH];
	PLAY_LIST_ITEM2()
	{
		Reset();
	}
	void Reset()
	{
		memset(Url, 0, sizeof(wchar_t)*MAX_PATH);
		memset(SubUrl, 0, sizeof(wchar_t)*MAX_PATH);
	}
};

struct TextLogoConfig
{
	bool bLogoEnable;
	wchar_t path[MAX_PATH];
	struct PLACE
	{
		int place;
		int x;
		int y;
	} place;

	struct ROLL
	{
		bool broll;
		int speed;
	} roll;

	LOGFONT lf;
	DWORD fontcolor;

	DWORD olcr;
	TextLogoConfig()
	{
		bLogoEnable = false;
		memset(path, 0, sizeof(wchar_t)*MAX_PATH);
		place.place = 0;
		place.x = 0;
		place.y = 0;
		roll.broll = false;
		roll.speed = 0;
		memset(&lf, 0, sizeof(lf));
		olcr = 0;
	}
};

struct CAPTURE_ITEM
{
	GUID Categoryguid;
	CString devicename;	
};

typedef LONG    (WINAPI   *pfnDllGetClassObject)(REFCLSID , REFIID, LPVOID *);
typedef long (*pfnRegister)(void);

#define BeginEnumPins(pBaseFilter, pEnumPins, pPin) \
{CComPtr<IEnumPins> pEnumPins; \
	if(pBaseFilter && SUCCEEDED(pBaseFilter->EnumPins(&pEnumPins))) \
{ \
	for(CComPtr<IPin> pPin; S_OK == pEnumPins->Next(1, &pPin, 0); pPin = NULL) \
{ \

#define EndEnumPins }}}

#define BeginEnumMediaTypes(pPin, pEnumMediaTypes, pMediaType) \
{CComPtr<IEnumMediaTypes> pEnumMediaTypes; \
	if(pPin && SUCCEEDED(pPin->EnumMediaTypes(&pEnumMediaTypes))) \
{ \
	AM_MEDIA_TYPE* pMediaType = NULL; \
	for(; S_OK == pEnumMediaTypes->Next(1, &pMediaType, NULL); DeleteMediaType(pMediaType), pMediaType = NULL) \
{ \

#define EndEnumMediaTypes(pMediaType) } if(pMediaType) DeleteMediaType(pMediaType); }}

#define BeginEnumSysDev(clsid, pMoniker) \
{CComPtr<ICreateDevEnum> pDevEnum4$##clsid; \
	pDevEnum4$##clsid.CoCreateInstance(CLSID_SystemDeviceEnum); \
	CComPtr<IEnumMoniker> pClassEnum4$##clsid; \
	if(SUCCEEDED(pDevEnum4$##clsid->CreateClassEnumerator(clsid, &pClassEnum4$##clsid, 0)) \
	&& pClassEnum4$##clsid) \
{ \
	pClassEnum4$##clsid->Reset(); \
	for(CComPtr<IMoniker> pMoniker; pClassEnum4$##clsid->Next(1, &pMoniker, 0) == S_OK; pMoniker = NULL) \
{ \

#define EndEnumSysDev }}}
