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
	NKConfig();
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
		MessageBox(NULL, _T("制作完成"), _T("完成"), MB_OK|MB_TOPMOST);
		return true;
	}
	else
	{
		MessageBox(NULL, _T("制作失败"), _T("完成"), MB_OK|MB_TOPMOST);
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
	wstring targetPathName = pathManager.GetSysConfig1Path();
	wstring sourcePathName(lcdPath);
	boost::wformat projectFormat(_T("Project : %1%."));
	boost::wformat projectFileNameFormat(_T("sys_config1_%1%.fex"));
	projectFormat % projectName_;
	projectFileNameFormat % projectName_;
	wstring projectFileName = projectFileNameFormat.str();
	sourcePathName += projectFileName;
	if (!boost::filesystem::exists(sourcePathName))
	{
		MessageBox(NULL, sourcePathName.c_str(), _T("找不到文件"), MB_OK|MB_TOPMOST);
		ExitProcess(-1);
	}
	wstring projectIndex = _T("Project");
	wstring projectContent = projectFormat.str();
	CLogManager::GetInstance().AddLog(projectIndex, projectContent);
	CMakerTools tools;
	if (!tools.CopyFileInt(sourcePathName, targetPathName))
	{
		MessageBox(NULL, sourcePathName.c_str(), _T("复制项目文件错误"), MB_OK|MB_TOPMOST);
		return false;
	}

	//copy filter
	wstring filterDir(pathManager.GetTopDirPath());
	filterDir += _T("Filter\\");
	filterDir += projectName_;
	filterDir += _T("\\Filter");

	wstring targetFilterDir(pathManager.GetRootPath());
	targetFilterDir += _T("root");
	wstring targetFilterPath(pathManager.GetRootPath());
	targetFilterPath += _T("root\\Filter");

	if (!boost::filesystem::exists(filterDir)) {
		boost::filesystem::remove_all(targetFilterPath);
		Sleep(1000);
		wstring filterIndex = _T("filter");
		wstring filterContent(_T("Filter:"));
		filterContent += _T("no filter");
		CLogManager::GetInstance().AddLog(filterIndex, filterContent);
		return true;
	}

	boost::filesystem::remove_all(targetFilterPath);
	Sleep(1000);

	if (tools.CopyFolder(filterDir.c_str(), targetFilterDir.c_str()) == FALSE)
	{
		MessageBox(NULL, filterDir.c_str(), _T("解码器复制失败"), MB_OK|MB_TOPMOST);
		return false;
	}
	wstring filterIndex = _T("filter");
	wstring filterContent(_T("Filter:"));
	filterContent += filterDir;
	CLogManager::GetInstance().AddLog(filterIndex, filterContent);


	//copy video txt
	wstring videoFile(pathManager.GetTopDirPath());
	videoFile += _T("mode\\");
	videoFile += projectName_;
	videoFile += _T("\\video.txt");

	wstring targetVideoFile(pathManager.GetRootPath());
	targetVideoFile += _T("root\\video.txt");

	if (!boost::filesystem::exists(videoFile)) {
		MessageBox(NULL, videoFile.c_str(), _T("找不到文件"), MB_OK|MB_TOPMOST);
		ExitProcess(-1);
	}
	
	if (CopyFile(videoFile.c_str(), targetVideoFile.c_str(), FALSE) == FALSE) {
		MessageBox(NULL, targetVideoFile.c_str(), videoFile.c_str(), MB_OK|MB_TOPMOST);
		return false;
	}
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
	CMakerTools tools;
	CPathManager pathManager;
	wstring&& msmbrExeName = pathManager.GetMsmbrExePath();
	wstring&& sysConfigFileName = pathManager.GetSysConfigPath();
	boost::wformat argmentFormat(_T("-cfg  %1% -user  FAT_APPFS  -o  msmbr.fex "));
	argmentFormat % sysConfigFileName;
	wstring argmentContent = argmentFormat.str();

	if (!tools.ExcuteCommand(msmbrExeName, argmentContent))
	{
		MessageBox(NULL, _T("MsmbrConfig失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	return true;
}

bool CImageMakerImp::BootfsISOConfig()
{
	CPathManager pathManager;
	CMakerTools tools;
	wstring&& scriptExeName = pathManager.GetScriptPath();
	wstring&& sysConfig1Path = pathManager.GetSysConfig1Path();
	
	if (!tools.ExcuteCommand(scriptExeName, sysConfig1Path))
	{
		MessageBox(NULL, _T("bootfs script失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	if (!tools.CopyFileInt(pathManager.GetSourceNand0PathName(), pathManager.GetTargetNand0PathName()))
	{
		MessageBox(NULL, _T("复制nand boot0失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	if (!tools.CopyFileInt(pathManager.GetSourceNand1PathName(), pathManager.GetTargetNand1PathName()))
	{
		MessageBox(NULL, _T("复制nand boot1失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	//Make update params
	wstring updateExePath = pathManager.GetUpdateToolPathName();
	boost::wformat argmentFormat(_T(" %1% %2% %3%"));
	argmentFormat % pathManager.GetSysConfig1BinPath();
	argmentFormat % pathManager.GetTargetNand0PathName();
	argmentFormat % pathManager.GetTargetNand1PathName();

	wstring argmentContent = argmentFormat.str();

	if (!tools.ExcuteCommand(updateExePath, argmentContent))
	{
		MessageBox(NULL, _T("升级nand boot失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	if (!tools.CopyFileInt(pathManager.GetSourceSdcardBoot0PathName(), pathManager.GetTargetSdcardBoot0PathName()))
	{
		MessageBox(NULL, _T("复制sdcard boot0失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	if (!tools.CopyFileInt(pathManager.GetSourceSdcardBoot1PathName(), pathManager.GetTargetSdcardBoot1PathName()))
	{
		MessageBox(NULL, _T("复制sdcard boot1失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	//Make sd params
	boost::wformat argmentSdcardFormat(_T(" %1% %2% %3% %4%"));
	argmentSdcardFormat % pathManager.GetSysConfig1BinPath();
	argmentSdcardFormat % pathManager.GetTargetSdcardBoot0PathName();
	argmentSdcardFormat % pathManager.GetTargetSdcardBoot1PathName();
	argmentSdcardFormat % _T("SDMMC_CARD");
	argmentContent = argmentSdcardFormat.str();

	if (!tools.ExcuteCommand(updateExePath, argmentContent))
	{
		MessageBox(NULL, _T("升级sdcard boot失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	boost::filesystem::remove(pathManager.GetBootfsScriptPathName());
	boost::filesystem::remove(pathManager.GetBootfsScript0PathName());

	if (!tools.CopyFileInt(pathManager.GetSysConfig1BinPath(), pathManager.GetBootfsScriptPathName()))
	{
		MessageBox(NULL, _T("复制script失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	if (!tools.CopyFileInt(pathManager.GetSysConfig1BinPath(), pathManager.GetBootfsScript0PathName()))
	{
		MessageBox(NULL, _T("复制script0失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	return true;
}

bool CImageMakerImp::MBRConfig()
{
	CPathManager pathManager;
	CMakerTools tools;
	boost::filesystem::remove(pathManager.GetSysConfigBinPath());
	
	if (!tools.ExcuteCommand(pathManager.GetScriptOldPath(), pathManager.GetSysConfigPath()))
	{
		MessageBox(NULL, _T("script old失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	boost::wformat argmentFormat(_T(" %1% %2%"));
	argmentFormat % pathManager.GetSysConfigBinPath();
	argmentFormat % pathManager.GetCardMBRPath();
	wstring argmentContent = argmentFormat.str();

	if (!tools.ExcuteCommand(pathManager.GetUpdateMBRPath(), argmentContent))
	{
		MessageBox(NULL, _T("升级mbr失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	return true;
}

bool CImageMakerImp::BootfsConfig()
{
	CPathManager pathManager;
	CMakerTools tools;
	boost::filesystem::remove(pathManager.GetBootfsPath());

	boost::wformat argmentFormat(_T(" %1% %2%"));
	argmentFormat % pathManager.GetBootfsIniPath();
	argmentFormat % pathManager.GetSplitBinPath();
	wstring argmentContent = argmentFormat.str();

	if (!tools.ExcuteCommand(pathManager.GetFsBuildPath(), argmentContent))
	{
		MessageBox(NULL, _T("bootfs失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	return true;
}

bool CImageMakerImp::AppfsConfig()
{
	CPathManager pathManager;
	CMakerTools tools;

	boost::wformat argmentFormat(_T(" %1% %2% %3%"));
	argmentFormat % pathManager.GetRootIniPath();
	argmentFormat % pathManager.GetSplitBinPath();
	argmentFormat % _T(" -zero");
	wstring argmentContent = argmentFormat.str();

	if (!tools.ExcuteCommand(pathManager.GetFsBuildPath(), argmentContent))
	{
		MessageBox(NULL, _T("appfs失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	LogAppInfo();
	return true;
}

bool CImageMakerImp::VeryfyFileConfig()
{
	CPathManager pathManager;
	CMakerTools tools;

	//Make nk params
	boost::wformat argmentFormat(_T(" %1% %2%"));
	argmentFormat % pathManager.GetNKPath();
	argmentFormat % pathManager.GetNKVPath();
	wstring argmentContent = argmentFormat.str();

	if (!tools.ExcuteCommand(pathManager.GetFileAddSumPath(), argmentContent))
	{
		MessageBox(NULL, _T("add sum nk失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	//Make bootfs params
	boost::wformat argmentFormatBoot(_T(" %1% %2%"));
	argmentFormatBoot % pathManager.GetBootfsPath();
	argmentFormatBoot % pathManager.GetBootFsVPath();
	argmentContent = argmentFormatBoot.str();

	if (!tools.ExcuteCommand(pathManager.GetFileAddSumPath(), argmentContent))
	{
		MessageBox(NULL, _T("add sum boot失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

	//Make app params
	boost::wformat argmentFormatApp(_T(" %1% %2%"));
	argmentFormatApp % pathManager.GetAppFsPath();
	argmentFormatApp % pathManager.GetAppFsVPath();
	argmentContent = argmentFormatApp.str();

	if (!tools.ExcuteCommand(pathManager.GetFileAddSumPath(), argmentContent))
	{
		MessageBox(NULL, _T("add sum app失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}

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
		MessageBox(NULL, _T("image失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	WaitForSingleObject(exeInfo.hProcess, 10000);
	/*
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
		MessageBox(NULL, _T("datacopy失败"), NULL, MB_OK|MB_TOPMOST);
		return false;
	}
	
	WaitForSingleObject(exeInfo.hProcess, 10000);
	*/
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

	wstring newPath;
	if (!imageTail_.empty())
	{
		
		boost::wformat newPathFormat(_T("%1%wdk3_%2%_%3%.img"));
		newPathFormat % pathManager.GetRootPath();
		newPathFormat % projectName_;
		newPathFormat % imageTail_;
		newPath = newPathFormat.str();

	}
	else
	{
		boost::wformat newPathFormat(_T("%1%wdk3_%2%.img"));
		newPathFormat % pathManager.GetRootPath();
		newPathFormat % projectName_;		
		newPath = newPathFormat.str();
	}

	boost::filesystem::rename(pathManager.GetImgPath(), newPath);
	return true;
}

void CImageMakerImp::NKConfig()
{
	wstring nkIndex = _T("NK");
	wstring nkLogContent(_T("NKPATH:"));
	nkLogContent += nkSourcePath_;
	CLogManager::GetInstance().AddLog(nkIndex, nkLogContent);
}

bool CImageMakerImp::DelAndCopyLogoFile()
{
	
	CPathManager pathManager;
	wstring sourceLogoDir(pathManager.GetTopDirPath());
	sourceLogoDir += _T("Logo\\");
	sourceLogoDir += projectName_;
	sourceLogoDir += _T("\\Custom\\");
	wstring targetLogoDir(pathManager.GetRootPath());
	targetLogoDir += _T("root\\Logo\\");
	
	if (boost::filesystem::exists(targetLogoDir)) {
		boost::system::error_code errorCode;
		boost::filesystem::remove_all(targetLogoDir, errorCode);
		if (errorCode.value() == 0x20) {
			MessageBox(NULL, _T("目录中有文件被占用"), targetLogoDir.c_str(), MB_OK|MB_TOPMOST);
			return false;
		}
		Sleep(200);
		boost::filesystem::create_directory(targetLogoDir);
	} else {
		boost::filesystem::create_directory(targetLogoDir);
	}
	
	if (boost::filesystem::exists(sourceLogoDir) && (!boost::filesystem::is_empty(sourceLogoDir))) {
		CMakerTools makerTools;
		if (!makerTools.CopyFolderWithoutDir(sourceLogoDir.c_str(), targetLogoDir.c_str())) {
			MessageBox(NULL, _T("copy custom image fail."), sourceLogoDir.c_str(), MB_OK|MB_TOPMOST);
			return false;
		}
	}
	
	return true;
}

void CImageMakerImp::FunctionBind()
{
	functionContainer_.push_back(bind(&CImageMakerImp::DelAndCopyLogoFile, this));
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