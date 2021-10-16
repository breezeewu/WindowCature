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

//#include "QvodStructure.h"
#include "Struct.h"
#include <map>
#include <vector>



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
