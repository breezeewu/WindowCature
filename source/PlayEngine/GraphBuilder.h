#pragma once
#include "GraphBaseBuilder.h"
#include "GraphStructure.h"
#include "QVodFilterManager.h"
#include "QMediaInfo.h"
#include "..\include\IKeyFrameInfo.h"

extern HINSTANCE	g_hInstance;//dllʵ�����

class CGraphBuilder :
	public CGraphBaseBuilder
{
public:
	CGraphBuilder(void);
public:
	~CGraphBuilder(void);

public:
/*********************************************************************************************
	������:     ResetFlags                                
	��������:   �������б�ʶ      
	����ֵ��    ��
*********************************************************************************************/
	virtual void ResetFlags();

/*********************************************************************************************
	������:     ResetInterface                                
	��������:   �������нӿ�      
	����ֵ��    ��
*********************************************************************************************/

	virtual void ResetInterface();

/*********************************************************************************************
	������:     ResetFilters                                
	��������:   ��������filter      
	����ֵ��    ��
*********************************************************************************************/
	virtual void ResetFilters();

/*********************************************************************************************
	������:     DestroyGraph                                
	��������:   ������·     
	����ֵ��    ��
*********************************************************************************************/
	virtual void DestroyGraph();

/*********************************************************************************************
	������:     GetPreFilterOnPath
	��������:   ��ȡָ��filter���͵�ǰһ��filter
	����1:		e_FilterType t��filter����
	����2:		int* idx_audio_decoder��filter����
	����ֵ��    ��
*********************************************************************************************/
CComQIPtr<IBaseFilter>	GetPreFilterOnPath(e_FilterType t, int* idx_audio_decoder = NULL);

/*********************************************************************************************
	������:     RepalcePath                                
	��������:   �ض�λdll·��
	����1:		CString& path��·��
	����ֵ��    ��
*********************************************************************************************/
	void	RepalcePath(CString& path);

/*********************************************************************************************
	������:     PrepareFilter                                
	��������:   �ض�λFilter����������dll·��
	����1:		QVOD_FILTER_INFO& filter_info��filter��Ϣ�ṹ��
	����ֵ��    ��
*********************************************************************************************/
	void	PrepareFilter(QVOD_FILTER_INFO& filter_info);

/*********************************************************************************************
	������:     LoadComponents                                
	��������:   �������
	����1:		QVOD_FILTER_INFO& filter_info��filter��Ϣ�ṹ��
	����2:		std::vector<HMODULE>&  modules���ļ�����б�����
	����ֵ��    �ɹ�����true������Ϊfalse
*********************************************************************************************/
	bool	LoadComponents(QVOD_FILTER_INFO& filter_info, std::vector<HMODULE>&  modules);

/*********************************************************************************************
	������:     LoadFilter                                
	��������:   ����filter
	����1:		const CLSID& clsid��filter clsid
	����2:		QVOD_FILTER_INFO& filter_info��filter��Ϣ�ṹ��
	����ֵ��    �ɹ�����IBaseFilterָ�룬���򷵻ؿ�ָ��
*********************************************************************************************/
	CComQIPtr<IBaseFilter>		LoadFilter(const CLSID& clsid, QVOD_FILTER_INFO& filter_info);

/*********************************************************************************************
	������:     ReadCheckBytes                                
	��������:   ��ȡcheckbyte
	����1:		BYTE* buffer������bufferָ��
	����2:		int size_to_read��filter��Ϣ�ṹ��
	����ֵ��    �ɹ�����IBaseFilterָ�룬���򷵻ؿ�ָ��
*********************************************************************************************/
bool			ReadCheckBytes(BYTE* buffer, int size_to_read);

/*********************************************************************************************
	������:     GetContainerByCheckBytes                                
	��������:   ����CheckByte��ȡ��������
	����1:		wchar_t* pPath���ļ�·��
	����2:		e_CONTAINER& container����������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*********************************************************************************************/
	HRESULT			GetContainerByCheckBytes(wchar_t* pPath, e_CONTAINER& container);
	
/*********************************************************************************************
	������:     GetContainerBySuffix                                
	��������:   ����clsid���ݺ�׺����ȡ��������
	����1:		wchar_t* pPath���ļ�·��
	����2:		e_CONTAINER& container����������
	����ֵ��    �ɹ�����S_OK������Ϊʧ��
*********************************************************************************************/
	HRESULT			GetContainerBySuffix(wchar_t* pPath, e_CONTAINER& container);

/*********************************************************************************************
	������:     Load_ReaderFilter                                
	��������:   ����clsid����file reader
	����1:		const CLSID& clsid��filter clsid
	����ֵ��    �ɹ�����IBaseFilterָ�룬���򷵻ؿ�ָ��
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_ReaderFilter(CString path, const CLSID& clsid = CLSID_NULL);

/*********************************************************************************************
	������:     Load_SourceFilter                                
	��������:   ����clsid����Դ����filter
	����1:		const CLSID& clsid��filter clsid
	����ֵ��    �ɹ�����IBaseFilterָ�룬���򷵻ؿ�ָ��
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_SourceFilter(CString path, const CLSID& clsid);

/*********************************************************************************************
	������:     Load_SplitterFilter                                
	��������:   ����clsid���ؽ���filter
	����1:		const CLSID& clsid��filter clsid
	����ֵ��    �ɹ�����IBaseFilterָ�룬���򷵻ؿ�ָ��
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_SplitterFilter(const CLSID& clsid);

/*********************************************************************************************
	������:     Load_VideoDecoderByCLSID                                
	��������:   ����clsid������Ƶ����filter
	����1:		const CLSID& clsid��filter clsid
	����ֵ��    �ɹ�����IBaseFilterָ�룬���򷵻ؿ�ָ��
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_VideoDecoderByCLSID(CComQIPtr<IPin>& pVideoPin, const CLSID& clsid);

/*********************************************************************************************
	������:     Load_AudioDecoderByCLSID                                
	��������:   ����clsid������Ƶ����filter
	����1:		const CLSID& clsid��filter clsid
	����ֵ��    �ɹ�����IBaseFilterָ�룬���򷵻ؿ�ָ��
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_AudioDecoderByCLSID(const CLSID& clsid);

/*********************************************************************************************
	������:     Load_VideoDecoderAuto                                
	��������:   �Զ�������Ƶ����filter
	����ֵ��    �ɹ�����IBaseFilterָ�룬���򷵻ؿ�ָ��
*********************************************************************************************/
	CComQIPtr<IBaseFilter> 	Load_VideoDecoderAuto(CComQIPtr<IPin>& pVideoPin);

/*********************************************************************************************
	������:     Load_AudioDecoderAuto                                
	��������:   �Զ�������Ƶ����filter
	����ֵ��    �ɹ�����true�����򷵻�false
*********************************************************************************************/
	bool			Load_AudioDecoderAuto();

/*********************************************************************************************
	������:     Load_VideoRenderByClsid                                
	��������:   ����clsid������Ƶ��Ⱦ��
	����ֵ��    �ɹ�����true�����򷵻�false
*********************************************************************************************/
	bool			Load_VideoRenderByClsid(/*const CLSID& clsid*/ CLSID clsid);

/*********************************************************************************************
	������:     ConnectAudioRenderAuto                                
	��������:   �Զ�������Ƶ��Ⱦ��
	����ֵ��    �ɹ�����true�����򷵻�false
*********************************************************************************************/
	bool			ConnectAudioRenderAuto();

/*********************************************************************************************
	������:     Load_VideoRenderAuto                                
	��������:   �Զ�������Ƶ��Ⱦ��
	����ֵ��    �ɹ�����true�����򷵻�false
*********************************************************************************************/
	bool			Load_VideoRenderAuto();

/*********************************************************************************************
	������:     Load_DVDNavigater                                
	��������:   ����dvd����
	����ֵ��    �ɹ�����true�����򷵻�false
*********************************************************************************************/
	bool			Load_DVDNavigater(wchar_t* pPath);

protected:
	void SortFilterByPriority(std::vector<CLSID>& filters, QVOD_MEDIA_TYPE& mt);

	bool CreateAudioRender();

protected:

	e_CONTAINER						m_ConType;					// ��������
	e_V_ENCODER						m_VideoType;				// ��Ƶ��������
	std::vector<e_A_ENCODER>		m_AudioTypes;				// ��Ƶ��������
	CComQIPtr<IDvdGraphBuilder>		m_pDvdGraph;				// DVD

	// Filters
	CComQIPtr<IBaseFilter>			m_pAudioCapture;			// ��Ƶ�ɼ�filter
	CComQIPtr<IBaseFilter>			m_pVideoCapture;			// ��Ƶ�ɼ�filter
	CComQIPtr<IBaseFilter>			m_pFilterFileReader;		// SYS FileReader/XLMV Reader/...
	CComQIPtr<IBaseFilter>			m_pFilterSource;			// ���ز��ŵ�Source
	CComQIPtr<IBaseFilter>			m_pFilterSplitter;			// ���ء����粥�ŵ�Splitter
	CComQIPtr<IBaseFilter>			m_pFilterVideoDecoder;		// ѡ�õ�Video Decoder Filter
	CComQIPtr<IBaseFilter>			m_pFilterVideoRender;		// ��ƵRender
	CComQIPtr<IBaseFilter>			m_pFilterAudioRender;		// ��ƵRender
	CComQIPtr<IBaseFilter>			m_pFilterVobSubTitle;		// ��Ļ������
	CComQIPtr<IBaseFilter>			m_pFilterQvodSubTitle;		// QVOD SubTitle
	CComQIPtr<IBaseFilter>			m_pFilterQvodPostVideo;		// QVOD PostVideo
	CComQIPtr<IBaseFilter>			m_pFilterQvodSound;			// QVOD Sound filter
	CComQIPtr<IBaseFilter>			m_pFilterAudioSwitch;		// ��Ƶ���л���
	CComQIPtr<IBaseFilter>			m_pFilterColorSpaceTrans;	// Color Space trans
	CComQIPtr<IBaseFilter>			m_pFilterSubTitle;			// Color Space trans 2 ��POST VIDEO->Render�䣩
	CComQIPtr<IBaseFilter>			m_pFilterVideoEncoder;		//��Ƶ������
	CComQIPtr<IBaseFilter>			m_pFilterAudioEncoder;		//��Ƶ������
	CComQIPtr<IBaseFilter>			m_pFilterMuxer;				//muxer filter
	CComQIPtr<IBaseFilter>			m_pFilterSink;				//Sink filter
	std::vector<CComQIPtr<IBaseFilter>>	
									m_pFilterAudioDecoders;			// ѡ�õ�Audio Decoder Filter


	// DVD
	CComQIPtr<IDvdControl2>			m_pDvdControl;
	CComQIPtr<IDvdInfo2>			m_pIDvdInfo2;

	// ý�����
	CComQIPtr<IMediaEvent>			m_pIMediaEvent;					//�¼�֪ͨ�ӿ�
	CComQIPtr<IKeyFrameInfo>		m_pKeyFrameInfo;				//�ؼ�֡��Ϣ�ӿ�
	CComPtr<IKeyFrameInfoEx>		m_pKeyFrameInfoEx;				//�ؼ�֡��չ��Ϣ�ӿ�


	// filter -> dlls
	std::map<IBaseFilter*, std::vector<HMODULE> > 
									m_Module;						// �Ѽ��ص�Dll


	QVOD_MEDIA_TYPE					m_MTToVRender;

	int								m_CurVideoBitCout;				//��Ƶλ��

	CComQIPtr<IPin>					m_CurVideoOutputPin;//ѡ�õ����������Ƶ����PIN INDEX)

	CString							m_SoundDevice;				//��Ƶ����豸
	CString							m_DllPath;					//dll�ļ�·��
	CString							m_ExePath;					//��ִ���ļ�·��
	HMODULE							m_D3DX9_32HMD;
	HMODULE							m_EVRPresenterHMD;

	// ȱ�ٵ�Filter
	std::map<CString, QVOD_FILTER_INFO>	m_LossFilters;

	// ��Ƶ����
	int								m_FileBitrate;					//�ļ�����
	int								m_VideoBitrate;					//��Ƶ����	
	int								m_VideoWidth;					//��Ƶ���
	int								m_VideoHeight;					//��Ƶ�߶�

};
