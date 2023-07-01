#include "A3DCounter.h"
#include "A3DTime.h"

A3DCounter g_theA3DCounter;
__int64 A3DCounter::m_nCPUFrequency = 1;

A3DCounter::A3DCounter()
{
	m_dwFPSLastTick  = 0;
	m_vFPS = 60.0f;
	m_dwFPSCounter = 0;
	__int64 nMS1 = GetMicroSecondNow();
	__int64 nCycle1 = GetCPUCycle();
	while( GetMicroSecondNow() - nMS1 < 1000 )
	{
	}
	__int64 nMS2 = GetMicroSecondNow();
	__int64 nCycle2 = GetCPUCycle();
	m_nCPUFrequency = (nCycle2 - nCycle1) * 1000000 / (nMS2 - nMS1);
}

A3DCounter::~A3DCounter()
{
}

bool A3DCounter::Init()
{
	m_dwFPSLastTick = A3DGetTime();
	m_vFPS = 60.0f;
	m_dwFPSCounter = 0;
	return true;
}

bool A3DCounter::Release()
{
	return true;
}

bool A3DCounter::ResetFPSCounter()
{
	m_dwFPSLastTick = A3DGetTime();
	m_vFPS = 60.0f;
	m_dwFPSCounter = 0;

	return true;
}

bool A3DCounter::UpdateFPSCounter(bool * pFPSChanged)
{
	DWORD dwTickNow = A3DGetTime();

	m_dwFPSCounter ++;

	if( dwTickNow > m_dwFPSLastTick + 1000 )
	{
		m_vFPS				= m_dwFPSCounter * 1000.0f / (dwTickNow - m_dwFPSLastTick);
		m_dwFPSLastTick		= dwTickNow;
		m_dwFPSCounter		= 0;

		if( pFPSChanged )
			*pFPSChanged = true;
	}
	else
	{
		if( pFPSChanged )
			*pFPSChanged = false;
	}
	
	return true;
}

__int64 A3DCounter::GetMicroSecondNow()
{
	static LARGE_INTEGER frequency;
	static bool bFirstTime = true;

	if( bFirstTime )
	{
		if( !QueryPerformanceFrequency(&frequency) )
			return 0;
		bFirstTime = false;
	}
	
	LARGE_INTEGER large_int;
	if( !QueryPerformanceCounter(&large_int) )
		return 0;

	return (large_int.QuadPart * 1000000 / frequency.QuadPart);
	return (__int64)(large_int.QuadPart * 1000000.0 / frequency.QuadPart);
}

__int64 A3DCounter::GetCPUCycle()
{
	//	RDTSC store time stamp in EDX:EAX, here we only use low 32-bit, because
	//	we only care the delta between two stamps, which won't be very large.
	DWORD dwTime1;
	DWORD dwTime2;

    __asm
	{
		RDTSC
		mov		dwTime1, edx
		mov		dwTime2, eax
	}

	return (__int64(dwTime1) << 32) + dwTime2;
}

__int64 A3DCounter::GetCPUFrequency()
{
	return m_nCPUFrequency;
}