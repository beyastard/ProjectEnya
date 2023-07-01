/*
 * FILE: A3DTime.h
 *
 * DESCRIPTION: A class used to get some kind of time in A3D Engine
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTIME_H_
#define _A3DTIME_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"

void A3DGetSystemTime(WORD * pYear, WORD * pMonth, WORD * pDay, WORD * pDate, WORD * pHour, WORD * pMinute, WORD * pSecond);
DWORD A3DGetTime();

#endif
