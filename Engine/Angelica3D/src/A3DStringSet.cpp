/*
 * FILE: A3DStringSet.cpp
 *
 * DESCRIPTION: Routines for drawing multiline text strings at a same time
 *
 * CREATED BY: duyuxin, 2002/1/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DErrLog.h"
#include "A3DStringSet.h"
#include "A3DEngine.h"
#include "A3DConfig.h"

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

A3DStringSet::A3DStringSet()
{
	m_pA3DDevice	= NULL;
	m_pA3DStream	= NULL;
	m_pA3DTexture	= NULL;

	m_bHWIStringSet	= false;

	m_iTexWid		= 0;
	m_iTexHei		= 0;
	m_iLineHei		= 0;
	m_iNumLines		= 0;
	m_iLineSpace	= 0;

	m_iNumString	= 0;
	m_iOutHei		= 16;
}

A3DStringSet::~A3DStringSet()
{
}

/*	Initialize object.

	Return true for success, otherwise return false

	pDevice: address of A3D device.
	hA3DFont: A3D font handle got from A3DFontMan
	bFilter: true, use antiliase font,
	bVertical: true, all strings layout vertically. 
			   false, all strings layout horizonally.
*/	
bool A3DStringSet::Init(A3DDevice* pDevice, HA3DFONT hA3DFont, bool bFilter,
						bool bVertical/* true */)
{
	if (!hA3DFont)
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::Init, invalid font handle.");
		return false;
	}

	m_pA3DDevice = pDevice;
	m_bFilter	 = bFilter;
	m_bVertical	 = bVertical;
	m_bReady	 = false;
	m_hA3DFont	 = hA3DFont;

	A3DFontMan* pFontMan = m_pA3DDevice->GetA3DEngine()->GetA3DFontMan();
	m_iOutHei = pFontMan->GetFontOutputHeight(hA3DFont);

	memset(m_aStrings, 0, sizeof (m_aStrings));
	m_iNumString = 0;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWIStringSet = true;
		return true;
	}

	//	Create texture and stream objects
	if (!(m_pA3DTexture = new A3DTexture))
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::Init Failed to create texture object.");
		Release();
		return false;
	}

	if (!(m_pA3DStream = new A3DStream))
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::Init Failed to create stream object.");
		Release();
		return false;
	}

	return true;
}

/*	Reset object.

	Return true for success, otherwise return false

	bAll: true, release strings, texture and vertex stream.
		  false, only release texture and vertex stream
*/
bool A3DStringSet::Reset(bool bAll/* true */)
{
	if (bAll)
	{
		//	Release all strings
		for (int i=0; i < m_iNumString; i++)
		{
			if (m_aStrings[i].szStr)
				free(m_aStrings[i].szStr);
		}

		memset(m_aStrings, 0, sizeof (m_aStrings));
		m_iNumString = 0;
	}

	if (m_pA3DTexture)
		m_pA3DTexture->Release();

	if (m_pA3DStream)
		m_pA3DStream->Release();

	m_bReady = false;

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DENGINE_OBJECT_STRING);
	return true;
}

//	Release all resource
void A3DStringSet::Release()
{
	Reset(true);

	//	Release texture and stream objects
	if (m_pA3DTexture)
	{
		delete m_pA3DTexture;
		m_pA3DTexture = NULL;
	}

	if (m_pA3DStream)
	{
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}

	m_hA3DFont	 = NULL;
	m_pA3DDevice = NULL;
}

