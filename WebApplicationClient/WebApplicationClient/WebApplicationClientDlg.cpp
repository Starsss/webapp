
// WebApplicationClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WebApplicationClient.h"
#include "WebApplicationClientDlg.h"
#include "..\..\Runtime\Include.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWebApplicationClientDlg 对话框




CWebApplicationClientDlg::CWebApplicationClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWebApplicationClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWebApplicationClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWebApplicationClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHOICE_FILE, &CWebApplicationClientDlg::OnBnClickedChoiceFile)
	ON_BN_CLICKED(IDC_CONNECT, &CWebApplicationClientDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CWebApplicationClientDlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_TRANSFER_FILE, &CWebApplicationClientDlg::OnBnClickedTransferFile)
	ON_BN_CLICKED(IDC_PAUSE_TRANSFER, &CWebApplicationClientDlg::OnBnClickedPauseTransfer)

	// 定时器
	ON_WM_TIMER()
	
END_MESSAGE_MAP()


// CWebApplicationClientDlg 消息处理程序

BOOL CWebApplicationClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	// 初始化
	InitClient();
	// 进度条初始化
	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pProg->SetRange(MIN_RANGE, MAX_RANGE);
	pProg->SetPos(0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWebApplicationClientDlg::OnPaint()
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
HCURSOR CWebApplicationClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*---------------------------------------------------------------------------
功能：OnBnClickedChoiceFile，选择文件按钮事件
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedChoiceFile()
{
	// 浏览文件
	CString _filePath = CUtility::BrowseFile();
	GetDlgItem(IDC_FILE_PATH)->SetWindowText(_filePath);
	GetDlgItem(IDC_FILE_SIZE)->SetWindowText(CUtility::GetFileSize(_filePath));
	// 初始化进度条
	CProgressCtrl* _progressCtr = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	_progressCtr->SetPos(0);
}

/*---------------------------------------------------------------------------
功能：OnBnClickedConnect，点击开始连接事件
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedConnect()
{
	// 设置服务端IP地址
	CIPAddressCtrl* _ctrl = (CIPAddressCtrl*)GetDlgItem(IDC_SERVER_IPADDR);
	char _serverIP[MAX_BUFF_LEN];
	::wsprintfA(_serverIP, "%ls", CUtility::IP2CString(_ctrl));
	// 设置端口
	CString _strPort;
	GetDlgItemText(IDC_PORT, _strPort);
	if(_strPort.Trim() == _T(""))
	{
		AfxMessageBox(_T("端口号不能为空"), MB_OK);
		return;
	}
	int _port = _ttoi(_strPort);
	m_control->SetServerIP(_serverIP);
	m_control->SetPort(_port);
	// 开始连接
	m_control->Connect();
	int _state = m_control->GetState();
	
	if(_state == CONNECTED)  // 连接成功
	{
		GetDlgItem(IDC_STATE)->SetWindowText(CONNECTED_MSG);
		// 设置按钮状态
		SetBtnState(FALSE);
		SetTimer(UPDATE_LIST, LIST_TIME, NULL);
	}
	else
	{
		ShowErrorMsg(m_control->GetErrorInfo().msg);
	}
}

/*---------------------------------------------------------------------------
功能：InitClient，初始化客户端
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::InitClient()
{
	// 初始化日志
	InitLog();
	// 初始化控制器
	m_control = new CSocketControl();
	GetDlgItem(IDC_STATE)->SetWindowText(INIT_MSG);
	// 获取本机IP地址
	CString _hostIP = CUtility::GetHostIPAddress();
	GetDlgItem(IDC_HOST_IPADDR)->SetWindowText(_hostIP);
	// 控件状态初始化
	SetBtnState(TRUE);
	// 初始化服务端ip和端口
	GetDlgItem(IDC_SERVER_IPADDR)->SetWindowText(_hostIP);
	GetDlgItem(IDC_PORT)->SetWindowText(_T("2030"));
}


/*---------------------------------------------------------------------------
功能：OnBnClickedDisconnect，断开连接按钮事件
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedDisconnect()
{
	if(m_control->CloseConnect())
	{
		//设置控件状态
		SetBtnState(TRUE);
		GetDlgItem(IDC_STATE)->SetWindowText(CLOSED_MSG);
		KillTimer(UPDATE_LIST);
	}
}

/*---------------------------------------------------------------------------
功能：SetBtnState，设置视图按钮
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::SetBtnState(BOOL _state)
{
	GetDlgItem(IDC_CONNECT)->EnableWindow(_state);
	// 控件数组
	int IDCS[] = {IDC_DISCONNECT, IDC_TRANSFER_FILE, IDC_PAUSE_TRANSFER} ;
	for(int i=0; i<3; i++)
	{
		GetDlgItem(IDCS[i])->EnableWindow(!_state);
	}
}

/*---------------------------------------------------------------------------
功能：OnBnClickedTransferFile，开始传输按钮事件
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedTransferFile()
{
	CString _filePath;
	GetDlgItem(IDC_FILE_PATH)->GetWindowText(_filePath);
	// 校验路径
	if(_filePath == _T(""))
	{
		AfxMessageBox(_T("传输的文件不能为空"), MB_OK);
		return;
	}
	// 设置文件路径
	CStringA _path(_filePath);
	m_control->SetFilePath(_path);
	// 开始传输
	if(!m_control->TransferFile())
	{
		ShowErrorMsg(m_control->GetErrorInfo().msg);
	}
	// 设置定时器
	SetTimer(UPDATE_SPEED, ELAPSE_TIME, NULL);
	m_control->SetStartOffset();
	GetDlgItem(IDC_STATE)->SetWindowText(TRANSFER_MSG);
}


LRESULT CWebApplicationClientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CLOSE:
		if (m_control->GetState() != INIT)
		{
			m_control->CloseConnect();
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

/*---------------------------------------------------------------------------
功能：OnTimer，定时器处理过程
参数: UINT_PTR nIDEvent 定时器标识
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case UPDATE_SPEED:				// 开始更新视图
		UpdateView();
		break;
	case UPDATE_LIST:
		if(m_control->GetState() == CLOSED)
		{
			KillTimer(UPDATE_LIST);
			m_control->CloseConnect();
			SetBtnState(TRUE);
			GetDlgItem(IDC_STATE)->SetWindowText(CLOSED_MSG);
		}
		break;
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent); 
}

/*---------------------------------------------------------------------------
功能：OnBnClickedPauseTransfer，暂停传输按钮事件
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedPauseTransfer()
{
	// 暂停传输
	m_control->InterruptTransfer(NULL, TRUE);
	KillTimer(UPDATE_SPEED);
	GetDlgItem(IDC_STATE)->SetWindowText(INTERRUPTED_MSG);
}

/*---------------------------------------------------------------------------
功能：ShowErrorMsg，显示控制器的错误信息
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::ShowErrorMsg(const char* _msg)
{
	CString _str(_msg);
	AfxMessageBox(_str, MB_OK);
}

/*---------------------------------------------------------------------------
功能：UpdateView，更新视图
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::UpdateView()
{
	// 进度控件
	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	// 正在传输中
	if(m_control->GetState() == TRANSFERING)
	{
		// 速度更新
		CString _speed(m_control->GetSpeed().c_str());
		GetDlgItem(IDC_SPEED)->SetWindowText(_speed);
		m_control->SetStartOffset();
		// 进度更新
		int _progress = m_control->GetProgress();
		pProg->SetPos(_progress);
	}
	else if(m_control->GetState() == COMPLETED)
	{
		// 传输结束关闭定时器
		KillTimer(UPDATE_SPEED);
		pProg->SetPos(100);
		GetDlgItem(IDC_STATE)->SetWindowText(COMPLETE_MSG);
	}
}
