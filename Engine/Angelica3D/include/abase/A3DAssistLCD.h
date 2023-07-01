// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistLCD.h
// Creator: Wei Hua (魏华)
// 输出LCD字母字体

#ifndef	__A3DASSISTLCD_H__
#define	__A3DASSISTLCD_H__

#include <a3d.h>

class	A3DAssistLCD
{
protected:
	A3DEngine		* m_pA3DEngine;
	float			m_fRatio;
	A2DSprite		* m_pCharGraph;
	unsigned char	m_nCharMapper[256];
	int				m_nUnitW, m_nUnitH;
	int				m_nSep;
public:
	inline void	SetSep(int nSep)
	{ m_nSep = nSep; }
public:
	A3DAssistLCD();
	~A3DAssistLCD();
	bool	Init(A3DEngine *pA3DEngine
		, char *pszChars, char *pszCharGraphFile
		, int nRow, int nCol
		, int nUnitW, int nUnitH
		);
	bool	Release();
	bool	TextOut(int nX, int nY, char *szText, A3DCOLOR color);
	bool	GetTextExtent(char *szText, int *pnW, int *pnH);
	void	SetRatio(float fRatio = 0);
};

#endif