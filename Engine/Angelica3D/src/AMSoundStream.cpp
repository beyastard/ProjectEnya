/*
 * FILE: AMSoundStream.cpp
 *
 * DESCRIPTION: sound stream class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "AMPI.h"
#include "AMSoundEngine.h"
#include "AMSoundStream.h"
#include "AMWaveFile.h"
#include "AMMp3File.h"

CRITICAL_SECTION	l_csExit;
bool				l_bQuit;
							
AMSoundStream::AMSoundStream()
{
	m_pAMSoundEngine	= NULL;
	m_pDSBuffer			= NULL;
	m_pDSNotify			= NULL;
	m_pWaveFile			= NULL;
	m_pMp3File			= NULL;
	m_pAFile			= NULL;
	m_fileType			= AMS_UNKOWNFILE;
	m_state				= AMSS_STOPPED;

	m_bLoop				= true; // By default we must loop again and again;
	m_nGapToInsert		= 3000;
	m_nGapToWait		= 0;

	m_bHasInit			= false;
	ZeroMemory(&m_streamDesc, sizeof(m_streamDesc));
	ZeroMemory(m_hEvent, sizeof(HANDLE) * AMSS_MAX_BUFFER_NUM);
	m_szStreamFile[0]		= '\0';

	m_dwVolume				= 100;
}

AMSoundStream::~AMSoundStream()
{
}

// Create the sound buffer;
bool AMSoundStream::Init(PAMSoundEngine pAMSoundEngine, char * pszStreamFile, AMSSTREAMDESC * pDesc)
{
	m_pAMSoundEngine = pAMSoundEngine;
	if( pDesc )
	{
		m_streamDesc.dwBufferLen = pDesc->dwBufferLen;
		m_streamDesc.dwBufferNum = pDesc->dwBufferNum;
	}
	else
	{
		m_streamDesc.dwBufferLen = 60;
		m_streamDesc.dwBufferNum = 10;
	}
	m_nBufferLen = m_streamDesc.dwBufferLen; //ms
	m_nBufferNum = m_streamDesc.dwBufferNum;
	if( m_nBufferNum > AMSS_MAX_BUFFER_NUM )
	{
		AMERRLOG(("AMSoundStream::Init(), buffer number must be less than %d", AMSS_MAX_BUFFER_NUM));
		return false;
	}

	int					i;
	for(i=0; i<m_nBufferNum; i++)
	{
		m_hEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		ResetEvent(m_hEvent[i]);
	}

	InitializeCriticalSection(&l_csExit);

	// If we did not create direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
	{
		// Set init flag to false, and set state to playing, so this will be playing for ever;
		m_bHasInit = false;
		m_state = AMSS_PLAYING;
		return true;
	}

	if( pszStreamFile )
	{
		if( !InitFromFile(pszStreamFile) )
		{
			AMERRLOG(("AMSoundStream::Init(), InitFromFile fail!"));
			return false;
		}
	}

	return true;
}

bool AMSoundStream::Release()
{
	ReleaseResource();

	DeleteCriticalSection(&l_csExit);
	for(int i=0; i<m_nBufferNum; i++)
	{
		if( m_hEvent[i] )
		{
			CloseHandle(m_hEvent[i]);
			m_hEvent[i] = NULL;
		}
	}
	return true;
}

bool AMSoundStream::Play(char * pszStreamFile)
{
	if( pszStreamFile && 0 != _stricmp(m_szStreamFile, pszStreamFile) )
	{
		// Now we need to change a stream file;
		ReleaseResource();
		if( !InitFromFile(pszStreamFile) )
		{
			AMERRLOG(("AMSoundStream::Play(), Call InitFromFile [%s] again fail!", pszStreamFile));
			return false;
		}
	}

	if( !m_bHasInit )
		return true;

	if( m_fileType == AMS_WAVEFILE )
		m_pWaveFile->Seek(0);
	else if( m_fileType == AMS_MP3FILE )
		m_pMp3File->Seek(0);

	m_nLeftBufferNum = 0;
	m_nPlayedBufferNum = 0;
	m_bEnd = false;

	for(int i=0; i<m_nBufferNum; i++)
	{
		if( !FillBuffer(i) )
		{
			AMERRLOG(("AMSoundStream::Play(), Fill Buffer [%d] fail!", i));
			return false;
		}
		ResetEvent(m_hEvent[i]);
	}

	HANDLE hThread;
	DWORD  dwThreadID;
	l_bQuit = false;

	hThread = CreateThread(NULL, 0, BgmHandle, this, 0, &dwThreadID);

	m_pDSBuffer->SetCurrentPosition(0);
	m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

	m_state = AMSS_PLAYING;
	return true;
}

bool AMSoundStream::Stop()
{
	if( !m_bHasInit )
		return true;

	m_pDSBuffer->Stop();
	m_pDSBuffer->SetCurrentPosition(0);

	l_bQuit = true;
	EnterCriticalSection(&l_csExit);
	LeaveCriticalSection(&l_csExit);

	if( m_fileType == AMS_WAVEFILE )
		m_pWaveFile->Seek(0);
	else if( m_fileType == AMS_MP3FILE )
		m_pMp3File->Seek(0);

	for(int i=0; i<m_nBufferNum; i++)
		ResetEvent(m_hEvent[i]);

	m_nLeftBufferNum = 0;
	m_nPlayedBufferNum = 0;
	m_nGapToWait = 0;
	m_state = AMSS_STOPPED;
	return true;
}

bool AMSoundStream::Pause(bool bPause)
{
	if( !m_bHasInit )
		return true;

	if( bPause )
	{
		m_pDSBuffer->Stop();
		m_state = AMSS_PAUSED;
	}
	else
	{
		m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
		m_state = AMSS_PLAYING;
	}
		
	return true;
}

DWORD WINAPI BgmHandle(LPVOID pArg)
{
	AMSoundStream * pThis = (AMSoundStream *) pArg;

	EnterCriticalSection(&l_csExit);
	
	while( !l_bQuit )
	{
		if( !pThis->HandleNotifyMessage() )
			break;
		Sleep(pThis->GetBufferLen() / 2);
	}

	LeaveCriticalSection(&l_csExit);
	return 0x88;
}

bool AMSoundStream::FillBuffer(int index)
{
	DWORD				dwStart, dwLen, dwReadLen;
	DWORD				dwBytes1, dwBytes2;
	LPVOID				psndBuffer;

	dwStart = index * m_dwBufferSize;
	dwLen	= m_dwBufferSize;
	if FAILED(m_pDSBuffer->Lock(dwStart, dwLen, &psndBuffer, &dwBytes1, NULL, &dwBytes2, NULL)) 
	{
		AMERRLOG(("AMSoundStream::FillBuffer(), Lock sound buffer fail!"));
		return false;
	}

	if( dwBytes1 < dwLen )
	{
		AMERRLOG(("AMSoundStream::FillBuffer(), Lock result error!"));
		return false;
	}

 	if( m_bEnd )
	{
		//Insert solince here;
		ZeroMemory(psndBuffer, dwLen);
	}
	else
	{
		if( m_fileType == AMS_WAVEFILE )
		{
			if( !m_pWaveFile->ReadData((LPBYTE)psndBuffer, dwLen, &dwReadLen, &m_bEnd) )
			{
				AMERRLOG(("AMSoundStream::FillBuffer(), read wave file data fail!"));
				return false;
			}
		}
		else if( m_fileType == AMS_MP3FILE )
		{
			if( !m_pMp3File->ReadData((LPBYTE)psndBuffer, dwLen, &dwReadLen, &m_bEnd) )
			{
				AMERRLOG(("AMSoundStream::FillBuffer(), read wave file data fail!"));
				return false;
			}
		}

		if( dwReadLen < dwLen )
			ZeroMemory((LPBYTE)psndBuffer + dwReadLen, dwLen - dwReadLen);
		m_nLeftBufferNum ++;
	}

	if FAILED(m_pDSBuffer->Unlock(psndBuffer, dwBytes1, NULL, 0))
	{
		AMERRLOG(("AMSoundStream::FillBuffer(), Unlock dsound buffer fail!"));
		return false;
	}
	return true;
}

bool AMSoundStream::HandleNotifyMessage()
{
	DWORD	dwRet; 
	int		idEvent = m_nPlayedBufferNum % m_nBufferNum;

	dwRet = WaitForSingleObject(m_hEvent[idEvent], 0);
	if( dwRet == WAIT_TIMEOUT )
		return true;
	else
	{
		m_nPlayedBufferNum ++;
		ResetEvent(m_hEvent[idEvent]);
		m_nLeftBufferNum --;
		if( m_nLeftBufferNum == 0 )
		{
			// we should stop here;
			m_pDSBuffer->Stop();
			m_pDSBuffer->SetCurrentPosition(0);

			if( m_fileType == AMS_WAVEFILE )
				m_pWaveFile->Seek(0);
			else if( m_fileType == AMS_MP3FILE )
				m_pMp3File->Seek(0);

			for(int i=0; i<m_nBufferNum; i++)
				ResetEvent(m_hEvent[i]);

			m_nLeftBufferNum = 0;
			m_nPlayedBufferNum = 0;
			m_state = AMSS_ENDTRACK;
			return false;
		}

		if( !FillBuffer(idEvent) )
		{
			AMERRLOG(("AMSoundStream::HandleNotifyMessage(), FillBuffer fail!"));
			return false;
		}
	}

	return true;
}

bool AMSoundStream::InitFromFile(char * szStreamFile)
{
	char szFileNameLwr[MAX_PATH];

	strncpy(m_szStreamFile, szStreamFile, MAX_PATH);

	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return true;

	strncpy(szFileNameLwr, szStreamFile, MAX_PATH);
	_strlwr(szFileNameLwr);

	if( strstr(szFileNameLwr, ".wav") )
	{
		m_fileType = AMS_WAVEFILE;

		// We use wave file here;
		m_pAFile = new AFile();
		if( NULL == m_pAFile )
		{
			AMERRLOG(("AMSoundStream::Init(), Not enough memory!"));
			return false;
		}
		if( !m_pAFile->Open(szStreamFile, AFILE_OPENEXIST) )
		{
			m_pAFile->Close();
			delete m_pAFile;
			m_pAFile = NULL;
			AMERRLOG(("AMSoundStream::Init(), Can not open file [%s]!", szStreamFile));
			return false;
		}

		m_pWaveFile = new AMWaveFile();
		if( NULL == m_pWaveFile )
		{
			m_pAFile->Close();
			delete m_pAFile;
			m_pAFile = NULL;
			AMERRLOG(("AMSoundStream::Init(), Not enough memory!"));
			return false;
		}
		if( !m_pWaveFile->Open(m_pAFile) )
		{
			m_pAFile->Close();
			delete m_pAFile;
			m_pAFile = NULL;
			m_pWaveFile->Close();
			delete m_pWaveFile;
			m_pWaveFile = NULL;
			AMERRLOG(("AMSoundStream::Init(), Can not open [%s] as a wave file!", szStreamFile));
			return false;
		}
		m_streamDesc.wfxFormat = m_pWaveFile->GetWaveformatEx();
	}
	else if( strstr(szFileNameLwr, ".mp3") )
	{
		m_fileType = AMS_MP3FILE;

		// We use mp3 file here;
		m_pMp3File = new AMMp3File();
		if( NULL == m_pMp3File )
		{
			AMERRLOG(("AMSoundStream::Init(), Not enough memory!"));
			return false;
		}
		if( !m_pMp3File->Open(szStreamFile) )
		{
			delete m_pMp3File;
			m_pMp3File = NULL;
			AMERRLOG(("AMSoundStream::Init(), Can not open [%s] as a mp3 file!", szStreamFile));
			return false;
		}
		m_streamDesc.wfxFormat = m_pMp3File->GetWaveformatEx();
	}

	// Now create the buffers here;
	int					i;
	DSBPOSITIONNOTIFY	dspn[AMSS_MAX_BUFFER_NUM];

	m_dwBufferSize = m_nBufferLen * m_streamDesc.wfxFormat.nAvgBytesPerSec / 1000;

	//Create the dsound buffer
	HRESULT			hval;
	DSBUFFERDESC	dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC); 
    dsbd.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = m_nBufferNum * m_dwBufferSize;
    dsbd.lpwfxFormat = &m_streamDesc.wfxFormat; 
	
	hval = m_pAMSoundEngine->GetDS()->CreateSoundBuffer(&dsbd, &m_pDSBuffer, NULL);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundStream::Init(), CreateSoundBuffer fail!"));
		return false;
	}

	hval = m_pDSBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&m_pDSNotify);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundStream::Init(), Query DSNotify fail!"));
		return false;
	}

	for(i=0; i<m_nBufferNum; i++)
	{
		ResetEvent(m_hEvent[i]);

		dspn[i].dwOffset = (i + 1) * m_dwBufferSize - 1;
		dspn[i].hEventNotify = m_hEvent[i];
	}

	hval = m_pDSNotify->SetNotificationPositions(m_nBufferNum, dspn);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundStream::Init(), SetNotificationPositions Fail!"));
		return false;
	}

	m_state = AMSS_STOPPED;

	m_bHasInit = true;
	SetVolume(m_dwVolume);
	return true;
}

bool AMSoundStream::ReleaseResource()
{
	Stop();

	if( m_pWaveFile )
	{
		m_pWaveFile->Close();
		delete m_pWaveFile;
		m_pWaveFile = NULL;
	}
	if( m_pAFile )
	{
		m_pAFile->Close();
		delete m_pAFile;
		m_pAFile = NULL;
	}
	if( m_pMp3File )
	{
		m_pMp3File->Close();
		delete m_pMp3File;
		m_pMp3File = NULL;
	}
	if( m_pDSNotify )
	{
		m_pDSNotify->Release();
		m_pDSNotify = NULL;
	}
	if( m_pDSBuffer )
	{
		m_pDSBuffer->Stop();
		m_pDSBuffer->Release();
		m_pDSBuffer = NULL;
	}

	m_fileType  = AMS_UNKOWNFILE;
	m_state		= AMSS_STOPPED;

	m_bHasInit = false;
	return true;
}

bool AMSoundStream::Tick()
{
	// Here we should do something on stop;
	// For example, we should check if there is some queued stream or loop again?
	if( m_state == AMSS_ENDTRACK )
	{
		// Just end of playing one track, so check if we need to play another track;
		if( m_bLoop )
		{
			m_nGapToWait = m_nGapToInsert;
			m_nLastWaitTime = GetTickCount();
			m_state = AMSS_BEGINTRACK;
		}
		else
			return Stop();
	}
	else if( m_state == AMSS_BEGINTRACK )
	{
		m_nGapToWait -= GetTickCount() - m_nLastWaitTime;
		m_nLastWaitTime = GetTickCount();

		if( m_nGapToWait <= 0 )
		{
			if( !Play() )
			{
				AMERRLOG(("AMSoundStream::Tick() Replay the stream again!"));
				return false;
			}
		}
	}

	return true;
}
