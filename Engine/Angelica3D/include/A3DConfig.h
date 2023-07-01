/*
 * FILE: A3DConfig.h
 *
 * DESCRIPTION: configuration class for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCONFIG_H_
#define _A3DCONFIG_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"

enum A3DRUNENV
{
	A3DRUNENV_UNKNOWN		= -1,
	A3DRUNENV_GAME			= 0,
	A3DRUNENV_MODEDITOR		= 1,
	A3DRUNENV_GFXEDITOR		= 2,
	A3DRUNENV_SCENEEDITOR	= 3,
	A3DRUNENV_PURESERVER	= 4,
	A3DRUNENV_CONSOLETOOL	= 5
};

enum A3DTEXTURE_QUALITY
{
	A3DTEXTURE_QUALITY_NULL		= -1,	// Not set yet, use m_bFlagHQTexture flag
	A3DTEXTURE_QUALITY_HIGH		= 0,	// origin size 
	A3DTEXTURE_QUALITY_MEDIUM	= 1,	// half of origin size in width and height
	A3DTEXTURE_QUALITY_LOW		= 2,	// quarter of origin size in width and height
};

class A3DConfig
{
private:
	static A3DRUNENV	m_nRunEnv; // 0 -- Game; 1 -- ModEditor; 2 -- GfxEditor; 3 -- SceneEditor;

	bool	m_bFlagDetailScene; // Flag indicates whether to use detail texture on scene buildings;
	bool	m_bFlagDetailModel; // Flag indicates whether to use detail texture on model;
	bool	m_bFlagDetailTerrain; // Flag indicates whether to use detail texture on terrain;
	bool	m_bFlagLightMapScene; // Flag indicates whether to use light maps to shade the scene;
	bool	m_bFlagDynamicLight; // Flag indicates whether to use dynamic lights;
	bool	m_bFlagHQSky; // Flag indicates whether to use high quality sky;
	bool	m_bFlagHQTexture; // Flag indicates whether to use large texture or not;	
	bool	m_bFlagTrilinear; // Flag indicates whether we will use trilinear to blend textures;

	bool	m_bFlagNoTextures; // Flag indicates whether to load textures;

	bool	m_bFlagUseDynamicStream; // Flag indicates whether to use dynamic stream or not;

	bool	m_bCompatibleWithDemo; // Flag indicates using a compatible directory structure with demo released in May 2002.

	A3DTEXTURE_QUALITY m_TextureQuality;

protected:
public:
	A3DConfig();
	~A3DConfig();

	bool Init();
	bool Release();

	bool LoadConfig(char * szConfigFile);

	static A3DRUNENV GetRunEnv() { return m_nRunEnv; }
	static void SetRunEnv(A3DRUNENV env) { m_nRunEnv = env; }

	inline bool GetFlagDetailScene() { return m_bFlagDetailScene; }
	inline void SetFlagDetailScene(bool bFlag) { m_bFlagDetailScene = bFlag; }

	inline bool GetFlagDetailModel() { return m_bFlagDetailModel; }
	inline void SetFlagDetailModel(bool bFlag) { m_bFlagDetailModel = bFlag; }

	inline bool GetFlagDetailTerrain() { return m_bFlagDetailTerrain; }
	inline void SetFlagDetailTerrain(bool bFlag) { m_bFlagDetailTerrain = bFlag; }

	inline bool GetFlagLightMapScene() { return m_bFlagLightMapScene; }
	inline void SetFlagLightMapScene(bool bFlag) { m_bFlagLightMapScene = bFlag; }

	inline bool GetFlagDynamicLight() { return m_bFlagDynamicLight; }
	inline void SetFlagDynamicLight(bool bFlag) { m_bFlagDynamicLight = bFlag; }

	inline bool GetFlagHQSky() { return m_bFlagHQSky; }
	inline void SetFlagHQSky(bool bFlag) { m_bFlagHQSky = bFlag; }

	inline bool GetFlagHQTexture() { return m_bFlagHQTexture; }
	inline void SetFlagHQTexture(bool bFlag) { m_bFlagHQTexture = bFlag; }

	inline A3DTEXTURE_QUALITY GetTextureQuality() { return m_TextureQuality; }
	inline void SetTextureQuality(A3DTEXTURE_QUALITY quality) { m_TextureQuality = quality; }

	inline bool GetFlagUseDynamicStream() { return m_bFlagUseDynamicStream; }
	inline void SetFlagUseDynamicStream(bool bFlag)	{ m_bFlagUseDynamicStream = bFlag; }

	inline bool GetFlagTrilinear() { return m_bFlagTrilinear; }
	inline void SetFlagTrilinear(bool bFlag) { m_bFlagTrilinear = bFlag; }

	inline bool GetFlagNoTextures() { return m_bFlagNoTextures; }
	inline void SetFlagNoTextures(bool bFlag) { m_bFlagNoTextures = bFlag; }
};

typedef class A3DConfig * PA3DConfig;
extern A3DConfig * g_pA3DConfig;
#endif