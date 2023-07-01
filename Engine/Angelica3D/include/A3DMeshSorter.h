/*
 * FILE: A3DMeshSorter.h
 *
 * DESCRIPTION: Routines for sorting meshes
 *
 * CREATED BY: duyuxin, 2001/12/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DMeshSorterER_H_
#define _A3DMeshSorterER_H_

#include "A3DFuncs.h"
#include "A3DMesh.h"
#include "AList.h"
#include "A3DLight.h"

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
//	Class A3DMeshSorter
//
///////////////////////////////////////////////////////////////////////////

class A3DMeshSorter
{
public:		//	Types

	enum
	{
		SIZE_BUFGROUP	= 16,		//	Size of node buffer group
		SIZE_NODEBUF	= 1024		//	Size of each node buffer
	};

	typedef	struct _MESHNODE
	{
		A3DMesh*			pMesh;			//	Mesh's address
		int					iCurFrame;		//	Mesh's current frame
		A3DMATRIX4			matTrans;		//	Mesh's translate matrix
		float				fWeight;		//	Mesh's weight used for sorting
		A3DIBLLIGHTPARAM 	iblLightParam;	//	parameter describe current ibl light
		ALISTELEMENT*		pRecordElement; //	Record element in the AList, used when delete it;

	} MESHNODE, *PMESHNODE;

public:		//	Constructors and Destructors

	A3DMeshSorter();
	virtual ~A3DMeshSorter();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pDevice);	//	Initialize object
	void		Release();					//	Release objbect

	bool		InsertMesh(A3DMATRIX4& matTrans, A3DIBLLIGHTPARAM& iblLightParam, int iCurFrame, A3DMesh* pMesh);	//	Insert a mesh
	void		RemoveAllMeshes();			//	Remove all meshes from list

	void		SetCameraPos(A3DVECTOR3& vPos)	{	m_vCameraPos = vPos;		}
	void		SetSortOrder(bool bIncrease)	{	m_bIncrease = bIncrease;	}

	bool		Render(A3DViewport* pCurViewport);	//	Render sorted meshes

protected:	//	Attributes

	A3DDevice*	m_pDevice;			//	A3DDevice

	bool		m_bIncrease;		//	true, increase sorting
	AList		m_MeshList;			//	Mesh list
	A3DVECTOR3	m_vCameraPos;		//	Camera position

	MESHNODE*	m_aNodeBufs[SIZE_BUFGROUP];	//	Node buffer
	int			m_iNumNode;			//	Current used number of node in last group
	int			m_iCurGroup;		//	Current used group
	int			m_iNumGroup;		//	Number of buffer group

	ALISTELEMENT*	m_pCurNode;		//	Current mesh node

	// IBL Light sections;
	A3DLight	* m_pIBLStaticLight;//	a static light used for IBL scene
	A3DLight	* m_pIBLDynamicLight;// a dynamic light used for IBL scene
protected:	//	Operations

	bool		AppendNodeBuffer();	//	Append a group of node buffer

public:
	inline void SetIBLLight(A3DLight * pStaticLight, A3DLight * pDynamicLight) 
	{ m_pIBLStaticLight = pStaticLight; m_pIBLDynamicLight = pDynamicLight; }
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DMeshSorterER_H_


