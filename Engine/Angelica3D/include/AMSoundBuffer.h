/*
 * FILE: AMSoundBuffer.h
 *
 * DESCRIPTION: sound buffer class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMSOUNDBUFFER_H_
#define _AMSOUNDBUFFER_H_

#include "AMTypes.h"
#include "AMPlatform.h"

#include <AMImmEffect.h>

typedef class AMSoundEngine * PAMSoundEngine;
typedef class AMSoundBufferMan * PAMSoundBufferMan;

class AMSoundBuffer
{
	friend class AMSoundBufferMan;
	friend class AM3DSoundBufferMan;

protected:
	char						m_szFileName[MAX_PATH];

	PAMSoundEngine				m_pAMSoundEngine;
	AMSBUFFERDESC				m_bufferDesc;
	IDirectSoundBuffer *		m_pDSBuffer;
	IDirectSoundNotify8 *		m_pDSNotify8;
	IDirectSoundBuffer8 *		m_pDSBuffer8;

	HANDLE						m_hEndEvent;

	AMImmEffect *				m_pImmEffect;

	bool						m_bLooping;
	AMSBSTATE					m_nState;

	DWORD						m_dwVolume;

	bool						m_bFadeIn;		// flag indicate fade in
	bool						m_bFadeOut;		// flag indicate fade out
	DWORD						m_dwVolumeSet;	// used for fade in(out);

	bool						m_bLoaded;

public:
	AMSoundBuffer();
	virtual ~AMSoundBuffer();

protected:
	// Create the sound buffer;
	virtual bool Create(PAMSoundEngine pAMSoundEngine, AMSBUFFERDESC * pDesc);
	virtual bool Release();
	bool DownSample(LPBYTE pDestBuf, LPBYTE pSrcBuf, WAVEFORMATEX wfx, int nSampleCount, DWORD dwDownRate);

public:
	// Create and load sound data from a file;
	// In:
	//		pFileToLoad	- the opened AMFile pointer;
	//		szFileName  - the file's name, include a relative path maybe;
	virtual bool Load(PAMSoundEngine pAMSoundEngine, AFile * pFileToLoad, char * szFileName);
	virtual bool Save(AFile * pFileToSave);

	virtual bool LoadImmEffect();
	virtual bool UnloadImmEffect();

	virtual AFileImage * FindSoundFileInMem();

	// Use this method to reload the sound resource;
	virtual bool ReloadSoundData();
	// Use this method to free the sound resource when it will not used for sometime;
	virtual bool UnloadSoundData();

	virtual bool Play(bool bLoop);
	virtual bool Stop();
	virtual bool Pause();

	virtual bool CanHear() { return true; }

private:
	DWORD GetPlayPos();
	DWORD GetWritePos();

public:
	// update something;
	bool TickFade();

	// See if it has reached the end;
	virtual bool CheckEnd();

	bool FadeIn();
	bool FadeOut();

public:
	inline IDirectSoundBuffer * GetDSoundBuffer() { return m_pDSBuffer; }
	inline AMSBUFFERDESC * GetBufferDesc() { return &m_bufferDesc; }
	inline bool IsPlaying() { return m_nState == AMSBSTATE_PLAYING; }
	inline bool IsPaused() { return m_nState == AMSBSTATE_PAUSED; }
	inline bool IsStopped() { return m_nState == AMSBSTATE_STOPPED; }
	inline AMSBSTATE GetState() { return m_nState; }
	inline char * GetFileName() { return m_szFileName; }
	inline DWORD GetVolume() { return m_dwVolume; }
	
	inline void SetVolume(DWORD dwVolume) 
	{
		m_dwVolume = dwVolume;
		if( m_pDSBuffer8 )
		{
			if( m_dwVolume == 0 )
				m_pDSBuffer8->SetVolume(DSBVOLUME_MIN);
			else
				m_pDSBuffer8->SetVolume(AMSOUND_MINVOLUME + (AMSOUND_MAXVOLUME - AMSOUND_MINVOLUME) * m_dwVolume / 100);
		}
	}
	inline bool IsLooping() { return m_bLooping; }
};

typedef class AMSoundBuffer * PAMSoundBuffer;
#endif//_AMSOUNDBUFFER_H_