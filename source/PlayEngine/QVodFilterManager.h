#pragma once

/*******************************************************************************
Copyright (C), 1988-1999, QVOD Technology Co.,Ltd.
File name:      QvodFilterManager.h
Author:       zhongweiyuan
Version:      1.0.0
Date:2011-3-7 
Description:  ����ʵ��XML��ȡ������������Ⱦ��·�����Filter��Ϣ
Others:        
Function List:  GetFilterByClsid��
History:        
1. Date:
Author:
Modification:
********************************************************************************/

#include "GraphStructure.h"
#include <map>
#include <vector>

//<!-- Filter Capacity Bitλ˵�� -->
//<!-- ��һλ ��Filter�߱���ȡ������������SYS FileReader��QVodSource,DCBass Source, MIDI Parser -->
//<!-- �ڶ�λ ��Filter�߱���������������Splitter)-->
//<!-- ����λ ��Filter�߱���Ƶ����������Video Decoder�� -->
//<!-- ����λ ��Filter�߱���Ƶ����������Audio Decoder�� -->
//<!-- ����λ ��Filter�߱�Video Trans��������QvodPostVideo��QvodSubTitle,������ĻFilter�� -->
//<!-- ����λ ��Filter�߱�Audio Trans��������Qvod Sound Filter -->
//<!-- ����λ ��Filter�߱�Video Render��������CSF Render, FFMPEG Filter -->
//<!-- �ڰ�λ ��Filter�߱�Audio Render��������CSF Render, FFMPEG Filter -->
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
#define E_FILTER_CAPACITY_AUDIO_MUX		0x400
#define E_FILTER_CAPACITY_AUDIO_REND	0x800

/*
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
*/
class TiXmlElement;

class CXMLElement;
// Filter������
class QvodFilterManager
{

protected:
	typedef struct _BestGraph
	{
		e_CONTAINER					container;		// ��������
		e_V_ENCODER					vencoder;		// ��Ƶ��������
		e_A_ENCODER					aencoder;		// ��Ƶ��������

		std::vector<CLSID>			vectFilters;	// ���ŷ���

	} INNER_BEST_GRAPH;

public:
	// ��ȡQvodFilterManagerʵ��
	static QvodFilterManager& GetInstance();
	
	bool	LoadXMLs(CString path);
	
	// ��ȡĬ�ϼ��ٷ���
	bool	GetAccelSolution_Default(QVOD_ACCEL_SOLUTION& solution) const;
	
	// ��ȡ���õ����м��ٷ���
	bool	GetAccelSolutionList(std::vector<QVOD_ACCEL_SOLUTION>& solutions) const;
	
	// ��ȡָ�����ٷ���
	bool	GetAccelSolutionByName(const CString& name,QVOD_ACCEL_SOLUTION& solution) const;

	// ��ȡָ���������͡�ָ��capacity��Filter
	void	GetFilterByMediaType(DWORD capacity, QVOD_MEDIA_TYPE& type, std::vector<CLSID>&) const;
	
	// ��ȡָ��capacity��Filter
	void	GetFilterByCapacity(DWORD capacity, std::vector<CLSID>& vectClsid) const;
	
	// ��ȡFilter�Ľ������ȼ�
	DWORD	GetFilterPriority(const CLSID& clsid, QVOD_MEDIA_TYPE& mt) const;

	// ��ȡFilter
	bool	GetFilterByCLSID(const CLSID& clsid, QVOD_FILTER_INFO& node) const;
	
	// ��ȡFilter Capacity
	DWORD	GetFilterCapacity(const CLSID& clsid) const;

	// �����������͡���Ƶ�������͡���Ƶ�������ͻ�ȡ������·
	bool	GetBestGraph(e_CONTAINER c, e_V_ENCODER ve, e_A_ENCODER ae, std::vector<CLSID>& filters) const;

	// ����CheckByte��Suffix��Filter��ӳ���
	const std::vector<QVOD_FILTER_MAP>& GetFilterMaps() const;

// 	// ����һ��ԭʼ�ֽ����ݣ���ȡ֧�ֵ�Source Filter
// 	bool	GetSourceFilterByCheckByte(BYTE* raw_data, CLSID& source) const;
// 
// 	// ����һ��ԭʼ�ֽ����ݣ���ȡ֧�ֵ�Splitter Filter
// 	bool	GetSplitterFilterByCheckByte(BYTE* raw_data, CLSID& splitter) const;
// 
// 	// ����һ����׺������ȡ�ܽ����˺�׺�ļ���Source Filter
// 	bool	GetSourceFilterBySuffix(CString suffix, CLSID& source) const;
// 
// 	// ����һ����׺������ȡ�ܽ����˺�׺�ļ���Splitter Filter
// 	bool	GetSplitterFilterBySuffix(CString suffix, CLSID& splitter) const;
private:
	QvodFilterManager(void);
	
	bool		LoadDataFromXML(const CString& path);	// ��XML�ж�ȡ
	bool		ReadFromBin();							// ����Դ�ļ�������������
	bool		ParserXML(BYTE* src);					// ����XML
	bool		LoadXML_Filters(CXMLElement* );
	bool		LoadXML_BestGraph(CXMLElement*);
	bool		LoadXML_AccelSolution(CXMLElement*);
	bool		LoadXML_FilterMap(CXMLElement*);
private:
	std::vector<QVOD_FILTER_INFO>		mFilters;
	std::vector<QVOD_ACCEL_SOLUTION>	mAccelSolutions;// ����������
	std::vector<QVOD_FILTER_MAP>		mFilterMaps;	// CheckByte/Suffix -> Filter CLSID
	std::vector<INNER_BEST_GRAPH>		mBestGraphs;	// ������·��
	bool								mbLoaded;
};
