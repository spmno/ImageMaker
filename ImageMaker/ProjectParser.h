#pragma once
#include <vector>
#include <string>

using namespace std;

class CProjectParser
{
public:
	CProjectParser(void);
	~CProjectParser(void);
	vector<wstring>& GetProjects()
	{
		return projectContainer;
	}
private:
	void ParserProjectFile();
	vector<wstring> projectContainer;
};

