/*
 * FILE: ADebug.h
 *
 * DESCRIPTION: A "new" operator which is used to detect memory leaks
 *				in debug mode. 
 *
 * CREATED BY: JiangLi, 2002/08/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.	
 */

#ifndef	__ADEBUG_MEMLEAK_H__
#define __ADEBUG_MEMLEAK_H__

// 如果不需要进行内存泄漏的检测，那么就解除对#define ADEBUG_NO_LEAK_TEST的
// 注释。同时所有的库必须重新编译才能正常使用。

#ifndef _DEBUG
#define	ADEBUG_NO_LEAK_TEST
#endif

#ifndef	ADEBUG_NO_LEAK_TEST

// 必须在Release模式下显式链接必要的库文件，
// 否则在Release模式下会出现_CrtDumpMemoryLeaks未定义的错误信息。
// 以下是采用不同的运行库设置的时候需要链接的库文件。
#ifdef	_MT

#ifdef	_DLL

#pragma comment(lib, "MSVCRTD.LIB")

#else

#pragma comment(lib, "LIBCMTD.LIB")

#endif	// _DLL

#else	// _MT

#pragma comment(lib, "LIBCD.LIB")

#endif	// _MT

#pragma comment(linker, "/NODEFAULTLIB:LIBCMT.LIB")
#pragma comment(linker, "/NODEFAULTLIB:LIBC.LIB")
#pragma comment(linker, "/NODEFAULTLIB:MSVCRT.LIB")

// 如果定义了ADEBUG_NO_LEAK_TEST，那么就不会进行内存泄漏的监测。
// 如果没有定义，那么即使在Release模式下也会检查内存泄漏问题。
#ifdef	ADEBUG_NO_LEAK_TEST

#define ADEBUG_NEW new

#else	// ADEBUG_NO_LEAK_TEST

#define	ADEBUG_NEW new(__FILE__, __LINE__, (void*)NULL)

#endif	// ADEBUG_NO_LEAK_TEST

void* operator new(size_t nSize, const char* pszFileName, int nLine, void* pReserved);
	
// define this functions to avoid warning C4291
void operator delete(void* pMem, const char* pszFileName, int nLine, void* pReserved);

#ifndef	ADEBUG_HIDE_OPERATORS

#ifndef	_MFC_VER

#define new ADEBUG_NEW

#endif	// _MFC_VER

#endif	// ADEBUG_HIDE_OPERATORS

#endif	// ADEBUG_NO_LEAK_TEST


#ifdef	_DEBUG

void ATRACE(const char* pszInfo);
void AASSERT_SAVE(const char* pszFileName, int nLine);

#define AASSERT(BOOL_bExp) \
{\
	if(!(BOOL_bExp))\
	{\
		AASSERT_SAVE(__FILE__, __LINE__);\
	}\
}

#else

#define ATRACE(LPCSTR_pszInfo)
#define AASSERT(BOOL_bExp)

#endif	// _DEBUG

#endif	// __ADEBUG_MEMLEAK_H__