#pragma once
#include <string>
#include <vector>
#include <functional>
using namespace std;
class CImageMakerImp
{
public:
	typedef enum
	{
		MEMORY_INVALID_PARAM = -1,
		MEMORY_256M,
		MEMORY_512M,
	}MEMORY_SIZE;

	typedef enum
	{
		PROJECT_INVALID_PARAM = -1,
		PROJECT_4389V,
		PROJECT_8033V,
		PROJECT_5133V,
		PROJECT_EVB,
	}PRJECT_KIND;

	CImageMakerImp(void);
	~CImageMakerImp(void);
	void SetMemorySize(MEMORY_SIZE size)
	{
		memorySize_ = size;
	}
	void SetLCDKind(wstring& project)
	{
		projectName_ = project;
	}
	void SetNKPath(wstring& path)
	{
		nkSourcePath_ = path;
	}
	void SetImageTail(wstring& tail)
	{
		imageTail_ = tail;
	}
	bool Make();
private:
	bool DeleteNKAndImg();
	bool MemoryConfig();
	bool LCDConfig();
	bool DebugReleaseConfig();
	bool MsmbrConfig();
	bool BootfsISOConfig();
	bool MBRConfig();
	bool BootfsConfig();
	bool AppfsConfig();
	bool VeryfyFileConfig();
	bool ImageConfig();
	void DeleteMiddleFiles();
	bool CopyFileInt(wstring& sourceFileName, wstring& targetFileName);
	void LogAppInfo();
	void ShowLog();
	bool RenameImage();
	void FunctionBind();
	vector<function<bool(void)> > functionContainer_;
	MEMORY_SIZE memorySize_;
	wstring projectName_;
	wstring nkSourcePath_;
	wstring imageTail_;
};
