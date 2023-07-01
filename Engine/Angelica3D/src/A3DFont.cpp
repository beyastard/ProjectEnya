#include "A3DFont.h"
#include "A3DErrLog.h"
#include "A3DMacros.h"
#include "A3DConfig.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define MAX_STRINGLEN		256			//	Maximum string length

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

A3DFont::A3DFont()
{
	m_pA3DStream = NULL;
	m_pA3DDevice = NULL;
	m_iOutHei	 = 16;
	m_hA3DFont	 = 0;

	m_bHWIFont	 = false;
}

A3DFont::~A3DFont()
{
}

/*	Initialize object.

	Return true for success, otherwise return false

	pDevice: address of A3D device.
	hA3DFont: A3D font handle got from A3DFontMan
	bFilter: true, use antiliase font, 
*/	
bool A3DFont::Init(A3DDevice* pDevice, HA3DFONT hA3DFont, bool bFilter)
{
	if (!pDevice || !hA3DFont)
		return false;

	HBITMAP	hBitmap, hOldBmp;	//	Memory bitmap's handle
	HDC	hDC;					//	Memory DC's handle
	HFONT hFont, hOldFont;		//	Font's handle
	DWORD* pDIBData;			//	Address of memory bitmap's data

	m_pA3DDevice = pDevice;
	m_bFilter	 = bFilter;
	m_iType		 = FONT_TRUETYPE;
	m_hA3DFont	 = hA3DFont;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWIFont = true;
		return true;
	}

	//	Get windows's DC and font from A3DFontMan
	A3DFontMan* pFontMan = pDevice->GetA3DEngine()->GetA3DFontMan();
	m_iOutHei = pFontMan->GetFontOutputHeight(hA3DFont);

    //	Establish the font and texture size
	m_fTextScale = 1.0f;

	int iHeight	= pFontMan->GetFontOutputHeight(hA3DFont);

	//	Large fonts need larger textures
	if (iHeight > 40)
		m_nMapWidth = m_nMapHeight = 1024;
	else if (iHeight > 20)
		m_nMapWidth = m_nMapHeight = 512;
	else
		m_nMapWidth = m_nMapHeight = 256;

	D3DCAPS8 d3dCaps;
	d3dCaps = pDevice->GetD3DCaps();
	if (m_nMapWidth > (int)d3dCaps.MaxTextureWidth)
	{
		m_fTextScale = (float)d3dCaps.MaxTextureWidth / (float)m_nMapWidth;
		m_nMapWidth  = m_nMapHeight = d3dCaps.MaxTextureWidth;
		
		//	Register a new font
		iHeight  = (int)(iHeight * m_fTextScale);
		hA3DFont = pFontMan->RegisterFont(hA3DFont, iHeight);
	}

	hDC		 = pFontMan->GetWindowsDC();
	hFont	 = pFontMan->GetWindowsFont(hA3DFont);
	hOldFont = (HFONT)SelectObject(hDC, hFont);
		
	//	Create texture
	if (!m_Texture.Create(pDevice, m_nMapWidth, m_nMapHeight, A3DFMT_A4R4G4B4))
	{
		g_pA3DErrLog->ErrLog("A3DFont::Init failed to create new texture.");
		Release();
		return false;
	}

	//	Create a memory bitmap to store text
	BITMAPINFO bmi;
	memset(&bmi.bmiHeader, 0, sizeof (BITMAPINFOHEADER));
	bmi.bmiHeader.biSize        = sizeof (BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = m_nMapWidth;
	bmi.bmiHeader.biHeight      = -m_nMapHeight;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**) &pDIBData, NULL, 0);
	hOldBmp	= (HBITMAP)SelectObject(hDC, hBitmap);

	int iPitch = ((m_nMapWidth * 4 + 3) >> 2) << 2;

	//	Set text properties
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0);
	SetTextAlign(hDC, TA_TOP);

	//	Select font into screen DC so that anti-aliased can do effect
	HDC hScreenDC = GetDC(NULL);
	HFONT hOldSFont = (HFONT)SelectObject(hScreenDC, hFont);

	//	Loop through all printable character and output them to the bitmap..
	//	Meanwhile, keep track of the corresponding tex coords for each character.
	int x=0, y=0;
	char szStr[2] = {0, 0};
	SIZE Size;

	for (char c=32; c < 127; c++)
	{
		szStr[0] = c;
		GetTextExtentPoint32(hDC, szStr, 1, &Size);
		
		if (x + Size.cx + 2 > m_nMapWidth)
		{
			x  = 0;
			y += Size.cy + 1;
		}

		ExtTextOut(hDC, x, y, ETO_OPAQUE, NULL, szStr, 1, NULL);
		
		m_aCharSet[c-32].aTexCoords[0] = (float)x / m_nMapWidth;
		m_aCharSet[c-32].aTexCoords[1] = (float)y / m_nMapHeight;
		m_aCharSet[c-32].aTexCoords[2] = (float)(x + Size.cx) / m_nMapWidth;
		m_aCharSet[c-32].aTexCoords[3] = (float)(y + Size.cy) / m_nMapHeight;
		
		x += Size.cx + 2;
	}

	SelectObject(hScreenDC, hOldSFont);
	ReleaseDC(NULL, hScreenDC);

    //	Lock the surface and write the alpha values for the set pixels
	BYTE* pDest, *pSrc = (BYTE*)pDIBData;
	int iTexPitch;
	if (!m_Texture.LockRect(NULL, (void**) &pDest, &iTexPitch, 0))
	{
		g_pA3DErrLog->ErrLog("A3DFont::Init failed to lock texture.");
		Release();
		return false;
	}

	WORD* pDestPixel;
	DWORD* pSrcPixel;
	BYTE byAlpha;

	for (y=0; y < m_nMapHeight; y++)
	{
		pDestPixel	= (WORD*)pDest;
		pSrcPixel	= (DWORD*)pSrc;

		for (x=0; x < m_nMapWidth; x++, pDestPixel++, pSrcPixel++)
		{
			byAlpha = (BYTE)((*pSrcPixel & 0xff) >> 4);
			if (byAlpha > 0)
				*pDestPixel = (byAlpha << 12) | 0x0fff;
			else
				*pDestPixel = 0x0000;
		}

		pDest += iTexPitch;
		pSrc  += iPitch;
    }

	if (!m_Texture.UnlockRect())
	{
		g_pA3DErrLog->ErrLog("A3DFont::Init failed to unlock texture.");
		Release();
		return false;
	}

	//	Done updating texture, so clean up used objects
	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hOldBmp);
	DeleteObject(hBitmap);

	//	Create vertices and indices buffer
	if (!CreateStream())
		return false;

	return true;
}

