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

//#include "QvodStructure.h"
#include "Struct.h"
#include <map>
#include <vector>



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
