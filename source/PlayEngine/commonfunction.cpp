//#include "../include/IQvodPlayMedia.h"
#include "commonfunction.h"
#include <vector>
#include "Commonstructure.h"
#include "GraphStructure.h"
#include "LazyLock.h"

// CString to clsid
CLSID CStringToClsid(CString strClsid)
{
	if (strClsid.IsEmpty())
	{
		return CLSID_NULL;
	}
	CLSID clsid;
	CLSIDFromString(strClsid.GetBuffer(strClsid.GetLength()),&clsid);
	return clsid;
}

CString CLSID2String(const CLSID& clsid)
{
	LPOLESTR str;
	StringFromCLSID(clsid,&str);
	CString strClsid = str;
	CoTaskMemFree(str);

	return strClsid;
}

/*************************************************************************************
Function:    CStringtoBin
Description: 将十六进制的字符串数字转化为整数
Input   CString str 十六进制的字符串数字
Output   pdata 返回转化后的数字
Return:    无
Other:
**************************************************************************************/
void CStringtoBin(CString str, BYTE *pdata)
{
	BYTE b = 0;
	str.MakeUpper();
	for(int i = 0, j = str.GetLength(); i < j; i++)
	{
		TCHAR c = str[i];
		if(c >= '0' && c <= '9') 
		{
			if(!(i&1)) b = ((char(c-'0')<<4)&0xf0)|(b&0x0f);
			else b = (char(c-'0')&0x0f)|(b&0xf0);
		}
		else if(c >= 'A' && c <= 'F')
		{
			if(!(i&1)) b = ((char(c-'A'+10)<<4)&0xf0)|(b&0x0f);
			else b = (char(c-'A'+10)&0x0f)|(b&0xf0);
		}
		else break;

		if(i&1)
		{
			//data[i>>1] = b;
			pdata[i>>1] = b;
			b = 0;
		}
	}
}

