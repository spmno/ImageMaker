
// ImageMaker.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CImageMakerApp:
// �йش����ʵ�֣������ ImageMaker.cpp
//

class CImageMakerApp : public CWinApp
{
public:
	CImageMakerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CImageMakerApp theApp;