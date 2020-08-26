/******************************************************************
ģ������CSocketControl
�ġ�����CSocketControl.h	
�����ˣ�Wang WenJie
�������ڣ�2019-8-2
��  ����v1.2.0
����˵��������Ӧ�ó��������ͷ�ļ�
*******************************************************************/

#pragma once

#include "../Config/AppConfig.h"
#include <string>
#include <vector>
#include "../Model/FileModel.h"
#include "Client.h"

class CSocketModel;
class CFileModel;

// ��Ϣ�����߳�
UINT MessageProcessThread(LPVOID lpParam);
// ��Ϣ�����߳�
UINT AcceptConnectThread(LPVOID lpParam);

/*
* ������� : CSocketControl
* ������; : �ṩ�������Ӵ���Ŀ��Ʒ���
*/

class CSocketControl
{
public:
	/**************************CSocketControl��Get��Set����***************************/
	int GetState();													// ��ȡ������״̬
	void SetServerIP(const char* _ip);								// ���÷����IP
	void SetPort(int _port);										// ���ö˿ں�
	void SetMaxConnect(int _maxConnect);							// �����������
	void SetFilePath(const char* _filePath);						// �����ļ�·��
	void SetDirectory(const char* _directory);						// �����ļ�Ŀ¼
	int GetProgress();												// ��ȡ�ļ��������
	std::string GetSpeed();											// ��ȡ�ļ������ٶ�
	void SetStartOffset();											// ��¼��ʼ��
	ErrorInfo GetErrorInfo();										// ���ش�����Ϣ
	ClientMap GetClients();											// ��ȡ�������Socket

	/**************************CSocketControl�Ĺ�������***************************/
	int Connect();													// ����TCP����
	int ReceiveProcess();											// ������Ϣ����
	int TransferFile();												// ��ʼ�����ļ�
	int InterruptTransfer(SOCKET _socket,BOOL _positive);			// �жϴ���
	int CloseConnect();												// �ر�����
	int AcceptConnect();											// ���տͻ��˷���
	int Listen();													// �����ͻ��˼���
	int CloseListen();												// �رռ���


	CSocketControl();
	~CSocketControl();

private:
	/**************************CSocketControl��˽�з���***************************/
	void Init();													// �ڲ���ʼ��
	void Handle(Message _recvMsg, SOCKET _sokcet);					// ʵ�ʵ���Ϣ�������
	int TransferFileInfo();											// �����ļ���Ϣ
	int TransferFileData(Message _recvMsg);							// �����ļ���
	int BeginAccept();												// ��ʼ���տͻ���
	int ReceiveFileInfo(Message _recvMsg, SOCKET _socket);			// �����ļ���Ϣ
	int ReceiveFileData(Message _recvMsg, SOCKET _socket);			// �����ļ���
	int CloseProcess(MessageType _state);							// �رմ������	
	void TransferOver(SOCKET _sokcet);								// �ļ��������
	void ClearClients();											// ����ͻ���

	/**************************CSocketControl��˽������***************************/
	std::string m_serverIP;											// ��������ַ
	int m_port;														// �������˿�
	int m_maxConnect;												// ���������
	int m_state;													// ����״̬
	long m_offset;													// �ļ�ƫ��λ��
	long m_startOffset;												// ��¼�ļ�ƫ�ƿ�ʼλ��
	long m_endOffset;												// ��¼�ļ�ƫ�ƽ���λ��
	FileInfo m_fileInfo;											// �ļ���Ϣ
	std::string m_strFilePath;										// ������ļ���ַ
	std::string m_strDirectory;										// �����ļ���Ŀ¼
	int m_action;													// ��¼��ǰ����

	CWinThread* m_msgThread;										// ��Ϣ�߳�
	CWinThread* m_acceptThread;										// ���������߳�
	CRITICAL_SECTION m_msgSection;									// �ٽ���

	CSocketModel* m_model;											// ģ�ͱ���
	SOCKET m_connectSocket;											// �����׽���
	SOCKET m_listenSocket;											// �����׽���
	ErrorInfo m_error;												// ������Ϣ����
	CFileModel m_sendFile;											// �����ļ�ģ��
	ClientMap m_client;												// ���տͻ�������
};