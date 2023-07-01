/*
 * FILE: A3DConfig.cpp
 *
 * DESCRIPTION: configuration class for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DConfig.h"
#include "A3DErrLog.h"

A3DConfig * g_pA3DConfig;

A3DRUNENV A3DConfig::m_nRunEnv = A3DRUNENV_UNKNOWN;

A3DConfig::A3DConfig()
{
}

A3DConfig::~A3DConfig()
{
}

bool A3DConfig::Init()
{
	// Default Values;
	if( m_nRunEnv == A3DRUNENV_UNKNOWN )
	{
		// Not set running environment yet, we use default value;
		m_nRunEnv = A3DRUNENV_GAME; //In Game;
	}

	m_bFlagDetailScene		= true;
	m_bFlagDetailModel		= true;
	m_bFlagDetailTerrain	= true;
	m_bFlagLightMapScene	= true;
	m_bFlagDynamicLight		= true;
	m_bFlagHQSky			= true;
	m_bFlagHQTexture		= true;
	m_TextureQuality		= A3DTEXTURE_QUALITY_NULL;
	m_bFlagTrilinear		= true;
	m_bFlagNoTextures		= false;

	// On ATI Display Card, use dynamic stream will be very slow!
	m_bFlagUseDynamicStream	= true;
	return true;
}

bool A3DConfig::LoadConfig(char * szConfigFile)
{
	return true;
}

bool A3DConfig::Release()
{
	return true;
}
