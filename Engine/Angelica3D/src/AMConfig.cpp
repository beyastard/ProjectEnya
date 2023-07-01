/*
 * FILE: AMConfig.cpp
 *
 * DESCRIPTION: configuration class for Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AMConfig.h"

AMConfig * g_pAMConfig;

AMRUNENV AMConfig::m_nRunEnv = AMRUNENV_UNKNOWN;

AMConfig::AMConfig()
{
}

AMConfig::~AMConfig()
{
}

bool AMConfig::Init()
{
	if( m_nRunEnv == AMRUNENV_UNKNOWN )
	{
		m_nRunEnv = AMRUNENV_GAME;
	}

	m_bFlagForceFeedback = true;
	m_SoundQuality = AMSOUND_QUALITY_HIGH;
	return true;
}

bool AMConfig::Release()
{
	return true;
}

bool AMConfig::LoadConfig(char * szConfigFile)
{
	return true;
}