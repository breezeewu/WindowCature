#pragma once

//�����ļ������� DS SWF
enum e_FileType
{
	_FT_Unknow = 0,
	_FT_LocalFile,
	_FT_DvdFile,
	_FT_ImageFile,
	_FT_TS_STREAM,
	_FT_Equipment,
	_FT_SwfFile   = 0x10,
	_FT_SwfF4v

};
//ý���ļ�����
enum e_CONTAINER
{
	_E_CON_UNKNOW = 0,

	_E_CON_RMVB = 1,
	_E_CON_AVI,
	_E_CON_MKV,
	_E_CON_MP4,		//MPEG-4 Part 14,3GPϵ
	_E_CON_3GP,		//3GP

	_E_CON_MOV,		//Appleϵ ����MOV��QT
	//_E_CON_QT,
	
	_E_CON_MPEG,	//MPEG1/2/4 ��׼������ʽ
	_E_CON_DAT,
	_E_CON_MPEG4ES,	//MPEG4������
	_E_CON_MPEGTS,	//TS
	_E_CON_MPEGPS,	//VOB EVO HD-DVD
	_E_CON_HKMPG,	//Hikvision mp4
	_E_CON_DIVX,	//Divx Media Format(DMF)
	_E_CON_WM,		//ASF/WMV/WMA/WMP
	_E_CON_TS,		//TS
	_E_CON_M2TS,	//M2TS/MTS,֧��DBAV �� AVCHD
	_E_CON_DVD,		//DVD�̣����⴦��DVD������

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

	_E_CON_XLMV,//Ѹ�׸�ʽ

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
	_E_CON_QMVB,
	_E_CON_OTHER = 0x100
};

//��Ƶ��������
enum e_V_ENCODER
{
	/*MPEG ϵ��*/
	_V_MPEG_1,
	_V_MPEG_2,

	_V_MPEG_4_Visual,//Simple
	_V_MPEG_4_Advanced_Simple,//Advanced Simple

	_V_XVID,
	_V_DIVX,
	//_V_AVC,//AVC1/H264/X264

	/*ITU ϵ��*/
	_V_H261,
	_V_H263,
	_V_H263_PLUS,
	_V_H264,

	/*Real Mediaϵ��*/
	_V_RV10,
	_V_RV20,
	_V_RV30,
	_V_RV40,			

/*Windows ϵ��*/
// 	Windows Media Video v7	WMV1	0	
// 	Microsoft MPEG-4 Video Codec v3	MP43	1	
// 	Windows Media Video v8	WMV2	2	
// 	Microsoft MPEG-4 Video Codec v2	MP42	3	
// 	Microsoft ISO MPEG-4 Video Codec v1	MP4S	4	
// 	Windows Media Video v9	WMV3	5	
// 	Windows Media Video v9 Advanced Profile	WMVA	6	deprecated as not VC-1����ȫ���ݡ�
// 	Windows Media Video v9 Advanced Profile	WVC1	7[��Դ����]	VC-1����֧��

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

	/*����*/
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
	_V_OTHER	= 0x100	//δ֪
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
	_A_PCM,				//����������
	_A_DPCM,			//�������������
	_A_APCM,			//����Ӧ����������
	_A_ADPCM,			//����Ӧ�������������
	_A_PCM_LE,			//PCM Little Endian
	_A_PCM_BE,			//PCM Big	 Endian

	_A_COOK,			//rmvb AUDIO
	_A_SIPR,			//rmvb AUDIO
	_A_TSCC,			//tscc

	_A_QCELP,			//Qualcomm��˾��1993������˿ɱ����ʵ�CELP������ΪQCELP,QCELP ����ѹ�������㷨����8K��13K����

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
	_A_OTHER   = 0x100	//δ֪
};

//����ѡ��
enum e_Play_Option
{
	E_INTELLIGENCE, //����ģʽ
	E_DXVA_ACCEL,   //��������
	E_VIDEO_POST    //��������
};

struct PlayOption_Param
{
	DWORD std_min_height;//������С�߶ȣ����أ���Ĭ��ֵ480p
	DWORD std_max_height;//�������߶ȣ����أ���Ĭ��ֵ720p
	DWORD std_min_bitrate;//������С���ʣ�bps����Ĭ��ֵ450000bps
	DWORD low_min_bitrate;//������С���ʣ�bps����Ĭ��ֵ300000bps
};
//�����ļ���Э��
enum e_FileProtocol
{
	_FP_Unknow = 0,
	_FP_LocalFile = 1,
	_FP_P2PFile,
	_FP_Torrent,
	_FP_HttpFile,
	_FP_FtpFile,
	_FP_TSFile,
	_FP_RtspFile,
	_FP_MmsFile,
	_FP_TSSTREAM
};
//�ļ�����״̬
enum e_PlayState
{
	_PS_Init = 0,
	_PS_Stop,
	_PS_Pause,
	_PS_Playing,
	_PS_Buffering,
	_PS_Waiting,
	_PS_MediaEnded,
	_PS_Ready,
	_PS_Conning,
	_PS_BTDowning,
	_PS_CreateDS,
	_PS_Stoping,
	_PS_PlayAd,
	_PS_PlayLocalAd,
	_PS_PlayPauseAd,
	_PS_PlayBufferingAd,
	_PS_Recording
};
