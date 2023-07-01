/*
 * FILE: AMSoundBuffer.cpp
 *
 * DESCRIPTION: sound buffer class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "AMConfig.h"
#include "AMPI.h"
#include "AMEngine.h"
#include "AMSoundEngine.h"
#include "AMSoundBuffer.h"
#include "AMWaveFile.h"
#include "AMMp3File.h"
#include "AMSoundBufferMan.h"

#include <AMImmWrapper.h>

AMSoundBuffer::AMSoundBuffer()
{
	m_pAMSoundEngine	= NULL;
	m_pDSBuffer			= NULL;
	m_pDSBuffer8		= NULL;
	m_pDSNotify8		= NULL;

	m_hEndEvent			= NULL;
	m_pImmEffect		= NULL;

	m_nState			= AMSBSTATE_STOPPED;
	m_bLooping			= false;

	m_bLoaded			= false;

	m_dwVolume			= 100;
	m_bFadeIn			= false;
	m_bFadeOut			= false;
}

AMSoundBuffer::~AMSoundBuffer()
{
}

// Create the sound buffer;
bool AMSoundBuffer::Create(PAMSoundEngine pAMSoundEngine, AMSBUFFERDESC * pDesc)
{
	m_pAMSoundEngine = pAMSoundEngine;
	m_bufferDesc = *pDesc;

	// If we did not create the direct sound object;
	if( NULL == pAMSoundEngine->GetDS() )
		return true;

	DSBUFFERDESC	dsbDesc;
	ZeroMemory(&dsbDesc, sizeof(dsbDesc));
	dsbDesc.dwSize = sizeof(dsbDesc);
	dsbDesc.dwBufferBytes = m_bufferDesc.dwBufferBytes;
	dsbDesc.lpwfxFormat = &m_bufferDesc.wfxFormat;
	dsbDesc.dwFlags = m_bufferDesc.dwFlags | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;

	switch( m_bufferDesc.algorithm )
	{
	case AMS3DALG_DEFAULT:
		dsbDesc.guid3DAlgorithm = DS3DALG_DEFAULT;
		break;
	case AMS3DALG_NO_VIRTUALIZATION:
		dsbDesc.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
		break;
	case AMS3DALG_HRTF_FULL:
		dsbDesc.guid3DAlgorithm = DS3DALG_HRTF_FULL;
		break;
	case AMS3DALG_HRTF_LIGHT:
		dsbDesc.guid3DAlgorithm = DS3DALG_HRTF_LIGHT;
		break;
	}

	HRESULT hval;
	hval = m_pAMSoundEngine->GetDS()->CreateSoundBuffer(&dsbDesc, &m_pDSBuffer, NULL);
	if FAILED(hval)
	{
		AMERRLOG(("AMSoundBuffer::Init, CreateSoundBuffer fail!"));
		return false;
	}

	// Now we create the notify event here;
	hval = m_pDSBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&m_pDSBuffer8);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundBuffer::Init(), Query DS3DBuffer8 fail!"));
		return false;
	}

	// Now we create the notify event here;
	hval = m_pDSBuffer8->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&m_pDSNotify8);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundBuffer::Init(), Query DSNotify fail!"));
		return false;
	}

	m_hEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if( NULL == m_hEndEvent )
	{
		AMERRLOG(("AMSoundBuffer::Init(), CreateEvent fail!"));
		return false;
	}

	DSBPOSITIONNOTIFY	dspn;
	// we must use DSBPN_OFFSETSTOP instead of last byte of the buffer, because
	// read curor is somewhat beyond the real playcursor
	dspn.dwOffset = DSBPN_OFFSETSTOP;//dsbDesc.dwBufferBytes - 1; 
	dspn.hEventNotify = m_hEndEvent;

	hval = m_pDSNotify8->SetNotificationPositions(1, &dspn);
	if FAILED(hval) 
	{
		AMERRLOG(("AMSoundBuffer::Init(), SetNotificationPositions fail!"));
		return false;
	}

	SetVolume(m_pAMSoundEngine->GetVolume());
	
	m_nState = AMSBSTATE_STOPPED;
	m_bLooping = false;
	return true;
}

bool AMSoundBuffer::Release()
{
	UnloadSoundData();

	if( m_pImmEffect )
		UnloadImmEffect();
	return true;
}

bool AMSoundBuffer::UnloadSoundData()
{
	if( m_pDSNotify8 )
	{
		m_pDSNotify8->Release();
		m_pDSNotify8 = NULL;
	}

	if( m_pDSBuffer8 )
	{
		m_pDSBuffer8->Stop();
		m_pDSBuffer8->Release();
		m_pDSBuffer8 = NULL;
	}

	if( m_pDSBuffer )
	{
		m_pDSBuffer->Stop();
		m_pDSBuffer->Release();
		m_pDSBuffer = NULL;
	}

	if( m_hEndEvent )
	{
		CloseHandle(m_hEndEvent);
		m_hEndEvent = NULL;
	}

	m_bLoaded = false;
	return true;
}

bool AMSoundBuffer::ReloadSoundData()
{
	if( m_bLoaded )
		return true;

	AFileImage * pFile = FindSoundFileInMem();
	if( NULL == pFile )
	{
		AMERRLOG(("AMSoundBuffer::ReloadSoundData(), Can not find the file image in memory! Reload fail!"));
		return false;
	}

	pFile->ResetPointer();

	if( !Load(m_pAMSoundEngine, pFile, m_szFileName) )
	{
		AMERRLOG(("AMSoundBuffer::ReloadSoundData(), Can not load the sound data!"));
		return false;
	}

	m_bLoaded = true;	
	return true;
}

bool AMSoundBuffer::Play(bool bLoop)
{
	if( !m_bLoaded )
	{
		//By now we will not load sound data automatically;
		return true;
		// Reload the sound data, if it can not be reloaded, just return true
		if( !ReloadSoundData() )
			return true;
	}

	if( m_pDSBuffer8 )
	{
		HRESULT		hval;

		DWORD	dwFlags;

		// We should start the sound from the begin if not be paused;
		if( m_nState != AMSBSTATE_PAUSED )
			m_pDSBuffer8->SetCurrentPosition(0);	
		
		if( bLoop )
			dwFlags = DSBPLAY_LOOPING;
		else
			dwFlags = NULL;

		ResetEvent(m_hEndEvent);

		hval = m_pDSBuffer8->Play(0, 0, dwFlags);
		if FAILED(hval)
		{
			AMERRLOG(("AMSoundBuffer::Play, Call IDirectSoundBuffer8::Play fail"));
			return false;
		}
	}

	if( m_nState != AMSBSTATE_PAUSED )
	{
		if( m_pImmEffect && g_pAMConfig->GetFlagForceFeedback() )
			m_pImmEffect->Start();
	}

	m_nState = AMSBSTATE_PLAYING;
	m_bLooping = bLoop;

	m_bFadeIn = false;
	m_bFadeOut = false;
	return true;
}

bool AMSoundBuffer::Stop()
{
	if( m_pDSBuffer8 )
	{
		m_pDSBuffer8->Stop();
		m_pDSBuffer8->SetCurrentPosition(0);
	}

	if( m_pImmEffect )
		m_pImmEffect->Stop();

	m_nState = AMSBSTATE_STOPPED;
	m_bLooping = false;

	if( m_bFadeIn || m_bFadeOut )
	{
		// If we have done some fade, we must set to default volume;
		SetVolume(m_pAMSoundEngine->GetVolume());
	}
	m_bFadeIn = false;
	m_bFadeOut = false;
	return true;
}

bool AMSoundBuffer::Pause()
{
	if( m_pDSBuffer8 )
	{
		m_pDSBuffer8->Stop();
	}

	if( m_pImmEffect )
		m_pImmEffect->Stop();

	m_nState = AMSBSTATE_PAUSED;
	return true;
}

bool AMSoundBuffer::DownSample(LPBYTE pDestBuf, LPBYTE pSrcBuf, WAVEFORMATEX wfx, int nSampleCount, DWORD dwDownRate)
{
	DWORD dwDestIndex = 0;
	DWORD dwSrcIndex = 0;
	for(int i=0; i<nSampleCount; i+=dwDownRate)
	{
		memcpy(pDestBuf + dwDestIndex, pSrcBuf + dwSrcIndex, wfx.nBlockAlign);
		dwDestIndex += wfx.nBlockAlign;
		dwSrcIndex += dwDownRate * wfx.nBlockAlign;
	}

	return true;
}

// Create and load sound data from a file;
bool AMSoundBuffer::Load(PAMSoundEngine pAMSoundEngine, AFile * pFileToLoad, char * szFileName)
{
	char szFileNameLwr[MAX_PATH];

	m_pAMSoundEngine = pAMSoundEngine;
	strncpy(m_szFileName, szFileName, MAX_PATH);

	// If we did not create the direct sound object;
	if( NULL == pAMSoundEngine->GetDS() )
		return true;

	strncpy(szFileNameLwr, szFileName, MAX_PATH);
	_strlwr(szFileNameLwr);

	if( strstr(szFileNameLwr, ".wav") )
	{
		// It's a wave format file
		AMWaveFile waveFile;

		if( !waveFile.Open(pFileToLoad) )
		{
			AMERRLOG(("AMSoundBuffer::Load, open the file as a wave file fail!"));
			return false;
		}
		AMSBUFFERDESC	bufferDesc;

		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.wfxFormat = waveFile.GetWaveformatEx();
		bufferDesc.dwBufferBytes = waveFile.GetDataSize();
		bufferDesc.dwFlags = NULL;
		bufferDesc.algorithm = AMS3DALG_DEFAULT;

		DWORD dwDownRate = 1;
		if( g_pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_MEDIUM )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 22050 )
				dwDownRate = 2;
		}
		else if( g_pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_LOW )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 44100 )
				dwDownRate = 4;
			else if( bufferDesc.wfxFormat.nSamplesPerSec >= 22050 )
				dwDownRate = 2;
		}

		int nSampleCount = bufferDesc.dwBufferBytes / bufferDesc.wfxFormat.nBlockAlign / dwDownRate;
		if( nSampleCount * dwDownRate < waveFile.GetDataSize() / waveFile.GetWaveformatEx().nBlockAlign )
			nSampleCount ++;
		bufferDesc.dwBufferBytes = nSampleCount * bufferDesc.wfxFormat.nBlockAlign;
		bufferDesc.wfxFormat.nSamplesPerSec /= dwDownRate;
		bufferDesc.wfxFormat.nAvgBytesPerSec /= dwDownRate;
		if( !Create(pAMSoundEngine, &bufferDesc) )
		{
			AMERRLOG(("AMSoundBuffer::Load, Call Create Fail!"));
			return false;
		}

		// Now lock the direct sound buffer and load data into it;
		HRESULT hval;
		LPBYTE	pBuffer1;
		DWORD	dwBufferLength1;
		LPBYTE	pBuffer2;
		DWORD	dwBufferLength2;

		hval = m_pDSBuffer8->Lock(0, 0, (LPVOID *)&pBuffer1, &dwBufferLength1, 
			(LPVOID *)&pBuffer2, &dwBufferLength2, DSBLOCK_ENTIREBUFFER);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer Fail!"));
			return false;
		}

		if( pBuffer2 || dwBufferLength1 != bufferDesc.dwBufferBytes )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer has generated an loop back pointer!"));
			return false;
		}

		DWORD dwReadSize;
		bool bEnd;
		
		if( dwDownRate == 1 ) // Origin sample rate;
		{
			if( !waveFile.ReadData(pBuffer1, waveFile.GetDataSize(), &dwReadSize, &bEnd) )
			{
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
		}
		else
		{
			LPBYTE pOrgBuf = (LPBYTE) malloc(waveFile.GetDataSize());
			if( NULL == pOrgBuf )
				return false;
			if( !waveFile.ReadData(pOrgBuf, waveFile.GetDataSize(), &dwReadSize, &bEnd) )
			{
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
			DownSample(pBuffer1, pOrgBuf, waveFile.GetWaveformatEx(), waveFile.GetDataSize() / waveFile.GetWaveformatEx().nBlockAlign, dwDownRate);
			free(pOrgBuf);
		}

		hval = m_pDSBuffer8->Unlock(pBuffer1, dwBufferLength1, pBuffer2, dwBufferLength2);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Unlock DSBuffer Fail!"));
			return false;
		}
		waveFile.Close();
	}
	else if( strstr(szFileNameLwr, ".mp3") )
	{
		// It's a mp3 format file
		AMMp3File mp3File;

		if( !mp3File.Open(pFileToLoad->GetFileName()) )
		{
			AMERRLOG(("AMSoundBuffer::Load, open the file as a mp3 file fail!"));
			return false;
		}
		AMSBUFFERDESC	bufferDesc;

		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.wfxFormat = mp3File.GetWaveformatEx();
		bufferDesc.dwBufferBytes = mp3File.GetDataSize();
		bufferDesc.dwFlags = NULL;
		bufferDesc.algorithm = AMS3DALG_DEFAULT;

		DWORD dwDownRate = 1;
		if( g_pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_MEDIUM )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 22100 )
				dwDownRate = 2;
		}
		else if( g_pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_LOW )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 44100 )
				dwDownRate = 4;
			else if( bufferDesc.wfxFormat.nSamplesPerSec >= 22050 )
				dwDownRate = 2;
		}

		int nSampleCount = bufferDesc.dwBufferBytes / bufferDesc.wfxFormat.nBlockAlign / dwDownRate;
		if( nSampleCount * dwDownRate < mp3File.GetDataSize() / mp3File.GetWaveformatEx().nBlockAlign )
			nSampleCount ++;
		bufferDesc.dwBufferBytes = nSampleCount * bufferDesc.wfxFormat.nBlockAlign;
		bufferDesc.wfxFormat.nSamplesPerSec /= dwDownRate;
		bufferDesc.wfxFormat.nAvgBytesPerSec /= dwDownRate;

		if( !Create(pAMSoundEngine, &bufferDesc) )
		{
			AMERRLOG(("AMSoundBuffer::Load, Call Create Fail!"));
			return false;
		}

		// Now lock the direct sound buffer and load data into it;
		HRESULT hval;
		LPBYTE	pBuffer1;
		DWORD	dwBufferLength1;
		LPBYTE	pBuffer2;
		DWORD	dwBufferLength2;

		hval = m_pDSBuffer8->Lock(0, 0, (LPVOID *)&pBuffer1, &dwBufferLength1, 
			(LPVOID *)&pBuffer2, &dwBufferLength2, DSBLOCK_ENTIREBUFFER);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer Fail!"));
			return false;
		}

		if( pBuffer2 || dwBufferLength1 != bufferDesc.dwBufferBytes )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer has generated an loop back pointer!"));
			return false;
		}

		DWORD dwReadSize;
		bool bEnd;
		
		if( dwDownRate == 1 ) // Origin sample rate;
		{
			if( !mp3File.ReadData(pBuffer1, mp3File.GetDataSize(), &dwReadSize, &bEnd) )
			{
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
		}
		else
		{
			LPBYTE pOrgBuf = (LPBYTE) malloc(mp3File.GetDataSize());
			if( NULL == pOrgBuf )
				return false;
			if( !mp3File.ReadData(pOrgBuf, mp3File.GetDataSize(), &dwReadSize, &bEnd) )
			{
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
			DownSample(pBuffer1, pOrgBuf, mp3File.GetWaveformatEx(), mp3File.GetDataSize() / mp3File.GetWaveformatEx().nBlockAlign, dwDownRate);
			free(pOrgBuf);
		}

		hval = m_pDSBuffer8->Unlock(pBuffer1, dwBufferLength1, pBuffer2, dwBufferLength2);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Unlock DSBuffer Fail!"));
			return false;
		}
		mp3File.Close();
	}
	else
	{
		AMERRLOG(("AMSoundBuffer::Load, File [%s] Unsupported file extension", pFileToLoad->GetFileName()));
		return false;
	}

	m_bLoaded = true;
	return true;
}

bool AMSoundBuffer::Save(AFile * pFileToSave)
{
	return true;
}

bool AMSoundBuffer::TickFade()
{
	// Do fade in(out)
	if( m_bFadeIn )
	{
		if( m_dwVolumeSet <= m_dwVolume )
		{
			m_bFadeIn = false; // finish
			return true;
		}

		if( 1 )//m_pAMSoundEngine->GetAMEngine()->GetTicks() % 2 )
		{
			m_dwVolume ++;
			SetVolume(m_dwVolume);
		}
	}
	else if( m_bFadeOut )
	{
		if( m_dwVolumeSet >= m_dwVolume )
		{
			m_bFadeOut = false;
			Stop();
			return true;
		}

		if( 1 )//m_pAMSoundEngine->GetAMEngine()->GetTicks() % 2 )
		{
			m_dwVolume --;
			SetVolume(m_dwVolume);
		}
	}

	return true;
}

// Play position control methods;
bool AMSoundBuffer::CheckEnd()
{
	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
	{
		Stop();
		return true;
	}

	if( m_nState == AMSBSTATE_PLAYING )
	{
		// We should check if we have reached the end;
		if( WAIT_OBJECT_0 == WaitForSingleObject(m_hEndEvent, 0) )
		{
			// Because we use position notify value DSBPN_OFFSETSTOP, so only when
			// no loop sound reach the end of buffer, this event will be signaled.
			// if( !m_bLooping )
			Stop();
		}
	}

	return true;
}

DWORD AMSoundBuffer::GetPlayPos()
{
	DWORD		dwPlayPos, dwWritePos;
	HRESULT		hval;

	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return 0;

	hval = m_pDSBuffer8->GetCurrentPosition(&dwPlayPos, &dwWritePos);
	if FAILED(hval) 
	{
		AMERRLOG(("AMSoundBuffer::GetPlayPos(), Get Current position fail!"));
		return 0;
	}

	return dwPlayPos;
}

DWORD AMSoundBuffer::GetWritePos()
{
	DWORD		dwPlayPos, dwWritePos;
	HRESULT		hval;

	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return 0;

	hval = m_pDSBuffer8->GetCurrentPosition(&dwPlayPos, &dwWritePos);
	if FAILED(hval) 
	{
		AMERRLOG(("AMSoundBuffer::GetPlayPos(), Get Current position fail!"));
		return 0;
	}

	return dwWritePos;
}

// Force feed back effect load and unload method;
bool AMSoundBuffer::LoadImmEffect()
{
	// If we did not create the direct sound object, we will not use force feed back effect either;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return true;

	if( m_pImmEffect )
		UnloadImmEffect();

	// Now we try to load a force feed back effect here;
	if( m_pAMSoundEngine->GetAMEngine()->GetAMImmWrapper() )
	{
		char szTitle[MAX_PATH];
		if( !A3DGetFileTitle(m_szFileName, szTitle, MAX_PATH) )
			return false;

		szTitle[strlen(szTitle) - 4] = '\0';
		_strlwr(szTitle);
		if( !m_pAMSoundEngine->GetAMEngine()->GetAMImmWrapper()->LoadImmEffect(szTitle, &m_pImmEffect) )
		{
			// It has no imm effects;
			m_pImmEffect = NULL;
			return true;
		}
	}

	return true;
}

bool AMSoundBuffer::UnloadImmEffect()
{
	if( m_pImmEffect )
	{
		m_pAMSoundEngine->GetAMEngine()->GetAMImmWrapper()->ReleaseImmEffect(m_pImmEffect);
		m_pImmEffect = NULL;		
	}

	return true;
}

AFileImage * AMSoundBuffer::FindSoundFileInMem()
{
	return m_pAMSoundEngine->GetSoundBufferMan()->FindSoundFileImage(m_szFileName);
}

bool AMSoundBuffer::FadeIn()
{
	m_bFadeIn = true;

	SetVolume(0);
	m_dwVolumeSet = m_pAMSoundEngine->GetVolume();

	return true;
}

bool AMSoundBuffer::FadeOut()
{
	m_bFadeOut = true;
	m_dwVolumeSet = 1; // do not set to 0, because may not stop properly

	return true;
}