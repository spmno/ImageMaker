#pragma once
#include <string>
#include <map>
#include <fstream>
#include "PathManager.h"

using namespace std;
class CLogManager
{
public:

	~CLogManager(void);
	static CLogManager& GetInstance()
	{
		static CLogManager logManager;
		return logManager;
	}
	void AddLog(const wstring& index, const wstring& level, const wstring& content)
	{
		logContainer_[index].level = level;
		logContainer_[index].content = content;
	}
	void PrintLog()
	{
		CPathManager pathManager;

		wofstream outPutLogFile(pathManager.GetLogFilePath(), ios_base::out);

		if (outPutLogFile)
		{
			for (auto logItem : logContainer_)
			{
				outPutLogFile << logItem.second.level.c_str();
				outPutLogFile << logItem.second.content.c_str() << endl;
			}
		}
		logContainer_.clear();
		Sleep(1000);

	}

	struct Log {
		wstring level;
		wstring content;
	};
private:
	CLogManager(void);
	map<wstring, Log > logContainer_;
};

