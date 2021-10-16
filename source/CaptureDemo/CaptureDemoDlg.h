// CaptureDemoDlg.h : ͷ�ļ�
//

#pragma once
#include <windows.h>
#include "..\include\IMediaPost.h"
//#include "..\include\IVideoCapture.h"
typedef HRESULT (*InitMediaPost)(void** ppMediaPost);


typedef HRESULT (*DestoryMediaPost)(void** ppMediaPost);

// CCaptureDemoDlg �Ի���
class CCaptureDemoDlg : public CDialog
{
// ����
public:
	CCaptureDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CCaptureDemoDlg();

// �Ի�������
	enum { IDD = IDD_CAPTUREDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	InitMediaPost		m_pfnInitMediaPost;			//��Ƶ¼��ģ���ʼ������ָ��
	DestoryMediaPost	m_pfnDestoryMediaPost;		//��Ƶ¼��ģ�����ٺ���ָ��
	IMediaPost*			m_pIMediaPost;				//ý�崦��ӿ�
#ifdef COM_INTERFACE
	CComQIPtr<IVideoCapture>		m_pIVideoCapture;				//��Ƶ¼�ƽӿ�ָ��
#else
	IVideoCapture*		m_pIVideoCapture;
#endif
	HMODULE				m_phdm;						//VideoCapture dll���
public:
	afx_msg void OnBnClickedButton1();
};
