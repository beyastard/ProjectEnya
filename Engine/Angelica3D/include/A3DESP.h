/*
 * FILE: A3DESP.h
 *
 * DESCRIPTION: Routines for ESP algorithm and file
 *
 * CREATED BY: duyuxin, 2001/10/12
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DESP_H_
#define _A3DESP_H_

#include "A3DFuncs.h"
#include "AFile.h"
#include "ESPFile.h"
#include "A3DTrace.h"
#include "A3DVertex.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DESP
//
///////////////////////////////////////////////////////////////////////////

class A3DESP
{
public:		//	Types

	enum
	{
		MAXNUM_MARKVERT		= 256,	//	Maximum number of vertices of splited mark
		MAXNUM_MARKINDEX	= 512	//	Maximum number of indices of splited mark
	};

	typedef struct _ESPBRUSH
	{
		A3DPLANE**	aPlanes;		//	Plane pointer array
		int			iNumPlane;		//	Number of plane reference

	} ESPBRUSH, *PESPBRUSH;

	typedef struct _ESPSIDE
	{
		PA3DPLANE	pPlane;			//	Plane
		A3DVECTOR3*	aVerts;			//	Vertex array of mesh to which this side belong
		int			iNumVert;		//	Number of vertex in aVerts, this in fact is mesh's vertex number
		WORD*		aIndices;		//	Index array
		int			iNumIdx;		//	Number of index in aIndices, this in fact is side's vertex number
		DWORD		dwFlags;		//	dwFlags
		PESPBRUSH	pBrush;			//	Brush

		A3DVECTOR3	vCenter;		//	Side's center
		A3DVECTOR3	vExtents;		//	Bounding box's extents
		A3DVECTOR3	vMins;			//	Bounding box
		A3DVECTOR3	vMaxs;

		DWORD		dwTraceCnt;		//	Trace count
	
	} ESPSIDE, *PESPSIDE;
	
	typedef struct _ESPLEAF
	{
		int*		aSideRefs;		//	Side reference array
		int			iNumRefs;		//	Number of side reference
		
	} ESPLEAF, *PESPLEAF;

	typedef struct _ESPCLUSTER
	{
		A3DVECTOR3	vCenter;		//	Cluster's center
		A3DVECTOR3	vExtents;		//	AABB's extents
		A3DVECTOR3	vMins;			//	Cluster's accurate AABB
		A3DVECTOR3	vMaxs;
		A3DVECTOR3	vAlignMins;		//	Align bounding box
		A3DVECTOR3	vAlignMaxs;
		int			vLength[3];		//	Length of each border of leaf
		int			vSize[3];		//	Size of world in leaves
		A3DVECTOR3	vInvLength;		//	Reciprocal of lengths
		
		PESPSIDE	aSides;			//	Sides array
		int			iNumSides;		//	Number of sides
		PESPLEAF	aLeaves;		//	Leaf array
		int			iNumLeaves;		//	Number of leaves
		int			vSpans[3];		//	Spans when x, y, z increase

		DWORD		dwTraceCnt;		//	Trace count
		
	} ESPCLUSTER, *PESPCLUSTER;

	typedef struct _ESPBLOCK
	{
		int*		aClusterRefs;	//	Cluster reference array
		int			iNumRefs;		//	Number of cluster reference

	} ESPBLOCK, *PESPBLOCK;

	typedef struct _ESPAREA
	{
		float		vMins[2];		//	Area's bounding box
		float		vMaxs[2];
		float		vAlignMins[2];	//	Align bounding box
		float		vAlignMaxs[2];
		int			vLength[2];		//	Length of every block
		int			vSize[2];		//	Area's size in blocks
		float		vInvLength[2];	//	Reciprocal of lengths

	} ESPAREA, *PESPAREA;

	//	Structure for 3DDDA algorithm
	typedef struct _THREEDDDA
	{
		int			iMajor;			//	Major axis
		int			iNumSteps;		//	Steps need to go
		int			vStart[3];		//	Start voxel
		int			vEnd[3];		//	End voxel
		int			sx, sy, sz;		//	Sign delta
		float		ex, ey, ez;		//	Errors' initial value
		float		ix, iy, iz;		//	Increments
		float		dx, dy, dz;		//	Deltas to next voxel
		float		cx, cy, cz;		//	Compare value
	
	} THREEDDDA, *PTHREEDDDA;

	//	Structure for 2DDDA algorithm
	typedef struct _TWODDDA
	{
		int			iMajor;			//	Major axis
		int			iNumSteps;		//	Steps need to go
		int			vStart[2];		//	Start voxel
		int			vEnd[2];		//	End voxel
		int			su, sv;			//	Sign delta
		float		eu, ev;			//	Errors' initial value
		float		iu, iv;			//	Increments
		float		du, dv;			//	Deltas to next voxel
		float		cu, cv;			//	Compare value
		
	} TWODDDA, *PTWODDDA;

	//	Internal structure for AABB trace
	typedef struct _AABBINFO
	{
		PAABBTRACEINFO	pInfo;		//	AABB trace information
		float			fFraction;	//	Collision fraction
		PESPSIDE		pSide;		//	Nearest side
		A3DVECTOR3		vNormal;	//	Side's normal

	} AABBINFO, *PAABBINFO;

	//	Internal structure for ray trace
	typedef struct _RAYINFO
	{
		A3DVECTOR3	vStart;			//	Start point of ray segment
		A3DVECTOR3	vEnd;			//	end point of ray segment
		A3DVECTOR3	vDir;			//	Unnormalized direction (vEnd - vStart)
		int			iAxial;			//	-1: non-axial, 0, 1, 2: x, y, z

		A3DVECTOR3	vPoint;			//	Hit point
		FLOAT		fFraction;		//	Adjusted hit fraction (0 start point, 1 end point)
		PESPSIDE	pSide;			//	Nearest side
		A3DVECTOR3	vNormal;		//	Side's normal
		FLOAT		fHitFrac;		//	Real hit fraction

	} RAYINFO, *PRAYINFO;

	//	Internal structure for mark spliting
	typedef struct _MARKSPLIT
	{
		A3DAABB*	paabb;			//	Explosion box
		A3DVECTOR3*	pvNormal;		//	Normal of plane explosion is on
		A3DLVERTEX* aVerts;			//	Vertex buffer
		WORD*		aIndices;		//	Index buffer
		int			iNumVert;		//	Number of vertex in aVerts
		int			iNumIdx;		//	Number of index in aIndices
		float		fRadius;		//	Affect radius
		PESPSIDE	pSide;			//	Current side used to split mark
		bool		bJog;			//	true, push mark out a little
		A3DVECTOR3	vSumDir;		//	Sum direction of sides

	} MARKSPLIT, *PMARKSPLIT;

public:		//	Constructors and Destructors

	A3DESP();
	virtual ~A3DESP();

public:		//	Attributes

	int			m_iNumCheckedSide;		//	Number of checked side, this value can be used for debug

public:		//	Operations

	bool		Load(char* szFileName);	//	Load ESP data from file and initialize object
	void		Release();				//	Release all resources

	bool		RayTrace(PRAYTRACE pTrace, A3DVECTOR3& vStart, A3DVECTOR3& vVelocity, FLOAT fTime);	//	Do ray trace
	bool		AABBTrace(PAABBTRACE pTrace, PAABBTRACEINFO pInfo);			//	Do AABB trace
	bool		SplitMark(A3DAABB& aabb, A3DVECTOR3 vNormal, A3DLVERTEX* aVerts, 
						  WORD* aIndices, bool bJog, int* piNumVert, int* piNumIdx,
						  float fRadiusScale=0.2f);		//	Split explosion mark

	void		EnumAllClusters(bool bEnum)		{	m_bAreaTrace = !bEnum;	}

protected:	//	Attributes

	PA3DPLANE	m_aPlanes;			//	Plane array
	PESPSIDE	m_aSides;			//	Side array
	PESPLEAF	m_aLeaves;			//	Leaf array
	PESPCLUSTER	m_aClusters;		//	Cluster array
	PESPBLOCK	m_aBlocks;			//	Blocks array
	A3DVECTOR3*	m_aVerts;			//	Vertex array
	ESPAREA		m_Area;				//	World area
	WORD*		m_aIndices;			//	Index array
	int*		m_aSideRefs;		//	Side reference array
	int*		m_aClusterRefs;		//	Cluster reference array
	PESPBRUSH	m_aBrushes;			//	Brush array
	A3DPLANE**	m_aPlaneRefs;		//	Plane reference array

	int			m_iNumPlane;		//	Number of plane
	int			m_iNumSide;			//	Number of side
	int			m_iNumLeaf;			//	Number of leaf
	int			m_iNumCluster;		//	Number of cluster
	int			m_iNumVert;			//	Number of vertex
	int			m_iNumIndex;		//	Number of index
	int			m_iNumSideRef;		//	Number of side reference
	int			m_iNumBlock;		//	Number of block
	int			m_iNumClusterRef;	//	Number of cluster reference
	int			m_iNumBrush;		//	Number of brush
	int			m_iNumPlaneRef;		//	Number of plane reference of brushes

	bool		m_bAreaTrace;		//	true, use area information
	DWORD		m_dwTraceCnt;		//	Trace count
	RAYINFO		m_Ray;				//	Ray trace information
	AABBINFO	m_AABB;				//	AABB trace information

	TWODDDA		m_2DDDA;			//	2DDDA algorithm variables
	PESPBLOCK	m_pEverHitBlock;	//	Hit ever occures in this block
	THREEDDDA	m_3DDDA;			//	3DDDA algorithm variables
	PESPLEAF	m_pEverHitLeaf;		//	Hit ever occures in this leaf
	PESPCLUSTER	m_pCurCluster;		//	Current cluster

	bool		m_bRayTraceEnable;	//	Flag to see if ESP will be used for ray trace;
	bool		m_bAABBTraceEnable;	//	Flag to see if ESP will be used for AABB trace;

protected:	//	Operations

	//	Lump read functions
	int			ReadLump(AFile* pFile, PESPFILEHEADER pHeader, int iLump, void** ppBuf, int iSize);	//	read lump
	bool		ReadPlaneLump(AFile* pFile, PESPFILEHEADER pHeader);		//	Read plane lump
	bool		ReadSideLump(AFile* pFile, PESPFILEHEADER pHeader);			//	Read side lump
	bool		ReadLeafLump(AFile* pFile, PESPFILEHEADER pHeader);			//	Read leaf lump
	bool		ReadClusterLump(AFile* pFile, PESPFILEHEADER pHeader);		//	Read cluster lump
	bool		ReadVertexLump(AFile* pFile, PESPFILEHEADER pHeader);		//	Read vertex lump
	bool		ReadIndexLump(AFile* pFile, PESPFILEHEADER pHeader);		//	Read index lump
	bool		ReadSideRefLump(AFile* pFile, PESPFILEHEADER pHeader);		//	Read side reference lump
	bool		ReadBlockLump(AFile* pFile, PESPFILEHEADER pHeader);		//	Read block lump
	bool		ReadAreaLump(AFile* pFile, PESPFILEHEADER pHeader);			//	Read area lump
	bool		ReadClusterRefLump(AFile* pFile, PESPFILEHEADER pHeader);	//	Read cluster reference lump
	bool		ReadBrushLump(AFile* pFile, PESPFILEHEADER pHeader);		//	Read brush lump
	bool		ReadBrushPlaneLump(AFile* pFile, PESPFILEHEADER pHeader);	//	Read brush plane reference lump

	A3DMESH_PROP	ConvertSideFlags(DWORD dwFlags);		//	Convert side flags

	//	Ray tracing
	bool		RayToCluster(int iCluster);					//	Trace a ray to cluster
	void		Init3DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1);	//	Initialize variables for tracing ray in cluster
	bool		TraceRayInCluster();						//	Trace ray in current cluster
	bool		TraceRayInLeaf(PESPLEAF pLeaf);				//	Trace ray in leaf
	bool		RayToSide(PESPSIDE pSide, PESPLEAF pLeaf);	//	Ray-side intersection routine
	void		AxialInit3DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1);		//	Initialize variable for axial ray tracing in cluster
	bool		AxialTraceRayInCluster();					//	Axial trace ray in current cluster

	bool		RayToArea();								//	Trace ray to area
	void		Init2DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1);	//	Initialize variables for tracing ray in area
	bool		TraceRayInArea();							//	Trace ray in area
	bool		TraceRayInBlock(PESPBLOCK pBlock);			//	Trace ray in block

	bool		TraceAABBInCluster();				//	Trace AABB in current cluster
	bool		AABBMoveToSide(PESPSIDE pSide);		//	Collision routine for moving AABB and side

	void		SplitMarkInCluster(MARKSPLIT* pMarkSplit);	//	Split mark in a cluster
	bool		SplitMarkBySide(MARKSPLIT* pMarkSplit);		//	Split mark using a side

public:

	inline void SetRayTraceEnable(bool bEnable) { m_bRayTraceEnable = bEnable; }
	inline void SetAABBTraceEnable(bool bEnable) { m_bAABBTraceEnable = bEnable; }
	inline bool GetRayTraceEnable() { return m_bRayTraceEnable; }
	inline bool GetAABBTraceEnable() { return m_bAABBTraceEnable; }
};



#endif	//	_A3DESP_H_

