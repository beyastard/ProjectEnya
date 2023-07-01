/*
 * FILE: A3DFontMan.h
 *
 * DESCRIPTION: Routines for managing font used by A3D
 *
 * CREATED BY: hedi, duyuxin, 2001/4/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DFONTMAN_H_
#define _A3DFONTMAN_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Font flags
#define A3DFONT_BOLD		0x0001		//	Bold
#define A3DFONT_ITALIC		0x0002		//	Italic
#define A3DFONT_FILTERED	0x0100		//	Filterd

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

typedef DWORD	HA3DFONT;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DFontMan
//
///////////////////////////////////////////////////////////////////////////

class A3DFontMan
{
public:		//	Types

	enum
	{
		MAXNUM_FONT		= 32		//	Maximum number of font
	};

	typedef struct _FONTITEM
	{
		HFONT		hFont;			//	Font's handle
		char		szName[128];	//	Font's name
		int			iHeight;		//	User set height
		int			iOutHei;		//	Font's output height
		DWORD		dwFlags;		//	Flags

	} FONTITEM, *PFONTITEM;

public:		//	Constructors and Destructors

	A3DFontMan();
	virtual ~A3DFontMan();

public:		//	Attributes

public:		//	Operations

	bool		Init();
	void		Release();

	HA3DFONT	RegisterFont(char* szName, int iHeight, DWORD dwFlags);	//	Register font
	HA3DFONT	RegisterFont(HA3DFONT hOldFont, int iHeight);			//	Register font with new height
	HA3DFONT	SearchFont(char* szName, int iHeight, DWORD dwFlags);	//	Get a font by name
	bool		GetTextExtent(HA3DFONT hA3DFont, char* szText, int iNumChar, int* piWid, int* piHei);	//	Get text string's extent

	inline HFONT	GetWindowsFont(HA3DFONT hA3DFont);		//	Get windows font handle
	inline int		GetFontHeight(HA3DFONT hA3DFont);		//	Get font's height
	inline int		GetFontOutputHeight(HA3DFONT hA3DFont);	//	Get font's output height
	inline DWORD	GetFontFlags(HA3DFONT hA3DFont);		//	Get font's flags

	HDC				GetWindowsDC()	{	return m_hDC;	}

protected:	//	Attributes

	FONTITEM	m_aFonts[MAXNUM_FONT];		//	Information of fonts
	int			m_iNumFont;					//	Number of fonts in m_aFonts
	HDC			m_hDC;						//	Windows DC handle
	HBITMAP		m_hBitmap;					//	Bitmap

protected:	//	Operations

	int			CaluOutputFont(HFONT hFont);	//	Calculate font's output height
};

typedef A3DFontMan*	PA3DFontMan;

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////

/*	Get windows font handle.

	Return windows font handle for success, otherwise return NULL
*/
HFONT A3DFontMan::GetWindowsFont(HA3DFONT hA3DFont)
{
	int iIndex = (int)(hA3DFont - 1);

	if (iIndex < 0 || iIndex >= m_iNumFont)
		return NULL;

	return m_aFonts[iIndex].hFont;
}

//	Get font's height
int	A3DFontMan::GetFontHeight(HA3DFONT hA3DFont)
{
	int iIndex = (int)(hA3DFont - 1);

	if (iIndex < 0 || iIndex >= m_iNumFont)
		return 0;

	return m_aFonts[iIndex].iHeight;
}

//	Get font's output height
int	A3DFontMan::GetFontOutputHeight(HA3DFONT hA3DFont)
{
	int iIndex = (int)(hA3DFont - 1);

	if (iIndex < 0 || iIndex >= m_iNumFont)
		return 0;

	return m_aFonts[iIndex].iOutHei;
}

//	Get font's flags
DWORD A3DFontMan::GetFontFlags(HA3DFONT hA3DFont)
{
	int iIndex = (int)(hA3DFont - 1);

	if (iIndex < 0 || iIndex >= m_iNumFont)
		return 0;

	return m_aFonts[iIndex].dwFlags;
}


#endif	//	_A3DFONTMAN_H_


