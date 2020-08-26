/******************************************************************
ģ������CLog
�ġ�����Log.h	
�����ˣ�Wang WenJie
�������ڣ�2019-7-31
��  ����v1.0.0
����˵��������Ӧ�ó�����־��ͷ�ļ����ṩTrace��Warning��Error�ȷ���
*******************************************************************/

#pragma once

// ����ϵͳ�����ļ�
#include "../Config/AppConfig.h"


/*
* ������� : CLog
* ������; : �ṩ��־��¼����
*/
class CLog
{
public:
	CLog(void);
	~CLog(void);

	// Trace
	static int Trace(const char* _function, long _line, const char* _content);

	// Warning
	static int Warning(const char* _function, long _line, const char* _content);

	// Error
	static int Error(const char* _function, long _line, const char* _content);

	// ��ʼ����־
	friend void InitLog();

private:
	// д���ļ�����
	static int WriteLog(const char* _filePath, const char* _function, const char* _line, const char* _content);

	static char m_strTrace[MAX_FILE_PATH_LEN];      //Trace·��

	static char m_strWarning[MAX_FILE_PATH_LEN];    //Warning·��

	static char m_strError[MAX_FILE_PATH_LEN];      //Error·��
	
};
