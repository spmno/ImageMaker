#pragma once
#include <string>
using namespace std;
class CPathManager
{
public:
	CPathManager(void)
	{
		TCHAR name[MAX_PATH] = {0};
		GetModuleFileName(NULL, name, MAX_PATH);
		TCHAR* p = _tcsrchr(name, '\\');
		if (p == NULL)
		{
			MessageBox(NULL, _T("目录有问题"), NULL, MB_OK|MB_TOPMOST);
			ExitProcess(-1);
		}
		*(p + 1) = '\0';
		rootPath_ = name;

		*p = '\0';
		p = _tcsrchr(name, '\\');
		if (p == NULL)
		{
			MessageBox(NULL, _T("目录有问题"), NULL, MB_OK|MB_TOPMOST);
			ExitProcess(-1);
		}
		*(p+1) = '\0';
		workspacePath_ = name;

		*p = '\0';
		p = _tcsrchr(name, '\\');
		if (p == NULL)
		{
			MessageBox(NULL, _T("目录有问题"), NULL, MB_OK|MB_TOPMOST);
			ExitProcess(-1);
		}
		*(p+1) = '\0';
		toolPath_ = name;
		toolPath_ += _T("tools\\");

		*p = '\0';
		p = _tcsrchr(name, '\\');
		if (p == NULL)
		{
			MessageBox(NULL, _T("目录有问题"), NULL, MB_OK|MB_TOPMOST);
			ExitProcess(-1);
		}
		*(p+1) = '\0';
		topDirPath_ = name;
	}
	~CPathManager(void);
	wstring& GetRootPath()
	{
		return rootPath_;
	}
	wstring& GetTopDirPath()
	{
		return topDirPath_;
	}
	wstring& GetWorkSpacePath()
	{
		return workspacePath_;
	}
	wstring&& GetMemoryPath()
	{
		memoryPath_ = rootPath_;
		memoryPath_ += _T("bootfs\\wince\\");
		return std::move(memoryPath_);
	}
	wstring&& GetLCDPath()
	{
		lcdPath_ = workspacePath_;
		lcdPath_ += _T("eFex\\sys_config1\\");
		return std::move(lcdPath_);
	}

	wstring&& GetNKPath()
	{
		nkPath_ = rootPath_;
		nkPath_ += _T("nk.fex");
		return std::move(nkPath_);
	}
	wstring&& GetImgPath()
	{
		imgPath_ = rootPath_;
		imgPath_ += _T("wdk3.img");
		return std::move(imgPath_);
	}
	wstring&& GetMsmbrExePath()
	{
		msmbrExePath_ = toolPath_;
		msmbrExePath_ += _T("msmbr\\msmbr.exe");
		return std::move(msmbrExePath_);
	}

	wstring&& GetMsmbrPath()
	{
		msmbrPath_ = rootPath_;
		msmbrPath_ += _T("msmbr.fex");
		return std::move(msmbrPath_);
	}

	wstring&& GetSysConfigPath()
	{
		sysConfig_ = workspacePath_;
		sysConfig_ += _T("eFex\\sys_config.fex");
		return std::move(sysConfig_);
	}
	wstring&& GetSysConfig1Path()
	{
		sysConfig1_ = workspacePath_;
		sysConfig1_ += _T("eFex\\sys_config1.fex");
		return std::move(sysConfig1_);
	}
	wstring&& GetScriptPath()
	{
		scriptPath_ = toolPath_;
		scriptPath_ += _T("pctools\\mod_update\\script.exe");
		return std::move(scriptPath_);
	}

	wstring&& GetSysConfig1BinPath()
	{
		sysConfig1BinPath_ = workspacePath_;
		sysConfig1BinPath_ += _T("eFex\\sys_config1.bin");
		return std::move(sysConfig1BinPath_);
	}

	wstring&& GetSysConfigBinPath()
	{
		sysConfigBinPath_ = rootPath_;
		sysConfigBinPath_ += _T("eFex\\sys_config.bin");
		return std::move(sysConfigBinPath_);
	}

	wstring&& GetSourceNand0PathName()
	{
		sourceNandBoot0PathName_ = workspacePath_;
		sourceNandBoot0PathName_ += _T("eGon\\storage_media\\nand\\Boot0.bin");
		return std::move(sourceNandBoot0PathName_);
	}

