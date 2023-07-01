/*
 * FILE: A3DFontMan.cpp
 *
 * DESCRIPTION: Routines for managing font used by A3D
 *
 * CREATED BY: hedi, duyuxin, 2001/4/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DFontMan.h"
#include "A3DErrLog.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DFontMan::A3DFontMan()
{
	m_hDC		= NULL;
	m_iNumFont	= 0;
	m_hBitmap	= NULL;

	memset(m_aFonts, 0, sizeof (m_aFonts));
}

A3DFontMan::~A3DFontMan()
{
}

//	Initialize object.
//	Return true for success, otherwise return false
bool A3DFontMan::Init()
{
	//	Create memory DC
	if (!(m_hDC = CreateCompatibleDC(NULL)))
	{
		g_pA3DErrLog->ErrLog("A3DFontMan::Init, Failed to create memory DC.");
		return false;
	}

	BITMAPINFO bmi;
	memset(&bmi.bmiHeader, 0, sizeof (BITMAPINFOHEADER));
	bmi.bmiHeader.biSize        = sizeof (BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = 4;
	bmi.bmiHeader.biHeight      = -4;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	DWORD* pDIBData;	//	Address of memory bitmap's data
	m_hBitmap = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, (void**) &pDIBData, NULL, 0);
	if (!m_hBitmap)
	{
		g_pA3DErrLog->ErrLog("A3DFontMan::Init, Failed to create bitmap.");
		return false;
	}

	SetMapMode(m_hDC, MM_TEXT);

	return true;
}

//	Release object's resource
void A3DFontMan::Release()
{
	int i;
	for (i=0; i < m_iNumFont; i++)
	{
		if (m_aFonts[i].hFont)
			DeleteObject(m_aFonts[i].hFont);
	}

	memset(m_aFonts, 0, sizeof (m_aFonts));

	m_iNumFont = 0;

	//	Release windows DC
	if (m_hDC)
	{
		DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	if (m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
}

/*	Register font

	Return font's handle for success, otherwise return 0.

	szName: name of font will be created
	iHeight: font's height
	dwFlags: A3D font flag
*/
HA3DFONT A3DFontMan::RegisterFont(char* szName, int iHeight, DWORD dwFlags)
{
	PFONTITEM pFont;
	HA3DFONT hA3DFont;

	//	This font has been registered ?
	if ((hA3DFont = SearchFont(szName, iHeight, dwFlags)))
		return hA3DFont;

	if (m_iNumFont >= MAXNUM_FONT)
	{
		g_pA3DErrLog->ErrLog("A3DFontMan::RegisterFont, Too many font registered.");
		return 0;
	}

	pFont = &m_aFonts[m_iNumFont];

	//	Create font
	DWORD dwBold	= (dwFlags & A3DFONT_BOLD) ? FW_BOLD : FW_NORMAL;
	DWORD dwItalic	= (dwFlags & A3DFONT_ITALIC) ? TRUE : FALSE;
	HFONT hFont		= CreateFont(-iHeight, 0, 0, 0, dwBold, dwItalic,
								 FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
								 VARIABLE_PITCH, szName);
	if (!hFont)
	{
		g_pA3DErrLog->ErrLog("A3DFontMan::RegisterFont, Failed to register font.");
		return 0;
	}

	//	Remove A3DFONT_FILTERED flag
	dwFlags &= ~A3DFONT_FILTERED;

	pFont->hFont	= hFont;
	pFont->dwFlags	= dwFlags;
	pFont->iHeight	= iHeight;
	pFont->iOutHei	= CaluOutputFont(hFont);

	strcpy(pFont->szName, szName);

	return (DWORD)(++m_iNumFont);
}

/*	Register a new font with same type and properties as specified old font
	but new height.

	Return font's handle for success, otherwise return 0.

	hOldFont: valid A3D font's handle
	iHeight: new height of font.
*/
HA3DFONT A3DFontMan::RegisterFont(HA3DFONT hOldFont, int iHeight)
{
	int iIndex = (int)(hOldFont - 1);

	if (iIndex < 0 || iIndex >= m_iNumFont)
		return 0;

	return RegisterFont(m_aFonts[iIndex].szName, iHeight, m_aFonts[iIndex].dwFlags);
}

/*	Search a font

	Return font's handle for success, otherwise return 0.

	szName: font's name
	iHeight: font's height
	dwFlags: A3D font flag
*/
HA3DFONT A3DFontMan::SearchFont(char* szName, int iHeight, DWORD dwFlags)
{
	int i;
	for (i=0; i < m_iNumFont; i++)
	{
		if (iHeight == m_aFonts[i].iHeight && dwFlags == m_aFonts[i].dwFlags &&
			!strcmp(szName, m_aFonts[i].szName))
			return (DWORD)(i+1);
	}

	return 0;
}

/*	Get text string's extent

	Return true for success, otherwise return false.
	
	hA3DFont: A3D font handle.
	szText: text string.
	iNumChar: number of character will be calculated in szText
	piWid, piHei (out): used to receive text string's extent, can be NULL
*/
bool A3DFontMan::GetTextExtent(HA3DFONT hA3DFont, char* szText, int iNumChar, int* piWid, int* piHei)
{
	int iIndex = (int)(hA3DFont - 1);

	if (iIndex < 0 || iIndex >= m_iNumFont)
		return false;

	HFONT hOldFont = (HFONT)SelectObject(m_hDC, m_aFonts[iIndex].hFont);

	//	Select a bitmap into DC is necessary for anti-aliased font
	HBITMAP hOldBmp	= (HBITMAP)SelectObject(m_hDC, m_hBitmap);

	SIZE Size;
	GetTextExtentPoint32(m_hDC, szText, iNumChar, &Size);
	
	if (piWid)
		*piWid = Size.cx;

	if (piHei)
		*piHei = Size.cy;

	//	Restore DC's resource
	SelectObject(m_hDC, hOldBmp);
	SelectObject(m_hDC, hOldFont);

	return true;
}

/*	Calculate font's output height

	Return font's output height.

	hFont: font's handle
*/
int	A3DFontMan::CaluOutputFont(HFONT hFont)
{
	HFONT hOldFont = (HFONT)SelectObject(m_hDC, hFont);

	//	Select a bitmap into DC is necessary for anti-aliased font
	HBITMAP hOldBmp	= (HBITMAP)SelectObject(m_hDC, m_hBitmap);

	SIZE Size;
	GetTextExtentPoint32(m_hDC, "W", 1, &Size);

	SelectObject(m_hDC, hOldBmp);
	SelectObject(m_hDC, hOldFont);

	return Size.cy;
}

