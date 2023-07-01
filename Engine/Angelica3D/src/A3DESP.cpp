/*
 * FILE: A3DESP.cpp
 *
 * DESCRIPTION: Routines for ESP algorithm and file
 *
 * CREATED BY: duyuxin, 2001/10/12
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DErrLog.h"
#include "A3DESP.h"
#include "A3DCollision.h"
#include "AFileImage.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define ESPBRUSH_THICK		0.5f

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

A3DESP::A3DESP()
{
	m_aPlanes		= NULL;
	m_aSides		= NULL;
	m_aLeaves		= NULL;
	m_aClusters		= NULL;
	m_aVerts		= NULL;
	m_aIndices		= NULL;
	m_aBlocks		= NULL;
	m_aClusterRefs	= NULL;
	m_aSideRefs		= NULL;
	m_aBrushes		= NULL;
	m_aPlaneRefs	= NULL;
	m_iNumPlane		= 0;
	m_iNumSide		= 0;
	m_iNumLeaf		= 0;
	m_iNumCluster	= 0;
	m_iNumVert		= 0;
	m_iNumIndex		= 0;
	m_iNumSideRef	= 0;
	m_iNumBlock		= 0;
	m_iNumClusterRef = 0;
	m_iNumBrush		= 0;
	m_iNumPlaneRef	= 0;

	m_bAreaTrace	= false;
	m_dwTraceCnt	= 0;
	m_pCurCluster	= NULL;

	m_bRayTraceEnable	= true;
	m_bAABBTraceEnable  = true;
}

A3DESP::~A3DESP()
{
}

/*	Load ESP data from file and initialize object.

	Return true for success, otherwise return false.

	szFileName: .esp file's name.
*/
bool A3DESP::Load(char* szFileName)
{
	AFileImage File;

	if (!File.Open(szFileName, AFILE_OPENEXIST))
	{
		File.Close();
		g_pA3DErrLog->ErrLog("A3DESP::Load(), Cannot open file %s!", szFileName);
		return false;
	}

	//	Release old resources
	Release();

	ESPFILEHEADER Header;
	DWORD dwRead;

	//	Read ESP file header
	if (!File.Read(&Header, sizeof (Header), &dwRead) || dwRead != sizeof (Header))
	{
		g_pA3DErrLog->ErrLog("A3DESP::Load(), Failed to read file header");
		File.Close();
		return false;
	}

	//	Check format and version
	if (Header.dwIdentify != ESP_IDENTIFY || Header.dwVersion != ESP_VERSION)
	{
		g_pA3DErrLog->ErrLog("A3DESP::Load(), Wrong file format or version");
		File.Close();
		return false;
	}

	//	Read lumps
	ReadVertexLump(&File, &Header);		//	Vertex lump before side lump
	ReadIndexLump(&File, &Header);		//	Index lump before side lump
	ReadPlaneLump(&File, &Header);		//	Plane lump before side and brush lump
	ReadBrushPlaneLump(&File, &Header);	//	Plane reference lump before brush lump
	ReadBrushLump(&File, &Header);		//	Brush lump before side lump
	ReadSideRefLump(&File, &Header);	//	Side reference lump before leaf lump
	ReadSideLump(&File, &Header);		//	Side lump before cluster lump
	ReadLeafLump(&File, &Header);		//	Leaf lump before cluster lump
	ReadClusterLump(&File, &Header);
	ReadAreaLump(&File, &Header);
	ReadClusterRefLump(&File, &Header);	//	Cluster reference lump before block lump
	ReadBlockLump(&File, &Header);

	File.Close();

	return true;
}

//	Release all resources
void A3DESP::Release()
{
	if (m_aPlanes)
	{
		free(m_aPlanes);
		m_aPlanes = NULL;
		m_iNumPlane = 0;
	}

	if (m_aSides)
	{
		free(m_aSides);
		m_aSides = NULL;
		m_iNumSide = 0;
	}

	if (m_aLeaves)
	{
		free(m_aLeaves);
		m_aLeaves = NULL;
		m_iNumLeaf = 0;
	}

	if (m_aClusters)
	{
		free(m_aClusters);
		m_aClusters = NULL;
		m_iNumCluster = 0;
	}

	if (m_aVerts)
	{
		free(m_aVerts);
		m_aVerts = NULL;
		m_iNumVert = 0;
	}

	if (m_aIndices)
	{
		free(m_aIndices);
		m_aIndices = NULL;
		m_iNumIndex = 0;
	}

	if (m_aSideRefs)
	{
		free(m_aSideRefs);
		m_aSideRefs = NULL;
		m_iNumSideRef = 0;
	}

	if (m_aBlocks)
	{
		free(m_aBlocks);
		m_aBlocks = NULL;
		m_iNumBlock = 0;
	}

	if (m_aClusterRefs)
	{
		free(m_aClusterRefs);
		m_aClusterRefs = NULL;
		m_iNumClusterRef = 0;
	}

	if (m_aBrushes)
	{
		free(m_aBrushes);
		m_aBrushes	= NULL;
		m_iNumBrush = 0;
	}

	if (m_aPlaneRefs)
	{
		free(m_aPlaneRefs);
		m_aPlaneRefs	= NULL;
		m_iNumPlaneRef	= 0;
	}
}

/*	Read a lump in ESP file.

	Return number of item in this lump

	pFile: file's pointer
	pHeader: ESP file header
	iLump: lump will be read.
	ppBuf (out): address of a variable that will be set to lump data buffer's pointer.
	iSize: item size in lump
*/	
int	A3DESP::ReadLump(AFile* pFile, PESPFILEHEADER pHeader, int iLump, void** ppBuf, int iSize)
{
	PEFLUMP pLump = &pHeader->aLumps[iLump];
	DWORD dwRead;

	if (pLump->lSize % iSize)
		return -1;
	
	void *pBuf;
	if (!(pBuf = malloc(pLump->lSize)))
		return -1;

	pFile->Seek(pLump->lOff, AFILE_SEEK_SET);
	pFile->Read(pBuf, pLump->lSize, &dwRead);

	*ppBuf = pBuf;

	return pLump->lSize / iSize;
}

//	Read plane lump
bool A3DESP::ReadPlaneLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	PEFPLANE aEFPlanes;
	int iNumPlane = ReadLump(pFile, pHeader, EFLUMP_PLANES, (void**)&aEFPlanes, sizeof (EFPLANE));

	if (iNumPlane == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read plane lump A3DESP::ReadPlaneLump");
		return false;
	}
	
	PA3DPLANE aPlanes = (PA3DPLANE)malloc(iNumPlane * sizeof (A3DPLANE));
	if (!aPlanes)
	{
		g_pA3DErrLog->ErrLog("Not enough memory in A3DESP::ReadPlaneLump");
		return false;
	}

	for (int i=0; i < iNumPlane; i++)
	{
		aPlanes[i].vNormal.x	= aEFPlanes[i].vNormal[0];
		aPlanes[i].vNormal.y	= aEFPlanes[i].vNormal[1];
		aPlanes[i].vNormal.z	= aEFPlanes[i].vNormal[2];
		aPlanes[i].fDist		= aEFPlanes[i].fDist;
		aPlanes[i].byType		= GetA3DPlaneType(aPlanes[i].vNormal);
		aPlanes[i].bySignBits	= GetA3DPlaneSignBits(aPlanes[i].vNormal);
	}

	m_aPlanes	= aPlanes;
	m_iNumPlane	= iNumPlane;

	free(aEFPlanes);

	return true;
}

//	Read side lump
bool A3DESP::ReadSideLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	PEFSIDE aEFSides;
	int iNumSide = ReadLump(pFile, pHeader, EFLUMP_SIDES, (void**)&aEFSides, sizeof (EFSIDE));

	if (iNumSide == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read side lump A3DESP::ReadSideLump");
		return false;
	}
	
	PESPSIDE aSides = (PESPSIDE)malloc(iNumSide * sizeof (ESPSIDE));
	if (!aSides)
	{
		g_pA3DErrLog->ErrLog("Not enough memory in A3DESP::ReadSideLump");
		return false;
	}

	int i, j;
	A3DVECTOR3 vVert, vNormal;

	for (i=0; i < iNumSide; i++)
	{
		if (aEFSides[i].lPlane >= 0)
			aSides[i].pPlane = &m_aPlanes[aEFSides[i].lPlane];
		else
			aSides[i].pPlane = NULL;

		aSides[i].aVerts	 = &m_aVerts[aEFSides[i].lFirstVert];
		aSides[i].iNumVert	 = aEFSides[i].lNumVert;
		aSides[i].aIndices	 = &m_aIndices[aEFSides[i].lFirstIdx];
		aSides[i].iNumIdx	 = aEFSides[i].lNumIdx;
		aSides[i].dwFlags	 = aEFSides[i].dwFlags;
		aSides[i].dwTraceCnt = 0;
		aSides[i].pBrush	 = &m_aBrushes[aEFSides[i].lBrush];

		//	Calculate side's bounding box
		ClearAABB(aSides[i].vMins, aSides[i].vMaxs);

		//	Nudge side out along it's normal
		if (aSides[i].pPlane)
			vNormal = aSides[i].pPlane->vNormal * (-ESPBRUSH_THICK);	//	Brush thick
		else
			vNormal = A3DVECTOR3(0.0f);

		for (j=0; j < aSides[i].iNumIdx; j++)
		{
			vVert = aSides[i].aVerts[aSides[i].aIndices[j]];
			AddVertexToAABB(aSides[i].vMins, aSides[i].vMaxs, vVert);

			AddVertexToAABB(aSides[i].vMins, aSides[i].vMaxs, vVert + vNormal);

			//	Conside two sides
			if (aSides[i].dwFlags & SIDEFLAG_TWOSIDES)
				AddVertexToAABB(aSides[i].vMins, aSides[i].vMaxs, vVert - vNormal);
		}

		//	Side's center and extents
		aSides[i].vCenter  = (aSides[i].vMins + aSides[i].vMaxs) * 0.5f;
		aSides[i].vExtents = (aSides[i].vMaxs - aSides[i].vMins) * 0.5f;
	}

	m_aSides	= aSides;
	m_iNumSide	= iNumSide;

	free(aEFSides);

	return true;
}

