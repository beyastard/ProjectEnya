/*
 * FILE: A2DSprite.cpp
 *
 * DESCRIPTION: 2D Graphics Engine for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A3DSurfaceMan.h"
#include "A3DErrLog.h"
#include "A3DConfig.h"

A2DSprite::A2DSprite()
{
	m_pA3DDevice			= NULL;
	m_bHWISprite			= false;

	m_nTextureNum			= 0;
	m_nItemNum				= 0;
	m_pRects				= NULL;
							
	m_pA3DSurface			= NULL;
	m_pCurItem				= NULL;
	m_nCurItem				= -1;
							
	m_bTextureLoaded		= false;
	m_bItemBuilt			= false;

	m_ptCenterPos.x			= 0;
	m_ptCenterPos.y			= 0;

	m_ptLocalCenterPos.x	= 0;
	m_ptLocalCenterPos.y	= 0;

	m_nDegree				= 0;
	m_vScaleX				= 1.0f;
	m_vScaleY				= 1.0f;

	m_color					= A3DCOLORRGBA(255, 255, 255, 255);
	m_SrcBlend				= A3DBLEND_SRCALPHA;
	m_DestBlend				= A3DBLEND_INVSRCALPHA;

	m_bFilterLinear			= false;
}

A2DSprite::~A2DSprite()
{
}

bool A2DSprite::InitWithoutSurface(A3DDevice * pA3DDevice, int nWidth, int nHeight, A3DFORMAT format, int nItemCount, A3DRECT * pRects, bool bAutoBuild)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWISprite = true;
		return true;
	}

	m_ListTextures.Init();
	m_ListItems.Init();

	m_pA3DSurface	= NULL;

	m_pA3DDevice	= pA3DDevice;
	m_nWidth		= nWidth;
	m_nHeight		= nHeight;
	m_Format		= format;

	m_nItemNum = nItemCount;
	m_pRects = (A3DRECT *) malloc(sizeof(A3DRECT) * nItemCount);
	if( NULL == m_pRects )
	{
		g_pA3DErrLog->ErrLog("A2DSprite::Init() Not enough memory!");
		return false;
	}
	memcpy(m_pRects, pRects, sizeof(A3DRECT) * m_nItemNum);

	if( bAutoBuild )
	{
		bool	bval;

		bval = BuildTextures();
		if( !bval )
		{
			g_pA3DErrLog->ErrLog("A2DSprite::Init BuildTextures Fail!");
			return false;
		}
	}

	SetCurrentItem(0);

	return true;
}

bool A2DSprite::Init(A3DDevice * pA3DDevice, char * szSpriteMap, int nWidth, int nHeight, A3DCOLOR colorKey, int nItemCount, A3DRECT * pRects, bool bAutoBuild, bool bNeedBackup)
{
	m_pA3DDevice	= pA3DDevice;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWISprite = true;
		return true;
	}

	m_ListTextures.Init();
	m_ListItems.Init();

	m_nWidth		= nWidth;
	m_nHeight		= nHeight;
	
	bool	bval;
	bval = m_pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->LoadSurfaceFromFile(nWidth, nHeight, szSpriteMap, colorKey, &m_pA3DSurface);
	if( !bval ) 
	{
		g_pA3DErrLog->ErrLog("A2DSprite::Init LoadSurfaceFromFile Fail!");
		return false;
	}
	m_Format = m_pA3DSurface->GetFormat();

	if( m_Format == A3DFMT_X8R8G8B8 )
		m_Format = A3DFMT_A8R8G8B8;
	
	m_nItemNum = nItemCount;
	m_pRects = (A3DRECT *) malloc(sizeof(A3DRECT) * nItemCount);
	if( NULL == m_pRects )
	{
		g_pA3DErrLog->ErrLog("A2DSprite::Init() Not enough memory!");
		return false;
	}
	memcpy(m_pRects, pRects, sizeof(A3DRECT) * m_nItemNum);

	if( bAutoBuild )
	{
		bval = BuildTextures();
		if( !bval )
		{
			g_pA3DErrLog->ErrLog("A2DSprite::Init BuildTextures Fail!");
			return false;
		}
	}

	if( !bNeedBackup )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->ReleaseSurface(m_pA3DSurface);
		m_pA3DSurface = NULL;
	}

	SetCurrentItem(0);

	SetName(szSpriteMap);

	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DENGINE_OBJECT_SPRITE);

	//g_pA3DErrLog->ErrLog("%s, A2DSprite::Init()", szSpriteMap);
	return true;
}

bool A2DSprite::Release()
{
	if( m_bHWISprite ) return true;

	//g_pA3DErrLog->ErrLog("%s, A2DSprite::Release()", GetName());

	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DENGINE_OBJECT_SPRITE);

	if( m_pRects )
	{
		free(m_pRects);
		m_pRects = NULL;
	}

	UnloadTextures();
	m_ListTextures.Release();

	ReleaseItems();
	m_ListItems.Release();

	if( m_pA3DSurface )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DSurfaceMan()->ReleaseSurface(m_pA3DSurface);
	}

	m_bTextureLoaded = false;
	m_bItemBuilt = false;
	return true;
}

// Update the content of the textures from the surface;
bool A2DSprite::UpdateTextures(LPBYTE pDataBuffer, DWORD dwPitch, A3DFORMAT format)
{
	if( m_bHWISprite ) return true;

	if( !m_bTextureLoaded ) return false;

	DWORD dwBytesPerPixel;
	if( format == A3DFMT_R5G6B5 || format == A3DFMT_X1R5G5B5 )
		dwBytesPerPixel = 2;
	else if( format == A3DFMT_X8R8G8B8 || format == A3DFMT_A8R8G8B8 )
		dwBytesPerPixel = 3;
	else
	{
		g_pA3DErrLog->ErrLog("A2DSprite::UpdateTextures(), Unsupported video format");
		return false;
	}

	ALISTELEMENT * pElement = m_ListTextures.GetFirst();
	while( pElement != m_ListTextures.GetTail() )
	{
		TEXTURE_RECT *	pTextureRect = (TEXTURE_RECT *) pElement->pData;

		A3DTexture *	pTexture = pTextureRect->pA3DTexture;
		A3DRECT			rect = pTextureRect->rect;
		int				nTexWidth = rect.right - rect.left;//pTextureRect->nWidth;
		int				nTexHeight = rect.bottom - rect.top;//pTextureRect->nHeight;

		LPBYTE			pTexBuffer;
		int				iTexPitch;
		if( !pTexture->LockRect(NULL, (LPVOID *)&pTexBuffer, &iTexPitch, 0) )
		{
			g_pA3DErrLog->ErrLog("A2DSprite::UpdateTextures(), Can not lock texture!");
			return false;
		}

		if( format != m_Format ) 
		{
			g_pA3DErrLog->ErrLog("A2DSprite::UpdateTextures(), Different format!");
			return false;
		}

		// Copy the bits    
		// OPTIMIZATION OPPORTUNITY: Use a video and texture
		// format that allows a simpler copy than this one.
		int nSX, nSY;
		if( m_Format == A3DFMT_X8R8G8B8 || m_Format == A3DFMT_A8R8G8B8 )
		{
			for(int y=0; y<nTexHeight; y++)
			{
				nSY = y + rect.top;
				nSX = rect.left;

				BYTE *pBmpBuffer = pDataBuffer + (m_nHeight - 1 - nSY) * dwPitch + nSX * dwBytesPerPixel;
				BYTE *pSurfaceBuffer = pTexBuffer;
				for(int x = 0; x<nTexWidth; x++) 
				{
					pSurfaceBuffer[0] = pBmpBuffer[0];
					pSurfaceBuffer[1] = pBmpBuffer[1];
					pSurfaceBuffer[2] = pBmpBuffer[2];
					pSurfaceBuffer[3] = 0xff;
					
					pSurfaceBuffer += 4;
					pBmpBuffer += 3;
					nSX ++;
				}
				pTexBuffer += iTexPitch;
			}
		}
		else if( m_Format == A3DFMT_R5G6B5 || m_Format == A3DFMT_X1R5G5B5 )
		{
			for(int y=0; y<nTexHeight; y++)
			{
				nSY = y + rect.top;
				nSX = rect.left;

				BYTE *pBmpBuffer = pDataBuffer + (m_nHeight - 1 - nSY) * dwPitch + nSX * dwBytesPerPixel;
				BYTE *pSurfaceBuffer = pTexBuffer;

				memcpy(pSurfaceBuffer, pBmpBuffer, nTexWidth * dwBytesPerPixel);

				pTexBuffer += iTexPitch;
			}
		}
		else
		{
			g_pA3DErrLog->ErrLog("A2DSprite::UpdateTextures(), Unsupported format!");
			return false;
		}
		
		if( !pTexture->UnlockRect() )
		{
			g_pA3DErrLog->ErrLog("A2DSprite::UpdateTextures(), Can not unlock texture!");
			return false;
		}

    	pElement = pElement->pNext;
	}

	return true;
}

bool A2DSprite::BuildTextures()
{
	if( m_bHWISprite ) return true;

	if( m_bTextureLoaded ) return true;
	m_nTextureNum = 0;

	int		x = 0, y = 0;
	int		nThisWidth, nThisHeight;

	while(x < m_nWidth)
	{
		if( m_nWidth - x > 256 )
			nThisWidth = 256;
		else
			nThisWidth = m_nWidth - x;

		y = 0;
		while(y < m_nHeight)
		{
			if( m_nHeight - y > 256 )
				nThisHeight = 256;
			else
				nThisHeight = m_nHeight - y;

			A3DRECT rect;
			int		nTexWidth, nTexHeight;

			rect.left = x; rect.top = y;
			rect.right = x + nThisWidth; rect.bottom = y + nThisHeight;

			A3DTexture * pA3DTexture = new A3DTexture();
			if( NULL == pA3DTexture )
			{
				g_pA3DErrLog->ErrLog("A2DSprite::BuildTextures(), not enough memory!");
				return false;
			}
			
			if( !pA3DTexture->CreateFromSurface(m_pA3DDevice, m_pA3DSurface, rect, &nTexWidth, &nTexHeight, m_Format) )
			{
				g_pA3DErrLog->ErrLog("A2DSprite::BuildTextures(), CreateTextureFromSurface Fail!");
				return false;
			}
			
			TEXTURE_RECT * pTextureRect = (TEXTURE_RECT *) malloc(sizeof(TEXTURE_RECT));
			if( NULL == pTextureRect )
			{
				g_pA3DErrLog->ErrLog("A2DSprite::BuildTextures Not enough memory!");
				return false;
			}

			pTextureRect->nWidth = nTexWidth;
			pTextureRect->nHeight = nTexHeight;
			pTextureRect->rect = rect;
			pTextureRect->pA3DTexture = pA3DTexture;

			m_ListTextures.Append((LPVOID)pTextureRect);
			m_nTextureNum ++;

			y += nThisHeight;
		}

		x += nThisWidth;
	}

	m_bTextureLoaded = true;

	if( !BuildItems() )
	{
		g_pA3DErrLog->ErrLog("A2DSprite::Init BuildItems Fail!");
		return false;
	}

	return true;
}

bool A2DSprite::BuildItems()
{
	A3DRECT rect;
	bool	bval;

	if( m_bItemBuilt ) return true;

	for(int i=0; i<m_nItemNum; i++)
	{
		A2DSpriteItem * pItem;

		rect = m_pRects[i];

		if( rect.left < 0 || rect.top < 0 || rect.right > m_nWidth || rect.bottom > m_nHeight )
		{
			g_pA3DErrLog->ErrLog("A2DSprite::BuildItems one of the item rects is invalid");
			return false;
		}

		pItem = new A2DSpriteItem();
		if( NULL == pItem )
		{
			g_pA3DErrLog->ErrLog("A2DSprite::BuildItems Not enough memory!");
			return false;
		}

		bval = pItem->Init(m_pA3DDevice, this, rect);
		if( !bval )
		{
			g_pA3DErrLog->ErrLog("A2DSprite::BuildItems Init Item Fail!");
			return false;
		}

		m_ListItems.Append((LPVOID) pItem);
	}

	m_bItemBuilt = true;
	return true;
}

bool A2DSprite::UnloadTextures()
{
	if( m_bHWISprite ) return true;

	if( !m_bTextureLoaded ) return true;

	ALISTELEMENT * pTextureElement = m_ListTextures.GetFirst();
	while( pTextureElement != m_ListTextures.GetTail() )
	{
		TEXTURE_RECT * pTextureRect = (TEXTURE_RECT *)pTextureElement->pData;

		A3DTexture * pA3DTexture = (A3DTexture *) pTextureRect->pA3DTexture;
		pA3DTexture->Release();
		delete pA3DTexture;
		pA3DTexture = NULL;

		free(pTextureRect);
		pTextureRect = NULL;

		pTextureElement = pTextureElement->pNext;
	}

	m_ListTextures.Reset();
	m_bTextureLoaded = false;
	return true;
}

bool A2DSprite::ReleaseItems()
{
	ALISTELEMENT * pItemElement = m_ListItems.GetFirst();
	while( pItemElement != m_ListItems.GetTail() )
	{
		A2DSpriteItem * pItem = (A2DSpriteItem *) pItemElement->pData;
		pItem->Release();
		delete pItem;
		pItem = NULL;

		pItemElement = pItemElement->pNext;
	}

	m_bItemBuilt = false;
	return true;
}

bool A2DSprite::DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, int x, int y)
{
	if( m_bHWISprite ) return true;

	if( !m_bTextureLoaded && !m_bItemBuilt )
	{
		g_pA3DErrLog->ErrLog("A2DSprite::DrawToBack() BuildTextures and BuildItems has not been called! Now force build them...");
		if( !BuildTextures() )
			return false;
		g_pA3DErrLog->ErrLog("A2DSprite::DrawToBack() Force BuildTextures and BuildItems success, now continue to work...");

		if( m_nCurItem >= 0 )
		{
			if( !SetCurrentItem(m_nCurItem) )
				return false;
		}
	}

	if( m_pCurItem )
	{
		if( !m_pCurItem->DrawToBuffer(pSpriteBuffer, x, y, m_color, m_nDegree, m_ptLocalCenterPos, m_vScaleX, m_vScaleY) )
			return false;
	}

	return true;
}

bool A2DSprite::DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer)
{
	return DrawToBuffer(pSpriteBuffer, m_ptCenterPos.x, m_ptCenterPos.y);
}

bool A2DSprite::DrawToBack(int x, int y)
{
	if( m_bHWISprite ) return true;

	if( !m_bTextureLoaded && !m_bItemBuilt )
	{
		g_pA3DErrLog->ErrLog("A2DSprite::DrawToBack() BuildTextures and BuildItems has not been called! Now force build them...");
		if( !BuildTextures() )
			return false;
		g_pA3DErrLog->ErrLog("A2DSprite::DrawToBack() Force BuildTextures and BuildItems success, now continue to work...");

		if( m_nCurItem >= 0 )
		{
			if( !SetCurrentItem(m_nCurItem) )
				return false;
		}
	}

	if( m_pCurItem )
	{
		m_pA3DDevice->SetSourceAlpha(m_SrcBlend);
		m_pA3DDevice->SetDestAlpha(m_DestBlend);

		if( !m_pCurItem->DrawToBack(x, y, m_color, m_nDegree, m_ptLocalCenterPos, m_bFilterLinear, m_vScaleX, m_vScaleY) )
			return false;

		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}
	return true;
}

bool A2DSprite::DrawToBack()
{
	return DrawToBack(m_ptCenterPos.x, m_ptCenterPos.y);
}

A3DTexture * A2DSprite::GetTextureByID(int nTexID)
{
	if( m_bHWISprite ) return NULL;

	ALISTELEMENT * pElement = m_ListTextures.GetElementByOrder(nTexID);
	if( NULL == pElement ) return NULL;

	return ((TEXTURE_RECT *)pElement->pData)->pA3DTexture;
}

bool A2DSprite::SetCurrentItem(int nItem)
{
	if( m_bHWISprite ) return true;

	if( !m_bTextureLoaded )
	{
		// Not loaded yet, so we have to record the item number and load it at necessary time;
		m_nCurItem = nItem;
		m_pCurItem = NULL;
		return true;
	}

	if( nItem >= m_nItemNum )
		nItem = 0;

	ALISTELEMENT * pElement = m_ListItems.GetElementByOrder(nItem);
	if( NULL == pElement ) return false;

	m_pCurItem = (A2DSpriteItem *) pElement->pData;
	m_nCurItem = nItem;
	return true;
}

/*
	Get the intersection of one item with a texture
	IN:
		nTexID			texture's id
		rectItem		the item's rect
	OUT:
		rectTexture		if intersect this will be the texture's real covered rect
		rectIntersect	if intersect this will be the intersection rect
	return true if intersects or return false;
*/
bool A2DSprite::GetInterRect(int nTexID, A3DRECT& rectItem, A3DRECT& rectTexture, A3DRECT& rectIntersect)
{
	A3DRECT rectDest, rectSrc1, rectSrc2;

	TEXTURE_RECT * pTextureRect = (TEXTURE_RECT *)m_ListTextures.GetElementByOrder(nTexID)->pData;
	if( NULL == pTextureRect )
		return false;

	rectSrc1 = rectItem;
	rectSrc2 = pTextureRect->rect;

	if( !IntersectRect((RECT *)&rectDest, (RECT *)&rectSrc1, (RECT *)&rectSrc2) )
		return false;

	rectTexture.left = pTextureRect->rect.left;
	rectTexture.top = pTextureRect->rect.top;
	rectTexture.right = rectTexture.left + pTextureRect->nWidth;
	rectTexture.bottom = rectTexture.top + pTextureRect->nHeight;
	rectIntersect = rectDest;
	return true;
}

A2DSpriteItem * A2DSprite::GetItem(int nIndex)
{
	if( m_bHWISprite ) return NULL;

	ALISTELEMENT * pElement = m_ListItems.GetElementByOrder(nIndex);
	if( NULL == pElement )
		return NULL;

	return (A2DSpriteItem *) pElement->pData;
}