
// ImageMakerDlg.h : ͷ�ļ�
//

#pragma once
#include <opencv\cv.h>
#include "ImageMakerImp.h"
#include "afxwin.h"


// CImageMakerDlg �Ի���
class CImageMakerDlg : public CDialogEx
{
// ����
public:
	CImageMakerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IMAGEMAKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	afx_msg void OnBnClickedButtonSelectNk();
	afx_msg void OnBnClickedButtonMake();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonSelectLogo();
	afx_msg void OnBnClickedButtonBootfs();
	afx_msg void OnCbnSelchangeCombo1();
	void ShowImage(IplImage* img, UINT ID);
	void ResizeImage(IplImage* img, int index);
	CImageMakerImp imageMakerImp_;
	int memorySize_;
	CString nkPathName_;
	CString logoFilePath_;
	CString logo2FilePath_;
	IplImage* logo1Image_;
	IplImage* logo2Image_;
	afx_msg void OnBnClickedButtonSelectLogo2();
	afx_msg void OnBnClickedButtonBootload();
	afx_msg void OnBnClickedButtonApp();
	CComboBox projectSelector;
	CButton makeButton_;

};
