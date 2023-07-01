/*
 * FILE: A3DAABBTrace.cpp
 *
 * DESCRIPTION: Routines for AABB trace, part of A3D trace routines
 *
 * CREATED BY: duyuxin, 2001/10/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DErrLog.h"
#include "A3DCollision.h"
#include "A3DTrace.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	AABB trace type
#define AABBTRACE_STATIC	0	//	Not move
#define AABBTRACE_MOVING	1

#define EPSILON_FRACTION	0.03125f

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

bool TRA_MovingAABBToBrush(PAABBTRACEINFO pInfo, A3DPLANE** aPlanes, int iNumPlane);
bool TRA_StaticAABBToBrush(PAABBTRACEINFO pInfo, A3DPLANE** aPlanes, int iNumPlane);

///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

/*	Build OBB bevels for AABB trace.

	OBB: OBB information.
	pBevels: buffer used to store bevels
*/
void TRA_BuildOBBBevels(A3DOBB& OBB, OBBBEVELS* pBevels)
{
	A3DVECTOR3 vVert, vNormal;
	A3DVECTOR3* aAxis[3];
	int i, j, n = 0;
	A3DPLANE* pPlane;
	float fDist;

	aAxis[0] = &OBB.XAxis;
	aAxis[1] = &OBB.YAxis;
	aAxis[2] = &OBB.ZAxis;

	vVert = OBB.Center + OBB.ExtX + OBB.ExtY + OBB.ExtZ;

	//	6 side planes ----------------------------
	for (i=0; i < 3; i++)
	{
		//	Positive side
		pPlane = &pBevels->aPlanes[n++];
		pPlane->vNormal = *aAxis[i];
		pPlane->fDist	= fDist = DotProduct(vVert, pPlane->vNormal);
		
		//	Negative side
		pPlane = &pBevels->aPlanes[n++];
		pPlane->vNormal = -(*aAxis[i]);
		pPlane->fDist	= -(fDist - OBB.Extents.m[i] * 2.0f);
	}

	//	If this OBB is a AABB, these are enough
	for (i=0; i < 2; i++)
	{
		for (j=0; j < 3; j++)
		{
			if (aAxis[i]->m[j] == 1.0f || aAxis[i]->m[j] == -1.0f)
				break;
		}

		if (j >= 3)
			break;
	}

	if (i >= 2)		//	Ok, two axis are axial
		goto End;

	//	8 vertex planes --------------------------
	for (i=-1; i <= 1; i+=2)
	{
		for (j=-1; j <= 1; j+=2)
		{
			vVert = OBB.Center + OBB.ExtX*(float)i + OBB.ExtY*(float)j - OBB.ExtZ;

			pPlane = &pBevels->aPlanes[n++];
			pPlane->vNormal = Normalize(OBB.XAxis*(float)i + OBB.YAxis*(float)j - OBB.ZAxis);
			pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);
			
			vVert = OBB.Center + OBB.ExtX*(float)i + OBB.ExtY*(float)j + OBB.ExtZ;

			pPlane = &pBevels->aPlanes[n++];
			pPlane->vNormal = Normalize(OBB.XAxis*(float)i + OBB.YAxis*(float)j + OBB.ZAxis);
			pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);
		}
	}

	//	12 border planes --------------------------
	//	Pair 1 ---------
	vVert  = OBB.Center + OBB.ExtX + OBB.ExtY;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.XAxis + OBB.YAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtX - OBB.ExtY;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 2---------
	vVert  = OBB.Center + OBB.ExtX - OBB.ExtY;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.XAxis - OBB.YAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtX + OBB.ExtY;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 3 --------
	vVert  = OBB.Center + OBB.ExtX + OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.XAxis + OBB.ZAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtX - OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 4 --------
	vVert  = OBB.Center + OBB.ExtX - OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.XAxis - OBB.ZAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtX + OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 5 --------
	vVert  = OBB.Center + OBB.ExtY + OBB.ExtZ;
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.YAxis + OBB.ZAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtY - OBB.ExtZ;
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 6 --------
	vVert  = OBB.Center + OBB.ExtY - OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.YAxis - OBB.ZAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtY + OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

