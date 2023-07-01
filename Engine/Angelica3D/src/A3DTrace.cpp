/*
 * FILE: A3DTrace.cpp
 *
 * DESCRIPTION: Routines for trace algorithm and file
 *
 * CREATED BY: duyuxin, 2001/11/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DErrLog.h"
#include "A3DTrace.h"
#include "A3DWorld.h"
#include <assert.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define	MAX_MOVECLIP_PLANES		10

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


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

/*	Initlaize OBB for trace.

	pOBB (out): initialized OBB.
	vStart: start point
	vVelocity: velocity
	fTime: time
	Shape: shape used to build OBB
*/
void TRA_OBBTraceInit(PA3DOBB pOBB, A3DVECTOR3& vStart, A3DVECTOR3& vVelocity, FLOAT fTime, OBBSHAPE& Shape)
{
	//	Build OBB coordinates
	float fDotProduct, fLength;
	A3DVECTOR3 vUp, vView;

	fLength = Magnitude(vVelocity);

	//	Normalize the z basis vector
	if (fLength < 1e-6f)
	{
		vView	= A3DVECTOR3(0, 0, 1);
		vUp		= A3DVECTOR3(0, 1, 0);
		fLength	= 0;
	}
	else
	{
		vView = vVelocity / fLength;

		if (1.0f - vView.y < 1e-6f || vView.y + 1.0f < 1e-6f)
			vUp = A3DVECTOR3(1, 0, 0);
		else
		{
			fDotProduct	= DotProduct(A3DVECTOR3(0, 1, 0), vView);
			vUp	= A3DVECTOR3(0, 1, 0) - fDotProduct * vView;
			vUp = Normalize(vUp);
		}
	}

	fLength *= fTime;

	pOBB->Center	= (vStart + vStart + vView * (Shape.fRadius + fLength)) * 0.5f;
	pOBB->ZAxis		= vView;
	pOBB->YAxis		= vUp;
	pOBB->XAxis		= CrossProduct(vUp, vView);
	pOBB->Extents.x	= Shape.fRadius;
	pOBB->Extents.y	= Shape.fRadius;
	pOBB->Extents.z	= (fLength + Shape.fRadius) * 0.5f;
	pOBB->ExtX		= pOBB->XAxis * pOBB->Extents.x;
	pOBB->ExtY		= pOBB->YAxis * pOBB->Extents.y;
	pOBB->ExtZ		= pOBB->ZAxis * pOBB->Extents.z;
}

/*	Clip velocity along specified plane.

	vInVel: original velocity.
	vNormal: plane's normal
	vOutVel: result velocity.
	fOverBounce: over bounce.
*/
void TRA_ClipVelocity(A3DVECTOR3& vInVel, A3DVECTOR3& vNormal, A3DVECTOR3& vOutVel, float fOverBounce)
{
	float	fBackOff;
	
	fBackOff = DotProduct(vInVel, vNormal);
	
	if (fBackOff < 0)
		fBackOff *= fOverBounce;
	else
		fBackOff = 0.0f;// /= fOverBounce;
	
	vOutVel = vInVel - vNormal * fBackOff;

	if (Magnitude(vOutVel) > 1000)
		assert(0);
}