	wstring&& GetSourceNand1PathName()
	{
		sourceNandBoot1PathName_ = workspacePath_;
		sourceNandBoot1PathName_ += _T("eGon\\storage_media\\nand\\Boot1.bin");
		return std::move(sourceNandBoot1PathName_);
	}

	wstring&& GetTargetNand0PathName()
	{
		targetNandBoot0PathName_ = workspacePath_;
		targetNandBoot0PathName_ += _T("eGon\\Boot0.bin");
		return std::move(targetNandBoot0PathName_);
	}

	wstring&& GetTargetNand1PathName()
	{
		taregtNandBoot1PathName_ = workspacePath_;
		taregtNandBoot1PathName_ += _T("eGon\\Boot1.bin");
		return std::move(taregtNandBoot1PathName_);
	}

	wstring&& GetUpdateToolPathName()
	{
		updateToolPathName_ = toolPath_;
		updateToolPathName_ += _T("pctools\\mod_update\\update_23.exe");
		return std::move(updateToolPathName_);
	}

	wstring&& GetSourceSdcardBoot0PathName()
	{
		sourceSdcardBoot0PathName_ = workspacePath_;
		sourceSdcardBoot0PathName_ += _T("eGon\\storage_media\\sdcard\\Boot0.bin");
		return std::move(sourceSdcardBoot0PathName_);
	}

	wstring&& GetSourceSdcardBoot1PathName()
	{
		sourceSdcardBoot1PathName_ = workspacePath_;
		sourceSdcardBoot1PathName_ += _T("eGon\\storage_media\\sdcard\\Boot1.bin");
		return std::move(sourceSdcardBoot1PathName_);
	}

	wstring&& GetTargetSdcardBoot0PathName()
	{
		targetSdcardBoot0PathName_ = workspacePath_;
		targetSdcardBoot0PathName_ += _T("\\eFex\\card\\card_boot0.fex");
		return std::move(targetSdcardBoot0PathName_);
	}

	wstring&& GetTargetSdcardBoot1PathName()
	{
		targetSdcardBoot1PathName_ = workspacePath_;
		targetSdcardBoot1PathName_ += _T("\\eFex\\card\\card_boot1.fex");
		return std::move(targetSdcardBoot1PathName_);
	}

	wstring&& GetBootfsScriptPathName()
	{
		bootfsScriptPathName_ = rootPath_;
		bootfsScriptPathName_ += _T("bootfs\\script.bin");
		return std::move(bootfsScriptPathName_);
	}
	
	wstring&& GetBootfsScript0PathName()
	{
		bootfsScript0PathName_ = rootPath_;
		bootfsScript0PathName_ += _T("bootfs\\script0.bin");
		return std::move(bootfsScript0PathName_);
	}

	wstring&& GetScriptOldPath()
	{
		scriptOldExePath_ = toolPath_;
		scriptOldExePath_ += _T("pctools\\mod_update\\script_old.exe");
		return std::move(scriptOldExePath_);
	}

	wstring&& GetUpdateMBRPath()
	{
		updateMbrExePath_ = toolPath_;
		updateMbrExePath_ += _T("pctools\\mod_update\\update_mbr.exe");
		return std::move(updateMbrExePath_);
	}

	wstring&& GetCardMBRPath()
	{
		cardMbrPath_ = workspacePath_;
		cardMbrPath_ += _T("eFex\\card\\mbr.fex");
		return std::move(cardMbrPath_);
	}
	
	wstring&& GetBootfsPath()
	{
		bootfs_ = rootPath_;
		bootfs_ += _T("bootfs.fex");
		return std::move(bootfs_);
	}

	wstring&& GetFsBuildPath()
	{
		fsBuildExePath_ = toolPath_;
		fsBuildExePath_ += _T("pctools\\fsbuild200\\fsbuild.exe");
		return std::move(fsBuildExePath_);
	}

	wstring&& GetBootfsIniPath()
	{
		bootfsIni_ = rootPath_;
		bootfsIni_ += _T("script\\bootfs.ini");
		return std::move(bootfsIni_);
	}

	wstring&& GetRootIniPath()
	{
		rootIni_ = rootPath_;
		rootIni_ +=  _T("script\\root.ini");
		return std::move(rootIni_);
	}

	wstring&& GetSplitBinPath()
	{
		splitBinPath_ = workspacePath_;
		splitBinPath_ += _T("efex\\split_c43a2a92-a8ed-4f92-abdb-ca0d5d29666d.bin");
		return std::move(splitBinPath_);
	}

