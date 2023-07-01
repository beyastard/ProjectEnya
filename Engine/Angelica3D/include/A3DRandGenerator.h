/*
 * FILE: A3DRandGenerator.h
 *
 * DESCRIPTION: An interface of a random number generator;
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DRANDGENERATOR_H_
#define _A3DRANDGENERATOR_H_

#include "A3DPlatform.h"

class A3DRandGenerator
{
private:
	static DWORD m_dwMultiplier;
	static DWORD m_dwModuler;
	static DWORD m_dwSeed;

protected:

public:
	A3DRandGenerator();
	~A3DRandGenerator();

	// Set the seed of the random number generator;
	inline static void SetSeed(DWORD dwSeed) { m_dwSeed = dwSeed; }
	// Return current seed;
	inline static DWORD GetSeed() { return m_dwSeed; }
	// Generate Next int random number, ranges[0, m_dwModuler);
	inline static DWORD RandUniformInt();
	// Generate Next float random number with a uniformity distribution, ranges [a, b]
	inline static FLOAT RandUniformFloat(FLOAT a, FLOAT b);
	// Generate Next float random number with a normal distribution, ranges [a, b]
	inline static FLOAT RandNormalFloat(FLOAT a, FLOAT b, FLOAT vVariation);
};

DWORD A3DRandGenerator::RandUniformInt()
{
	DWORD dwRandUniform = DWORD(((__int64)m_dwMultiplier * m_dwSeed) % (__int64)m_dwModuler);
	m_dwSeed = dwRandUniform;
	return dwRandUniform;
}

FLOAT A3DRandGenerator::RandUniformFloat(FLOAT a, FLOAT b)
{
	DWORD dwRandUniform = RandUniformInt();

	return a + (b - a) * (dwRandUniform * 1.0f / m_dwModuler);
}

FLOAT A3DRandGenerator::RandNormalFloat(FLOAT a, FLOAT b, FLOAT vVariation)
{
	return 0.0f;
}

#endif//_A3DRANDGENERATOR_H_