#include "stdafx.h"
#include "ImageMakerImp.h"
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/archive/codecvt_null.hpp>
#include "PathManager.h"
#include "LogManager.h"
#include "StdioFileEx.h"
#include "ProgressDialog.h"
#include "MakerTools.h"

using namespace std;

CImageMakerImp::CImageMakerImp(void) : 
	memorySize_(MEMORY_INVALID_PARAM),
	imageTail_(_T(""))
{

}


CImageMakerImp::~CImageMakerImp(void)
{
}



bool CImageMakerImp::Make()
{
	CProgressDialog progressDialog;
	progressDialog.Create(IDD_DIALOG_PROGRESS);
	progressDialog.ShowWindow(SW_SHOW);
	progressDialog.CenterWindow();
	FunctionBind();
	progressDialog.progressController_.SetRange(0, functionContainer_.size());
	progressDialog.progressController_.SetStep(1);
	for (auto makeFunction : functionContainer_)
	{
		if (makeFunction())
		{
			progressDialog.progressController_.StepIt();
		}
		else
		{
			progressDialog.DestroyWindow();
			return false;
		}
	}
	CLogManager::GetInstance().PrintLog();
	DeleteMiddleFiles();
	bool makeResult = RenameImage();
	progressDialog.DestroyWindow();
	ShowLog();
	if (makeResult)
	{
		MessageBox(NULL, _T("÷∆◊˜ÕÍ≥…"), _T("ÕÍ≥…"), MB_OK|MB_TOPMOST);
		return true;
	}
	else
	{
		MessageBox(NULL, _T("÷∆◊˜ ß∞‹"), _T("ÕÍ≥…"), MB_OK|MB_TOPMOST);
		return false;
	}
	
}

bool CImageMakerImp::DeleteNKAndImg()
{
	CPathManager pathManager;
	boost::filesystem::remove(pathManager.GetNKPath());
	boost::filesystem::remove(pathManager.GetImgPath());
	return true;
}

bool CImageMakerImp::MemoryConfig()
{
	CPathManager pathManager;
	wstring memoryPath = pathManager.GetMemoryPath();
	wstring targetPathName = memoryPath + _T("wince.ini");
	wstring currentMemoryName(memoryPath);
	boost::wformat logFormat(_T("Memory Size : %1% "));
	if (memorySize_ == MEMORY_256M)
	{
		currentMemoryName += _T("wince-256M.ini");	
		logFormat % _T("256M");
	}
	else
	{
		currentMemoryName += _T("wince-512M.ini");
		logFormat % _T("512M");
	}
	
	wstring logContent = logFormat.str();
	wstring index = _T("Mem");
	CLogManager::GetInstance().AddLog(index, logContent);
	CMakerTools tools;
	return tools.CopyFileInt(currentMemoryName, targetPathName);
}

bool CImageMakerImp::LCDConfig()
{

	CPathManager pathManager;
	wstring&& lcdPath = pathManager.GetLCDPath();
	wstring targetPathName = lcdPath + _T("sys_config1.fex");
	wstring sourcePathName(lcdPath);
	boost::wformat projectFormat(_T("Project : %1%."));
	boost::wformat projectFileNameFormat(_T("sys_config1_%1%.fex"));
	projectFormat % projectName_;
	projectFileNameFormat % projectName_;
	wstring projectFileName = projectFileNameFormat.str();
	sourcePathName += projectFileName;
	if (!boost::filesystem::exists(sourcePathName))
	{
		MessageBox(NULL, sourcePathName.c_str(), _T("’“≤ªµΩŒƒº˛"), MB_OK|MB_TOPMOST);
		ExitProcess(-1);
	}
	wstring projectIndex = _T("Project");
	wstring projectContent = projectFormat.str();
	CLogManager::GetInstance().AddLog(projectIndex, projectContent);
	CMakerTools tools;
	if (!tools.CopyFileInt(sourcePathName, targetPathName))
	{
		MessageBox(NULL, sourcePathName.c_str(), _T("∏¥÷∆œÓƒøŒƒº˛¥ÌŒÛ"), MB_OK|MB_TOPMOST);
		return false;
	}

	wstring filterDir(pathManager.GetTopDirPath());
	
	if (projectName_.compare(_T("H8033V")) == 0)
	{
		filterDir += _T("Filter\\H8033V\\Filter");
	}
	else if (projectName_.compare(_T("DS4389GDA")) == 0)
	{
		filterDir += _T("Filter\\DS4389GDA\\Filter");
	}
	else if (projectName_.compare(_T("BM-5133V")) == 0)
	{
		filterDir += _T("Filter\\DS4389GDA\\Filter");
	}
	wstring targetFilterDir(pathManager.GetRootPath());
	targetFilterDir += _T("root");
	wstring targetFilterPath(pathManager.GetRootPath());
	targetFilterPath += _T("root\\Filter");

	boost::filesystem::remove_all(targetFilterPath);
	Sleep(1000);

	if (tools.CopyFolder(filterDir.c_str(), targetFilterDir.c_str()) == FALSE)
	{
		MessageBox(NULL, filterDir.c_str(), _T("Ω‚¬Î∆˜∏¥÷∆ ß∞‹"), MB_OK|MB_TOPMOST);
		return false;
	}
	wstring filterIndex = _T("filter");
	wstring filterContent(_T("Ω‚¬Î∆˜£∫"));
	filterContent += filterDir;
	CLogManager::GetInstance().AddLog(filterIndex, filterContent);
	return true;
}

