#pragma once
#include "afxcmn.h"
#include "resource.h"


// CProgressDialog �Ի���

class CProgressDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDialog)

public:
	CProgressDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProgressDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl progressController_;
};
