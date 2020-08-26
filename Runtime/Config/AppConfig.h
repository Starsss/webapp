/******************************************************************
模块名：系统配置文件
文　件：AppConfig.h	
创建人：Wang WenJie
创建日期：2019-8-1
功能说明：网络应用程序配置公共头文件,数据结构和常量在此定义	
*******************************************************************/

#pragma once

#include "winsock2.h"
#pragma comment(lib, "wsock32.lib")

/********************公共信息信息配置*********************************/
#define MAX_BUFF_LEN		1024				//缓冲区长度
#define MAX_MESSAGE_LEN		256					//消息缓冲区长度
#define DEFAULT_PATH_NAME	_T("\\downloads")		//默认文件路径

// 状态定义
typedef enum
{
	INIT			=	0,				//初始化
	CONNECTED		=	1,				//已连接
	TRANSFERING		=	2,				//传输中
	CLOSED			=	3,				//已关闭
	LISTENING		=	4,				//正在监听
	CONNECTING		=	5,				//正在连接
	COMPLETED		=	6				//传输完成
};

// 出错信息
typedef struct  
{
	int Error;
	char msg[MAX_BUFF_LEN];
}ErrorInfo;

/********************文件信息配置*********************************/
// 文件长度定义
#define KB					1024
#define MB					(KB*KB)
#define GB					(KB*MB)
#define	BLOCK_SIZE			(6*KB)		//数据块大小
#define MAX_FILE_PATH_LEN	256			//文件路径长度

// 文件信息结构体定义
typedef struct
{
	char _name[MAX_FILE_PATH_LEN];		//文件名
	long _fileSize;						//文件大小
}FileInfo;

/********************CFileModel读写配置*********************************/
#define READ_ERROR			-1			//读错误码
#define WRITE_ERROR			-2			//写错误码

/********************文件传输配置*********************************/

#define CLIENT				1			//客户端
#define SERVER				2			//服务端

typedef enum
{
	CONNECT_SUCCESS		=  	1,			//连接成功
	SEND_FILE_INFO_REQ 	= 	2,			//发送文件信息请求
	SEND_FILE_REQ 		= 	3,			//发送文件请求
	RECV_FILE_INFO_ACK 	= 	4,			//接收文件信息成功
	RECV_FILE_ACK		= 	5,			//接收文件成功
	TRANSFERING_FILE	=	6,			//正在传输文件
	INTERRUPT_TRANSFER	=	7,			//中断传输
	TRANSFER_OVER		=	8,			//传输完毕
	CLOSE_CONNECT		=	9,			//关闭连接
	CLOSE_LISTEN		=	10			//关闭监听
}MessageType;

// 一次传输文件信息结构体定义
typedef struct 
{
	BYTE _buf[BLOCK_SIZE];				//传输的文件块大小
	int _len;							//分块的长度
	long _offset;						//文件偏移值
}FileData;

// 客户端给服务端发送的数据定义
typedef union
{
	FileInfo _fileInfo;					//第一次发送给服务端用于保存文件名
	FileData _transferData;				//用于传输实际文件
}TranferData;

// 客户端给服务端发送的消息定义
typedef struct
{
	MessageType _messageType;			//发送当前消息状态
	TranferData _data;					//发送文件相关数据
}Message;

#define MAX_ELAPSE		1000			//延时信息时长

// 文件传输进度
#define MAX_RANGE		100				
#define MIN_RANGE		0
#define STEP			1
#define ELAPSE_TIME		400					//时间间隔
#define LIST_TIME		1000				//列表更新时间间隔	


#define UPDATE_SPEED	1					//更新速度
#define UPDATE_LIST		2					//更新客户端列表