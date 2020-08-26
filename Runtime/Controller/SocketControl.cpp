/******************************************************************
ģ������CSocketControl
�ġ�����CSocketControl.h, CSocketControl.cpp	
�����ˣ�Wang WenJie
�������ڣ�2019-8-2
��  ����v1.2.0
����˵��������Ӧ�ó��������ʵ���ļ�
*******************************************************************/

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SocketControl.h"
#include "../Model/SocketModel.h"
#include "../Extend/Log.h"
#include "../Model/FileModel.h"
#include "../Config/AppString.h"



/********************ȫ���̷߳���*********************************/
/*---------------------------------------------------------------------------
���ܣ�MessageProcessThread����Ϣ�̴߳��������ڵ�
����: LPVOID ��ǰģ��
����ֵ��UINT
ע�⣺
---------------------------------------------------------------------------*/
UINT MessageProcessThread(LPVOID lpParam)
{
	CSocketControl *_control = (CSocketControl *)lpParam;
	_control->ReceiveProcess();
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�AcceptConnectThread����Ϣ�̴߳��������ڵ�
����: LPVOID ��ǰģ��
����ֵ��UINT
ע�⣺
---------------------------------------------------------------------------*/
UINT AcceptConnectThread(LPVOID lpParam)
{
	CSocketControl *_control = (CSocketControl *)lpParam;
	_control->AcceptConnect();
	return TRUE;
}

/**************************CSocketControl��Get��Set����***************************/
// ��ȡ������״̬
int CSocketControl::GetState()
{
	return m_state;
}

// ���÷���˵�ַ
void CSocketControl::SetServerIP(const char* _ip)
{
	this->m_serverIP = _ip;
}

// ���ö˿ں�
void CSocketControl::SetPort(int _port)
{
	this->m_port = _port;
}

// �������������
void CSocketControl::SetMaxConnect(int _maxConnect)
{
	this->m_maxConnect = _maxConnect;
}

// �����ļ�·��
void CSocketControl::SetFilePath(const char* _filePath)
{
	this->m_strFilePath = _filePath;
}

// �����ļ�Ŀ¼
void CSocketControl::SetDirectory(const char* _directory)
{
	this->m_strDirectory = _directory;
}

// ��ȡ�ļ��������
int CSocketControl::GetProgress()
{
	long _total = m_fileInfo._fileSize / BLOCK_SIZE;
	EnterCriticalSection(&m_msgSection);
	long _transferred = m_offset / BLOCK_SIZE;
	LeaveCriticalSection(&m_msgSection);
	int _progress = 0;
	// ����0ʱ��������
	if(_total > 0)
	{
		_progress = (int)(_transferred*(MAX_RANGE - MIN_RANGE)/(_total * STEP));
	}	
	return _progress;
}

// ��ȡ�ļ������ٶ�
string CSocketControl::GetSpeed()
{
	EnterCriticalSection(&m_msgSection);
	// �����ֵ
	long _diff = m_endOffset - m_startOffset;
	LeaveCriticalSection(&m_msgSection);
	long _ratio = 1000 / ELAPSE_TIME;
	long _speed = _diff * _ratio;
	string _strSpeed = "0";
	char speed[MAX_BUFF_LEN];
	// ��ʽ�����
	if(_speed < KB)
	{
		// �ֽ�/s
		sprintf_s(speed, MAX_BUFF_LEN, "%ld B/s", _speed);
	}
	else if(_speed >= KB && _speed < MB)
	{
		// KB/s
		double _tmp = (double)_speed / KB;
		sprintf_s(speed, MAX_BUFF_LEN, "%ld KB/s", _tmp);
	}
	else if(_speed >= MB && _speed < GB)
	{
		// MB/s
		double _tmp = (double)_speed / MB;
		sprintf_s(speed, MAX_BUFF_LEN, "%.2f MB", _tmp);
	}
	else if(_speed >= GB)
	{
		// MB/s
		double _tmp = (double)_speed / GB;
		sprintf_s(speed, MAX_BUFF_LEN, "%.2f GB", _tmp);
	}
	_strSpeed = speed;
	return _strSpeed;
}

// ��¼��ʼ��
void CSocketControl::SetStartOffset()
{
	EnterCriticalSection(&m_msgSection);
	this->m_startOffset = m_offset;
	LeaveCriticalSection(&m_msgSection);
}

// ���ش�����Ϣ
ErrorInfo CSocketControl::GetErrorInfo()
{
	return this->m_error;
}

// ��ȡ����Socket������
ClientMap CSocketControl::GetClients()
{
	return this->m_client;
}

/********************CSocketControl���캯������������*********************************/

/*---------------------------------------------------------------------------
���ܣ����캯����ʼ��
����: 
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
CSocketControl::CSocketControl()
{
	Init();
}

CSocketControl::~CSocketControl()
{
	// �ͷ��ڴ�
	delete m_model;
	this->m_model = NULL;
}


/********************CSocketControl��������*********************************/
/*---------------------------------------------------------------------------
���ܣ�Connect, �ͻ��˽�������
����: ��
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::Connect()
{
	this->m_model = new CSocketModel();
	if(m_model->BeginConnect(m_serverIP.c_str(), m_port, m_connectSocket))
	{
		// ���ӳɹ������Ŀ�����״̬
		m_state = CONNECTED;
		m_action = CONNECTING;
		// ����������Ϣ�߳�
		m_msgThread = AfxBeginThread(MessageProcessThread, this);
		if(m_msgThread == NULL)
		{
			CLog::Error(__FUNCTION__, __LINE__, MESSAGE_THREAD_FAIL);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, MESSAGE_THREAD_FAIL);
			return FALSE;
		}
	}
	else
	{
		CLog::Error(__FUNCTION__, __LINE__, CLIENT_CONNECT_FAIL);
		m_error.Error = FALSE;
		strcpy_s(m_error.msg, MAX_BUFF_LEN, CLIENT_CONNECT_FAIL);
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�ReceiveProcess, ������Ϣ����
����: ��
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::ReceiveProcess()
{
	SOCKET _socket = m_connectSocket;
	while(TRUE)
	{
		//�Ͽ����Ӻ󣬹ر���Ϣ�߳�
		if (m_state == INIT)						
		{
			AfxEndThread(0);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, STATE_ERROR);
			return FALSE;
		}
		else
		{
			// ��ʼ������Ϣ׼��
			Message _recvMsg;
			int _len = m_model->ReceiveMessage(_recvMsg, _socket);
			if (_len != SOCKET_ERROR)
			{
				// ��Ϣ������
				Handle(_recvMsg, _socket);
			}
			else	// Զ�̹ر�������
			{
				if(m_action == CONNECTING)
				{
					// ������ڴ����ļ��͹ر��ļ���
					if(m_state == TRANSFERING)
					{
						m_sendFile.Close();
					}
					m_state = CLOSED;
				}
				else if(m_action == LISTENING)
				{
					if(m_client.find(_socket) != m_client.end())
					{
						// ������ڴ����ļ��͹ر��ļ���
						if(m_client[_socket].state == TRANSFERING)
						{
							m_client[_socket].file.Close();
						}
						m_client[_socket].state = CLOSED;
					}
				}
				Sleep(1);
				break;
			}
		}
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�TransferFile�������ļ�������̵Ĵ���
����: ��
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::TransferFile()
{
	// �ȴ��ļ���Ϣ�ٴ��ļ�
	if (TransferFileInfo() == SOCKET_ERROR)
	{
		CLog::Error(__FUNCTION__, __LINE__, FILE_SEND_FAIL);
		m_error.Error = FALSE;
		strcpy_s(m_error.msg, MAX_BUFF_LEN, FILE_SEND_FAIL);
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�InterruptTransfer���ж��ļ�������̵Ĵ���
����: BOOL _positive TRUEΪ�����жϣ�FALSEΪ�����ж�
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::InterruptTransfer(SOCKET _socket, BOOL _positive)
{
	if(_socket == NULL)
	{
		_socket = m_connectSocket; // Ĭ��Socket
	}
	BOOL _fail = FALSE;
	if(_positive)
	{
		Message _sendMsg;
		memset(&_sendMsg, 0, sizeof(Message));
		// �����жϴ�������
		_sendMsg._messageType = INTERRUPT_TRANSFER;
		if (m_model->SendMessage(_sendMsg, _socket) == SOCKET_ERROR)
		{
			CLog::Error(__FUNCTION__, __LINE__, MESSAGE_SEND_FAIL);
			Sleep(1);
			_fail = TRUE;
		}
	}

	// �޸Ŀ�����״̬
	if(m_action == CONNECTING) //���Ӷ�
	{
		m_state = INTERRUPT_TRANSFER;
		m_offset = 0;
		memset(&m_fileInfo, 0, sizeof(FileInfo));
		m_sendFile.Close();
	}
	else if(m_action == LISTENING) //������
	{
		if(m_client.find(_socket) != m_client.end())
		{
			m_client[_socket].state = INTERRUPT_TRANSFER;
			m_client[_socket].offset = 0;
			memset(&m_client[_socket].fileInfo, 0, sizeof(FileInfo));
			m_client[_socket].file.Close();
		}
	}

	if(_fail)
	{
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�CloseConnect���ر����ӵĴ������
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::CloseConnect()
{
	return CloseProcess(CLOSE_CONNECT);
}


/*---------------------------------------------------------------------------
���ܣ�CloseListen���رռ����Ĵ������
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::CloseListen()
{
	return CloseProcess(CLOSE_LISTEN);
}


/*---------------------------------------------------------------------------
���ܣ�Listen, ����˿�������
����: ��
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::Listen()
{
	this->m_model = new CSocketModel();
	if(m_model->BeginListen(m_serverIP.c_str(), m_port, m_connectSocket, m_listenSocket))
	{
		// ���������ɹ�����¿�����״̬
		m_state = LISTENING;
		m_action = LISTENING;
		return BeginAccept();
	}
	else
	{
		CLog::Error(__FUNCTION__, __LINE__, SERVER_LISTEN_FAIL);
		m_error.Error = FALSE;
		strcpy_s(m_error.msg, MAX_BUFF_LEN, SERVER_LISTEN_FAIL);
		return FALSE;
	}
}


/*---------------------------------------------------------------------------
���ܣ�AcceptConnect, ����˿������տͻ���
����: ��
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::AcceptConnect()
{
	while(m_state != INIT && m_state != CLOSED && m_listenSocket != INVALID_SOCKET)
	{
		// ��������Ч����
		ClearClients();
		// �������ӹ���
		CClient _connect;
		// ��ʼ���տͻ���
		if(m_model->AcceptConnect(_connect.sin, m_connectSocket, m_listenSocket))
		{
			if(m_client.size() < m_maxConnect)		// С������������ͽ���
			{
				m_state = CONNECTED;
				_connect.state = CONNECTED;
				_connect.socket = m_connectSocket;
				// ����������Ϣ�߳�
				_connect.cthread = AfxBeginThread(MessageProcessThread, this);
				if(_connect.cthread == NULL)
				{
					CLog::Error(__FUNCTION__, __LINE__, MESSAGE_THREAD_FAIL);
					m_error.Error = FALSE;
					strcpy_s(m_error.msg, MAX_BUFF_LEN, MESSAGE_THREAD_FAIL);
					return FALSE;
				}
				m_client.insert(pair<SOCKET, CClient>(_connect.socket, _connect));
			}
			else
			{
				Message _sendMsg;
				memset(&_sendMsg, 0, sizeof(Message));
				// ���ͶϿ�����
				_sendMsg._messageType = CLOSE_LISTEN;
				// ֻ�跢������ȷ��
				m_model->SendMessage(_sendMsg, m_connectSocket);
				// ���������ֱ�ӹر�
				closesocket(m_connectSocket);	
			}	
		}
	}
	// ��ֹ�߳�
	AfxEndThread(0);
	return TRUE;
}



/********************CSocketControl˽�з���*********************************/
/*---------------------------------------------------------------------------
���ܣ�Init, ��ʼ����Ϣ
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CSocketControl::Init()
{
	this->m_state = INIT;
	this->m_offset = 0;
	this->m_maxConnect = 1;
}


/*---------------------------------------------------------------------------
���ܣ�Handle��ʵ�ʵ���Ϣ�������
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CSocketControl::Handle(Message _recvMsg, SOCKET _socket)
{
	MessageType _msgType = _recvMsg._messageType;
	switch (_msgType)
	{
	case RECV_FILE_INFO_ACK:									//�ͻ��˽��յ�����˷��͵��ļ���Ϣ���ճɹ��ź�
		m_state = TRANSFERING;									//���յ�ȷ����Ϣ���л�״̬
		m_offset = _recvMsg._data._transferData._offset;		//����ƫ��ֵ���ṩ�ϵ�����
		if (m_offset >= m_fileInfo._fileSize)					//ƫ��ֵ���ڵ����ļ���С��˵���ļ���ȫ������
		{
			// �������
			TransferOver(_socket);
		}
		else
		{
			if(!TransferFileData(_recvMsg))						//��ʼ�����ļ���
			{
				InterruptTransfer(_socket, FALSE);
				CLog::Error(__FUNCTION__, __LINE__, FILE_SEND_FAIL);
				m_error.Error = FALSE;
				strcpy_s(m_error.msg, MAX_BUFF_LEN, FILE_SEND_FAIL);
			}
		}
		break;
	case RECV_FILE_ACK:											//�����ļ���ɹ�
		if(!TransferFileData(_recvMsg))							//���������ļ�
		{
			InterruptTransfer(_socket, FALSE);
			CLog::Error(__FUNCTION__, __LINE__, FILE_SEND_FAIL);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, FILE_SEND_FAIL);
		}
		break;
	case INTERRUPT_TRANSFER:									//�жϴ���
		InterruptTransfer(_socket, FALSE);
		break;
	case CLOSE_LISTEN:											//�������رռ���
		if(!CloseConnect())
		{
			CLog::Error(__FUNCTION__, __LINE__, CLOSE_SOCKET_FAILE);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, CLOSE_SOCKET_FAILE);
		}
		break;
	case SEND_FILE_INFO_REQ:									//������յ��ͻ��˵ķ����ļ���Ϣ����
		if(!ReceiveFileInfo(_recvMsg, _socket))
		{
			CLog::Error(__FUNCTION__, __LINE__, SERVER_REPLY_FAIL);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, SERVER_REPLY_FAIL);
		}
		break;
	case SEND_FILE_REQ:											//������յ��ͻ��˵ķ����ļ���Ϣ����
		if(!ReceiveFileData(_recvMsg, _socket))
		{
			InterruptTransfer(_socket, FALSE);
			CLog::Error(__FUNCTION__, __LINE__, FILE_RECV_FAIL);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, FILE_RECV_FAIL);
		}
		break;
	case CLOSE_CONNECT:											//������յ��ͻ��˹ر���������
		m_state = CONNECTED;
		break;	
	default:
		break;
	}
}

/*---------------------------------------------------------------------------
���ܣ�TransferFileInfo�����ļ�������Ϣ���͸�server
����: ��
����ֵ�����ͳɹ�����ʵ�ʴ���Ĵ�С������ʧ�ܷ���SOKCET_ERROR
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::TransferFileInfo()
{
	Message _sendMsg;
	memset(&_sendMsg, 0, sizeof(Message));
	// ������Ϣ����Ϊ�������ļ���Ϣ
	_sendMsg._messageType = SEND_FILE_INFO_REQ;
	m_sendFile.InitFile(m_strFilePath.c_str(), m_offset, TRUE);
	// ȡ�ļ���Ϣ
	memset(&m_fileInfo, 0, sizeof(FileInfo));
	if(m_sendFile.GetFileInfo(m_fileInfo))
	{
		_sendMsg._data._fileInfo = m_fileInfo;
		if(m_model->SendMessage(_sendMsg, m_connectSocket))
		{
			// ��ʼ���߳���
			InitializeCriticalSection(&m_msgSection);
			return TRUE;
		}
		else
		{
			// �ļ���Ϣ����ʧ��
			CLog::Error(__FUNCTION__, __LINE__, FILE_INFO_ERROR);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, FILE_INFO_ERROR);
			return FALSE;
		}
	}
	return FALSE;
}

/*---------------------------------------------------------------------------
���ܣ�TransferFileData���������ݿ����ҵ��
����: ��
����ֵ���ɹ����ط����ļ���Ĵ�С��ʧ�ܷ���FALSE
ע�⣺�����ļ����С��һ�����ڿͻ���д����ļ����С
---------------------------------------------------------------------------*/
int CSocketControl::TransferFileData(Message _recvMsg)
{
	EnterCriticalSection(&m_msgSection);
	// ���·������������ļ�ƫ��λ�÷���
	m_offset = _recvMsg._data._transferData._offset;
	m_endOffset = m_offset;
	LeaveCriticalSection(&m_msgSection);

	if (m_offset >= m_fileInfo._fileSize)
	{
		//�������
		TransferOver(m_connectSocket);	
		return TRUE;
	}
	
	if ( (m_offset < m_fileInfo._fileSize) && (m_fileInfo._fileSize > 0) )
	{
		if (m_state != TRANSFERING)
		{
			return FALSE;
		}
		
		//��ȡһ���ļ�
		int _size = m_sendFile.Read(m_offset);

		if (_size == READ_ERROR)
		{
			CLog::Error(__FUNCTION__, __LINE__, MESSAGE_SEND_FAIL);
			return FALSE;
		}
		
		// ����������Ϣ����
		Message _sendMsg;
		memset(&_sendMsg, 0, sizeof(Message));
		_sendMsg._messageType					= SEND_FILE_REQ;
		_sendMsg._data._transferData._len		= _size;
		_sendMsg._data._transferData._offset	= m_offset;
		m_sendFile.GetBuff(_sendMsg._data._transferData._buf);
		
		//����һ���ļ�
		if (m_model->SendMessage(_sendMsg, m_connectSocket) == SOCKET_ERROR)				
		{
			CLog::Error(__FUNCTION__, __LINE__, FILE_SEND_FAIL);
			return FALSE;
		}
	}
	else
	{
		CLog::Error(__FUNCTION__, __LINE__, FILE_INFO_ERROR);
		return FALSE;
	}
	return TRUE;
}


/*---------------------------------------------------------------------------
���ܣ�ReceiveFileInfo����ʼ�����ļ���Ϣ
����: ��
����ֵ�����ͳɹ�����ʵ�ʴ���Ĵ�С������ʧ�ܷ���SOKCET_ERROR
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::ReceiveFileInfo(Message _recvMsg, SOCKET _socket)
{
	if(m_client.find(_socket) != m_client.end())
	{
		CClient &_connect = m_client[_socket];
		//�����ļ���Ϣ
		_connect.fileInfo = _recvMsg._data._fileInfo;	
		_connect.lastFileName = _recvMsg._data._fileInfo._name;
		// ƴ�ӷ���˵��ļ�·��
		_connect.filePath = m_strDirectory + "\\" + _recvMsg._data._fileInfo._name;
		Message _sendMsg;
		memset(&_sendMsg, 0, sizeof(Message));

		// ���ý����ļ���Ϣ�ɹ�
		_sendMsg._messageType = RECV_FILE_INFO_ACK;
		//�����ļ��Ķϵ�����λ����Ϣ
		_connect.file.InitFile(_connect.filePath.c_str(), 0, FALSE);
		_connect.offset = _connect.file.GetFileSize();
		_sendMsg._data._transferData._offset = _connect.offset;
		// �յ�����ͻ��˷�����Ϣ����
		if (m_model->SendMessage(_sendMsg, _socket) == SOCKET_ERROR)
		{
			CLog::Error(__FUNCTION__, __LINE__, FILE_SEND_FAIL);
			InterruptTransfer(_socket, FALSE);
			return FALSE;
		}
		else
		{
			// ���ƫ�ƴ��ڵ����ļ���С��˵���ѽ������
			if (_connect.offset >= _connect.fileInfo._fileSize)
			{
				TransferOver(_socket);
			}
			else
			{
				//����ȷ����Ϣ֮���л�״̬	
				_connect.state = TRANSFERING;
			}
		}
		// ��ʼ���߳���
		InitializeCriticalSection(&m_msgSection);
		return TRUE;
	}
	return FALSE;
}

/*---------------------------------------------------------------------------
���ܣ�ReceiveFileData�������ļ����ʵ�ʴ������
����: ��
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::ReceiveFileData(Message _recvMsg, SOCKET _socket)
{
	if(m_client.find(_socket) != m_client.end())
	{
		CClient &_connect = m_client[_socket];
		EnterCriticalSection(&m_msgSection);
		_connect.startOffset = _connect.offset;
		LeaveCriticalSection(&m_msgSection);
		// �ж��ļ��Ƿ���Է���
		if ( (_connect.offset < _connect.fileInfo._fileSize) && (_connect.fileInfo._fileSize > 0) )
		{
			if (_connect.state != TRANSFERING)
			{
				_connect.file.Close();
				return FALSE;
			}

			// ��ʼ���ļ�ģ��
			_connect.file.SetBuff(_recvMsg._data._transferData._buf, _recvMsg._data._transferData._len);

			// д������
			int _size = _connect.file.Write(_connect.offset);

			if (_size == WRITE_ERROR)
			{
				CLog::Error(__FUNCTION__, __LINE__, FILE_WRITE_FAIL);
				InterruptTransfer(_socket, TRUE);
				return FALSE;
			}

			EnterCriticalSection(&m_msgSection);
			// ���½����ļ�����
			_connect.offset = _connect.offset + _size;
			_connect.endOffset = _connect.offset;
			LeaveCriticalSection(&m_msgSection);

			Message _sendMsg;
			memset(&_sendMsg, 0, sizeof(Message));
			_sendMsg._messageType = RECV_FILE_ACK;
			_sendMsg._data._transferData._offset = _connect.offset;
			// �������յ�����Ϣ
			if (m_model->SendMessage(_sendMsg, _socket) == SOCKET_ERROR)
			{
				CLog::Error(__FUNCTION__, __LINE__, MESSAGE_SEND_FAIL);
				InterruptTransfer(_socket, FALSE);
				return FALSE;
			}

			//���յ��ļ������һ��Ȼ�����
			if (_connect.offset >= _connect.fileInfo._fileSize)		
			{
				TransferOver(_socket);
				return TRUE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

/*---------------------------------------------------------------------------
���ܣ�BeginAccept�����������ͻ���
����: ��
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::BeginAccept()
{
	// ���տͻ����߳�
	m_acceptThread = AfxBeginThread(AcceptConnectThread, this);
	if(m_acceptThread == NULL)
	{
		CLog::Error(__FUNCTION__, __LINE__, ACCEPT_THREAD_FAIL);
		m_error.Error = FALSE;
		strcpy_s(m_error.msg, MAX_BUFF_LEN, ACCEPT_THREAD_FAIL);
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�CloseProcess���ر����ӵ�ʵ�ʴ������
����: int _state ������״̬
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketControl::CloseProcess(MessageType _state)
{
	if(m_state == INIT)
	{
		return TRUE;
	}
	//�ر��׽���
	if(m_connectSocket != INVALID_SOCKET)
	{
		m_model->CloseConnect(m_connectSocket);
	}
	if(m_listenSocket != INVALID_SOCKET)
	{
		m_model->CloseConnect(m_listenSocket);
	}
	//�������
	if(m_action == LISTENING)
	{
		DWORD dwExitCode = 0;
		for(ClientMap::iterator it = m_client.begin(); it != m_client.end(); it++)
		{
			// �ر��߳�
			GetExitCodeThread(it->second.cthread, &dwExitCode);
			if(dwExitCode == STILL_ACTIVE)
			{
				TerminateThread(it->second.cthread, -1);
			}
		}
		m_client.clear();
	}
	
	//�ر��߳�
	DWORD dwExitCode = 0;
	GetExitCodeThread(m_msgThread, &dwExitCode);
	if(dwExitCode == STILL_ACTIVE)
	{
		TerminateThread(m_msgThread, -1);
	}
	dwExitCode = 0;
	GetExitCodeThread(m_msgThread, &dwExitCode);
	if(dwExitCode == STILL_ACTIVE)
	{
		TerminateThread(m_acceptThread, -1);
	}
	delete m_model;
	this->m_model = NULL;
	Init();
	// ������Ϣ
	m_state = CLOSED;
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�TransferOver���ļ�����������ʼ��
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CSocketControl::TransferOver(SOCKET _socket)
{
	m_state = COMPLETED;
	if(m_action == CONNECTING)			// ���Ӷ�
	{
		m_sendFile.Close();
		m_offset = 0;
		memset(&m_fileInfo, 0, sizeof(FileInfo));
	}
	else if(m_action == LISTENING)		// ������
	{
		// ��ն�ӦSocket������
		if(m_client.find(_socket) != m_client.end())
		{
			m_client[_socket].file.Close();
			m_client[_socket].offset = 0;
			memset(&m_client[_socket].fileInfo, 0, sizeof(FileInfo));
			m_client[_socket].state = COMPLETED;
		}
	}
}


/*---------------------------------------------------------------------------
���ܣ�ClearClients��������Ч������
����: ��
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CSocketControl::ClearClients()
{
	vector<SOCKET> _sockets;
	DWORD dwExitCode = 0;
	for(ClientMap::iterator it = m_client.begin(); it != m_client.end(); it++)
	{
		if(it->second.state == CLOSED)
		{
			// �ر��߳�
			GetExitCodeThread(it->second.cthread, &dwExitCode);
			if(dwExitCode == STILL_ACTIVE)
			{
				TerminateThread(it->second.cthread, -1);
			}
			_sockets.push_back(it->first);
		}	
	}
	for(int i=0; i<_sockets.size(); i++)
	{
		m_client.erase(_sockets[i]);
	}
}