e_CONTAINER ContainerFromString(CString str)
{
	str.MakeUpper();

	// 视频
	if (str.Find(L"RM") > -1 || str.Find(L"REALMEDIA") > -1)
		return _E_CON_RMVB;

	if (str.Find(L"AVI") > -1)
		return _E_CON_AVI;

	if (str.Find(L"MKV") > -1 || str.Find(L"MKA") > -1 || str.Find(L"MATROSKA") > -1)
		return _E_CON_MKV;
	
	//MPEG-Visual ES流
	if (str.Find(L"MPEG-4 ES") > -1)
		return _E_CON_MPEG4ES;

	// Apple系 MOV,QT
	if (str.Find(L"QT") > -1 || str.Find(L"QUICKTIME") > -1 || str.Find(L"MOV") > -1)
		return _E_CON_MOV;
	
	// MPEG4 Part 14标志
	if (str.Find(L"3GP") > -1)
		return _E_CON_3GP;

	if (str.Find(L"K3G") > -1)
		return _E_CON_MP4;

	if (str.Find(L"MP4") > -1 || str.Find(L"MPEG4") > -1 || str.Find(L"MPEG-4") > -1)
		return _E_CON_MP4;
	
	if (str.Find(L"DAT") > -1 || (str.Find(L"CDXA") > -1 && str.Find(L"MPEG-PS") > -1))
		return _E_CON_DAT;

	if (str.Find(L"MPEG2-TS") > -1 || str.Find(L"MPEG-TS") > -1)
		return _E_CON_MPEGTS;

	if (str.Find(L"MPEG2-PS") > -1 || str.Find(L"MPEG-PS") > -1 )
		return _E_CON_MPEGPS;

	if (str.Find(L"MPEG VIDEO") > -1)
		return _E_CON_MPEGPS;

	if (str == (L"MPEG"))
		return _E_CON_MPEG;
	if (str.Find(L"HKMPG") > -1)
		return _E_CON_HKMPG;
	if (str.Find(L"DIVX") > -1)
		return _E_CON_DIVX;
	if (str.Find(L"WM") > -1 || str.Find(L"WINDOWS MEDIA")> -1)
		return _E_CON_WM;

	if (str.Find(L"M2TS") > -1 || str.Find(L"BDAV") > -1 || str.Find(L"BLU-RAY") > -1)
		return _E_CON_M2TS;

	if(str.Find(L"QMVB") > -1)
	{
		return _E_CON_QMVB;
	}

	if (str.Find(L"QMV") > -1)
		return _E_CON_QMV;

	if (str.Find(L"VP5") > -1)
		return _E_CON_VP5;

	if (str.Find(L"VP6") > -1)
		return _E_CON_VP6;

	if (str.Find(L"VP7") > -1)
		return _E_CON_VP7;

	if (str.Find(L"WEBM") > -1)
		return _E_CON_WEBM;

	if (str.Find(L"FLV") > -1 || str.Find(L"FLASH VIDEO") > -1)
		return _E_CON_FLV;

	if (str.Find(L"AMV") > -1)
		return _E_CON_AMV;
	if (str.Find(L"PMP") > -1)
		return _E_CON_PMP;
	if (str.Find(L"BIK") > -1)
		return _E_CON_BIK;
	if (str.Find(L"CSF") > -1)
		return _E_CON_CSF;

	if (str.Find(L"XLMV") > -1)
		return _E_CON_XLMV;
	if (str.Find(L"SWF") > -1)
		return _E_CON_SWF;
	if (str.Find(L"IVF") > -1)
		return _E_CON_IVF;
	if (str.Find(L"WVP") > -1)
		return _E_CON_WVP;

	if (str.Find(L"DVD") > -1)
		return _E_CON_DVD;

	if (str.Find(L"DAT") > -1)
		return _E_CON_DAT;

	// 音频
	if (str.Find(L"AMR") > -1)
		return _E_CON_AMR;
	if (str.Find(L"AWB") > -1)
		return _E_CON_AWB;

	if (str.Find(L"DC") > -1)
		return _E_CON_DC;
	if (str.Find(L"DSM") > -1)
		return _E_CON_DSM;

	if (str.Find(L"MP3") > -1 || str.Find(L"MPEG AUDIO") > -1)
		return _E_CON_MPA;
	if (str.Find(L"MP2") > -1)
		return _E_CON_MPA;
	if (str.Find(L"MPA") > -1)
		return _E_CON_MPA;

	if (str.Find(L"OGG") > -1)
		return _E_CON_OGG;
	if (str.Find(L"DTS") > -1)
		return _E_CON_DTS;
	if (str.Find(L"MIDI") > -1)
		return _E_CON_MIDI;

	if (str.Find(L"AAC") > -1)
		return _E_CON_AAC;

	if (str.Find(L"AC3") > -1 || str.Find(L"AC-3") > -1)
		return _E_CON_AC3;

	if (str.Find(L"AIF") > -1 || str.Find(L"AIFF") > -1)
		return _E_CON_AIFF;

	if (str.Find(L"ALAW") > -1)
		return _E_CON_ALAW;

	if (str.Find(L"APE") > -1 || str.Find(L"MONKEY") > -1)
		return _E_CON_APE;

	if (str.Find(L"AU") > -1)
		return _E_CON_AU;

	if (str.Find(L"CDA") > -1)
		return _E_CON_CDA;

	if (str.Find(L"CDXA") > -1)
		return _E_CON_CDXA;

	if (str.Find(L"TTA") > -1)
		return _E_CON_TTA;

	if (str.Find(L"MPC") > -1)
		return _E_CON_MPC;

	if (str.Find(L"FLAC") > -1)
		return _E_CON_FLAC;

	if (str.Find(L"MOD") > -1)
		return _E_CON_MOD;

	if (str.Find(L"RA") > -1)
		return _E_CON_RA;

	if (str.Find(L"WAVPACK") > -1)
		return _E_CON_WVP;

	if (str.Find(L"WAV") > -1)
		return _E_CON_WAV;

	if (str.Find(L"DV") > -1)
		return _E_CON_DV;

	if (str.Find(L"MTV") > -1)
		return _E_CON_MTV;

	if (str.Find(L"MXF") > -1)
		return _E_CON_MXF;

	if (str.Find(L"SKM") > -1)
		return _E_CON_SKM;

	if (str.Find(L"AOB") > -1)
		return _E_CON_AOB;

	if (str.Find(L"IVM") > -1)
		return _E_CON_IVM;

	if (str.Find(L"HKMPG") > -1)
		return _E_CON_HKMPG;

	if (str.Find(L"SCM") > -1)
		return _E_CON_SCM;

	if (str.Find(L"MUSEPACK") > -1 || str.Find(L"MPC") > -1)
		return _E_CON_MPC;
	
	return _E_CON_OTHER;
}

