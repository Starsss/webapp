
// WebApplicationServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

class CControl;


// CWebApplicationServerDlg 对话框
class CWebApplicationServerDlg : public CDialog
{
// 构造
public:
	CWebApplicationServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WEBAPPLICATIONSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


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
	afx_msg void OnBnClickedChoiceFolder();
	CListCtrl m_clientList;
	CStringArray m_stateMap;							//初始化状态
	

private:
	
	void InitServer();									//服务端初始化
	void SetBtnState(BOOL _state);						//初始化按钮状态
	void ShowErrorMsg(const char* _msg);				//显示错误信息
	void SetList();										//设置客户端列表
	void InitState();									//初始化状态
	CSocketControl* m_control;							//全局控制器属性
	
	
	
	
public:
	afx_msg void OnBnClickedListen();
	afx_msg void OnBnClickedCloseListen();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
