/*
 * FILE: A3DErrLog.h
 *
 * DESCRIPTION: Class used to manipulate the error log file
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DERRLOG_H_
#define _A3DERRLOG_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"

class A3DErrLog
{
private:
	FILE *			m_pFile;

protected:
public:
	A3DErrLog();
	~A3DErrLog();

	bool Init(char * szLogFile, bool bAppend=false);
	bool Release();

	bool ErrLog(const char * fmt, ...);
	bool ErrString(char * szString);
};

typedef A3DErrLog * PA3DErrLog;

extern A3DErrLog * g_pA3DErrLog;

//#define ERRORLOG(fmt) {ErrLog fmt;}
//ERRORLOG(("%d",i));

#endif
