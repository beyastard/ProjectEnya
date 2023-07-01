/*
 * FILE: AM3DSoundBufferMan.h
 *
 * DESCRIPTION: 3D sound buffer manager class
 *
 * CREATED BY: Hedi, 2002/1/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AM3DSOUNDBUFFERMAN_H_
#define _AM3DSOUNDBUFFERMAN_H_

#include "AMPlatform.h"
#include "AMTypes.h"

#define MAX_3DSOUNDFILE			512
#define MAX_3DSOUNDFILEPLAYING	32
#define MAX_3DSOUNDBUFFERLOADED	512

typedef class AMSoundEngine * PAMSoundEngine;
typedef class AM3DSoundBuffer * PAM3DSoundBuffer;

class AM3DSoundBufferMan
{
	friend class AM3DSoundBuffer;
private:
	typedef struct _SOUND3DFILERECORD
	{
		char				szFilename[MAX_PATH];
		AFileImage			*pAFileImage;

	} SOUND3DFILERECORD, * PSOUND3DFILERECORD;

	typedef struct _SOUND3DBUFFERRECORD
	{
		AM3DSoundBuffer *	p3DSoundBuffer;
		int					nRepeatCount;
		int					nRepeatTimes;

	} SOUND3DBUFFERRECORD, * PSOUND3DBUFFERRECORD;

	char					m_szFolderName[32];
	AMSoundEngine *			m_pAMSoundEngine;
	int						m_nRecordNum;
	SOUND3DFILERECORD		m_pRecords[MAX_3DSOUNDFILE];

	int						m_n3DSoundFilePlaying;
	SOUND3DBUFFERRECORD		m_p3DSoundFilePlaying[MAX_3DSOUNDFILEPLAYING];

	int						m_n3DSoundBufferLoaded;
	AM3DSoundBuffer *		m_p3DSoundBufferLoaded[MAX_3DSOUNDBUFFERLOADED];

protected:
	AFileImage * Find3DSoundFileImage(char * szFilename);
	AFileImage * PreloadFile(char * szFilename);

public:
	AM3DSoundBufferMan();
	~AM3DSoundBufferMan();

	bool Init(AMSoundEngine * pAMSoundEngine);
	bool Release();

	bool Play3DSoundFile(char * sz3DSoundFile, A3DVECTOR3 vecPos, A3DVECTOR3 vecVelocity, FLOAT vMinDistance, FLOAT vMaxDistance, int nRepeat, bool bUseImmEffect, PAM3DSoundBuffer * pp3DSoundBuffer=NULL, bool bForce2D=false);
	bool Tick();

	bool SafeClose3DSoundBuffer(AM3DSoundBuffer * p3DSoundBuffer);

	bool Reset();

	bool Load3DSoundBufferFromFile(char * szFilename, PAM3DSoundBuffer * ppAM3DSoundBuffer);
	bool Release3DSoundBuffer(PAM3DSoundBuffer& pAM3DSoundBuffer);

	bool SetVolume(DWORD dwVolume);

	inline void SetFolderName(char * szFolderName) { strncpy(m_szFolderName, szFolderName, 32); }
	inline char * GetFolderName() { return m_szFolderName; }

	inline int GetLoaded3DSoundBufferNum() { return m_n3DSoundBufferLoaded; }
};

typedef class AM3DSoundBufferMan * PAM3DSoundBufferMan;
#endif//_AM3DSOUNDBUFFERMAN_H_
