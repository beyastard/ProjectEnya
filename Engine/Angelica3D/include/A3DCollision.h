/*
 * FILE: A3DCollision.h
 *
 * DESCRIPTION: Some common routines for collision detection
 *
 * CREATED BY: duyuxin, 2001/10/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCOLLISION_H_
#define _A3DCOLLISION_H_

#include "A3DFuncs.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

bool CLS_RayToAABB3(A3DVECTOR3& vStart, A3DVECTOR3& vDir, A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, A3DVECTOR3& vPoint, FLOAT* pfFraction, A3DVECTOR3& vNormal);
bool CLS_RayToAABB2(A3DVECTOR3& vStart, A3DVECTOR3& vDir, FLOAT vMins[2], FLOAT vMaxs[2], A3DVECTOR3& vPoint);
bool CLS_RayToOBB3(A3DVECTOR3& vStart, A3DVECTOR3& vDir, A3DOBB& OBB, A3DVECTOR3& vPoint, FLOAT* pfFraction, A3DVECTOR3& vNormal);
int CLS_PlaneToAABB(A3DVECTOR3& vNormal, FLOAT fDist, A3DVECTOR3& vOrigin, A3DVECTOR3& vExtents);
int CLS_PlaneToAABB(A3DPLANE& Plane, A3DVECTOR3& _vMins, A3DVECTOR3& _vMaxs);
int CLS_PlaneToSphere(A3DPLANE& Plane, A3DVECTOR3& vCenter, FLOAT fRadius);
bool CLS_TriangleToAABB(A3DVECTOR3& v0, A3DVECTOR3& v1, A3DVECTOR3& v2, A3DPLANE& Plane, A3DAABB& AABB);
bool CLS_RayToTriangle(A3DVECTOR3& vStart, A3DVECTOR3& vDir, A3DVECTOR3& v0, 
					   A3DVECTOR3& v1, A3DVECTOR3& v2, A3DVECTOR3& vPoint, bool b2Sides);
bool CLS_AABBToAABB(A3DVECTOR3& vCenter1, A3DVECTOR3& vExt1, A3DVECTOR3& vCenter2, A3DVECTOR3& vExt2);
bool CLS_RayToSphere(A3DVECTOR3& vStart, A3DVECTOR3& vDelta, A3DVECTOR3& vOrigin, FLOAT fRadius);
bool CLS_RayToPlane(A3DVECTOR3& vStart, A3DVECTOR3& vDelta, A3DVECTOR3& vPlaneNormal, float fDist, bool b2Sides, A3DVECTOR3& vHitPos, float* pfFraction);
bool CLS_OBBToQuadrangle(A3DOBB& OBB, A3DVECTOR3& ov0, A3DVECTOR3& ov1, A3DVECTOR3& ov2,
						 A3DVECTOR3& ov3, A3DVECTOR3& vNormal, FLOAT* pfFraction);
int CLS_AABBMoveToPlane(A3DVECTOR3& vStart, A3DVECTOR3& vEnd, A3DVECTOR3& vExts, A3DPLANE& Plane, float* pfFraction);

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

/*	Check whether a plane collision with a sphere.

	Return value:
		
		-1: sphere is behide of plane
		0: sphere cross with plane
		1: sphere is in front of plane

	vNormal: plane's normal.
	fDist: d parameter of plane equation
	vCenter: sphere's center
	fRadius: sphere's radius
*/
inline int CLS_PlaneToSphere(A3DVECTOR3& vNormal, FLOAT fDist, A3DVECTOR3& vCenter, FLOAT fRadius)
{
	float fDelta = DotProduct(vCenter, vNormal) - fDist;

	if (fDelta > fRadius)
		return 1;
	else if (fDelta < -fRadius)
		return -1;
	else
		return 0;
}

#endif	//	_A3DCOLLISION_H_

