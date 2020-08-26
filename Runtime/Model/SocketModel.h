/******************************************************************
ģ������CSocketModel
�ġ�����SocketModel.h	
�����ˣ�Wang WenJie
�������ڣ�2019-8-1
��  ����v1.1.0
����˵��������Ӧ�ó���������ģ��ͷ�ļ���ʵ�����ӡ����������͡����ա��жϺ͹رչ���
*******************************************************************/


#pragma once

// ����ϵͳ�����ļ�
#include "../Config/AppConfig.h"


/*
* ������� : CSocketModel
* ������; : �������ģ��
*/
class CSocketModel
{
public:
	/************************CSocketModel�Ĺ�������**********************************/
	// ����TCP����
	int BeginConnect(const char* _serverIP, int _port, SOCKET &_connectSocket);						
	// ����TCP����
	int BeginListen(const char* _serverIP, int _port, SOCKET &_connectSocket, SOCKET &_listenSokcet);	
	// ������Ϣ����
	int ReceiveMessage(Message &_msg, SOCKET _connectSocket);
	// ����������Ϣ
	int SendMessage(Message _msg, SOCKET _connectSocket);
	// �ر�����
	int CloseConnect(SOCKET &_sokcet);											
	// ��������TCP����
	int AcceptConnect(SOCKADDR_IN & _sin, SOCKET &_connectSocket, SOCKET _listenSocket);										


	CSocketModel();
	virtual ~CSocketModel();

private:
	/************************CSocketModel��˽�з���**********************************/
	// ��ʼ���׽���
	int InitSocket(SOCKET &_sock);											
};