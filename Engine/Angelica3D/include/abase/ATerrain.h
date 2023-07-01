/*
 * FILE: ATERRAIN.H
 *
 * DESCRIPTION : Wrap of A3DTerrain class
 *
 * CREATED BY: MengXin
 *
 * HISTORY:	2002 - 8 - 14
 *
 * Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
 */

#ifndef _ATERRAIN_H_
#define _ATERRAIN_H_

#include "ABaseA3D.h"
#include "AWorldInit.h"

class ATerrain
{

	A3DDevice *				m_pA3DDevice;
	A3DTerrain *			m_pA3DTerrain;

public:

	inline A3DTerrain * GetTerrain(){return m_pA3DTerrain;}
	inline void BindDevice(A3DDevice *  pD){m_pA3DDevice= pD;}
	inline bool BindFinished(){return m_pA3DDevice !=0;}
	ATerrain();
	virtual ~ATerrain();
	virtual bool Create(const PAWORLD_INIT_TERRAIN);
	virtual bool Create(ATerrainInit*);
	virtual bool Release();
};

#endif//_ATERRAIN_H_