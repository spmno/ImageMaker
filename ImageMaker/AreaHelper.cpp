#include "stdafx.h"

#include "AreaHelper.h"
#include "PathManager.h"
#pragma comment(lib, "lua51.lib")
#pragma comment(lib, "lua5.1.lib")

CAreaHelper::CAreaHelper(void)
{
	
}


CAreaHelper::~CAreaHelper(void)
{
}

wstring& CAreaHelper::GetAreaName(const wstring& code) 
{

	char luaFileName[MAX_PATH];
	char codeName[32];
	CPathManager pathManager;
	wstring luaFileNameW(pathManager.GetRootPath());
	luaFileNameW += _T("location.lua");
	WideCharToMultiByte(GetACP(), 0, luaFileNameW.c_str(), luaFileNameW.length()+1, luaFileName, MAX_PATH, NULL, NULL);
	WideCharToMultiByte(GetACP(), 0, code.c_str(), code.length()+1, codeName, 32, NULL, NULL);
	lua_State *L = lua_open();
	luaL_openlibs(L);
	if (luaL_loadfile(L, luaFileName) || lua_pcall(L, 0, 0, 0))
	{
		char errorLog[MAX_PATH];
		TCHAR errorLogW[MAX_PATH];
		sprintf(errorLog, "location.lua %s", lua_tostring(L, -1));
		MultiByteToWideChar(GetACP(), 0, errorLog, strlen(errorLog)+1, errorLogW, MAX_PATH);
		MessageBox(NULL, errorLogW, NULL, MB_OK|MB_TOPMOST);
	}
	string idCodeName(("ID"));
	idCodeName += codeName;
	lua_getglobal(L, idCodeName.c_str());

	if (lua_istable(L, -1))
	{
		GetField(L, "language");
	}
	return areaName_;
}

void CAreaHelper::GetField(lua_State* L, const char* key) 
{
	lua_getfield(L, -1, key);
	int t = lua_type(L, -1);
	
	const char* result = lua_tostring(L, -1);
	TCHAR locale[32];
	MultiByteToWideChar(GetACP(), 0, result, strlen(result)+1, locale, 32);
	areaName_ = locale;
	lua_pop(L, 1);
}