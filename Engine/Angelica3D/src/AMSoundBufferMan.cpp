/*
 * FILE: AMSoundBufferMan.cpp
 *
 * DESCRIPTION: sound buffer manager class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */
#include "AMSoundBufferMan.h"
#include "AMEngine.h"
#include "AMSoundEngine.h"
#include "AMSoundBuffer.h"
#include "AMPI.h"
#include "AMConfig.h"
#include <assert.h>

//#define AMSOUNDBUFFERMAN_CACHE_ENABLE

AMSoundBufferMan::AMSoundBufferMan()
{
	m_nRecordNum = 0;
	strcpy(m_szFolderName, "Sounds");

	m_nSoundFilePlaying = 0;
	m_nSoundBufferLoaded = 0;
}

AMSoundBufferMan::~AMSoundBufferMan()
{
}

bool AMSoundBufferMan::Init(PAMSoundEngine pAMSoundEngine)
{
	m_pAMSoundEngine = pAMSoundEngine;

	if( g_pAMConfig->GetRunEnv() == AMRUNENV_PURESERVER )
		return true;

#ifdef AMSOUNDBUFFERMAN_CACHE_ENABLE
	// Do the preload work;
	AFileImage soundsListFile;
	if( soundsListFile.Open(m_szFolderName, "Sounds.lst", AFILE_OPENEXIST) )
	{
		char	szLineBuffer[AFILE_LINEMAXLEN];
		DWORD	dwRead;

		while( soundsListFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwRead) )
		{
			if( strlen(szLineBuffer) == 0 )
				continue;

			if( !PreloadFile(szLineBuffer) )
			{
				AMERRLOG(("AM3DSoundBufferMan::Init(), Can not preload sfx file [%s]", szLineBuffer));
				continue;
			}
		}
		soundsListFile.Close();
	}
	else
	{
		// Warning message;
		AMERRLOG(("AMSoundBufferMan::Init(), Can not located the sound list file, preload work can not continued!"));
	}
#endif

	return true;
}

bool AMSoundBufferMan::Release()
{
	// We should release the preloaded resource here;
	for(int i=0; i<m_nRecordNum; i++)
	{
		if( m_pRecords[i].pAFileImage )
		{
			m_pRecords[i].pAFileImage->Close();
			delete m_pRecords[i].pAFileImage;
		}
	}
	m_nRecordNum = 0;

	// We should release all sound buffers played by PlaySoundFile
	for(int i=0; i<m_nSoundFilePlaying; i++)
	{
		// Release this buffer;
		m_pSoundFilePlaying[i].pSoundBuffer->Stop();
		ReleaseSoundBuffer(m_pSoundFilePlaying[i].pSoundBuffer);
	}
	m_nSoundFilePlaying = 0;

	// We should release all not released sound buffers;
	for(int i=0; i<m_nSoundBufferLoaded; i++)
	{
		// Release current loaded buffer;
		m_pSoundBufferLoaded[i]->Stop();
		ReleaseSoundBuffer(m_pSoundBufferLoaded[i]);
	}
	m_nSoundBufferLoaded = 0;
	return true;
}

