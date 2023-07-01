/*
 * FILE: ABaseTypes.h
 *
 * DESCRIPTION: 定义各种宏和数据结构
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef  __ABASETYPES__H__
#define  __ABASETYPES__H__

#include "windows.h"

class	AGame;
class	ADeviceMan;
class	AWorld;
class	A3DEngine;

//-----------------------------------------
#ifndef WIN32

#include <sys/types.h>

typedef u_int16_t WORD
typedef u_int32_t DWORD

#else

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef	__int64			__quad_t;
typedef	unsigned __int64	__u_quad_t;
	
//__BIT_TYPES_DEFINED__
typedef		unsigned __int8	u_int8_t;
typedef		__int8		int8_t;
typedef		unsigned __int16 u_int16_t;
typedef		__int16		int16_t;
typedef		unsigned __int32 u_int32_t;
typedef		__int32		int32_t;
typedef		__int64		int64_t;
typedef		unsigned __int64 u_int64_t;
typedef		__quad_t	quad_t;
typedef		__u_quad_t	u_quad_t;

// other
typedef		unsigned __int8	 uint8_t;
typedef		unsigned __int16 uint16_t;
typedef		unsigned __int32 uint32_t;
typedef		unsigned __int64 uint64_t;

#endif 

typedef char * LPSTR;
//-------------------------------------------
struct AMSG
{
	int		nSize;
	int		hObject;
	int		nType;
	int		message;
};

typedef AMSG *PAMSG;

#define ENGINETICKTIME 33.3F

#endif   //__ECLTYPES__H__