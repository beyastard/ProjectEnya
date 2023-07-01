/*
 * FILE: A2DSprite.h
 *
 * DESCRIPTION: 2D Graphics Engine for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A2DSPRITE_H_
#define _A2DSPRITE_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DSurface.h"
#include "A3DDevice.h"
#include "A3DObject.h"

class A2DSprite : public A3DObject
{
	friend class A2DSpriteBuffer;
	friend class A2DSpriteItem;

public:
	typedef struct _TEXTURE_RECT
	{
		A3DRECT			rect;
		int				nWidth;
		int				nHeight;
		A3DTexture *	pA3DTexture;
	} TEXTURE_RECT, * PTEXTURE_RECT;

protected:
	A3DDevice *				m_pA3DDevice;
	bool					m_bHWISprite; // Flag indicates using outside graphics engine;
	
	int						m_nTextureNum;
	A3DFORMAT				m_Format;

	A3DSurface *			m_pA3DSurface;
	A3DRECT *				m_pRects;

	bool					m_bTextureLoaded;
	bool					m_bItemBuilt;

	AList					m_ListTextures;
	AList					m_ListItems;

	int						m_nWidth;
	int						m_nHeight;

	// This is the position where the center of the sprite locates;
	A3DPOINT2				m_ptCenterPos;
	
	// This is the degree of this prite, range [0~360); Left handed; Screen Coordinates
	int						m_nDegree;

	// This is the local center position relative to the left-top corner of the sprite;
	A3DPOINT2				m_ptLocalCenterPos;

	// This is the color of this sprite, pay some attention to its alpha channel, because it will take effect;
	A3DBLEND				m_SrcBlend;
	A3DBLEND				m_DestBlend;			
	A3DCOLOR				m_color;

	// This is the scale factor that this sprite will applied;
	FLOAT					m_vScaleX;
	FLOAT					m_vScaleY;

	// This flag controls wether use linear interpolation or not;
	// For large still sprites, be sure not set this flag, but for small rotating sprite, you may set this 
	bool					m_bFilterLinear;

	int						m_nCurItem;
	int						m_nItemNum;
	A2DSpriteItem *			m_pCurItem;

public:
	A2DSprite();
	~A2DSprite();

	bool InitWithoutSurface(A3DDevice * pA3DDevice, int nWidth, int nHeight, A3DFORMAT format, int nItemCount, A3DRECT * pRects, bool bAutoBuild=true);

	bool Init(A3DDevice * pA3DDevice, char * szSpriteMap, int nWidth, int nHeight, A3DCOLOR colorKey, int nItemCount, A3DRECT * pRects, bool bAutoBuild=true, bool bNeedBackup=false);
	bool Release();

	// Copy the surface into a3dtextures for using;
	bool BuildTextures();
	// Unload these a3dtextures to free some video memory!
	bool UnloadTextures();
	// Update the content of the textures from one image buffer;
	bool UpdateTextures(LPBYTE pDataBuffer, DWORD dwPitch, A3DFORMAT format);

protected:
	bool BuildItems();
	bool ReleaseItems();

public:
	// Draw to back at the position of (x, y);
	bool DrawToBack(int x, int y);
	// Draw to back using preset m_ptPos;
	bool DrawToBack();

	// Draw to Sprite Buffer to reduce the call to DrawPrimitives
	bool DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, int x, int y);
	bool DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer);

	// Set current active item to show;
	bool SetCurrentItem(int nItem);
	// Get a sprite item pointer;
	A2DSpriteItem * GetItem(int nIndex);

protected:
	A3DTexture * GetTextureByID(int nTexID);
	bool GetInterRect(int nTexID, A3DRECT& rectItem, A3DRECT& rectTexture, A3DRECT& rectIntersect);

public:
	inline int GetItemNum() { return m_nItemNum; }
	inline int GetTextureNum() { return m_nTextureNum; }
	inline A3DPOINT2 GetPosition() { return m_ptCenterPos; }
	inline A3DPOINT2 GetLocalCenterPosition() { return m_ptLocalCenterPos; }
	inline int GetDegree() { return m_nDegree; }

	inline bool IsTextureLoaded() { return m_bTextureLoaded; }
	inline void SetPosition(int x, int y) { m_ptCenterPos.x = x; m_ptCenterPos.y = y; }
	inline void SetDegree(int nDegree) { m_nDegree = nDegree; }
	inline void SetLocalCenterPos(int x, int y) { m_ptLocalCenterPos.x = x; m_ptLocalCenterPos.y = y; }
	inline void SetColor(A3DCOLOR color) { m_color = color; }
	inline void SetScaleX(FLOAT vScaleX) { m_vScaleX = vScaleX; }
	inline void SetScaleY(FLOAT vScaleY) { m_vScaleY = vScaleY; }
	inline FLOAT GetScaleX() { return m_vScaleX; }
	inline FLOAT GetScaleY() { return m_vScaleY; }
	inline void SetLinearFilter(bool bEnable) { m_bFilterLinear = bEnable; }
	inline bool IsLinearFilter() { return m_bFilterLinear; }

	inline void SetSrcBlend(A3DBLEND srcBlend) { m_SrcBlend = srcBlend; }
	inline void SetDestBlend(A3DBLEND destBlend) { m_DestBlend = destBlend; }
	inline A3DBLEND GetSrcBlend() { return m_SrcBlend; }
	inline A3DBLEND GetDestBlend() { return m_DestBlend; }
};

typedef A2DSprite * PA2DSprite;
	
#endif //_A2DSPRITE_H_