//	Read leaf lump
bool A3DESP::ReadLeafLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	PEFLEAF aEFLeaves;
	int iNumLeaf = ReadLump(pFile, pHeader, EFLUMP_LEAVES, (void**)&aEFLeaves, sizeof (EFLEAF));

	if (iNumLeaf == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read leaf lump A3DESP::ReadLeafLump");
		return false;
	}
	
	PESPLEAF aLeaves = (PESPLEAF)malloc(iNumLeaf * sizeof (ESPLEAF));
	if (!aLeaves)
	{
		g_pA3DErrLog->ErrLog("Not enough memory in A3DESP::ReadLeafLump");
		return false;
	}

	for (int i=0; i < iNumLeaf; i++)
	{
		aLeaves[i].aSideRefs	= &m_aSideRefs[aEFLeaves[i].lFirstLeafSide];
		aLeaves[i].iNumRefs		= aEFLeaves[i].lNumLeafSides;
	}

	m_aLeaves	= aLeaves;
	m_iNumLeaf	= iNumLeaf;

	free(aEFLeaves);

	return true;
}

//	Read cluster lump
bool A3DESP::ReadClusterLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	PEFCLUSTER aEFClusters;
	int iNumCluster = ReadLump(pFile, pHeader, EFLUMP_CLUSTERS, (void**)&aEFClusters, sizeof (EFCLUSTER));

	if (iNumCluster == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read cluster lump A3DESP::ReadClusterLump");
		return false;
	}
	
	PESPCLUSTER aClusters = (PESPCLUSTER)malloc(iNumCluster * sizeof (ESPCLUSTER));
	if (!aClusters)
	{
		g_pA3DErrLog->ErrLog("Not enough memory in A3DESP::ReadClusterLump");
		return false;
	}

	int i, j;
	for (i=0; i < iNumCluster; i++)
	{
		for (j=0; j < 3; j++)
		{
			aClusters[i].vLength[j]			= aEFClusters[i].vLength[j];
			aClusters[i].vSize[j]			= aEFClusters[i].vSize[j];
			aClusters[i].vAlignMaxs.m[j]	= aEFClusters[i].vAlignMaxs[j];
			aClusters[i].vAlignMins.m[j]	= aEFClusters[i].vAlignMins[j];
			aClusters[i].vMaxs.m[j]			= aEFClusters[i].vMaxs[j];
			aClusters[i].vMins.m[j]			= aEFClusters[i].vMins[j];
			aClusters[i].vInvLength.m[j]	= 1.0f / aClusters[i].vLength[j];
		}

		aClusters[i].vCenter	= (aClusters[i].vMins + aClusters[i].vMaxs) * 0.5f;
		aClusters[i].vExtents	= (aClusters[i].vMaxs - aClusters[i].vMins) * 0.5f;
		aClusters[i].aLeaves	= &m_aLeaves[aEFClusters[i].lFirstLeaf];
		aClusters[i].iNumLeaves	= aEFClusters[i].lNumLeaves;
		aClusters[i].aSides		= &m_aSides[aEFClusters[i].lFirstSide];
		aClusters[i].iNumSides	= aEFClusters[i].lNumSides;

		aClusters[i].vSpans[0]	= 1;
		aClusters[i].vSpans[1]	= aClusters[i].vSize[0] * aClusters[i].vSize[2];
		aClusters[i].vSpans[2]	= aClusters[i].vSize[0];

		aClusters[i].dwTraceCnt	= 0;
	}

	m_aClusters		= aClusters;
	m_iNumCluster	= iNumCluster;

	free(aEFClusters);

	return true;
}

//	Read vertex lump
bool A3DESP::ReadVertexLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	PEFVERTEX aEFVerts;
	int iNumVert = ReadLump(pFile, pHeader, EFLUMP_VERTICES, (void**)&aEFVerts, sizeof (EFVERTEX));

	if (iNumVert == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read vertex lump A3DESP::ReadVertexLump");
		return false;
	}
	
	A3DVECTOR3* aVerts = (A3DVECTOR3*)malloc(iNumVert * sizeof (A3DVECTOR3));
	if (!aVerts)
	{
		g_pA3DErrLog->ErrLog("Not enough memory in A3DESP::ReadVertexLump");
		return false;
	}

	for (int i=0; i < iNumVert; i++)
	{
		aVerts[i].x = aEFVerts[i].vPos[0];
		aVerts[i].y = aEFVerts[i].vPos[1];
		aVerts[i].z = aEFVerts[i].vPos[2];
	}

	m_aVerts	= aVerts;
	m_iNumVert	= iNumVert;

	free(aEFVerts);

	return true;
}

//	Read index lump
bool A3DESP::ReadIndexLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	WORD* aIndices;
	int iNumIndex = ReadLump(pFile, pHeader, EFLUMP_INDICES, (void**)&aIndices, sizeof (WORD));

	if (iNumIndex == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read index lump A3DESP::ReadIndexLump");
		return false;
	}
	
	m_aIndices	= aIndices;
	m_iNumIndex	= iNumIndex;

	return true;
}

//	Read side reference lump
bool A3DESP::ReadSideRefLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	int* aRefs;
	int iNumRef = ReadLump(pFile, pHeader, EFLUMP_LEAFSIDES, (void**)&aRefs, sizeof (int));

	if (iNumRef == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read leaf side lump A3DESP::ReadSideRefLump");
		return false;
	}
	
	m_aSideRefs		= aRefs;
	m_iNumSideRef	= iNumRef;

	return true;
}

//	Read block lump
bool A3DESP::ReadBlockLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	PEFBLOCK aEFBlocks;
	int iNumBlock = ReadLump(pFile, pHeader, EFLUMP_BLOCKS, (void**)&aEFBlocks, sizeof (EFBLOCK));

	if (iNumBlock == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read block lump A3DESP::ReadBlockLump");
		return false;
	}
	
	PESPBLOCK aBlocks = (PESPBLOCK)malloc(iNumBlock * sizeof (ESPBLOCK));
	if (!aBlocks)
	{
		g_pA3DErrLog->ErrLog("Not enough memory in A3DESP::ReadBlockLump");
		return false;
	}

	for (int i=0; i < iNumBlock; i++)
	{
		aBlocks[i].aClusterRefs	= &m_aClusterRefs[aEFBlocks[i].lFirstClusterRef];
		aBlocks[i].iNumRefs		= aEFBlocks[i].lNumClusterRef;
	}

	m_aBlocks	= aBlocks;
	m_iNumBlock	= iNumBlock;

	free(aEFBlocks);

	return true;
}

//	Read area lump
bool A3DESP::ReadAreaLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	PEFAREA pEFArea;
	ReadLump(pFile, pHeader, EFLUMP_AREAS, (void**)&pEFArea, sizeof (EFAREA));

	for (int i=0; i < 2; i++)
	{
		m_Area.vMins[i]			= pEFArea->vMins[i];
		m_Area.vMaxs[i]			= pEFArea->vMaxs[i];
		m_Area.vAlignMins[i]	= pEFArea->vAlignMins[i];
		m_Area.vAlignMaxs[i]	= pEFArea->vAlignMaxs[i];
		m_Area.vLength[i]		= pEFArea->vLength[i];
		m_Area.vSize[i]			= pEFArea->vSize[i];
		m_Area.vInvLength[i]	= 1.0f / m_Area.vLength[i];
	}

	free(pEFArea);

	return true;
}

//	Read cluster reference lump
bool A3DESP::ReadClusterRefLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	int* aRefs;
	int iNumRef = ReadLump(pFile, pHeader, EFLUMP_CLUSTERREFS, (void**)&aRefs, sizeof (int));

	if (iNumRef == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read cluster reference lump A3DESP::ReadClusterRefLump");
		return false;
	}
	
	m_aClusterRefs	 = aRefs;
	m_iNumClusterRef = iNumRef;

	return true;
}

//	Read brush lump
bool A3DESP::ReadBrushLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	PEFBRUSH aEFBrushes;
	int iNumBrush = ReadLump(pFile, pHeader, EFLUMP_BRUSHES, (void**)&aEFBrushes, sizeof (EFBRUSH));

	if (iNumBrush == -1)
	{
		g_pA3DErrLog->ErrLog("A3DESP::ReadBrushLump, Failed to read brush lump");
		return false;
	}

	if (!iNumBrush)		//	ESP file may not contain brushes
	{
		m_aBrushes  = NULL;
		m_iNumBrush = 0;
		free(aEFBrushes);
		return true;
	}

	PESPBRUSH aBrushes = (PESPBRUSH)malloc(iNumBrush * sizeof (ESPBRUSH));
	if (!aBrushes)
	{
		g_pA3DErrLog->ErrLog("A3DESP::ReadBrushLump, Not enough memory");
		return false;
	}

	int i;
	for (i=0; i < iNumBrush; i++)
	{
		aBrushes[i].aPlanes		= &m_aPlaneRefs[aEFBrushes[i].lFirstPlane];
		aBrushes[i].iNumPlane	= aEFBrushes[i].lNumPlane;
	}

	m_aBrushes  = aBrushes;
	m_iNumBrush = iNumBrush;

	free(aEFBrushes);

	return true;
}

