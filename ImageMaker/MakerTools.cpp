#include "stdafx.h"
#include "MakerTools.h"


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