
// WebApplicationClientDlg.h : 头文件
//

#pragma once

class CControl;


// CWebApplicationClientDlg 对话框
class CWebApplicationClientDlg : public CDialog
{
// 构造
public:
	CWebApplicationClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WEBAPPLICATIONCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChoiceFile();
	// 定时器事件
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	CSocketControl * m_control;										//视图控制对象
	void InitClient();												//初始化客户端
	void SetBtnState(BOOL _state);									//设置按钮状态
	void ShowErrorMsg(const char* _msg);							//显示错误信息
	void UpdateView();												//更新视图

public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedTransferFile();
	afx_msg void OnBnClickedPauseTransfer();
};