//	Read brush plane reference lump
bool A3DESP::ReadBrushPlaneLump(AFile* pFile, PESPFILEHEADER pHeader)
{
	int* aRefs;
	int iNumRef = ReadLump(pFile, pHeader, EFLUMP_BRUSHPLANES, (void**)&aRefs, sizeof (int));

	if (iNumRef == -1)
	{
		g_pA3DErrLog->ErrLog("Failed to read plane reference lump A3DESP::ReadBrushPlaneLump");
		return false;
	}

	if (!iNumRef)		//	ESP file may not contain brushes
	{
		m_aPlaneRefs	= NULL;
		m_iNumPlaneRef	= 0;
		free(aRefs);
		return true;
	}

	A3DPLANE** aPlanes = (A3DPLANE**)malloc(iNumRef * sizeof (A3DPLANE*));
	if (!aPlanes)
	{
		g_pA3DErrLog->ErrLog("Not enough memory, A3DESP::ReadBrushPlaneLump");
		return false;
	}
	
	int i;
	for (i=0; i < iNumRef; i++)
		aPlanes[i] = &m_aPlanes[aRefs[i]];

	m_aPlaneRefs	= aPlanes;
	m_iNumPlaneRef	= iNumRef;

	free(aRefs);

	return true;
}

/*	Convert side flags to mesh properties.

	Return converted mesh properties.

	dwFlags: side flags.
*/
A3DMESH_PROP A3DESP::ConvertSideFlags(DWORD dwFlags)
{
	A3DMESH_PROP MeshProp;

	memset(&MeshProp, 0, sizeof (MeshProp));

	if (dwFlags & SIDEFLAG_TWOSIDES)
		MeshProp.b2Sided = true;

	if (dwFlags & SIDEFLAG_SHOOTTHROUGH)
		MeshProp.bCanShootThrough = true;

	if (dwFlags & SIDEFLAG_NOMARK)
		MeshProp.bNoMark = true;

	if (dwFlags & SIDEFLAG_PICKTHROUGH)
		MeshProp.bCanPickThrough = true;

	switch (dwFlags & SIDEFLAG_METERIALMASK)
	{
	case SIDEFLAG_CONCRETE:		MeshProp.nMaterialType = MATERIAL_CONCRETE;		break;
	case SIDEFLAG_WOOD:			MeshProp.nMaterialType = MATERIAL_WOOD;			break;
	case SIDEFLAG_METALSOLID:	MeshProp.nMaterialType = MATERIAL_METALSOLID;	break;
	case SIDEFLAG_METALBARREL:	MeshProp.nMaterialType = MATERIAL_METALBARREL;	break;
	case SIDEFLAG_GLASS:		MeshProp.nMaterialType = MATERIAL_GLASS;		break;
	case SIDEFLAG_RUBBER:		MeshProp.nMaterialType = MATERIAL_RUBBER;		break;
	case SIDEFLAG_FLESH:		MeshProp.nMaterialType = MATERIAL_FLESH;		break;
	}

	return MeshProp;
}

/*	Do ray trace. This function search the side hit by ray and calculate the collision
	point.

	Return true ray hit a side, otherwise return false.

	pTrace (out): trace information.
	vStart: ray's start point of segment.
	vVelocity: velocity.
	fTime: time
*/
bool A3DESP::RayTrace(PRAYTRACE pTrace, A3DVECTOR3& vStart, A3DVECTOR3& vVelocity, FLOAT fTime)
{
	if( !GetRayTraceEnable() )
		return false;

	m_dwTraceCnt++;

	bool bRet = false;
	int i;
	A3DVECTOR3 vDelta = vVelocity * fTime;

	//	Initialize data for ray trace
	m_Ray.vStart	= vStart;
	m_Ray.vEnd		= vStart + vDelta;
	m_Ray.vDir		= m_Ray.vEnd - m_Ray.vStart;
	m_Ray.pSide		= NULL;
	m_Ray.fFraction	= 1.0f;

	//	Is axial ray ?
	if (m_Ray.vDir.x == 0.0f)
	{
		if (m_Ray.vDir.z == 0.0f)
			m_Ray.iAxial = 1;
		else if (m_Ray.vDir.y == 0.0f)
			m_Ray.iAxial = 2;
	}
	else if (m_Ray.vDir.y == 0.0f && m_Ray.vDir.z == 0.0f)
		m_Ray.iAxial = 0;
	else
		m_Ray.iAxial = -1;

	pTrace->fFraction = 1.0f;
	pTrace->vPoint	  = m_Ray.vEnd;
	
	m_iNumCheckedSide = 0;

	if (!m_bAreaTrace)	//	Enumerate all clusters
	{
		for (i=0; i < m_iNumCluster; i++)
		{
			if (RayToCluster(i))
			{
				bRet = true;

				pTrace->fFraction	 = m_Ray.fFraction;
				pTrace->vPoint		 = m_Ray.vPoint;
				pTrace->vNormal		 = m_Ray.vNormal;
				pTrace->objectType	 = TRACE_OBJECT_ESPMODEL;
				pTrace->meshProperty = ConvertSideFlags(m_Ray.pSide->dwFlags);
				pTrace->vHitPos		 = m_Ray.vStart + m_Ray.vDir * m_Ray.fHitFrac;
			}
		}
	}
	else
	{
		if ((bRet = RayToArea()))
		{
			pTrace->fFraction	 = m_Ray.fFraction;
			pTrace->vPoint		 = m_Ray.vPoint;
			pTrace->vNormal		 = m_Ray.vNormal;
			pTrace->objectType	 = TRACE_OBJECT_ESPMODEL;
			pTrace->meshProperty = ConvertSideFlags(m_Ray.pSide->dwFlags);
			pTrace->vHitPos		 = m_Ray.vStart + m_Ray.vDir * m_Ray.fHitFrac;
		}
	}

	return bRet;
}

/*	Trace ray to area.

	Return true if hit point in this area is the nearest one.
*/
bool A3DESP::RayToArea()
{
	//	Only use x and z axis
	A3DVECTOR3 vStart(m_Ray.vStart.x, m_Ray.vStart.z, 0);
	A3DVECTOR3 vEnd(m_Ray.vEnd.x, m_Ray.vEnd.z, 0);
	A3DVECTOR3 vDir(m_Ray.vDir.x, m_Ray.vDir.z, 0);
	A3DVECTOR3 vHitCluster1, vHitCluster2;

	float vMins[2], vMaxs[2];
	vMins[0] = m_Area.vMins[0] + 0.2f;
	vMins[1] = m_Area.vMins[1] + 0.2f;
	vMaxs[0] = m_Area.vMaxs[0] - 0.2f;
	vMaxs[1] = m_Area.vMaxs[1] - 0.2f;
	
	//	Calculate hit point of ray (from start to end) and cluster
	if (!CLS_RayToAABB2(vStart, vDir, vMins, vMaxs, vHitCluster1))
		return false;

	//	Calculate hit point of ray (from end to start) and cluster
	if (!CLS_RayToAABB2(vEnd, -vDir, vMins, vMaxs, vHitCluster2))
		return false;

	m_pEverHitBlock = NULL;

	//	Prepare to use 2DDDA algorithm
	Init2DDDA(vHitCluster1, vHitCluster2);

	//	Trace ray in area with 2DDDA algorithm
	return TraceRayInArea();
}

/*	Initialize variables for tracing ray in area

	v0, v1: start and end point of ray, they must in area and only the first
			two components of them are valid.
*/
void A3DESP::Init2DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1)
{
	A3DVECTOR3 vDelta = v1 - v0;
	A3DVECTOR3 vAbs;
	int i, s[2], vStart[2], vEnd[2], vSteps[2];
	float d[2], fSlope;

	//	TODO: avoid dev 0 error
//	if (vDelta == A3DVECTOR3(0, 0, 0))
//		return;

	for (i=0; i < 2; i++)
	{
		vStart[i]	= (int)((v0.m[i] - m_Area.vAlignMins[i]) * m_Area.vInvLength[i]);
		vEnd[i]		= (int)((v1.m[i] - m_Area.vAlignMins[i]) * m_Area.vInvLength[i]);

		if (vDelta.m[i] < 0)
		{
			vAbs.m[i]	= -vDelta.m[i];
			s[i]		= -1;
			vSteps[i]	= vStart[i] - vEnd[i];
			d[i]		= v0.m[i] - (m_Area.vAlignMins[i] + vStart[i] * m_Area.vLength[i]);
		}
		else
		{
			vAbs.m[i]	= vDelta.m[i];
			s[i]		= 1;
			vSteps[i]	= vEnd[i] - vStart[i];
			d[i]		= m_Area.vAlignMins[i] + (vStart[i] + 1) * m_Area.vLength[i] - v0.m[i]; 
		}

		m_2DDDA.vStart[i]	= vStart[i];
		m_2DDDA.vEnd[i]		= vEnd[i];
	}

	if (vAbs.m[0] > vAbs.m[1])	//	u major
	{
		m_2DDDA.iMajor = 0;
		fSlope = vAbs.m[1] / vAbs.m[0];
		
		m_2DDDA.dv	= d[1];
		m_2DDDA.ev	= m_Area.vLength[1] - d[1];
		m_2DDDA.iv	= fSlope * m_Area.vLength[0];
		m_2DDDA.cv	= fSlope * d[0];
	}
	else	//	v major
	{
		m_2DDDA.iMajor = 1;
		fSlope = vAbs.m[0] / vAbs.m[1];
		
		m_2DDDA.du	= d[0];
		m_2DDDA.eu	= m_Area.vLength[0] - d[0];
		m_2DDDA.iu	= fSlope * m_Area.vLength[1];
		m_2DDDA.cu	= fSlope * d[1];
	}

	m_2DDDA.su = s[0];
	m_2DDDA.sv = s[1];
	m_2DDDA.iNumSteps = vSteps[m_2DDDA.iMajor];
}

