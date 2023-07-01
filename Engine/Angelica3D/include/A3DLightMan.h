/*
 * FILE: A3DLightMan.h
 *
 * DESCRIPTION: The managing class of lights;
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLIGHTMAN_H_
#define _A3DLIGHTMAN_H_

#include "A3DPlatform.h"
#include "AList.h"
#include "A3DData.h"
#include "A3DLight.h"
#include "A3DDevice.h"
#include "A3DTypes.h"

class A3DLightMan : public A3DData
{
private:
	AList		m_LightList;
	A3DDevice * m_pA3DDevice;

protected:

public:
	A3DLightMan();
	~A3DLightMan();

	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	bool Reset();
	bool RestoreLightResource(); // Restore all light resource when transition from lost state to operational state;

	bool CreateLight(int nLightID, A3DLight ** ppA3DLight);
	bool CreateDirecionalLight(int nLightID, A3DLight ** ppA3DLight, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DCOLORVALUE diffuse, A3DCOLORVALUE specular, A3DCOLORVALUE ambient);
	bool CreateSpotLight(int nLightID, A3DLight ** ppA3DLight, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DCOLORVALUE diffuse, A3DCOLORVALUE specular, A3DCOLORVALUE ambient, FLOAT range, FLOAT falloff, FLOAT theta, FLOAT phi, FLOAT attenuation);
	bool CreatePointLight(int nLightID, A3DLight ** ppA3DLight, A3DVECTOR3 vecPos, A3DCOLORVALUE diffuse, A3DCOLORVALUE specular, A3DCOLORVALUE ambient, FLOAT range, FLOAT attenuation);
	bool SetAmbient(A3DCOLOR colorAmbient);

	bool ReleaseLight(PA3DLight& pA3DLight);

	inline AList * GetLightList() { return &m_LightList; }
};

typedef A3DLightMan * PA3DLightMan;
#endif