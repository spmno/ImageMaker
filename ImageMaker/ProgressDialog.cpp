// ProgressDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImageMaker.h"
#include "ProgressDialog.h"
#include "afxdialogex.h"


// CProgressDialog �Ի���

IMPLEMENT_DYNAMIC(CProgressDialog, CDialogEx)

CProgressDialog::CProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgressDialog::IDD, pParent)
{

}

CProgressDialog::~CProgressDialog()
{
}

void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, progressController_);
}


BEGIN_MESSAGE_MAP(CProgressDialog, CDialogEx)
END_MESSAGE_MAP()


// CProgressDialog ��Ϣ�������
