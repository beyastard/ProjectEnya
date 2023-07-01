/*
 * FILE: AM3DSoundBufferMan.cpp
 *
 * DESCRIPTION: 3D sound buffer manager class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */
#include "AM3DSoundBufferMan.h"
#include "AMEngine.h"
#include "AMSoundEngine.h"
#include "AM3DSoundDevice.h"
#include "AM3DSoundBuffer.h"
#include "AMPI.h"
#include "AMConfig.h"

#include <assert.h>

AM3DSoundBufferMan::AM3DSoundBufferMan()
{
	m_nRecordNum = 0;
	strcpy(m_szFolderName, "Sfx");

	m_n3DSoundFilePlaying = 0;
	m_n3DSoundBufferLoaded = 0;
}

AM3DSoundBufferMan::~AM3DSoundBufferMan()
{
}

bool AM3DSoundBufferMan::Init(PAMSoundEngine pAMSoundEngine)
{
	m_pAMSoundEngine = pAMSoundEngine;

	if( g_pAMConfig->GetRunEnv() == AMRUNENV_PURESERVER )
		return true;

	return true;
	// Do the preload work;
	AFileImage sfxListFile;
	if( sfxListFile.Open(m_szFolderName, "Sfx.lst", AFILE_OPENEXIST) )
	{
		char	szLineBuffer[AFILE_LINEMAXLEN];
		DWORD	dwRead;

		while( sfxListFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwRead) )
		{
			if( strlen(szLineBuffer) == 0 )
				continue;

			if( !PreloadFile(szLineBuffer) )
			{
				AMERRLOG(("AM3DSoundBufferMan::Init(), Can not preload sfx file [%s]", szLineBuffer));
				continue;
			}
		}
		sfxListFile.Close();
	}
	else
	{
		// Warning message;
		AMERRLOG(("AM3DSoundBufferMan::Init(), Can not located the sfx list file, preload work can not continued!"));
	}
	return true;
}

bool AM3DSoundBufferMan::Release()
{
	int i;

	// We should release the preloaded resource here;
	for(i=0; i<m_nRecordNum; i++)
	{
		if( m_pRecords[i].pAFileImage )
		{
			m_pRecords[i].pAFileImage->Close();
			delete m_pRecords[i].pAFileImage;
		}
	}
	m_nRecordNum = 0;

	// We should release all 3D sound buffer played by Play3DSoundFile here;
	for(i=0; i<m_n3DSoundFilePlaying; i++)
	{
		// Release this buffer;
		m_p3DSoundFilePlaying[i].p3DSoundBuffer->Stop();
		Release3DSoundBuffer(m_p3DSoundFilePlaying[i].p3DSoundBuffer);
	}
	m_n3DSoundFilePlaying = 0;

	// We should release all unreleased 3d sound buffers;
	for(i=0; i<m_n3DSoundBufferLoaded; i++)
	{
		m_p3DSoundBufferLoaded[i]->Stop();
		Release3DSoundBuffer(m_p3DSoundBufferLoaded[i]);
	}
	m_n3DSoundBufferLoaded = 0;
	return true;
}

bool AM3DSoundBufferMan::Load3DSoundBufferFromFile(char * szFilename, PAM3DSoundBuffer * ppAM3DSoundBuffer)
{
	if( g_pAMConfig->GetRunEnv() == AMRUNENV_PURESERVER )
	{
		// Now we have to create a dummy 3d soundbuffer object
		AM3DSoundBuffer * pNew3DSoundBuffer = new AM3DSoundBuffer();
		if( NULL == pNew3DSoundBuffer )
		{
			AMERRLOG(("AM3DSoundBufferMan::Load3DSoundBufferFromFile, Not enough memory!"));
			return false;
		}
		
		if( !pNew3DSoundBuffer->Load(m_pAMSoundEngine, NULL, szFilename) )
		{
			AMERRLOG(("AM3DSoundBufferMan::Load3DSoundBufferFromFile, The file [%s] loaded failure!", szFilename));
			return false;
		}

		*ppAM3DSoundBuffer = pNew3DSoundBuffer;
		return true;
	}

	AFileImage * pAFileImage;

	pAFileImage = Find3DSoundFileImage(szFilename);
	if( !pAFileImage )
	{
		pAFileImage = PreloadFile(szFilename);
		if( !pAFileImage )
		{
			AMERRLOG(("AM3DSoundBufferMan::Load3DSoundBufferFromFile, Can't load file [%s]", szFilename));
			return false;
		}
	}

	// Now we have to load it from the disk;
	AM3DSoundBuffer * pNew3DSoundBuffer = new AM3DSoundBuffer();
	if( NULL == pNew3DSoundBuffer )
	{
		AMERRLOG(("AM3DSoundBufferMan::Load3DSoundBufferFromFile, Not enough memory!"));
		return false;
	}
	
	pAFileImage->ResetPointer();

	if( !pNew3DSoundBuffer->Load(m_pAMSoundEngine, pAFileImage, szFilename) )
	{
		AMERRLOG(("AM3DSoundBufferMan::Load3DSoundBufferFromFile, The file [%s] loaded failure!", szFilename));
		return false;
	}

	*ppAM3DSoundBuffer = pNew3DSoundBuffer;

	if( m_n3DSoundBufferLoaded >= MAX_3DSOUNDBUFFERLOADED )
	{
		// Warning message;
		AMERRLOG(("AM3DSoundBufferMan::Load3DSoundBufferFromFile(), Too many sound buffer loaded!"));
	}
	else
	{
		m_p3DSoundBufferLoaded[m_n3DSoundBufferLoaded] = pNew3DSoundBuffer;
		++ m_n3DSoundBufferLoaded;
	}
	return true;
}

AFileImage * AM3DSoundBufferMan::Find3DSoundFileImage(char * szFilename)
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

bool AM3DSoundBufferMan::Release3DSoundBuffer(PAM3DSoundBuffer& pAM3DSoundBuffer)
{
	bool bFound = false;

	if( g_pAMConfig->GetRunEnv() == AMRUNENV_PURESERVER )
		bFound = true;
	else
	{
		// First we have to find out the buffer in the current loaded buffer list;
		for(int i=0; i<m_n3DSoundBufferLoaded; i++)
		{
			if( m_p3DSoundBufferLoaded[i] == pAM3DSoundBuffer )
			{
				// Swap this one with the last one;
				if( i == m_n3DSoundBufferLoaded - 1 )
					m_n3DSoundBufferLoaded --;
				else
				{
					m_p3DSoundBufferLoaded[i] = m_p3DSoundBufferLoaded[m_n3DSoundBufferLoaded - 1];
					m_n3DSoundBufferLoaded --;
				}
				bFound = true;
				break;
			}
		}
	}

	if( bFound )
	{
		pAM3DSoundBuffer->Release();
		delete pAM3DSoundBuffer;
		pAM3DSoundBuffer = NULL;
	}
	return true;
}

bool AM3DSoundBufferMan::Reset()
{	
	int i;
	for(i=0; i<m_nRecordNum; i++)
	{
		if( m_pRecords[i].pAFileImage )
		{
			m_pRecords[i].pAFileImage->Close();
			delete m_pRecords[i].pAFileImage;
		}
	}
	m_nRecordNum = 0;

	for(i=0; i<m_n3DSoundFilePlaying; i++)
	{
		// Release this buffer;
		m_p3DSoundFilePlaying[i].p3DSoundBuffer->Stop();
		Release3DSoundBuffer(m_p3DSoundFilePlaying[i].p3DSoundBuffer);
	}
	m_n3DSoundFilePlaying = 0;

	// We should release all unreleased 3d sound buffers;
	for(i=0; i<m_n3DSoundBufferLoaded; i++)
	{
		m_p3DSoundBufferLoaded[i]->Stop();
		Release3DSoundBuffer(m_p3DSoundBufferLoaded[i]);
	}
	m_n3DSoundBufferLoaded = 0;
	return true;
}

