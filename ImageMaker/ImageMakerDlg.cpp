
// ImageMakerDlg.cpp : ʵ���ļ�
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


// CImageMakerDlg �Ի���



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

// CImageMakerDlg ��Ϣ�������

BOOL CImageMakerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//_CrtSetBreakAlloc(592);
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
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
	logoFilePath_ = pathManager.GetLogo1Path().c_str();			// ��ȡͼƬ·��
	char multiPathName[MAX_PATH];
	WideCharToMultiByte(GetACP(), 0, logoFilePath_, logoFilePath_.GetLength()+1, multiPathName, MAX_PATH, NULL, NULL);
	IplImage* ipl = cvLoadImage( multiPathName, 1 );	// ��ȡͼƬ�����浽һ���ֲ����� ipl ��
	if( !ipl )									// �ж��Ƿ�ɹ���ȡͼƬ
		return FALSE;
	if( logo1Image_ )								// ����һ����ʾ��ͼƬ��������
		cvZero( logo1Image_ );
	
	ResizeImage( ipl, 1 );	// �Զ����ͼƬ�������ţ�ʹ��������ֵ�߸պõ��� 256���ٸ��Ƶ� TheImage ��
	ShowImage( logo1Image_, IDC_STATIC_LOGO1 );			// ������ʾͼƬ����	
	cvReleaseImage( &ipl );	
	
	logo2FilePath_ = pathManager.GetLogo2Path().c_str();	// ��ȡͼƬ·��
	WideCharToMultiByte(GetACP(), 0, logo2FilePath_, logo2FilePath_.GetLength()+1, multiPathName, MAX_PATH, NULL, NULL);
	IplImage* ipl2 = cvLoadImage( multiPathName, 1 );	// ��ȡͼƬ�����浽һ���ֲ����� ipl ��
	if( !ipl2 )									// �ж��Ƿ�ɹ���ȡͼƬ
		return FALSE;
	if( logo2Image_ )								// ����һ����ʾ��ͼƬ��������
		cvZero( logo2Image_ );
	
	ResizeImage( ipl2, 2 );	// �Զ����ͼƬ�������ţ�ʹ��������ֵ�߸պõ��� 256���ٸ��Ƶ� TheImage ��
	ShowImage( logo2Image_, IDC_STATIC_LOGO2 );			// ������ʾͼƬ����	
	cvReleaseImage( &ipl2 );						// �ͷ� ipl ռ�õ��ڴ�// �ͷ� ipl ռ�õ��ڴ�
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CImageMakerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);


	}
	else
	{
		CDialogEx::OnPaint();
		CDialog::UpdateWindow();				// ����windows���ڣ�������ⲽ���ã�ͼƬ��ʾ�����������
		ShowImage( logo1Image_, IDC_STATIC_LOGO1 );		// �ػ�ͼƬ����
		ShowImage(logo2Image_, IDC_STATIC_LOGO2);

	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CImageMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CImageMakerDlg::OnBnClickedButtonSelectNk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	makeButton_.EnableWindow(FALSE);
	UpdateData(TRUE);
	if (nkPathName_.GetLength() == 0)
	{
		MessageBox(_T("��ѡ��NK"));
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}


void CImageMakerDlg::OnBnClickedButtonSelectLogo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog dlg(
		TRUE, _T("*.bmp"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image files (*.bmp; *.jpg) |*.bmp; *.jpg | All Files (*.*) |*.*||"), NULL
		);										// ѡ��ͼƬ��Լ��
	dlg.m_ofn.lpstrTitle = _T("Open Image");	// ���ļ��Ի���ı�����
	if( dlg.DoModal() != IDOK )					// �ж��Ƿ���ͼƬ
		return;
	
	CopyFile(dlg.GetPathName().GetBuffer(), logoFilePath_.GetBuffer(), FALSE);
	Sleep(500);
	char multiPathName[MAX_PATH];
	WideCharToMultiByte(GetACP(), 0, logoFilePath_, logoFilePath_.GetLength()+1, multiPathName, MAX_PATH, NULL, NULL);
	IplImage* ipl = cvLoadImage( multiPathName, 1 );	// ��ȡͼƬ�����浽һ���ֲ����� ipl ��
	if( !ipl )									// �ж��Ƿ�ɹ���ȡͼƬ
		return;
	if( logo1Image_ )								// ����һ����ʾ��ͼƬ��������
		cvZero( logo1Image_ );
	
	ResizeImage( ipl, 1 );	// �Զ����ͼƬ�������ţ�ʹ��������ֵ�߸պõ��� 256���ٸ��Ƶ� TheImage ��
	ShowImage( logo1Image_, IDC_STATIC_LOGO1 );			// ������ʾͼƬ����	
	cvReleaseImage( &ipl );						// �ͷ� ipl ռ�õ��ڴ�
}



void CImageMakerDlg::OnBnClickedButtonSelectLogo2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog dlg(
		TRUE, _T("*.bmp"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image files (*.bmp; *.jpg) |*.bmp; *.jpg | All Files (*.*) |*.*||"), NULL
		);										// ѡ��ͼƬ��Լ��
	dlg.m_ofn.lpstrTitle = _T("Open Image");	// ���ļ��Ի���ı�����
	if( dlg.DoModal() != IDOK )					// �ж��Ƿ���ͼƬ
		return;
	
	CopyFile(dlg.GetPathName().GetBuffer(), logo2FilePath_.GetBuffer(), FALSE);			// ��ȡͼƬ·��
	Sleep(500);
	char multiPathName[MAX_PATH];
	WideCharToMultiByte(GetACP(), 0, logo2FilePath_, logo2FilePath_.GetLength()+1, multiPathName, MAX_PATH, NULL, NULL);
	IplImage* ipl = cvLoadImage( multiPathName, 1 );	// ��ȡͼƬ�����浽һ���ֲ����� ipl ��
	if( !ipl )									// �ж��Ƿ�ɹ���ȡͼƬ
		return;
	if( logo2Image_ )								// ����һ����ʾ��ͼƬ��������
		cvZero( logo2Image_ );
	
	ResizeImage( ipl, 2 );	// �Զ����ͼƬ�������ţ�ʹ��������ֵ�߸պõ��� 256���ٸ��Ƶ� TheImage ��
	ShowImage( logo2Image_, IDC_STATIC_LOGO2 );			// ������ʾͼƬ����	
	cvReleaseImage( &ipl );						// �ͷ� ipl ռ�õ��ڴ�
}

void CImageMakerDlg::ShowImage( IplImage* img, UINT ID )	// ID ��Picture Control�ؼ���ID��
{
	CDC* pDC = GetDlgItem( ID ) ->GetDC();		// �����ʾ�ؼ��� DC
	HDC hDC = pDC ->GetSafeHdc();				// ��ȡ HDC(�豸���) �����л�ͼ����

	CRect rect;
	GetDlgItem(ID) ->GetClientRect( &rect );
	int rw = rect.right - rect.left;			// ���ͼƬ�ؼ��Ŀ�͸�
	int rh = rect.bottom - rect.top;
	int iw = img->width;						// ��ȡͼƬ�Ŀ�͸�
	int ih = img->height;
	int tx = (int)(rw - iw)/2;					// ʹͼƬ����ʾλ�������ڿؼ�������
	int ty = (int)(rh - ih)/2;
	SetRect( rect, tx, ty, tx+iw, ty+ih );

	CvvImage cimg;
	cimg.CopyOf( img );							// ����ͼƬ
	cimg.DrawToHDC( hDC, &rect );				// ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������

	ReleaseDC( pDC );
}

void CImageMakerDlg::ResizeImage(IplImage* img, int index)
{
	// ��ȡͼƬ�Ŀ�͸�
    int w = img->width;
	int h = img->height;

	// �ҳ���͸��еĽϴ�ֵ��
	int max = (w > h)? w: h;

	// ���㽫ͼƬ���ŵ�TheImage��������ı�������
	float scale = (float) ( (float) max / 256.0f );
	
	// ���ź�ͼƬ�Ŀ�͸�
	int nw = (int)( w/scale );
	int nh = (int)( h/scale );

	// Ϊ�˽����ź��ͼƬ���� TheImage �����в�λ�������ͼƬ�� TheImage ���Ͻǵ���������ֵ
	int tlx = (nw > nh)? 0: (int)(256-nw)/2;
	int tly = (nw > nh)? (int)(256-nh)/2: 0;

	if (index == 1)
	{
		// ���� TheImage �� ROI ������������ͼƬ img
		cvSetImageROI( logo1Image_, cvRect( 0, 0, nw, nh) );

		// ��ͼƬ img �������ţ������뵽 TheImage ��
		cvResize( img, logo1Image_ );

		// ���� TheImage �� ROI ׼��������һ��ͼƬ
		cvResetImageROI( logo1Image_ );
	}
	else
	{
		// ���� TheImage �� ROI ������������ͼƬ img
		cvSetImageROI( logo2Image_, cvRect( 0, 0, nw, nh) );

		// ��ͼƬ img �������ţ������뵽 TheImage ��
		cvResize( img, logo2Image_ );

		// ���� TheImage �� ROI ׼��������һ��ͼƬ
		cvResetImageROI( logo2Image_ );
	}

}


void CImageMakerDlg::OnBnClickedButtonBootload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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