/*
 * FILE: AMEngine.h
 *
 * DESCRIPTION: Interface class for Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMENGINE_H_
#define _AMENGINE_H_

#include "AMTypes.h"
#include "AMPlatform.h"

typedef class AMSoundEngine * PAMSoundEngine;
typedef class AMVideoEngine * PAMVideoEngine;
typedef class AMImmWrapper  * PAMImmWrapper;

class AMEngine
{
private:
	static char				m_szWorkDir[MAX_PATH];
	HWND					m_hWnd;
	DWORD					m_dwTicks;

	PAMSoundEngine			m_pAMSoundEngine;
	PAMVideoEngine			m_pAMVideoEngine;

	// Wrapper class for ifc
	PAMImmWrapper			m_pAMImmWrapper;

protected:
public:
	AMEngine();
	~AMEngine();

	bool Init(HWND hWnd);
	bool Release();

	bool Tick();

	bool Reset();

	bool InitSoundEngine(AMSOUNDDEVFORMAT * pDevFormat, bool bUse3DDevice); 
	bool InitVideoEngine(); // Must cooperate with sound engine;
	bool InitMusicEngine(); // Maybe derived class from sound engine;
	bool InitImmWrapper(char * szProjectName);

	static bool GetFullPath(char * szFullpath, char * szFolderName, char * szFilename);
	static inline char * GetWorkDir() { return m_szWorkDir; }
	static bool GetRelativePath(char * szFullpath, char * szRelativepath);

	inline void SetWorkDir(char * szWorkDir) { strncpy(m_szWorkDir, szWorkDir, MAX_PATH); }
	inline AMSoundEngine * GetAMSoundEngine() { return m_pAMSoundEngine; }
	inline AMVideoEngine * GetAMVideoEngine() { return m_pAMVideoEngine; }
	inline AMImmWrapper * GetAMImmWrapper() { return m_pAMImmWrapper; }
	inline DWORD GetTicks() { return m_dwTicks; }
};

typedef class AMEngine * PAMEngine;
#endif//_AMENGINE
