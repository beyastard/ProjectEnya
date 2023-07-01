/*
 * FILE: A2DSpriteItem.cpp
 *
 * DESCRIPTION: Sprite Item class
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A2DSpriteItem.h"
#include "A2DSprite.h"
#include "A2DSpriteBuffer.h"
#include "A3DErrLog.h"

A2DSpriteItem::A2DSpriteItem()
{
	m_pA3DDevice = NULL;
	m_pParentSprite = NULL;
	m_pVertices = NULL;
}

A2DSpriteItem::~A2DSpriteItem()
{
}

bool A2DSpriteItem::Init(A3DDevice * pA3DDevice, A2DSprite * pParentSprite, A3DRECT& rect)
{
	m_pA3DDevice	= pA3DDevice;
	m_pParentSprite = pParentSprite;
	m_rectItem		= rect;

	m_ListSlices.Init();

	m_pVertices = (A3DTLVERTEX *) malloc(6 * sizeof(A3DTLVERTEX));
	if( NULL == m_pVertices )
	{
		g_pA3DErrLog->ErrLog("A2DSpriteItem::Init Not enough memory!");
		return false;
	}

	for(int i=0; i<m_pParentSprite->GetTextureNum(); i++)
	{
		A3DRECT rectTexture, rectIntersect;

		if( m_pParentSprite->GetInterRect(i, m_rectItem, rectTexture, rectIntersect) )
		{
			SPRITESLICE * pSlice = (SPRITESLICE *) malloc(sizeof(SPRITESLICE));
			if( NULL == pSlice )
			{
				g_pA3DErrLog->ErrLog("A2DSpriteItem::Init Not enough memory!");
				return false;
			}

			pSlice->nTextureID	= i;
			pSlice->rect.left	= rectIntersect.left - m_rectItem.left;
			pSlice->rect.top	= rectIntersect.top - m_rectItem.top;
			pSlice->rect.right	= rectIntersect.right - m_rectItem.left;
			pSlice->rect.bottom = rectIntersect.bottom - m_rectItem.top;
			pSlice->u0 = (rectIntersect.left - rectTexture.left) * 1.0f / (rectTexture.right - rectTexture.left);
			pSlice->u1 = (rectIntersect.right - rectTexture.left) * 1.0f / (rectTexture.right - rectTexture.left);
			pSlice->v0 = (rectIntersect.top - rectTexture.top) * 1.0f / (rectTexture.bottom - rectTexture.top);
			pSlice->v1 = (rectIntersect.bottom - rectTexture.top) * 1.0f / (rectTexture.bottom - rectTexture.top);
			
			m_ListSlices.Append((LPVOID) pSlice);
		}
	}
	return true;
}

bool A2DSpriteItem::Release()
{
	ALISTELEMENT * pThisSliceElement = m_ListSlices.GetFirst();
	while( pThisSliceElement != m_ListSlices.GetTail() )
	{
		SPRITESLICE * pSlice = (SPRITESLICE *)pThisSliceElement->pData;
		free(pSlice);
		pSlice = NULL;

		pThisSliceElement = pThisSliceElement->pNext;
	}
	m_ListSlices.Release();

	if( m_pVertices )
	{
		free(m_pVertices);
		m_pVertices = NULL;
	}
	return true;
}

bool A2DSpriteItem::DrawToBack(int x, int y, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY)
{
	ALISTELEMENT * pThisSliceElement = m_ListSlices.GetFirst();

	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	if( !bFilterLinear )
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	
	m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DTLVERTEX);

	while( pThisSliceElement != m_ListSlices.GetTail() )
	{
		SPRITESLICE * pSlice = (SPRITESLICE *)pThisSliceElement->pData;
		
		if( !Render(x, y, color, nDegree, ptLocalCenter, pSlice, bFilterLinear, vScaleX, vScaleY) )
		{
			if( !bFilterLinear )
				m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
			m_pA3DDevice->SetZWriteEnable(true);
			m_pA3DDevice->SetZTestEnable(true);
			return false;
		}
		
		pThisSliceElement = pThisSliceElement->pNext;
	}

	if( !bFilterLinear )
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	return true;
}

bool A2DSpriteItem::Render(int x, int y, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, SPRITESLICE * pSlice, bool bFilterLinear, FLOAT vScaleX, FLOAT vScaleY)
{
	A3DCOLOR	diffuse = color;//A3DCOLORRGBA(255, 255, 255, 255);
	A3DCOLOR	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR3  vec3[4];
	A3DVECTOR3  vecCenter;
	A3DVECTOR4	vec4[4];

	A3DMATRIX4  matRotate;

	A3DTLVERTEX	v[4];

	A3DTexture * pTexture = m_pParentSprite->GetTextureByID(pSlice->nTextureID);
	if( !pTexture ) return false;

	//First construct a rect in the coorinates of ptLocalCenter;
	vec3[0] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
	vec3[1] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
	vec3[2] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
	vec3[3] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);

	vecCenter = A3DVECTOR3(x * 1.0f, y * 1.0f, 0.0f);

	matRotate = RotateZ(DEG2RAD(nDegree));
	for(int i=0; i<4; i++)
	{
		if( nDegree != 0 )
			vec3[i] = vec3[i] * matRotate;

		vec3[i] = vec3[i] + vecCenter;
		vec4[i] = A3DVECTOR4(vec3[i].x, vec3[i].y, vec3[i].z, 1.0f);
	}

	v[0] = A3DTLVERTEX(vec4[0], diffuse, specular, pSlice->u0, pSlice->v0);
	v[1] = A3DTLVERTEX(vec4[1], diffuse, specular, pSlice->u1, pSlice->v0);
	v[2] = A3DTLVERTEX(vec4[2], diffuse, specular, pSlice->u0, pSlice->v1);
	v[3] = A3DTLVERTEX(vec4[3], diffuse, specular, pSlice->u1, pSlice->v1);

	// face is 0-1-2, 1-3-2
	m_pVertices[0] = v[0];
	m_pVertices[1] = v[1];
	m_pVertices[2] = v[2];
	m_pVertices[3] = v[1];
	m_pVertices[4] = v[3];
	m_pVertices[5] = v[2];
	
	pTexture->Appear(0);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	if( !m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, 2, m_pVertices, sizeof(A3DTLVERTEX)) )
		return false;

	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	pTexture->Disappear(0);

	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	return true;
}

bool A2DSpriteItem::DrawToBuffer(A2DSpriteBuffer * pSpriteBuffer, int x, int y, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, FLOAT vScaleX, FLOAT vScaleY)
{
	ALISTELEMENT * pThisSliceElement = m_ListSlices.GetFirst();

	while( pThisSliceElement != m_ListSlices.GetTail() )
	{
		SPRITESLICE * pSlice = (SPRITESLICE *)pThisSliceElement->pData;
		
		if( !RenderToBuffer(pSpriteBuffer, x, y, color, nDegree, ptLocalCenter, pSlice, vScaleX, vScaleY) )
		{
			return false;
		}
		
		pThisSliceElement = pThisSliceElement->pNext;
	}

	return true;
}

bool A2DSpriteItem::RenderToBuffer(A2DSpriteBuffer * pSpriteBuffer, int x, int y, A3DCOLOR color, int nDegree, A3DPOINT2& ptLocalCenter, SPRITESLICE * pSlice, FLOAT vScaleX, FLOAT vScaleY)
{
	A3DCOLOR	diffuse = color;
	A3DCOLOR	specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR3  vec3[4];
	A3DVECTOR3  vecCenter;
	A3DVECTOR4	vec4[4];

	A3DMATRIX4  matRotate;

	A3DTLVERTEX	v[4];

	A3DTexture * pTexture = m_pParentSprite->GetTextureByID(pSlice->nTextureID);
	if( !pTexture ) return false;

	//First construct a rect in the coorinates of ptLocalCenter;
	vec3[0] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
	vec3[1] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.top - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
	vec3[2] = A3DVECTOR3((pSlice->rect.left - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);
	vec3[3] = A3DVECTOR3((pSlice->rect.right - ptLocalCenter.x) * vScaleX - 0.5f, (pSlice->rect.bottom - ptLocalCenter.y) * vScaleY - 0.5f, 0.0f);

	vecCenter = A3DVECTOR3(x * 1.0f, y * 1.0f, 0.0f);

	matRotate = RotateZ(DEG2RAD(nDegree));
	for(int i=0; i<4; i++)
	{
		if( nDegree != 0 )
			vec3[i] = vec3[i] * matRotate;

		vec3[i] = vec3[i] + vecCenter;
		vec4[i] = A3DVECTOR4(vec3[i].x, vec3[i].y, vec3[i].z, 1.0f);
	}

	v[0] = A3DTLVERTEX(vec4[0], diffuse, specular, pSlice->u0, pSlice->v0);
	v[1] = A3DTLVERTEX(vec4[1], diffuse, specular, pSlice->u1, pSlice->v0);
	v[2] = A3DTLVERTEX(vec4[2], diffuse, specular, pSlice->u0, pSlice->v1);
	v[3] = A3DTLVERTEX(vec4[3], diffuse, specular, pSlice->u1, pSlice->v1);

	return pSpriteBuffer->AddToBuffer(pTexture, v);
}