	wstring&& GetFileAddSumPath()
	{
		fileAddSumExePath_ = toolPath_;
		fileAddSumExePath_ += _T("FileAddSum\\FileAddSum.exe");
		return std::move(fileAddSumExePath_);
	}

	wstring&& GetAppFsPath()
	{
		appfsPath_ = rootPath_;
		appfsPath_ += _T("appfs.fex");
		return std::move(appfsPath_);
	}

	wstring&& GetNKVPath()
	{
		nkVPath_ = rootPath_;
		nkVPath_ += _T("nk_v.fex");
		return std::move(nkVPath_);
	}

	wstring&& GetBootFsVPath()
	{
		bootfsVPath_ = rootPath_;
		bootfsVPath_ += _T("bootfs_verify.fex");
		return std::move(bootfsVPath_);
	}

	wstring&& GetAppFsVPath()
	{
		appfsVPath_ = rootPath_;
		appfsVPath_ += _T("appfs_v.fex");
		return std::move(appfsVPath_);
	}

	wstring&& GetDragonPath()
	{
		dragonPath_ = toolPath_;
		dragonPath_ += _T("pctools\\eDragonEx200\\dragon.exe");
		return std::move(dragonPath_);
	}

	wstring&& GetImageConfigPath()
	{
		imageConfigPath_ = rootPath_;
		imageConfigPath_ += _T("script\\image.cfg");
		return std::move(imageConfigPath_);
	}

	wstring&& GetDateCopyPath()
	{
		datecopyPath_ = rootPath_;
		datecopyPath_ += _T("tools\\datecopy.exe");
		return std::move(datecopyPath_);
	}

	wstring&& GetLogo1Path()
	{
		logo1Path_ = rootPath_;
		logo1Path_ += _T("bootfs\\wince\\logo.bmp");
		return std::move(logo1Path_);
	}

	wstring&& GetLogo2Path()
	{
		logo2Path_ = rootPath_;
		logo2Path_ += _T("root\\driver\\logo.bmp");
		return std::move(logo2Path_);
	}

	wstring&& GetBootloadDirPath()
	{
		bootloadDirPath_ = rootPath_;
		bootloadDirPath_ += _T("bootfs");
		return std::move(bootloadDirPath_);
	}

	wstring&& GetAppDirPath()
	{
		appDirPath_ = rootPath_;
		appDirPath_ += _T("root\\F33APP");
		return std::move(appDirPath_);
	}

	wstring&& GetLogFilePath()
	{
		logFilePath_ = rootPath_;
		logFilePath_ += _T("ImageMaker.log");
		return std::move(logFilePath_);
	}
	wstring& GetProjectFilePath()
	{
		projectFilePath_ = rootPath_;
		projectFilePath_ += _T("project.json");
		return (projectFilePath_);
	}
private:
	wstring rootPath_;
	wstring workspacePath_;
	wstring toolPath_;
	wstring memoryPath_;
	wstring lcdPath_;
	wstring nkPath_;
	wstring imgPath_;
	wstring msmbrExePath_;
	wstring msmbrPath_;
	wstring sysConfig1BinPath_;
	wstring sysConfigBinPath_;
	wstring sysConfig1_;
	wstring sysConfig_;
	wstring scriptPath_;
	wstring sourceNandBoot0PathName_;
	wstring sourceNandBoot1PathName_;
	wstring targetNandBoot0PathName_;
	wstring taregtNandBoot1PathName_;
	wstring updateToolPathName_;
	wstring sourceSdcardBoot0PathName_;
	wstring sourceSdcardBoot1PathName_;
	wstring targetSdcardBoot0PathName_;
	wstring targetSdcardBoot1PathName_;
	wstring bootfsScriptPathName_;
	wstring bootfsScript0PathName_;
	wstring scriptOldExePath_;
	wstring updateMbrExePath_;
	wstring cardMbrPath_;
	wstring bootfs_;
	wstring fsBuildExePath_;
	wstring bootfsIni_;
	wstring rootIni_;
	wstring splitBinPath_;
	wstring fileAddSumExePath_;
	wstring appfsPath_;
	wstring nkVPath_;
	wstring bootfsVPath_;
	wstring appfsVPath_;
	wstring dragonPath_;
	wstring imageConfigPath_;
	wstring datecopyPath_;
	wstring logo1Path_;
	wstring logo2Path_;
	wstring bootloadDirPath_;
	wstring appDirPath_;
	wstring logFilePath_;
	wstring projectFilePath_;
	wstring topDirPath_;
};

