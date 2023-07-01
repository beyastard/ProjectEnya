/*
 * FILE: A3DPixelShader.cpp
 *
 * DESCRIPTION: Routines for pixel shader
 *
 * CREATED BY: duyuxin, 2002/3/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DPixelShader.h"
#include "A3DErrLog.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	Default pixel shader constants
float A3DPixelShader::m_aDefPSConsts[8][4] = 
{
	//  Red  Green  Blue  Alpha
	{1.0f, 0.0f, 0.0f, 1.0f},	//	red
	{0.0f, 1.0f, 0.0f, 1.0f},	//	green
	{0.0f, 0.0f, 1.0f, 1.0f},	//	blue
	{1.0f, 1.0f, 0.0f, 1.0f},	//	yellow
	{0.0f, 1.0f, 1.0f, 1.0f},	//	cyan
	{1.0f, 0.0f, 1.0f, 1.0f},	//	purple
	{1.0f, 1.0f, 1.0f, 1.0f},	//	white
	{0.0f, 0.0f, 0.0f, 1.0f},	//	black
};

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DPixelShader::A3DPixelShader()
{
	m_pA3DDevice = NULL;
	m_szFile[0]  = '\0';

	m_pD3DXShaderBuf = NULL;
	m_hShader		 = NULL;
	m_bApplyConsts	 = false;

	memset(m_aPSConsts, 0, sizeof (m_aPSConsts));
}

A3DPixelShader::~A3DPixelShader()
{
}

/*	Initialize object

	Return true for success, otherwise return false;
*/
bool A3DPixelShader::Init(A3DDevice* pDevice)
{
	m_pA3DDevice = pDevice;
	m_szFile[0]  = '\0';

	memcpy(m_aPSConsts, m_aDefPSConsts, sizeof (m_aPSConsts));

	return true;
}

//	Release object
void A3DPixelShader::Release()
{
	if (!m_pA3DDevice)
		return;

	IDirect3DDevice8* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	if (m_hShader)
	{
		pD3DDevice->DeletePixelShader(m_hShader);
		m_hShader = NULL;
	}

	if (m_pD3DXShaderBuf)
	{
		m_pD3DXShaderBuf->Release();
		m_pD3DXShaderBuf = NULL;
	}
}

/*	Load pixel shader from file

	Return true for success, otherwise return false;

	szFullName: full file name including absolute path
	szFileName: relative path and file name
*/
bool A3DPixelShader::Load(char* szFullName, char* szFileName)
{
	HRESULT hr;

	if ((hr = D3DXAssembleShaderFromFile(szFullName, 0, NULL, &m_pD3DXShaderBuf, NULL)) != D3D_OK)
	{
		g_pA3DErrLog->ErrLog("A3DPixelShader::Load, Failed to load pixel shader.");
		return false;
	}

	IDirect3DDevice8* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	if ((hr = pD3DDevice->CreatePixelShader((DWORD*)m_pD3DXShaderBuf->GetBufferPointer(),
											&m_hShader)) != D3D_OK)
	{
		g_pA3DErrLog->ErrLog("A3DPixelShader::Load, Failed to create pixel shader.");
		return false;
	}

	//	Record file name
	strcpy(m_szFile, szFileName);

	return true;
}

/*	Apply pixel shader

	Return true for success, otherwise return false.
	
	bApplyConsts: true, apply this pixel shader's constants at the same time
				  false, don't change pixel shader constants.
*/
bool A3DPixelShader::Appear(bool bApplyConsts)
{
	if (!m_hShader)
		return false;

	IDirect3DDevice8* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	if (bApplyConsts)
		pD3DDevice->SetPixelShaderConstant(0, (void *)m_aPSConsts, 8);

	pD3DDevice->SetPixelShader(m_hShader);

	m_bApplyConsts = bApplyConsts;

	return true;
}

//	Abolish pixel shader
bool A3DPixelShader::Disappear()
{
	IDirect3DDevice8* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	if (m_bApplyConsts)
		pD3DDevice->SetPixelShaderConstant(0, (void *)m_aDefPSConsts, 8);

	pD3DDevice->SetPixelShader(NULL);

	return true;
}

