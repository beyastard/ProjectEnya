#include "ADebug.h"
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>
#include <time.h>
//#include <mtime.h>
#include "AMemLeak.h"

ADebug::ADebug()
{

}

ADebug::~ADebug()
{

}

void ADebug::Msg(int __nLevel, const char *__str, ...)
{
	va_list	arglist;
	char	buf[1024];
	va_start( arglist, __str);
	vsprintf( buf, __str, arglist );
	va_end( arglist );
	if(__nLevel == DEBUGLEVEL_FATAL)
	{
		MsgToFile(buf);
		//MsgToFile("\n");
	}
	//else if(__nLevel == 1)
	{
		OutputDebugString(buf);
		OutputDebugString("\n");
	}
}
void ADebug::MsgStr(int __nLevel, const char *__str, ...)
{
	va_list	arglist;
	char	buf[1024];
	va_start( arglist, __str);
	vsprintf( buf, __str, arglist );
	va_end( arglist );
	if(__nLevel == DEBUGLEVEL_FATAL)
	{
		MsgToFile(buf);
	}
	//else if(__nLevel == 1)
	{
		OutputDebugString(buf);
	}
}

void ADebug::MsgToFile(char * strInfo)
{
	static bool bFirst = true;
	FILE * fp;
	if(bFirst)
	{
		fp=fopen("Logs/Errors.log","wt");
		bFirst = false;
	}
	else
	{
		fp=fopen("Logs/Errors.log","at");
	}
	if(fp==NULL) return ;
	struct tm	*newtime;
	time_t		long_time;
	long_time	= time(NULL);
	newtime		= localtime(&long_time);
	fprintf(fp, "[%02d %02d %02d:%02d:%02d] %s\n"
		,newtime->tm_mon+1
		,newtime->tm_mday
		, newtime->tm_hour
		, newtime->tm_min
		, newtime->tm_sec
		, strInfo);

	fclose(fp);
}

__declspec(naked) unsigned __int64 ADebug::GetCpuCount()
{
	__asm
	{
		rdtsc
		ret
	}
}

//#define	ADEBUG_HOOK_ENABLE
//#define ADEBUG_REPLACE_ALL

#ifdef	ADEBUG_HOOK_ENABLE

#include <Dbghelp.h>
#include <Tlhelp32.h>


class ADebugHookTool
{
public:
	class AHookRecord
	{
	public:
		FARPROC	m_pfnHook;
		FARPROC	m_pfnOld;
		PCSTR	m_pszMod;
		PCSTR	m_pszName;

		AHookRecord();

		virtual BOOL	Init(PCSTR pszMod, PCSTR pszName, FARPROC pfnHook);
		virtual VOID	Free();
	};

	class AHeapHook
	{
	public:
		SIZE_T		m_nHeapSize;
		UINT		m_uAllocCount;
		UINT		m_uSmallAllocCount;
		UINT		m_uMemCount;
		UINT		m_uSmallCount;
		UINT64		m_uAllocTime;
		UINT64		m_uFreeTime;

		AHookRecord	m_recHeapReAlloc;
		AHookRecord	m_recHeapAlloc;
		AHookRecord	m_recHeapFree;

	public:
		AHeapHook();
		~AHeapHook();

		static LPVOID WINAPI hook_HeapReAlloc(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem, IN SIZE_T dwBytes);
		static LPVOID WINAPI hook_HeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes);
		static BOOL   WINAPI hook_HeapFree(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem);
		LPVOID WINAPI do_HeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes);
		LPVOID WINAPI do_HeapReAlloc(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem, IN SIZE_T dwBytes);
		BOOL   WINAPI do_HeapFree(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem);
	};
public:
	AHeapHook	m_HeapHook;
	UINT64		m_uCpuFreq;
	UINT64		m_uCpuOverhead;

	ADebugHookTool();
	~ADebugHookTool();

	static ADebugHookTool	m_Tool;

	static VOID ReplaceApiInMod(HMODULE hModCaller, PCSTR pszModCallee, PROC pfnCurrent, PROC pfnNew);
};

