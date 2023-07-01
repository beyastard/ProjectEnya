/*
 * FILE: A3DFont.h
 *
 * DESCRIPTION: Routines for display font
 *
 * CREATED BY: hedi, duyuxin, 2001/11/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DFONT_H_
#define _A3DFONT_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DTexture.h"
#include "A3DVertex.h"
#include "A3DStream.h"
#include "A3DDevice.h"
#include "A3DFontMan.h"
#include "A3DEngine.h"

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
//	Class A3DFont
//
///////////////////////////////////////////////////////////////////////////

class A3DFont
{
public:		//	Types

	enum
	{
		FONT_TRUETYPE	= 0,	//	Window's truetype font
		FONT_MAPFILE	= 1		//	Map file font
	};

	//	Character properties
	typedef struct _CHARPROP
	{
		float		aTexCoords[4];		//	Texture coordinates

	} CHARPROP, *PCHARPROP;

public:		//	Constructors and Destructors

	A3DFont();
	virtual ~A3DFont();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pDevice, HA3DFONT hA3DFont, bool bFilter);
	bool		Load(A3DDevice* pDevice, char* szFontMap, int chWidth, int chHeight, int cellWidth, int cellHeight);
					/*int nTexWidth, int nTexHeight, int nTexRow, int nTexCol);*/
	bool		Release();

	bool		TextOut(int x, int y, char* szTextOrg, A3DCOLOR color, int iHeight=0);
	void		GetTextExtent(char* szText, int* piWid, int* piHei);

	void		SetOutputHeight(int iHei)	{	m_iOutHei = iHei;		}
	int			GetOutputHeight()			{	return m_iOutHei;		}

protected:	//	Attributes
	bool		m_bHWIFont;				// Flag indicates using outside graphics engine;

	A3DStream*	m_pA3DStream;
	A3DTexture	m_Texture;
	A3DDevice*	m_pA3DDevice;
	HA3DFONT	m_hA3DFont;

	int			m_iType;				//	Font type.

	int			m_nCharWidth;			//	Output character size
	int			m_nCharHeight;
	int			m_nMapWidth;			//	Texture size
	int			m_nMapHeight;

	//	Variables for turetype font
	bool		m_bFilter;				//	Filter flag
	float		m_fTextScale;			//	Font scale
	CHARPROP	m_aCharSet[96];			//	128-32 characters

	//	Variables for map file font
	int			m_nCellWidth;			//	character size on texture
	int			m_nCellHeight;
	float		m_fCellWidth;			//	character size normalized to 0-1
	float		m_fCellHeight;
	int			m_iOutHei;				//	Output height

	char		m_szFontMap[MAX_PATH];

protected:	//	Operations

	bool		CreateStream();			//	Create stream
	bool		MapFileTextOut(int x, int y, char* szTextOrg, A3DCOLOR color);		//	Textout text using map file font
	bool		TrueTypeTextOut(int x, int y, char * szTextOrg, A3DCOLOR color,
								int iHeight);	//	Textout text using truetype font
};

typedef A3DFont * PA3DFont;


#endif	//	_A3DFONT_H_