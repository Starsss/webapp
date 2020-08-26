/******************************************************************
模块名：CLog
文　件：Log.h	
创建人：Wang WenJie
创建日期：2019-7-31
版  本：v1.0.0
功能说明：网络应用程序日志类头文件，提供Trace、Warning和Error等方法
*******************************************************************/

#pragma once

// 包含系统配置文件
#include "../Config/AppConfig.h"


/*
* 类的名称 : CLog
* 基本用途 : 提供日志记录方法
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

	// 初始化日志
	friend void InitLog();

private:
	// 写入文件方法
	static int WriteLog(const char* _filePath, const char* _function, const char* _line, const char* _content);

	static char m_strTrace[MAX_FILE_PATH_LEN];      //Trace路径

	static char m_strWarning[MAX_FILE_PATH_LEN];    //Warning路径

	static char m_strError[MAX_FILE_PATH_LEN];      //Error路径
	
};
