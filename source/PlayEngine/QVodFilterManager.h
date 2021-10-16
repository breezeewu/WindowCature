#pragma once

/*******************************************************************************
Copyright (C), 1988-1999, QVOD Technology Co.,Ltd.
File name:      QvodFilterManager.h
Author:       zhongweiyuan
Version:      1.0.0
Date:2011-3-7 
Description:  本类实现XML读取并解析构造渲染链路所需的Filter信息
Others:        
Function List:  GetFilterByClsid等
History:        
1. Date:
Author:
Modification:
********************************************************************************/

#include "GraphStructure.h"
#include <map>
#include <vector>

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
#define E_FILTER_CAPACITY_AUDIO_MUX		0x400
#define E_FILTER_CAPACITY_AUDIO_REND	0x800

/*
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

// Filter属性
enum e_Filter_System_Apply
{
	e_Apply_All,
	e_Apply_XP_Only,
	e_Apply_Vista_Only,//Vista+
};

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
*/
class TiXmlElement;

class CXMLElement;
// Filter管理类
class QvodFilterManager
{

protected:
	typedef struct _BestGraph
	{
		e_CONTAINER					container;		// 容器类型
		e_V_ENCODER					vencoder;		// 视频编码类型
		e_A_ENCODER					aencoder;		// 音频编码类型

		std::vector<CLSID>			vectFilters;	// 最优方案

	} INNER_BEST_GRAPH;

public:
	// 获取QvodFilterManager实例
	static QvodFilterManager& GetInstance();
	
	bool	LoadXMLs(CString path);
	
	// 获取默认加速方案
	bool	GetAccelSolution_Default(QVOD_ACCEL_SOLUTION& solution) const;
	
	// 获取可用的所有加速方案
	bool	GetAccelSolutionList(std::vector<QVOD_ACCEL_SOLUTION>& solutions) const;
	
	// 获取指定加速方案
	bool	GetAccelSolutionByName(const CString& name,QVOD_ACCEL_SOLUTION& solution) const;

	// 获取指定输入类型、指定capacity的Filter
	void	GetFilterByMediaType(DWORD capacity, QVOD_MEDIA_TYPE& type, std::vector<CLSID>&) const;
	
	// 获取指定capacity的Filter
	void	GetFilterByCapacity(DWORD capacity, std::vector<CLSID>& vectClsid) const;
	
	// 获取Filter的解码优先级
	DWORD	GetFilterPriority(const CLSID& clsid, QVOD_MEDIA_TYPE& mt) const;

	// 获取Filter
	bool	GetFilterByCLSID(const CLSID& clsid, QVOD_FILTER_INFO& node) const;
	
	// 获取Filter Capacity
	DWORD	GetFilterCapacity(const CLSID& clsid) const;

	// 根据容器类型、视频编码类型、音频编码类型获取最优链路
	bool	GetBestGraph(e_CONTAINER c, e_V_ENCODER ve, e_A_ENCODER ae, std::vector<CLSID>& filters) const;

	// 返回CheckByte、Suffix到Filter的映射表
	const std::vector<QVOD_FILTER_MAP>& GetFilterMaps() const;

// 	// 给定一段原始字节数据，获取支持的Source Filter
// 	bool	GetSourceFilterByCheckByte(BYTE* raw_data, CLSID& source) const;
// 
// 	// 给定一段原始字节数据，获取支持的Splitter Filter
// 	bool	GetSplitterFilterByCheckByte(BYTE* raw_data, CLSID& splitter) const;
// 
// 	// 给定一个后缀名，获取能解析此后缀文件的Source Filter
// 	bool	GetSourceFilterBySuffix(CString suffix, CLSID& source) const;
// 
// 	// 给定一个后缀名，获取能解析此后缀文件的Splitter Filter
// 	bool	GetSplitterFilterBySuffix(CString suffix, CLSID& splitter) const;
private:
	QvodFilterManager(void);
	
	bool		LoadDataFromXML(const CString& path);	// 从XML中读取
	bool		ReadFromBin();							// 从资源文件读二进制数据
	bool		ParserXML(BYTE* src);					// 解析XML
	bool		LoadXML_Filters(CXMLElement* );
	bool		LoadXML_BestGraph(CXMLElement*);
	bool		LoadXML_AccelSolution(CXMLElement*);
	bool		LoadXML_FilterMap(CXMLElement*);
private:
	std::vector<QVOD_FILTER_INFO>		mFilters;
	std::vector<QVOD_ACCEL_SOLUTION>	mAccelSolutions;// 高清解决方案
	std::vector<QVOD_FILTER_MAP>		mFilterMaps;	// CheckByte/Suffix -> Filter CLSID
	std::vector<INNER_BEST_GRAPH>		mBestGraphs;	// 最优链路表
	bool								mbLoaded;
};
