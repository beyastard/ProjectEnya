#include "AMemStatus.h"
#include <Windows.h>
#include "ADebug.h"

AMemStatus g_mem;

AMemStatus::AMemStatus()
{
	m_bEnd = true;
}

AMemStatus::~AMemStatus()
{
	if(m_bEnd==false)
	{
		End();
	}
}

void AMemStatus::Begin(char *szInfo)
{
	MEMINFO info;
	info.info = szInfo;
	MEMORYSTATUS  mem;
	GlobalMemoryStatus(&mem);
	info.nPreMem = mem.dwAvailPhys;
	m_List.AddHead(info);
	m_bEnd = false;
}

void AMemStatus::End()
{
	MEMORYSTATUS  mem;
	GlobalMemoryStatus(&mem);
	int nUseSize = mem.dwAvailPhys;
	MEMINFO info = m_List.RemoveHead();
	ADebug::Msg(0,"%s Memory: %d bytes %.3f K",info.info,info.nPreMem-nUseSize,(info.nPreMem-nUseSize)/1024.f);
	m_bEnd = true;
}
