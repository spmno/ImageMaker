#pragma once
#include "memoryconfig.h"
class CMemory512M :
	public CMemoryConfig
{
public:
	CMemory512M(void);
	~CMemory512M(void);
	virtual bool Config();
};