bool CImageMakerImp::DebugReleaseConfig()
{
	CPathManager pathManager;
	wstring&& targetNKPathName = pathManager.GetNKPath();
	CMakerTools tools;
	return tools.CopyFileInt(nkSourcePath_, targetNKPathName);
}

bool CImageMakerImp::MsmbrConfig()
{
	CPathManager pathManager;
	wstring&& msmbrExeName = pathManager.GetMsmbrExePath();
	wstring&& sysConfigFileName = pathManager.GetSysConfigPath();
	boost::wformat argmentFormat(_T("-cfg  %1% -user  FAT_APPFS  -o  msmbr.fex > log.txt"));
	argmentFormat % sysConfigFileName;
	wstring argmentContent = argmentFormat.str();
	SHELLEXECUTEINFO exeInfo;
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_DEFAULT;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = msmbrExeName.c_str();
	exeInfo.lpParameters = argmentContent.c_str();
	exeInfo.lpDirectory = pathManager.GetRootPath().c_str();
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	return true;
}

bool CImageMakerImp::BootfsISOConfig()
{
	CPathManager pathManager;
	wstring&& scriptExeName = pathManager.GetScriptPath();
	wstring&& sysConfig1Path = pathManager.GetSysConfig1Path();
	
	SHELLEXECUTEINFO exeInfo;
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_DEFAULT;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = scriptExeName.c_str();
	exeInfo.lpParameters = sysConfig1Path.c_str();
	exeInfo.lpDirectory = NULL;
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		return false;
	}
	CMakerTools tools;
	if (!tools.CopyFileInt(pathManager.GetSourceNand0PathName(), pathManager.GetTargetNand0PathName()))
	{
		MessageBox(NULL, _T("∏¥÷∆nand boot0 ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	if (!tools.CopyFileInt(pathManager.GetSourceNand1PathName(), pathManager.GetTargetNand1PathName()))
	{
		MessageBox(NULL, _T("∏¥÷∆nand boot1 ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	wstring updateExePath = pathManager.GetUpdateToolPathName();
	boost::wformat argmentFormat(_T(" %1% %2% %3% %4%"));
	argmentFormat % pathManager.GetSysConfig1BinPath();
	argmentFormat % pathManager.GetTargetNand0PathName();
	argmentFormat % pathManager.GetTargetNand1PathName();
	argmentFormat % _T("> log.txt");
	wstring argmentContent = argmentFormat.str();
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_DEFAULT;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = updateExePath.c_str();
	exeInfo.lpParameters = argmentContent.c_str();
	exeInfo.lpDirectory = NULL;
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("…˝º∂nand boot ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	WaitForSingleObject(exeInfo.hProcess, 10000);
	if (!tools.CopyFileInt(pathManager.GetSourceSdcardBoot0PathName(), pathManager.GetTargetSdcardBoot0PathName()))
	{
		MessageBox(NULL, _T("∏¥÷∆sdcard boot0 ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	if (!tools.CopyFileInt(pathManager.GetSourceSdcardBoot1PathName(), pathManager.GetTargetSdcardBoot1PathName()))
	{
		MessageBox(NULL, _T("∏¥÷∆sdcard boot1 ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}


	boost::wformat argmentSdcardFormat(_T(" %1% %2% %3% %4% %5%"));
	argmentSdcardFormat % pathManager.GetSysConfig1BinPath();
	argmentSdcardFormat % pathManager.GetTargetSdcardBoot0PathName();
	argmentSdcardFormat % pathManager.GetTargetSdcardBoot1PathName();
	argmentSdcardFormat % _T("SDMMC_CARD");
	argmentSdcardFormat % _T("> log.txt");
	argmentContent = argmentSdcardFormat.str();
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_DEFAULT;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = updateExePath.c_str();
	exeInfo.lpParameters = argmentContent.c_str();
	exeInfo.lpDirectory = NULL;
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("…˝º∂nand boot ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	boost::filesystem::remove(pathManager.GetBootfsScriptPathName());
	boost::filesystem::remove(pathManager.GetBootfsScript0PathName());

	if (!tools.CopyFileInt(pathManager.GetSysConfig1BinPath(), pathManager.GetBootfsScriptPathName()))
	{
		MessageBox(NULL, _T("∏¥÷∆script ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	if (!tools.CopyFileInt(pathManager.GetSysConfig1BinPath(), pathManager.GetBootfsScript0PathName()))
	{
		MessageBox(NULL, _T("∏¥÷∆script0 ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	return true;
}

bool CImageMakerImp::MBRConfig()
{
	CPathManager pathManager;
	boost::filesystem::remove(pathManager.GetSysConfigBinPath());
	
	SHELLEXECUTEINFO exeInfo;
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_DEFAULT;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = pathManager.GetScriptOldPath().c_str();
	exeInfo.lpParameters = pathManager.GetSysConfigPath().c_str();
	exeInfo.lpDirectory = NULL;
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("script old ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	boost::wformat argmentFormat(_T(" %1% %2% %3%"));
	argmentFormat % pathManager.GetSysConfigBinPath();
	argmentFormat % pathManager.GetCardMBRPath();
	argmentFormat % _T("> log.txt");
	wstring argmentContent = argmentFormat.str();
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_DEFAULT;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = pathManager.GetUpdateMBRPath().c_str();
	exeInfo.lpParameters = argmentContent.c_str();
	exeInfo.lpDirectory = NULL;
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("…˝º∂mbr ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	return true;
}

bool CImageMakerImp::BootfsConfig()
{
	CPathManager pathManager;
	boost::filesystem::remove(pathManager.GetBootfsPath());
	SHELLEXECUTEINFO exeInfo;
	boost::wformat argmentFormat(_T(" %1% %2%"));
	argmentFormat % pathManager.GetBootfsIniPath();
	argmentFormat % pathManager.GetSplitBinPath();

	wstring argmentContent = argmentFormat.str();
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = pathManager.GetFsBuildPath().c_str();
	exeInfo.lpParameters = argmentContent.c_str();
	exeInfo.lpDirectory = pathManager.GetRootPath().c_str();
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("bootfs ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	return true;
}

bool CImageMakerImp::AppfsConfig()
{
	CPathManager pathManager;
	SHELLEXECUTEINFO exeInfo;
	boost::wformat argmentFormat(_T(" %1% %2% %3%"));
	argmentFormat % pathManager.GetRootIniPath();
	argmentFormat % pathManager.GetSplitBinPath();
	argmentFormat % _T(" -zero");

	wstring argmentContent = argmentFormat.str();
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = pathManager.GetFsBuildPath().c_str();
	exeInfo.lpParameters = argmentContent.c_str();
	exeInfo.lpDirectory =  pathManager.GetRootPath().c_str();
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("appfs ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	LogAppInfo();
	return true;
}

bool CImageMakerImp::VeryfyFileConfig()
{
	CPathManager pathManager;
	
	SHELLEXECUTEINFO exeInfo;
	boost::wformat argmentFormat(_T(" %1% %2%"));
	argmentFormat % pathManager.GetNKPath();
	argmentFormat % pathManager.GetNKVPath();

	wstring argmentContent = argmentFormat.str();
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = pathManager.GetFileAddSumPath().c_str();
	exeInfo.lpParameters = argmentContent.c_str();
	exeInfo.lpDirectory = NULL;
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("add sum nk ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	boost::wformat argmentFormatBoot(_T(" %1% %2%"));
	argmentFormatBoot % pathManager.GetBootfsPath();
	argmentFormatBoot % pathManager.GetBootFsVPath();

	argmentContent = argmentFormatBoot.str();
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = pathManager.GetFileAddSumPath().c_str();
	exeInfo.lpParameters = argmentContent.c_str();
	exeInfo.lpDirectory = NULL;
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("add sum boot ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	WaitForSingleObject(exeInfo.hProcess, 10000);
	boost::wformat argmentFormatApp(_T(" %1% %2%"));
	argmentFormatApp % pathManager.GetAppFsPath();
	argmentFormatApp % pathManager.GetAppFsVPath();

	argmentContent = argmentFormatApp.str();
	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = pathManager.GetFileAddSumPath().c_str();
	exeInfo.lpParameters = argmentContent.c_str();
	exeInfo.lpDirectory = NULL;
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("add sum app ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	return true;
}

bool CImageMakerImp::ImageConfig()
{
	CPathManager pathManager;
	
	SHELLEXECUTEINFO exeInfo;

	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = pathManager.GetDragonPath().c_str();;
	exeInfo.lpParameters = pathManager.GetImageConfigPath().c_str();
	exeInfo.lpDirectory = pathManager.GetRootPath().c_str();
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("image ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);

	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = NULL;
	exeInfo.lpFile = pathManager.GetDateCopyPath().c_str();
	exeInfo.lpParameters = _T("wdk3.img");//pathManager.GetImgPath().c_str();
	exeInfo.lpDirectory = pathManager.GetRootPath().c_str();
	exeInfo.nShow = SW_MINIMIZE;
	exeInfo.hInstApp = NULL;
	//MessageBox(NULL, exeInfo.lpFile, exeInfo.lpParameters, MB_OK|MB_TOPMOST);
	if (!ShellExecuteEx(&exeInfo))
	{
		MessageBox(NULL, _T("datacopy ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	return true;
}

void CImageMakerImp::DeleteMiddleFiles()
{
	CPathManager pathManager;
	boost::filesystem::remove(pathManager.GetBootfsPath());
	boost::filesystem::remove(pathManager.GetAppFsPath());
	boost::filesystem::remove(pathManager.GetMsmbrPath());
	boost::filesystem::remove(pathManager.GetNKVPath());
	boost::filesystem::remove(pathManager.GetBootFsVPath());
	boost::filesystem::remove(pathManager.GetAppFsVPath());
}

void CImageMakerImp::LogAppInfo()
{
	CPathManager pathManager;
	wstring appInfoFileName(pathManager.GetAppDirPath());
	appInfoFileName += _T("Config\\AppConfigInfo.dat");

	CStdioFileEx appInfoFile;
	if(!appInfoFile.Open(appInfoFileName.c_str(), CFile::modeRead))
	{
		return ;
	}
	CString logBuffer;
	
	const TCHAR* versionInfo = _T("VersionInfo");
	const TCHAR* productNo = _T("ProductNo");
	
	while(appInfoFile.ReadString(logBuffer))
	{
		if (logBuffer.Find(versionInfo) != -1)
		{
			wstring versionLog = logBuffer.GetBuffer();
			CLogManager::GetInstance().AddLog(versionInfo, versionLog);
		}
		else if (logBuffer.Find(productNo) != -1)
		{
			wstring productNoLog = logBuffer.GetBuffer();
			CLogManager::GetInstance().AddLog(productNo, productNoLog);
		}
	}
	
}

void CImageMakerImp::ShowLog()
{
	char cmdContent[MAX_PATH];
	CPathManager pathManager;
	boost::wformat cmdFormat(_T("notepad %1%%2%"));
	cmdFormat % pathManager.GetRootPath();
	cmdFormat % _T("ImageMaker.log");
	wstring cmdContentW = cmdFormat.str();
	WideCharToMultiByte(GetACP(), 0, cmdContentW.c_str(), cmdContentW.length()+1, cmdContent, MAX_PATH, NULL, NULL);
	system(cmdContent);
}

bool CImageMakerImp::RenameImage()
{
	CPathManager pathManager;
	if (!boost::filesystem::exists(pathManager.GetImgPath()))
	{
		return false;
	}
	if (!imageTail_.empty())
	{
		
		boost::wformat newPathFormat(_T("%1%wdk3_%2%.img"));
		newPathFormat % pathManager.GetRootPath();
		newPathFormat % imageTail_;
		
		wstring newPath = newPathFormat.str();
		
		boost::filesystem::rename(pathManager.GetImgPath(), newPath);

		//MessageBox(NULL, pathManager.GetImgPath().c_str(), newPath.c_str(), MB_OK|MB_TOPMOST);
	}
	return true;
}

void CImageMakerImp::FunctionBind()
{
	functionContainer_.push_back(bind(&CImageMakerImp::DeleteNKAndImg, this));
	functionContainer_.push_back(bind(&CImageMakerImp::MemoryConfig, this));
	functionContainer_.push_back(bind(&CImageMakerImp::LCDConfig, this));
	functionContainer_.push_back(bind(&CImageMakerImp::DebugReleaseConfig, this));
	functionContainer_.push_back(bind(&CImageMakerImp::MsmbrConfig, this));
	functionContainer_.push_back(bind(&CImageMakerImp::BootfsISOConfig, this));
	functionContainer_.push_back(bind(&CImageMakerImp::MBRConfig, this));
	functionContainer_.push_back(bind(&CImageMakerImp::BootfsConfig, this));
	functionContainer_.push_back(bind(&CImageMakerImp::AppfsConfig, this));
	functionContainer_.push_back(bind(&CImageMakerImp::VeryfyFileConfig, this));
	functionContainer_.push_back(bind(&CImageMakerImp::ImageConfig, this));

}