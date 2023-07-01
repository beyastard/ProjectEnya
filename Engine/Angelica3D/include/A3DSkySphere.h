/*
 * FILE: A3DSkySphere.h
 *
 * DESCRIPTION: Class that standing for the sphere sky method in A3D Engine
 *
 * CREATED BY: Hedi, 2003/1/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKYSPHERE_H_
#define _A3DSKYSPHERE_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DStream.h"
#include "A3DVertex.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DFrame.h"
#include "A3DCamera.h"
#include "A3DSky.h"

class A3DSkySphere : public A3DSky
{
protected:
	A3DDevice *			m_pA3DDevice;
	A3DCamera *			m_pCamera;
	A3DMATRIX4			m_WorldMatrix;
	A3DMATRIX4			m_tmCapCloud;

private:
	//Creation Param;
	WORD				m_nSegmentCount;
	FLOAT				m_vPitchRange;
	FLOAT				m_fRadius;
	FLOAT				m_fFlySpeedU;
	FLOAT				m_fFlySpeedV;
	FLOAT				m_fTile;

	A3DStream *			m_pStreamSurround;
	A3DStream *			m_pStreamCap;

	int					m_nVertCountS;
	int					m_nIndexCountS;
	
	int					m_nVertCountF;
	int					m_nIndexCountF;
	
	int					m_nVertCountC;
	int					m_nIndexCountC;
	
	A3DTexture *		m_pTextureSurroundF;
	A3DTexture *		m_pTextureSurroundB;
	A3DTexture *		m_pTextureCap;

protected:
	bool CreateSurround();
	bool CreateCap();

public:
	A3DSkySphere();
	~A3DSkySphere();

	bool Init(A3DDevice * pDevice, A3DCamera * pCamera, char * szCap, char * szSurroundF, char * szSurroundB);
	virtual bool Release();

	virtual bool Render();
	virtual bool TickAnimation();

	virtual bool SetCamera(A3DCamera * pCamera);

	inline A3DTexture * GetTextureSurroundF() { return m_pTextureSurroundF; }
	inline A3DTexture * GetTextureSurroundB() { return m_pTextureSurroundB; }
	inline A3DTexture * GetTextureCap() { return m_pTextureCap; }

	bool SetTextureSurroundF(char * szTextureFile);
	bool SetTextureSurroundB(char * szTextureFile);
	bool SetTextureCap(char * szTextureFile);

	inline float GetTile() { return m_fTile; }
	inline void SetTile(float fTile) { m_fTile = fTile; }

	inline float GetFlySpeedU() { return m_fFlySpeedU; }
	inline void SetFlySpeedU(float u) { m_fFlySpeedU = u; }

	inline float GetFlySpeedV() { return m_fFlySpeedV; }
	inline void SetFlySpeedV(float v) { m_fFlySpeedV = v; }
};

typedef A3DSkySphere * PA3DSkySphere;

#endif//_A3D_SKYSPHERE_H_