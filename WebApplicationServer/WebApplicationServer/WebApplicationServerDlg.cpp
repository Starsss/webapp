/******************************************************************
ģ������WebApplicationServer������Ӧ�ó�������
�ġ�����WebApplicationServer.cpp, WebApplicationServer.h	
�����ˣ�Wang WenJie
�������ڣ�2019-7-29
����˵��������Ӧ�ó������ˣ����ڽ������Կͻ��˵��ļ�	
*******************************************************************/
// WebApplicationServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WebApplicationServer.h"
#include "WebApplicationServerDlg.h"
#include "../../Runtime/Include.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CWebApplicationServerDlg �Ի���




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

	//��ʱ��
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWebApplicationServerDlg ��Ϣ�������

BOOL CWebApplicationServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// ��ʼ���ͻ���
	InitServer();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWebApplicationServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CWebApplicationServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



/*---------------------------------------------------------------------------
���ܣ�OnBnClickedChoiceFolder��ѡ���ļ�����·���¼�
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::OnBnClickedChoiceFolder()
{
	CString _filePath = CUtility::BrowseFolder();
	GetDlgItem(IDC_SAVE_PATH)->SetWindowText(_filePath);
	CStringA _path(_filePath);
	m_control->SetDirectory(_path);
}

/*---------------------------------------------------------------------------
���ܣ�OnBnClickedListen����ʼ�����¼�
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::OnBnClickedListen()
{
	// ���÷����IP
	CString _strIP = CUtility::GetHostIPAddress();
	char ip[MAX_BUFF_LEN];
	::wsprintfA(ip, "ls", (LPCTSTR)_strIP);
	m_control->SetServerIP(ip);
	// ȡ�˿ں�
	CString _port;
	GetDlgItemText(IDC_PORT, _port);
	if(_port.Trim() == _T(""))
	{
		AfxMessageBox(_T("�˿ںŲ���Ϊ��"), MB_OK);
		return;
	}
	// ȡ������
	CString _maxconn;
	GetDlgItemText(IDC_MAXCONN, _maxconn);
	if(_maxconn.Trim() == _T(""))
	{
		AfxMessageBox(_T("����������Ϊ��"), MB_OK);
		return;
	}
	if(_ttoi(_maxconn) <= 0)
	{
		AfxMessageBox(_T("������ȡֵ����"), MB_OK);
		return;
	}
	// ���ü�������
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
���ܣ�InitServer����ʼ�������
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::InitServer()
{
	// ��ʼ��������
	m_control = new CSocketControl();
	GetDlgItem(IDC_HOSTIP)->SetWindowText(CUtility::GetHostIPAddress());
	// ��ʼ��Ĭ���ļ�����·��
	CString _defaultPathName = CUtility::GetWorkDir() + DEFAULT_PATH_NAME;
	if(!PathIsDirectory(_defaultPathName))
	{
		CreateDirectory(_defaultPathName, NULL);
	}
	GetDlgItem(IDC_SAVE_PATH)->SetWindowText(_defaultPathName);
	CStringA _filePath(_defaultPathName);
	m_control->SetDirectory(_filePath);
	// ���ð�ť״̬
	SetBtnState(TRUE);
	// Ĭ�϶˿ں�
	GetDlgItem(IDC_PORT)->SetWindowText(_T("2030"));
	// Ĭ��������
	GetDlgItem(IDC_MAXCONN)->SetWindowText(_T("5"));
	// ��ʼ���б�ؼ�
	m_clientList.SetExtendedStyle(m_clientList.GetExtendedStyle()| LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_clientList.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 50);
	m_clientList.InsertColumn(1, _T("Զ��IP"), LVCFMT_LEFT, 90);
	m_clientList.InsertColumn(2, _T("����״̬"), LVCFMT_LEFT, 70);
	m_clientList.InsertColumn(3, _T("�ļ���"), LVCFMT_LEFT, 199);
	// ��ʼ����Ϣӳ��
	InitState();
}

/*---------------------------------------------------------------------------
���ܣ�OnBnClickedCloseListen���Ͽ�������ť
����: ��
����ֵ����
ע�⣺
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
���ܣ�SetBtnState����ʼ����ť״̬
����: BOOL _state
����ֵ����
ע�⣺
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
���ܣ�ShowErrorMsg����ʾ�������Ĵ�����Ϣ
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationServerDlg::ShowErrorMsg(const char* _msg)
{
	CString _str(_msg);
	AfxMessageBox(_str, MB_OK);
}

/*---------------------------------------------------------------------------
���ܣ�SetList�����ÿͻ����б�
����: ��
����ֵ����
ע�⣺
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
		// ״̬
		if(it->second.state < 7)
		{
			m_clientList.SetItemText(i, 2, m_stateMap.GetAt(it->second.state));
		}
		// �ļ���
		m_clientList.SetItemText(i, 3, CString(it->second.lastFileName.c_str()));
		i++;
	}
}

/*---------------------------------------------------------------------------
���ܣ�OnTimer����ʱ���������
����: UINT_PTR nIDEvent ��ʱ����ʶ
����ֵ����
ע�⣺
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
���ܣ�InitState����ʼ����Ϣӳ��
����: ��
����ֵ����
ע�⣺
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