/*	Trace ray in area.

	Return true if hit point in this area is the nearest one.
*/
bool A3DESP::TraceRayInArea()
{
	int iSpanU, i, u, v;
	int iSpanV = m_Area.vSize[0];

	u = m_2DDDA.vStart[0];
	v = m_2DDDA.vStart[1];

	PESPBLOCK pBlock = &m_aBlocks[iSpanV*v + u];

	iSpanU = m_2DDDA.su;

	if (m_2DDDA.sv < 0)
		iSpanV = -iSpanV;

	int su = m_2DDDA.su;
	int sv = m_2DDDA.sv;

	//	Visit the start block
	if (TraceRayInBlock(pBlock))
		return true;

	//	Handle a special case, voxel increament on major axis is 0. When
	//	start and end point are near to each other, this case will occur
	if (!m_2DDDA.iNumSteps)
	{
		if (m_2DDDA.iMajor == 0)	//	u major
		{
			if (v != m_2DDDA.vEnd[1])
			{
				//	Visit (u, v+sv)
				if (TraceRayInBlock(pBlock + iSpanV))
					return true;
			}				
		}
		else	//	v major
		{
			if (u != m_2DDDA.vEnd[0])
			{
				//	Visit (u+su, v)
				if (TraceRayInBlock(pBlock + iSpanU))
					return true;
			}
		}

		return false;
	}

	if (m_2DDDA.iMajor == 0)	//	u major
	{
		float ev	= m_2DDDA.ev;
		float fLenV = (float)m_Area.vLength[1];
		bool bIncv	= false;

		//	<= here is necessary. Through we may take one more step, but this
		//	ensure us won't miss any possible blocks.
		for (i=0; i <= m_2DDDA.iNumSteps; i++)
		{
			ev += m_2DDDA.iv;

			if (ev > fLenV)
			{
				v	   += m_2DDDA.sv;
				ev	   -= fLenV;
				pBlock += iSpanV;
				bIncv	= true;
			}

			u		+= m_2DDDA.su;
			pBlock	+= iSpanU;

			//	Ensure blocks are 4-connected
			if (bIncv)
			{
				if (m_2DDDA.cv > m_2DDDA.dv)
				{
					//	Visit (u-su, v)
					if (TraceRayInBlock(pBlock - iSpanU))
						return true;
				}
				else
				{
					//	Visit (u, v-sv)
					if (TraceRayInBlock(pBlock - iSpanV))
						return true;
				}

				bIncv = false;
			}

			//	Visit (u, v)
			if (TraceRayInBlock(pBlock))
				return true;

			m_2DDDA.dv = fLenV - ev;
		}
	}
	else	//	v major
	{
		float eu	= m_2DDDA.eu;
		float fLenU = (float)m_Area.vLength[0];
		bool bIncu	= false;

		//	<= here is necessary. Through we may take one more step, but this
		//	ensure us won't miss any possible blocks.
		for (i=0; i <= m_2DDDA.iNumSteps; i++)
		{
			eu += m_2DDDA.iu;

			if (eu > fLenU)
			{
				u	   += m_2DDDA.su;
				eu	   -= fLenU;
				pBlock += iSpanU;
				bIncu	= true;
			}

			v		+= m_2DDDA.sv;
			pBlock	+= iSpanV;

			//	Ensure blocks are 4-connected
			if (bIncu)
			{
				if (m_2DDDA.cu > m_2DDDA.du)
				{
					//	Visit (u, v-sv)
					if (TraceRayInBlock(pBlock - iSpanV))
						return true;
				}
				else
				{
					//	Visit (u-su, v)
					if (TraceRayInBlock(pBlock - iSpanU))
						return true;
				}

				bIncu = false;
			}

			//	Visit (u, v)
			if (TraceRayInBlock(pBlock))
				return true;

			m_2DDDA.du = fLenU - eu;
		}
	}	

	return false;
}

/*	Trace ray in specified block.

	Return true if hit point in this block is the nearest one.

	pBlock: block the ray is in
*/
bool A3DESP::TraceRayInBlock(PESPBLOCK pBlock)
{
	//	This is necessary if we use "i <= steps" rather than 
	//	"i < steps" in TraceRayInArea()
	if (pBlock < m_aBlocks || pBlock >= m_aBlocks + m_iNumBlock)
		return true;

	int i;
	bool bRet = false;
	PESPCLUSTER pCluster;

	for (i=0; i < pBlock->iNumRefs; i++)
	{
		pCluster = &m_aClusters[pBlock->aClusterRefs[i]];
		if (pCluster->dwTraceCnt == m_dwTraceCnt)
			continue;

		if (RayToCluster(pBlock->aClusterRefs[i]))
		{
			//	Check whether hit point is in current leaf
			int u = (int)((m_Ray.vPoint.m[0] - m_Area.vAlignMins[0]) * m_Area.vInvLength[0]);
			int v = (int)((m_Ray.vPoint.m[2] - m_Area.vAlignMins[1]) * m_Area.vInvLength[1]);
			m_pEverHitBlock = &m_aBlocks[v * m_Area.vSize[0] + u];
			
			if (m_pEverHitBlock == pBlock)
				bRet = true;
		}

		pCluster->dwTraceCnt = m_dwTraceCnt;
	}

	if (bRet || m_pEverHitBlock == pBlock)
		return true;

	return false;
}

/*	Check whether a ray collision with specified cluster, if true, trace ray into
	cluster and check which side in it hit by the ray.

	Return true if hit point in this cluster is the nearest one.

	iCluster: specified cluster.
*/
bool A3DESP::RayToCluster(int iCluster)
{
	PESPCLUSTER pCluster = &m_aClusters[iCluster];
	A3DVECTOR3 vHitCluster1, vHitCluster2, vNormal;
	FLOAT fFraction;

	//	No sides in this cluster
	if (!pCluster->aSides)
		return false;
	
	A3DVECTOR3 vMins = pCluster->vMins - A3DVECTOR3(0.2f);
	A3DVECTOR3 vMaxs = pCluster->vMaxs + A3DVECTOR3(0.2f);

	//	Calculate hit point of ray (from start to end) and cluster
	if (!CLS_RayToAABB3(m_Ray.vStart, m_Ray.vDir, vMins, vMaxs, vHitCluster1, 
						&fFraction, vNormal))
		return false;

	//	Convert ray's direction
	A3DVECTOR3 vDir = m_Ray.vStart - m_Ray.vEnd;

	//	Calculate hit point of ray (from end to start) and cluster
	if (!CLS_RayToAABB3(m_Ray.vEnd, vDir, vMins, vMaxs, vHitCluster2, 
						&fFraction, vNormal))
		return false;

	//	Use vHitCluster as new start point and calcualte voxel coordinates for it
	//	and end point.
	m_pCurCluster	= pCluster;
	m_pEverHitLeaf	= NULL;

	if (m_Ray.iAxial >= 0)
	{
		//	Prepare to use 3DDDA algorithm
		AxialInit3DDDA(vHitCluster1, vHitCluster2);

		//	Trace ray in cluster with 3DDDA algorithm
		if (AxialTraceRayInCluster() || m_pEverHitLeaf)
			return true;
	}
	else
	{
		//	Prepare to use 3DDDA algorithm
		Init3DDDA(vHitCluster1, vHitCluster2);

		//	Trace ray in cluster with 3DDDA algorithm
		if (TraceRayInCluster() || m_pEverHitLeaf)
			return true;
	}

	return false;
}

