/*
 * FILE: AMSoundEngine.h
 *
 * DESCRIPTION: The class standing for the sound engine of Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMSOUNDENGINE_H_
#define _AMSOUNDENGINE_H_

#include "AMTypes.h"
#include "AMPlatform.h"

typedef class AMEngine * PAMEngine;
typedef class AMSoundDevice * PAMSoundDevice;
typedef class AMSoundBuffer * PAMSoundBuffer;
typedef class AM3DSoundDevice * PAM3DSoundDevice;
typedef class AM3DSoundBuffer * PAM3DSoundBuffer;
typedef class AMSoundBufferMan * PAMSoundBufferMan;
typedef class AM3DSoundBufferMan * PAM3DSoundBufferMan;

class AMSoundEngine
{
private:
	HWND					m_hWnd;
	IDirectSound8 *			m_pDS;

	PAMSoundDevice			m_pAMSoundDevice;
	PAM3DSoundDevice		m_pAM3DSoundDevice;
	PAMSoundBufferMan		m_pAMSoundBufferMan;
	PAM3DSoundBufferMan		m_pAM3DSoundBufferMan;

	PAMEngine				m_pAMEngine;

	DWORD					m_dwVolume;

protected:

public:
	AMSoundEngine();
	~AMSoundEngine();

	bool Init(PAMEngine pAMEngine, HWND hWnd, AMSOUNDDEVFORMAT * pDevFormat, bool bUse3DDevice);
	bool Release();

	bool Tick();

	bool Reset();

	inline IDirectSound8 * GetDS() { return m_pDS; }
	inline HWND GetHWnd() { return m_hWnd; }
	inline AMEngine * GetAMEngine() { return m_pAMEngine; }
	inline AMSoundDevice * GetAMSoundDevice() { return m_pAMSoundDevice; }
	inline AM3DSoundDevice * GetAM3DSoundDevice() { return m_pAM3DSoundDevice; }
	inline AMSoundBufferMan * GetSoundBufferMan() { return m_pAMSoundBufferMan; }
	inline AM3DSoundBufferMan * Get3DSoundBufferMan() { return m_pAM3DSoundBufferMan; }
	inline DWORD GetVolume() { return m_dwVolume; }
	bool SetVolume(DWORD dwVolume); 

	int GetLoaded2DSoundNum();
	int GetLoaded3DSoundNum();
};

typedef class AMSoundEngine * PAMSoundEngine;

#endif//_AMSOUNDENGINE_H_
