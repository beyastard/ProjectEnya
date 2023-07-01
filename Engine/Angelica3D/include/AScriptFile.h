/*
 * FILE: AScriptFile.h
 *
 * DESCRIPTION: Routines for script file
 *
 * CREATED BY: duyuxin, 2002/2/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ASCRIPTFILE_H_
#define _ASCRIPTFILE_H_

#include "AFile.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class AScriptFile
//
///////////////////////////////////////////////////////////////////////////

class AScriptFile
{
public:		//	Types

	typedef struct _SCRIPTINFO
	{
		BYTE*	pStart;		//	Start address of buffer
		BYTE*	pEnd;		//	End address of buffer
		BYTE*	pCur;		//	Current pointer
		int		iLine;		//	Line counter

	} SCRIPTINFO, *PSCRIPTINFO;

public:		//	Constructors and Destructors

	AScriptFile();
	virtual ~AScriptFile();

public:		//	Attributes

	char		m_szToken[AFILE_LINEMAXLEN];

public:		//	Operations

	bool		Open(AFile* pFile);		//	Open an existing script file
	void		Close();					//	Close file

	void		ResetScriptFile();					//	Reset pointers
	bool		GetNextToken(bool bCrossLine);		//	Get next token and move file pointer forward
	bool		PeekNextToken(bool bCrossLine);		//	Peek next token and don't move file pointer
	bool		SkipLine();							//	Skip current line
	bool		MatchToken(char* szToken, bool bCaseSensitive);		//	Search specified token

	bool		IsEnd()			{	return m_Script.pCur >= m_Script.pEnd;	}

protected:	//	Attributes

	SCRIPTINFO	m_Script;		//	Script file information

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////



#endif	//	_ASCRIPTFILE_H_



