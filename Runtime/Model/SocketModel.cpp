/******************************************************************
ģ������CSocketModel
�ġ�����SocketModel.h, SocketModel.cpp
�����ˣ�Wang WenJie
�������ڣ�2019-7-30
��  ����v1.1.0
����˵��������Ӧ�ó�������ģ��ʵ���ļ���ʵ�����ӡ����������͡����ա��жϺ͹رչ���
*******************************************************************/

#include "stdafx.h"
#include "SocketModel.h"
#include "../Extend/Log.h"
#include "../Config/AppString.h"

/********************CSocketModel���캯������������*********************************/

CSocketModel::CSocketModel()
{
	// ��ʼ����־
	InitLog();
}

CSocketModel::~CSocketModel()
{

}


/***********************************************************************/
/************************CSocketModel�Ĺ�������*******************************/
/***********************************************************************/

/*---------------------------------------------------------------------------
���ܣ�BeginConnect������TCP���Ӵ���
����: const char* _serverIP ����˵�ַ, int _port ����˶˿�, SOCKET &_connectSocket �������ӵ�SOCKET
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketModel::BeginConnect(const char* _serverIP, int _port, SOCKET &_connectSocket)
{
	// ��ʼ��SOCKET��Ϣ
	if(FALSE == InitSocket(_connectSocket))
	{
		return FALSE;
	}
	SOCKADDR_IN _sin;
	_sin.sin_family			= AF_INET;
	_sin.sin_port			= htons(_port);
	_sin.sin_addr.s_addr	= inet_addr(_serverIP);

	// ��ʼ��������
	if (connect(_connectSocket, (SOCKADDR *)&_sin, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		CLog::Error(__FUNCTION__, __LINE__, CLIENT_CONNECT_FAIL);
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�BeginListen������TCP��������
����: const char* _serverIP ����˵�ַ, int _port ����˶˿�,int _maxConnect ���������, SOCKET &_connectSocket ���ӽ�����SOCKET, SOCKET &_listenSokcet ���ڼ�����SOCKET
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketModel::BeginListen(const char* _serverIP, int _port, SOCKET &_connectSocket, SOCKET &_listenSokcet)
{
	// ��ʼ��SOCKET��Ϣ
	if(FALSE == InitSocket(_listenSokcet))
	{
		return FALSE;
	}
	SOCKADDR_IN _sin;
	_sin.sin_family			= AF_INET;
	_sin.sin_addr.s_addr	= htonl(INADDR_ANY);
	_sin.sin_port			= htons(_port);

	// ��������
	if (bind(_listenSokcet,(sockaddr *)&_sin,sizeof(sockaddr)) == SOCKET_ERROR )
	{
		CLog::Error(__FUNCTION__, __LINE__, SERVER_BIND_FAIL);
		return FALSE;
	}

	// ��������������
	if ( listen(_listenSokcet, SOMAXCONN) < 0 )
	{
		CLog::Error(__FUNCTION__, __LINE__, SERVER_LISTEN_FAIL);
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�ReceiveMessage��������Ϣ�������
����: Message &_msg�����յ���Ϣ, SOCKET _connectSocket ������Ϣ���׽���
����ֵ�����յ���Ϣ��С
ע�⣺
---------------------------------------------------------------------------*/
int CSocketModel::ReceiveMessage(Message &_msg, SOCKET _connectSocket)
{
	memset(&_msg, 0, sizeof(Message));
	return recv(_connectSocket, (char *)(&_msg), sizeof(Message), 0);
}

/*---------------------------------------------------------------------------
���ܣ�SendMessage��������Ϣ
����: Message &_msg, �����͵���Ϣ����, SOCKET _connectSocket ���ڷ��͵��׽���
����ֵ�����ͳɹ�����ʵ�ʷ��͵���Ϣ���ȣ�����ʧ�ܷ���SOCKET_ERROR
ע�⣺
---------------------------------------------------------------------------*/
int CSocketModel::SendMessage(Message _msg, SOCKET _connectSocket)
{
	return send(_connectSocket, (const char *)(&_msg), sizeof(Message), 0);
}


/*---------------------------------------------------------------------------
���ܣ�CloseConnect���ر����ӵĴ������
����: SOCKET &_socket Ҫ�رյ��׽���
����ֵ���ɹ�����TRUE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketModel::CloseConnect(SOCKET &_socket)
{
	closesocket(_socket);
	WSACleanup();
	_socket = INVALID_SOCKET;
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�AcceptConnect����ʼ��������
����: SOCKADDR_IN & _sin, SOCKET &_connectSocket ���յ����׽���, SOCKET _listenSocket ���ڼ������׽���
����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketModel::AcceptConnect(SOCKADDR_IN & _sin, SOCKET &_connectSocket, SOCKET _listenSocket)
{
	int _size = sizeof(SOCKADDR_IN);
	// ��������
	if( (_connectSocket = accept(_listenSocket, (sockaddr *)&_sin, &_size)) == SOCKET_ERROR )
	{
		return FALSE;
	}
	return TRUE;
}



/***********************************************************************/
/************************CSocketModel��˽�з���*******************************/
/***********************************************************************/
/*---------------------------------------------------------------------------
���ܣ�InitClientSocket����ʼ���׽���
����: SOCKET & _sock, ����ʼ�����׽��ֶ���
����ֵ����ʼ���ɹ�����TRUE�����򷵻�FALSE
ע�⣺
---------------------------------------------------------------------------*/
int CSocketModel::InitSocket(SOCKET &_sock)
{
	WSADATA WSAData;
	// �汾���
	if (WSAStartup(MAKEWORD(2,2), &WSAData) != 0)
	{
		CLog::Error(__FUNCTION__, __LINE__, SOCKET_INIT_FAIL);
		return FALSE;
	}
	// �����׽���
	if ((_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR )
	{
		WSACleanup();
		CLog::Error(__FUNCTION__, __LINE__, SOCKET_CREATE_FAIL);
		return FALSE;
	}
	return TRUE;
}