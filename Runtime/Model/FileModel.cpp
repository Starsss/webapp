/******************************************************************
模块名：CFileModel
文　件：FileModel.h, FileModel.cpp	
创建人：Wang WenJie
创建日期：2019-7-29
版  本：v1.0.0
功能说明：网络应用程序文件读写模块实现文件
*******************************************************************/

#include "stdafx.h"
#include "FileModel.h"
#include "../Extend/Log.h"
#include "../Config/AppString.h"
#include <string>

using namespace std;

/********************CFileModel构造函数和析构函数*********************************/

CFileModel::CFileModel()
{
	m_length = 0;
	m_strFilePath = "";
}

CFileModel::~CFileModel()
{

}


/********************CFileModel的公共方法*********************************/
// 返回缓冲区长度
long CFileModel::GetLenth()
{
	return this->m_length;
}

// 获取缓冲区内容
void CFileModel::GetBuff(BYTE _buff[])
{
	memcpy(_buff, m_buff, m_length);
}

// 设置缓冲区长度
void CFileModel::SetBuff(LPBYTE _lpByte, size_t _len)
{
	if(_len >0 && _len <= BLOCK_SIZE)
	{
		memset(m_buff, 0, BLOCK_SIZE);
		m_length = _len;
		memcpy(m_buff, _lpByte, _len);
	}
}

/*---------------------------------------------------------------------------
功能：InitFile，用于初始化文件流
参数：_strFilePath文件路径，_offset文件偏移值
返回值：
注意：
---------------------------------------------------------------------------*/
int CFileModel::InitFile(const char* _strFilePath, long _offset, BOOL _read)
{
	this->m_strFilePath = _strFilePath;
	if(_read)	//读文件
	{
		if(0 != fopen_s(&m_fp, _strFilePath, "r+b"))
		{
			CLog::Error(__FUNCTION__, __LINE__, FILE_OPEN_FAIL);
			return READ_ERROR;
		}
	}
	else
	{
		// 如果文件没有创建，则创建该文件
		if(0 == _offset)
		{
			if(0 != fopen_s(&m_fp, _strFilePath, "w+b"))
			{
				CLog::Error(__FUNCTION__, __LINE__, FILE_CREATE_FAIL);
				return WRITE_ERROR;
			}
		}
		else	// 如果文件存在则覆盖方式写入
		{
			if(0 != fopen_s(&m_fp, _strFilePath, "rb+"))
			{
				CLog::Error(__FUNCTION__, __LINE__, FILE_OPEN_FAIL);
				return WRITE_ERROR;
			}
		}
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
功能：Read方法用于读取指定文件偏移处的一个文件块
参数：
返回值：成功返回读取的文件大小，失败返回READ_ERROR
注意：
---------------------------------------------------------------------------*/
int CFileModel::Read(long _offset)
{
	// 定位文件
	if(0 != fseek(m_fp, _offset, SEEK_SET))
	{
		fclose(m_fp);
		CLog::Error(__FUNCTION__, __LINE__, FILE_SEEK_FAIL);
		return READ_ERROR;
	}
	memset(m_buff, 0, BLOCK_SIZE);
	m_length = 0;
	int _size = 0;
	// 读文件
	if(!feof(m_fp))
	{
		_size = fread(m_buff, sizeof(BYTE), BLOCK_SIZE, m_fp);
	}
	// 读文件判断
	if(ferror(m_fp))
	{
		fclose(m_fp);
		CLog::Error(__FUNCTION__, __LINE__, FILE_READ_FAIL);
		return READ_ERROR;
	}
	m_length = _size;
	return _size;
}

/*---------------------------------------------------------------------------
功能：Write方法用于写入指定文件偏移处的一个文件块
参数：
返回值：成功返回写入的文件大小，失败返回WRITE_ERROR
注意：
---------------------------------------------------------------------------*/
int CFileModel::Write(long _offset)
{
	// 定位文件
	if(0 != fseek(m_fp, _offset, SEEK_SET))
	{
		fclose(m_fp);
		CLog::Error(__FUNCTION__, __LINE__, FILE_SEEK_FAIL);
		return 0;
	}
	// 写入文件
	int _size = fwrite(m_buff, sizeof(BYTE), m_length, m_fp);
	if(ferror(m_fp))
	{
		fclose(m_fp);
		CLog::Error(__FUNCTION__, __LINE__, FILE_WRITE_FAIL);
		return WRITE_ERROR;
	}
	return _size;
}

/*---------------------------------------------------------------------------
功能：Close方法用于关闭文件流
参数：无
返回值：无
注意：
---------------------------------------------------------------------------*/
void CFileModel::Close()
{
	if(m_fp != NULL)
	{
		fclose(m_fp);
	}
}

/*---------------------------------------------------------------------------
功能：GetFileInfo，获取文件信息
参数: const char* _strFilePath 文件路径, FileInfo &_fileInfo文件信息
返回值：成功返回TRUE
注意：
---------------------------------------------------------------------------*/
int CFileModel::GetFileInfo(FileInfo &_fileInfo)
{
	memset(&_fileInfo, 0, sizeof(FileInfo));
	// 获取文件大小
	_fileInfo._fileSize = GetFileSize();
	// 获取文件名
	string _fileName = GetFileName(m_strFilePath.c_str());
	strcpy_s(_fileInfo._name, _fileName.length() + 1, _fileName.c_str());
	return TRUE;
}

/*---------------------------------------------------------------------------
功能：GetFileSize方法用于获取文件大小
参数：const char* _strFilePath 文件路径
返回值：成功返回文件大小，失败返回0
注意：
---------------------------------------------------------------------------*/
long CFileModel::GetFileSize()
{
	// 定位文件末尾
	if(0 != fseek(m_fp, 0L, SEEK_END))
	{
		fclose(m_fp);
		CLog::Error(__FUNCTION__, __LINE__, FILE_SEEK_FAIL);
		return READ_ERROR;
	}
	// 取文件大小
	long _size = ftell(m_fp);
	return _size;
}

/*---------------------------------------------------------------------------
功能：GetFileName，通过文件路径获取文件名
参数：const char* _strFilePath，文件路径
返回值：const char*文件名
注意：
---------------------------------------------------------------------------*/
string CFileModel::GetFileName(const char* _strFilePath)
{
	string strFullName = _strFilePath;
	// 路径为空直接返回
	if (strFullName.empty())
	{
		return "";
	}
	
	// 替换文件路径分隔符
	ReplaceStr(strFullName, "/", "\\");
	// 截取文件名
	string::size_type iPos = strFullName.find_last_of('\\') + 1;
	return strFullName.substr(iPos, strFullName.length() - iPos);
}

/********************CFileModel的私有方法*********************************/

/*---------------------------------------------------------------------------
功能：ReplaceStr，把字符串的strsrc替换成strdst
参数：string &strBig 待替换字符, const string &strsrc 原始字符串, const string &strdst 目标字符串
返回值：无
注意：
**********************************************************************/
void CFileModel::ReplaceStr( string &strBig, const string &strsrc, const string &strdst)
{
	string::size_type pos = 0;
	string::size_type srclen = strsrc.size();
	string::size_type dstlen = strdst.size();

	while( (pos=strBig.find(strsrc, pos)) != string::npos )
	{
		strBig.replace( pos, srclen, strdst );
		pos += dstlen;
	}
}