VOID ADebugMemStat(
	SIZE_T&	nHeapSize,
	UINT& uAllocCount,
	UINT& uSmallAllocCount,
	UINT& uMemCount,
	UINT& uSmallCount,
	UINT64& uAllocTime,
	UINT64& uFreeTime
)
{
	ADebugHookTool::AHeapHook& Hook	= ADebugHookTool::m_Tool.m_HeapHook;
	nHeapSize		= Hook.m_nHeapSize;
	uAllocCount		= Hook.m_uAllocCount;
	uSmallAllocCount= Hook.m_uSmallAllocCount;
	uMemCount		= Hook.m_uMemCount;
	uSmallCount		= Hook.m_uSmallCount;
	uAllocTime		= Hook.m_uAllocTime;
	uFreeTime		= Hook.m_uFreeTime;
}

VOID ADebugMemStat(
	SIZE_T&	nHeapSize,
	UINT& uAllocCount,
	UINT& uSmallAllocCount,
	UINT& uMemCount,
	UINT& uSmallCount,
	DOUBLE& fAllocTime,
	DOUBLE& fFreeTime
)
{
	ADebugHookTool::AHeapHook& Hook	= ADebugHookTool::m_Tool.m_HeapHook;
	nHeapSize		= Hook.m_nHeapSize;
	uAllocCount		= Hook.m_uAllocCount;
	uSmallAllocCount= Hook.m_uSmallAllocCount;
	uMemCount		= Hook.m_uMemCount;
	uSmallCount		= Hook.m_uSmallCount;
	DOUBLE double1 = (DOUBLE)((__int64)ADebugHookTool::m_Tool.m_uCpuFreq);

	DOUBLE dAlloc = (DOUBLE)((__int64)(Hook.m_uAllocTime));
	DOUBLE dFreeTime = (DOUBLE)((__int64)(Hook.m_uFreeTime));
	fAllocTime		= dAlloc/double1;
	fFreeTime		= dFreeTime/double1;
}

SIZE_T ADebugMemSize()
{
	return ADebugHookTool::m_Tool.m_HeapHook.m_nHeapSize;
}

char* g_szModNames[]	= 
{
	"product.exe",
	"msvcrt.dll",
	"msvcrtd.dll",
	"user32.dll",
	"d3d8.dll",
	"dsound.dll"
};

ADebugHookTool::AHookRecord::AHookRecord():
m_pfnHook	(NULL),
m_pfnOld	(NULL),
m_pszMod	(NULL),
m_pszName	(NULL)
{
}

