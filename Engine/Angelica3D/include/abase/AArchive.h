/*
 * FILE: AArchive.h
 *
 * DESCRIPTION: 串行化动作类AArchive
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef _AArchive_HEADER_
#define _AArchive_HEADER_

#include <stdio.h>
#include "AStream.h"
#include "AClassInfo.h"
#include "AUString.h"

class AObject;

class AArchive  
{
public:
	bool ReadString(AUString & str);
	bool WriteString(AUString & str);

	AClassInfo * ReadClassInfo();
	bool WriteClassInfo(AClassInfo * pInfo);

	bool WriteClassInfo(AObject * pObject);
	AObject * CreateObject();

	bool Write(const void * buff, int size);
	bool Read(void * buff, int size);

	bool IsStoring();
	bool IsLoading();

	AArchive(AStream * pStream, bool bSave);
	virtual ~AArchive();

protected:

	AStream *	m_pStream;
	bool		m_bMode;
};

#endif