End:

	pBevels->iNumPlane = n;

	for (i=0; i < n; i++)
	{
		pBevels->aPlaneAddrs[i] = &pBevels->aPlanes[i];
		pBevels->aPlanes[i].bySignBits = GetA3DPlaneSignBits(pBevels->aPlanes[i].vNormal);
	}
}

/*	Initialize data for AABB trace.

	pInfo: used to receive initalized data.
	vStart: start point of AABB's center
	vExts: AABB's half extents
	vVelocity: AABB's velocity.
	fTime: time.
*/
void TRA_AABBTraceInit(PAABBTRACEINFO pInfo, A3DVECTOR3& vStart, A3DVECTOR3& vExts,
					   A3DVECTOR3& vVelocity, FLOAT fTime)
{
	A3DVECTOR3 vDelta = vVelocity * fTime;

	pInfo->vStart		= vStart;
	pInfo->vExtents		= vExts;
	pInfo->vDelta		= vDelta;
	pInfo->vEnd			= vStart + vDelta;
	pInfo->fTime		= fTime;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;

	//	Calculate bounding box for whole sweep area
	A3DVECTOR3 vDest = pInfo->vEnd;

	//	Extents bound AABB a little is necessary! This ensure AABB won't
	//	be too near to sides in scene.
	pInfo->BoundAABB.Mins	 = Min(vDest - vExts, vStart - vExts) - 0.5f;
	pInfo->BoundAABB.Maxs	 = Max(vDest + vExts, vStart + vExts) + 0.5f;
	pInfo->BoundAABB.Center  = (pInfo->BoundAABB.Mins + pInfo->BoundAABB.Maxs) * 0.5f;
	pInfo->BoundAABB.Extents = pInfo->BoundAABB.Maxs - pInfo->BoundAABB.Center;

	if (vDelta.x == 0.0f && vDelta.y == 0.0f && vDelta.z == 0.0f)
		pInfo->iType = AABBTRACE_STATIC;
	else
		pInfo->iType = AABBTRACE_MOVING;
}

/*	Collision routine between AABB and a brush

	Return true if collision occurs, otherwise return false.

	pInfo: AABB trace information.
	aPlanes: plane pointer array contains brush's planes
	iNumPlane: number of plane pointer in aPlanes
*/
bool TRA_AABBMoveToBrush(PAABBTRACEINFO pInfo, A3DPLANE** aPlanes, int iNumPlane)
{
	if (!iNumPlane)
		return false;

	bool bRet;

	if (pInfo->iType == AABBTRACE_STATIC)
		bRet = TRA_StaticAABBToBrush(pInfo, aPlanes, iNumPlane);
	else
		bRet = TRA_MovingAABBToBrush(pInfo, aPlanes, iNumPlane);

	return bRet;
}