bool A3DFont::Load(A3DDevice * pDevice, char * szFontMap, int chWidth, int chHeight, int cellWidth, int cellHeight)
{
	m_pA3DDevice = pDevice;

	sprintf(m_szFontMap, "Textures\\Font\\%s", szFontMap);

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWIFont = true;
		return true;
	}

	if( !m_Texture.Init(pDevice) )
		return false;

	m_Texture.SetDesciredMipLevel(1);

	if( !m_Texture.LoadFromFile(pDevice, m_szFontMap, m_szFontMap) )
		return false;

	m_nMapWidth		= 128;	//	Should get from the texture;
	m_nMapHeight	= 256;	//	Should get from the texture;

	m_nCharWidth	= chWidth;
	m_nCharHeight	= chHeight;

	m_nCellWidth	= cellWidth;
	m_nCellHeight	= cellHeight;

	m_fCellWidth	= m_nCellWidth * 1.0f / m_nMapWidth;
	m_fCellHeight	= m_nCellHeight * 1.0f / m_nMapHeight;

	m_iType	= FONT_MAPFILE;

	if (!CreateStream())
		return false;

	return true;
}

/*	Create vertices and indices stream

	Return true for success, otherwise return false.
*/
bool A3DFont::CreateStream()
{
	if (!(m_pA3DStream = new A3DStream()))
	{
		g_pA3DErrLog->ErrLog("A3DFont::CreateStream Not Enough Memory!");
		return false;
	}

	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, MAX_STRINGLEN * 4, MAX_STRINGLEN * 6, 0))
	{
		g_pA3DErrLog->ErrLog("A3DFont::CreateStream m_pA3DStream Init Fail!");
		return false;
	}

	WORD* pVBIndices;
	if (!m_pA3DStream->LockIndexBuffer(0, 0, (LPBYTE*) &pVBIndices, NULL))
	{
		g_pA3DErrLog->ErrLog("A3DFont::CreateStream m_pA3DStream->LockIndexBuffer Fail!");
		return false;
	}

	for (int n=0; n < MAX_STRINGLEN; n++)
	{
		pVBIndices[n * 6] = n * 4;
		pVBIndices[n * 6 + 1] = n * 4 + 1;
		pVBIndices[n * 6 + 2] = n * 4 + 2;
		pVBIndices[n * 6 + 3] = n * 4 + 2;
		pVBIndices[n * 6 + 4] = n * 4 + 1;
		pVBIndices[n * 6 + 5] = n * 4 + 3;
	}

	if (!m_pA3DStream->UnlockIndexBuffer())
	{
		g_pA3DErrLog->ErrLog("A3DFont::CreateStream m_pA3DStream->UnlockIndexBuffer Fail!");
		return false;
	}

	return true;
}

