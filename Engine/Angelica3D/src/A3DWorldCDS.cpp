/*
 * FILE: A3DWorldCDS.cpp
 *
 * DESCRIPTION: The class that collect the collision detection interface of A3DWorld
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DTypes.h"
#include "A3DWorld.h"
#include "A3DESP.h"
#include "A3DTerrain.h"
#include "A3DWorldCDS.h"

A3DWorldCDS::A3DWorldCDS()
{
	m_pA3DWorld = NULL;
}

A3DWorldCDS::~A3DWorldCDS()
{
}

bool A3DWorldCDS::Init(A3DWorld * pA3DWorld)
{
	m_pA3DWorld = pA3DWorld;
	return true;
}

bool A3DWorldCDS::Release()
{
	return true;
}

bool A3DWorldCDS::RayTrace(A3DVECTOR3& vecStart, A3DVECTOR3& vecVelocity, FLOAT vTime, RAYTRACE * pRayTrace, A3DModel * pModelExclude)
{
	return m_pA3DWorld->RayTrace(vecStart, vecVelocity, vTime, pRayTrace, pModelExclude);
}

bool A3DWorldCDS::OBBTrace(A3DVECTOR3& vecStart, A3DVECTOR3& vecVelocity, FLOAT vTime, OBBSHAPE& obbShape, OBBTRACE * pOBBTrace, A3DModel * pModelExclude, bool bFirstTime)
{
	return m_pA3DWorld->OBBTrace(vecStart, vecVelocity, vTime, obbShape, pOBBTrace, pModelExclude, bFirstTime);
}

bool A3DWorldCDS::AABBTrace(A3DVECTOR3& vStart, A3DVECTOR3& vExts, A3DVECTOR3& vVelocity, FLOAT fTime, AABBTRACE* pTrace, A3DModel * pModelExclude)
{
	return m_pA3DWorld->AABBTrace(vStart, vExts, vVelocity, fTime, pTrace, pModelExclude);
}

A3DESP * A3DWorldCDS::GetA3DESP()
{
	return m_pA3DWorld->GetA3DESP();
}

A3DTerrain * A3DWorldCDS::GetA3DTerrain()
{
	return m_pA3DWorld->GetA3DTerrain();
}

void A3DWorldCDS::DisableModelRayTrace()
{
	m_pA3DWorld->DisableModelRayTrace();
}

void A3DWorldCDS::EnableModelRayTrace()
{
	m_pA3DWorld->EnableModelRayTrace();
}

void A3DWorldCDS::SetModelRayTraceMask(DWORD dwMask)
{
	m_pA3DWorld->SetModelRayTraceMask(dwMask);
}

void A3DWorldCDS::SetModelAABBTraceMask(DWORD dwMask)
{
	m_pA3DWorld->SetModelAABBTraceMask(dwMask);
}

void A3DWorldCDS::ClearModelRayTraceMask(DWORD dwMask)
{
	m_pA3DWorld->ClearModelRayTraceMask(dwMask);
}

void A3DWorldCDS::ClearModelAABBTraceMask(DWORD dwMask)
{
	m_pA3DWorld->ClearModelAABBTraceMask(dwMask);
}

DWORD A3DWorldCDS::GetModelRayTraceMask()
{
	return m_pA3DWorld->GetModelRayTraceMask();
}

DWORD A3DWorldCDS::GetModelAABBTraceMask()
{
	return m_pA3DWorld->GetModelAABBTraceMask();
}