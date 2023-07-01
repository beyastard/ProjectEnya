// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistA3dStringSet.h
// Creator: Wei Hua (魏华)
// 为A3dString做的缓存

#ifndef	__A3DASSISTA3DSTRINGSET_H__
#define	__A3DASSISTA3DSTRINGSET_H__

#include "A3DAssistCache.h"
#include "ABinString.h"
#include <a3d.h>

#define	MAX_COLORBINSTR_LEN		1024
class	ColorBinStr
{
private:
	char	m_szBinStr[MAX_COLORBINSTR_LEN];
	int		m_nOffset;
public:
	inline char *	GetStr()
	{ return m_szBinStr; }
public:
	ColorBinStr();
	~ColorBinStr();
	bool	SetCpy(char *szText);
	void	BeginWrite();
	void	BeginRead();
	bool	Write(int nDX, A3DCOLOR color, char *szText);
	bool	Read(int *nDX, A3DCOLOR *color, char *szText);
};

#define	MAX_AAASS_HTML_COLOR	64

class	A3DAssistA3dStringSet	: public A3DAssistCache_BinString
{
protected:
	A3DStringSet	* m_pA3DStringSet;
	A3DEngine		* m_pA3DEngine;
	HA3DFONT		m_hFont;
	A3DCOLOR		m_color[MAX_AAASS_HTML_COLOR];
	ColorBinStr		m_cbs;
protected:
	int		CheckAndSetString(char *szText);
public:
	A3DAssistA3dStringSet();
	~A3DAssistA3dStringSet();
	bool	RegisterHtmlColor(int nColor, A3DCOLOR color);
	bool	Init(A3DEngine *pA3DEngine, HA3DFONT hFont, int nTotal, int nStrLen);
	bool	Release();
	bool	ResetAll();
	bool	HtmlTextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade, int nHeight = 0);
	bool	TextOut_Begin();
	bool	TextOut_Add(int nDX, A3DCOLOR color, char *szText);
	bool	TextOut_End(int nX, int nY, bool bShade);
};

#endif