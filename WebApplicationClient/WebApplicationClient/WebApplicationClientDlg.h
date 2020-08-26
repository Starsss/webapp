
// WebApplicationClientDlg.h : ͷ�ļ�
//

#pragma once

class CControl;


// CWebApplicationClientDlg �Ի���
class CWebApplicationClientDlg : public CDialog
{
// ����
public:
	CWebApplicationClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WEBAPPLICATIONCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChoiceFile();
	// ��ʱ���¼�
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	CSocketControl * m_control;										//��ͼ���ƶ���
	void InitClient();												//��ʼ���ͻ���
	void SetBtnState(BOOL _state);									//���ð�ť״̬
	void ShowErrorMsg(const char* _msg);							//��ʾ������Ϣ
	void UpdateView();												//������ͼ

public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedTransferFile();
	afx_msg void OnBnClickedPauseTransfer();
};
