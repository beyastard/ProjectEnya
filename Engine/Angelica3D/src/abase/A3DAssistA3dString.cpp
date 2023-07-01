// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistA3dString.cpp
// Creator: Wei Hua (魏华)
// 为A3dString做的缓存

#include "A3DAssistA3dString.h"

A3DAssistA3dString::A3DAssistA3dString()
{
	m_pA3DString	= NULL;
	m_pA3DEngine	= NULL;
	m_hFont			= NULL;
}

A3DAssistA3dString::~A3DAssistA3dString()
{
}

bool A3DAssistA3dString::Init(A3DEngine *pA3DEngine, HA3DFONT hFont, int nTotal, int nStrLen)
{
	bool bval;

	m_pA3DEngine	= pA3DEngine;
	m_hFont			= hFont;
	
	bval			= A3DAssistCache_String::Init(nTotal, nStrLen);
	if( !bval )
	{
		return	bval;
	}

	m_pA3DString	= new A3DString [nTotal];
	if( !m_pA3DString )
	{
		return	false;
	}

	return	true;
}

bool	A3DAssistA3dString::Release()
{
	ResetAll();
	if( m_pA3DString )
	{
		delete []	m_pA3DString;
		m_pA3DString	= NULL;
	}
	A3DAssistCache_String::Release();
	return	true;
}

bool	A3DAssistA3dString::ResetAll()
{
	int	i;
	for(i=0;i<m_nTotal;i++)
	{
		if( m_pUnits[i].m_bTaken )
		{
			m_pA3DString[i].Release();
		}
	}
	A3DAssistCache_String::ResetAll();
	return	true;
}

int		A3DAssistA3dString::CheckAndSetString(char *szText)
{
	int		nIdx;
	nIdx	= SearchString(szText);
	if( nIdx<0 )
	{
		nIdx	= GetMEUU();
		if( RegisterString(nIdx, szText)<0 )
		{
			return	-1;
		}
		// 为了保险先要释放一次
		m_pA3DString[nIdx].Release();
		m_pA3DString[nIdx].Init(m_pA3DEngine->GetA3DDevice(), szText, m_hFont, true);
	}
	return	nIdx;
}

bool	A3DAssistA3dString::TextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade, int nHeight)
{
	int		nIdx;
	nIdx	= CheckAndSetString(szText);
	if( nIdx<0 )
	{
		return	false;
	}
	if( bShade )
	{
		m_pA3DString[nIdx].DrawText(nX+1, nY+1, A3DCOLORRGB(0,0,0), nHeight);
	}
	SetLastUsedTick(nIdx);
	return	m_pA3DString[nIdx].DrawText(nX, nY, color, nHeight);
}

bool	A3DAssistA3dString::TextOut3D(A3DVECTOR3 vecPos, char *szText, A3DCOLOR color, float f3DRatio)
{
	int		nIdx;
	nIdx	= CheckAndSetString(szText);
	if( nIdx<0 )
	{
		return	false;
	}
	if( f3DRatio==0 )
	{
		f3DRatio	= 0.5f;
	}
	SetLastUsedTick(nIdx);
	return	m_pA3DString[nIdx].Draw3DText(m_pA3DEngine->GetActiveViewport(), vecPos, f3DRatio, color);
}