/*	Initialize variables for tracing ray in cluster.

	v0: ray's start point
	v1: ray's end point
*/
void A3DESP::Init3DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1)
{
	A3DVECTOR3 vDelta = v1 - v0;
	A3DVECTOR3 vAbs;
	int i, s[3], vStart[3], vEnd[3], vSteps[3];
	float d[3], fSlope1, fSlope2;

	for (i=0; i < 3; i++)
	{
		vStart[i]	= (int)((v0.m[i] - m_pCurCluster->vAlignMins.m[i]) * m_pCurCluster->vInvLength.m[i]);
		vEnd[i]		= (int)((v1.m[i] - m_pCurCluster->vAlignMins.m[i]) * m_pCurCluster->vInvLength.m[i]);

		if (vDelta.m[i] < 0.0f)
		{
			vAbs.m[i]	= -vDelta.m[i];
			s[i]		= -1;
			vSteps[i]	= vStart[i] - vEnd[i];
			d[i]		= v0.m[i] - (m_pCurCluster->vAlignMins.m[i] + vStart[i] * m_pCurCluster->vLength[i]);
		}
		else
		{
			vAbs.m[i]	= vDelta.m[i];
			s[i]		= 1;
			vSteps[i]	= vEnd[i] - vStart[i];
			d[i]		= m_pCurCluster->vAlignMins.m[i] + (vStart[i] + 1) * m_pCurCluster->vLength[i] - v0.m[i]; 
		}

		m_3DDDA.vStart[i]	= vStart[i];
		m_3DDDA.vEnd[i]		= vEnd[i];
	}

	if (vAbs.x > vAbs.y)
	{
		if (vAbs.x > vAbs.z)	//	x major
		{
			m_3DDDA.iMajor = 0;
			fSlope1 = vAbs.m[1] / vAbs.m[0];
			fSlope2 = vAbs.m[2] / vAbs.m[0];

			m_3DDDA.dy	= d[1];
			m_3DDDA.dz	= d[2];
			m_3DDDA.ey	= m_pCurCluster->vLength[1] - d[1];
			m_3DDDA.ez	= m_pCurCluster->vLength[2] - d[2];
			m_3DDDA.iy	= fSlope1 * m_pCurCluster->vLength[0];
			m_3DDDA.iz	= fSlope2 * m_pCurCluster->vLength[0];
			m_3DDDA.cy	= fSlope1 * d[0];
			m_3DDDA.cz	= fSlope2 * d[0];
		}
		else	//	z major
			m_3DDDA.iMajor = 2;
	}
	else
	{
		if (vAbs.y > vAbs.z)	//	y major
		{
			m_3DDDA.iMajor = 1;
			fSlope1 = vAbs.m[0] / vAbs.m[1];
			fSlope2 = vAbs.m[2] / vAbs.m[1];

			m_3DDDA.dx	= d[0];
			m_3DDDA.dz	= d[2];
			m_3DDDA.ex	= m_pCurCluster->vLength[0] - d[0];
			m_3DDDA.ez	= m_pCurCluster->vLength[2] - d[2];
			m_3DDDA.ix	= fSlope1 * m_pCurCluster->vLength[1];
			m_3DDDA.iz	= fSlope2 * m_pCurCluster->vLength[1];
			m_3DDDA.cx	= fSlope1 * d[1];
			m_3DDDA.cz	= fSlope2 * d[1];
		}
		else	//	z major
			m_3DDDA.iMajor = 2;
	}

	if (m_3DDDA.iMajor == 2)
	{
		fSlope1 = vAbs.m[0] / vAbs.m[2];
		fSlope2 = vAbs.m[1] / vAbs.m[2];
		
		m_3DDDA.dx	= d[0];
		m_3DDDA.dy	= d[1];
		m_3DDDA.ex	= m_pCurCluster->vLength[0] - d[0];
		m_3DDDA.ey	= m_pCurCluster->vLength[1] - d[1];
		m_3DDDA.ix	= fSlope1 * m_pCurCluster->vLength[2];
		m_3DDDA.iy	= fSlope2 * m_pCurCluster->vLength[2];
		m_3DDDA.cx	= fSlope1 * d[2];
		m_3DDDA.cy	= fSlope2 * d[2];
	}

	m_3DDDA.sx = s[0];
	m_3DDDA.sy = s[1];
	m_3DDDA.sz = s[2];
	m_3DDDA.iNumSteps = vSteps[m_3DDDA.iMajor];
}

//	Initialize variable for axial ray tracing in cluster
void A3DESP::AxialInit3DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1)
{
	int i, s;

	m_3DDDA.iMajor = m_Ray.iAxial;

	for (i=0; i < 3; i++)
	{
		m_3DDDA.vStart[i]	= (int)((v0.m[i] - m_pCurCluster->vAlignMins.m[i]) * m_pCurCluster->vInvLength.m[i]);
		m_3DDDA.vEnd[i]		= (int)((v1.m[i] - m_pCurCluster->vAlignMins.m[i]) * m_pCurCluster->vInvLength.m[i]);

		if (i == m_3DDDA.iMajor)
		{
			if (m_3DDDA.vStart[i] <= m_3DDDA.vEnd[i])
			{
				m_3DDDA.iNumSteps = m_3DDDA.vEnd[i] - m_3DDDA.vStart[i];
				s = 1;
			}
			else
			{
				m_3DDDA.iNumSteps = m_3DDDA.vStart[i] - m_3DDDA.vEnd[i];
				s = -1;
			}
		}
	}

	m_3DDDA.sx = s;
	m_3DDDA.sy = s;
	m_3DDDA.sz = s;
}

