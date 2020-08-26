/******************************************************************
模块名：CUtility
文　件：Utility.h, Utility.cpp	
创建人：Wang WenJie
创建日期：2019-8-2
版  本：v1.0.0
功能说明：网络应用程序工具实现文件
*******************************************************************/

#include "stdafx.h"
#include "Utility.h"
#include "../Config/AppConfig.h"


/*---------------------------------------------------------------------------
功能：GetHostIPAddress，获取本机IP地址
参数: 无
返回值：成功返回本机IP地址，失败返回空
注意：
---------------------------------------------------------------------------*/
CString CUtility::GetHostIPAddress()
{
	WORD wVersionRequested;
	// 初始化套接字
	WSADATA wsaData;
	char name[MAX_BUFF_LEN];
	CString ip = _T("");
	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2,0);
	// 验证套接字
	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		// 获取主机名
		if( gethostname( name, sizeof(name) ) == 0 )
		{
			// 通过主机名获取主机信息
			if((hostinfo = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa(*(struct in_addr * ) *hostinfo->h_addr_list);
			}   
		}
		// 关闭
		WSACleanup();
	}
	return ip;
}

/*---------------------------------------------------------------------------
功能：IP2CString，IP控件转CString
参数: IP控件
返回值：IP控件中的IP地址
注意：
---------------------------------------------------------------------------*/
CString CUtility::IP2CString(CIPAddressCtrl* _ctrl)
{
	BYTE nf1, nf2, nf3, nf4;
	_ctrl->GetAddress(nf1, nf2, nf3, nf4);
	CString _ip;
	_ip.Format(_T("%d.%d.%d.%d"), nf1, nf2, nf3, nf4);
	return _ip;
}

/*---------------------------------------------------------------------------
功能：BrowseFile，浏览要发送的文件
参数: 无
返回值：成功返回文件地址
注意：
---------------------------------------------------------------------------*/
CString CUtility::BrowseFile()
{
	CFileDialog* pDlg;
	pDlg = new CFileDialog(TRUE);
	CString _strFilePath = _T("");
	if(pDlg->DoModal() == IDOK)
	{
		_strFilePath = pDlg->GetPathName();
	}
	// 释放内存
	delete pDlg;
	pDlg = NULL;
	return _strFilePath;
}

/*---------------------------------------------------------------------------
功能：GetFileSize，获取文件大小
参数: 文件路径
返回值：成功返回文件大小带单位
注意：
---------------------------------------------------------------------------*/
CString CUtility::GetFileSize(CString _filePath)
{
	ULONGLONG _size;
	CString _fileSize = _T("0 KB");
	// 没有文件路径直接返回
	if(_filePath == _T(""))
	{
		return _fileSize;
	}
	CFileStatus _status;
	// 获取文件大小
	if(CFile::GetStatus(_filePath, _status))
	{
		_size = _status.m_size;
		// 格式化输出
		if(_size < KB)
		{
			// 字节
			_fileSize.Format(_T("%ld B"), _size);
		}
		else if(_size > KB && _size < MB)
		{
			// KB
			double _tmp = (double)_size / KB;
			_fileSize.Format(_T("%.2f KB"), _tmp);
		}
		else if(_size > MB && _size < GB)
		{
			// MB
			double _tmp = (double)_size / MB;
			_fileSize.Format(_T("%.2f MB"), _tmp);
		}
		else if(_size > GB)
		{
			// GB
			double _tmp = (double)_size / GB;
			_fileSize.Format(_T("%.2f GB"), _tmp);
		}
	}
	
	return _fileSize;
}

/*---------------------------------------------------------------------------
功能：BrowseFolder，浏览文件夹
参数: 无
返回值：成功返回选择的文件夹
注意：
---------------------------------------------------------------------------*/
CString CUtility::BrowseFolder()
{
	// 文件返回路径
	TCHAR szFolderPath[MAX_PATH] = {0};
	CString strFolderPath = _T("");

	// 浏览文件夹对话框信息配置
	BROWSEINFO sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	// 文件夹目录的根
	sInfo.pidlRoot = 0; 
	// 对话框外观和功能标志
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX; 
	// 处理事件回调函数
	sInfo.lpfn = NULL; 

	// 显示选择文件夹对话框
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if(lpidlBrowse != NULL)
	{
		if(::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			// 复制文件夹路径
			strFolderPath = szFolderPath;
		}
		// 释放内存
		::CoTaskMemFree(lpidlBrowse);
	}
	return strFolderPath;
}

/*---------------------------------------------------------------------------
功能：GetWorkDir，获取当前程序路径
参数: 无
返回值：成功返回当前程序路径
注意：
---------------------------------------------------------------------------*/

CString CUtility::GetWorkDir()
{
	TCHAR _dir[MAX_FILE_PATH_LEN];
	int _pos = GetCurrentDirectory(MAX_FILE_PATH_LEN, _dir);
	CString _workDir = _T("");
	// 如果失败返回空路径
	if(_pos < 0)
	{
		return _workDir;
	}
	// 返回当前路径
	_workDir = _dir;
	return _workDir;
}

