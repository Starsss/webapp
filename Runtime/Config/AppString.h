/******************************************************************
模块名：系统配置文件
文　件：AppString.h	
创建人：Wang WenJie
创建日期：2019-8-1
功能说明：网络应用程序全局字符串配置
*******************************************************************/

#pragma once

/********************SOCKET信息配置*********************************/
#define SOCKET_INIT_FAIL			"初始化套接字失败"
#define SOCKET_CREATE_FAIL			"客户端SOCKET创建失败"
#define CLIENT_CONNECT_FAIL			"客户端SOCKET连接失败"
#define SERVER_BIND_FAIL			"服务器绑定失败"
#define SERVER_LISTEN_FAIL			"服务器监听失败"
#define ACCEPT_TCP_FAIL				"接收连接失败"
#define CLOSE_SOCKET_FAILE			"关闭套接字失败"
#define CONNECT_CLOSED				"远程关闭了连接"
#define CLOSED_MSG					_T("已断开")
#define INTERRUPTED_MSG				_T("已中断")
#define CONNECTED_MSG				_T("已连接")
#define INIT_MSG					_T("已初始化")
#define LISTEN_MSG					_T("正在监听")
#define TRANSFER_MSG				_T("正在传输")
#define COMPLETE_MSG				_T("发送完毕")
#define STATE_ERROR					"状态不一致"

/********************线程信息配置*********************************/
#define MESSAGE_THREAD_FAIL			"消息线程创建失败"
#define MESSAGE_RECV_FAIL			"消息接收失败"
#define MESSAGE_SEND_FAIL			"消息发送失败"
#define FILE_SEND_FAIL				"文件发送失败"
#define ACCEPT_THREAD_FAIL			"消息接收线程开启失败"
#define SERVER_REPLY_FAIL			"服务端消息回复失败"
#define FILE_RECV_FAIL				"文件接收失败"


/********************文件信息配置*********************************/
#define FILE_OPEN_FAIL				"文件打开失败"
#define FILE_SEEK_FAIL				"文件定位失败"
#define FILE_WRITE_FAIL				"文件写入失败"
#define FILE_READ_FAIL				"文件读取失败"
#define FILE_CREATE_FAIL			"文件创建失败"
#define FILE_INFO_ERROR				"文件信息错误"
#define INTERRUPT_FAIL				"中断传输失败"
#define OFFSET_ERROR				"文件偏移信息不一致"
