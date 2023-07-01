/*
 * FILE: A2DSpriteBuffer.cpp
 *
 * DESCRIPTION: This is a cache buffer for 2D Graphics Engine for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2003/1/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A2DSprite.h"
#include "A2DSpriteBuffer.h"
#include "A3DErrLog.h"

A2DSpriteBuffer::A2DSpriteBuffer()
{
	m_pA3DDevice	= NULL;
	m_pSprite		= NULL;
	m_nTextureNum	= 0;
	m_pTextureSlots = NULL;
	m_nElementNum	= 0;
}

A2DSpriteBuffer::~A2DSpriteBuffer()
{
}

bool A2DSpriteBuffer::Init(A3DDevice * pA3DDevice, A2DSprite * pSprite, int nElementNum)
{
	m_pA3DDevice	= pA3DDevice;
	m_pSprite		= pSprite;
	m_nTextureNum	= m_pSprite->GetTextureNum();
	m_nElementNum	= nElementNum;

	// Now alloc the texture slots buffer
	m_pTextureSlots = (TEXTURE_SLOT *) malloc(m_nTextureNum * sizeof(TEXTURE_SLOT));
	if( NULL == m_pTextureSlots )
	{
		g_pA3DErrLog->ErrLog("A2DSpriteBuffer::Init(), Not enough memory!");
		return false;
	}

	// Init each slot;
	for(int i=0; i<m_nTextureNum; i++)
	{
		m_pTextureSlots[i].nVertsCount = 0;
		m_pTextureSlots[i].pA3DTexture = m_pSprite->GetTextureByID(i);
		m_pTextureSlots[i].pVertices = (A3DTLVERTEX *) malloc(m_nElementNum * 6 * sizeof(A3DTLVERTEX));
		if( NULL == m_pTextureSlots[i].pVertices )
		{
			g_pA3DErrLog->ErrLog("A2DSpriteBuffer::Init(), Not enough memory!");
			return false;
		}
	}
	
	return true;
}

bool A2DSpriteBuffer::Release()
{
	// First free each slot;
	if( m_pTextureSlots )
	{
		for(int i=0; i<m_nTextureNum; i++)
		{
			if( m_pTextureSlots[i].pVertices )
			{
				free(m_pTextureSlots[i].pVertices);
				m_pTextureSlots[i].pVertices = NULL;
			}
		}

		free(m_pTextureSlots);
		m_pTextureSlots = NULL;
	}

	return true;
}

bool A2DSpriteBuffer::FlushBuffer(A3DViewport * pViewport)
{
	// First of all, setup render states;
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	if( !m_pSprite->IsLinearFilter() )
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetSourceAlpha(m_pSprite->GetSrcBlend());
	m_pA3DDevice->SetDestAlpha(m_pSprite->GetDestBlend());

	m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DTLVERTEX);
	for(int i=0; i<m_nTextureNum; i++)
	{
		if( m_pTextureSlots[i].nVertsCount > 0 )
		{
			m_pTextureSlots[i].pA3DTexture->Appear(0);
			m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, m_pTextureSlots[i].nVertsCount / 3, m_pTextureSlots[i].pVertices, sizeof(A3DTLVERTEX));
			m_pTextureSlots[i].pA3DTexture->Disappear(0);

			m_pTextureSlots[i].nVertsCount = 0;
		}
	}

	if( !m_pSprite->IsLinearFilter() )
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	return true;
}

bool A2DSpriteBuffer::AddToBuffer(A3DTexture * pTexture, A3DTLVERTEX * v)
{
	int idSlot = 0;
	// First find the texture slot
	for(; idSlot<m_nTextureNum; idSlot ++)
	{
		if( m_pTextureSlots[idSlot].pA3DTexture == pTexture )
			break;
	}

	if( idSlot == m_nTextureNum )
		return false; // Not found

	TEXTURE_SLOT * pSlot = &m_pTextureSlots[idSlot];

	if( pSlot->nVertsCount >= m_nElementNum * 6 )
		return false; // Too many elements;

	int nVertsCount = pSlot->nVertsCount;
	pSlot->pVertices[nVertsCount + 0] = v[0];
	pSlot->pVertices[nVertsCount + 1] = v[1];
	pSlot->pVertices[nVertsCount + 2] = v[2];

	pSlot->pVertices[nVertsCount + 3] = v[1];
	pSlot->pVertices[nVertsCount + 4] = v[3];
	pSlot->pVertices[nVertsCount + 5] = v[2];
	
	pSlot->nVertsCount += 6;
	return true;
}