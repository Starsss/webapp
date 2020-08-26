/******************************************************************
模块名：CUtility
文　件：Utility.h	
创建人：Wang WenJie
创建日期：2019-8-2
版  本：v1.0.0
功能说明：网络应用程序工具类头文件
*******************************************************************/

#pragma once


class CUtility
{
public:
	static CString GetHostIPAddress();						// 获取本机IP地址
	static CString IP2CString(CIPAddressCtrl* _ctrl);		// IP地址控件转CString
	static CString BrowseFile();							// 浏览文件
	static CString GetFileSize(CString _strFilePath);		// 获取文件大小
	static CString BrowseFolder();							// 浏览文件夹
	static CString GetWorkDir();							// 获取当前路径

	CUtility();
	~CUtility();
};