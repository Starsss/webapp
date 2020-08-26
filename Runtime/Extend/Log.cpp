/******************************************************************
模块名：CLog
文　件：Log.h, Log.cpp	
创建人：Wang WenJie
创建日期：2019-7-31
版  本：v1.0.0
功能说明：网络应用程序日志类实现文件
*******************************************************************/
#include "StdAfx.h"
#include "Log.h"
#include <time.h>

/********************CLog构造函数和析构函数*********************************/
CLog::CLog(void)
{
}

CLog::~CLog(void)
{
}

/********************CLog构静态成员初始化*********************************/
char CLog::m_strTrace[MAX_FILE_PATH_LEN] = {};
char CLog::m_strWarning[MAX_FILE_PATH_LEN] = {};
char CLog::m_strError[MAX_FILE_PATH_LEN] = {};

/*****************************************************************/
/********************CLog公共方法*********************************/
/*****************************************************************/

// Trace方法
int CLog::Trace(const char* _function, long _line, const char* _content)
{
	char _sline[10];
	_ltoa_s(_line, _sline, 10);
	return WriteLog(CLog::m_strTrace, _function, _sline, _content);
}

// Warning方法
int CLog::Warning(const char* _function, long _line, const char* _content)
{
	char _sline[10];
	_ltoa_s(_line, _sline, 10);
	return WriteLog(CLog::m_strWarning, _function, _sline, _content);
}

// Error方法
int CLog::Error(const char* _function, long _line, const char* _content)
{
	char _sline[10];
	_ltoa_s(_line, _sline, 10);
	return WriteLog(CLog::m_strError, _function, _sline, _content);
}

/*****************************************************************/
/********************CLog私有方法*********************************/
/*****************************************************************/

/*---------------------------------------------------------------------------
功能：WriteLog，写日志实现方法
参数: const char* _filePath文件路径，const char* _function方法名称, const char* _line行号, const char* _content日志内容
返回值：无
注意：
---------------------------------------------------------------------------*/
int CLog::WriteLog(const char* _filePath, const char* _function, const char* _line, const char* _content)
{
	FILE* fp;
	// 追加模式
	if(0 != fopen_s(&fp, _filePath, "a+"))
	{
		AfxMessageBox(_T("日志文件打开错误"), MB_OK);
		// 关闭文件流
		fclose(fp);
		return FALSE;
	}
	// 格式化时间
	time_t now_t;
	struct tm t;
	time(&now_t);
	localtime_s(&t, &now_t);
	char _time[64];
	strftime(_time, sizeof(_time), "%Y-%m-%d %H:%M:%S", &t);

	char _tmp[MAX_BUFF_LEN];
	sprintf_s(_tmp, MAX_BUFF_LEN, "%s\t|%s\t|\t%s\t|\t%s\r\n", _time, _function, _line, _content);
	// 写入文件
	fwrite(_tmp, strlen(_tmp), 1, fp);
	if(ferror(fp))
	{
		AfxMessageBox(_T("日志文件写入错误"), MB_OK);
		// 关闭文件流
		fclose(fp);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}

/*---------------------------------------------------------------------------
功能：InitLog，初始化日志文件路径
参数: 无
返回值：初始化成功返回TRUE
注意：
---------------------------------------------------------------------------*/
void InitLog()
{
	// 格式化时间
	time_t now_t;
	struct tm t;
	time(&now_t);
	localtime_s(&t, &now_t);
	char _tmp[64];
	strftime(_tmp, sizeof(_tmp), "%Y-%m-%d", &t);

	// Trace路径拼接
	sprintf_s(CLog::m_strTrace, MAX_FILE_PATH_LEN, "%s_Trace.txt", _tmp);
	// Warning路径拼接
	sprintf_s(CLog::m_strWarning, MAX_FILE_PATH_LEN, "%s_Warning.txt", _tmp);
	// Error路径拼接
	sprintf_s(CLog::m_strError, MAX_FILE_PATH_LEN, "%s_Error.txt", _tmp);
}
