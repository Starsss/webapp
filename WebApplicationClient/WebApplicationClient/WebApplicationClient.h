
// WebApplicationClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "../../Runtime/Include.h"



// CWebApplicationClientApp:
// �йش����ʵ�֣������ WebApplicationClient.cpp
//

class CWebApplicationClientApp : public CWinAppEx
{
public:
	CWebApplicationClientApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWebApplicationClientApp theApp;