/*	Get a string's extent.

	szText: text string.
	piWid (out): used to receive string's width, can be NULL
	piHei (out): used to receive string's height, can be NULL
*/
void A3DFont::GetTextExtent(char* szText, int* piWid, int* piHei)
{
	if (!m_hA3DFont || !szText)
		return;

	A3DFontMan* pFontMan = m_pA3DDevice->GetA3DEngine()->GetA3DFontMan();
	pFontMan->GetTextExtent(m_hA3DFont, szText, strlen(szText), piWid, piHei);
}

/*	Textout text using map file font.

	x, y: text string's left-up corner in screen coordinates
	szText: text string will be output
	color: text's color
	iHeight: output height of text. 0 means to use m_iOutHei. This parameter won't
			 change the value of m_iOutHei.
*/	
bool A3DFont::TextOut(int x, int y, char * szText, A3DCOLOR color, int iHeight/* 0 */)
{
	if( m_bHWIFont )	return true;

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetZTestEnable(false);

	m_Texture.Appear(0);
	m_pA3DStream->Appear();

	bool bval;

	if (m_iType == FONT_MAPFILE)
		bval = MapFileTextOut(x, y, szText, color);
	else	//	m_iType == FONT_TRUETYPE
	{
		if (m_bFilter)
			m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
		else
			m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);

		bval = TrueTypeTextOut(x, y, szText, color, iHeight);
	}

	m_Texture.Disappear(0);

	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetZTestEnable(true);

	return true;
}

bool A3DFont::Release()
{
	if( m_pA3DStream )
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}

	if( !m_bHWIFont )
		m_Texture.Release();
	return true;
}

/*	Textout text using map file font.

	x, y: text string's left-up corner in screen coordinates
	szTextOrg: text string will be output
	color: text's color
*/	
bool A3DFont::MapFileTextOut(int x, int y, char* szTextOrg, A3DCOLOR color)
{
	if( m_bHWIFont )	return true;

	bool bval;
	A3DTLVERTEX* pVBVerts;
	char szText[MAX_STRINGLEN + 1];
	int iStrLen;

	szText[MAX_STRINGLEN] = '\0';
	strncpy(szText, szTextOrg, MAX_STRINGLEN);
	iStrLen = strlen(szText);

	if (!iStrLen)
		return true;

	bval = m_pA3DStream->LockVertexBuffer(0, 0, (LPBYTE *)&pVBVerts, NULL);
	if( !bval ) return false;

	for(int n=0; n < iStrLen; n++)
	{
		if( szText[n] == ' ' )
			szText[n] = '~' + 1;

		char ch = szText[n] - '!';

		//Left up corner [0];
		pVBVerts[n * 4].x = x * 1.0f + n * m_nCharWidth;
		pVBVerts[n * 4].y = y * 1.0f;
		pVBVerts[n * 4].z = 0.0f;
		pVBVerts[n * 4].rhw = 1.0f;
		pVBVerts[n * 4].tu = (ch % 10) * m_fCellWidth;
		pVBVerts[n * 4].tv = (ch / 10) * m_fCellHeight;
		pVBVerts[n * 4].diffuse = color;
		pVBVerts[n * 4].specular = A3DCOLORRGBA(0, 0, 0, 255);

		//Right up corner [1];
		pVBVerts[n * 4 + 1] = pVBVerts[n * 4];
		pVBVerts[n * 4 + 1].x  += m_nCharWidth;
		pVBVerts[n * 4 + 1].tu += m_fCellWidth;
		
		//Left bottom corner [2];
		pVBVerts[n * 4 + 2] = pVBVerts[n * 4];
		pVBVerts[n * 4 + 2].y  += m_nCharHeight;
		pVBVerts[n * 4 + 2].tv += m_fCellHeight;

		//Right bottom corner [3];
		pVBVerts[n * 4 + 3] = pVBVerts[n * 4];
		pVBVerts[n * 4 + 3].x  += m_nCharWidth;
		pVBVerts[n * 4 + 3].tu += m_fCellWidth;
		pVBVerts[n * 4 + 3].y  += m_nCharHeight;
		pVBVerts[n * 4 + 3].tv += m_fCellHeight;
	}

	bval = m_pA3DStream->UnlockVertexBuffer();
	if( !bval ) return false;

	bval = m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, iStrLen * 4, 0, iStrLen * 2);
	if( !bval ) return false;

	return true;
}

