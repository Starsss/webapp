
// WebApplicationServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "../../Runtime/Include.h"


// CWebApplicationServerApp:
// �йش����ʵ�֣������ WebApplicationServer.cpp
//

class CWebApplicationServerApp : public CWinAppEx
{
public:
	CWebApplicationServerApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWebApplicationServerApp theApp;