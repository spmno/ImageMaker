#pragma once
#include <string>
#include <map>
#include <lua.hpp>
using namespace std;

class CAreaHelper
{
public:
	CAreaHelper(void);
	~CAreaHelper(void);
	wstring& GetAreaName(const wstring& code);
private:
	void GetField(lua_State* L, const char*key);
	map<wstring, wstring> areaTable_;
	wstring areaName_;

};