bool AMSoundBufferMan::LoadSoundBufferFromFile(char * szFilename, PAMSoundBuffer * ppAMSoundBuffer)
{
	if( g_pAMConfig->GetRunEnv() == AMRUNENV_PURESERVER )
	{
		// Create a dummy sound buffer object here
		AMSoundBuffer * pNewSoundBuffer = new AMSoundBuffer();
		if( NULL == pNewSoundBuffer )
		{
			AMERRLOG(("AMSoundBufferMan::LoadSoundBufferFromFile, Not enough memory!"));
			return false;
		}
		
		if( !pNewSoundBuffer->Load(m_pAMSoundEngine, NULL, szFilename) )
		{
			AMERRLOG(("AMSoundBufferMan::LoadSoundBufferFromFile, The file [%s] loaded failure!"));
			return false;
		}

		*ppAMSoundBuffer = pNewSoundBuffer;
		return true;
	}

	AFileImage * pAFileImage;

#ifdef AMSOUNDBUFFERMAN_CACHE_ENABLE
	pAFileImage = FindSoundFileImage(szFilename);
	if( !pAFileImage )
	{
		pAFileImage = PreloadFile(szFilename);
		if( !pAFileImage )
		{
			AMERRLOG(("AMSoundBufferMan::LoadSoundBufferFromFile, Can't load file [%s]", szFilename));
			return false;
		}
	}
#else
	pAFileImage = PreloadFile(szFilename);
	if( !pAFileImage )
	{
		AMERRLOG(("AMSoundBufferMan::LoadSoundBufferFromFile, Can't load file [%s]", szFilename));
		return false;
	}
#endif

	// Now we have to load it from the disk;
	AMSoundBuffer * pNewSoundBuffer = new AMSoundBuffer();
	if( NULL == pNewSoundBuffer )
	{
		AMERRLOG(("AMSoundBufferMan::LoadSoundBufferFromFile, Not enough memory!"));
		return false;
	}
	
	pAFileImage->ResetPointer();
	
	if( !pNewSoundBuffer->Load(m_pAMSoundEngine, pAFileImage, szFilename) )
	{
		AMERRLOG(("AMSoundBufferMan::LoadSoundBufferFromFile, The file [%s] loaded failure!"));
		return false;
	}

#ifndef AMSOUNDBUFFERMAN_CACHE_ENABLE
	pAFileImage->Close();
	delete pAFileImage;
#endif

	*ppAMSoundBuffer = pNewSoundBuffer;
	if( m_nSoundBufferLoaded >= MAX_SOUNDBUFFERLOADED )
	{
		// Warning message;
		AMERRLOG(("AMSoundBufferMan::LoadSoundBufferFromFile(), Too many sound buffer loaded!"));
	}
	else
	{
		m_pSoundBufferLoaded[m_nSoundBufferLoaded] = pNewSoundBuffer;
		++ m_nSoundBufferLoaded;
	}
	return true;
}

AFileImage * AMSoundBufferMan::FindSoundFileImage(char * szFilename)
{
	for(int i=0; i<m_nRecordNum; i++)
	{
		if( _stricmp(szFilename, m_pRecords[i].szFilename) == 0 )
		{
			return m_pRecords[i].pAFileImage;
		}
	}
	return NULL;
}

bool AMSoundBufferMan::ReleaseSoundBuffer(PAMSoundBuffer& pAMSoundBuffer)
{
	bool bFound = false;

	if( g_pAMConfig->GetRunEnv() == AMRUNENV_PURESERVER )
		bFound = true;
	else
	{
		// First we have to find out the buffer in the current loaded buffer list;
		for(int i=0; i<m_nSoundBufferLoaded; i++)
		{
			if( m_pSoundBufferLoaded[i] == pAMSoundBuffer )
			{
				// Swap this one with the last one;
				if( i == m_nSoundBufferLoaded - 1 )
					m_nSoundBufferLoaded --;
				else
				{
					m_pSoundBufferLoaded[i] = m_pSoundBufferLoaded[m_nSoundBufferLoaded - 1];
					m_nSoundBufferLoaded --;
				}
				bFound = true;
				break;
			}
		}
	}

	if( bFound )
	{
		// Maybe we should check if the sound buffer in wave file list is being released;
		pAMSoundBuffer->Release();
		delete pAMSoundBuffer;
		pAMSoundBuffer = NULL;
	}
	return true;
}

bool AMSoundBufferMan::Reset()
{
	for(int i=0; i<m_nRecordNum; i++)
	{
		if( m_pRecords[i].pAFileImage )
		{
			m_pRecords[i].pAFileImage->Close();
			delete m_pRecords[i].pAFileImage;
		}
	}
	m_nRecordNum = 0;

	for(int i=0; i<m_nSoundFilePlaying; i++)
	{
		// Release this buffer;
		m_pSoundFilePlaying[i].pSoundBuffer->Stop();
		ReleaseSoundBuffer(m_pSoundFilePlaying[i].pSoundBuffer);
	}
	m_nSoundFilePlaying = 0;

	for(int i=0; i<m_nSoundBufferLoaded; i++)
	{
		// Release current loaded buffer;
		m_pSoundBufferLoaded[i]->Stop();
		ReleaseSoundBuffer(m_pSoundBufferLoaded[i]);
	}
	m_nSoundBufferLoaded = 0;
	return true;
}

