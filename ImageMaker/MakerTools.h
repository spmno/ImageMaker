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
	bool CopyFileInt(wstring& sourceFileName, wstring& targetFileName) const;
};

