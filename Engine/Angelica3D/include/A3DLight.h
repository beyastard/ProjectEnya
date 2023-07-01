/*
 * FILE: A3DLight.h
 *
 * DESCRIPTION: Class representing for a light in A3D Engine;
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLIGHT_H_
#define _A3DLIGHT_H_

#include "A3DPlatform.h"
#include "A3DObject.h"
#include "A3DTypes.h"
#include "A3DDevice.h"

class A3DLight : public A3DObject
{
private:
	A3DLIGHTTYPE		m_Type;
	A3DLIGHTPARAM		m_LightParam;
	bool				m_bOn;
	int					m_nLightID;

	A3DDevice *			m_pA3DDevice;
protected:
public:
	A3DLight();
	~A3DLight();
	bool Init(A3DDevice * pDevice, int nLightID);
	bool Release();

	bool TurnOn();
	bool TurnOff();
	bool SetLightParam(A3DLIGHTPARAM * pLightParam);
	A3DLIGHTPARAM GetLightparam()	{	return m_LightParam;	}
	inline bool IsOn() { return m_bOn; }
};

typedef A3DLight * PA3DLight;
#endif