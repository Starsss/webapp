/******************************************************************
模块名：CSocketControl
文　件：CSocketControl.h, CSocketControl.cpp	
创建人：Wang WenJie
创建日期：2019-8-2
版  本：v1.2.0
功能说明：网络应用程序控制器实现文件
*******************************************************************/

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SocketControl.h"
#include "../Model/SocketModel.h"
#include "../Extend/Log.h"
#include "../Model/FileModel.h"
#include "../Config/AppString.h"



/********************全局线程方法*********************************/
/*---------------------------------------------------------------------------
功能：MessageProcessThread，消息线程处理过程入口点
参数: LPVOID 当前模型
返回值：UINT
注意：
---------------------------------------------------------------------------*/
UINT MessageProcessThread(LPVOID lpParam)
{
	CSocketControl *_control = (CSocketControl *)lpParam;
	_control->ReceiveProcess();
	return TRUE;
}

/*---------------------------------------------------------------------------
功能：AcceptConnectThread，消息线程处理过程入口点
参数: LPVOID 当前模型
返回值：UINT
注意：
---------------------------------------------------------------------------*/
UINT AcceptConnectThread(LPVOID lpParam)
{
	CSocketControl *_control = (CSocketControl *)lpParam;
	_control->AcceptConnect();
	return TRUE;
}

/**************************CSocketControl的Get和Set方法***************************/
// 获取控制器状态
int CSocketControl::GetState()
{
	return m_state;
}

// 设置服务端地址
void CSocketControl::SetServerIP(const char* _ip)
{
	this->m_serverIP = _ip;
}

// 设置端口号
void CSocketControl::SetPort(int _port)
{
	this->m_port = _port;
}

// 设置最大连接数
void CSocketControl::SetMaxConnect(int _maxConnect)
{
	this->m_maxConnect = _maxConnect;
}

// 设置文件路径
void CSocketControl::SetFilePath(const char* _filePath)
{
	this->m_strFilePath = _filePath;
}

// 设置文件目录
void CSocketControl::SetDirectory(const char* _directory)
{
	this->m_strDirectory = _directory;
}

// 获取文件传输进度
int CSocketControl::GetProgress()
{
	long _total = m_fileInfo._fileSize / BLOCK_SIZE;
	EnterCriticalSection(&m_msgSection);
	long _transferred = m_offset / BLOCK_SIZE;
	LeaveCriticalSection(&m_msgSection);
	int _progress = 0;
	// 大于0时才有意义
	if(_total > 0)
	{
		_progress = (int)(_transferred*(MAX_RANGE - MIN_RANGE)/(_total * STEP));
	}	
	return _progress;
}

