/*
 * FILE: AGame.h
 *
 * DESCRIPTION: 调试信息类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */
#ifndef __ADEBUG__H__
#define __ADEBUG__H__

#include "AMemLeak.h"

enum 
{
	DEBUGLEVEL_FATAL	= 0,		//致命错误
	DEBUGLEVEL_WARNING	= 1,		//警告，（可能出现的错误）
	DEBUGLEVEL_TRACE	= 2,		//程序调试信息

};
//#define		AERROR(errorMsg)	
class ADebug  
{
public:
	static void Msg(int __nLevel,const char *__str,...);
	static void MsgStr(int __nLevel,const char *__str, ...);
	static void MsgToFile(char * strInfo);
	static unsigned __int64	GetCpuCount();
	ADebug();
	virtual ~ADebug();

};

// By JiangLi
#define TRY_METHOD(method, info) \
if(!method)\
{\
	ADebug::Msg(0, "%s(%d) : %s", __FILE__, __LINE__, info);\
	assert(false);\
	return false;\
}

#define	ABASE_SAFE_RELEASE(p) if(p != NULL) {p->Release(); delete p; p = NULL;}
#define	ABASE_SAFE_DELETE(p) if(p != NULL) {delete[] p; p = NULL;}

#define ADEBUG_TRACE(info) ADebug::Msg(0, "%s(%d) : %s", __FILE__, __LINE__, info)

#define ADEBUG_TRACE1(info, param1) \
	ADebug::MsgStr(0, "%s(%d) : ", __FILE__, __LINE__);\
	ADebug::Msg(0, info, param1);

#define ADEBUG_TRACE2(info, param1, param2) \
	ADebug::MsgStr(0, "%s(%d) : ", __FILE__, __LINE__);\
	ADebug::Msg(0, info, param1, param2);

#define ADEBUG_TRACE3(info, param1, param2, param3) \
	ADebug::MsgStr(0, "%s(%d) : ", __FILE__, __LINE__);\
	ADebug::Msg(0, info, param1, param2, param3);

#define ADEBUG_TIME_TEST(Func) \
{\
	unsigned __int64 T = ADebug::GetCpuCount();\
	Func;\
	T = ADebug::GetCpuCount() - T;\
	ADEBUG_TRACE1("Function "#Func" time : %d", (INT)T);\
}

#endif // __ADEBUG__H__