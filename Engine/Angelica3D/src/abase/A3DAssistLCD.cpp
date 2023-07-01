// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistLCD.cpp
// Creator: Wei Hua (魏华)
// 输出LCD字母字体

#include "A3DAssistLCD.h"
#include "APlatform.h"

A3DAssistLCD::A3DAssistLCD()
{
	m_pA3DEngine	= NULL;
	m_fRatio		= 0;
	m_pCharGraph	= NULL;
	m_nUnitW		= 0;
	m_nUnitH		= 0;
	m_nSep			= 0;
	memset(m_nCharMapper, 0, sizeof(m_nCharMapper));
}
A3DAssistLCD::~A3DAssistLCD()
{
}
bool	A3DAssistLCD::Init(A3DEngine *pA3DEngine
	, char *pszChars, char *pszCharGraphFile
	, int nRow, int nCol
	, int nUnitW, int nUnitH
	)
{
	int		nNum, i, r, c;
	bool	bval;

	m_pA3DEngine	= pA3DEngine;

	nNum			= strlen(pszChars);
	if( nNum>256 )
	{
		assert(0);
		return	false;
	}
	for(i=0;i<nNum;i++)
	{
		m_nCharMapper[pszChars[i]]	= i;
	}

	A3DRECT	*pRects;
	pRects	= new A3DRECT[nNum];
	for(i=0;i<nNum;i++)
	{
		r	= i / nCol;
		c	= i % nCol;
		pRects[i].left		= c * nUnitW;
		pRects[i].right		= (c+1) * nUnitW;// -1;
		pRects[i].top		= r * nUnitH;
		pRects[i].bottom	= (r+1) * nUnitH;// -1;
	}

	m_pCharGraph	= new A2DSprite;
	if( !m_pCharGraph )
	{
		return	false;
	}
	bval	= m_pCharGraph->Init(
		m_pA3DEngine->GetA3DDevice()
		, pszCharGraphFile
		, nUnitW * nCol
		, nUnitH * nRow
		, 0
		, nNum
		, pRects);
	if( !bval )
	{
		delete	m_pCharGraph;
		m_pCharGraph	= NULL;
	}

	m_nUnitW		= nUnitW;
	m_nUnitH		= nUnitH;

	return	true;
}
bool	A3DAssistLCD::Release()
{
	if( m_pCharGraph )
	{
		m_pCharGraph->Release();
		delete	m_pCharGraph;
		m_pCharGraph	= NULL;
	}
	return	true;
}
bool	A3DAssistLCD::TextOut(int nX, int nY, char *szText, A3DCOLOR color)
{
	int		i;
	int		nSpaceW;
	bool	bval;

	nSpaceW	= m_nUnitW + m_nSep;
	if( m_fRatio!=0 )
	{
		nSpaceW	= int(nSpaceW * m_fRatio+0.9f);
	}
	m_pCharGraph->SetColor(color);
	for(i=0;i<(int)strlen(szText);i++)
	{
		if( szText[i]!=' ' )
		{
			bval	= m_pCharGraph->SetCurrentItem(m_nCharMapper[szText[i]]);
			bval	= m_pCharGraph->DrawToBack(nX, nY);
		}
		nX	+= nSpaceW;
	}

	return	true;
}
bool	A3DAssistLCD::GetTextExtent(char *szText, int *pnW, int *pnH)
{
	*pnH	= m_nUnitH;
	*pnW	= (m_nUnitW + m_nSep) * strlen(szText);
	if( m_fRatio!=0 )
	{
		*pnH	= int(*pnH * m_fRatio + 0.9f);
		*pnW	= int(*pnW * m_fRatio + 0.9f);
	}
	return	true;
}
void	A3DAssistLCD::SetRatio(float fRatio)
{
	m_fRatio = fRatio;
	if( m_fRatio==0 )
	{
		m_pCharGraph->SetScaleX(1.f);
		m_pCharGraph->SetScaleY(1.f);
	}
	else
	{
		m_pCharGraph->SetScaleX(m_fRatio);
		m_pCharGraph->SetScaleY(m_fRatio);
	}
}