/*	Trace ray in cluster with 3DDDA algorighm

	Return true if ray hit one side in current cluster
*/
bool A3DESP::TraceRayInCluster()
{
	int iSpanX, i, x, y, z;
	int iSpanY = m_pCurCluster->vSpans[1];
	int iSpanZ = m_pCurCluster->vSpans[2];

	x = m_3DDDA.vStart[0];
	y = m_3DDDA.vStart[1];
	z = m_3DDDA.vStart[2];

	PESPLEAF pLeaf = &m_pCurCluster->aLeaves[iSpanY*y + iSpanZ*z + x];

	iSpanX = m_3DDDA.sx;

	if (m_3DDDA.sy < 0)
		iSpanY = -iSpanY;

	if (m_3DDDA.sz < 0)
		iSpanZ = -iSpanZ;

	int sx = m_3DDDA.sx;
	int sy = m_3DDDA.sy;
	int sz = m_3DDDA.sz;

	//	Visit the start voxel
	if (TraceRayInLeaf(pLeaf))
		return true;

	//	Handle a special case, voxel increament on major axis is 0. When
	//	start and end point are near to each other, this case will occur
	if (!m_3DDDA.iNumSteps)
	{
		if (m_3DDDA.iMajor == 0)	//	x major
		{
			if (y != m_3DDDA.vEnd[1] && z != m_3DDDA.vEnd[2])
			{
				if (m_3DDDA.dy * m_3DDDA.iz - m_3DDDA.dz * m_3DDDA.iy > 0)
				{
					//	Visit (x, y, z+sz)
					if (TraceRayInLeaf(pLeaf + iSpanZ))
						return true;
				}
				else
				{
					//	Visit (x, y+sy, z)
					if (TraceRayInLeaf(pLeaf + iSpanY))
						return true;
				}

				//	Visit (x, y+sy, z+sz)
				if (TraceRayInLeaf(pLeaf + iSpanY + iSpanZ))
					return true;
			}
			else if (y != m_3DDDA.vEnd[1])
			{
				//	Visit (x, y+sy, z)
				if (TraceRayInLeaf(pLeaf + iSpanY))
					return true;
			}
			else if (z != m_3DDDA.vEnd[2])
			{
				//	Visit (x, y, z+sz)
				if (TraceRayInLeaf(pLeaf + iSpanZ))
					return true;
			}
		}
		else if (m_3DDDA.iMajor == 1)	//	y major
		{
			if (x != m_3DDDA.vEnd[0] && z != m_3DDDA.vEnd[2])
			{
				if (m_3DDDA.dx * m_3DDDA.iz - m_3DDDA.dz * m_3DDDA.ix > 0)
				{
					//	Visit (x, y, z+sz)
					if (TraceRayInLeaf(pLeaf + iSpanZ))
						return true;
				}
				else
				{
					//	Visit (x+sx, y, z)
					if (TraceRayInLeaf(pLeaf + iSpanX))
						return true;
				}

				//	Visit (x+sx, y, z+sz)
				if (TraceRayInLeaf(pLeaf + iSpanX + iSpanZ))
					return true;
			}
			else if (x != m_3DDDA.vEnd[0])
			{
				//	Visit (x+sx, y, z)
				if (TraceRayInLeaf(pLeaf + iSpanX))
					return true;
			}
			else if (z != m_3DDDA.vEnd[2])
			{
				//	Visit (x, y, z+sz)
				if (TraceRayInLeaf(pLeaf + iSpanZ))
					return true;
			}
		}
		else	//	z major
		{
			if (x != m_3DDDA.vEnd[0] && y != m_3DDDA.vEnd[1])
			{
				if (m_3DDDA.dx * m_3DDDA.iy - m_3DDDA.dy * m_3DDDA.ix > 0)
				{
					//	Visit (x, y+sy, z)
					if (TraceRayInLeaf(pLeaf + iSpanY))
						return true;
				}
				else
				{
					//	Visit (x+sx, y, z)
					if (TraceRayInLeaf(pLeaf + iSpanX))
						return true;
				}

				//	Visit (x+sx, y+sy, z)
				if (TraceRayInLeaf(pLeaf + iSpanX + iSpanY))
					return true;
			}
			else if (x != m_3DDDA.vEnd[0])
			{
				//	Visit (x+sx, y, z)
				if (TraceRayInLeaf(pLeaf + iSpanX))
					return true;
			}
			else if (y != m_3DDDA.vEnd[1])
			{
				//	Visit (x, y+sy, z)
				if (TraceRayInLeaf(pLeaf + iSpanY))
					return true;
			}
		}

		return false;
	}

	if (m_3DDDA.iMajor == 0)		//	x major
	{
		float ey	= m_3DDDA.ey;
		float ez	= m_3DDDA.ez;
		float fLenY = (float)m_pCurCluster->vLength[1];
		float fLenZ = (float)m_pCurCluster->vLength[2];
		bool bIncy	= false;
		bool bIncz	= false;
		int y1 = y, z1 = z;

		//	<= here is necessary. Through we may take one more step, but this
		//	ensure us won't miss any possible leaves.
		for (i=0; i <= m_3DDDA.iNumSteps; i++)
		{
			ey += m_3DDDA.iy;
			ez += m_3DDDA.iz;

			if (ey > fLenY)
			{
				y	   += m_3DDDA.sy;
				ey	   -= fLenY;
				pLeaf  += iSpanY;
				bIncy	= true;

				if (m_3DDDA.cy > m_3DDDA.dy)
					y1 = y;
			}

			if (ez > fLenZ)
			{
				z	   += m_3DDDA.sz;
				ez	   -= fLenZ;
				pLeaf  += iSpanZ;
				bIncz	= true;

				if (m_3DDDA.cz > m_3DDDA.dz)
					z1 = z;
			}

			x		+= m_3DDDA.sx;
			pLeaf	+= iSpanX;

			//	Ensure voxels are 6-connected
			if (bIncy && bIncz)
			{
				bIncy = bIncz = false;

				if (m_3DDDA.dy * m_3DDDA.iz - m_3DDDA.dz * m_3DDDA.iy > 0)
				{
					if (z1 == z)
					{
						//	Visit (x-sx, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanY))
							return true;

						if (y1 == y)
						{
							//	Visit (x-sx, y, z)
							if (TraceRayInLeaf(pLeaf - iSpanX))
								return true;
						}
						else
						{
							//	Visit (x, y-sy, z)
							if (TraceRayInLeaf(pLeaf - iSpanY))
								return true;
						}
					}
					else
					{
						//	Visit (x, y-sy, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanY - iSpanZ))
							return true;

						//	Visit (x, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanY))
							return true;
					}
				}
				else
				{
					if (y1 == y)
					{
						//	Visit (x-sx, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanZ))
							return true;

						if (z1 == z)
						{
							//	Visit (x-sx, y, z)
							if (TraceRayInLeaf(pLeaf - iSpanX))
								return true;
						}
						else
						{
							//	Visit (x, y, z-sz)
							if (TraceRayInLeaf(pLeaf - iSpanZ))
								return true;
						}
					}
					else
					{
						//	Visit (x, y-sy, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanY - iSpanZ))
							return true;

						//	Visit (x, y, z-sx)
						if (TraceRayInLeaf(pLeaf - iSpanZ))
							return true;
					}
				}
			}
			else if (bIncy)
			{
				if (y1 == y)
				{
					//	Visit (x-sx, y, z)
					if (TraceRayInLeaf(pLeaf - iSpanX))
						return true;
				}
				else
				{
					//	Visit (x, y-sy, z)
					if (TraceRayInLeaf(pLeaf - iSpanY))
						return true;
				}

				bIncy = false;
			}
			else if (bIncz)
			{
				if (z1 == z)
				{
					//	Visit (x-sx, y, z)
					if (TraceRayInLeaf(pLeaf - iSpanX))
						return true;
				}
				else
				{
					//	Visit (x, y, z-sz)
					if (TraceRayInLeaf(pLeaf - iSpanZ))
						return true;
				}

				bIncz = false;
			}

			//	Visit (x, y, z)
			if (TraceRayInLeaf(pLeaf))
				return true;

			m_3DDDA.dy = fLenY - ey;
			m_3DDDA.dz = fLenZ - ez;
		}
	}
	else if (m_3DDDA.iMajor == 1)	//	y major
	{
		float ex	= m_3DDDA.ex;
		float ez	= m_3DDDA.ez;
		float fLenX = (float)m_pCurCluster->vLength[0];
		float fLenZ = (float)m_pCurCluster->vLength[2];
		bool bIncx	= false;
		bool bIncz	= false;
		int x1 = x, z1 = z;

		//	<= here is necessary. Through we may take one more step, but this
		//	ensure us won't miss any possible leaves.
		for (i=0; i <= m_3DDDA.iNumSteps; i++)
		{
			ex += m_3DDDA.ix;
			ez += m_3DDDA.iz;

			if (ex > fLenX)
			{
				x	   += m_3DDDA.sx;
				ex	   -= fLenX;
				pLeaf  += iSpanX;
				bIncx	= true;

				if (m_3DDDA.cx > m_3DDDA.dx)
					x1 = x;
			}

			if (ez > fLenZ)
			{
				z	   += m_3DDDA.sz;
				ez	   -= fLenZ;
				pLeaf  += iSpanZ;
				bIncz	= true;

				if (m_3DDDA.cz > m_3DDDA.dz)
					z1 = z;
			}

			y		+= m_3DDDA.sy;
			pLeaf	+= iSpanY;

			//	Ensure voxels are 6-connected
			if (bIncx && bIncz)
			{
				bIncx = bIncz = false;

				if (m_3DDDA.dx * m_3DDDA.iz - m_3DDDA.dz * m_3DDDA.ix > 0)
				{
					if (z1 == z)
					{
						//	Visit (x-sx, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanY))
							return true;

						if (x1 == x)
						{
							//	Visit (x, y-sy, z)
							if (TraceRayInLeaf(pLeaf - iSpanY))
								return true;
						}
						else
						{
							//	Visit (x-sx, y, z)
							if (TraceRayInLeaf(pLeaf - iSpanX))
								return true;
						}
					}
					else
					{
						//	Visit (x-sx, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanZ))
							return true;

						//	Visit (x-sx, y, z)
						if (TraceRayInLeaf(pLeaf - iSpanX))
							return true;
					}
				}
				else
				{
					if (x1 == x)
					{
						//	Visit (x, y-sy, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanY - iSpanZ))
							return true;

						if (z1 == z)
						{
							//	Visit (x, y-sy, z)
							if (TraceRayInLeaf(pLeaf - iSpanY))
								return true;
						}
						else
						{
							//	Visit (x, y, z-sz)
							if (TraceRayInLeaf(pLeaf - iSpanZ))
								return true;
						}
					}
					else
					{
						//	Visit (x-sx, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanZ))
							return true;

						//	Visit (x, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanZ))
							return true;
					}
				}
			}
			else if (bIncx)
			{
				if (x1 == x)
				{
					//	Visit (x, y-sy, z)
					if (TraceRayInLeaf(pLeaf - iSpanY))
						return true;
				}
				else
				{
					//	Visit (x-sx, y, z)
					if (TraceRayInLeaf(pLeaf - iSpanX))
						return true;
				}

				bIncx = false;
			}
			else if (bIncz)
			{
				if (z1 == z)
				{
					//	Visit (x, y-sy, z)
					if (TraceRayInLeaf(pLeaf - iSpanY))
						return true;
				}
				else
				{
					//	Visit (x, y, z-sz)
					if (TraceRayInLeaf(pLeaf - iSpanZ))
						return true;
				}

				bIncz = false;
			}

			//	Visit (x, y, z)
			if (TraceRayInLeaf(pLeaf))
				return true;

			m_3DDDA.dx = fLenX - ex;
			m_3DDDA.dz = fLenZ - ez;
		}
	}
	else	//	z major
	{
		float ex	= m_3DDDA.ex;
		float ey	= m_3DDDA.ey;
		float fLenX = (float)m_pCurCluster->vLength[0];
		float fLenY = (float)m_pCurCluster->vLength[1];
		bool bIncx	= false;
		bool bIncy	= false;
		int x1 = x, y1 = y;

		//	<= here is necessary. Through we may take one more step, but this
		//	ensure us won't miss any possible leaves.
		for (i=0; i <= m_3DDDA.iNumSteps; i++)
		{
			ex += m_3DDDA.ix;
			ey += m_3DDDA.iy;

			if (ex > fLenX)
			{
				x	   += m_3DDDA.sx;
				ex	   -= fLenX;
				pLeaf  += iSpanX;
				bIncx	= true;

				if (m_3DDDA.cx > m_3DDDA.dx)
					x1 = x;
			}

			if (ey > fLenY)
			{
				y	   += m_3DDDA.sy;
				ey	   -= fLenY;
				pLeaf  += iSpanY;
				bIncy	= true;

				if (m_3DDDA.cy > m_3DDDA.dy)
					y1 = y;
			}

			z		+= m_3DDDA.sz;
			pLeaf	+= iSpanZ;

			//	Ensure voxels are 6-connected
			if (bIncx && bIncy)
			{
				bIncx = bIncy = false;

				if (m_3DDDA.dy * m_3DDDA.ix - m_3DDDA.dx * m_3DDDA.iy > 0)
				{
					if (x1 == x)
					{
						//	Visit (x, y-sy, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanY - iSpanZ))
							return true;

						if (y1 == y)
						{
							//	Visit (x, y, z-sz)
							if (TraceRayInLeaf(pLeaf - iSpanZ))
								return true;
						}
						else
						{
							//	Visit (x, y-sy, z)
							if (TraceRayInLeaf(pLeaf - iSpanY))
								return true;
						}
					}
					else
					{
						//	Visit (x-sx, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanY))
							return true;

						//	Visit (x, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanY))
							return true;
					}
				}
				else
				{
					if (y1 == y)
					{
						//	Visit (x-sx, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanZ))
							return true;

						if (x1 == x)
						{
							//	Visit (x, y, z-sz)
							if (TraceRayInLeaf(pLeaf - iSpanZ))
								return true;
						}
						else
						{
							//	Visit (x-sx, y, z)
							if (TraceRayInLeaf(pLeaf - iSpanX))
								return true;
						}
					}
					else
					{
						//	Visit (x-sx, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanY))
							return true;

						//	Visit (x-sx, y, z)
						if (TraceRayInLeaf(pLeaf - iSpanX))
							return true;
					}
				}
			}
			else if (bIncx)
			{
				if (x1 == x)
				{
					//	Visit (x, y, z-sz)
					if (TraceRayInLeaf(pLeaf - iSpanZ))
						return true;
				}
				else
				{
					//	Visit (x-sx, y, z)
					if (TraceRayInLeaf(pLeaf - iSpanX))
						return true;
				}

				bIncx = false;
			}
			else if (bIncy)
			{
				if (y1 == y)
				{
					//	Visit (x, y, z-sz)
					if (TraceRayInLeaf(pLeaf - iSpanZ))
						return true;
				}
				else
				{
					//	Visit (x, y-sy, z)
					if (TraceRayInLeaf(pLeaf - iSpanY))
						return true;
				}

				bIncy = false;
			}

			//	Visit (x, y, z)
			if (TraceRayInLeaf(pLeaf))
				return true;

			m_3DDDA.dx = fLenX - ex;
			m_3DDDA.dy = fLenY - ey;
		}	
	}

	return false;
}

