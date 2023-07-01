/*
 * FILE: AMConfig.h
 *
 * DESCRIPTION: configuration class for Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMCONFIG_H_
#define _AMCONFIG_H_

#include "AMPlatform.h"
#include "AMTypes.h"

enum AMRUNENV
{
	AMRUNENV_UNKNOWN		= -1,
	AMRUNENV_GAME			= 0,
	AMRUNENV_MODEDITOR		= 1,
	AMRUNENV_GFXEDITOR		= 2,
	AMRUNENV_SCENEEDITOR	= 3,
	AMRUNENV_PURESERVER		= 4,
	AMRUNENV_CONSOLETOOL	= 5
};
		  
enum AMSOUND_QUALITY
{
	AMSOUND_QUALITY_NULL	= -1,	// Not set yet
	AMSOUND_QUALITY_HIGH	= 0,	// origin sample rate
	AMSOUND_QUALITY_MEDIUM	= 1,	// half of origin sample rate, but keep above 11k
	AMSOUND_QUALITY_LOW		= 2,	// quarter of origian sample rate, but keep above 11k
};

class AMConfig
{
private:
	static AMRUNENV		m_nRunEnv;				// 0 -- Game; 1 -- ModEditor; 2 -- GfxEditor; 3 -- SceneEditor;

	bool				m_bFlagForceFeedback;	// Flag indicates whether to use force feed back effects;
	AMSOUND_QUALITY		m_SoundQuality;			// Quality type of sound;

protected:
public:
	AMConfig();
	~AMConfig();

	bool Init();
	bool Release();

	bool LoadConfig(char * szConfigFile);

	static AMRUNENV GetRunEnv() { return m_nRunEnv; }
	static void SetRunEnv(AMRUNENV env) { m_nRunEnv = env; }

	inline bool GetFlagForceFeedback() { return m_bFlagForceFeedback; }
	void SetFlagForceFeedback(bool bFlag) { m_bFlagForceFeedback = bFlag; }

	inline AMSOUND_QUALITY GetSoundQuality() { return m_SoundQuality; }
	inline void SetSoundQuality(AMSOUND_QUALITY quality) { m_SoundQuality = quality; }
};

typedef class AMConfig * PAMConfig;
extern AMConfig * g_pAMConfig;
#endif