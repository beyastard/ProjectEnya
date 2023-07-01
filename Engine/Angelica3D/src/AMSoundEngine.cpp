/*
 * FILE: AMSoundEngine.cpp
 *
 * DESCRIPTION: The class standing for the sound engine of Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */
#include "AMPI.h"
#include "AMSoundEngine.h"
#include "AMSoundDevice.h"
#include "AM3DSoundDevice.h"
#include "AMSoundBufferMan.h"
#include "AM3DSoundBufferMan.h"
#include "AMConfig.h"

AMSoundEngine::AMSoundEngine()
{
	m_hWnd				= NULL;
	m_pDS				= NULL;

	m_pAMEngine			= NULL;
	m_pAMSoundDevice	= NULL;
	m_pAM3DSoundDevice	= NULL;
	m_dwVolume			= 100;
}

AMSoundEngine::~AMSoundEngine()
{
}

bool AMSoundEngine::Init(PAMEngine pAMEngine, HWND hWnd, AMSOUNDDEVFORMAT * pDevFormat, bool bUse3DDevice)
{
	HRESULT		hval;

	m_pAMEngine = pAMEngine;
	m_hWnd = hWnd;

	if( g_pAMConfig->GetRunEnv() == AMRUNENV_PURESERVER )
	{
		AMERRLOG(("AMSoundEngine::Init(), Init as a pureserver angelica media engine!"));
		m_pDS = NULL;
	}
	else
	{
		hval = DirectSoundCreate8(NULL, &m_pDS, NULL);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundEngine::Init, Call DirectSoundCreate8 Fail!"));
			m_pDS = NULL;
			// We will still create the sound engine, and any function will still can be called, but has not effect;
			//return false;
		}
	}

	if( m_pDS )
	{
		DWORD dwLevel;

		switch( pDevFormat->soundCL )
		{
		case AMSCL_EXCLUSIVE:
			dwLevel = DSSCL_EXCLUSIVE;
			break;

		case AMSCL_NORMAL:
			dwLevel = DSSCL_NORMAL;
			break;

		case AMSCL_PRIORITY:
			dwLevel = DSSCL_PRIORITY;
			break;

		case AMSCL_WRITEPRIMARY:
			dwLevel = DSSCL_WRITEPRIMARY;
			break;
		}

		hval = m_pDS->SetCooperativeLevel(m_hWnd, dwLevel);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundEngine::Init, SetCooperativeLevel Fail"));
			return false;
		}
	}

	if( bUse3DDevice )
	{
		m_pAM3DSoundDevice = new AM3DSoundDevice();
		if( NULL == m_pAM3DSoundDevice )
		{
			AMERRLOG(("AMSoundEngine::Init, Not enough memory!"));
			return false;
		}

		if( !m_pAM3DSoundDevice->Init(this, pDevFormat) )
		{
			AMERRLOG(("AMSoundEngine::Init, 3D Sound Device Init Fail!"));
			return false;
		}
	}
	else
	{
		m_pAMSoundDevice = new AMSoundDevice();
		if( NULL == m_pAMSoundDevice )
		{
			AMERRLOG(("AMSoundEngine::Init, Not enough memory!"));
			return false;
		}

		if( !m_pAMSoundDevice->Init(this, pDevFormat) )
		{
			AMERRLOG(("AMSoundEngine::Init, Sound Device Init Fail!"));
			return false;
		}
	}

	m_pAMSoundBufferMan = new AMSoundBufferMan();
	if( NULL == m_pAMSoundBufferMan )
	{
		AMERRLOG(("AMSoundEngine, Not enough memory!"));
		return false;
	}

	if( !m_pAMSoundBufferMan->Init(this) )
	{
		AMERRLOG(("AMSoundEngine, Init SoundBufferMan Fail!"));
		return false;
	}

	m_pAM3DSoundBufferMan = new AM3DSoundBufferMan();
	if( NULL == m_pAM3DSoundBufferMan )
	{
		AMERRLOG(("AMSoundEngine, Not enough memory!"));
		return false;
	}

	if( !m_pAM3DSoundBufferMan->Init(this) )
	{
		AMERRLOG(("AMSoundEngine, Init 3DSoundBufferMan Fail!"));
		return false;
	}
	return true;
}

bool AMSoundEngine::Release()
{
	if( m_pAM3DSoundBufferMan )
	{
		m_pAM3DSoundBufferMan->Release();
		delete m_pAM3DSoundBufferMan;
		m_pAM3DSoundBufferMan = NULL;
	}

	if( m_pAMSoundBufferMan )
	{
		m_pAMSoundBufferMan->Release();
		delete m_pAMSoundBufferMan;
		m_pAMSoundBufferMan = NULL;
	}
	
	if( m_pAMSoundDevice )
	{
		m_pAMSoundDevice->Release();
		delete m_pAMSoundDevice;
		m_pAMSoundDevice = NULL;
	}

	if( m_pAM3DSoundDevice )
	{
		m_pAM3DSoundDevice->Release();
		delete m_pAM3DSoundDevice;
		m_pAM3DSoundDevice = NULL;
	}

	if( m_pDS )
	{
		m_pDS->Release();
		m_pDS = NULL;
	}
	return true;
}

bool AMSoundEngine::Reset()
{
	if( m_pAMSoundBufferMan )
	{
		m_pAMSoundBufferMan->Reset();
	}

	if( m_pAM3DSoundBufferMan )
	{
		m_pAM3DSoundBufferMan->Reset();
	}
	return true;
}

bool AMSoundEngine::Tick()
{
	if( m_pAMSoundBufferMan )
	{
		m_pAMSoundBufferMan->Tick();
	}

	if( m_pAM3DSoundBufferMan )
	{
		m_pAM3DSoundBufferMan->Tick();
	}

	return true;
}

bool AMSoundEngine::SetVolume(DWORD dwVolume)
{
	m_dwVolume = dwVolume;
	m_pAMSoundBufferMan->SetVolume(dwVolume);
	m_pAM3DSoundBufferMan->SetVolume(dwVolume);
	return true;
}

int AMSoundEngine::GetLoaded2DSoundNum()
{ 
	return m_pAMSoundBufferMan->GetLoadedSoundBufferNum(); 
}

int AMSoundEngine::GetLoaded3DSoundNum()
{
	return m_pAM3DSoundBufferMan->GetLoaded3DSoundBufferNum(); 
}