bool AM3DSoundBufferMan::Play3DSoundFile(char * sz3DSoundFile, A3DVECTOR3 vecPos, A3DVECTOR3 vecVelocity, FLOAT vMinDistance, FLOAT vMaxDistance, int nRepeat, bool bUseImmEffect, PAM3DSoundBuffer * pp3DSoundBuffer, bool bForce2D)
{
	if( pp3DSoundBuffer )
		*pp3DSoundBuffer = NULL;

	// If we did not create the direct sound object or too many sounds are playing now;
	if( NULL == m_pAMSoundEngine->GetDS() || m_n3DSoundFilePlaying >= MAX_3DSOUNDFILEPLAYING )
		return true;

	if( !bForce2D )
	{
		FLOAT vDisToEar;
		vDisToEar = Magnitude(vecPos - m_pAMSoundEngine->GetAM3DSoundDevice()->GetPosition());
		if( vMaxDistance < vDisToEar )
			return true;
	}

	bool			bval;
	AM3DSoundBuffer	* pAM3DSoundBuffer;

	bval = Load3DSoundBufferFromFile(sz3DSoundFile, &pAM3DSoundBuffer);
	if( !bval )
	{
		AMERRLOG(("AM3DSoundBufferMan::Play3DSoundFile(), Load3DSoundBufferFromFile [%s] fail!", sz3DSoundFile));
		return false;
	}

	if( bUseImmEffect )
		pAM3DSoundBuffer->LoadImmEffect();

	pAM3DSoundBuffer->SetMinDistance(vMinDistance);
	pAM3DSoundBuffer->SetMaxDistance(vMaxDistance);
	pAM3DSoundBuffer->SetPosition(vecPos);
	pAM3DSoundBuffer->SetVelocity(vecVelocity);
	pAM3DSoundBuffer->UpdateChanges();
	pAM3DSoundBuffer->SetForce2D(bForce2D);

	pAM3DSoundBuffer->Play(false);

	m_p3DSoundFilePlaying[m_n3DSoundFilePlaying].p3DSoundBuffer = pAM3DSoundBuffer;
	m_p3DSoundFilePlaying[m_n3DSoundFilePlaying].nRepeatCount   = nRepeat;
	m_p3DSoundFilePlaying[m_n3DSoundFilePlaying].nRepeatTimes   = 0;

	m_n3DSoundFilePlaying ++;
	if( pp3DSoundBuffer )
		*pp3DSoundBuffer = pAM3DSoundBuffer;
	return true;
}

bool AM3DSoundBufferMan::Tick()
{
	int					i=0;

	while( i < m_n3DSoundFilePlaying )
	{
		if( !m_p3DSoundFilePlaying[i].p3DSoundBuffer->CheckEnd() )
		{
			AMERRLOG(("AM3DSoundBufferMan::Tick(), Error check one buffer's end!"));
			return false;
		}

		if( m_p3DSoundFilePlaying[i].p3DSoundBuffer->IsStopped() )
		{
			// Never need this buffer, but we should use garbage collector instead of realsing it;
			m_p3DSoundFilePlaying[i].nRepeatTimes ++;

			if( m_p3DSoundFilePlaying[i].nRepeatCount == 0 || m_p3DSoundFilePlaying[i].nRepeatTimes < m_p3DSoundFilePlaying[i].nRepeatCount )
			{
				m_p3DSoundFilePlaying[i].p3DSoundBuffer->Play(false);
			}
			else
			{
				// Release this buffer;
				Release3DSoundBuffer(m_p3DSoundFilePlaying[i].p3DSoundBuffer);
				m_p3DSoundFilePlaying[i] = m_p3DSoundFilePlaying[--m_n3DSoundFilePlaying];
				continue;
			}
		}

		i ++;
	}

	return true;
}

AFileImage * AM3DSoundBufferMan::PreloadFile(char * szFilename)
{
	// If too many .wav files has been loaded into memory!
	if( m_nRecordNum >= MAX_3DSOUNDFILE )
		return NULL;

	AFileImage * pAFileImage;
	
	// Now load the file image and add it to records;
	pAFileImage = new AFileImage();
	if( NULL == pAFileImage )
	{
		AMERRLOG(("AM3DSoundBufferMan::PreloadFile(), Not enough memory!"));
		return false;
	}

	if( !pAFileImage->Open(m_szFolderName, szFilename, AFILE_OPENEXIST) )
	{
		AMERRLOG(("AM3DSoundBufferMan::PreloadFile(), Can't load file image [%s]", szFilename));
		return false;
	}

	//Now record this new sound buffer;
	strncpy(m_pRecords[m_nRecordNum].szFilename, szFilename, MAX_PATH);
	m_pRecords[m_nRecordNum].pAFileImage = pAFileImage;
	m_nRecordNum ++;

	return pAFileImage;
}

bool AM3DSoundBufferMan::SafeClose3DSoundBuffer(AM3DSoundBuffer * p3DSoundBuffer)
{
	int					i=0;

	while( i < m_n3DSoundFilePlaying )
	{
		if( m_p3DSoundFilePlaying[i].p3DSoundBuffer == p3DSoundBuffer )
		{
			p3DSoundBuffer->Stop();
			Release3DSoundBuffer(m_p3DSoundFilePlaying[i].p3DSoundBuffer);
			m_p3DSoundFilePlaying[i] = m_p3DSoundFilePlaying[--m_n3DSoundFilePlaying];
			return true;
		}

		i ++;
	}

	return true;
}

bool AM3DSoundBufferMan::SetVolume(DWORD dwVolume)
{
	for(int i=0; i<m_n3DSoundBufferLoaded; i++)
		m_p3DSoundBufferLoaded[i]->SetVolume(dwVolume);

	return true;
}