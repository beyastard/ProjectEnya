/*
 * FILE: A3DPixelShader.h
 *
 * DESCRIPTION: Routines for pixel shader
 *
 * CREATED BY: duyuxin, 2002/3/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DPIXELSHADER_H_
#define _A3DPIXELSHADER_H_

#include "A3DFuncs.h"
#include "A3DDevice.h"
#include <D3dx8core.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

	
///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DPixelShader
//
///////////////////////////////////////////////////////////////////////////

class A3DPixelShader
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DPixelShader();
	virtual ~A3DPixelShader();

public:		//	Attributes

	static float	m_aDefPSConsts[8][4];	//	Default pixel shader constants

public:		//	Operations

	bool		Init(A3DDevice* pDevice);	//	Initialize object
	void		Release();					//	Release object

	bool		Load(char* szFullName, char* szFileName);	//	Load shader from file
	bool		Appear(bool bApplyConsts);	//	Apply this pixel shader
	bool		Disappear();				//	Abolish pixel shader

	inline void	SetPSConstants(float aConsts[8][4]);		//	Set current pixel shader constants
	inline void	GetPSConstants(float aConsts[8][4]);		//	Get current pixel shader constants

	char*		GetShaderFile()			{	return m_szFile;	}

protected:	//	Attributes

	LPD3DXBUFFER	m_pD3DXShaderBuf;		//	Pixel shader code buffer
	DWORD			m_hShader;				//	Pixel shader handle
	float			m_aPSConsts[8][4];		//	Current pixel shader constants
	bool			m_bApplyConsts;			//	true, m_aPSConsts has been used 

	A3DDevice*		m_pA3DDevice;			//	Device pointer
	char			m_szFile[MAX_PATH];		//	File name relative to shader path

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

/*	Set current pixel shader constants

	aConsts: buffer which contain new constants
*/
void A3DPixelShader::SetPSConstants(float aConsts[8][4])
{
	memcpy(m_aPSConsts, aConsts, sizeof (m_aPSConsts));
}

/*	Get current pixel shader constants

	aConsts: buffer used to receive current constants
*/
void A3DPixelShader::GetPSConstants(float aConsts[8][4])
{
	memcpy(aConsts, m_aPSConsts, sizeof (m_aPSConsts));
}


#endif	//	_A3DPIXELSHADER_H_