/*	Collision routine when AABB move to a brush (convex polyhedron)

	Return true if collision occurs, otherwise return false.

	vStart: start position of aabb's center
	vEnd: end position of aabb's ceter
	vExts: aabb's half extents
	aPlanes: plane pointer array contains brush's planes
	iNumPlane: number of plane pointer in aPlanes
	ppClipPlane (out): used to receive clip plane's pointer, this parameter is valid
					   only when 1 is returned.
	pfFraction (out): used to receive moving fraction of aabb.
*/
bool TRA_MovingAABBToBrush(PAABBTRACEINFO pInfo, A3DPLANE** aPlanes, int iNumPlane)
{
	int	i;
	A3DPLANE* pPlane, *pClipPlane;
	float fDist, fEnterFrac, fLeaveFrac, d1, d2, fFrac;
	A3DVECTOR3 vOffs, vExts;
	bool bGetOut, bStartOut;

	pInfo->fFraction	= 1.0f;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;

	fEnterFrac	= -1e7f;
	fLeaveFrac	= 1e7f;
	pClipPlane	= NULL;
	bGetOut		= false;
	bStartOut	= false;
	vExts		= pInfo->vExtents;

	for (i=0 ; i < iNumPlane; i++)
	{
		pPlane = aPlanes[i];

		//	Push the plane out apropriately for mins/maxs
		switch (pPlane->bySignBits)
		{
		case 0:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 1:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 2:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 3:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 4:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 5:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 6:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		case 7:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		default: return false;
		}
		
		fDist = pPlane->fDist - DotProduct(vOffs, pPlane->vNormal);
		d1 = DotProduct(pInfo->vStart, pPlane->vNormal) - fDist;
		d2 = DotProduct(pInfo->vEnd, pPlane->vNormal) - fDist;

		if (d2 > 0.0f)
			bGetOut = true;		//  Endpoint is not in solid

		if (d1 > 0.0f)
			bStartOut = true;

		//	If completely in front of face, no intersection
		if (d1 > 0.0f && d2 >= d1)
			return 0;

		//	If completely behide face, no intersection
		if (d1 <= 0.0f && d2 <= 0.0f)
			continue;

		//	Crosses face
		if (d1 > d2)
		{
			//	Enter
			fFrac = (d1 - EPSILON_FRACTION) / (d1 - d2);
			if (fFrac > fEnterFrac)
			{
				fEnterFrac = fFrac;
				pClipPlane = pPlane;
			}
		}
		else
		{	//	Leave
			fFrac = (d1 + EPSILON_FRACTION) / (d1 - d2);
			if (fFrac < fLeaveFrac)
				fLeaveFrac = fFrac;
		}
	}

	if (!bStartOut)
	{
		//	Ooriginal point was inside brush
		pInfo->bStartSolid = true;

		if (!bGetOut)
			pInfo->bAllSolid = true;

	//	pInfo->fFraction = 0.0f;
	//	pInfo->ClipPlane = *aPlanes[0];	//	This isn't correct, but better than random
	//	return true;
		return false;
	}

	if (fEnterFrac < fLeaveFrac)
	{
	//	if (fEnterFrac > -1.0f && fEnterFrac < 1.0f)
		if (fEnterFrac != -1e7 && fEnterFrac < 1.0f)
		{
			if (fEnterFrac < 0.0f)
				fEnterFrac = 0.0f;

			pInfo->fFraction = fEnterFrac;
			pInfo->ClipPlane = *pClipPlane;
			return true;
		}
	}

	return false;
}

/*	Collision routine of static AABB and brush (convex polyhedron)

	Return true if collision occurs, otherwise return false.

	vStart: start position of aabb's center
	vExts: aabb's half extents
	aPlanes: plane pointer array contains brush's planes
	iNumPlane: number of plane pointer in aPlanes
*/
bool TRA_StaticAABBToBrush(PAABBTRACEINFO pInfo, A3DPLANE** aPlanes, int iNumPlane)
{
	int	i;
	A3DPLANE* pPlane;
	float fDist, d1;
	A3DVECTOR3 vOffs, vExts;

	pInfo->fFraction	= 1.0f;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;

	vExts = pInfo->vExtents;

	for (i=0; i < iNumPlane; i++)
	{
		pPlane = aPlanes[i];

		//	Push the plane out apropriately for mins/maxs
		switch (pPlane->bySignBits)
		{
		case 0:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 1:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 2:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 3:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 4:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 5:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 6:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		case 7:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		default: return false;
		}
		
		fDist = pPlane->fDist - DotProduct(vOffs, pPlane->vNormal);
		d1 = DotProduct(pInfo->vStart, pPlane->vNormal) - fDist;

		//	If completely in front of face, no intersection
		if (d1 > 0.0f)
			return false;
	}

	//	Inside this brush
	pInfo->bStartSolid	= true;
	pInfo->bAllSolid	= true;
	pInfo->fFraction	= 0.0f;
	pInfo->ClipPlane	= *aPlanes[0];	//	This isn't correct, but better than random

	return true;
}