e_V_ENCODER VideoEncoderFromString(CString str)
{
	str.MakeUpper();
	if (str.Find(L"AVC") > -1)
		return _V_H264;
	if (str.Find(L"H264") > -1)
		return _V_H264;
	if (str.Find(L"H263") > -1 || str.Find(L"H.263") > -1)
		return _V_H263;
	if (str.Find(L"RV4") > -1 || str.Find(L"REALVIDEO 4") > -1)
		return _V_RV40;
	if (str.Find(L"RV3") > -1 || str.Find(L"REALVIDEO 3") > -1)
		return _V_RV30;
	if (str.Find(L"RV2") > -1 || str.Find(L"REALVIDEO 2") > -1)
		return _V_RV20;
	if (str.Find(L"RV1") > -1 || str.Find(L"REALVIDEO 1") > -1)
		return _V_RV10;

	if (str.Find(L"MPEG") > -1 && str.Find(L"V3") > -1 || str.Find(L"MP43") > -1)
		return _V_WM_MPEG4_V3;
	if (str.Find(L"MPEG") > -1 && str.Find(L"V2") > -1 || str.Find(L"MP42") > -1)
		return _V_WM_MPEG4_V2;
	if (str.Find(L"MPEG") > -1 && str.Find(L"V1") > -1 || str.Find(L"MP4S") > -1)
		return _V_WM_MPEG4_V1;

	if (str.Find(L"MPEG-4") > -1)//if (str.Find(L"MPEG-4 ES") > -1)
		return _V_MPEG_4_Visual;

	if (str.Find(L"MPEG1") > -1 || str.Find(L"MPEG VIDEO VERSION 1") > -1)
		return _V_MPEG_1;

	if (str.Find(L"MPEG2") > -1 || str.Find(L"MPEG VIDEO VERSION 2") > -1)
		return _V_MPEG_2;

	if (str.Find(L"MPEG VIDEO") > -1)
		return _V_MPEG_2;

	if (str.Find(L"WMV1") > -1)
		return _V_WMV7;

	if (str.Find(L"WMV2") > -1)
		return _V_WMV8;

	if (str.Find(L"WMV3") > -1 || str.Find(L"MSS2") > -1)//windows media video 9 screen
		return _V_WMV9;

	if (str.Find(L"VC1") > -1 || str.Find(L"VC-1") > -1)
		return _V_WVC1;

	if (str.Find(L"MS VIDEO") > -1)
		return _V_MSVIDEO1;

	if (str.Find(L"XVID") > -1)
		return _V_XVID;

	if (str.Find(L"DIVX") > -1)
		return _V_DIVX;

	if (str.Find(L"RV10") > -1)
		return _V_RV10;

	if (str.Find(L"H263+") > -1)
		return _V_H263_PLUS;

	if (str.Find(L"H261") > -1)
		return _V_H261;

	if (str.Find(L"JPEG") > -1)
		return _V_JPEG;

	if (str.Find(L"MJPG") > -1)
		return _V_JPEG;

	if (str.Find(L"LJPG") > -1)
		return _V_LJPG;

	if (str.Find(L"VP5") > -1)
		return _V_VP5;
	if (str.Find(L"VP6") > -1)
		return _V_VP6;
	if (str.Find(L"VP7") > -1)
		return _V_VP7;
	if (str.Find(L"VP8") > -1)
		return _V_VP8;
	if (str.Find(L"HFYU") > -1)
		return _V_HFYU;
	
	if (str.Find(L"PNG") > -1)
		return _V_PNG;

	if (str.Find(L"IV31") > -1)
		return _V_IV31;
	if (str.Find(L"IV32") > -1)
		return _V_IV32;

	if (str.Find(L"ZLIB") > -1)
		return _V_ZLIB;
	if (str.Find(L"TSCC") > -1)
		return _V_TSCC;
	if (str.Find(L"CVID") > -1)
		return _V_CVID;

	if (str.Find(L"RLE") > -1)
		return _V_RLE;
	if (str.Find(L"RAW") > -1)
		return _V_RAW;

	if (str.Find(L"SMC") > -1)
		return _V_RAW;
	if (str.Find(L"APPV") > -1)
		return _V_APPV;
	if (str.Find(L"SCRN") > -1)
		return _V_SCRN;
	if (str.Find(L"THEO") > -1)
		return _V_THEORA;
	if (str.Find(L"YV12") > -1)
		return _V_YV12;

	if (str.Find(L"SORENSON") > -1)
		return _V_SORV;

	if (str.Find(L"ZMP5") > -1)
		return _V_ZMP5;

	// 未压缩
	if (str.Find(L"YUV") > -1)
		return _V_YUV;

	if (str.Find(L"UYVY") > -1)
		return _V_UYVY;

	if(str.Find(L"RGB") > -1)
	{
		return _V_RGB;
	}

	return _V_OTHER;
}

