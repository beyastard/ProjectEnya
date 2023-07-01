/*
 * FILE: ESPFile.h
 *
 * DESCRIPTION: Common operations for ESP file
 *
 * CREATED BY: Duyuxin, 2001/10/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_ESPFILE_H_
#define _ESPFILE_H_

#include "A3DTypes.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

//	First 8 bytes in ESP file
#define	ESP_IDENTIFY			(('A'<<24) | ('E'<<16) | ('S'<<8) | 'P')
#define ESP_VERSION				3	

//	Number of lumps in ESP file
#define NUM_ESPLUMP				12

//	Lumps in ESP file
#define EFLUMP_LEAVES			0		//	Leaves
#define EFLUMP_PLANES			1		//	Planes
#define EFLUMP_VERTICES			2		//	Vertice
#define EFLUMP_INDICES			3		//	Indices
#define EFLUMP_LEAFSIDES		4		//	Leaf sides
#define EFLUMP_SIDES			5		//	Sides
#define EFLUMP_CLUSTERS			6		//	Clusters
#define EFLUMP_AREAS			7		//	Areas
#define EFLUMP_BLOCKS			8		//	Blocks
#define EFLUMP_CLUSTERREFS		9		//	Cluster references
#define EFLUMP_BRUSHES			10		//	Brushes
#define EFLUMP_BRUSHPLANES		11		//	Planes of brushes

//	Side flags
#define SIDEFLAG_TWOSIDES		0x00000001	//	Two sides
#define SIDEFLAG_SHOOTTHROUGH	0x00000002	//	Can shout through
#define SIDEFLAG_NOMARK			0x00000004	//	Don't left mark on it
#define SIDEFLAG_PICKTHROUGH	0x00000008	//	Can pick through
#define SIDEFLAG_ALPHA			0x00000010	//	Alpha flag

//	Side materials exclude each other
#define SIDEFLAG_METERIALMASK	0x1f000000	//	Material mask
#define SIDEFLAG_CONCRETE		0x01000000	//	Concrete side
#define SIDEFLAG_WOOD			0x02000000	//	Wood side
#define SIDEFLAG_METALSOLID		0x03000000	//	Metal solid
#define SIDEFLAG_METALBARREL	0x04000000	//	Metal barrel
#define SIDEFLAG_GLASS			0x05000000	//	Glass side
#define SIDEFLAG_RUBBER			0x06000000	//	Rubber side
#define SIDEFLAG_FLESH			0x07000000	//	Flesh

/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////

typedef struct _EFLUMP
{
	LONG		lOff;			//	Offset from file header to lump's data
	LONG		lSize;			//	Size of this lump

} EFLUMP, *PEFLUMP;

//	File header
typedef struct _ESPFILEHEADER
{
	DWORD		dwIdentify;		//	Identity, must be ESP_IDENTITY
	DWORD		dwVersion;		//	Version, must be ESP_VERSION

	EFLUMP		aLumps[NUM_ESPLUMP];	//	Lump's information

} ESPFILEHEADER, *PESPFILEHEADER;

typedef struct _EFCLUSTER
{
	FLOAT		vMins[3];		//	Cluster's bounding box
	FLOAT		vMaxs[3];
	FLOAT		vAlignMins[3];	//	Align bounding box
	FLOAT		vAlignMaxs[3];
	LONG		vLength[3];		//	Length of each border of leaf
	LONG		vSize[3];		//	Size of world in leaves
	LONG		lFirstSide;		//	First side in side array
	LONG		lNumSides;		//	Number of sides
	LONG		lFirstLeaf;		//	First leaf in leaf array
	LONG		lNumLeaves;		//	Number of leaves

} EFCLUSTER, *PEFCLUSTER;

typedef struct _EFPLANE
{
	FLOAT		vNormal[3];
	FLOAT		fDist;

} EFPLANE, *PEFPLANE;

typedef struct _EFSIDE
{
	LONG		lPlane;			//	Plane index
	LONG		lFirstVert;		//	First vertex in vertex lump
	LONG		lNumVert;		//	Number of vertex
	LONG		lFirstIdx;		//	First index in index lump
	LONG		lNumIdx;		//	Number of index
	DWORD		dwFlags;		//	dwFlags
	LONG		lBrush;			//	Brush index

} EFSIDE, *PEFSIDE;

typedef struct _EFLEAF
{
	LONG		lFirstLeafSide;	//	First leaf side
	LONG		lNumLeafSides;	//	Number of leaf sides

} EFLEAF, *PEFLEAF;

typedef struct _EFVERTEX
{
	FLOAT		vPos[3];		//	Position

} EFVERTEX, *PEFVERTEX;

typedef struct _EFBLOCK
{
	LONG		lFirstClusterRef;	//	First cluster reference
	LONG		lNumClusterRef;		//	Number of cluster reference

} EFBLOCK, *PEFBLOCK;

typedef struct _EFAREA
{
	FLOAT		vMins[2];		//	Area's bounding box
	FLOAT		vMaxs[2];
	FLOAT		vAlignMins[2];	//	Align bounding box
	FLOAT		vAlignMaxs[2];
	LONG		vLength[2];		//	Length of every block
	LONG		vSize[2];		//	Area's size in blocks

} EFAREA, *PEFAREA;

typedef struct _EFBRUSH
{
	LONG		lFirstPlane;	//	First brush plane's index
	LONG		lNumPlane;		//	Number of planes of this brush

} EFBRUSH, *PEFBRUSH; 

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////

void ESP_WriteLump(FILE* fp, PESPFILEHEADER pHeader, LONG lLump, void* pData, LONG lSize);
int ESP_ReadLump(FILE* fp, PESPFILEHEADER pHeader, LONG lLump, void** ppBuf, LONG lSize);


#endif	//	_ESPFILE_H_


