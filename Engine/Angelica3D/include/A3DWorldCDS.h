/*
 * FILE: A3DWorldCDS.h
 *
 * DESCRIPTION: The class that collect the collision detection interface of A3DWorld
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DWORLDCDS_H_
#define _A3DWORLDCDS_H_

#include "A3DTypes.h"
#include "A3DCDS.h"

class A3DWorld;

class A3DWorldCDS : public A3DCDS
{
protected:
	A3DWorld *			m_pA3DWorld;

public:
	A3DWorldCDS();
	~A3DWorldCDS();

	bool Init(A3DWorld * pA3DWorld);
	bool Release();

	//Return true if collision, false not collide;
	virtual bool RayTrace(A3DVECTOR3& vecStart, A3DVECTOR3& vecVelocity, FLOAT vTime, RAYTRACE * pRayTrace, A3DModel * pModelExclude);
	virtual bool OBBTrace(A3DVECTOR3& vecStart, A3DVECTOR3& vecVelocity, FLOAT vTime, OBBSHAPE& obbShape, OBBTRACE * pOBBTrace, A3DModel * pModelExclude, bool bFirstTime);
	virtual bool AABBTrace(A3DVECTOR3& vStart, A3DVECTOR3& vExts, A3DVECTOR3& vVelocity, FLOAT fTime, AABBTRACE* pTrace, A3DModel * pModelExclude);

	virtual void DisableModelRayTrace();
	virtual void EnableModelRayTrace();
	virtual void SetModelRayTraceMask(DWORD dwMask);
	virtual void SetModelAABBTraceMask(DWORD dwMask);
	virtual void ClearModelRayTraceMask(DWORD dwMask);
	virtual void ClearModelAABBTraceMask(DWORD dwMask);
	virtual DWORD GetModelRayTraceMask();
	virtual DWORD GetModelAABBTraceMask();

	virtual A3DESP * GetA3DESP();
	virtual A3DTerrain * GetA3DTerrain();
};

#endif//_A3DCSD_H_
