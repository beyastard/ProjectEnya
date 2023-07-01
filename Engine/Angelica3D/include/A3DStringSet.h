/*
 * FILE: A3DStringSet.h
 *
 * DESCRIPTION: Routines for drawing multiline text strings at a same time
 *
 * CREATED BY: duyuxin, 2002/1/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSTRINGSET_H_
#define _A3DSTRINGSET_H_

#include "A3DPlatform.h"
#include "A3DFuncs.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DFontMan.h"

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
//	Class A3DStringSet
//
///////////////////////////////////////////////////////////////////////////

class A3DStringSet
{
public:		//	Types

	enum
	{
		MAXNUM_STRING	= 64	//	Maximum number of string
	};

	typedef struct _STRING
	{
		char*	szStr;			//	String
		int		iLen;			//	String length in bytes
		int		iIntent;		//	Output intent
		int		iExtX;			//	String's extent
		int		iExtY;
		DWORD	dwCol;			//	Color of string
		DWORD	dwSaveCol;		//	Saved color

		int		iFirstLine;		//	First line in texture
		int		iFirstOff;		//	Offset of the first line in texture
		int		iLastLine;		//	Last line in texture
		int		iLastOff;		//	Offset of the last line in texture

	} STRING, *PSTRING;

public:		//	Constructors and Destructors

	A3DStringSet();
	virtual ~A3DStringSet();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pDevice, HA3DFONT hA3DFont, bool bFilter,
					 bool bVertical=true);	//	Initialize object
	bool		Reset(bool bAll=true);		//	Reset
	void		Release();					//	Release object

	bool		Build();		//	Build texture and prepare to render

	bool		AddString(char* szStr, A3DCOLOR Color, int iIntent=0);	//	Add a string
	bool		GetStringExtent(int iIndex, int* piWid, int* piHei);	//	Get a string's extent
	bool		SetColor(int iIndex, A3DCOLOR Color);		//	Set color of a string or all strings
	bool		SetIntent(int iIndex, int iIntent);			//	Set intent of a string or all strings
	bool		SetIntents(int* aIntents);					//	Set all string's intents
	bool		GetOutputAreaSize(int* piWid, int* piHei, int iOutHei=0);	//	Get area size of output rectangle
	void		SaveColors();		//	Save all strings' color
	void		RestoreColors();	//	Restore all strings' color

	bool		OutputStrings(int x, int y, int iHeight=0);		//	Draw all strings

	void		EnableFilter(bool bEnable)	{	m_bFilter = bEnable;	}
	int			GetStringNum()				{	return m_iNumString;	}
	void		SetLineSpace(int iSpace)	{	m_iLineSpace = iSpace;	}
	int			GetLineSpace()				{	return m_iLineSpace;	}
	void		SetOutputHeight(int iHei)	{	m_iOutHei = iHei;		}
	int			GetOutputHeight()			{	return m_iOutHei;		}

protected:	//	Attributes

	bool		m_bHWIStringSet;	//	using outside graphics engine;

	A3DDevice*	m_pA3DDevice;	//	A3D device object
	A3DStream*	m_pA3DStream;	//	A3D stream object
	A3DTexture*	m_pA3DTexture;	//	A3D texture object

	bool		m_bReady;		//	Ready to output ?
	bool		m_bFilter;		//	Filter
	bool		m_bVertical;	//	true, All strings layout vertically
	int			m_iLineSpace;	//	Space of lines in pixel when output strings

	HA3DFONT	m_hA3DFont;		//	A3D font handle

	int			m_iTexWid;		//	Texture's width
	int			m_iTexHei;		//	Texture's height
	int			m_iLineHei;		//	Height of every line in texture
	int			m_iNumLines;	//	Number of line of text laid in texture
	int			m_iOutHei;		//	Output height

	STRING		m_aStrings[MAXNUM_STRING];	//	String array
	int			m_iNumString;	//	Number of string

protected:	//	Operations

	bool		CalculateTextureSize(SIZE* pSize);	//	Calculate texture size
	bool		CreateStream();						//	Create vertices and indices
	bool		FillTexture(BYTE* pDIBData, int iPitch);	//	Fill text's texture
	void		FillRect(BYTE* pDst, RECT* pDstRect, int iDstPitch, 
						 BYTE* pSrc, RECT* pSrcRect, int iSrcPitch);	//	Copy rectangle
	void		ClearControlChars(BYTE* szSrc, BYTE* szDest);			//	Clear control characters in text strings
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSTRINGSET_H_



