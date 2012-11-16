#pragma once
#include "memoryconfig.h"
class CMemory256M :
	public CMemoryConfig
{
public:
	CMemory256M(void);
	~CMemory256M(void);
	virtual bool Config();
};