BOOL ADebugHookTool::AHookRecord::Init(PCSTR pszMod, PCSTR pszName, FARPROC pfnHook)
{
	HMODULE			hMod;
	MODULEENTRY32	me;

	m_pszMod	= pszMod;
	m_pszName	= pszName;
	m_pfnHook	= pfnHook;
	hMod		= GetModuleHandle(pszMod);
	m_pfnOld	= GetProcAddress(hMod, m_pszName);
	if(m_pfnOld == NULL)
	{
		ADEBUG_TRACE2("Cannot get <%s> address of module <%s>", m_pszName, m_pszMod);
		return FALSE;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if(hSnapshot == NULL)
	{
		ADEBUG_TRACE("Cannot create snapshot.");
		return FALSE;
	}
	ZeroMemory(&me, sizeof(me));
	me.dwSize = sizeof(me);

	ADebug::Msg(0, "Hooking <%s.%s>", m_pszMod, m_pszName);

	if(Module32First(hSnapshot, &me))
	{
		do
		{
#ifdef	ADEBUG_REPLACE_ALL

			if(stricmp(me.szModule, "ntdll.dll") != 0)
			{
				ReplaceApiInMod(me.hModule, m_pszMod, m_pfnOld, m_pfnHook);
			}

#else

			for(int i = 0; i < sizeof(g_szModNames)/(sizeof(char*)); i ++)
			{
				if(stricmp(me.szModule, g_szModNames[i]) == 0)
				{
					ReplaceApiInMod(me.hModule, m_pszMod, m_pfnOld, m_pfnHook);
				}
			}

#endif	// ADEBUG_REPLACE_ALL
		}while(Module32Next(hSnapshot, &me));
	}
	CloseHandle(hSnapshot);
	
	return TRUE;
}

VOID ADebugHookTool::AHookRecord::Free()
{
	MODULEENTRY32	me;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if(hSnapshot == NULL)
	{
		ADEBUG_TRACE("Cannot create snapshot.");
		return;
	}
	ZeroMemory(&me, sizeof(me));
	me.dwSize = sizeof(me);

	ADebug::Msg(0, "Unhooking <%s.%s>", m_pszMod, m_pszName);

	if(Module32First(hSnapshot, &me))
	{
		do
		{
#ifdef	ADEBUG_REPLACE_ALL

			if(m_pfnOld != NULL)
			{
				if(stricmp(me.szModule, "ntdll.dll") != 0)
				{
					ReplaceApiInMod(me.hModule, m_pszMod, m_pfnHook, m_pfnOld);
				}
			}

#else

			for(int i = 0; i < sizeof(g_szModNames)/(sizeof(char*)); i ++)
			{
				if(stricmp(me.szModule, g_szModNames[i]) == 0)
				{
					if(m_pfnOld != NULL)
					{
						ReplaceApiInMod(me.hModule, m_pszMod, m_pfnHook, m_pfnOld);
					}
				}
			}

#endif	// ADEBUG_REPLACE_ALL
		}while(Module32Next(hSnapshot, &me));
	}
	CloseHandle(hSnapshot);
}

void ADebugHookTool::ReplaceApiInMod(
	HMODULE hModCaller, PCSTR pszModCallee, PROC pfnCurrent, PROC pfnNew)
{
	ULONG	uImportSize;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	pImportDesc	= (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
		hModCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &uImportSize);

	if(pImportDesc == NULL)
	{
		return;
	}

	for(; pImportDesc->Name; pImportDesc ++)
	{
		PCSTR pszModName	=
			(PCSTR) hModCaller + pImportDesc->Name;
		if(stricmp(pszModName, pszModCallee) == 0)
		{
			break;
		}
	}

	if(pImportDesc->Name == 0)
	{
		return;
	}

	PIMAGE_THUNK_DATA pThunk	= 
		(PIMAGE_THUNK_DATA) ((PBYTE)hModCaller + pImportDesc->FirstThunk);

	for(; pThunk->u1.Function; pThunk ++)
	{
		PROC* ppfnCurr	= (PROC*) &pThunk->u1.Function;

		if(*ppfnCurr == pfnCurrent)
		{
			BOOL b;
			DWORD dwOldProtect	= 0;
			b = VirtualProtect(ppfnCurr, sizeof(pfnNew), PAGE_READWRITE, &dwOldProtect);
			if(b)
			{
				b = WriteProcessMemory(GetCurrentProcess(),
					ppfnCurr, &pfnNew, sizeof(pfnNew), NULL);
			}
			char szModName[MAX_PATH];
			GetModuleFileName(hModCaller, szModName, MAX_PATH);
			if(b)
			{

				ADEBUG_TRACE1("succeed exchange <%s>.", szModName);
			}
			else
			{
				ADEBUG_TRACE1("failed exchange <%s>.", szModName);
			}
			break;
		}
	}
}

ADebugHookTool::AHeapHook::AHeapHook():
m_nHeapSize			(0),
m_uAllocCount		(0),
m_uSmallAllocCount	(0),
m_uMemCount			(0),
m_uSmallCount		(0),
m_uAllocTime		(0),
m_uFreeTime			(0)
{
	m_recHeapReAlloc.Init("kernel32.dll", "HeapReAlloc", (FARPROC)hook_HeapReAlloc);
	m_recHeapAlloc.Init("kernel32.dll", "HeapAlloc", (FARPROC)hook_HeapAlloc);
	m_recHeapFree.Init("kernel32.dll", "HeapFree", (FARPROC)hook_HeapFree);
}

ADebugHookTool::AHeapHook::~AHeapHook()
{
	ADebug::Msg(0, "memstat begin");

	ADebug::Msg(0, "alloc count : %u", m_uAllocCount);
	ADebug::Msg(0, "small alloc count : %u", m_uSmallAllocCount);
	ADebug::Msg(0, "alloc time(s) : %f", (DOUBLE)((__int64)m_uAllocTime)/(DOUBLE)((__int64)ADebugHookTool::m_Tool.m_uCpuFreq));
	ADebug::Msg(0, "free time(s) : %f", (DOUBLE)((__int64)m_uFreeTime)/(DOUBLE)((__int64)ADebugHookTool::m_Tool.m_uCpuFreq));

	ADebug::Msg(0, "memstat end");

	m_recHeapFree.Free();
	m_recHeapAlloc.Free();
	m_recHeapReAlloc.Free();
}

LPVOID WINAPI ADebugHookTool::AHeapHook::hook_HeapReAlloc(
	IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem, IN SIZE_T dwBytes)
{
	return m_Tool.m_HeapHook.do_HeapReAlloc(hHeap, dwFlags, lpMem, dwBytes);
}
LPVOID WINAPI ADebugHookTool::AHeapHook::hook_HeapAlloc(
	HANDLE hHeap, DWORD dwFlags, DWORD dwBytes)
{
	return m_Tool.m_HeapHook.do_HeapAlloc(hHeap, dwFlags, dwBytes);
}
BOOL WINAPI ADebugHookTool::AHeapHook::hook_HeapFree(
	IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem)
{
	return m_Tool.m_HeapHook.do_HeapFree(hHeap, dwFlags, lpMem);
}

LPVOID WINAPI ADebugHookTool::AHeapHook::do_HeapReAlloc(
	IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem, IN SIZE_T dwBytes)
{
	typedef LPVOID (WINAPI* POldProc)(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem, IN SIZE_T dwBytes);

	UINT64		uCurrent;
	POldProc	pfnOld;
	PVOID		pRet;
	SIZE_T		nAllocSize;

	nAllocSize		= ::HeapSize(hHeap, dwFlags, lpMem);
	m_nHeapSize		-= nAllocSize;
	if(nAllocSize <= 1024)
	{
		m_uSmallCount --;
	}

	pfnOld			= (POldProc)m_recHeapReAlloc.m_pfnOld;

	uCurrent		= ADebug::GetCpuCount();
	pRet			= pfnOld(hHeap, dwFlags, lpMem, dwBytes);
	uCurrent		= ADebug::GetCpuCount() - uCurrent;
	m_uAllocTime	+= uCurrent - m_Tool.m_uCpuOverhead;
	nAllocSize		= ::HeapSize(hHeap, dwFlags, pRet);
	m_nHeapSize		+= nAllocSize;
	m_uAllocCount	++;
	if(nAllocSize <= 1024)
	{
		m_uSmallAllocCount	++;
		m_uSmallCount		++;
	}

	return pRet;
}
LPVOID WINAPI ADebugHookTool::AHeapHook::do_HeapAlloc(
	HANDLE hHeap, DWORD dwFlags, DWORD dwBytes)
{
	typedef LPVOID (WINAPI* POldProc)(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes);

	UINT64		uCurrent;
	POldProc	pfnOld;
	PVOID		pRet;
	SIZE_T		nAllocSize;
	
	pfnOld			= (POldProc)m_recHeapAlloc.m_pfnOld;
	
	uCurrent		= ADebug::GetCpuCount();
	pRet			= pfnOld(hHeap, dwFlags, dwBytes);
	uCurrent		= ADebug::GetCpuCount() - uCurrent;
	m_uAllocTime	+= uCurrent - m_Tool.m_uCpuOverhead;
	nAllocSize		= ::HeapSize(hHeap, dwFlags, pRet);
	m_nHeapSize		+= nAllocSize;
	m_uAllocCount	++;
	if(nAllocSize <= 1024)
	{
		m_uSmallAllocCount	++;
		m_uSmallCount		++;
	}

	m_uMemCount	++;
	return pRet;
}
BOOL WINAPI ADebugHookTool::AHeapHook::do_HeapFree(
	IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem)
{
	typedef BOOL (WINAPI* POldProc)(IN HANDLE hHeap, IN DWORD dwFlags, IN LPVOID lpMem);

	UINT64		uCurrent;
	POldProc	pfnOld;
	BOOL		bRet;
	SIZE_T		nAllocSize;

	nAllocSize		= ::HeapSize(hHeap, dwFlags, lpMem);
	m_nHeapSize		-= nAllocSize;
	if(nAllocSize <= 1024)
	{
		m_uSmallCount --;
	}
	
	pfnOld			= (POldProc)m_recHeapFree.m_pfnOld;
	
	uCurrent		= ADebug::GetCpuCount();
	bRet			= pfnOld(hHeap, dwFlags, lpMem);
	uCurrent		= ADebug::GetCpuCount() - uCurrent;
	m_uFreeTime		+= uCurrent - m_Tool.m_uCpuOverhead;

	m_uMemCount	--;

	return bRet;
}

ADebugHookTool::ADebugHookTool():
m_uCpuFreq		(1000000000),
m_uCpuOverhead	(500)
{
	UINT64	uMaxDelta		= 10;
	UINT64	uPerfFreq		= 0;
	UINT64	uPerfCounter0	= 0;
	UINT64	uPerfCounter1	= 0;
	UINT64	uPerfTemp0		= 0;
	UINT64	uPerfTemp1		= 0;
	UINT64	uCpuCounter0	= 0;
	UINT64	uCpuCounter1	= 0;
	INT		nMaxTry			= 10;
	INT		nTryCount;
	INT		i;

	for(i = 0; i < 8; i ++)
	{
		UINT64	uCounter	= ADebug::GetCpuCount();
		uCounter	= ADebug::GetCpuCount() - uCounter;
		if(uCounter < m_uCpuOverhead)
		{
			m_uCpuOverhead	= uCounter;
		}
	}
	
	nTryCount	= 0;
	do
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&uPerfTemp0);
		uCpuCounter0	= ADebug::GetCpuCount();
		::QueryPerformanceCounter((LARGE_INTEGER*)&uPerfTemp1);
		nTryCount++;
		if(nTryCount >= nMaxTry)
		{
			goto label_failed;
		}
	}while(uPerfTemp1-uPerfTemp0 > uMaxDelta);
	uPerfCounter0	= uPerfTemp0;

	nTryCount	= 0;
	do
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&uPerfTemp0);
		uCpuCounter1	= ADebug::GetCpuCount();
		::QueryPerformanceCounter((LARGE_INTEGER*)&uPerfTemp1);
		nTryCount++;
		if(nTryCount >= nMaxTry)
		{
			goto label_failed;
		}
	}while(uPerfTemp1-uPerfTemp0 > uMaxDelta);
	uPerfCounter1	= uPerfTemp0;

	if(!::QueryPerformanceFrequency((LARGE_INTEGER*)&uPerfFreq))
	{
		goto label_failed;
	}

	m_uCpuFreq	= (uCpuCounter1-uCpuCounter0)*
		uPerfFreq/(uPerfCounter1-uPerfCounter0);

	return;

label_failed:

	return;
}

ADebugHookTool::~ADebugHookTool()
{
}

#pragma warning(disable : 4073)
#pragma init_seg(lib)

ADebugHookTool ADebugHookTool::m_Tool;

#else

VOID ADebugMemStat(
	SIZE_T&	nHeapSize,
	UINT& uAllocCount,
	UINT& uSmallAllocCount,
	UINT& uMemCount,
	UINT& uSmallCount,
	UINT64& uAllocTime,
	UINT64& uFreeTime
)
{
}

VOID ADebugMemStat(
	SIZE_T&	nHeapSize,
	UINT& uAllocCount,
	UINT& uSmallAllocCount,
	UINT& uMemCount,
	UINT& uSmallCount,
	DOUBLE& fAllocTime,
	DOUBLE& fFreeTime
)
{
}

SIZE_T ADebugMemSize()
{
	return 0;
}

#endif	// ADEBUG_HOOK_ENABLE