//	Axial trace ray in current cluster
bool A3DESP::AxialTraceRayInCluster()
{
	int iSpan, i;

	PESPLEAF pLeaf = &m_pCurCluster->aLeaves[m_pCurCluster->vSpans[1] * m_3DDDA.vStart[1] + 
											 m_pCurCluster->vSpans[2] * m_3DDDA.vStart[2] + 
											 m_3DDDA.vStart[0]];
	switch (m_3DDDA.iMajor)
	{
	case 0:	//	x major

		iSpan = m_3DDDA.sx;
		break;

	case 1:	//	y major

		iSpan = m_pCurCluster->vSpans[1];

		if (m_3DDDA.sy < 0)
			iSpan = -iSpan;
	
		break;

	case 2:	//	z major

		iSpan = m_pCurCluster->vSpans[2];

		if (m_3DDDA.sz < 0)
			iSpan = -iSpan;

		break;
	}

	for (i=0; i <= m_3DDDA.iNumSteps; i++)
	{
		//	Visit voxel
		if (TraceRayInLeaf(pLeaf))
			return true;

		pLeaf += iSpan;
	}

	return false;
}

/*	Trace ray in leaf.

	Return true if hit point in this leaf is the nearest one.

	pLeaf: spedified leaf
*/
bool A3DESP::TraceRayInLeaf(PESPLEAF pLeaf)
{
	//	This is necessary if we use "i <= steps" rather than 
	//	"i < steps" in TraceRayInCluster()
	if (pLeaf < m_aLeaves || pLeaf >= m_aLeaves + m_iNumLeaf)
		return false;

	int i;
	PESPSIDE pSide;
	bool bRet = false;

	for (i=0; i < pLeaf->iNumRefs; i++)
	{
		pSide = &m_pCurCluster->aSides[pLeaf->aSideRefs[i]];
		if (pSide->dwTraceCnt == m_dwTraceCnt)
			continue;

		m_iNumCheckedSide++; 

		if (RayToSide(pSide, pLeaf))
			bRet = true;

		pSide->dwTraceCnt = m_dwTraceCnt;
	}

	if (bRet)
		return true;
	else if (m_pEverHitLeaf == pLeaf)	//	Hit ever occures in this leaf ?
		return true;
	
	return false;
}

/*	Check whether a ray hit specified side and hit point is in specified leaf.

	Return true if the nearest hit occures in specified leaf, otherwise return false.

	pSide: specified side
	pLeaf: collision detection is be doing in this leaf
*/	
bool A3DESP::RayToSide(PESPSIDE pSide, PESPLEAF pLeaf)
{
	float d, d1, d2;
	PA3DPLANE pPlane;
	A3DVECTOR3 vNormal;

	if (!(pPlane = pSide->pPlane))
		return false;

	if (pPlane->byType > 5)
	{
		d1 = DotProduct(m_Ray.vStart, pPlane->vNormal) - pPlane->fDist;
		d2 = DotProduct(m_Ray.vEnd, pPlane->vNormal) - pPlane->fDist;
	}
	else if (pPlane->byType < 3)
	{
		d1 = m_Ray.vStart.m[pPlane->byType] - pPlane->fDist;
		d2 = m_Ray.vEnd.m[pPlane->byType] - pPlane->fDist;
	}
	else	//	pPlane->byType < 6
	{
		d1 = -m_Ray.vStart.m[pPlane->byType-3] - pPlane->fDist;
		d2 = -m_Ray.vEnd.m[pPlane->byType-3] - pPlane->fDist;
	}

	if ((d1 < 0.0f && d2 < 0.0f) || (d1 > 0.0f && d2 > 0.0f))
		return false;

	if (pSide->dwFlags & SIDEFLAG_TWOSIDES)
		vNormal = d1 < 0.0f ? -pPlane->vNormal : pPlane->vNormal;
	else
	{
		//	Start point is behind side
		if (d1 < 0)
			return false;
	
		vNormal = pPlane->vNormal;
	}

	if (DotProduct(m_Ray.vDir, vNormal) >= 0.0f)
		return false;

	if (d1 < 0.0f)
		d = d1 / (d1 - d2);
	else
		d = d1 / (d1 - d2);

	if (d < 0.0f)
		d = 0.0f;

	if (d >= m_Ray.fFraction)
		return false;

	A3DVECTOR3 vInter;
	bool bInter=false, bFlag1, bFlag2;
	int i, i0, i1;
	float *vert1, *vert2, *vHit;

	//	Calculate intersection point of line and plane
	vInter	= m_Ray.vDir * d + m_Ray.vStart;
	vHit	= vInter.m;

	switch (pPlane->byType)
	{
	case A3DPLANE_PX:
	case A3DPLANE_NX:
	case A3DPLANE_MAJORX:	i0 = 1;	i1 = 2;	break;

	case A3DPLANE_PY:
	case A3DPLANE_NY:
	case A3DPLANE_MAJORY:	i0 = 0; i1 = 2; break;

	case A3DPLANE_PZ:
	case A3DPLANE_NZ:
	case A3DPLANE_MAJORZ:	i0 = 0; i1 = 1; break;

	default:	return false;
	}

	WORD* aIndices = pSide->aIndices;

	vert1	= pSide->aVerts[aIndices[pSide->iNumIdx-1]].m;
	bFlag1	= (vHit[i1] >= vert1[i1]);

	for (i=0; i < pSide->iNumIdx; i++)
	{
		vert2	= pSide->aVerts[aIndices[i]].m;
		bFlag2	= (vHit[i1] >= vert2[i1]);

		if (bFlag1 != bFlag2)
		{
			if (((vert2[i1] - vHit[i1]) * (vert1[i0] - vert2[i0]) >=
				(vert2[i0] - vHit[i0]) * (vert1[i1] - vert2[i1])) == bFlag2)
				bInter = !bInter;
		}

		vert1  = vert2;
		bFlag1 = bFlag2;
	}

	//	Calculate intersection point's position
	if (bInter)
	{
		m_Ray.fHitFrac	= d1 / (d1 - d2);
		m_Ray.fFraction = d1 < 0.0f ? (d1+0.01f) / (d1 - d2) : (d1-0.01f) / (d1 - d2);
		m_Ray.vPoint	= m_Ray.vStart + m_Ray.vDir * m_Ray.fFraction;
		m_Ray.pSide		= pSide;

		if (m_Ray.fFraction < 0.0f)
			m_Ray.fFraction = 0.0f;

		if (d1 >= 0)
			m_Ray.vNormal = pPlane->vNormal;
		else
			m_Ray.vNormal = A3DVECTOR3(0, 0, 0) - pPlane->vNormal;

		//	Check whether hit point is in current leaf
		int x = (int)((vInter.m[0] - m_pCurCluster->vAlignMins.m[0]) * m_pCurCluster->vInvLength.x);
		int y = (int)((vInter.m[1] - m_pCurCluster->vAlignMins.m[1]) * m_pCurCluster->vInvLength.y);
		int z = (int)((vInter.m[2] - m_pCurCluster->vAlignMins.m[2]) * m_pCurCluster->vInvLength.z);
		m_pEverHitLeaf = &m_pCurCluster->aLeaves[y * m_pCurCluster->vSpans[1] + z * m_pCurCluster->vSpans[2] + x];

		if (m_pEverHitLeaf == pLeaf)
			return true;
	}

	return false;
}

/*	Split explosion mark.

	Return true for success, otherwise return false.
	
	aabb: explosion aabb.
	vNormal: normal of plane on which explosion occured.
	aVerts: buffer used to receive splited marks
	aIndices: buffer used to receive index
	bJog: true, push mark out a little.
	piNumVert (out): number of vertices in aVerts.
	piNumIdx (out): number of indices in aIndices.
*/
bool A3DESP::SplitMark(A3DAABB& aabb, A3DVECTOR3 vNormal, A3DLVERTEX* aVerts, 
					   WORD* aIndices, bool bJog, int* piNumVert, int* piNumIdx,
					   float fRadiusScale/* 0.2f */)
{
	m_dwTraceCnt++;

	MARKSPLIT MarkSplit;

	MarkSplit.paabb		= &aabb;
	MarkSplit.pvNormal	= &vNormal;
	MarkSplit.aVerts	= aVerts;
	MarkSplit.aIndices	= aIndices;
	MarkSplit.iNumVert	= 0;
	MarkSplit.iNumIdx	= 0;
	MarkSplit.fRadius	= aabb.Extents.x * fRadiusScale;
	MarkSplit.bJog		= bJog;
	MarkSplit.vSumDir	= A3DVECTOR3(0.0f);

	int i;

	for (i=0; i < m_iNumCluster; i++)
	{
		//	Check whether aabb intersect with cluster
		if (CLS_AABBToAABB(aabb.Center, aabb.Extents, m_aClusters[i].vCenter, m_aClusters[i].vExtents))
		{
			m_pCurCluster = &m_aClusters[i];
			SplitMarkInCluster(&MarkSplit);

			break;	//	Don't check other cluster anymore
		}
	}

	//	Push all clipped vertices along a proper direction, this may fix
	//	splits in mark
	A3DVECTOR3 vJog = Normalize(MarkSplit.vSumDir) * (bJog ? 0.1f : 0.0f);

	for (i=0; i < MarkSplit.iNumVert; i++)
	{
		aVerts[i].x = aVerts[i].x + vJog.x;
		aVerts[i].y = aVerts[i].y + vJog.y;
		aVerts[i].z = aVerts[i].z + vJog.z;
	}

	*piNumVert	= MarkSplit.iNumVert;
	*piNumIdx	= MarkSplit.iNumIdx;

	return true;
}

