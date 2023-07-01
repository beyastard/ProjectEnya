/*
 * FILE: AMSoundBufferMan.h
 *
 * DESCRIPTION: sound buffer manager class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMSOUNDBUFFERMAN_H_
#define _AMSOUNDBUFFERMAN_H_

#include "AMPlatform.h"
#include "AMTypes.h"

#define MAX_SOUNDFILE			512
#define MAX_SOUNDFILEPLAYING	32
#define MAX_SOUNDBUFFERLOADED	512

typedef class AMSoundEngine * PAMSoundEngine;
typedef class AMSoundBuffer * PAMSoundBuffer;

class AMSoundBufferMan
{
	friend class AMSoundBuffer;
private:
	typedef struct _SOUNDFILERECORD
	{
		char			szFilename[MAX_PATH];
		AFileImage		*pAFileImage;

	} SOUNDFILERECORD, * PSOUNDFILERECORD;

	typedef struct _SOUNDBUFFERRECORD
	{
		AMSoundBuffer *		pSoundBuffer;
		int					nRepeatCount;
		int					nRepeatTimes;

	} SOUNDBUFFERRECORD, PSOUNDBUFFERRECORD;

	char					m_szFolderName[32];
	AMSoundEngine *			m_pAMSoundEngine;
	int						m_nRecordNum;
	SOUNDFILERECORD			m_pRecords[MAX_SOUNDFILE];
	
	int						m_nSoundFilePlaying;
	SOUNDBUFFERRECORD		m_pSoundFilePlaying[MAX_SOUNDFILEPLAYING];

	int						m_nSoundBufferLoaded;
	AMSoundBuffer *			m_pSoundBufferLoaded[MAX_SOUNDBUFFERLOADED];

protected:
	AFileImage * FindSoundFileImage(char * szFilename);
	AFileImage * PreloadFile(char * szFilename);

public:
	AMSoundBufferMan();
	~AMSoundBufferMan();

	bool Init(AMSoundEngine * pAMSoundEngine);
	bool Release();	

	bool PlaySoundFile(char * szSoundFile, int nRepeat, bool bUseImmEffect, PAMSoundBuffer * ppSoundBuffer=NULL);
	bool Tick();

	bool SafeCloseSoundBuffer(AMSoundBuffer * pSoundBuffer);

	bool Reset();

	bool LoadSoundBufferFromFile(char * szFilname, PAMSoundBuffer * ppAMSoundBuffer);
	bool ReleaseSoundBuffer(PAMSoundBuffer& pAeMSoundBuffer);

	bool SetVolume(DWORD dwVolume);

	inline void SetFolderName(char * szFolderName) { strncpy(m_szFolderName, szFolderName, 32); }
	inline char * GetFolderName() { return m_szFolderName; }

	inline int GetLoadedSoundBufferNum() { return m_nSoundBufferLoaded; }
};

typedef class AMSoundBufferMan * PAMSoundBufferMan;
#endif//_AMSOUNDBUFFERMAN_H_
