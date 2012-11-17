
// ImageMakerDlg.h : 头文件
//

#pragma once
#include <opencv\cv.h>
#include "ImageMakerImp.h"
#include "afxwin.h"


// CImageMakerDlg 对话框
class CImageMakerDlg : public CDialogEx
{
// 构造
public:
	CImageMakerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IMAGEMAKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
