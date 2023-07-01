#pragma once

#include "AUString.h"
#include "AUList.h"

struct MEMINFO
{
	AUString info;
	int nPreMem;
};

class AMemStatus  
{
protected:
	bool		m_bEnd;
	AListT<MEMINFO>	m_List;
public:
	void End();
	void Begin(char * szInfo);
	AMemStatus();
	virtual ~AMemStatus();

};

#ifndef _DEBUG
#define MEMSTATUSBEGIN(strinfo) g_mem.Begin(strinfo);
#define MEMSTATUSEND() g_mem.End();

#else
#define MEMSTATUSBEGIN(strinfo) g_mem.Begin(strinfo);
#define MEMSTATUSEND() g_mem.End();

#endif 

extern AMemStatus g_mem;