// 获取文件传输速度
string CSocketControl::GetSpeed()
{
	EnterCriticalSection(&m_msgSection);
	// 计算差值
	long _diff = m_endOffset - m_startOffset;
	LeaveCriticalSection(&m_msgSection);
	long _ratio = 1000 / ELAPSE_TIME;
	long _speed = _diff * _ratio;
	string _strSpeed = "0";
	char speed[MAX_BUFF_LEN];
	// 格式化输出
	if(_speed < KB)
	{
		// 字节/s
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

// 记录起始点
void CSocketControl::SetStartOffset()
{
	EnterCriticalSection(&m_msgSection);
	this->m_startOffset = m_offset;
	LeaveCriticalSection(&m_msgSection);
}

// 返回错误信息
ErrorInfo CSocketControl::GetErrorInfo()
{
	return this->m_error;
}

// 获取管理Socket的容器
ClientMap CSocketControl::GetClients()
{
	return this->m_client;
}

/********************CSocketControl构造函数和析构函数*********************************/

/*---------------------------------------------------------------------------
功能：构造函数初始化
参数: 
返回值：无
注意：
---------------------------------------------------------------------------*/
CSocketControl::CSocketControl()
{
	Init();
}

CSocketControl::~CSocketControl()
{
	// 释放内存
	delete m_model;
	this->m_model = NULL;
}


/********************CSocketControl公共方法*********************************/
/*---------------------------------------------------------------------------
功能：Connect, 客户端建立连接
参数: 无
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketControl::Connect()
{
	this->m_model = new CSocketModel();
	if(m_model->BeginConnect(m_serverIP.c_str(), m_port, m_connectSocket))
	{
		// 连接成功，更改控制器状态
		m_state = CONNECTED;
		m_action = CONNECTING;
		// 开启接收消息线程
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
功能：ReceiveProcess, 开启信息接收
参数: 无
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketControl::ReceiveProcess()
{
	SOCKET _socket = m_connectSocket;
	while(TRUE)
	{
		//断开连接后，关闭消息线程
		if (m_state == INIT)						
		{
			AfxEndThread(0);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, STATE_ERROR);
			return FALSE;
		}
		else
		{
			// 开始接收消息准备
			Message _recvMsg;
			int _len = m_model->ReceiveMessage(_recvMsg, _socket);
			if (_len != SOCKET_ERROR)
			{
				// 消息处理句柄
				Handle(_recvMsg, _socket);
			}
			else	// 远程关闭了连接
			{
				if(m_action == CONNECTING)
				{
					// 如果正在传输文件就关闭文件流
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
						// 如果正在传输文件就关闭文件流
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
功能：TransferFile，开启文件传输过程的处理
参数: 无
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketControl::TransferFile()
{
	// 先传文件信息再传文件
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
功能：InterruptTransfer，中断文件传输过程的处理
参数: BOOL _positive TRUE为主动中断，FALSE为被动中断
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketControl::InterruptTransfer(SOCKET _socket, BOOL _positive)
{
	if(_socket == NULL)
	{
		_socket = m_connectSocket; // 默认Socket
	}
	BOOL _fail = FALSE;
	if(_positive)
	{
		Message _sendMsg;
		memset(&_sendMsg, 0, sizeof(Message));
		// 传送中断传输请求
		_sendMsg._messageType = INTERRUPT_TRANSFER;
		if (m_model->SendMessage(_sendMsg, _socket) == SOCKET_ERROR)
		{
			CLog::Error(__FUNCTION__, __LINE__, MESSAGE_SEND_FAIL);
			Sleep(1);
			_fail = TRUE;
		}
	}

	// 修改控制器状态
	if(m_action == CONNECTING) //连接端
	{
		m_state = INTERRUPT_TRANSFER;
		m_offset = 0;
		memset(&m_fileInfo, 0, sizeof(FileInfo));
		m_sendFile.Close();
	}
	else if(m_action == LISTENING) //监听端
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
功能：CloseConnect，关闭连接的处理过程
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
int CSocketControl::CloseConnect()
{
	return CloseProcess(CLOSE_CONNECT);
}


/*---------------------------------------------------------------------------
功能：CloseListen，关闭监听的处理过程
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
int CSocketControl::CloseListen()
{
	return CloseProcess(CLOSE_LISTEN);
}


/*---------------------------------------------------------------------------
功能：Listen, 服务端开启监听
参数: 无
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketControl::Listen()
{
	this->m_model = new CSocketModel();
	if(m_model->BeginListen(m_serverIP.c_str(), m_port, m_connectSocket, m_listenSocket))
	{
		// 开启监听成功后更新控制器状态
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
功能：AcceptConnect, 服务端开启接收客户端
参数: 无
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketControl::AcceptConnect()
{
	while(m_state != INIT && m_state != CLOSED && m_listenSocket != INVALID_SOCKET)
	{
		// 先清理无效连接
		ClearClients();
		// 创建连接管理
		CClient _connect;
		// 开始接收客户端
		if(m_model->AcceptConnect(_connect.sin, m_connectSocket, m_listenSocket))
		{
			if(m_client.size() < m_maxConnect)		// 小于最大连接数就接收
			{
				m_state = CONNECTED;
				_connect.state = CONNECTED;
				_connect.socket = m_connectSocket;
				// 开启接收消息线程
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
				// 传送断开请求
				_sendMsg._messageType = CLOSE_LISTEN;
				// 只需发送无需确认
				m_model->SendMessage(_sendMsg, m_connectSocket);
				// 多余的连接直接关闭
				closesocket(m_connectSocket);	
			}	
		}
	}
	// 终止线程
	AfxEndThread(0);
	return TRUE;
}



/********************CSocketControl私有方法*********************************/
/*---------------------------------------------------------------------------
功能：Init, 初始化信息
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CSocketControl::Init()
{
	this->m_state = INIT;
	this->m_offset = 0;
	this->m_maxConnect = 1;
}


/*---------------------------------------------------------------------------
功能：Handle，实际的消息处理过程
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CSocketControl::Handle(Message _recvMsg, SOCKET _socket)
{
	MessageType _msgType = _recvMsg._messageType;
	switch (_msgType)
	{
	case RECV_FILE_INFO_ACK:									//客户端接收到服务端发送的文件信息接收成功信号
		m_state = TRANSFERING;									//接收到确认信息后切换状态
		m_offset = _recvMsg._data._transferData._offset;		//保存偏移值，提供断点续传
		if (m_offset >= m_fileInfo._fileSize)					//偏移值大于等于文件大小，说明文件已全部发送
		{
			// 发送完毕
			TransferOver(_socket);
		}
		else
		{
			if(!TransferFileData(_recvMsg))						//开始传输文件块
			{
				InterruptTransfer(_socket, FALSE);
				CLog::Error(__FUNCTION__, __LINE__, FILE_SEND_FAIL);
				m_error.Error = FALSE;
				strcpy_s(m_error.msg, MAX_BUFF_LEN, FILE_SEND_FAIL);
			}
		}
		break;
	case RECV_FILE_ACK:											//接收文件块成功
		if(!TransferFileData(_recvMsg))							//继续传输文件
		{
			InterruptTransfer(_socket, FALSE);
			CLog::Error(__FUNCTION__, __LINE__, FILE_SEND_FAIL);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, FILE_SEND_FAIL);
		}
		break;
	case INTERRUPT_TRANSFER:									//中断传输
		InterruptTransfer(_socket, FALSE);
		break;
	case CLOSE_LISTEN:											//服务器关闭监听
		if(!CloseConnect())
		{
			CLog::Error(__FUNCTION__, __LINE__, CLOSE_SOCKET_FAILE);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, CLOSE_SOCKET_FAILE);
		}
		break;
	case SEND_FILE_INFO_REQ:									//服务端收到客户端的发送文件信息请求
		if(!ReceiveFileInfo(_recvMsg, _socket))
		{
			CLog::Error(__FUNCTION__, __LINE__, SERVER_REPLY_FAIL);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, SERVER_REPLY_FAIL);
		}
		break;
	case SEND_FILE_REQ:											//服务端收到客户端的发送文件信息请求
		if(!ReceiveFileData(_recvMsg, _socket))
		{
			InterruptTransfer(_socket, FALSE);
			CLog::Error(__FUNCTION__, __LINE__, FILE_RECV_FAIL);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, FILE_RECV_FAIL);
		}
		break;
	case CLOSE_CONNECT:											//服务端收到客户端关闭连接请求
		m_state = CONNECTED;
		break;	
	default:
		break;
	}
}

/*---------------------------------------------------------------------------
功能：TransferFileInfo，将文件基本信息发送给server
参数: 无
返回值：发送成功返回实际传输的大小，发送失败返回SOKCET_ERROR
注意：
---------------------------------------------------------------------------*/
int CSocketControl::TransferFileInfo()
{
	Message _sendMsg;
	memset(&_sendMsg, 0, sizeof(Message));
	// 设置消息类型为请求发送文件信息
	_sendMsg._messageType = SEND_FILE_INFO_REQ;
	m_sendFile.InitFile(m_strFilePath.c_str(), m_offset, TRUE);
	// 取文件信息
	memset(&m_fileInfo, 0, sizeof(FileInfo));
	if(m_sendFile.GetFileInfo(m_fileInfo))
	{
		_sendMsg._data._fileInfo = m_fileInfo;
		if(m_model->SendMessage(_sendMsg, m_connectSocket))
		{
			// 初始化线程锁
			InitializeCriticalSection(&m_msgSection);
			return TRUE;
		}
		else
		{
			// 文件信息发送失败
			CLog::Error(__FUNCTION__, __LINE__, FILE_INFO_ERROR);
			m_error.Error = FALSE;
			strcpy_s(m_error.msg, MAX_BUFF_LEN, FILE_INFO_ERROR);
			return FALSE;
		}
	}
	return FALSE;
}

/*---------------------------------------------------------------------------
功能：TransferFileData，发送数据块具体业务
参数: 无
返回值：成功返回发送文件块的大小，失败返回FALSE
注意：发送文件块大小不一定等于客户端写入的文件块大小
---------------------------------------------------------------------------*/
int CSocketControl::TransferFileData(Message _recvMsg)
{
	EnterCriticalSection(&m_msgSection);
	// 更新服务器发来的文件偏移位置反馈
	m_offset = _recvMsg._data._transferData._offset;
	m_endOffset = m_offset;
	LeaveCriticalSection(&m_msgSection);

	if (m_offset >= m_fileInfo._fileSize)
	{
		//发送完毕
		TransferOver(m_connectSocket);	
		return TRUE;
	}
	
	if ( (m_offset < m_fileInfo._fileSize) && (m_fileInfo._fileSize > 0) )
	{
		if (m_state != TRANSFERING)
		{
			return FALSE;
		}
		
		//读取一块文件
		int _size = m_sendFile.Read(m_offset);

		if (_size == READ_ERROR)
		{
			CLog::Error(__FUNCTION__, __LINE__, MESSAGE_SEND_FAIL);
			return FALSE;
		}
		
		// 构建发送消息内容
		Message _sendMsg;
		memset(&_sendMsg, 0, sizeof(Message));
		_sendMsg._messageType					= SEND_FILE_REQ;
		_sendMsg._data._transferData._len		= _size;
		_sendMsg._data._transferData._offset	= m_offset;
		m_sendFile.GetBuff(_sendMsg._data._transferData._buf);
		
		//发送一块文件
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
功能：ReceiveFileInfo，开始接收文件信息
参数: 无
返回值：发送成功返回实际传输的大小，发送失败返回SOKCET_ERROR
注意：
---------------------------------------------------------------------------*/
int CSocketControl::ReceiveFileInfo(Message _recvMsg, SOCKET _socket)
{
	if(m_client.find(_socket) != m_client.end())
	{
		CClient &_connect = m_client[_socket];
		//保存文件信息
		_connect.fileInfo = _recvMsg._data._fileInfo;	
		_connect.lastFileName = _recvMsg._data._fileInfo._name;
		// 拼接服务端的文件路径
		_connect.filePath = m_strDirectory + "\\" + _recvMsg._data._fileInfo._name;
		Message _sendMsg;
		memset(&_sendMsg, 0, sizeof(Message));

		// 设置接收文件信息成功
		_sendMsg._messageType = RECV_FILE_INFO_ACK;
		//发送文件的断点续传位置信息
		_connect.file.InitFile(_connect.filePath.c_str(), 0, FALSE);
		_connect.offset = _connect.file.GetFileSize();
		_sendMsg._data._transferData._offset = _connect.offset;
		// 收到后给客户端发送消息反馈
		if (m_model->SendMessage(_sendMsg, _socket) == SOCKET_ERROR)
		{
			CLog::Error(__FUNCTION__, __LINE__, FILE_SEND_FAIL);
			InterruptTransfer(_socket, FALSE);
			return FALSE;
		}
		else
		{
			// 如果偏移大于等于文件大小，说明已接收完成
			if (_connect.offset >= _connect.fileInfo._fileSize)
			{
				TransferOver(_socket);
			}
			else
			{
				//发送确认信息之后切换状态	
				_connect.state = TRANSFERING;
			}
		}
		// 初始化线程锁
		InitializeCriticalSection(&m_msgSection);
		return TRUE;
	}
	return FALSE;
}

/*---------------------------------------------------------------------------
功能：ReceiveFileData，接收文件块的实际处理过程
参数: 无
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketControl::ReceiveFileData(Message _recvMsg, SOCKET _socket)
{
	if(m_client.find(_socket) != m_client.end())
	{
		CClient &_connect = m_client[_socket];
		EnterCriticalSection(&m_msgSection);
		_connect.startOffset = _connect.offset;
		LeaveCriticalSection(&m_msgSection);
		// 判断文件是否可以发送
		if ( (_connect.offset < _connect.fileInfo._fileSize) && (_connect.fileInfo._fileSize > 0) )
		{
			if (_connect.state != TRANSFERING)
			{
				_connect.file.Close();
				return FALSE;
			}

			// 初始化文件模型
			_connect.file.SetBuff(_recvMsg._data._transferData._buf, _recvMsg._data._transferData._len);

			// 写入数据
			int _size = _connect.file.Write(_connect.offset);

			if (_size == WRITE_ERROR)
			{
				CLog::Error(__FUNCTION__, __LINE__, FILE_WRITE_FAIL);
				InterruptTransfer(_socket, TRUE);
				return FALSE;
			}

			EnterCriticalSection(&m_msgSection);
			// 更新接收文件进度
			_connect.offset = _connect.offset + _size;
			_connect.endOffset = _connect.offset;
			LeaveCriticalSection(&m_msgSection);

			Message _sendMsg;
			memset(&_sendMsg, 0, sizeof(Message));
			_sendMsg._messageType = RECV_FILE_ACK;
			_sendMsg._data._transferData._offset = _connect.offset;
			// 反馈接收到的信息
			if (m_model->SendMessage(_sendMsg, _socket) == SOCKET_ERROR)
			{
				CLog::Error(__FUNCTION__, __LINE__, MESSAGE_SEND_FAIL);
				InterruptTransfer(_socket, FALSE);
				return FALSE;
			}

			//接收到文件的最后一块然后结束
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
功能：BeginAccept，开启监听客户端
参数: 无
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketControl::BeginAccept()
{
	// 接收客户端线程
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
功能：CloseProcess，关闭连接的实际处理过程
参数: int _state 控制器状态
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketControl::CloseProcess(MessageType _state)
{
	if(m_state == INIT)
	{
		return TRUE;
	}
	//关闭套接字
	if(m_connectSocket != INVALID_SOCKET)
	{
		m_model->CloseConnect(m_connectSocket);
	}
	if(m_listenSocket != INVALID_SOCKET)
	{
		m_model->CloseConnect(m_listenSocket);
	}
	//清空容器
	if(m_action == LISTENING)
	{
		DWORD dwExitCode = 0;
		for(ClientMap::iterator it = m_client.begin(); it != m_client.end(); it++)
		{
			// 关闭线程
			GetExitCodeThread(it->second.cthread, &dwExitCode);
			if(dwExitCode == STILL_ACTIVE)
			{
				TerminateThread(it->second.cthread, -1);
			}
		}
		m_client.clear();
	}
	
	//关闭线程
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
	// 返回消息
	m_state = CLOSED;
	return TRUE;
}

/*---------------------------------------------------------------------------
功能：TransferOver，文件传输结束后初始化
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CSocketControl::TransferOver(SOCKET _socket)
{
	m_state = COMPLETED;
	if(m_action == CONNECTING)			// 连接端
	{
		m_sendFile.Close();
		m_offset = 0;
		memset(&m_fileInfo, 0, sizeof(FileInfo));
	}
	else if(m_action == LISTENING)		// 监听端
	{
		// 清空对应Socket的内容
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
功能：ClearClients，清理无效的连接
参数: 无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CSocketControl::ClearClients()
{
	vector<SOCKET> _sockets;
	DWORD dwExitCode = 0;
	for(ClientMap::iterator it = m_client.begin(); it != m_client.end(); it++)
	{
		if(it->second.state == CLOSED)
		{
			// 关闭线程
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