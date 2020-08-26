
// WebApplicationClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WebApplicationClient.h"
#include "WebApplicationClientDlg.h"
#include "..\..\Runtime\Include.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWebApplicationClientDlg �Ի���




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

	// ��ʱ��
	ON_WM_TIMER()
	
END_MESSAGE_MAP()


// CWebApplicationClientDlg ��Ϣ�������

BOOL CWebApplicationClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// ��ʼ��
	InitClient();
	// ��������ʼ��
	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pProg->SetRange(MIN_RANGE, MAX_RANGE);
	pProg->SetPos(0);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWebApplicationClientDlg::OnPaint()
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
HCURSOR CWebApplicationClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*---------------------------------------------------------------------------
���ܣ�OnBnClickedChoiceFile��ѡ���ļ���ť�¼�
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedChoiceFile()
{
	// ����ļ�
	CString _filePath = CUtility::BrowseFile();
	GetDlgItem(IDC_FILE_PATH)->SetWindowText(_filePath);
	GetDlgItem(IDC_FILE_SIZE)->SetWindowText(CUtility::GetFileSize(_filePath));
	// ��ʼ��������
	CProgressCtrl* _progressCtr = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	_progressCtr->SetPos(0);
}

/*---------------------------------------------------------------------------
���ܣ�OnBnClickedConnect�������ʼ�����¼�
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedConnect()
{
	// ���÷����IP��ַ
	CIPAddressCtrl* _ctrl = (CIPAddressCtrl*)GetDlgItem(IDC_SERVER_IPADDR);
	char _serverIP[MAX_BUFF_LEN];
	::wsprintfA(_serverIP, "%ls", CUtility::IP2CString(_ctrl));
	// ���ö˿�
	CString _strPort;
	GetDlgItemText(IDC_PORT, _strPort);
	if(_strPort.Trim() == _T(""))
	{
		AfxMessageBox(_T("�˿ںŲ���Ϊ��"), MB_OK);
		return;
	}
	int _port = _ttoi(_strPort);
	m_control->SetServerIP(_serverIP);
	m_control->SetPort(_port);
	// ��ʼ����
	m_control->Connect();
	int _state = m_control->GetState();
	
	if(_state == CONNECTED)  // ���ӳɹ�
	{
		GetDlgItem(IDC_STATE)->SetWindowText(CONNECTED_MSG);
		// ���ð�ť״̬
		SetBtnState(FALSE);
		SetTimer(UPDATE_LIST, LIST_TIME, NULL);
	}
	else
	{
		ShowErrorMsg(m_control->GetErrorInfo().msg);
	}
}

/*---------------------------------------------------------------------------
���ܣ�InitClient����ʼ���ͻ���
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::InitClient()
{
	// ��ʼ����־
	InitLog();
	// ��ʼ��������
	m_control = new CSocketControl();
	GetDlgItem(IDC_STATE)->SetWindowText(INIT_MSG);
	// ��ȡ����IP��ַ
	CString _hostIP = CUtility::GetHostIPAddress();
	GetDlgItem(IDC_HOST_IPADDR)->SetWindowText(_hostIP);
	// �ؼ�״̬��ʼ��
	SetBtnState(TRUE);
	// ��ʼ�������ip�Ͷ˿�
	GetDlgItem(IDC_SERVER_IPADDR)->SetWindowText(_hostIP);
	GetDlgItem(IDC_PORT)->SetWindowText(_T("2030"));
}


/*---------------------------------------------------------------------------
���ܣ�OnBnClickedDisconnect���Ͽ����Ӱ�ť�¼�
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedDisconnect()
{
	if(m_control->CloseConnect())
	{
		//���ÿؼ�״̬
		SetBtnState(TRUE);
		GetDlgItem(IDC_STATE)->SetWindowText(CLOSED_MSG);
		KillTimer(UPDATE_LIST);
	}
}

/*---------------------------------------------------------------------------
���ܣ�SetBtnState��������ͼ��ť
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::SetBtnState(BOOL _state)
{
	GetDlgItem(IDC_CONNECT)->EnableWindow(_state);
	// �ؼ�����
	int IDCS[] = {IDC_DISCONNECT, IDC_TRANSFER_FILE, IDC_PAUSE_TRANSFER} ;
	for(int i=0; i<3; i++)
	{
		GetDlgItem(IDCS[i])->EnableWindow(!_state);
	}
}

/*---------------------------------------------------------------------------
���ܣ�OnBnClickedTransferFile����ʼ���䰴ť�¼�
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedTransferFile()
{
	CString _filePath;
	GetDlgItem(IDC_FILE_PATH)->GetWindowText(_filePath);
	// У��·��
	if(_filePath == _T(""))
	{
		AfxMessageBox(_T("������ļ�����Ϊ��"), MB_OK);
		return;
	}
	// �����ļ�·��
	CStringA _path(_filePath);
	m_control->SetFilePath(_path);
	// ��ʼ����
	if(!m_control->TransferFile())
	{
		ShowErrorMsg(m_control->GetErrorInfo().msg);
	}
	// ���ö�ʱ��
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
���ܣ�OnTimer����ʱ���������
����: UINT_PTR nIDEvent ��ʱ����ʶ
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case UPDATE_SPEED:				// ��ʼ������ͼ
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
���ܣ�OnBnClickedPauseTransfer����ͣ���䰴ť�¼�
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::OnBnClickedPauseTransfer()
{
	// ��ͣ����
	m_control->InterruptTransfer(NULL, TRUE);
	KillTimer(UPDATE_SPEED);
	GetDlgItem(IDC_STATE)->SetWindowText(INTERRUPTED_MSG);
}

/*---------------------------------------------------------------------------
���ܣ�ShowErrorMsg����ʾ�������Ĵ�����Ϣ
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::ShowErrorMsg(const char* _msg)
{
	CString _str(_msg);
	AfxMessageBox(_str, MB_OK);
}

/*---------------------------------------------------------------------------
���ܣ�UpdateView��������ͼ
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CWebApplicationClientDlg::UpdateView()
{
	// ���ȿؼ�
	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	// ���ڴ�����
	if(m_control->GetState() == TRANSFERING)
	{
		// �ٶȸ���
		CString _speed(m_control->GetSpeed().c_str());
		GetDlgItem(IDC_SPEED)->SetWindowText(_speed);
		m_control->SetStartOffset();
		// ���ȸ���
		int _progress = m_control->GetProgress();
		pProg->SetPos(_progress);
	}
	else if(m_control->GetState() == COMPLETED)
	{
		// ��������رն�ʱ��
		KillTimer(UPDATE_SPEED);
		pProg->SetPos(100);
		GetDlgItem(IDC_STATE)->SetWindowText(COMPLETE_MSG);
	}
}
