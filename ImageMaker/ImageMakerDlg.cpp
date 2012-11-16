
// ImageMakerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <boost/filesystem.hpp>
#include <tchar.h>
#include "ImageMaker.h"
#include "ImageMakerDlg.h"
#include "afxdialogex.h"
#include "CvvImage.h"
#include "PathManager.h"
#include "ProjectParser.h"
#include "LogManager.h"
#include "AreaHelper.h"

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 154
#define IMAGE_CHANNELS 3

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageMakerDlg 对话框



CImageMakerDlg::CImageMakerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageMakerDlg::IDD, pParent)
	, memorySize_(0)
	, nkPathName_(_T(""))
	, logoFilePath_(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO1, memorySize_);
	DDX_Text(pDX, IDC_EDIT1, nkPathName_);
	DDX_Control(pDX, IDC_COMBO1, projectSelector);
	DDX_Control(pDX, IDC_BUTTON_MAKE, makeButton_);
}

BEGIN_MESSAGE_MAP(CImageMakerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_NK, &CImageMakerDlg::OnBnClickedButtonSelectNk)
	ON_BN_CLICKED(IDC_BUTTON_MAKE, &CImageMakerDlg::OnBnClickedButtonMake)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CImageMakerDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_LOGO, &CImageMakerDlg::OnBnClickedButtonSelectLogo)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_LOGO2, &CImageMakerDlg::OnBnClickedButtonSelectLogo2)
	ON_BN_CLICKED(IDC_BUTTON_BOOTLOAD, &CImageMakerDlg::OnBnClickedButtonBootload)
	ON_BN_CLICKED(IDC_BUTTON_APP, &CImageMakerDlg::OnBnClickedButtonApp)
END_MESSAGE_MAP()

// CImageMakerDlg 消息处理程序

BOOL CImageMakerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//_CrtSetBreakAlloc(592);
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CProjectParser parser;
	for (auto projectName : parser.GetProjects())
	{
		projectSelector.AddString(projectName.c_str());
	}
	projectSelector.SetCurSel(0);
	
	CvSize ImgSize;
	ImgSize.height = IMAGE_HEIGHT;
	ImgSize.width = IMAGE_WIDTH;
	logo1Image_ = cvCreateImage( ImgSize, IPL_DEPTH_8U, IMAGE_CHANNELS );
	logo2Image_ = cvCreateImage( ImgSize, IPL_DEPTH_8U, IMAGE_CHANNELS );

	CPathManager pathManager;
	logoFilePath_ = pathManager.GetLogo1Path().c_str();			// 获取图片路径
	char multiPathName[MAX_PATH];
	WideCharToMultiByte(GetACP(), 0, logoFilePath_, logoFilePath_.GetLength()+1, multiPathName, MAX_PATH, NULL, NULL);
	IplImage* ipl = cvLoadImage( multiPathName, 1 );	// 读取图片、缓存到一个局部变量 ipl 中
	if( !ipl )									// 判断是否成功读取图片
		return FALSE;
	if( logo1Image_ )								// 对上一幅显示的图片数据清零
		cvZero( logo1Image_ );
	
	ResizeImage( ipl, 1 );	// 对读入的图片进行缩放，使其宽或高最大值者刚好等于 256，再复制到 TheImage 中
	ShowImage( logo1Image_, IDC_STATIC_LOGO1 );			// 调用显示图片函数	
	cvReleaseImage( &ipl );	
	
	logo2FilePath_ = pathManager.GetLogo2Path().c_str();	// 获取图片路径
	WideCharToMultiByte(GetACP(), 0, logo2FilePath_, logo2FilePath_.GetLength()+1, multiPathName, MAX_PATH, NULL, NULL);
	IplImage* ipl2 = cvLoadImage( multiPathName, 1 );	// 读取图片、缓存到一个局部变量 ipl 中
	if( !ipl2 )									// 判断是否成功读取图片
		return FALSE;
	if( logo2Image_ )								// 对上一幅显示的图片数据清零
		cvZero( logo2Image_ );
	
	ResizeImage( ipl2, 2 );	// 对读入的图片进行缩放，使其宽或高最大值者刚好等于 256，再复制到 TheImage 中
	ShowImage( logo2Image_, IDC_STATIC_LOGO2 );			// 调用显示图片函数	
	cvReleaseImage( &ipl2 );						// 释放 ipl 占用的内存// 释放 ipl 占用的内存
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CImageMakerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);


	}
	else
	{
		CDialogEx::OnPaint();
		CDialog::UpdateWindow();				// 更新windows窗口，如果无这步调用，图片显示还会出现问题
		ShowImage( logo1Image_, IDC_STATIC_LOGO1 );		// 重绘图片函数
		ShowImage(logo2Image_, IDC_STATIC_LOGO2);

	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImageMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CImageMakerDlg::OnBnClickedButtonSelectNk()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDialog(TRUE);
	if (fileDialog.DoModal() == IDOK)
	{
		nkPathName_ = fileDialog.GetPathName();
		
		wstring nkPath(nkPathName_.GetBuffer());
		wstring nkName(fileDialog.GetFileName().GetBuffer());
		int splitPostion = nkName.find(_T("_"));
		if (splitPostion != string::npos)
		{
			wstring codeName = nkName.substr(splitPostion+1, 4);
			CAreaHelper areaHelper;
			areaHelper.GetAreaName(codeName);
			imageMakerImp_.SetImageTail(codeName);
		}
		imageMakerImp_.SetNKPath(nkPath);
		wstring nkIndex = _T("NK");
		wstring nkLogContent(_T("NKPATH:"));
		nkLogContent += nkPath;
		CLogManager::GetInstance().AddLog(nkIndex, nkLogContent);
		UpdateData(FALSE);
	}
}


