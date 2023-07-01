// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistA3dString.h
// Creator: Wei Hua (魏华)
// 为A3dString做的缓存

#ifndef	__A3DASSISTA3DSTRING_H__
#define	__A3DASSISTA3DSTRING_H__

#include "A3DAssistCache.h"
#include <a3d.h>

class	A3DAssistA3dString	: public A3DAssistCache_String
{
protected:
	A3DString	* m_pA3DString;
	A3DEngine	* m_pA3DEngine;
	HA3DFONT	m_hFont;
protected:
	int		CheckAndSetString(char *szText);
public:
	A3DAssistA3dString();
	~A3DAssistA3dString();
	bool	Init(A3DEngine *pA3DEngine, HA3DFONT hFont, int nTotal, int nStrLen);
	bool	Release();
	bool	ResetAll();
	bool	TextOut(int nX, int nY, char *szText, A3DCOLOR color, bool bShade, int nHeight = 0);
	bool	TextOut3D(A3DVECTOR3 vecPos, char *szText, A3DCOLOR color, float f3DRatio = 0);
};

#endif