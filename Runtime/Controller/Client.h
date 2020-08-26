/******************************************************************
模块名：CClient
文　件：Client.h	
创建人：Wang WenJie
创建日期：2019-8-7
版  本：v1.0.0
功能说明：网络应用程序客户端管理类头文件
*******************************************************************/

#pragma once

// 包含系统配置文件
#include "../Config/AppConfig.h"
#include "../Model/FileModel.h"
#include <map>

class CFileModel;

class CClient
{
public:
	
	SOCKET socket;					// 套接字
	SOCKADDR_IN sin;				// sockaddr结构
	CFileModel file;				// 文件流
	long offset;					// 文件偏移管理
	long startOffset;				// 起始位置
	long endOffset;					// 传输后位置
	FileInfo fileInfo;				// 文件信息管理
	CWinThread* cthread;			// 线程管理 
	int msg;						// 全局消息
	int state;						// 连接状态
	string filePath;				// 文件路径
	string lastFileName;			// 传输完成后的文件名

	CClient();
	~CClient();

};

typedef map<SOCKET, CClient> ClientMap;