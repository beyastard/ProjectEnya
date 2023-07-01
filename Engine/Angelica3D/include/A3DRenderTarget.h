/*
 * FILE: A3DRenderTarget.h
 *
 * DESCRIPTION: class representing a render target;
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DRENDERTARGET_H_
#define _A3DRENDERTARGET_H_

#include "A3DPlatform.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DStream.h"

class A3DRenderTarget
{
private:
	A3DDevice				* m_pA3DDevice;

	IDirect3DTexture8		* m_pTargetTexture;
	IDirect3DSurface8		* m_pTargetSurface;
	IDirect3DTexture8		* m_pDepthTexture;
	IDirect3DSurface8		* m_pDepthSurface;

	IDirect3DSurface8		* m_pColorSurface;

	A3DStream				* m_pA3DStream;

	A3DDEVFMT				m_DevFormat;
	bool					m_bNewTarget;
	bool					m_bNewDepth;

	bool					m_bUsingTexture;

	AList					m_ListViewport;
protected:

public:
	A3DRenderTarget();
	~A3DRenderTarget();

	bool AddViewport(A3DViewport * pViewport, ALISTELEMENT ** ppElement);
	bool RenderAllViewport();

	bool PresentToBack(int x, int y, FLOAT z=0.0f);

	bool Init(A3DDevice * pA3DDevice, A3DDEVFMT &devFmt, bool bNewTarget, bool bNewDepth);
	bool Release();

	bool ExportColorToFile(char * szFullpath);
	bool ExportDepthToFile(char * szFullpath);

	inline IDirect3DTexture8 * GetTargetTexture() { return m_pTargetTexture; }
	inline IDirect3DTexture8 * GetDepthTexture() { return m_pDepthTexture; }
	inline IDirect3DSurface8 * GetTargetSurface() { return m_pColorSurface; }
	inline IDirect3DSurface8 * GetDepthSurface() { return m_pDepthSurface; }
	inline int GetViewNum() { return m_ListViewport.GetSize(); }
};

typedef A3DRenderTarget * PA3DRenderTarget;

#endif