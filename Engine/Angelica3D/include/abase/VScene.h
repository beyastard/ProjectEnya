/*
 * FILE: VWorld.h
 *
 * DESCRIPTION: class representing the whole world
 *
 * CREATED BY: Hedi, 2002/7/29
 *
 * HISTORY:
 *
 * Copyright (c) 2002-2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _VWORLD_H_
#define _VWORLD_H_

#include <A3D.h>
#include "VSSetCommon.h"
#include "VSSetSceneRender.h"

class VWorld
{
private:
	CVSSetSceneRender		m_VSRenderer;

protected:

public:
	VWorld();
	~VWorld();

	bool Init();
	bool Release();
};

#endif//_VWORLD_H_