
// WebApplicationServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

class CControl;


// CWebApplicationServerDlg �Ի���
class CWebApplicationServerDlg : public CDialog
{
// ����
public:
	CWebApplicationServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WEBAPPLICATIONSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


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
	afx_msg void OnBnClickedChoiceFolder();
	CListCtrl m_clientList;
	CStringArray m_stateMap;							//��ʼ��״̬
	

private:
	
	void InitServer();									//����˳�ʼ��
	void SetBtnState(BOOL _state);						//��ʼ����ť״̬
	void ShowErrorMsg(const char* _msg);				//��ʾ������Ϣ
	void SetList();										//���ÿͻ����б�
	void InitState();									//��ʼ��״̬
	CSocketControl* m_control;							//ȫ�ֿ���������
	
	
	
	
public:
	afx_msg void OnBnClickedListen();
	afx_msg void OnBnClickedCloseListen();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
