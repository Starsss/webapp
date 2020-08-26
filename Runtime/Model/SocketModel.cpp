/******************************************************************
模块名：CSocketModel
文　件：SocketModel.h, SocketModel.cpp
创建人：Wang WenJie
创建日期：2019-7-30
版  本：v1.1.0
功能说明：网络应用程序网络模型实现文件，实现连接、监听、发送、接收、中断和关闭功能
*******************************************************************/

#include "stdafx.h"
#include "SocketModel.h"
#include "../Extend/Log.h"
#include "../Config/AppString.h"

/********************CSocketModel构造函数和析构函数*********************************/

CSocketModel::CSocketModel()
{
	// 初始化日志
	InitLog();
}

CSocketModel::~CSocketModel()
{

}


/***********************************************************************/
/************************CSocketModel的公共方法*******************************/
/***********************************************************************/

/*---------------------------------------------------------------------------
功能：BeginConnect，开启TCP连接处理
参数: const char* _serverIP 服务端地址, int _port 服务端端口, SOCKET &_connectSocket 用于连接的SOCKET
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketModel::BeginConnect(const char* _serverIP, int _port, SOCKET &_connectSocket)
{
	// 初始化SOCKET信息
	if(FALSE == InitSocket(_connectSocket))
	{
		return FALSE;
	}
	SOCKADDR_IN _sin;
	_sin.sin_family			= AF_INET;
	_sin.sin_port			= htons(_port);
	_sin.sin_addr.s_addr	= inet_addr(_serverIP);

	// 开始建立连接
	if (connect(_connectSocket, (SOCKADDR *)&_sin, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		CLog::Error(__FUNCTION__, __LINE__, CLIENT_CONNECT_FAIL);
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
功能：BeginListen，开启TCP监听处理
参数: const char* _serverIP 服务端地址, int _port 服务端端口,int _maxConnect 最大连接数, SOCKET &_connectSocket 连接进来的SOCKET, SOCKET &_listenSokcet 用于监听的SOCKET
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketModel::BeginListen(const char* _serverIP, int _port, SOCKET &_connectSocket, SOCKET &_listenSokcet)
{
	// 初始化SOCKET信息
	if(FALSE == InitSocket(_listenSokcet))
	{
		return FALSE;
	}
	SOCKADDR_IN _sin;
	_sin.sin_family			= AF_INET;
	_sin.sin_addr.s_addr	= htonl(INADDR_ANY);
	_sin.sin_port			= htons(_port);

	// 服务器绑定
	if (bind(_listenSokcet,(sockaddr *)&_sin,sizeof(sockaddr)) == SOCKET_ERROR )
	{
		CLog::Error(__FUNCTION__, __LINE__, SERVER_BIND_FAIL);
		return FALSE;
	}

	// 服务器开启监听
	if ( listen(_listenSokcet, SOMAXCONN) < 0 )
	{
		CLog::Error(__FUNCTION__, __LINE__, SERVER_LISTEN_FAIL);
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
功能：ReceiveMessage，接收消息处理过程
参数: Message &_msg待接收的消息, SOCKET _connectSocket 接收消息的套接字
返回值：接收的消息大小
注意：
---------------------------------------------------------------------------*/
int CSocketModel::ReceiveMessage(Message &_msg, SOCKET _connectSocket)
{
	memset(&_msg, 0, sizeof(Message));
	return recv(_connectSocket, (char *)(&_msg), sizeof(Message), 0);
}

/*---------------------------------------------------------------------------
功能：SendMessage，发送消息
参数: Message &_msg, 待发送的消息内容, SOCKET _connectSocket 用于发送的套接字
返回值：发送成功返回实际发送的消息长度，发送失败返回SOCKET_ERROR
注意：
---------------------------------------------------------------------------*/
int CSocketModel::SendMessage(Message _msg, SOCKET _connectSocket)
{
	return send(_connectSocket, (const char *)(&_msg), sizeof(Message), 0);
}


/*---------------------------------------------------------------------------
功能：CloseConnect，关闭连接的处理过程
参数: SOCKET &_socket 要关闭的套接字
返回值：成功返回TRUE
注意：
---------------------------------------------------------------------------*/
int CSocketModel::CloseConnect(SOCKET &_socket)
{
	closesocket(_socket);
	WSACleanup();
	_socket = INVALID_SOCKET;
	return TRUE;
}

/*---------------------------------------------------------------------------
功能：AcceptConnect，开始接收连接
参数: SOCKADDR_IN & _sin, SOCKET &_connectSocket 接收到的套接字, SOCKET _listenSocket 用于监听的套接字
返回值：成功返回TRUE，失败返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketModel::AcceptConnect(SOCKADDR_IN & _sin, SOCKET &_connectSocket, SOCKET _listenSocket)
{
	int _size = sizeof(SOCKADDR_IN);
	// 开启接收
	if( (_connectSocket = accept(_listenSocket, (sockaddr *)&_sin, &_size)) == SOCKET_ERROR )
	{
		return FALSE;
	}
	return TRUE;
}



/***********************************************************************/
/************************CSocketModel的私有方法*******************************/
/***********************************************************************/
/*---------------------------------------------------------------------------
功能：InitClientSocket，初始化套接字
参数: SOCKET & _sock, 待初始化的套接字对象
返回值：初始化成功返回TRUE，否则返回FALSE
注意：
---------------------------------------------------------------------------*/
int CSocketModel::InitSocket(SOCKET &_sock)
{
	WSADATA WSAData;
	// 版本检测
	if (WSAStartup(MAKEWORD(2,2), &WSAData) != 0)
	{
		CLog::Error(__FUNCTION__, __LINE__, SOCKET_INIT_FAIL);
		return FALSE;
	}
	// 创建套接字
	if ((_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR )
	{
		WSACleanup();
		CLog::Error(__FUNCTION__, __LINE__, SOCKET_CREATE_FAIL);
		return FALSE;
	}
	return TRUE;
}