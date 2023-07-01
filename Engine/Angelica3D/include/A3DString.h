/*
 * FILE: A3DString.h
 *
 * DESCRIPTION: Routines for drawing text strings
 *
 * CREATED BY: duyuxin, 2001/11/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSTRING_H_
#define _A3DSTRING_H_

#include "A3DPlatform.h"
#include "A3DFuncs.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DFontMan.h"
#include "A3DViewport.h"

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
//	Class A3DString
//
///////////////////////////////////////////////////////////////////////////

class A3DString
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DString();
	virtual ~A3DString();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pDevice, char* szText, HA3DFONT hA3DFont, bool bFilter,
					 bool bCharSize=false);		//	Initialize object
	void		Release();		//	Release object

	bool		DrawText(int x, int y, A3DCOLOR Color, int iHeight=0);	//	Output 2D text
	bool		Draw3DText(A3DViewport* pView, A3DVECTOR3& vPos, float fRatio, A3DCOLOR Color);	//	Draw 3D text
	bool		GetCharsExtent(int iStart, int iEnd, int* piWid, int* piHei);	//	Get sub-string's extent

	void		EnableFilter(bool bEnable)				{	m_bFilter = bEnable;	}
	void		GetTextExtent(int* piWid, int* piHei)	{	*piWid = m_iTextExtX; *piHei = m_iTextExtY;	}
	int			GetTextLength()							{	return m_iTextLen;		}
	void		SetOutputHeight(int iHei)				{	m_iOutHei = iHei;		}
	int			GetOutputHeight()						{	return m_iOutHei;		}

protected:	//	Attributes

	bool		m_bHWIString;	//	Using outside graphics engine

	A3DDevice*	m_pA3DDevice;	//	A3D device object
	A3DStream*	m_pA3DStream;	//	A3D stream object
	A3DTexture*	m_pA3DTexture;	//	A3D texture object

	bool		m_bFilter;		//	Filter flag
	bool		m_bReady;		//	Ready flag

	int			m_iTexWid;		//	Texture's width
	int			m_iTexHei;		//	Texture's height
	int			m_iLineHei;		//	Height of every line in texture
	int			m_iNumLines;	//	Number of line of text laid in texture
	int			m_iLastWid;		//	Width of text on the last line of texture
	int			m_iOutHei;		//	Output height

	int			m_iTextExtX;	//	Text's real extent on screen
	int			m_iTextExtY;

	int*		m_aCharWids;	//	Every character's width
	int			m_iTextLen;		//	Text length in character

protected:	//	Operations

	bool		CalculateTextureSize(HDC hDC, char* szText, SIZE* pSize);	//	Calculate texture size
	bool		CreateStream();								//	Create vertices and indices
	bool		FillTexture(BYTE* pDIBData, int iPitch);	//	Fill text's texture
	bool		RecordCharWidth(char* szText, HDC hDC, HFONT hFont);	//	Record every character's width
	void		ClearControlChars(BYTE* szSrc, BYTE* szDest);			//	Clear control characters in text strings
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSTRING_H_



