/*
 * FILE: A3DShaderMan.h
 *
 * DESCRIPTION: Routines used to management shader
 *
 * CREATED BY: duyuxin, 2002/1/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSHADERMAN_H_
#define _A3DSHADERMAN_H_

#include "A3DShader.h"
#include "A3DPixelShader.h"

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
//	Class A3DShaderMan
//
///////////////////////////////////////////////////////////////////////////

class A3DShaderMan
{
public:		//	Types

	enum
	{
		MAXNUM_PIXELSHADER	= 256	//	Maximum number of pixel shader
	};

	typedef struct _SHADERITEM
	{
		DWORD		dwRefCnt;		//	Reference count
		void*		pShader;		//	Shader object
	
	} SHADERITEM, *PSHADERITEM;

public:		//	Constructors and Destructors

	A3DShaderMan();
	virtual ~A3DShaderMan();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice *pDevice);		//	Initialize object
	void		Release();						//	Release

	A3DShader*	LoadShaderFile(char* szFileName);		//	Load shader file
	void		ReleaseShader(A3DShader** ppShader);	//	Release shader

	bool		LoadPixelShader(char* szFileName, A3DPixelShader** ppShader);	//	Load pixel shader
	bool		ReleasePixelShader(A3DPixelShader* pShader);	//	Release pixel shader 

	void		SetFolderName(char* szFolderName)	{	strcpy(m_szFolderName, szFolderName);	}

protected:	//	Attributes

	A3DDevice*	m_pA3DDevice;
	char		m_szFolderName[MAX_PATH];

	SHADERITEM	m_aPixelShaders[MAXNUM_PIXELSHADER];	//	Pixel shader array
	int			m_iNumPixelShader;						//	Number of element in m_aPixelShaders

protected:	//	Operations

	int			FindPixelShader(char* szFileName);		//	Search a shader with specified name

	inline char * GetFolderName() { return m_szFolderName; }
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSHADERMAN_H_