/*	Add a string. All strings want to be rendered by A3DStringSet should be added by
	this function before A3DStringSet::Build is called, otherwise this operation
	will fail.

	Return true for success, otherwise return false

	szStr: text string
	Color: string's color
	iIntent: intent of this string when output
*/
bool A3DStringSet::AddString(char* szStr, A3DCOLOR Color, int iIntent/* 0 */)
{
	if (m_bHWIStringSet)	return true;

	if (m_bReady || m_iNumString >= MAXNUM_STRING)
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::AddString, Too many string in A3DStringSet or Build has been called");
		return false;
	}

	A3DFontMan* pFontMan = m_pA3DDevice->GetA3DEngine()->GetA3DFontMan();

	if (!szStr || !szStr[0])	//	Insert an empty string
	{
		m_aStrings[m_iNumString].szStr		= NULL;
		m_aStrings[m_iNumString].dwCol		= Color;
		m_aStrings[m_iNumString].dwSaveCol	= Color;
		m_aStrings[m_iNumString].iIntent	= iIntent;
		m_aStrings[m_iNumString].iExtX		= 0;
		m_aStrings[m_iNumString].iExtY		= pFontMan->GetFontOutputHeight(m_hA3DFont);

		m_iNumString++;
		return true;
	}

	m_aStrings[m_iNumString].szStr = (char*)malloc(strlen(szStr) + 1);
	ClearControlChars((BYTE*)szStr, (BYTE*)m_aStrings[m_iNumString].szStr);

	int iLen = strlen(m_aStrings[m_iNumString].szStr);

	m_aStrings[m_iNumString].iLen	 = iLen;
	m_aStrings[m_iNumString].dwCol	 = Color;
	m_aStrings[m_iNumString].iIntent = iIntent;

	//	Get string's extent from A3DFontMan
	pFontMan->GetTextExtent(m_hA3DFont, m_aStrings[m_iNumString].szStr, iLen, 
							&m_aStrings[m_iNumString].iExtX,
							&m_aStrings[m_iNumString].iExtY);

	m_iNumString++;

	return true;
}

//	Clear control characters in text strings
void A3DStringSet::ClearControlChars(BYTE* szSrc, BYTE* szDest)
{
	int i, j, iLen = strlen((char*)szSrc);

	for (i=0, j=0; i < iLen; i++)
	{
		if (szSrc[i] >= 32)
			szDest[j++] = szSrc[i];
	}

	szDest[j] = '\0';
}

/*	Get a string's extent.

	Return true for success, otherwise return false.

	iIndex: string's index.
	piWid, piHei (out): receive string's extent if true is returned. can be NULL
*/
bool A3DStringSet::GetStringExtent(int iIndex, int* piWid, int* piHei)
{
	if( m_bHWIStringSet )
	{
		*piWid = 0;
		*piHei = 0;
		return true;
	}

	if (iIndex < 0 || iIndex >= m_iNumString)
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::GetStringExtent, Invalid parameter");
		return false;
	}

	if (piWid)
		*piWid = m_aStrings[iIndex].iExtX;

	if (piHei)
		*piHei = m_aStrings[iIndex].iExtY;

	return true;
}

/*	Set color of a stirng or all strings.

	Return true for success, otherwise return false.

	iIndex: string's index. -1 means set color for all strings
	Color: new color of string
*/
bool A3DStringSet::SetColor(int iIndex, A3DCOLOR Color)
{
	if( m_bHWIStringSet ) return true;
	
	if (iIndex < -1 || iIndex >= m_iNumString)
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::SetColor, Invalid parameter");
		return false;
	}

	if (iIndex == -1)
	{
		for (int i=0; i < m_iNumString; i++)
			m_aStrings[i].dwCol = Color;
	}
	else
		m_aStrings[iIndex].dwCol = Color;

	return true;
}

/*	Set intent of a string or all strings.

	Return true for success, otherwise return false.

	iIndex: string's index. -1 means set indent for all strings
	iIntent: output intent of this string relative to the first string
*/
bool A3DStringSet::SetIntent(int iIndex, int iIntent)
{
	if (m_bHWIStringSet)
		return true;

	if (iIndex < -1 || iIndex >= m_iNumString)
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::SetIntent, Invalid parameter");
		return false;
	}

	if (iIndex == -1)
	{
		for (int i=0; i < m_iNumString; i++)
			m_aStrings[i].iIntent = iIntent;
	}
	else
		m_aStrings[iIndex].iIntent = iIntent;

	return true;
}

/*	Set all string's intents

	aIntents: intents of all string. Number of string in string set object can be
			  got from GetStringNum().
*/
bool A3DStringSet::SetIntents(int* aIntents)
{
	for (int i=0; i < m_iNumString; i++)
		m_aStrings[i].iIntent = aIntents[i];

	return true;
}

//	Save all strings' color
void A3DStringSet::SaveColors()
{
	for (int i=0; i < m_iNumString; i++)
		m_aStrings[i].dwSaveCol = m_aStrings[i].dwCol;
}

//	Restore all strings' color
void A3DStringSet::RestoreColors()
{
	for (int i=0; i < m_iNumString; i++)
		m_aStrings[i].dwCol = m_aStrings[i].dwSaveCol;
}

