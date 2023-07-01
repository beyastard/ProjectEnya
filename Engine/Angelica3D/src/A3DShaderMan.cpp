/*
 * FILE: A3DShaderMan.cpp
 *
 * DESCRIPTION: Routines used to management shader
 *
 * CREATED BY: duyuxin, 2002/1/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DShaderMan.h"
#include "A3DErrLog.h"
#include "A3DEngine.h"

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

A3DShaderMan::A3DShaderMan()
{
	m_pA3DDevice	  = NULL;
	m_iNumPixelShader = 0;

	strcpy(m_szFolderName, "Shaders");

	memset(m_aPixelShaders, 0, sizeof (m_aPixelShaders));
}

A3DShaderMan::~A3DShaderMan()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pDevice: address of A3D device object
*/
bool A3DShaderMan::Init(A3DDevice *pDevice)
{
	m_pA3DDevice = pDevice;

	m_iNumPixelShader = 0;

	return true;
}

//	Release
void A3DShaderMan::Release()
{
	//	Release all pixel shader
	int i;
	for (i=0; i < m_iNumPixelShader; i++)
	{
		if (m_aPixelShaders[i].pShader)
			((A3DPixelShader*)m_aPixelShaders[i].pShader)->Release();
	}

	m_iNumPixelShader = 0;
	memset(m_aPixelShaders, 0, sizeof (m_aPixelShaders));

	m_pA3DDevice = NULL;
}

/*	Load shader file.

	Return shader object's address for success, otherwise return NULL.

	szFileName: shader file's name relative to shader folder
*/
A3DShader* A3DShaderMan::LoadShaderFile(char* szFileName)
{
	AFileImage File;
	A3DShader* pShader;
	char szShaderTitle[MAX_PATH];
	
	AFileMod_GetFileTitle(szFileName, szShaderTitle, MAX_PATH);
	if (!(pShader = new A3DShader()))
	{
		g_pA3DErrLog->ErrLog("A3DShaderMan::LoadShaderFile, not enough memory");
		return NULL;
	}

	if (!pShader->Init(m_pA3DDevice))
	{
		g_pA3DErrLog->ErrLog("A3DShaderMan::LoadShaderFile, failed to initialize shader object");
		return NULL;
	}
	
	if (!File.Open(m_szFolderName, szShaderTitle, AFILE_OPENEXIST | AFILE_BINARY))
	{
		//	No shader file was found, only output a error log and continue
		//	We should not release pShader, because we must use it without effects
		g_pA3DErrLog->ErrLog("A3DShaderMan::LoadShaderFile, [%s] can not be found", szShaderTitle);
		return pShader;
	}

	if (!pShader->Load(NULL, &File))
	{
		//	Load shader file fail, only output a error log and continue
		delete pShader;
		File.Close();
		g_pA3DErrLog->ErrLog("A3DShaderMan::LoadShaderFile, failed to load shader %s", szShaderTitle);
	}

	File.Close();
	
	pShader->SetMapFile(szShaderTitle);
	return pShader;
}

/*	Release A3D shader.

	ppShader: address of A3DShader object's pointer
*/
void A3DShaderMan::ReleaseShader(A3DShader** ppShader)
{
	A3DShader* pShader = *ppShader;

	if (pShader)
	{
		pShader->Release();
		delete pShader;
		pShader = NULL;
	}
}

/*	Search a shader with specified name.

	Return shader's index if it was found, otherwise return -1.

	szFileName: shader's name used to compare
*/
int A3DShaderMan::FindPixelShader(char* szFileName)
{
	int i;
	A3DPixelShader* pCurShader;

	for (i=0; i < m_iNumPixelShader; i++)
	{
		pCurShader = (A3DPixelShader*)m_aPixelShaders[i].pShader;
		if (!_stricmp(szFileName, pCurShader->GetShaderFile()))
			return i;
	}

	return -1;
}

/*	Load pixel shader from file

	Return true for success, otherwise return false.
	
	szFileName: absolute or relative file name of shader
	ppShader (out): address of pixel shader pointer
*/
bool A3DShaderMan::LoadPixelShader(char* szFileName, A3DPixelShader** ppShader)
{
	A3DPixelShader* pNewShader;
	int iIndex;

	*ppShader = NULL;

	if (!szFileName || !szFileName[0])
	{
		g_pA3DErrLog->ErrLog("A3DShaderMan::LoadPixelShader, invalid file name");
		return false;
	}

	if ((iIndex = FindPixelShader(szFileName)) != -1)
	{
		//	Shader with same name has been found
		*ppShader = (A3DPixelShader*)m_aPixelShaders[iIndex].pShader;
		m_aPixelShaders[iIndex].dwRefCnt++;
		return true;
	}
	
	if (m_iNumPixelShader >= MAXNUM_PIXELSHADER)
	{
		g_pA3DErrLog->ErrLog("A3DShaderMan::LoadPixelShader, Pixel shader list has been full!");
		return false;
	}

	if (!(pNewShader = new A3DPixelShader))
	{
		g_pA3DErrLog->ErrLog("A3DShaderMan::LoadPixelShader, Not enough memory!");
		return false;
	}

	if (!pNewShader->Init(m_pA3DDevice))
	{
		delete pNewShader;
		g_pA3DErrLog->ErrLog("A3DShaderMan::LoadPixelShader, Failed to initialize pixel shader!");
		return false;
	}

	char szFullPath[MAX_PATH];

	//	Parse file path
	AFileMod_GetFullPath(szFullPath, m_szFolderName, szFileName);
	
	//	Load pixel shader from file
	if (!pNewShader->Load(szFullPath, szFileName))
	{
		g_pA3DErrLog->ErrLog("A3DTextureMan::LoadPixelShader, Fail to load pixel shader from file %s!", szFullPath);
		return false;
	}

	//	Add new shader to list
	m_aPixelShaders[m_iNumPixelShader].dwRefCnt = 1;
	m_aPixelShaders[m_iNumPixelShader].pShader	= pNewShader;

	m_iNumPixelShader++;

	*ppShader = pNewShader;

	return true;
}

/*	Release a pixel shader 

	Return true for success, otherwise return false.

	pShader: shader's address
*/
bool A3DShaderMan::ReleasePixelShader(A3DPixelShader* pShader)
{
	int i;
	A3DPixelShader* pCurShader;

	for (i=0; i < m_iNumPixelShader; i++)
	{
		pCurShader = (A3DPixelShader*)m_aPixelShaders[i].pShader;
		if (pCurShader == pShader)
		{
			if (!(--m_aPixelShaders[i].dwRefCnt))
			{
				//	Release shader
				pCurShader->Release();
				delete pCurShader;

				//	Use the last element to replace current one
				if (i != m_iNumPixelShader-1)
					m_aPixelShaders[i] = m_aPixelShaders[m_iNumPixelShader-1];

				m_aPixelShaders[m_iNumPixelShader-1].pShader	= NULL;
				m_aPixelShaders[m_iNumPixelShader-1].dwRefCnt	= 0;

				m_iNumPixelShader--;
			}

			return true;
		}
	}

	return false;
}