e_A_ENCODER AudioEncoderFromString(CString str)
{
	str.MakeUpper();

	if (str.Find(L"AAC") > -1)
		return _A_AAC;

	if (str.Find(L"E-AC-3") > -1 || str.Find(L"E-AC3") > -1)
		return _A_E_AC3;

	if (str.Find(L"AC3") > -1 || str.Find(L"AC-3") > -1)
		return _A_AC3;

	if (str.Find(L"MP2") > -1 || (str.Find(L"MPEG AUDIO") > -1 && str.Find(L"LAYER 2") > -1) )
		return _A_MP2;

	if (str.Find(L"MP3") > -1 || (str.Find(L"MPEG AUDIO") > -1 && str.Find(L"LAYER 3") > -1 ))
		return _A_MP3;

	if (str.Find(L"MP4") > -1)
		return _A_MP4;

	if (str.Find(L"AMR-NB") > -1 || str.Find(L"AMR") > -1)
		return _A_AMR_NB;
	if (str.Find(L"AMR-WB") > -1)
		return _A_AMR_WB;

	if (str.Find(L"DTS") > -1)
		return _A_DTS;
	if (str.Find(L"OGG") > -1 )
		return 	_A_VORBIS;
	
	if (str.Find(L"ADPCM") > -1)
		return _A_ADPCM;
	if (str.Find(L"APCM") > -1)
		return _A_APCM;
	if (str.Find(L"DPCM") > -1)
		return _A_DPCM;
	if (str.Find(L"PCM-LE") > -1 )
		return _A_PCM_LE;
	if (str.Find(L"PCM-BE") > -1 || str.Find(L"PCM TWOS") > -1)
		return _A_PCM_BE;

	if (str.Find(L"PCM") > -1)
		return _A_PCM;

	if (str.Find(L"COOK") > -1)
		return _A_COOK;

	if (str.Find(L"SIPR") > -1)
		return _A_SIPR;

	if (str.Find(L"TSCC") > -1 || str.Find(L"TECHSMITH") > -1)
		return _A_TSCC;

	if (str.Find(L"WMA7") > -1)
		return _A_WMA7;

	if (str.Find(L"WMA8") > -1)
		return _A_WMA8;

	if (str.Find(L"WMA9") > -1)
		return _A_WMA9;

	if (str.Find(L"WMA2") > -1 || (str.Find(L"WMA") > -1 && str.Find(L"VERSION 2") > -1))
		return _A_WMA2;

	if (str.Find(L"WMA3") > -1 || (str.Find(L"WMA") > -1 && str.Find(L"VERSION 3") > -1))
		return _A_WMA3;
	
	if (str.Find(L"WMA") > -1)
		return _A_WMA2;

	if (str.Find(L"RAW") > -1)
		return _A_RAW;

	if (str.Find(L"MAC") > -1)
		return _A_MAC;

	if (str.Find(L"IMA") > -1)
		return _A_IMA;

	if (str.Find(L"LAW") > -1)
		return _A_LAW;

	if (str.Find(L"ACM") > -1)
		return _A_ACM;

	if (str.Find(L"DVA") > -1)
		return _A_DVA;

	if (str.Find(L"QDM") > -1 || str.Find(L"QDESIGN") > -1)
		return _A_QDM;

	if (str.Find(L"QCA") > -1)
		return _A_QCA;

	if (str.Find(L"G711") > -1)
		return _A_WMA9;

	if (str.Find(L"NYM") > -1)
		return _A_NYM;

	if (str.Find(L"SPX") > -1)
		return _A_SPX;

	if (str.Find(L"MIDI") > -1)
		return _A_MIDI;
	
	if (str.Find(L"MLP") > -1)
		return _A_MLP;

	if (str.Find(L"APE") > -1 || str.Find(L"MONKEY") > -1)
		return _A_APE;

	if (str.Find(L"FLAC") > -1)
		return _A_FLAC;

	if (str.Find(L"QCELP") > -1)
		return _A_QCELP;

	if (str.Find(L"TTA") > -1)
		return _A_TTA;
	
	if (str.Find(L"AIFF") > -1)
		return _A_AIFF;

	if (str.Find(L"VORBIS") > -1 || str.Find(L"OGG") > -1)
		return _A_VORBIS;
	
	if (str.Find(L"VP7") > -1)
		return _A_VP7;

	if (str.Find(L"ALAC") > -1)
		return _A_ALAC;

	if (str.Find(L"MUSEPACK") > -1 || str.Find(L"MPC") > -1)
		return _A_MPC;
	
	if (str.Find(L"TRUEHD") > -1)
		return _A_TRUEHD;

	if (str.Find(L"WAVPACK") > -1)
		return _A_WVP;
	return _A_OTHER;
}

