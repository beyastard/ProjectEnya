/*
 * FILE: A3DCounter.h
 *
 * DESCRIPTION: An counter class for A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCOUNTER_H_
#define _A3DCOUNTER_H_

#include "A3DPlatform.h"
#include "A3DData.h"

#define MAX_PERFORMANCE_COUNTER		256

class A3DCounter : public A3DData
{
private:
	DWORD				m_dwFPSLastTick;
	DWORD				m_dwFPSCounter;
	FLOAT				m_vFPS;

protected:
	static _int64		m_nCPUFrequency;

public:
	A3DCounter();
	~A3DCounter();

	bool Init();
	bool Release();

	static DWORD GetMilliSecondNow();
	static __int64 GetMicroSecondNow();
	static __int64 GetCPUCycle();
	static __int64 GetCPUFrequency();

	bool ResetFPSCounter();
	bool UpdateFPSCounter(bool * pFPSChanged = NULL);
	inline FLOAT GetFPS() { return m_vFPS; }
};

typedef A3DCounter * PA3DCounter;

extern A3DCounter g_theA3DCounter;
#endif