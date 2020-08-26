/******************************************************************
模块名：WebApplicationServer，网络应用程序服务端
文　件：WebApplicationServer.cpp, WebApplicationServer.h	
创建人：Wang WenJie
创建日期：2019-7-29
功能说明：网络应用程序服务端，用于接收来自客户端的文件	
*******************************************************************/
// WebApplicationServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WebApplicationServer.h"
#include "WebApplicationServerDlg.h"
#include "../../Runtime/Include.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CWebApplicationServerDlg 对话框




CWebApplicationServerDlg::CWebApplicationServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWebApplicationServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWebApplicationServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLIENT_LIST, m_clientList);
}

BEGIN_MESSAGE_MAP(CWebApplicationServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHOICE_FOLDER, &CWebApplicationServerDlg::OnBnClickedChoiceFolder)
	ON_BN_CLICKED(IDC_LISTEN, &CWebApplicationServerDlg::OnBnClickedListen)
	ON_BN_CLICKED(IDC_CLOSE_LISTEN, &CWebApplicationServerDlg::OnBnClickedCloseListen)

	//定时器
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWebApplicationServerDlg 消息处理程序

BOOL CWebApplicationServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 初始化客户端
	InitServer();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWebApplicationServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWebApplicationServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWebApplicationServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



/*---------------------------------------------------------------------------
功能：OnBnClickedChoiceFolder，选择文件保存路径事件
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::OnBnClickedChoiceFolder()
{
	CString _filePath = CUtility::BrowseFolder();
	GetDlgItem(IDC_SAVE_PATH)->SetWindowText(_filePath);
	CStringA _path(_filePath);
	m_control->SetDirectory(_path);
}

/*---------------------------------------------------------------------------
功能：OnBnClickedListen，开始监听事件
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::OnBnClickedListen()
{
	// 设置服务端IP
	CString _strIP = CUtility::GetHostIPAddress();
	char ip[MAX_BUFF_LEN];
	::wsprintfA(ip, "ls", (LPCTSTR)_strIP);
	m_control->SetServerIP(ip);
	// 取端口号
	CString _port;
	GetDlgItemText(IDC_PORT, _port);
	if(_port.Trim() == _T(""))
	{
		AfxMessageBox(_T("端口号不能为空"), MB_OK);
		return;
	}
	// 取连接数
	CString _maxconn;
	GetDlgItemText(IDC_MAXCONN, _maxconn);
	if(_maxconn.Trim() == _T(""))
	{
		AfxMessageBox(_T("连接数不能为空"), MB_OK);
		return;
	}
	if(_ttoi(_maxconn) <= 0)
	{
		AfxMessageBox(_T("连接数取值错误"), MB_OK);
		return;
	}
	// 设置监听参数
	m_control->SetPort(_ttoi(_port));
	m_control->SetMaxConnect(_ttoi(_maxconn));
	m_control->Listen();
	int _state = m_control->GetState();
	if(_state == LISTENING)
	{
		GetDlgItem(IDC_STATE)->SetWindowText(LISTEN_MSG);
		SetBtnState(FALSE);
		SetTimer(UPDATE_LIST, LIST_TIME, NULL);
	}
	else
	{
		ShowErrorMsg(m_control->GetErrorInfo().msg);
	}
}

/*---------------------------------------------------------------------------
功能：InitServer，初始化服务端
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::InitServer()
{
	// 初始化控制器
	m_control = new CSocketControl();
	GetDlgItem(IDC_HOSTIP)->SetWindowText(CUtility::GetHostIPAddress());
	// 初始化默认文件保存路径
	CString _defaultPathName = CUtility::GetWorkDir() + DEFAULT_PATH_NAME;
	if(!PathIsDirectory(_defaultPathName))
	{
		CreateDirectory(_defaultPathName, NULL);
	}
	GetDlgItem(IDC_SAVE_PATH)->SetWindowText(_defaultPathName);
	CStringA _filePath(_defaultPathName);
	m_control->SetDirectory(_filePath);
	// 设置按钮状态
	SetBtnState(TRUE);
	// 默认端口号
	GetDlgItem(IDC_PORT)->SetWindowText(_T("2030"));
	// 默认连接数
	GetDlgItem(IDC_MAXCONN)->SetWindowText(_T("5"));
	// 初始化列表控件
	m_clientList.SetExtendedStyle(m_clientList.GetExtendedStyle()| LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_clientList.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 50);
	m_clientList.InsertColumn(1, _T("远程IP"), LVCFMT_LEFT, 90);
	m_clientList.InsertColumn(2, _T("连接状态"), LVCFMT_LEFT, 70);
	m_clientList.InsertColumn(3, _T("文件名"), LVCFMT_LEFT, 199);
	// 初始化消息映射
	InitState();
}

/*---------------------------------------------------------------------------
功能：OnBnClickedCloseListen，断开监听按钮
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::OnBnClickedCloseListen()
{
	if(m_control->CloseListen())
	{
		KillTimer(UPDATE_LIST);
		GetDlgItem(IDC_STATE)->SetWindowText(CLOSED_MSG);
		SetBtnState(TRUE);
	}
	else
	{
		ShowErrorMsg(m_control->GetErrorInfo().msg);
	}
}


/*---------------------------------------------------------------------------
功能：SetBtnState，初始化按钮状态
参数: BOOL _state
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::SetBtnState(BOOL _state)
{
	GetDlgItem(IDC_LISTEN)->EnableWindow(_state);
	int iIDS[] = {IDC_CLOSE_LISTEN, IDC_RESTART};
	for(int i=0; i<2; i++)
	{
		GetDlgItem(iIDS[i])->EnableWindow(!_state);
	}
}

LRESULT CWebApplicationServerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CLOSE)
	{
		if (m_control->GetState() != INIT || m_control->GetState() != CLOSED)
		{
			m_control->CloseListen();
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

/*---------------------------------------------------------------------------
功能：ShowErrorMsg，显示控制器的错误信息
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::ShowErrorMsg(const char* _msg)
{
	CString _str(_msg);
	AfxMessageBox(_str, MB_OK);
}

/*---------------------------------------------------------------------------
功能：SetList，设置客户端列表
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::SetList()
{
	ClientMap _clients = m_control->GetClients();
	int i = 0;
	CString _strSocket;
	CString _strIP;
	m_clientList.DeleteAllItems();
	for(ClientMap::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		// ID
		_strSocket.Format(_T("%d"), it->first);
		m_clientList.InsertItem(i, _strSocket);
		// IP
		_strIP = inet_ntoa(it->second.sin.sin_addr);
		m_clientList.SetItemText(i, 1, _strIP);
		// 状态
		if(it->second.state < 7)
		{
			m_clientList.SetItemText(i, 2, m_stateMap.GetAt(it->second.state));
		}
		// 文件名
		m_clientList.SetItemText(i, 3, CString(it->second.lastFileName.c_str()));
		i++;
	}
}

/*---------------------------------------------------------------------------
功能：OnTimer，定时器处理过程
参数: UINT_PTR nIDEvent 定时器标识
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case UPDATE_LIST:
		SetList();
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

/*---------------------------------------------------------------------------
功能：InitState，初始化消息映射
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::InitState()
{
	m_stateMap.Add(INIT_MSG);
	m_stateMap.Add(CONNECTED_MSG);
	m_stateMap.Add(TRANSFER_MSG);
	m_stateMap.Add(CLOSED_MSG);
	m_stateMap.Add(LISTEN_MSG);
	m_stateMap.Add(CONNECTED_MSG);
	m_stateMap.Add(COMPLETE_MSG);
}