void CImageMakerDlg::OnBnClickedButtonMake()
{
	// TODO: 在此添加控件通知处理程序代码
	makeButton_.EnableWindow(FALSE);
	UpdateData(TRUE);
	if (nkPathName_.GetLength() == 0)
	{
		MessageBox(_T("请选择NK"));
		return ;
	}
	if (memorySize_ == 0)
	{
		imageMakerImp_.SetMemorySize(CImageMakerImp::MEMORY_256M);
	}
	else
	{
		imageMakerImp_.SetMemorySize(CImageMakerImp::MEMORY_512M);
	}

	
	int index = projectSelector.GetCurSel();
	CString strCBText;
	projectSelector.GetLBText( index, strCBText);
	wstring projectName = strCBText.GetBuffer();
	imageMakerImp_.SetLCDKind(projectName);

	imageMakerImp_.Make();
	makeButton_.EnableWindow();
}


void CImageMakerDlg::OnBnClickedButtonExit()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}


void CImageMakerDlg::OnBnClickedButtonSelectLogo()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(
		TRUE, _T("*.bmp"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image files (*.bmp; *.jpg) |*.bmp; *.jpg | All Files (*.*) |*.*||"), NULL
		);										// 选项图片的约定
	dlg.m_ofn.lpstrTitle = _T("Open Image");	// 打开文件对话框的标题名
	if( dlg.DoModal() != IDOK )					// 判断是否获得图片
		return;
	
	CopyFile(dlg.GetPathName().GetBuffer(), logoFilePath_.GetBuffer(), FALSE);
	Sleep(500);
	char multiPathName[MAX_PATH];
	WideCharToMultiByte(GetACP(), 0, logoFilePath_, logoFilePath_.GetLength()+1, multiPathName, MAX_PATH, NULL, NULL);
	IplImage* ipl = cvLoadImage( multiPathName, 1 );	// 读取图片、缓存到一个局部变量 ipl 中
	if( !ipl )									// 判断是否成功读取图片
		return;
	if( logo1Image_ )								// 对上一幅显示的图片数据清零
		cvZero( logo1Image_ );
	
	ResizeImage( ipl, 1 );	// 对读入的图片进行缩放，使其宽或高最大值者刚好等于 256，再复制到 TheImage 中
	ShowImage( logo1Image_, IDC_STATIC_LOGO1 );			// 调用显示图片函数	
	cvReleaseImage( &ipl );						// 释放 ipl 占用的内存
}



void CImageMakerDlg::OnBnClickedButtonSelectLogo2()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(
		TRUE, _T("*.bmp"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image files (*.bmp; *.jpg) |*.bmp; *.jpg | All Files (*.*) |*.*||"), NULL
		);										// 选项图片的约定
	dlg.m_ofn.lpstrTitle = _T("Open Image");	// 打开文件对话框的标题名
	if( dlg.DoModal() != IDOK )					// 判断是否获得图片
		return;
	
	CopyFile(dlg.GetPathName().GetBuffer(), logo2FilePath_.GetBuffer(), FALSE);			// 获取图片路径
	Sleep(500);
	char multiPathName[MAX_PATH];
	WideCharToMultiByte(GetACP(), 0, logo2FilePath_, logo2FilePath_.GetLength()+1, multiPathName, MAX_PATH, NULL, NULL);
	IplImage* ipl = cvLoadImage( multiPathName, 1 );	// 读取图片、缓存到一个局部变量 ipl 中
	if( !ipl )									// 判断是否成功读取图片
		return;
	if( logo2Image_ )								// 对上一幅显示的图片数据清零
		cvZero( logo2Image_ );
	
	ResizeImage( ipl, 2 );	// 对读入的图片进行缩放，使其宽或高最大值者刚好等于 256，再复制到 TheImage 中
	ShowImage( logo2Image_, IDC_STATIC_LOGO2 );			// 调用显示图片函数	
	cvReleaseImage( &ipl );						// 释放 ipl 占用的内存
}

