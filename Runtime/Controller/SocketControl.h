/******************************************************************
模块名：CSocketControl
文　件：CSocketControl.h	
创建人：Wang WenJie
创建日期：2019-8-2
版  本：v1.2.0
功能说明：网络应用程序控制器头文件
*******************************************************************/

#pragma once

#include "../Config/AppConfig.h"
#include <string>
#include <vector>
#include "../Model/FileModel.h"
#include "Client.h"

class CSocketModel;
class CFileModel;

// 消息监听线程
UINT MessageProcessThread(LPVOID lpParam);
// 消息接收线程
UINT AcceptConnectThread(LPVOID lpParam);

/*
* 类的名称 : CSocketControl
* 基本用途 : 提供网络连接传输的控制方法
*/

class CSocketControl
{
public:
	/**************************CSocketControl的Get和Set方法***************************/
	int GetState();													// 获取控制器状态
	void SetServerIP(const char* _ip);								// 设置服务端IP
	void SetPort(int _port);										// 设置端口号
	void SetMaxConnect(int _maxConnect);							// 设置最大连接
	void SetFilePath(const char* _filePath);						// 设置文件路径
	void SetDirectory(const char* _directory);						// 设置文件目录
	int GetProgress();												// 获取文件传输进度
	std::string GetSpeed();											// 获取文件传输速度
	void SetStartOffset();											// 记录起始点
	ErrorInfo GetErrorInfo();										// 返回错误消息
	ClientMap GetClients();											// 获取容器里的Socket

	/**************************CSocketControl的公共方法***************************/
	int Connect();													// 建立TCP连接
	int ReceiveProcess();											// 接收消息方法
	int TransferFile();												// 开始传输文件
	int InterruptTransfer(SOCKET _socket,BOOL _positive);			// 中断传输
	int CloseConnect();												// 关闭连接
	int AcceptConnect();											// 接收客户端方法
	int Listen();													// 开启客户端监听
	int CloseListen();												// 关闭监听


	CSocketControl();
	~CSocketControl();

private:
	/**************************CSocketControl的私有方法***************************/
	void Init();													// 内部初始化
	void Handle(Message _recvMsg, SOCKET _sokcet);					// 实际的消息处理过程
	int TransferFileInfo();											// 传输文件信息
	int TransferFileData(Message _recvMsg);							// 传输文件块
	int BeginAccept();												// 开始接收客户端
	int ReceiveFileInfo(Message _recvMsg, SOCKET _socket);			// 接收文件信息
	int ReceiveFileData(Message _recvMsg, SOCKET _socket);			// 接收文件块
	int CloseProcess(MessageType _state);							// 关闭处理过程	
	void TransferOver(SOCKET _sokcet);								// 文件传输结束
	void ClearClients();											// 清理客户端

	/**************************CSocketControl的私有属性***************************/
	std::string m_serverIP;											// 服务器地址
	int m_port;														// 服务器端口
	int m_maxConnect;												// 最大连接数
	int m_state;													// 控制状态
	long m_offset;													// 文件偏移位置
	long m_startOffset;												// 记录文件偏移开始位置
	long m_endOffset;												// 记录文件偏移结束位置
	FileInfo m_fileInfo;											// 文件信息
	std::string m_strFilePath;										// 传输的文件地址
	std::string m_strDirectory;										// 传输文件的目录
	int m_action;													// 记录当前动作

	CWinThread* m_msgThread;										// 消息线程
	CWinThread* m_acceptThread;										// 接受连接线程
	CRITICAL_SECTION m_msgSection;									// 临界区

	CSocketModel* m_model;											// 模型变量
	SOCKET m_connectSocket;											// 连接套接字
	SOCKET m_listenSocket;											// 监听套接字
	ErrorInfo m_error;												// 错误信息变量
	CFileModel m_sendFile;											// 发送文件模型
	ClientMap m_client;												// 接收客户接容器
};