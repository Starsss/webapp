/******************************************************************
ģ������CClient
�ġ�����Client.h	
�����ˣ�Wang WenJie
�������ڣ�2019-8-7
��  ����v1.0.0
����˵��������Ӧ�ó���ͻ��˹�����ͷ�ļ�
*******************************************************************/

#pragma once

// ����ϵͳ�����ļ�
#include "../Config/AppConfig.h"
#include "../Model/FileModel.h"
#include <map>

class CFileModel;

class CClient
{
public:
	
	SOCKET socket;					// �׽���
	SOCKADDR_IN sin;				// sockaddr�ṹ
	CFileModel file;				// �ļ���
	long offset;					// �ļ�ƫ�ƹ���
	long startOffset;				// ��ʼλ��
	long endOffset;					// �����λ��
	FileInfo fileInfo;				// �ļ���Ϣ����
	CWinThread* cthread;			// �̹߳��� 
	int msg;						// ȫ����Ϣ
	int state;						// ����״̬
	string filePath;				// �ļ�·��
	string lastFileName;			// ������ɺ���ļ���

	CClient();
	~CClient();

};

typedef map<SOCKET, CClient> ClientMap;