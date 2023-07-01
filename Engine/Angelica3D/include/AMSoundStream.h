/*
 * FILE: AMSoundStream.h
 *
 * DESCRIPTION: sound stream class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMSOUNDSTREAM_H_
#define _AMSOUNDSTREAM_H_

#include "AMTypes.h"
#include "AMPlatform.h"

typedef class AMSoundEngine * PAMSoundEngine;
typedef class AMWaveFile * PAMWaveFile;
typedef class AMMp3File * PAMMp3File;

#define AMSS_MAX_BUFFER_NUM		256

class AMSoundStream
{
private:
	PAMSoundEngine				m_pAMSoundEngine;
	AMSSTREAMDESC				m_streamDesc;

	bool						m_bHasInit;
	char						m_szStreamFile[MAX_PATH];

	IDirectSoundBuffer *		m_pDSBuffer;
	IDirectSoundNotify *		m_pDSNotify;

	int							m_nBufferLen;
	int							m_nBufferNum;

	int							m_nPlayedBufferNum;
	int							m_nLeftBufferNum;
	DWORD						m_dwBufferSize;
	bool						m_bEnd;
	HANDLE						m_hEvent[AMSS_MAX_BUFFER_NUM];

	AMSOUNDFILETYPE				m_fileType;
	AMWaveFile					*m_pWaveFile;
	AMMp3File					*m_pMp3File;

	AFile						*m_pAFile;

	AMSS_STATE					m_state;
	bool						m_bLoop;

	int							m_nGapToInsert;
	int							m_nGapToWait;
	int							m_nLastWaitTime;

	DWORD						m_dwVolume;

protected:
	bool InitFromFile(char * szStreamFile);
	bool ReleaseResource();

	bool FillBuffer(int index);
	bool HandleNotifyMessage();

public:
	AMSoundStream();
	virtual ~AMSoundStream();

	// Create the sound stream;
	bool Init(PAMSoundEngine pAMSoundEngine, char * pszStreamFile, AMSSTREAMDESC * pDesc=NULL);
	bool Release();

	bool Queue(char * szStreamFile);

	bool Play(char * szStreamFile=NULL);
	bool Stop();
	bool Pause(bool bPause);

	bool Tick();

	friend DWORD WINAPI BgmHandle(LPVOID pArg);

	inline int GetBufferLen() { return m_nBufferLen; }
	inline int GetBufferNum() { return m_nBufferNum; }
	inline bool IsPlaying() { return m_state == AMSS_PLAYING; }
	inline bool IsStopped() { return m_state == AMSS_STOPPED; }
	inline bool IsPaused() { return m_state == AMSS_PAUSED; }
	inline char * GetStreamFileName() { return m_szStreamFile; }
	inline bool GetLoopFlag() { return m_bLoop; }
	inline void SetLoopFlag(bool bLoop) { m_bLoop = bLoop; }
	inline void SetGapToInsert(int nGapToInsert) { m_nGapToInsert = nGapToInsert; }
	inline int GetGapToInsert() { return m_nGapToInsert; }
	inline void SetVolume(DWORD dwVolume)
	{
		m_dwVolume = dwVolume;
		if( m_pDSBuffer )
		{
			if( m_dwVolume == 0 )
				m_pDSBuffer->SetVolume(DSBVOLUME_MIN);
			else
				m_pDSBuffer->SetVolume(AMSOUND_MINVOLUME + (AMSOUND_MAXVOLUME - AMSOUND_MINVOLUME) * m_dwVolume / 100);
		}
	}
	inline DWORD GetVolume() { return m_dwVolume; }
};

typedef class AMSoundStream * PAMSoundStream;
#endif//_AMSOUNDSTREAM_H_
