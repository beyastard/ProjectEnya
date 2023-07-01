/*
 * FILE: A3DCDS.h
 *
 * DESCRIPTION: The class that collect the collision detection interface
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCDS_H_
#define _A3DCDS_H_

#include "A3DTypes.h"
#include "A3DCollision.h"

class A3DDevice;
class A3DESP;
class A3DModel;

class A3DCDS
{
public:
	A3DCDS() {}
	~A3DCDS() {}

	//Return true if collision, false not collide;
	virtual bool RayTrace(A3DVECTOR3& vecStart, A3DVECTOR3& vecVelocity, FLOAT vTime, RAYTRACE * pRayTrace, A3DModel * pModelExclude) = 0;
	virtual bool OBBTrace(A3DVECTOR3& vecStart, A3DVECTOR3& vecVelocity, FLOAT vTime, OBBSHAPE& obbShape, OBBTRACE * pOBBTrace, A3DModel * pModelExclude, bool bFirstTime) = 0;
	virtual bool AABBTrace(A3DVECTOR3& vStart, A3DVECTOR3& vExts, A3DVECTOR3& vVelocity, FLOAT fTime, AABBTRACE* pTrace, A3DModel * pModelExclude) = 0;

	virtual A3DESP * GetA3DESP() = 0;	 
	virtual A3DTerrain * GetA3DTerrain() = 0;

	virtual void DisableModelRayTrace() = 0;
	virtual void EnableModelRayTrace() = 0;
	virtual void SetModelRayTraceMask(DWORD dwMask) = 0;
	virtual void SetModelAABBTraceMask(DWORD dwMask) = 0;
	virtual void ClearModelRayTraceMask(DWORD dwMask) = 0;
	virtual void ClearModelAABBTraceMask(DWORD dwMask) = 0;
	virtual DWORD GetModelRayTraceMask() = 0;
	virtual DWORD GetModelAABBTraceMask() = 0;
};

#endif//_A3DCSD_H_