/*	Get area size of output rectangle.
	Note: only vertical string set can be calculated area size

	Return true for success, otherwise return false.

	piWid, piHei (out): receive output rectangle's size
	iOutHei: text height will be used when output strings, 0 means use font height
*/
bool A3DStringSet::GetOutputAreaSize(int* piWid, int* piHei, int iOutHei/* 0 */)
{
	if (m_bHWIStringSet || !m_iNumString || !m_bVertical)
	{
		if (piWid)	*piWid = 0;
		if (piHei)	*piHei = 0;
		return true;
	}

	int i, iHei, iFontHei, iLeft=9999, iRight=-9999;

	for (i=0; i < m_iNumString; i++)
	{
		if (m_aStrings[i].iIntent < iLeft)
			iLeft = m_aStrings[i].iIntent;

		if (m_aStrings[i].iIntent + m_aStrings[i].iExtX > iRight)
			iRight = m_aStrings[i].iIntent + m_aStrings[i].iExtX;
	}

	iFontHei = m_aStrings[0].iExtY;
	iHei	 = iOutHei ? iOutHei : iFontHei;

	if (piWid)
		*piWid = (int)ceil((iRight - iLeft + 1) / (float)iFontHei * iHei);

	if (piHei)
	{
		if (m_iNumString == 1)
			*piHei = m_iNumString * iHei;
		else
			*piHei = m_iNumString * iHei + (m_iNumString-1) * m_iLineSpace;
	}

	return true;
}

/*	Build texture and prepare to render. After this operation, no stirng can be added
	to A3DStringSet unless Reset() is called.

	Return true for success, otherwise return false.
*/
bool A3DStringSet::Build()
{
	if( m_bHWIStringSet )	return true;

	if (!m_pA3DTexture || !m_pA3DStream)
		return false;

	if (!m_iNumString || m_bReady)
		return true;

	//	Get windows's DC and font from A3DFontMan
	A3DFontMan* pFontMan = m_pA3DDevice->GetA3DEngine()->GetA3DFontMan();

	HDC	hDC	= pFontMan->GetWindowsDC();
	HFONT hOldFont, hFont;

	hFont	 = pFontMan->GetWindowsFont(m_hA3DFont);
	hOldFont = (HFONT)SelectObject(hDC, hFont);

	SIZE Size;
	HBITMAP hBitmap, hOldBmp;
	BYTE* pDIBData;
	
	//	Calculate texture size
	if (!CalculateTextureSize(&Size))
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::Build, invalid size.");
		return false;
	}

	//	All strings are empty line ?
	if (!Size.cx || !Size.cy)
		return true;	//	Only return true but don't set m_bReady flag

	//	Create texture
	if (!m_pA3DTexture->Create(m_pA3DDevice, m_iTexWid, m_iTexHei, A3DFMT_A4R4G4B4))
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::Build failed to create new texture.");
		return false;
	}

	//	Create a memory bitmap to store text
	BITMAPINFO bmi;
	memset(&bmi.bmiHeader, 0, sizeof (BITMAPINFOHEADER));
	bmi.bmiHeader.biSize        = sizeof (BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = Size.cx;
	bmi.bmiHeader.biHeight      = -Size.cy;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (VOID**) &pDIBData, NULL, 0);
	hOldBmp	= (HBITMAP)SelectObject(hDC, hBitmap);

	//	Clear bitmap
	int iPitch = ((Size.cx * 4 + 3) >> 2) << 2;
	memset(pDIBData, 0, Size.cy * iPitch);

	//	Set text properties
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0);
	SetTextAlign(hDC, TA_TOP);

	//	Select font into screen DC so that anti-aliased can do effect
	HDC hScreenDC = GetDC(NULL);
	HFONT hOldSFont = (HFONT)SelectObject(hScreenDC, hFont);

	//	Text out string to memory bitmap
	int i, y;
	for (i=0, y=0; i < m_iNumString; i++)
	{
		if (m_aStrings[i].szStr)
		{
			ExtTextOut(hDC, 0, y, ETO_OPAQUE, NULL, m_aStrings[i].szStr, 
					   m_aStrings[i].iLen, NULL);

			y += m_iLineHei;
		}
	}

	SelectObject(hScreenDC, hOldSFont);
	ReleaseDC(NULL, hScreenDC);

	//	Fill texture
	FillTexture((BYTE*)pDIBData, iPitch);

	//	Release resources
	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hOldBmp);
	DeleteObject(hBitmap);

	//	Create vertices and indices buffer
	if (!CreateStream())
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::Build, failed to create stream.");
		return false;
	}

	m_bReady = true;

	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DENGINE_OBJECT_STRING);
	return true;
}

