#pragma once
#include <string>
using namespace std;

class CMakerTools
{
public:
	CMakerTools(void);
	~CMakerTools(void);
	BOOL CopyFolder(LPCTSTR lpszFromPath,LPCTSTR lpszToPath);
	bool SelectDir(const wstring& title, wstring& selectedDir) const;
	bool CopyFileInt(const wstring& sourceFileName, const wstring& targetFileName) const;
	bool ExcuteCommand(const wstring& sourceExeFilePath, const wstring& params) const;
};

