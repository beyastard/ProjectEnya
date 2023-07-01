/*
 * FILE: A2DSpriteBuffer.h
 *
 * DESCRIPTION: This is a cache buffer for 2D Graphics Engine for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2003/1/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A2DSPRITEBUFFER_H_
#define _A2DSPRITEBUFFER_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DVertex.h"

class A2DSprite;

class A2DSpriteBuffer
{
public:
	typedef struct _TEXTURE_SLOT
	{
		A3DTexture *	pA3DTexture;

		int				nVertsCount;
		A3DTLVERTEX *	pVertices; // all vertices using this texture

	} TEXTURE_SLOT, * PTEXTURE_SLOT;

protected:
	A3DDevice *				m_pA3DDevice;
	A2DSprite *				m_pSprite;
	int						m_nElementNum;					
	
	int						m_nTextureNum;
	TEXTURE_SLOT *			m_pTextureSlots;

public:
	A2DSpriteBuffer();
	~A2DSpriteBuffer();

	bool Init(A3DDevice * pA3DDevice, A2DSprite * pSprite, int nElementNum);
	bool Release();

public:
	bool FlushBuffer(A3DViewport * pViewport);

	// Add a slice to the buffer;
	// v is the vertex buffer that consist of 4 TLVERTEX, face is 0-1-2, 1-3-2
	bool AddToBuffer(A3DTexture * pTexture, A3DTLVERTEX * v);
};

typedef A2DSpriteBuffer * PA2DSpriteBuffer;
	
#endif //_A2DSPRITEBUFFER_H_