bool AMSoundBufferMan::PlaySoundFile(char * szSoundFile, int nRepeat, bool bUseImmEffect, PAMSoundBuffer * ppSoundBuffer)
{
	if( ppSoundBuffer )
		*ppSoundBuffer = NULL;

	// If we did not create the direct sound object or there are too many sounds playing now;
	if( NULL == m_pAMSoundEngine->GetDS() || m_nSoundFilePlaying >= MAX_SOUNDFILEPLAYING )
		return true;

	bool			bval;
	AMSoundBuffer	* pAMSoundBuffer;

	bval = LoadSoundBufferFromFile(szSoundFile, &pAMSoundBuffer);
	if( !bval )
	{
		AMERRLOG(("AMSoundBufferMan::PlaySoundFile(), LoadSoundBufferFromFile [%s] fail!", szSoundFile));
		return false;
	}

	if( bUseImmEffect )
		pAMSoundBuffer->LoadImmEffect();

	pAMSoundBuffer->Play(false);

	m_pSoundFilePlaying[m_nSoundFilePlaying].pSoundBuffer = pAMSoundBuffer;
	m_pSoundFilePlaying[m_nSoundFilePlaying].nRepeatCount = nRepeat;
	m_pSoundFilePlaying[m_nSoundFilePlaying].nRepeatTimes = 0;

	m_nSoundFilePlaying ++;

	if( ppSoundBuffer )
		*ppSoundBuffer = pAMSoundBuffer;
	return true;
}

bool AMSoundBufferMan::Tick()
{
	int					i=0;

	while( i < m_nSoundFilePlaying )
	{
		if( !m_pSoundFilePlaying[i].pSoundBuffer->CheckEnd() )
		{
			AMERRLOG(("AMSoundBufferMan::Tick(), Error check one buffer's end!"));
			return false;
		}

		if( m_pSoundFilePlaying[i].pSoundBuffer->IsStopped() )
		{
			// Never need this buffer, but we should use garbage collector instead of realsing it;
			m_pSoundFilePlaying[i].nRepeatTimes ++;

			if( m_pSoundFilePlaying[i].nRepeatCount == 0 || m_pSoundFilePlaying[i].nRepeatTimes < m_pSoundFilePlaying[i].nRepeatCount )
			{
				m_pSoundFilePlaying[i].pSoundBuffer->Play(false);
			}
			else
			{
				// Release this buffer;
				ReleaseSoundBuffer(m_pSoundFilePlaying[i].pSoundBuffer);
				m_pSoundFilePlaying[i] = m_pSoundFilePlaying[--m_nSoundFilePlaying];
				continue;
			}
		}

		i ++;
	}

	return true;
}

AFileImage * AMSoundBufferMan::PreloadFile(char * szFilename)
{
	// If too many .wav files has been loaded into memory!
	if( m_nRecordNum >= MAX_SOUNDFILE )
		return false;

	AFileImage * pAFileImage;
	
	pAFileImage = new AFileImage();
	if( NULL == pAFileImage )
	{
		AMERRLOG(("AMSoundBufferMan::PreloadFile(), Not enough memory!"));
		return false;
	}

	if( !pAFileImage->Open(m_szFolderName, szFilename, AFILE_OPENEXIST) )
	{
		AMERRLOG(("AMSoundBufferMan::PreloadFile(), Can't load file image [%s]", szFilename));
		return false;
	}

#ifdef AMSOUNDBUFFER_CACHE_ENABLE
	//Now record this new sound buffer;
	strncpy(m_pRecords[m_nRecordNum].szFilename, szFilename, MAX_PATH);
	m_pRecords[m_nRecordNum].pAFileImage = pAFileImage;
	m_nRecordNum ++;
#endif

	return pAFileImage;
}

bool AMSoundBufferMan::SafeCloseSoundBuffer(AMSoundBuffer * pSoundBuffer)
{
	int					i=0;

	while( i < m_nSoundFilePlaying )
	{
		if( m_pSoundFilePlaying[i].pSoundBuffer == pSoundBuffer )
		{
			pSoundBuffer->Stop();
			ReleaseSoundBuffer(m_pSoundFilePlaying[i].pSoundBuffer);
			m_pSoundFilePlaying[i] = m_pSoundFilePlaying[--m_nSoundFilePlaying];
			return true;
		}

		i ++;
	}
		
	return true;
}

bool AMSoundBufferMan::SetVolume(DWORD dwVolume)
{
	for(int i=0; i<m_nSoundBufferLoaded; i++)
		m_pSoundBufferLoaded[i]->SetVolume(dwVolume);

	return true;
}