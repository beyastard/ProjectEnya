/*
 * FILE: A3DCursor.cpp
 *
 * DESCRIPTION: A class manipulate the cursor
 *
 * CREATED BY: Hedi, 2002/5/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DCursor.h"
#include "A3DSurfaceMan.h"
#include "A3DEngine.h"
#include "A3DErrLog.h"

A3DCursor::A3DCursor()
{
	m_szCursorFile[0] = '\0';
	ZeroMemory(&m_ptHotSpot, sizeof(A3DPOINT2));
	ZeroMemory(&m_ptCursorPos, sizeof(A3DPOINT2));

	m_nFrameCount = 0;
	m_nFrame = -1;
}

A3DCursor::~A3DCursor()
{
}

bool A3DCursor::Init(A3DDevice * pDevice, char * szCursorFile, int nWidth, int nHeight, A3DCOLOR colorKey, int nFrameCount, A3DRECT * pRects, A3DPOINT2& ptHotSpot)
{
	if( !A2DSprite::Init(pDevice, szCursorFile, nWidth, nHeight, colorKey, nFrameCount, pRects, true) )
	{
		g_pA3DErrLog->ErrLog("A3DCursor::Init(), Can not init an A2DSprite!");
		return false;
	}

	strncpy(m_szCursorFile, szCursorFile, MAX_PATH);
	m_nFrameCount = nFrameCount;
	m_nFrame = -1;

	// For the first time, we set initializing data;
	SetHotSpot(ptHotSpot);
	TickAnimation();
	return true;
}

bool A3DCursor::Release()
{
	A2DSprite::Release();
	return true;
}

bool A3DCursor::Draw()
{
	return A2DSprite::DrawToBack(m_ptCursorPos.x, m_ptCursorPos.y);	
}

bool A3DCursor::TickAnimation()
{
	// Update cursor pos first;
	POINT pt;
	::GetCursorPos(&pt);
	ScreenToClient(m_pA3DDevice->GetDeviceWnd(), &pt);

	m_ptCursorPos.x = pt.x;
	m_ptCursorPos.y = pt.y;
	
	// Animation;
	if( m_nFrame == -1 )
		m_nFrame = 0;
	else
	{
		++ m_nFrame;
		m_nFrame %= m_nFrameCount;
	}
	A2DSprite::SetCurrentItem(m_nFrame);
	return true;
}

bool A3DCursor::SetPos(A3DPOINT2& ptNewPos)
{
	::SetCursorPos(ptNewPos.x, ptNewPos.y);

	m_ptCursorPos.x = ptNewPos.x;
	m_ptCursorPos.y = ptNewPos.y;
	return true;
}

bool A3DCursor::SetHotSpot(A3DPOINT2& ptHotSpot)
{
	m_ptHotSpot = ptHotSpot;
	if( m_ptHotSpot.x < 0 )
		m_ptHotSpot.x = 0;
	if( m_ptHotSpot.y < 0 )
		m_ptHotSpot.y = 0;
	if( m_ptHotSpot.x >= m_nWidth )
		m_ptHotSpot.x = m_nWidth - 1;
	if( m_ptHotSpot.y >= m_nHeight )
		m_ptHotSpot.y = m_nHeight - 1;

	A2DSprite::SetLocalCenterPos(m_ptHotSpot.x, m_ptHotSpot.y);
	return true;
}