/*	Slide move

	Return true if something is on the way user want to move, otherwise return false
	if user can move to the position they want.

	pMove: move information.
	bStepTry: true, this time is a step try
*/
bool TRA_SlideMove(PA3DMOVE pMove, bool bStepTry)
{
	int			iBumpCount, iNumBumps, iNumPlanes;
	A3DVECTOR3	vDir, vClipVelocity;
	float		d, fTimeLeft, fInto;
	A3DVECTOR3	aPlanes[MAX_MOVECLIP_PLANES];
	int			i, j, k;
	AABBTRACE	Trace;

	iNumBumps	= 4;
	iNumPlanes	= 0;
	fTimeLeft	= pMove->fTime;

	if (bStepTry && pMove->vVelocity.y > 0.0f)
		pMove->vVelocity.y = 0.0f;
	else if (!bStepTry)
	{
/*		//	Never turn against the ground plane
		if (pMove->GroundType == TRACE_OBJECT_ESPMODEL ||
			pMove->GroundType == TRACE_OBJECT_TERRAIN)
		{
			iNumPlanes	= 1;
			aPlanes[0]	= pMove->GroundTrace.vNormal;
		}
*/	}

	//	Never turn against original velocity
	aPlanes[iNumPlanes] = Normalize(pMove->vVelocity);
	iNumPlanes++;

	pMove->vDest = pMove->vStart;

	for (iBumpCount=0; iBumpCount < iNumBumps; iBumpCount++)
	{
		//	See if we can make it there
		pMove->pWorld->AABBTrace(pMove->vDest, pMove->Shape.vExtents, pMove->vVelocity, 
								 fTimeLeft, &Trace, pMove->pModel);

		if (Trace.fFraction > 0.0f)
		{
			//	Actually covered some distance
			pMove->vDest = Trace.vDestPos;
		}

		if (Trace.fFraction == 1.0f)
			break;

		fTimeLeft -= fTimeLeft * Trace.fFraction;

		if (iNumPlanes >= MAX_MOVECLIP_PLANES)
		{
			//	This shouldn't really happen
			pMove->vVelocity = A3DVECTOR3(0.0f);
			return true;
		}

		//	If this is the same plane we hit before, nudge velocity
		//	out along it, which fixes some epsilon issues with
		//	non-axial planes
		for (i=0; i < iNumPlanes; i++)
		{
			if (DotProduct(Trace.vNormal, aPlanes[i]) > 0.99f)
			{
				pMove->vVelocity = pMove->vVelocity + Trace.vNormal * 0.1f;
				break;
			}
		}

		if (i < iNumPlanes)
			continue;
		
		aPlanes[iNumPlanes] = Trace.vNormal;
		iNumPlanes++;

		//	Modify velocity so it parallels all of the clip planes
		
		//	Find a plane that it enters
		for (i=0; i < iNumPlanes; i++)
		{
			fInto = DotProduct(pMove->vVelocity, aPlanes[i]);
			if (fInto >= 0.0001f)
				continue;	//	Move doesn't interact with the plane
			
			//	Slide along the plane
			TRA_ClipVelocity(pMove->vVelocity, aPlanes[i], vClipVelocity, MOVE_OVERBOUNCE);

			//	See if there is a second plane that the new move enters
			for (j=0; j < iNumPlanes; j++)
			{
				if (j == i)
					continue;
				
				if (DotProduct(vClipVelocity, aPlanes[j]) >= 0.0001f)
					continue;	//	Move doesn't interact with the plane
				
				//	Try clipping the move to the plane
				TRA_ClipVelocity(vClipVelocity, aPlanes[j], vClipVelocity, MOVE_OVERBOUNCE);

				//	See if it goes back into the first clip plane
				if (DotProduct(vClipVelocity, aPlanes[i]) >= 0.0001f)
					continue;
				
				//	Slide the original velocity along the crease
				vDir = CrossProduct(aPlanes[i], aPlanes[j]);
				vDir = Normalize(vDir);
				d	 = DotProduct(vDir, pMove->vVelocity);
				vClipVelocity = vDir * d;

				//	See if there is a third plane the the new move enters
				for (k=0; k < iNumPlanes; k++)
				{
					if (k == i || k == j)
						continue;
					
					if (DotProduct(vClipVelocity, aPlanes[k]) >= 0.0001f)
						continue;	//	Move doesn't interact with the plane
					
					//	Stop dead at a tripple plane interaction
					pMove->vVelocity = A3DVECTOR3(0.0f);
					return true;
				}
			}

			if (bStepTry && vClipVelocity.y > 0.0f)
				vClipVelocity.y = 0.0f;

			//	If we have fixed all interactions, try another move
			pMove->vVelocity = vClipVelocity;
			break;
		}
	}

	return (iBumpCount != 0);
}

/*	Step slide move.

	pMove: move information.
	bGravity: this parameter isn't used now.
			  true, conside gravity, false, don't conside gravity
*/
void TRA_StepSlideMove(PA3DMOVE pMove, bool bGravity)
{
	A3DVECTOR3 vStartOri, vStartVel, vVelocity, vUp(0, 1, 0);
	A3DVECTOR3 vTempPos, vTempVel;
	AABBTRACE Trace;

	vStartOri = pMove->vStart;
	vStartVel = pMove->vVelocity;

	if (!TRA_SlideMove(pMove, false))
		return;		//	we got exactly where we wanted to go first try	

	vVelocity = A3DVECTOR3(0, -0.5f, 0);
	pMove->pWorld->AABBTrace(vStartOri, pMove->Shape.vExtents, vVelocity, 1.0f, &Trace, pMove->pModel);

	//	never step up when you still have up velocity
	float fHeight = vStartOri.y - pMove->Shape.vExtents.y - pMove->pWorld->GetA3DTerrain()->GetHeight(vStartOri);
	if (Trace.fFraction == 1.0f && fHeight > 0.5f)
		return;

	vUp		= vStartOri;
//	vUp.y  += pMove->fStepSize;

	//	test the player position if they were a stepheight higher
	vVelocity = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	pMove->pWorld->AABBTrace(vUp, pMove->Shape.vExtents, vVelocity, 1.0f, &Trace, pMove->pModel);

	if (Trace.fFraction != 1.0f)
		return;

	//	try slidemove from this position
	pMove->vStart	 = Trace.vDestPos;
	pMove->vVelocity = vStartVel;

	TRA_SlideMove(pMove, true);

	vTempPos = pMove->vDest;
	vTempVel = pMove->vVelocity;

	//	push down the final amount
	vVelocity = A3DVECTOR3(0, -pMove->fStepSize, 0);
	pMove->pWorld->AABBTrace(pMove->vDest, pMove->Shape.vExtents, vVelocity, 1.0f, &Trace, pMove->pModel);

	pMove->vDest = Trace.vDestPos;

	if (Trace.fFraction < 1.0f)
	{
		if (Trace.vNormal.y < 0.5f)		//	> 60 deg
		{
			//	Slide down
			A3DMOVE Move = *pMove;
			Move.vStart		= vTempPos;
			Move.GroundType	= TRACE_OBJECT_NULL;
			Move.vVelocity	= vVelocity;
			Move.fTime		= 1.0f;

			TRA_SlideMove(&Move, false);

			pMove->vDest	 = Move.vDest;
			pMove->vVelocity = vTempVel;
		}
		else
			TRA_ClipVelocity(pMove->vVelocity, Trace.vNormal, pMove->vVelocity, MOVE_OVERBOUNCE);
	}
}