// 检查CheckByte是否出现在pDatas中
// 返回
bool CheckCB(const TCHAR * pchkbytes, const BYTE* pDatas, int dataSize, int& needMoreData)
{
	if (!pchkbytes || !pDatas)
	{
		return false;
	}

	needMoreData = 0;

	std::vector<CString> vcheckbytes;
	int nlen = _tcslen(pchkbytes);
	const TCHAR *pc = pchkbytes;
	CString strtemp;
	while(nlen-- > 0)
	{
		if(*pc == ',')
		{
			if(strtemp.CompareNoCase(L",") == 0)
			{
				strtemp.Empty();
			}
			vcheckbytes.push_back(strtemp);
			strtemp.Empty();
		}
		else
		{
			strtemp += (*pc);
		}
		pc++;
	}
	vcheckbytes.push_back(strtemp);
	if(vcheckbytes.size() < 4)
	{	
		return false;
	}
	for (int i = 0; i < vcheckbytes.size(); i += 4)
	{
		CString offsetstr = vcheckbytes[i];
		CString cbstr = vcheckbytes[i+1];
		CString maskstr = vcheckbytes[i+2];
		CString valstr = vcheckbytes[i+3];
		long cb = _ttol(cbstr);

		if(offsetstr.IsEmpty() || cbstr.IsEmpty() 
			|| valstr.IsEmpty() || (valstr.GetLength() & 1)
			|| cb*2 != valstr.GetLength())
			return false;

		int pos = int(pDatas) + (int)_ttoi64(offsetstr);
		if (pos >  (int)pDatas + dataSize - valstr.GetLength()/2)
		{
			// 数据不够，要求更多数据
			needMoreData = (int)_ttoi64(offsetstr) + 64;
			return false;
		}

		// LAME
		while(maskstr.GetLength() < valstr.GetLength())
			maskstr += 'F';
		valstr.TrimLeft();
		valstr.TrimRight();
		BYTE *pmask = new BYTE[maskstr.GetLength()/2];
		BYTE *pval  = new BYTE[valstr.GetLength()/2];
		int nvallen = valstr.GetLength()/2;
		CStringtoBin(maskstr, pmask);
		CStringtoBin(valstr, pval);

		for(size_t i = 0; i < nvallen; i++, pos++)
		{
			BYTE b = *((BYTE*)pos);
			if( (b & pmask[i]) != pval[i])
			{
				SafeDeleteArray(pmask);
				SafeDeleteArray(pval);
				return false;
			}
		}
		SafeDeleteArray(pmask);
		SafeDeleteArray(pval);
	}
	return true;
}

bool ReadByte(const wchar_t * pPath, BYTE* pDatas, int dataSize, __int64 startpos)
{
	if(pPath == NULL || pDatas == NULL)
	{
		return false;
	}

	FILE* pfile = _wfopen(pPath, L"rb");
	if(pfile)
	{
		fseek(pfile, startpos, SEEK_SET);
		int len = fread(pDatas, 1, dataSize, pfile);
		fclose(pfile);
		return len >= dataSize ? true : false;
	}
	return false;
}