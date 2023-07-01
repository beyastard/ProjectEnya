/*
 * FILE: A2DSpriteItem.h
 *
 * DESCRIPTION: Sprite Item class
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A2DSPRITEITEM_H_
#define _A2DSPRITEITEM_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DDevice.h"
#include "AList.h"
#include "A3DTexture.h"
#include "A3DStream.h"
	   
typedef class A2DSprite * PA2DSprite;
typedef class A2DSpriteBuffer * PA2DSpriteBuffer;

class A2DSpriteItem
{
public:
	typedef struct _SPRITESLICE
	{
		// Rectangle in which this slice exist;
		A3DRECT rect;
		// Four Corner's UV Coordinates;
		FLOAT	u0, u1;
		FLOAT	v0, v1;
		int		nTextureID;
	} SPRITESLICE;

private:
	A3DDevice *		m_pA3DDevice;
	A2DSprite *		m_pParentSprite;
	A3DRECT			m_rectItem;

	A3DTLVERTEX *	m_pVertices;

	AList			m_ListSlices;

protected:
public:
	A2DSpriteItem();
	~A2DSpriteItem();

	bool Init(A3DDevice * pA3DDevice, A2DSprite * pParentSprite, A3DRECT& itemRect);
	bool Release();

	// Show this item onto screen;
	bool DrawToBack(int x, int y, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY);
	// Draw into the sprite buffer;
	bool DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, int x, int y, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, FLOAT vScaleX, FLOAT vScaleY);

	A3DRECT & GetRect() { return m_rectItem; }

private:
	bool Render(int x, int y, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, SPRITESLICE * pSlice, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY);
	bool RenderToBuffer(A2DSpriteBuffer * pSpriteBuffer, int x, int y, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, SPRITESLICE * pSlice, FLOAT vScaleX, FLOAT vScaleY);
};

typedef A2DSpriteItem * PA2DSpriteItem;

#endif //_A2DSPRITEITEM_H_