/*	Textout text using truetype font.

	x, y: text string's left-up corner in screen coordinates
	szTextOrg: text string will be output
	color: text's color
	iHeight: output height of text. 0 means to use m_iOutHei. This parameter won't
			 change the value of m_iOutHei.
*/
bool A3DFont::TrueTypeTextOut(int x, int y, char *szTextOrg, A3DCOLOR color, int iHeight)
{
	if( m_bHWIFont )	return true;

	char c, szText[MAX_STRINGLEN + 1];
	int iStrLen, n;
	float sx=(float)x, sy=(float)y;

	memset(szText, 0, MAX_STRINGLEN + 1);
	szText[MAX_STRINGLEN] = '\0';
	strncpy(szText, szTextOrg, MAX_STRINGLEN);
	iStrLen = strlen(szText);

	if (!iStrLen)
		return true;

	A3DTLVERTEX* pVerts;
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	//	Fill vertex buffer
	if (!m_pA3DStream->LockVertexBuffer(0, 0, (BYTE**) &pVerts, 0))
		return false;

	float fWidScale	= m_nMapWidth / m_fTextScale;
	float fHeiScale = m_nMapHeight / m_fTextScale;

	n = 0;
	
	while (c = szText[n++])
	{
		if (c < ' ')
			continue;

		c -= 32;

		float tx1 = m_aCharSet[c].aTexCoords[0];
		float ty1 = m_aCharSet[c].aTexCoords[1];
		float tx2 = m_aCharSet[c].aTexCoords[2];
		float ty2 = m_aCharSet[c].aTexCoords[3];

		float w = (tx2 - tx1) * fWidScale;
		float h = (ty2 - ty1) * fHeiScale;

		if (iHeight)
		{
			w = w / h * iHeight;
			h = (float)iHeight;
		}
		else
		{
			w = w / h * m_iOutHei;
			h = (float)m_iOutHei;
		}

		*pVerts++ = A3DTLVERTEX(A3DVECTOR4(sx-0.5f, sy-0.5f, 0.0f, 1.0f), color, Specular, tx1, ty1);
		*pVerts++ = A3DTLVERTEX(A3DVECTOR4(sx+w-0.5f, sy-0.5f, 0.0f, 1.0f), color, Specular, tx2, ty1);
		*pVerts++ = A3DTLVERTEX(A3DVECTOR4(sx-0.5f, sy+h-0.5f, 0.0f, 1.0f), color, Specular, tx1, ty2);
		*pVerts++ = A3DTLVERTEX(A3DVECTOR4(sx+w-0.5f, sy+h-0.5f, 0.0f, 1.0f), color, Specular, tx2, ty2);

		sx += w;
    }

    //	Unlock and render the vertex buffer
	if (!m_pA3DStream->UnlockVertexBuffer())
		return false;

	if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, iStrLen * 4, 0, iStrLen * 2))
		return false;

	return true;
}

