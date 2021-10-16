// CaptureDemoDlg.h : 头文件
//

#pragma once
#include <windows.h>
#include "..\include\IMediaPost.h"
//#include "..\include\IVideoCapture.h"
typedef HRESULT (*InitMediaPost)(void** ppMediaPost);


typedef HRESULT (*DestoryMediaPost)(void** ppMediaPost);

// CCaptureDemoDlg 对话框
class CCaptureDemoDlg : public CDialog
{
// 构造
public:
	CCaptureDemoDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CCaptureDemoDlg();

// 对话框数据
	enum { IDD = IDD_CAPTUREDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBegincapture();
public:
	afx_msg void OnBnClickedStopcapture();

protected:
	InitMediaPost		m_pfnInitMediaPost;			//视频录制模块初始化函数指针
	DestoryMediaPost	m_pfnDestoryMediaPost;		//视频录制模块销毁函数指针
	IMediaPost*			m_pIMediaPost;				//媒体处理接口
#ifdef COM_INTERFACE
	CComQIPtr<IVideoCapture>		m_pIVideoCapture;				//视频录制接口指针
#else
	IVideoCapture*		m_pIVideoCapture;
#endif
	HMODULE				m_phdm;						//VideoCapture dll句柄
public:
	afx_msg void OnBnClickedButton1();
};
