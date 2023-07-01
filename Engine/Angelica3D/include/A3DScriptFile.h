/*
 * FILE: A3DScriptFile.h
 *
 * DESCRIPTION: Routines for script file
 *
 * CREATED BY: duyuxin, 2002/2/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSCRIPTFILE_H_
#define _A3DSCRIPTFILE_H_

#include "A3DFuncs.h"
#include "A3DFile.h"

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
//	Class A3DScriptFile
//
///////////////////////////////////////////////////////////////////////////

class A3DScriptFile
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

	A3DScriptFile();
	virtual ~A3DScriptFile();

public:		//	Attributes

	char		m_szToken[A3DFILE_LINEMAXLEN];

public:		//	Operations

	bool		Open(A3DFile* pFile);		//	Open an existing script file
	void		Close();					//	Close file

	void		ResetScriptFile();					//	Reset pointers
	bool		GetNextToken(bool bCrossLine);		//	Get next token and move file pointer forward
	bool		PeekNextToken(bool bCrossLine);		//	Peek next token and don't move file pointer
	bool		SkinLine();							//	Skip current line
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



#endif	//	_A3DSCRIPTFILE_H_



