/*
 * FILE: A3DMeshSorter.cpp
 *
 * DESCRIPTION: Routines for sorting meshes
 *
 * CREATED BY: duyuxin, 2001/12/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DMeshSorter.h"
#include "A3DErrLog.h"
#include "A3DConfig.h"
#include "A3DIBLScene.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


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

A3DMeshSorter::A3DMeshSorter()
{
	m_bIncrease	= false;
	m_pCurNode	= NULL;
	m_iNumGroup	= 0;
	m_iNumNode	= 0;
	m_pDevice	= NULL;

	m_pIBLStaticLight = NULL;
	m_pIBLDynamicLight = NULL;

	memset(m_aNodeBufs, 0, sizeof (m_aNodeBufs));
}

A3DMeshSorter::~A3DMeshSorter()
{
}

//	Initialize object
bool A3DMeshSorter::Init(A3DDevice* pDevice)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	if (!m_MeshList.Init())
	{
		g_pA3DErrLog->ErrLog("A3DMeshSorter::Init initialize mesh list failed");
		return false;
	}

	MESHNODE* pNodeBuf;

	//	Allocate 1024 item as the first node buffer group
	if (!(pNodeBuf = (MESHNODE*)malloc(SIZE_NODEBUF * sizeof (MESHNODE))))
	{
		g_pA3DErrLog->ErrLog("A3DMeshSorter::Init Not enough memory");
		return false;
	}

	m_aNodeBufs[0]	= pNodeBuf;
	m_iNumGroup		= 1;
	m_iCurGroup		= 0;
	m_iNumNode		= 0;
	m_vCameraPos	= A3DVECTOR3(0.0f);
	m_pDevice		= pDevice;

	return true;
}

//	Release objbect
void A3DMeshSorter::Release()
{
	if( !m_pDevice ) return;

	for (int i=0; i < m_iNumGroup; i++)
	{
		if (m_aNodeBufs[i])
		{
			free(m_aNodeBufs[i]);
			m_aNodeBufs[i] = NULL;
		}
	}

	m_iNumGroup	= 0;
	m_iCurGroup	= 0;
	m_iNumNode	= 0;
	m_pDevice	= NULL;

	m_MeshList.Release();
}

/*	Insert a mesh to list.

	Return true for success, otherwise return false

	matTrans: mesh's translate matrix
	iCurFrame: mesh's current frame
	pMesh: new mesh's address.
*/
bool A3DMeshSorter::InsertMesh(A3DMATRIX4& matTrans, A3DIBLLIGHTPARAM& iblLightParam, int iCurFrame, A3DMesh* pMesh)
{
	if( !m_pDevice ) return true;

	if (!pMesh)
	{
		g_pA3DErrLog->ErrLog("A3DMeshSorter::InsertMesh, NULL pointer is ignored");
		return false;
	}

	//	Current node buffer has been used up
	if (m_iNumNode >= SIZE_NODEBUF)
	{
		if (!AppendNodeBuffer())
			return false;
	}

	MESHNODE* pMeshNode  = &m_aNodeBufs[m_iCurGroup][m_iNumNode++];
	pMeshNode->pMesh	 = pMesh;
	pMeshNode->iCurFrame = iCurFrame;
	pMeshNode->matTrans	 = matTrans;
	pMeshNode->iblLightParam = iblLightParam;

	//	Calculate mesh's weight
	A3DOBB obb = pMesh->GetMeshAutoOBB(iCurFrame);
	A3DVECTOR3 vPos	= matTrans * obb.Center - m_vCameraPos;

	pMeshNode->fWeight = DotProduct(vPos, vPos);

	//	Insert to a proper position in list
	ALISTELEMENT* pListElem = m_MeshList.GetFirst();

	if (m_bIncrease)	//	Increasing order
	{
		while (pListElem != m_MeshList.GetTail())
		{
			if (pMeshNode->fWeight < ((MESHNODE*)pListElem->pData)->fWeight)
			{
				m_MeshList.Insert(pMeshNode, pListElem, &pMeshNode->pRecordElement);
				return true;
			}

			pListElem = pListElem->pNext;
		}
	}
	else	//	Decreasing order
	{
		while (pListElem != m_MeshList.GetTail())
		{
			if (pMeshNode->fWeight > ((MESHNODE*)pListElem->pData)->fWeight)
			{
				m_MeshList.Insert(pMeshNode, pListElem, &pMeshNode->pRecordElement);
				return true;
			}

			pListElem = pListElem->pNext;
		}
	}

	//	Add this element to tail
	m_MeshList.Insert(pMeshNode, m_MeshList.GetTail(), &pMeshNode->pRecordElement);

	return true;
}

