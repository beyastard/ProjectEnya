/*
 * FILE: A3DMaterial.h
 *
 * DESCRIPTION: Class that representing one material;
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMATERIAL_H_
#define _A3DMATERIAL_H_

#include "A3DPlatform.h"
#include "A3DFuncs.h"
#include "A3DObject.h"
#include "A3DDevice.h"

class A3DMaterial : public A3DObject
{
private:
	A3DMATERIALPARAM	m_MaterialParam;
	bool				m_b2Sided;
	bool				m_bAlphaMaterial;
	
	A3DDevice *			m_pA3DDevice;
public:
	A3DMaterial();
	~A3DMaterial();

	bool SetValues(A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShininess, bool b2Sided=false);
	bool SetDiffuse(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
	bool Init(A3DDevice * pDevice);
	bool Init(A3DDevice * pDevice, A3DCOLORVALUE clAmbient, A3DCOLORVALUE clDiffuse, A3DCOLORVALUE clEmissive, A3DCOLORVALUE clSpecular, FLOAT fShiness);
	bool Init(A3DDevice * pDevice, A3DMATERIALPARAM& Params);
	bool Release();
	bool Appear();

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pDevice, AFile * pFileToLoad);
	inline A3DMATERIALPARAM GetMaterialParam() { return m_MaterialParam; }
	inline void Set2Sided(bool b2Sides)	{	m_b2Sided = b2Sides;	}
	inline void SetAlphaMaterial(bool bAlpha)	{	m_bAlphaMaterial = bAlpha;	}
	inline bool Is2Sided() { return m_b2Sided; }
	inline bool IsAlphaMaterial() { return m_bAlphaMaterial; }

	inline bool Match(A3DMaterial* pMesh);
};

typedef A3DMaterial * PA3DMaterial;

/*	Check whether specified material is same as this material.

	Return true if the two material match, otherwise return false.
*/
bool A3DMaterial::Match(A3DMaterial* pMaterial)
{
	A3DMATERIALPARAM Param = pMaterial->GetMaterialParam();
	
	if (m_MaterialParam.Ambient != Param.Ambient)
		return false;

	if (m_MaterialParam.Diffuse != Param.Diffuse)
		return false;

	if (m_MaterialParam.Specular != Param.Specular)
		return false;

	if (m_MaterialParam.Emissive != Param.Emissive)
		return false;

	if (m_MaterialParam.Power != Param.Power)
		return false;

	return true;
}


#endif