/*	Calculate texture size. This function will consider texture to be square

	Return true for success, otherwise return false.

	pSize (out): used to receive DC's size on which text will be output
*/
bool A3DStringSet::CalculateTextureSize(SIZE* pSize)
{
	int i, iArea, iWid, iHei, iMaxWid=0, iLen=0;
	int iNumString;

	m_iLineHei = 0;
	iNumString = 0;		//	Number of none empty string

	for (i=0; i < m_iNumString; i++)
	{
		if (!m_aStrings[i].szStr)
			continue;

		iLen += m_aStrings[i].iExtX;

		if (m_aStrings[i].iExtY > m_iLineHei)
			m_iLineHei = m_aStrings[i].iExtY;

		if (m_aStrings[i].iExtX > iMaxWid)
			iMaxWid = m_aStrings[i].iExtX;

		iNumString++;
	}

	if (!iMaxWid)	//	All strings are empty line
	{
		pSize->cx	= 0;
		pSize->cy	= 0;
		m_iTexWid	= 0;
		m_iTexHei	= 0;
		m_iNumLines	= 0;
		return true;	
	}

	iArea	= iLen * m_iLineHei;
	iWid	= 4;
	iHei	= 4;

	for (i=0; i < 16; i++)
	{
		if ((1 << (iWid + iHei)) > iArea)
			break;

		iWid++;

		if ((1 << (iWid + iHei)) > iArea)
			break;

		iHei++;
	}

	if (i == 16)	//	Too big texture !
		return false;

	m_iTexWid	= 1 << iWid;
	m_iTexHei	= 1 << iHei;
	m_iNumLines = iLen / m_iTexWid + 1;

	//	Check again
	if (m_iNumLines * m_iLineHei > m_iTexHei)
		m_iTexHei <<= 1;

	pSize->cx = iMaxWid;
	pSize->cy = iNumString * m_iLineHei;

	return true;
}

