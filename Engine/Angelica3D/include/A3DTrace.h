/*
 * FILE: A3DTrace.h
 *
 * DESCRIPTION: Routines for trace algorithm and file
 *
 * CREATED BY: duyuxin, 2001/11/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DTRACE_H_
#define _A3DTRACE_H_

#include "A3DFuncs.h"
#include "A3DTypes.h"

typedef class A3DModel * PA3DModel;
typedef class A3DWorld * PA3DWorld;
typedef struct _A3DMODELOBB A3DMODELOBB;

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define MOVE_OVERBOUNCE		1.001f

//	An obb may have 26 bevels, 6 side + 8 vertices + 12 borders
#define MAXNUM_OBBBEVEL		26

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

//  object's type that collided with the ray or the obb
enum TRACE_OBJECT
{
	TRACE_OBJECT_TERRAIN = 0,
	TRACE_OBJECT_ESPMODEL,
	TRACE_OBJECT_MODEL,
	TRACE_OBJECT_PLANTS,
	TRACE_OBJECT_NULL = -1
};

//	Ray trace structure
typedef struct _RAYTRACE
{
	A3DVECTOR3		vPoint;			//	Adjusted collision point
	A3DVECTOR3		vHitPos;		//	Real collision point, this is only valid when hit object is a part of ESP
	A3DVECTOR3		vNormal;		//	Normal of plane on which side lies
	FLOAT			fFraction;		//	Hit fraction (0 start point, 1 end point)

	TRACE_OBJECT	objectType;		//	The type of the object collided with this ray;
	A3DMESH_PROP	meshProperty;	//	The mesh's property in the esp world;
	A3DModel *		pModel;			//	If the object is a model, this is the reference pointer;
	A3DModel *		pRootModel;		//	If the object is a model, this is the root model pointer;
	A3DMODELOBB *	pModelOBB;		//	If the object is a model, this is the frameObb data;

	DWORD			dwExtraData;		//	Used when ray collided with trees, this is the tree index;

} RAYTRACE, *PRAYTRACE;

//	OBB trace structure
typedef struct _OBBTRACE
{
	A3DVECTOR3		vPoint;			//	Collision point
	A3DVECTOR3		vNormal;		//	Normal of plane on which side lies
	FLOAT			fFraction;		//	move fraction (0 start point, 1 end point)
	FLOAT			fOBBFraction;	//	OBB fraction

	TRACE_OBJECT	objectType;		//	The type of the object collided with this ray;
	A3DMESH_PROP	meshProperty;	//	The mesh's property in the esp world;
	A3DModel *		pModel;			//	If the object is a model, this is the reference pointer;
	A3DMODELOBB *	pModelOBB;		//	If the object is a model, this is the frameObb data;

} OBBTRACE, *POBBTRACE;

//	OBB shape size;
typedef struct _OBBSHAPE
{
	FLOAT			fRadius;		//	Current we use a sphere so radius is enough;
	A3DVECTOR3		vExtents;		//	Half extents

} OBBSHAPE, *POBBSHAPE;

//	AABB trace structure
typedef struct _AABBTRACE
{
	A3DVECTOR3		vDestPos;		//	Position AABB can move to
	A3DVECTOR3		vNormal;		//	Normal of plane on which side lies
	FLOAT			fFraction;		//	move fraction (0 start point, 1 end point)

	TRACE_OBJECT	objectType;		//	The type of the object collided with this ray;
	A3DMESH_PROP	meshProperty;	//	The mesh's property in the esp world;
	A3DModel *		pModel;			//	If the object is a model, this is the reference pointer;
	A3DMODELOBB *	pModelOBB;		//	If the object is a model, this is the frameObb data;

} AABBTRACE, *PAABBTRACE;

//	Move information
typedef struct _A3DMOVE
{
	A3DVECTOR3		vStart;			//	(in) Start point
	A3DVECTOR3		vVelocity;		//	(in) Velocity
	FLOAT			fTime;			//	(in) Time
	OBBSHAPE		Shape;			//	(in) Shape used to build OBB
	FLOAT			fStepSize;		//	(in) Step size
	FLOAT			fGravity;		//	(in) Value of gravity

	AABBTRACE		GroundTrace;	//	(in) Ground information
	TRACE_OBJECT	GroundType;		//	(in) Ground Type of current position
	PA3DWorld		pWorld;			//	(in) A3DWorld object
	PA3DModel		pModel;			//	(in) ModelMe, which will collided with;

	A3DVECTOR3		vDest;			//	(out) Destination point

} A3DMOVE, *PA3DMOVE;

//	Separate plane used in AABB trace routines
typedef struct _AABBSPLANE
{
	A3DVECTOR3	vNormal;		//	Normal
	FLOAT		fDist;			//	d parameter of plane
	int			iFront;
	int			iBack;

} AABBSPLANE, *PAABBSPLANE;

//	AABB trace information used in AABB trace routines
typedef struct _AABBTRACEINFO
{
	//	In
	A3DVECTOR3	vStart;			//	Start point
	A3DVECTOR3	vExtents;		//	AABB's extents
	FLOAT		fTime;			//	Time
	A3DVECTOR3	vDelta;			//	Move delta = velocity * time
	A3DVECTOR3	vEnd;			//	End point = vStart + vDelta
	A3DAABB		BoundAABB;		//	AABB which contain whole sweep area
	int			iType;			//	Trace type

	//	Out
	bool		bStartSolid;	//	Collide something at start point
	bool		bAllSolid;		//	All in something
	A3DPLANE	ClipPlane;		//	Clip plane
	float		fFraction;		//	Fraction

} AABBTRACEINFO, *PAABBTRACEINFO;

//	OBB bevels
typedef struct _OBBBEVELS
{
	A3DPLANE	aPlanes[MAXNUM_OBBBEVEL];		//	Bevel planes
	A3DPLANE*	aPlaneAddrs[MAXNUM_OBBBEVEL];	//	Plane addresses
	int			iNumPlane;						//	Number of planes in aPlanes

} OBBBEVELS, *POBBBEVELS;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

void TRA_OBBTraceInit(PA3DOBB pOBB, A3DVECTOR3& vStart, A3DVECTOR3& vVelocity, FLOAT fTime, OBBSHAPE& Shape);
void TRA_ClipVelocity(A3DVECTOR3& vInVel, A3DVECTOR3& vNormal, A3DVECTOR3& vOutVel, float fOverBounce);
void TRA_StepSlideMove(PA3DMOVE pMove, bool bGravity);
bool TRA_SlideMove(PA3DMOVE pMove, bool bStepTry);
void TRA_AABBTraceInit(PAABBTRACEINFO pInfo, A3DVECTOR3& vStart, A3DVECTOR3& vExts, A3DVECTOR3& vVelocity, FLOAT fTime);
bool TRA_AABBMoveToSide(PAABBTRACEINFO pInfo, A3DVECTOR3* aVerts, int iNumVert, A3DPLANE& SidePlane, FLOAT* pfFraction);
void TRA_BuildOBBBevels(A3DOBB& OBB, OBBBEVELS* pBevels);
bool TRA_AABBMoveToBrush(PAABBTRACEINFO pInfo, A3DPLANE** aPlanes, int iNumPlane);

#endif	//	_A3DTRACE_H_
