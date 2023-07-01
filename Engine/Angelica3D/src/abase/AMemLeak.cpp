#define ADEBUG_HIDE_OPERATORS
#include "AMemLeak.h"
#include "ADebug.h"
#include "ABaseTypes.h"
#include "stdio.h"

#define ADEBUG_MAX_DEBUG_BUFFER	1024

#ifndef	ADEBUG_NO_LEAK_TEST

#ifdef	_DEBUG
#include <crtdbg.h>
#else
#define	_DEBUG
#include <crtdbg.h>
#undef	_DEBUG
#endif	// _DEBUG

class ADumpMemoryLeak
{
private:
	static BOOL volatile	m_bInitialized;
	static BOOL volatile	m_bInitCalled;
	static _CRT_REPORT_HOOK	m_old_report_hook;

	static BOOL Initialize();
	static VOID Finalize();

	static void __cdecl at_exit_hook();
	static int  __cdecl vc_report_hook( int reportType, char *message, int *returnValue );

public:
	static void* Malloc(size_t uSize, const char* pszFileName, int nLine);
};

BOOL volatile	ADumpMemoryLeak::m_bInitialized	= FALSE;
BOOL volatile	ADumpMemoryLeak::m_bInitCalled	= ADumpMemoryLeak::Initialize();
_CRT_REPORT_HOOK	ADumpMemoryLeak::m_old_report_hook	= NULL;

static long nMaxRequest	= 0;

int __cdecl MyAllocHook(
   int      nAllocType,
   void   * pvData,
   size_t   nSize,
   int      nBlockUse,
   long     lRequest,
   const unsigned char * szFileName,
   int      nLine
   )
{
	/*/if(nAllocType == _HOOK_ALLOC)
	{
		nMaxRequest ++;
	}/**/
	/*static int nCount	= 0;
	static int nLast	= 0;
	static int nMatch[]	= 
	{
		12, 12, 12, 12, 12,
		12, 12, 12, 12, 12,
		12, 12, 12, 12, 12,
		12, 12, 12, 364, 444,
		12, 12, 12, 12, 924,
		364, 444, 12, 12, 12,
		12, 924, 364, 12, 12,
		12, 960, 12
	};
	if(nAllocType == _HOOK_ALLOC)
	{
		if((int)nSize == nMatch[nCount])
		{
			if(nLast + 1 == lRequest || nCount == 0)
			{
				nCount ++;
			}
			else
			{
				nCount = 0;
			}
			nLast	= lRequest;
		}
		else
		{
			if(nCount >= 17 && nSize == 364)
			{
			//	OutputDebugString("find\n");
			}
			nCount	= 0;
		}
		if(nCount == sizeof(nMatch)/sizeof(int))
		{
			//OutputDebugString("find\n");
			nCount = 0;
		}
	}*
	if(lRequest == 345)// || lRequest == 366197)// || lRequest == 36169)// && (int)nSize <= 0 && nAllocType == _HOOK_ALLOC)
	{
		if(nAllocType == _HOOK_ALLOC)
		{
			OutputDebugString("asdf\n");
		}
		else if(nAllocType == _HOOK_FREE)
		{
	//		OutputDebugString("asdf\n");
		}
	}/**/
	return TRUE;
}

BOOL ADumpMemoryLeak::Initialize()
{
	static volatile BOOL	bCalling	= FALSE;

	__asm
	{
		mov		eax,	1

Label_CallingTest:
		xchg	eax,	bCalling
		test	eax,	eax
		jne		Label_CallingTest
	}

	if(m_bInitialized)
	{
		return TRUE;
	}

	if(0 != ::atexit(at_exit_hook))
	{
		ADebug::Msg(0,"Canot set exit hook proc. Terminate!");
		//OutputDebugString("Canot set exit hook proc. Terminate!\n");
		exit(0);
		return FALSE;
	}

	_CrtSetAllocHook(MyAllocHook);

	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _crtDbgFlag);

	m_bInitialized	= TRUE;
	bCalling		= FALSE;
	return TRUE;
}

VOID ADumpMemoryLeak::Finalize()
{
	m_old_report_hook	= _CrtSetReportHook(vc_report_hook);
	//_CrtDumpMemoryLeaks();
	//_CrtSetReportHook(m_old_report_hook);
	//ADEBUG_TRACE1("nMaxRequest : %d \n", nMaxRequest);
}

void __cdecl ADumpMemoryLeak::at_exit_hook()
{
	Finalize();
}

int __cdecl ADumpMemoryLeak::vc_report_hook( int reportType, char *message, int *returnValue )
{
	static bool bOut	= true;
	static bool bLast	= false;
	static int nCount	= 0;
	static int nSize	= 0;

	if(message == 0 && message[0] == '\0')
	{
		returnValue	= 0;
		return TRUE;
	}
	/**/
	if(message[0] == '{')
	{
		if(!bLast)
			bOut = false;
	}
	else if(message[1] == ':')
	{
		bOut	= true;
		bLast	= true;
	}
	else
	{
		bLast	= false;
	}
//	bOut	= true;
	if(bOut)
	/**/
	{
		ADebug::MsgStr(0, message);
	}
	else
	{
		int adrr, len;
		if(sscanf(message, "normal block at 0x%x, %d bytes long.", &adrr, &len) >= 2)
		{
			nCount ++;
			nSize  += len;
		}
	}
	//if(m_old_report_hook != NULL)
	//{
	//	return (*m_old_report_hook)(reportType, message, returnValue);
	//}

	//if(message[0] == 'O')
	//	ADebug::Msg(0, message);
	if(0 == strcmp(message, "Object dump complete.\n"))
	{
		if(!bOut)
		{
			ADebug::MsgStr(0, message);
		}
		_CrtSetReportHook(m_old_report_hook);
		if(nCount > 0)
		{
			ADebug::MsgStr(0, "无文件信息的内存泄漏 %d 处，总长度 %d 字节。\n", nCount, nSize);
		}
	}
	returnValue	= 0;
	return TRUE;
}

void* ADumpMemoryLeak::Malloc(size_t uSize, const char* pszFileName, int nLine)
{
	if(!m_bInitialized)
	{
		if(!Initialize())
		{
			return NULL;
		}
	}

	return ::operator new(uSize, _NORMAL_BLOCK, pszFileName, nLine);
}

void* operator new(size_t uSize, const char* pszFileName, int nLine, void* pReserved)
{
	return ADumpMemoryLeak::Malloc(uSize, pszFileName, nLine);
}

void* operator new[](size_t uSize, const char* pszFileName, int nLine, void* pReserved)
{
	return ADumpMemoryLeak::Malloc(uSize, pszFileName, nLine);
}
	
void operator delete(void* pMem, const char* pszFileName, int nLine, void* pReserved)
{
	delete pMem;
}

void operator delete[](void* pMem, const char* pszFileName, int nLine, void* pReserved)
{
	delete pMem;
}

#endif	// ADEBUG_NO_LEAK_TEST

#ifdef	_DEBUG

void AASSERT_SAVE(const char* pszFileName, int nLine)
{
	CHAR szInfoBuf[ADEBUG_MAX_DEBUG_BUFFER];

	::wsprintf(szInfoBuf, "Assertion failed in file <%s> in line <%d>\r\n", pszFileName, nLine);
	OutputDebugString(szInfoBuf);	
}

void ATRACE(const char* pszInfo)
{
	OutputDebugString(pszInfo);
}

#endif	// _DEBUG