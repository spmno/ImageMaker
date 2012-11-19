#include "stdafx.h"
#include "MakerTools.h"
#include "PathManager.h"
#include <boost/filesystem.hpp>
CMakerTools::CMakerTools(void)
{
}


CMakerTools::~CMakerTools(void)
{
}

BOOL CMakerTools::CopyFolder(LPCTSTR lpszFromPath,LPCTSTR lpszToPath)
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

bool CMakerTools::SelectDir(const wstring& title, wstring& selectedDir) const
{
	CPathManager pathManager;
	BROWSEINFO  bi;
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = title.c_str();
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.iImage = 0;
	LPITEMIDLIST pidlSource = NULL;
	ULONG charsParsed = 0;
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidlSource);
	LPSHELLFOLDER pShellFolder = NULL;
	HRESULT hr = SHGetDesktopFolder(&pShellFolder);
	TCHAR topDir[MAX_PATH];
	_tcscpy(topDir, pathManager.GetTopDirPath().c_str());
	hr = pShellFolder->ParseDisplayName(NULL, NULL, topDir, &charsParsed, &pidlSource, NULL);
	bi.pidlRoot=pidlSource;
	LPCITEMIDLIST pidl=SHBrowseForFolder(&bi);
	pShellFolder->Release();
	if(!pidl)
		return false;
	TCHAR  szDisplayName[255];
	SHGetPathFromIDList(pidl,szDisplayName);
	selectedDir = szDisplayName;
	return true;
}

bool CMakerTools::CopyFileInt(const wstring& sourceFileName, const wstring& targetFileName) const
{
	
	const boost::filesystem::path currentPath(sourceFileName);
	const boost::filesystem::path targetPath(targetFileName);

	if (!boost::filesystem::exists(currentPath))
	{
		MessageBox(NULL, sourceFileName.c_str(), _T("’“≤ªµΩ"), MB_OK|MB_TOPMOST);
		ExitProcess(-1);
	}

	try
	{
		boost::filesystem::copy_file(currentPath, targetPath, boost::filesystem::copy_option::overwrite_if_exists);
	}
	catch(boost::filesystem::filesystem_error e)
	{
		TCHAR errorReport[MAX_PATH];
		MultiByteToWideChar(GetACP(), 0, e.what(), strlen(e.what()), errorReport, MAX_PATH);
		MessageBox(NULL, errorReport, _T("copy file error"), MB_OK|MB_TOPMOST);
		OutputDebugStringA(e.what());
		return false;
	}

	return true;
}

bool CMakerTools::ExcuteCommand(const wstring& sourceExeFilePath, const wstring& params) const
{
	CPathManager pathManager;
	SHELLEXECUTEINFO exeInfo;
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = sourceExeFilePath.c_str();
	exeInfo.lpParameters = params.c_str();
	exeInfo.lpDirectory = pathManager.GetRootPath().c_str();;
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	return true;
}