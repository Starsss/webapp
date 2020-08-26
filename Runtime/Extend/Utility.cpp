/******************************************************************
ģ������CUtility
�ġ�����Utility.h, Utility.cpp	
�����ˣ�Wang WenJie
�������ڣ�2019-8-2
��  ����v1.0.0
����˵��������Ӧ�ó��򹤾�ʵ���ļ�
*******************************************************************/

#include "stdafx.h"
#include "Utility.h"
#include "../Config/AppConfig.h"


/*---------------------------------------------------------------------------
���ܣ�GetHostIPAddress����ȡ����IP��ַ
����: ��
����ֵ���ɹ����ر���IP��ַ��ʧ�ܷ��ؿ�
ע�⣺
---------------------------------------------------------------------------*/
CString CUtility::GetHostIPAddress()
{
	WORD wVersionRequested;
	// ��ʼ���׽���
	WSADATA wsaData;
	char name[MAX_BUFF_LEN];
	CString ip = _T("");
	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2,0);
	// ��֤�׽���
	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		// ��ȡ������
		if( gethostname( name, sizeof(name) ) == 0 )
		{
			// ͨ����������ȡ������Ϣ
			if((hostinfo = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa(*(struct in_addr * ) *hostinfo->h_addr_list);
			}   
		}
		// �ر�
		WSACleanup();
	}
	return ip;
}

/*---------------------------------------------------------------------------
���ܣ�IP2CString��IP�ؼ�תCString
����: IP�ؼ�
����ֵ��IP�ؼ��е�IP��ַ
ע�⣺
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
���ܣ�BrowseFile�����Ҫ���͵��ļ�
����: ��
����ֵ���ɹ������ļ���ַ
ע�⣺
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
	// �ͷ��ڴ�
	delete pDlg;
	pDlg = NULL;
	return _strFilePath;
}

/*---------------------------------------------------------------------------
���ܣ�GetFileSize����ȡ�ļ���С
����: �ļ�·��
����ֵ���ɹ������ļ���С����λ
ע�⣺
---------------------------------------------------------------------------*/
CString CUtility::GetFileSize(CString _filePath)
{
	ULONGLONG _size;
	CString _fileSize = _T("0 KB");
	// û���ļ�·��ֱ�ӷ���
	if(_filePath == _T(""))
	{
		return _fileSize;
	}
	CFileStatus _status;
	// ��ȡ�ļ���С
	if(CFile::GetStatus(_filePath, _status))
	{
		_size = _status.m_size;
		// ��ʽ�����
		if(_size < KB)
		{
			// �ֽ�
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
���ܣ�BrowseFolder������ļ���
����: ��
����ֵ���ɹ�����ѡ����ļ���
ע�⣺
---------------------------------------------------------------------------*/
CString CUtility::BrowseFolder()
{
	// �ļ�����·��
	TCHAR szFolderPath[MAX_PATH] = {0};
	CString strFolderPath = _T("");

	// ����ļ��жԻ�����Ϣ����
	BROWSEINFO sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	// �ļ���Ŀ¼�ĸ�
	sInfo.pidlRoot = 0; 
	// �Ի�����ۺ͹��ܱ�־
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX; 
	// �����¼��ص�����
	sInfo.lpfn = NULL; 

	// ��ʾѡ���ļ��жԻ���
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if(lpidlBrowse != NULL)
	{
		if(::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			// �����ļ���·��
			strFolderPath = szFolderPath;
		}
		// �ͷ��ڴ�
		::CoTaskMemFree(lpidlBrowse);
	}
	return strFolderPath;
}

/*---------------------------------------------------------------------------
���ܣ�GetWorkDir����ȡ��ǰ����·��
����: ��
����ֵ���ɹ����ص�ǰ����·��
ע�⣺
---------------------------------------------------------------------------*/

CString CUtility::GetWorkDir()
{
	TCHAR _dir[MAX_FILE_PATH_LEN];
	int _pos = GetCurrentDirectory(MAX_FILE_PATH_LEN, _dir);
	CString _workDir = _T("");
	// ���ʧ�ܷ��ؿ�·��
	if(_pos < 0)
	{
		return _workDir;
	}
	// ���ص�ǰ·��
	_workDir = _dir;
	return _workDir;
}

