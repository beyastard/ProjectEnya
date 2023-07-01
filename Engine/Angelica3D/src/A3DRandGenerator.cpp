/*
 * FILE: A3DRandGenerator.cpp
 *
 * DESCRIPTION: An interface of a random number generator;
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DRandGenerator.h"

DWORD A3DRandGenerator::m_dwMultiplier = 16807;       // (7 ** 5)
DWORD A3DRandGenerator::m_dwModuler = 2147483647;  // (2 ** 31) - 1	
DWORD A3DRandGenerator::m_dwSeed = 765983;

A3DRandGenerator::A3DRandGenerator()
{
}

A3DRandGenerator::~A3DRandGenerator()
{
}