/*	Fill text's texture.

	Return true for success, otherwise return false.

	pDIBData: address of DIB which contains text.
	iPitch: pitch of text image (in bytes)
*/
bool A3DStringSet::FillTexture(BYTE* pDIBData, int iPitch)
{
	int i, iTexPitch, iOff, iRemain, iLine, iLeftWid, iSrcTop;
	RECT rcSrc, rcDst;
	BYTE* pTexBuf;
	STRING* pString;

	if (!m_pA3DTexture->LockRect(NULL, (void**) &pTexBuf, &iTexPitch, 0))
		return false;
	
	iOff	= 0;
	iRemain	= m_iTexWid;
	iLine	= 0;
	iSrcTop	= 0;

	for (i=0; i < m_iNumString; i++)
	{
		pString = &m_aStrings[i];

		if (!pString->szStr)	//	Is an empty line
			continue;

		pString->iFirstOff	= iOff;
		pString->iFirstLine	= iLine;

		rcSrc.top	 = iSrcTop;
		rcSrc.bottom = rcSrc.top + m_iLineHei - 1;

		rcDst.top	 = m_iLineHei * iLine;
		rcDst.bottom = rcDst.top + m_iLineHei - 1;

		iSrcTop += m_iLineHei;

		//	Total string can be put in this texture line ?
		if (pString->iExtX <= iRemain)
		{
			rcSrc.left	= 0;
			rcSrc.right	= pString->iExtX - 1;

			rcDst.left	= iOff;
			rcDst.right	= iOff + pString->iExtX - 1;

			FillRect(pTexBuf, &rcDst, iTexPitch, pDIBData, &rcSrc, iPitch);

			iOff	+= pString->iExtX;
			iRemain -= pString->iExtX;

			pString->iLastLine	= iLine;
			pString->iLastOff	= iOff;

			continue;
		}

		//	String must be divided to two or more segments
		iLeftWid	 = pString->iExtX - iRemain;
		rcSrc.left	 = 0;
		rcSrc.right	 = iRemain - 1;

		rcDst.left	 = iOff;
		rcDst.right	 = m_iTexWid - 1;

		FillRect(pTexBuf, &rcDst, iTexPitch, pDIBData, &rcSrc, iPitch);

		//	The rest segment
		rcSrc.left	= iRemain;
		iOff		= 0;
		iRemain		= m_iTexWid;

		while (iLeftWid > m_iTexWid)
		{
			iLine++;

			iLeftWid   -= m_iTexWid;
			rcSrc.right	= rcSrc.left + m_iTexWid - 1;
		
			rcDst.top	+= m_iLineHei;
			rcDst.bottom = rcDst.top + m_iLineHei - 1;	
			rcDst.left	 = 0;
			rcDst.right	 = m_iTexWid - 1;

			FillRect(pTexBuf, &rcDst, iTexPitch, pDIBData, &rcSrc, iPitch);

			rcSrc.left += m_iTexWid;
		}

		iLine++;

		//	The last tail of current string
		if (iLeftWid)
		{
			rcSrc.right	 = rcSrc.left + iLeftWid - 1;
			rcDst.top	+= m_iLineHei;
			rcDst.bottom = rcDst.top + m_iLineHei - 1;
			rcDst.left	 = 0;
			rcDst.right	 = iLeftWid - 1;

			FillRect(pTexBuf, &rcDst, iTexPitch, pDIBData, &rcSrc, iPitch);

			iOff	= iLeftWid;
			iRemain	= m_iTexWid - iOff;

			pString->iLastLine	= iLine;
			pString->iLastOff	= iOff;
		}
		else
		{
			iOff	= 0;
			iRemain	= m_iTexWid;

			pString->iLastLine	= iLine - 1;
			pString->iLastOff	= m_iTexWid;
		}
	}

	if (!m_pA3DTexture->UnlockRect())
		return false;

	return true;
}

/*	Fill a rectangle area in texture.

	pDst: 16-bit texture data pointer
	pDstRect: destination rectangle
	iDstPitch: texture's pitch
	pSrc: 16-bit source image data
	pSrcRect: source rectangle
	iSrcPitch: source image's pitch
*/
void A3DStringSet::FillRect(BYTE* pDst, RECT* pDstRect, int iDstPitch, 
							BYTE* pSrc, RECT* pSrcRect, int iSrcPitch)
{
	WORD *pDstPixel;
	DWORD *pSrcPixel;
	int i, j, iWidth, iHeight;
	BYTE byAlpha;

	iWidth	= pDstRect->right - pDstRect->left + 1;
	iHeight	= pDstRect->bottom - pDstRect->top + 1;

	pDst = pDst + pDstRect->top * iDstPitch + pDstRect->left * 2;
	pSrc = pSrc + pSrcRect->top * iSrcPitch + pSrcRect->left * 4;

	for (i=0; i < iHeight; i++)
	{
		pDstPixel = (WORD*)pDst;
		pSrcPixel = (DWORD*)pSrc;

		for (j=0; j < iWidth; j++, pDstPixel++, pSrcPixel++)
		{
			byAlpha = (*((BYTE*)pSrcPixel) & 0xff) >> 4;
			if (byAlpha > 0)
				*pDstPixel = (byAlpha << 12) | 0x0fff;
			else
				*pDstPixel = 0;
		}

		pDst += iDstPitch;
		pSrc += iSrcPitch;
	}
}

