/******************************************************************
ģ������CLog
�ġ�����Log.h, Log.cpp	
�����ˣ�Wang WenJie
�������ڣ�2019-7-31
��  ����v1.0.0
����˵��������Ӧ�ó�����־��ʵ���ļ�
*******************************************************************/
#include "StdAfx.h"
#include "Log.h"
#include <time.h>

/********************CLog���캯������������*********************************/
CLog::CLog(void)
{
}

CLog::~CLog(void)
{
}

/********************CLog����̬��Ա��ʼ��*********************************/
char CLog::m_strTrace[MAX_FILE_PATH_LEN] = {};
char CLog::m_strWarning[MAX_FILE_PATH_LEN] = {};
char CLog::m_strError[MAX_FILE_PATH_LEN] = {};

/*****************************************************************/
/********************CLog��������*********************************/
/*****************************************************************/

// Trace����
int CLog::Trace(const char* _function, long _line, const char* _content)
{
	char _sline[10];
	_ltoa_s(_line, _sline, 10);
	return WriteLog(CLog::m_strTrace, _function, _sline, _content);
}

// Warning����
int CLog::Warning(const char* _function, long _line, const char* _content)
{
	char _sline[10];
	_ltoa_s(_line, _sline, 10);
	return WriteLog(CLog::m_strWarning, _function, _sline, _content);
}

// Error����
int CLog::Error(const char* _function, long _line, const char* _content)
{
	char _sline[10];
	_ltoa_s(_line, _sline, 10);
	return WriteLog(CLog::m_strError, _function, _sline, _content);
}

/*****************************************************************/
/********************CLog˽�з���*********************************/
/*****************************************************************/

/*---------------------------------------------------------------------------
���ܣ�WriteLog��д��־ʵ�ַ���
����: const char* _filePath�ļ�·����const char* _function��������, const char* _line�к�, const char* _content��־����
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
int CLog::WriteLog(const char* _filePath, const char* _function, const char* _line, const char* _content)
{
	FILE* fp;
	// ׷��ģʽ
	if(0 != fopen_s(&fp, _filePath, "a+"))
	{
		AfxMessageBox(_T("��־�ļ��򿪴���"), MB_OK);
		// �ر��ļ���
		fclose(fp);
		return FALSE;
	}
	// ��ʽ��ʱ��
	time_t now_t;
	struct tm t;
	time(&now_t);
	localtime_s(&t, &now_t);
	char _time[64];
	strftime(_time, sizeof(_time), "%Y-%m-%d %H:%M:%S", &t);

	char _tmp[MAX_BUFF_LEN];
	sprintf_s(_tmp, MAX_BUFF_LEN, "%s\t|%s\t|\t%s\t|\t%s\r\n", _time, _function, _line, _content);
	// д���ļ�
	fwrite(_tmp, strlen(_tmp), 1, fp);
	if(ferror(fp))
	{
		AfxMessageBox(_T("��־�ļ�д�����"), MB_OK);
		// �ر��ļ���
		fclose(fp);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�InitLog����ʼ����־�ļ�·��
����: ��
����ֵ����ʼ���ɹ�����TRUE
ע�⣺
---------------------------------------------------------------------------*/
void InitLog()
{
	// ��ʽ��ʱ��
	time_t now_t;
	struct tm t;
	time(&now_t);
	localtime_s(&t, &now_t);
	char _tmp[64];
	strftime(_tmp, sizeof(_tmp), "%Y-%m-%d", &t);

	// Trace·��ƴ��
	sprintf_s(CLog::m_strTrace, MAX_FILE_PATH_LEN, "%s_Trace.txt", _tmp);
	// Warning·��ƴ��
	sprintf_s(CLog::m_strWarning, MAX_FILE_PATH_LEN, "%s_Warning.txt", _tmp);
	// Error·��ƴ��
	sprintf_s(CLog::m_strError, MAX_FILE_PATH_LEN, "%s_Error.txt", _tmp);
}
