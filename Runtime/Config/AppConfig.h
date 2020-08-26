/******************************************************************
ģ������ϵͳ�����ļ�
�ġ�����AppConfig.h	
�����ˣ�Wang WenJie
�������ڣ�2019-8-1
����˵��������Ӧ�ó������ù���ͷ�ļ�,���ݽṹ�ͳ����ڴ˶���	
*******************************************************************/

#pragma once

#include "winsock2.h"
#pragma comment(lib, "wsock32.lib")

/********************������Ϣ��Ϣ����*********************************/
#define MAX_BUFF_LEN		1024				//����������
#define MAX_MESSAGE_LEN		256					//��Ϣ����������
#define DEFAULT_PATH_NAME	_T("\\downloads")		//Ĭ���ļ�·��

// ״̬����
typedef enum
{
	INIT			=	0,				//��ʼ��
	CONNECTED		=	1,				//������
	TRANSFERING		=	2,				//������
	CLOSED			=	3,				//�ѹر�
	LISTENING		=	4,				//���ڼ���
	CONNECTING		=	5,				//��������
	COMPLETED		=	6				//�������
};

// ������Ϣ
typedef struct  
{
	int Error;
	char msg[MAX_BUFF_LEN];
}ErrorInfo;

/********************�ļ���Ϣ����*********************************/
// �ļ����ȶ���
#define KB					1024
#define MB					(KB*KB)
#define GB					(KB*MB)
#define	BLOCK_SIZE			(6*KB)		//���ݿ��С
#define MAX_FILE_PATH_LEN	256			//�ļ�·������

// �ļ���Ϣ�ṹ�嶨��
typedef struct
{
	char _name[MAX_FILE_PATH_LEN];		//�ļ���
	long _fileSize;						//�ļ���С
}FileInfo;

/********************CFileModel��д����*********************************/
#define READ_ERROR			-1			//��������
#define WRITE_ERROR			-2			//д������

/********************�ļ���������*********************************/

#define CLIENT				1			//�ͻ���
#define SERVER				2			//�����

typedef enum
{
	CONNECT_SUCCESS		=  	1,			//���ӳɹ�
	SEND_FILE_INFO_REQ 	= 	2,			//�����ļ���Ϣ����
	SEND_FILE_REQ 		= 	3,			//�����ļ�����
	RECV_FILE_INFO_ACK 	= 	4,			//�����ļ���Ϣ�ɹ�
	RECV_FILE_ACK		= 	5,			//�����ļ��ɹ�
	TRANSFERING_FILE	=	6,			//���ڴ����ļ�
	INTERRUPT_TRANSFER	=	7,			//�жϴ���
	TRANSFER_OVER		=	8,			//�������
	CLOSE_CONNECT		=	9,			//�ر�����
	CLOSE_LISTEN		=	10			//�رռ���
}MessageType;

// һ�δ����ļ���Ϣ�ṹ�嶨��
typedef struct 
{
	BYTE _buf[BLOCK_SIZE];				//������ļ����С
	int _len;							//�ֿ�ĳ���
	long _offset;						//�ļ�ƫ��ֵ
}FileData;

// �ͻ��˸�����˷��͵����ݶ���
typedef union
{
	FileInfo _fileInfo;					//��һ�η��͸���������ڱ����ļ���
	FileData _transferData;				//���ڴ���ʵ���ļ�
}TranferData;

// �ͻ��˸�����˷��͵���Ϣ����
typedef struct
{
	MessageType _messageType;			//���͵�ǰ��Ϣ״̬
	TranferData _data;					//�����ļ��������
}Message;

#define MAX_ELAPSE		1000			//��ʱ��Ϣʱ��

// �ļ��������
#define MAX_RANGE		100				
#define MIN_RANGE		0
#define STEP			1
#define ELAPSE_TIME		400					//ʱ����
#define LIST_TIME		1000				//�б����ʱ����	


#define UPDATE_SPEED	1					//�����ٶ�
#define UPDATE_LIST		2					//���¿ͻ����б