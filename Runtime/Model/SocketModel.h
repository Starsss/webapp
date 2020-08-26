/******************************************************************
模块名：CSocketModel
文　件：SocketModel.h	
创建人：Wang WenJie
创建日期：2019-8-1
版  本：v1.1.0
功能说明：网络应用程序网络类模型头文件，实现连接、监听、发送、接收、中断和关闭功能
*******************************************************************/


#pragma once

// 包含系统配置文件
#include "../Config/AppConfig.h"


/*
* 类的名称 : CSocketModel
* 基本用途 : 网络基本模型
*/
class CSocketModel
{
public:
	/************************CSocketModel的公共方法**********************************/
	// 开启TCP连接
	int BeginConnect(const char* _serverIP, int _port, SOCKET &_connectSocket);						
	// 开启TCP监听
	int BeginListen(const char* _serverIP, int _port, SOCKET &_connectSocket, SOCKET &_listenSokcet);	
	// 开启消息接收
	int ReceiveMessage(Message &_msg, SOCKET _connectSocket);
	// 开启发送消息
	int SendMessage(Message _msg, SOCKET _connectSocket);
	// 关闭连接
	int CloseConnect(SOCKET &_sokcet);											
	// 开启接收TCP连接
	int AcceptConnect(SOCKADDR_IN & _sin, SOCKET &_connectSocket, SOCKET _listenSocket);										


	CSocketModel();
	virtual ~CSocketModel();

private:
	/************************CSocketModel的私有方法**********************************/
	// 初始化套接字
	int InitSocket(SOCKET &_sock);											
};