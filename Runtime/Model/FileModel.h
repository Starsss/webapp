/******************************************************************
模块名：CFileModel
文　件：FileModel.h	
创建人：Wang WenJie
创建日期：2019-7-29
版  本：v1.0.0
功能说明：网络应用程序文件读写模块头文件
*******************************************************************/

#pragma once

// 包含系统配置文件
#include "../Config/AppConfig.h"
#include <string>

using namespace std;

class CFileModel
{

public:
	// 获取缓冲区长度
	long GetLenth();
	// 获取缓冲区内容
	void GetBuff(BYTE _byte[]);
	// 设置缓冲区内容
	void SetBuff(LPBYTE _lpByte, size_t _len);

	// 读文件
	int Read(long _offset);
	// 写文件
	int Write(long _offset);

	// 获取文件信息
	int GetFileInfo(FileInfo &_fileInfo);

	// 获取文件大小
	long GetFileSize();
	// 获取文件名
	string GetFileName(const char* _strFilePath);

	// 初始化文件
	int InitFile(const char* _strFilePath, long _offset, BOOL _read);

	// 关闭文件流
	void Close();

	// 构造和析构
	CFileModel();
	~CFileModel();

protected:

private:
	
	// 字符串替换
	void ReplaceStr(string &strBig, const string &strsrc, const string &strdst);
	

	// 缓冲区长度
	long m_length;
	// 文件路径
	string m_strFilePath;
	// 缓冲区内容
	BYTE m_buff[BLOCK_SIZE];
	// 文件流
	FILE* m_fp;
};