void CImageMakerDlg::ShowImage( IplImage* img, UINT ID )	// ID 是Picture Control控件的ID号
{
	CDC* pDC = GetDlgItem( ID ) ->GetDC();		// 获得显示控件的 DC
	HDC hDC = pDC ->GetSafeHdc();				// 获取 HDC(设备句柄) 来进行绘图操作

	CRect rect;
	GetDlgItem(ID) ->GetClientRect( &rect );
	int rw = rect.right - rect.left;			// 求出图片控件的宽和高
	int rh = rect.bottom - rect.top;
	int iw = img->width;						// 读取图片的宽和高
	int ih = img->height;
	int tx = (int)(rw - iw)/2;					// 使图片的显示位置正好在控件的正中
	int ty = (int)(rh - ih)/2;
	SetRect( rect, tx, ty, tx+iw, ty+ih );

	CvvImage cimg;
	cimg.CopyOf( img );							// 复制图片
	cimg.DrawToHDC( hDC, &rect );				// 将图片绘制到显示控件的指定区域内

	ReleaseDC( pDC );
}

void CImageMakerDlg::ResizeImage(IplImage* img, int index)
{
	// 读取图片的宽和高
    int w = img->width;
	int h = img->height;

	// 找出宽和高中的较大值者
	int max = (w > h)? w: h;

	// 计算将图片缩放到TheImage区域所需的比例因子
	float scale = (float) ( (float) max / 256.0f );
	
	// 缩放后图片的宽和高
	int nw = (int)( w/scale );
	int nh = (int)( h/scale );

	// 为了将缩放后的图片存入 TheImage 的正中部位，需计算图片在 TheImage 左上角的期望坐标值
	int tlx = (nw > nh)? 0: (int)(256-nw)/2;
	int tly = (nw > nh)? (int)(256-nh)/2: 0;

	if (index == 1)
	{
		// 设置 TheImage 的 ROI 区域，用来存入图片 img
		cvSetImageROI( logo1Image_, cvRect( 0, 0, nw, nh) );

		// 对图片 img 进行缩放，并存入到 TheImage 中
		cvResize( img, logo1Image_ );

		// 重置 TheImage 的 ROI 准备读入下一幅图片
		cvResetImageROI( logo1Image_ );
	}
	else
	{
		// 设置 TheImage 的 ROI 区域，用来存入图片 img
		cvSetImageROI( logo2Image_, cvRect( 0, 0, nw, nh) );

		// 对图片 img 进行缩放，并存入到 TheImage 中
		cvResize( img, logo2Image_ );

		// 重置 TheImage 的 ROI 准备读入下一幅图片
		cvResetImageROI( logo2Image_ );
	}

}


void CImageMakerDlg::OnBnClickedButtonBootload()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO  bi;
	bi.hwndOwner=NULL;
	bi.pidlRoot=NULL;
	bi.pszDisplayName=NULL;
	bi.lpszTitle=NULL;
	bi.ulFlags=0;
	bi.lpfn =NULL;
	bi.iImage =0;
	LPCITEMIDLIST pidl=SHBrowseForFolder(&bi);
	if(!pidl)
		return;
	TCHAR  szDisplayName[255];
	SHGetPathFromIDList(pidl,szDisplayName);

	CPathManager pathManager;
	CopyFolder(szDisplayName, pathManager.GetRootPath().c_str());
	wstring index = _T("BootLoad");
	wstring logContent(_T("BootLoadPath:"));
	logContent += szDisplayName;
	CLogManager::GetInstance().AddLog(index, logContent);
	//MessageBox(str,NULL,MB_OK);
}


void CImageMakerDlg::OnBnClickedButtonApp()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO  bi;
	bi.hwndOwner=NULL;
	bi.pidlRoot=NULL;
	bi.pszDisplayName=NULL;
	bi.lpszTitle=NULL;
	bi.ulFlags=0;
	bi.lpfn =NULL;
	bi.iImage =0;
	LPCITEMIDLIST pidl=SHBrowseForFolder(&bi);
	if(!pidl)
		return;
	TCHAR  szDisplayName[255];
	SHGetPathFromIDList(pidl,szDisplayName);
	
	CPathManager pathManager;
	CopyFolder(szDisplayName, pathManager.GetRootPath().c_str());
	wstring index = _T("App");
	wstring logContent(_T("AppPath:"));
	logContent += szDisplayName;
	CLogManager::GetInstance().AddLog(index, logContent);
	//MessageBox(str,NULL,MB_OK);
}

BOOL CImageMakerDlg::CopyFolder(LPCTSTR lpszFromPath,LPCTSTR lpszToPath)
{
	int nLengthFrm = _tcslen(lpszFromPath);

	TCHAR *NewPathFrm = new TCHAR[nLengthFrm+2];

	_tcscpy(NewPathFrm,lpszFromPath);

	NewPathFrm[nLengthFrm] = '\0';

	NewPathFrm[nLengthFrm+1] = '\0';

	SHFILEOPSTRUCT FileOp;

	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_NOCONFIRMATION ;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = NewPathFrm;
	FileOp.pTo = lpszToPath;
	FileOp.wFunc = FO_COPY;

	return SHFileOperation(&FileOp) == 0;

}