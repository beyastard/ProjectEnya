/*
 * FILE: ASky.H
 *
 * DESCRIPTION : Wrap of A3DSky ,A3DStars and A3DLamp classes
 *
 * CREATED BY: MengXin
 *
 * HISTORY:	2002 - 8 - 14
 *
 * Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
 */

#ifndef _ASKY_H_
#define _ASKY_H_

#include "ABaseA3D.h"
#include "AWorldInit.h"

class ASky
{
	float					m_fTickAnimationTime;
	A3DDevice *				m_pA3DDevice;
	A3DSkyBox *				m_pA3DBoxSky;
	A3DSkyGradient*			m_pA3DGradientSky;
	A3DSkySphere*			m_pA3DSphereSky;

public:

	inline A3DSkyGradient *	GetA3DSkyGradient(){return m_pA3DGradientSky;}
	inline A3DSkyBox *		GetA3DSkyBox(){return m_pA3DBoxSky;}
	inline A3DSkySphere *	GetA3DSkySphere(){return m_pA3DSphereSky;}

	inline void BindDevice(A3DDevice *  pD){m_pA3DDevice	= pD;}
	inline bool BindFinished()const{return m_pA3DDevice !=0;}
	ASky();
	virtual bool Create(const PAWORLD_INIT_SKY);
	virtual bool Create(ASkyInit*);
	virtual bool Release();
	virtual bool TickAnimation(float _ms);
	virtual ~ASky();
};

#endif//_ASKY_H_