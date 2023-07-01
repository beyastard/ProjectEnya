/*
 * FILE: A3DMeshCollector.h
 *
 * DESCRIPTION: Used to manager meshes which will be rendered
 *
 * CREATED BY: duyuxin, 2001/12/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMESHCOLLECTOR_H_
#define _A3DMESHCOLLECTOR_H_

#include "A3DFuncs.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DMesh.h"
#include "A3DStream.h"
#include "AList.h"
#include "vector.h"

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
//	Class A3DMeshCollector
//
///////////////////////////////////////////////////////////////////////////

class A3DMeshCollector
{
public:		//	Types

	//	Some limits of A3DMeshCollector
	enum
	{
		MAXNUM_MESHINSLOT	= 1024,		//	Maximum number of mesh in a texture slot
		MAXNUM_RENDERVERT	= 4096*2,	//	Maximum number of vertex can be rendered together
		MAXNUM_RENDERIDX	= 8192*2,	//	Maximum number of index can be rendered together
	};

	//	Mesh information
	typedef struct _MESHINFO
	{
		A3DMesh*		pMesh;			//	Mesh address
		int				iCurFrame;		//	Current frame of this mesh
		A3DMATRIX4		matTrans;		//	Translate matrix

	} MESHINFO, *PMESHINFO;

	//	Texture slot
	typedef struct _TEXTURESLOT
	{
		A3DTexture*		pTexture;		//	Texture's ID, in fact it is a pointer
		A3DMaterial*	pMaterial;		//	Material pointer
		int				iMeshCnt;		//	Mesh counter
		ALISTELEMENT*	pRecordElement;	//	Used by AList

		abase::vector<MESHINFO*>	aMeshInfo;	//	Mesh information's address

		_TEXTURESLOT::_TEXTURESLOT() : aMeshInfo(20) {}

	} TEXTURESLOT, *PTEXTURESLOT;

public:		//	Constructors and Destructors

	A3DMeshCollector();
	virtual ~A3DMeshCollector();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pA3DDevice);	//	Initialize object
	void		Release();		//	Release resources
	bool		Reset();		//	Reset mesh collector

	DWORD		RegisterTexture(A3DTexture* pTexture, A3DMaterial* pMaterial);	//	Add texture information
	bool		PrepareMeshToRender(A3DMesh* pMesh, int iCurFrame, A3DMATRIX4 matTrans, 
									DWORD hMeshInfo);	//	Add mesh's information
	bool		Render(A3DViewport* pCurViewport);		//	Render
	
	void		RemoveUnrenderedMeshes();		//	Remove unrendered meshes

protected:	//	Attributes

	A3DDevice*	m_pA3DDevice;		//	A3D device object
	A3DVERTEX*  m_pVertexBuffer;	//	A3D vertex buffer
	WORD*		m_pIndexBuffer;		//	A3D index buffer
	AList		m_TextureList;		//	Texture list

	abase::vector<MESHINFO*>	m_aMeshPool;	//	Mesh information poor
	int			m_iPoolCnt;						//	Pool counter

protected:	//	Operations

	void		ReleaseAllTextures(bool bReset);	//	Release all texture
	void		ReleaseAllMeshInfo();				//	Release all mesh information
};



#endif	//	_A3DMESHCOLLECTOR_H_