//	Remove all meshes from list. Note: this function only reset mesh list but not 
//	mesh buffer. To release mesh buffers call Release().
void A3DMeshSorter::RemoveAllMeshes()
{
	if( !m_pDevice ) return;

	m_MeshList.Reset();

	m_iCurGroup	= 0;
	m_iNumNode	= 0;
}

/*	Append a node buffer group. 

	Return true for success, otherwise return false.
*/
bool A3DMeshSorter::AppendNodeBuffer()
{
	if (m_iCurGroup+1 < m_iNumGroup)
	{
		m_iCurGroup++;
		m_iNumNode = 0;
		return true;
	}

	if (m_iNumGroup >= SIZE_BUFGROUP)
	{
		g_pA3DErrLog->ErrLog("A3DMeshSorter::AppendNodeBuffer, too much mesh inserted");
		return false;
	}

	MESHNODE* pNodeBuf = (MESHNODE*)malloc(SIZE_NODEBUF * sizeof (MESHNODE));
	if (!pNodeBuf)
	{
		g_pA3DErrLog->ErrLog("A3DMeshSorter::AppendNodeBuffer, Not enough memory");
		return false;
	}

	m_iCurGroup++;
	m_iNumGroup++;
	m_iNumNode = 0;

	return true;
}

/*	Render sorted meshes.

	pCurViewport: current viewport's address
*/
bool A3DMeshSorter::Render(A3DViewport* pCurViewport)
{
	if( !m_pDevice ) return true;

	MESHNODE* pMeshNode;
	A3DMesh* pMesh;
	ALISTELEMENT* pListElem = m_MeshList.GetFirst();	

	while (pListElem != m_MeshList.GetTail())
	{
		pMeshNode	= (MESHNODE*)pListElem->pData;
		pMesh		= pMeshNode->pMesh;

		m_pDevice->SetWorldMatrix(pMeshNode->matTrans);
		pMesh->UpdateToFrame(pMeshNode->iCurFrame);

		// Update static and dynamic light information if we use IBL scene
		if( A3DIBLScene::GetGobalLightGrid() )
		{
			if( m_pIBLStaticLight )
			{
				A3DLIGHTPARAM param = m_pIBLStaticLight->GetLightparam();
				param.Diffuse = pMeshNode->iblLightParam.clLightDirect;
				param.Direction = pMeshNode->iblLightParam.vecLightDir;
				m_pIBLStaticLight->SetLightParam(&param);
				m_pDevice->SetAmbient(pMeshNode->iblLightParam.clLightAmbient);
				m_pDevice->SetTextureColorOP(0, A3DIBLScene::GetGobalLightGrid()->GetColorOP());
			}
			if( m_pIBLDynamicLight )
			{
				if( pMeshNode->iblLightParam.dynamicLightParam.Type != A3DLIGHT_FORCE_DWORD )
				{
					m_pIBLDynamicLight->SetLightParam(&(pMeshNode->iblLightParam.dynamicLightParam));
					m_pIBLDynamicLight->TurnOn();
				}
				else
					m_pIBLDynamicLight->TurnOff();
			}
		}

		if (!pMesh->Render(pCurViewport))
			return false;

		pListElem = pListElem->pNext;
	}

	if( A3DIBLScene::GetGobalLightGrid() )
	{
		if( m_pIBLStaticLight )
			m_pDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
		if( m_pIBLDynamicLight )
			m_pIBLDynamicLight->TurnOff();
	}

	return true;
}


