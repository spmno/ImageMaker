#include "stdafx.h"
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "ProjectParser.h"
#include "PathManager.h"

using namespace std;
using namespace boost::property_tree;

CProjectParser::CProjectParser(void)
{
	ParserProjectFile();
}


CProjectParser::~CProjectParser(void)
{
}

void CProjectParser::ParserProjectFile()
{
	CPathManager pathManager;
	string fileContent;
	char jsonName[MAX_PATH];
	WideCharToMultiByte(GetACP(), 0, 
		pathManager.GetProjectFilePath().c_str(), 
		pathManager.GetProjectFilePath().length()+1, 
		jsonName,
		MAX_PATH,
		NULL,
		NULL);
	boost::property_tree::ptree jsonTree;
	try
	{
		json_parser::read_json(jsonName, jsonTree);
		ptree projectsArray = jsonTree.get_child("projects");
		for (auto project : projectsArray)
		{
			stringstream projectStream;
			write_json(projectStream, project.second);
			ptree projectJson;
			read_json(projectStream, projectJson);
			
			TCHAR projectName[MAX_PATH];
			MultiByteToWideChar(GetACP(), 
				0,
				projectJson.get<string>("name").c_str(),
				projectJson.get<string>("name").length()+1,
				projectName,
				MAX_PATH);
			projectContainer.push_back(projectName);
		}
	}
	catch(json_parser_error& e)
	{
		MessageBox(NULL, _T("Ω‚Œˆproject.json ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		ExitProcess(-1);
		return ;
	}
	catch(ptree& e)
	{
		MessageBox(NULL, _T("Ω‚Œˆproject.json ß∞‹"), NULL, MB_OK|MB_TOPMOST);
		ExitProcess(-1);
		return ;
	}

}