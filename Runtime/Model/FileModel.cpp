/******************************************************************
ģ������CFileModel
�ġ�����FileModel.h, FileModel.cpp	
�����ˣ�Wang WenJie
�������ڣ�2019-7-29
��  ����v1.0.0
����˵��������Ӧ�ó����ļ���дģ��ʵ���ļ�
*******************************************************************/

#include "stdafx.h"
#include "FileModel.h"
#include "../Extend/Log.h"
#include "../Config/AppString.h"
#include <string>

using namespace std;

/********************CFileModel���캯������������*********************************/

CFileModel::CFileModel()
{
	m_length = 0;
	m_strFilePath = "";
}

CFileModel::~CFileModel()
{

}


/********************CFileModel�Ĺ�������*********************************/
// ���ػ���������
long CFileModel::GetLenth()
{
	return this->m_length;
}

// ��ȡ����������
void CFileModel::GetBuff(BYTE _buff[])
{
	memcpy(_buff, m_buff, m_length);
}

// ���û���������
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
���ܣ�InitFile�����ڳ�ʼ���ļ���
������_strFilePath�ļ�·����_offset�ļ�ƫ��ֵ
����ֵ��
ע�⣺
---------------------------------------------------------------------------*/
int CFileModel::InitFile(const char* _strFilePath, long _offset, BOOL _read)
{
	this->m_strFilePath = _strFilePath;
	if(_read)	//���ļ�
	{
		if(0 != fopen_s(&m_fp, _strFilePath, "r+b"))
		{
			CLog::Error(__FUNCTION__, __LINE__, FILE_OPEN_FAIL);
			return READ_ERROR;
		}
	}
	else
	{
		// ����ļ�û�д������򴴽����ļ�
		if(0 == _offset)
		{
			if(0 != fopen_s(&m_fp, _strFilePath, "w+b"))
			{
				CLog::Error(__FUNCTION__, __LINE__, FILE_CREATE_FAIL);
				return WRITE_ERROR;
			}
		}
		else	// ����ļ������򸲸Ƿ�ʽд��
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
���ܣ�Read�������ڶ�ȡָ���ļ�ƫ�ƴ���һ���ļ���
������
����ֵ���ɹ����ض�ȡ���ļ���С��ʧ�ܷ���READ_ERROR
ע�⣺
---------------------------------------------------------------------------*/
int CFileModel::Read(long _offset)
{
	// ��λ�ļ�
	if(0 != fseek(m_fp, _offset, SEEK_SET))
	{
		fclose(m_fp);
		CLog::Error(__FUNCTION__, __LINE__, FILE_SEEK_FAIL);
		return READ_ERROR;
	}
	memset(m_buff, 0, BLOCK_SIZE);
	m_length = 0;
	int _size = 0;
	// ���ļ�
	if(!feof(m_fp))
	{
		_size = fread(m_buff, sizeof(BYTE), BLOCK_SIZE, m_fp);
	}
	// ���ļ��ж�
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
���ܣ�Write��������д��ָ���ļ�ƫ�ƴ���һ���ļ���
������
����ֵ���ɹ�����д����ļ���С��ʧ�ܷ���WRITE_ERROR
ע�⣺
---------------------------------------------------------------------------*/
int CFileModel::Write(long _offset)
{
	// ��λ�ļ�
	if(0 != fseek(m_fp, _offset, SEEK_SET))
	{
		fclose(m_fp);
		CLog::Error(__FUNCTION__, __LINE__, FILE_SEEK_FAIL);
		return 0;
	}
	// д���ļ�
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
���ܣ�Close�������ڹر��ļ���
��������
����ֵ����
ע�⣺
---------------------------------------------------------------------------*/
void CFileModel::Close()
{
	if(m_fp != NULL)
	{
		fclose(m_fp);
	}
}

/*---------------------------------------------------------------------------
���ܣ�GetFileInfo����ȡ�ļ���Ϣ
����: const char* _strFilePath �ļ�·��, FileInfo &_fileInfo�ļ���Ϣ
����ֵ���ɹ�����TRUE
ע�⣺
---------------------------------------------------------------------------*/
int CFileModel::GetFileInfo(FileInfo &_fileInfo)
{
	memset(&_fileInfo, 0, sizeof(FileInfo));
	// ��ȡ�ļ���С
	_fileInfo._fileSize = GetFileSize();
	// ��ȡ�ļ���
	string _fileName = GetFileName(m_strFilePath.c_str());
	strcpy_s(_fileInfo._name, _fileName.length() + 1, _fileName.c_str());
	return TRUE;
}

/*---------------------------------------------------------------------------
���ܣ�GetFileSize�������ڻ�ȡ�ļ���С
������const char* _strFilePath �ļ�·��
����ֵ���ɹ������ļ���С��ʧ�ܷ���0
ע�⣺
---------------------------------------------------------------------------*/
long CFileModel::GetFileSize()
{
	// ��λ�ļ�ĩβ
	if(0 != fseek(m_fp, 0L, SEEK_END))
	{
		fclose(m_fp);
		CLog::Error(__FUNCTION__, __LINE__, FILE_SEEK_FAIL);
		return READ_ERROR;
	}
	// ȡ�ļ���С
	long _size = ftell(m_fp);
	return _size;
}

/*---------------------------------------------------------------------------
���ܣ�GetFileName��ͨ���ļ�·����ȡ�ļ���
������const char* _strFilePath���ļ�·��
����ֵ��const char*�ļ���
ע�⣺
---------------------------------------------------------------------------*/
string CFileModel::GetFileName(const char* _strFilePath)
{
	string strFullName = _strFilePath;
	// ·��Ϊ��ֱ�ӷ���
	if (strFullName.empty())
	{
		return "";
	}
	
	// �滻�ļ�·���ָ���
	ReplaceStr(strFullName, "/", "\\");
	// ��ȡ�ļ���
	string::size_type iPos = strFullName.find_last_of('\\') + 1;
	return strFullName.substr(iPos, strFullName.length() - iPos);
}

/********************CFileModel��˽�з���*********************************/

/*---------------------------------------------------------------------------
���ܣ�ReplaceStr�����ַ�����strsrc�滻��strdst
������string &strBig ���滻�ַ�, const string &strsrc ԭʼ�ַ���, const string &strdst Ŀ���ַ���
����ֵ����
ע�⣺
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
