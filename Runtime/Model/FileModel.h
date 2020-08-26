/******************************************************************
ģ������CFileModel
�ġ�����FileModel.h	
�����ˣ�Wang WenJie
�������ڣ�2019-7-29
��  ����v1.0.0
����˵��������Ӧ�ó����ļ���дģ��ͷ�ļ�
*******************************************************************/

#pragma once

// ����ϵͳ�����ļ�
#include "../Config/AppConfig.h"
#include <string>

using namespace std;

class CFileModel
{

public:
	// ��ȡ����������
	long GetLenth();
	// ��ȡ����������
	void GetBuff(BYTE _byte[]);
	// ���û���������
	void SetBuff(LPBYTE _lpByte, size_t _len);

	// ���ļ�
	int Read(long _offset);
	// д�ļ�
	int Write(long _offset);

	// ��ȡ�ļ���Ϣ
	int GetFileInfo(FileInfo &_fileInfo);

	// ��ȡ�ļ���С
	long GetFileSize();
	// ��ȡ�ļ���
	string GetFileName(const char* _strFilePath);

	// ��ʼ���ļ�
	int InitFile(const char* _strFilePath, long _offset, BOOL _read);

	// �ر��ļ���
	void Close();

	// ���������
	CFileModel();
	~CFileModel();

protected:

private:
	
	// �ַ����滻
	void ReplaceStr(string &strBig, const string &strsrc, const string &strdst);
	

	// ����������
	long m_length;
	// �ļ�·��
	string m_strFilePath;
	// ����������
	BYTE m_buff[BLOCK_SIZE];
	// �ļ���
	FILE* m_fp;
};