/*	Split mark in a cluster

	pMarkSplit: mark splitting information
*/
void A3DESP::SplitMarkInCluster(MARKSPLIT* pMarkSplit)
{
	int i, x, y, z, vMins[3], vMaxs[3];

	for (i=0; i < 3; i++)
	{
		vMins[i] = (int)((pMarkSplit->paabb->Mins.m[i] - m_pCurCluster->vAlignMins.m[i]) * m_pCurCluster->vInvLength.m[i]);
		vMaxs[i] = (int)((pMarkSplit->paabb->Maxs.m[i] - m_pCurCluster->vAlignMins.m[i]) * m_pCurCluster->vInvLength.m[i]);

		if (vMins[i] >= m_pCurCluster->vSize[i] || vMaxs[i] < 0)
			return;

		if (vMins[i] < 0)
			vMins[i] = 0;

		if (vMaxs[i] >= m_pCurCluster->vSize[i])
			vMaxs[i] = m_pCurCluster->vSize[i] - 1;
	}

	//	Check all sides in those clusters
	int iSpanY = m_pCurCluster->vSpans[1];
	int iSpanZ = m_pCurCluster->vSpans[2];

	PESPLEAF pBaseLeaf = &m_pCurCluster->aLeaves[iSpanY*vMins[1] + iSpanZ*vMins[2] + vMins[0]];
	PESPLEAF pLeafY, pLeafZ, pLeaf;
	PESPSIDE pSide;

	pLeafY = pBaseLeaf;

	for (y=vMins[1]; y <= vMaxs[1]; y++, pLeafY+=iSpanY)
	{
		pLeafZ = pLeafY;

		for (z=vMins[2]; z <= vMaxs[2]; z++, pLeafZ+=iSpanZ)
		{
			pLeaf = pLeafZ;

			for (x=vMins[0]; x <= vMaxs[0]; x++, pLeaf++)
			{
				for (i=0; i < pLeaf->iNumRefs; i++)
				{
					pSide = &m_pCurCluster->aSides[pLeaf->aSideRefs[i]];
					if (!pSide->pPlane || pSide->dwTraceCnt == m_dwTraceCnt ||
						pSide->dwFlags & SIDEFLAG_ALPHA)
						continue;

					pMarkSplit->pSide = pSide;
					if (!SplitMarkBySide(pMarkSplit))
						return;	//	Buffer has been full

					pSide->dwTraceCnt = m_dwTraceCnt;
				}
			}
		}
	}
}

/*	Split mark using a side.

	Return false if vertex or index buffer has been full, otherwise return true;

	pMarkSplit: mark splitting information
*/
bool A3DESP::SplitMarkBySide(MARKSPLIT* pMarkSplit)
{
	PESPSIDE pSide	 = pMarkSplit->pSide;
	PA3DPLANE pPlane = pSide->pPlane;
	A3DAABB* paabb	 = pMarkSplit->paabb;
	int i;
	float fDist;

	//	Check whether side is in affected area
	fDist = DotProduct(paabb->Center, pPlane->vNormal) - pPlane->fDist;
	if (fDist < -pMarkSplit->fRadius || fDist > pMarkSplit->fRadius)
		return true;

	if (DotProduct(*pMarkSplit->pvNormal, pPlane->vNormal) < 0.0f)
		return true;

	//	Judge the main axis of side plane
	int t0, t1, t2;
	float fBd1, fBd2, fBd3, fBd4;

	switch (pPlane->byType)
	{
	case A3DPLANE_PX:
	case A3DPLANE_NX:
	case A3DPLANE_MAJORX:	
		
		t0 = 0;	t1 = 2; t2 = 1;
		fBd1 = paabb->Center.z + paabb->Extents.z;
		fBd2 = paabb->Center.y + paabb->Extents.y;
		fBd3 = paabb->Center.z - paabb->Extents.z;
		fBd4 = paabb->Center.y - paabb->Extents.y;
		break;

	case A3DPLANE_PY:
	case A3DPLANE_NY:
	case A3DPLANE_MAJORY:

		t0 = 1;	t1 = 0; t2 = 2;
		fBd1 = paabb->Center.x + paabb->Extents.x;
		fBd2 = paabb->Center.z + paabb->Extents.z;
		fBd3 = paabb->Center.x - paabb->Extents.x;
		fBd4 = paabb->Center.z - paabb->Extents.z;
		break;

	case A3DPLANE_PZ:
	case A3DPLANE_NZ:
	case A3DPLANE_MAJORZ:

		t0 = 2;	t1 = 0; t2 = 1;
		fBd1 = paabb->Center.x + paabb->Extents.x;
		fBd2 = paabb->Center.y + paabb->Extents.y;
		fBd3 = paabb->Center.x - paabb->Extents.x;
		fBd4 = paabb->Center.y - paabb->Extents.y;
		break;

	default:
		return true;
	}

	A3DVECTOR3 aProj1[16], aProj2[16];	//	Vertices projection on main plane
	A3DVECTOR3 *v1, *v2, *aOld, *aNew;
	bool bIn1, bIn2;
	int iNumOld, iNumNew;
	float d, d1, d2;

	//	Copy side's vertex from mesh vertex array
	for (i=0; i < pSide->iNumIdx; i++)
		aProj2[i] = pSide->aVerts[pSide->aIndices[i]];

	//	Clip side using the first border plane
	v1		= &aProj2[pSide->iNumIdx-1];
	bIn1	= ((d1 = v1->m[t1] - fBd1) <= 0);
	iNumNew = 0;
	aNew	= aProj1;

	for (i=0; i < pSide->iNumIdx; i++)
	{
		v2	 = &aProj2[i];
		bIn2 = ((d2 = v2->m[t1] - fBd1) <= 0);
	
		if (bIn1)
		{
			aNew[iNumNew] = *v1;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = fBd1;
				aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
			aNew[iNumNew].m[t1] = fBd1;
			aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if ((iNumOld = iNumNew) < 3)
		return true;

	//	Clip side using next border plane
	aOld	= aNew;
	aNew	= aProj2;
	v1		= &aOld[iNumOld-1];
	bIn1	= ((d1 = v1->m[t2] - fBd2) <= 0);
	iNumNew = 0;

	for (i=0; i < iNumOld; i++)
	{
		v2	 = &aOld[i];
		bIn2 = ((d2 = v2->m[t2] - fBd2) <= 0);
	
		if (bIn1)
		{
			aNew[iNumNew] = *v1;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
				aNew[iNumNew].m[t2] = fBd2;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
			aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
			aNew[iNumNew].m[t2] = fBd2;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if ((iNumOld = iNumNew) < 3)
		return true;

	//	Clip side using next border plane
	aOld	= aNew;
	aNew	= aProj1;
	v1		= &aOld[iNumOld-1];
	bIn1	= ((d1 = v1->m[t1] - fBd3) >= 0);
	iNumNew = 0;

	for (i=0; i < iNumOld; i++)
	{
		v2	 = &aOld[i];
		bIn2 = ((d2 = v2->m[t1] - fBd3) >= 0);
	
		if (bIn1)
		{
			aNew[iNumNew] = *v1;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = fBd3;
				aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
			aNew[iNumNew].m[t1] = fBd3;
			aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if ((iNumOld = iNumNew) < 3)
		return true;

	//	Clip side using the last border plane
	aOld	= aNew;
	aNew	= aProj2;
	v1		= &aOld[iNumOld-1];
	bIn1	= ((d1 = v1->m[t2] - fBd4) >= 0);
	iNumNew = 0;

	for (i=0; i < iNumOld; i++)
	{
		v2	 = &aOld[i];
		bIn2 = ((d2 = v2->m[t2] - fBd4) >= 0);
	
		if (bIn1)
		{
			aNew[iNumNew] = *v1;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
				aNew[iNumNew].m[t2] = fBd4;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
			aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
			aNew[iNumNew].m[t2] = fBd4;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if (iNumNew < 3)
		return true;

	//	Check whether buffer has been full
	if (pMarkSplit->iNumVert + iNumNew > MAXNUM_MARKVERT ||
		pMarkSplit->iNumIdx + (iNumNew - 2) * 3 > MAXNUM_MARKINDEX)
		return false;

	//	Calculate texture coordinates for left vertex
	A3DLVERTEX* pVert = &pMarkSplit->aVerts[pMarkSplit->iNumVert];
//	A3DVECTOR3 vJog = pPlane->vNormal * (pMarkSplit->bJog ? 0.1f : 0.0f);

	d1 = 1.0f / (fBd1 - fBd3);
	d2 = 1.0f / (fBd2 - fBd4);

	for (i=0; i < iNumNew; i++, pVert++)
	{
		pVert->x	= aNew[i].x; // + vJog.x;
		pVert->y	= aNew[i].y; // + vJog.y;
		pVert->z	= aNew[i].z; // + vJog.z;
		pVert->tu	= (aNew[i].m[t1] - fBd3) * d1;
		pVert->tv	= (aNew[i].m[t2] - fBd4) * d2;
	}

	//	Build triangle list
	WORD* aIndices = &pMarkSplit->aIndices[pMarkSplit->iNumIdx];
	
	iNumOld = 0;
	for (i=2; i < iNumNew; i++, aIndices+=3, iNumOld+=3)
	{
		aIndices[0] = pMarkSplit->iNumVert;
		aIndices[1] = pMarkSplit->iNumVert + i - 1;
		aIndices[2] = pMarkSplit->iNumVert + i;
	}

	pMarkSplit->vSumDir	= pMarkSplit->vSumDir + pPlane->vNormal;

	pMarkSplit->iNumVert += iNumNew;
	pMarkSplit->iNumIdx	 += iNumOld;

	return true;
}