/*	Create vertex and indices buffer

	Return true for success, otherwise return false
*/
bool A3DStringSet::CreateStream()
{
	//	Create vertex and index stream
	if (!m_pA3DStream)
	{
		if (!(m_pA3DStream = new A3DStream))
		{
			g_pA3DErrLog->ErrLog("A3DStringSet::CreateStream() not enough memory for stream!");
			return false;
		}
	}

	//	Calculate number of rectangle needed to render strings
	int i, n, m, iNumLine, iNumRect=0;

	for (i=0; i < m_iNumString; i++)
	{
		if (!m_aStrings[i].szStr)	//	Is an empty line ?
			continue;

		iNumLine = m_aStrings[i].iLastLine - m_aStrings[i].iFirstLine;
		if (!iNumLine)
			iNumRect += 1;
		else if (iNumLine == 1)
			iNumRect += 2;
		else
			iNumRect += iNumLine + 1;
	}

	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, iNumRect * 4, iNumRect * 6, 0))
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::CreateStream(), Failed to initialize stream!");
		return false;
	}

	//	Fill indices
	WORD* aIndices;

	if (!m_pA3DStream->LockIndexBuffer(0, 0, (BYTE**) &aIndices, 0))
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::CreateStream(), Failed to lock index buffer!");
		return false;
	}

	for (i=0, n=0, m=0; i < iNumRect; i++, n+=6, m+=4)
	{
		aIndices[n+0] = m + 0;
		aIndices[n+1] = m + 1;
		aIndices[n+2] = m + 2;
		aIndices[n+3] = m + 1;
		aIndices[n+4] = m + 3;
		aIndices[n+5] = m + 2;
	}

	if (!m_pA3DStream->UnlockIndexBuffer())
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::CreateStream(), Failed to unlock index buffer!");
		return false;
	}

	//	Fill vertices' texture coordinates
	A3DTLVERTEX* aVerts, *pVert;
	STRING* pString;
	float v, fTexLineHei;
	float fInvWSize = 1.0f / m_iTexWid;
	float fInvHSize = 1.0f / m_iTexHei;
	
	fTexLineHei = m_iLineHei * fInvHSize;

	if (!m_pA3DStream->LockVertexBuffer(0, 0, (BYTE**) &aVerts, 0))
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::CreateStream(), Failed to lock vertex buffer!");
		return false;
	}

	pVert = aVerts;

	for (i=0; i < m_iNumString; i++)
	{
		pString  = &m_aStrings[i];

		if (!pString->szStr)	//	Is an empty line ?
			continue;

		iNumLine = pString->iLastLine - pString->iFirstLine;

		if (!iNumLine)
		{
			//	Only one segment
			pVert->tu = pString->iFirstOff * fInvWSize;
			pVert->tv = v = (pString->iFirstLine * m_iLineHei) * fInvHSize;
			pVert++;

			pVert->tu = pString->iLastOff * fInvWSize;
			pVert->tv = v;
			pVert++;

			pVert->tu = pString->iFirstOff * fInvWSize;
			pVert->tv = v + fTexLineHei;
			pVert++;

			pVert->tu = pString->iLastOff * fInvWSize;
			pVert->tv = v + fTexLineHei;
			pVert++;

			continue;
		}
		
		//	First segment
		pVert->tu = pString->iFirstOff * fInvWSize;
		pVert->tv = v = (pString->iFirstLine * m_iLineHei) * fInvHSize;
		pVert++;

		pVert->tu = 1.0f;
		pVert->tv = v;
		pVert++;

		pVert->tu = pString->iFirstOff * fInvWSize;
		pVert->tv = v + fTexLineHei;
		pVert++;

		pVert->tu = 1.0f;
		pVert->tv = v + fTexLineHei;
		pVert++;

		m = pString->iFirstLine + 1;

		//	Middle segments
		for (n=0; n < iNumLine - 1; n++, m++)
		{
			pVert->tu = 0.0f;
			pVert->tv = v = (m * m_iLineHei) * fInvHSize;
			pVert++;

			pVert->tu = 1.0f;
			pVert->tv = v;
			pVert++;

			pVert->tu = 0.0f;
			pVert->tv = v + fTexLineHei;
			pVert++;

			pVert->tu = 1.0f;
			pVert->tv = v + fTexLineHei;
			pVert++;
		}

		//	The last segment
		pVert->tu = 0.0f;
		pVert->tv = v = (pString->iLastLine * m_iLineHei) * fInvHSize;
		pVert++;

		pVert->tu = pString->iLastOff * fInvWSize;
		pVert->tv = v;
		pVert++;

		pVert->tu = 0.0f;
		pVert->tv = v + fTexLineHei;
		pVert++;

		pVert->tu = pString->iLastOff * fInvWSize;
		pVert->tv = v + fTexLineHei;
		pVert++;
	}

	if (!m_pA3DStream->UnlockVertexBuffer())
	{
		g_pA3DErrLog->ErrLog("A3DStringSet::CreateStream(), Failed to unlock vertex buffer!");
		return false;
	}

	return true;
}

