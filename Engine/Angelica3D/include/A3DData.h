/*
 * FILE: A3DData.h
 *
 * DESCRIPTION: Base class for any class that can be saved onto disk
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DDATA_H_
#define _A3DDATA_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"

class A3DData
{
private:
protected:
public:
	A3DData();
	~A3DData();
};

typedef A3DData * PA3DData;
#endif