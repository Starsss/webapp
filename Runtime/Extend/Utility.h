/******************************************************************
ģ������CUtility
�ġ�����Utility.h	
�����ˣ�Wang WenJie
�������ڣ�2019-8-2
��  ����v1.0.0
����˵��������Ӧ�ó��򹤾���ͷ�ļ�
*******************************************************************/

#pragma once


class CUtility
{
public:
	static CString GetHostIPAddress();						// ��ȡ����IP��ַ
	static CString IP2CString(CIPAddressCtrl* _ctrl);		// IP��ַ�ؼ�תCString
	static CString BrowseFile();							// ����ļ�
	static CString GetFileSize(CString _strFilePath);		// ��ȡ�ļ���С
	static CString BrowseFolder();							// ����ļ���
	static CString GetWorkDir();							// ��ȡ��ǰ·��

	CUtility();
	~CUtility();
};