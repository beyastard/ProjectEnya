/*
 * FILE: A3DCursor.h
 *
 * DESCRIPTION: A class manipulate the cursor
 *
 * CREATED BY: Hedi, 2002/5/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCURSOR_H_
#define _A3DCURSOR_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DObject.h"
#include "A3DDevice.h"
#include "A2DSprite.h"

class A3DCursor : public A2DSprite
{
protected:
	char			m_szCursorFile[MAX_PATH];

	A3DPOINT2		m_ptHotSpot;
	A3DPOINT2		m_ptCursorPos;

	int				m_nFrameCount;
	int				m_nFrame;
public:
	A3DCursor();
	~A3DCursor();

	bool Init(A3DDevice * pDevice, char * szCursorFile, int nWidth, int nHeight, A3DCOLOR colorKey, int nFrameCount, A3DRECT * pRects, A3DPOINT2& ptHotSpot);
	bool Release();

	// Update the cursor pos information and make the cursor animated as possible;
	bool TickAnimation();

	bool SetPos(A3DPOINT2& ptNewPos);
	bool SetHotSpot(A3DPOINT2& ptHotSpot);

	bool Draw();
};

#endif//_A3DCURSOR_H_