/*	Draw all strings.

	Return true for success, otherwise return false

	x: If all strings layout vertically, x means the first string's left-top corner x 
	   position on screen.
	   If all strings layout horizonally, x is a offset of all strings on x axis. 
	y: First string's left-top corner position on screen
	iHeight: output height of text. 0 means to use the m_iOutHei. This parameter
			 won't change the value of m_iOutHei
*/
bool A3DStringSet::OutputStrings(int x, int y, int iHeight/* 0 */)
{
	if (m_bHWIStringSet || !m_bReady || !m_iNumString)
	{
		//	No string exists or all strings are empty lines will go here
	//	g_pA3DErrLog->ErrLog("A3DStringSet::OutputStrings(), Need building before render");
		return true;
	}

	if (m_bFilter)
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	else
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);

	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	m_pA3DTexture->Appear(0);
	m_pA3DStream->Appear();

	A3DTLVERTEX* aVerts, *pVert;
	int i, j, iNumLine;
	float rx, ry, fWid, fHei;
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);
	STRING* pString;
	A3DCOLOR Color;

	//	Set vertices' position and color
	if (!m_pA3DStream->LockVertexBuffer(0, 0, (BYTE**) &aVerts, 0))
		return false;

	if (!iHeight)
		iHeight = m_iOutHei;

	pVert = aVerts;
	ry	  = (float)y;
	fHei  = (float)iHeight;

	for (i=0; i < m_iNumString; i++)
	{
		pString = &m_aStrings[i];

		if (!pString->szStr)	//	Is an empty line ?
		{
			ry += fHei + m_iLineSpace; 
			continue;
		}

		iNumLine = pString->iLastLine - pString->iFirstLine;
		Color	 = pString->dwCol;

		rx = (float)x + pString->iIntent;
	
		if (!iNumLine)	//	Only one line this string hold in texture
		{
			fWid = (float)pString->iExtX / m_iLineHei * iHeight;

			pVert[0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pVert[0].tu, pVert[0].tv);
			pVert[1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pVert[1].tu, pVert[1].tv);
			pVert[2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pVert[2].tu, pVert[2].tv);
			pVert[3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pVert[3].tu, pVert[3].tv);

			pVert += 4;

			if (m_bVertical)
				ry += fHei + m_iLineSpace;

			continue;
		}

		//	First segment
		fWid = ((float)m_iTexWid - pString->iFirstOff) / m_iLineHei * iHeight;

		pVert[0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pVert[0].tu, pVert[0].tv);
		pVert[1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pVert[1].tu, pVert[1].tv);
		pVert[2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pVert[2].tu, pVert[2].tv);
		pVert[3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pVert[3].tu, pVert[3].tv);

		pVert += 4;
		rx	  += fWid;

		//	Middle segments
		fWid = (float)m_iTexWid / m_iLineHei * iHeight;

		for (j=0; j < iNumLine - 1; j++)
		{
			pVert[0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pVert[0].tu, pVert[0].tv);
			pVert[1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pVert[1].tu, pVert[1].tv);
			pVert[2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pVert[2].tu, pVert[2].tv);
			pVert[3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pVert[3].tu, pVert[3].tv);

			pVert	+= 4;
			rx		+= fWid;
		}

		//	Last segment
		fWid = (float)pString->iLastOff / m_iLineHei * iHeight;

		pVert[0] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pVert[0].tu, pVert[0].tv);
		pVert[1] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry-0.5f, 0.0f, 1.0f), Color, Specular, pVert[1].tu, pVert[1].tv);
		pVert[2] = A3DTLVERTEX(A3DVECTOR4(rx-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pVert[2].tu, pVert[2].tv);
		pVert[3] = A3DTLVERTEX(A3DVECTOR4(rx+fWid-0.5f, ry+fHei-0.5f, 0.0f, 1.0f), Color, Specular, pVert[3].tu, pVert[3].tv);

		pVert += 4;

		if (m_bVertical)
			ry += fHei + m_iLineSpace; 
	}

	if (!m_pA3DStream->UnlockVertexBuffer())
		return false;

	//	Render
	if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_pA3DStream->GetVertCount(),
											0, m_pA3DStream->GetVertCount() >> 1))
		return false;

	m_pA3DTexture->Disappear(0);
	//	Restore render state
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

	return true;
}


