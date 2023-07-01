/*
 * FILE: AMEngine.cpp
 *
 * DESCRIPTION: Interface class for Angelica Media engine
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "AMPI.h"
#include "AMEngine.h"
#include "AMSoundEngine.h"
#include "AMVideoEngine.h"
#include "AMImmWrapper.h"
#include "AMConfig.h"

char AMEngine::m_szWorkDir[MAX_PATH];

AMEngine::AMEngine()
{
	m_hWnd = NULL;
	m_pAMSoundEngine = NULL;
	m_pAMVideoEngine = NULL;
	m_pAMImmWrapper = NULL;
	m_szWorkDir[0] = '\0';

	m_dwTicks = 0;
}

AMEngine::~AMEngine()
{
}

bool AMEngine::Init(HWND hWnd)
{
	m_hWnd = hWnd;

	g_pAMErrLog = new ALog();
	if (NULL == g_pAMErrLog)
		return false;

	if (!g_pAMErrLog->Init("AM.log", "Angelica Media Error Log", false))
		return false;

	g_pAMConfig = new AMConfig();
	if (NULL == g_pAMConfig)
	{
		AMERRLOG(("AMEngine::Init(), Not enough memory!"));
		return false;
	}
	if (!g_pAMConfig->Init())
	{
		AMERRLOG(("AMEngine::Init(), AMConfig::Init() Fail!"));
		return false;
	}

	GetCurrentDirectory(MAX_PATH, m_szWorkDir);
	return true;
}

bool AMEngine::Release()
{
	if (m_pAMVideoEngine)
	{
		m_pAMVideoEngine->Release();
		delete m_pAMVideoEngine;
		m_pAMVideoEngine = NULL;
	}

	if (m_pAMSoundEngine)
	{
		m_pAMSoundEngine->Release();
		delete m_pAMSoundEngine;
		m_pAMSoundEngine = NULL;
	}

	if (m_pAMImmWrapper)
	{
		m_pAMImmWrapper->Release();
		delete m_pAMImmWrapper;
		m_pAMImmWrapper = NULL;
	}

	if (g_pAMConfig)
	{
		g_pAMConfig->Release();
		delete g_pAMConfig;
		g_pAMConfig = NULL;
	}

	if (g_pAMErrLog)
	{
		g_pAMErrLog->Release();
		delete g_pAMErrLog;
		g_pAMErrLog = NULL;
	}

	m_hWnd = NULL;
	return true;
}

bool AMEngine::InitSoundEngine(AMSOUNDDEVFORMAT* pDevFormat, bool bUse3DDevice)
{
	m_pAMSoundEngine = new AMSoundEngine();
	if (NULL == m_pAMSoundEngine)
	{
		AMERRLOG(("AMEngine::InitSoundEngine, Not enough memory!"));
		return false;
	}

	if (!m_pAMSoundEngine->Init(this, m_hWnd, pDevFormat, bUse3DDevice))
	{
		AMERRLOG(("AMEngine::InitSoundEngine, Call AMSoundEngine::Init Fail!"));
		return false;
	}
	return true;
}

bool AMEngine::InitVideoEngine()
{
	m_pAMVideoEngine = new AMVideoEngine();
	if (NULL == m_pAMVideoEngine)
	{
		AMERRLOG(("AMEngine::InitVideoEngine, Not enough memory!"));
		return false;
	}

	if (!m_pAMVideoEngine->Init(this))
	{
		AMERRLOG(("AMEngine::InitVideoEngine, Call AMVideoEngine::Init Fail!"));
		return false;
	}
	return true;
}

//This function is used to convert a relative(maybe absolute) path to a full path;
bool AMEngine::GetFullPath(char* szFullpath, char* szFolderName, char* szFilename)
{
	char* pszRealfile = szFilename;

	if (strlen(szFilename) == 0)
		return false;
	if (strlen(szFilename) > 1 && szFilename[0] == '\\')
		return false;

	//See if it is a absolute path;
	if (strlen(szFilename) > 3)
	{
		if (szFilename[1] == ':' && szFilename[2] == '\\')
		{
			strcpy(szFullpath, szFilename);
			return true;
		}
	}

	// Get rid of prefix .\, so to make a clean relative file path
	if (strlen(szFilename) > 2 && szFilename[0] == '.' && szFilename[1] == '\\')
		pszRealfile = szFilename + 2;

	sprintf(szFullpath, "%s\\%s\\%s", GetWorkDir(), szFolderName, pszRealfile);
	return true;
}

bool AMEngine::GetRelativePath(char* szFullpath, char* szRelativepath)
{
	char* p1, * p2;
	p1 = m_szWorkDir; p2 = szFullpath;
	while (*p1 && *p2 && *p1 == *p2)
	{
		p1++;
		p2++;
	}

	if (*p1) // not found;
	{
		strcpy(szRelativepath, szFullpath);
		return true;
	}

	if (*p2 == '\\')
		p2++;

	strcpy(szRelativepath, p2);
	return true;
}


bool AMEngine::Reset()
{
	if (m_pAMSoundEngine)
	{
		m_pAMSoundEngine->Reset();
	}
	if (m_pAMVideoEngine)
	{
		m_pAMVideoEngine->Reset();
	}
	return true;
}

bool AMEngine::Tick()
{
	if (m_pAMSoundEngine)
	{
		m_pAMSoundEngine->Tick();
	}

	if (m_pAMVideoEngine)
	{
		m_pAMVideoEngine->Tick();
	}

	m_dwTicks++;

	return true;
}

bool AMEngine::InitImmWrapper(char* szProjectName)
{
	m_pAMImmWrapper = new AMImmWrapper();
	if (NULL == m_pAMImmWrapper)
	{
		AMERRLOG(("AMEngine::InitImmWrapper(), Not enough memory!"));
		return false;
	}

	if (!m_pAMImmWrapper->Init(NULL, m_hWnd, szProjectName, ALog::GetLogDir()))
	{
		AMERRLOG(("AMEngine::InitImmWrapper(), Init AMImmWrapper Object fail!"));
		return false;
	}

	return true;
}
