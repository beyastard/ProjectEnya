// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistA3dStringSet.cpp
// Creator: Wei Hua (魏华)
// 为A3dString做的缓存

#include "A3DAssistA3dStringSet.h"

////////////////////////////////////////////////////////////////////
// ColorBinStr
////////////////////////////////////////////////////////////////////
ColorBinStr::ColorBinStr()
{
	memset(m_szBinStr, 0, sizeof(m_szBinStr));
	m_nOffset	= 0;
}

ColorBinStr::~ColorBinStr()
{
}

bool	ColorBinStr::SetCpy(char *szText)
{
	BinStrCpy(m_szBinStr, sizeof(m_szBinStr), szText);
	return	true;
}

void	ColorBinStr::BeginWrite()
{
	memset(m_szBinStr, 0, sizeof(m_szBinStr));
}

void	ColorBinStr::BeginRead()
{
	m_nOffset	= 0;
}

bool	ColorBinStr::Write(int nDX, A3DCOLOR color, char *szText)
{
	BinStrCatBin(m_szBinStr, sizeof(m_szBinStr), &nDX, sizeof(nDX));
	BinStrCatBin(m_szBinStr, sizeof(m_szBinStr), &color, sizeof(color));
	BinStrCatStr(m_szBinStr, sizeof(m_szBinStr), szText);
	return	true;
}

bool	ColorBinStr::Read(int *nDX, A3DCOLOR *color, char *szText)
{
	int	ds, ts;
	BinStrGetSize(m_szBinStr, &ds, &ts);
	if( m_nOffset >= ds )
	{
		return	false;
	}
	m_nOffset	= BinStrReadBin(m_szBinStr, m_nOffset, sizeof(*nDX), nDX);
	m_nOffset	= BinStrReadBin(m_szBinStr, m_nOffset, sizeof(*color), color);
	m_nOffset	= BinStrReadStr(m_szBinStr, m_nOffset, szText);
	return	true;
}

////////////////////////////////////////////////////////////////////
// A3DAssistA3dStringSet
////////////////////////////////////////////////////////////////////
A3DAssistA3dStringSet::A3DAssistA3dStringSet()
{
	m_pA3DStringSet	= NULL;
	m_pA3DEngine	= NULL;
	m_hFont			= NULL;
	memset(m_color, 0, sizeof(m_color));
}

A3DAssistA3dStringSet::~A3DAssistA3dStringSet()
{
}

bool	A3DAssistA3dStringSet::RegisterHtmlColor(int nColor, A3DCOLOR color)
{
	if( nColor<0 || nColor>=MAX_AAASS_HTML_COLOR )
	{
		return	false;
	}
	m_color[nColor]		= color;
	return	true;
}

bool	A3DAssistA3dStringSet::Init(A3DEngine *pA3DEngine, HA3DFONT hFont, int nTotal, int nStrLen)
{
	bool			bval;

	m_pA3DEngine	= pA3DEngine;
	m_hFont			= hFont;
	
	bval			= A3DAssistCache_BinString::Init(nTotal, nStrLen);
	if( !bval )
	{
		return	bval;
	}

	m_pA3DStringSet	= new A3DStringSet [nTotal];
	if( !m_pA3DStringSet )
	{
		return	false;
	}
	int				i;
	for(i=0;i<m_nTotal;i++)
	{
		m_pA3DStringSet[i].Init(m_pA3DEngine->GetA3DDevice(), m_hFont, true, false);
	}

	return	true;
}

bool	A3DAssistA3dStringSet::Release()
{
	int	i;
	for(i=0;i<m_nTotal;i++)
	{
		m_pA3DStringSet[i].Release();
	}
	if( m_pA3DStringSet )
	{
		delete []	m_pA3DStringSet;
		m_pA3DStringSet	= NULL;
	}
	A3DAssistCache_BinString::Release();
	return	true;
}

bool	A3DAssistA3dStringSet::ResetAll()
{
	// 因为只初始化一次，所以不能在这里Release
	A3DAssistCache_BinString::ResetAll();
	return	true;
}

int		A3DAssistA3dStringSet::CheckAndSetString(char *szText)
{
	int		nIdx;
	bool	bval;
	int		nDX, nX;
	A3DCOLOR	color;
	char	szStr[MAX_COLORBINSTR_LEN];
	A3DStringSet	*pAAS;

	nIdx	= SearchString(szText);
	nX		= 0;
	if( nIdx<0 )
	{
		nIdx	= GetMEUU();
		if( RegisterString(nIdx, szText)<0 )
		{
			return	-1;
		}
		pAAS	= &m_pA3DStringSet[nIdx];
		bval	= pAAS->Reset();
		m_cbs.SetCpy(szText);
		m_cbs.BeginRead();
		while(m_cbs.Read(&nDX, &color, szStr))
		{
			nX	+= nDX;
			pAAS->AddString(szStr, color, nX);
		}
		bval	= pAAS->Build();
	}
	return	nIdx;
}

bool	A3DAssistA3dStringSet::HtmlTextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade, int nHeight)
{
	return	true;
}

bool	A3DAssistA3dStringSet::TextOut_Begin()
{
	m_cbs.BeginWrite();
	return	true;
}

bool	A3DAssistA3dStringSet::TextOut_Add(int nDX, A3DCOLOR color, char *szText)
{
	return	m_cbs.Write(nDX, color, szText);
}

bool	A3DAssistA3dStringSet::TextOut_End(int nX, int nY, bool bShade)
{
	int		nIdx;
	nIdx	= CheckAndSetString(m_cbs.GetStr());
	if( nIdx<0 )
	{
		return	false;
	}
	A3DStringSet	*pASS;
	pASS	= &m_pA3DStringSet[nIdx];
	if( bShade )
	{
		pASS->SaveColors();
		pASS->SetColor(-1, A3DCOLORRGB(0,0,0));
		pASS->OutputStrings(nX+1, nY+1);
		pASS->RestoreColors();
	}
	pASS->OutputStrings(nX, nY);
	SetLastUsedTick(nIdx);
	return	true;
}
