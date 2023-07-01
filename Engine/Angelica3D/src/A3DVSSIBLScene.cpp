/*
 * FILE: A3DVSSIBLScene.cpp
 *
 * DESCRIPTION: Render scenes using Visible Surface Set.
 *
 * CREATED BY: duyuxin, 2002/7/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTypes.h"
#include "A3DErrLog.h"
#include "A3DVSSCommon.h"
#include "A3DVSSIBLScene.h"

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
//	Implement of A3DVSSIBLScene
//
///////////////////////////////////////////////////////////////////////////

A3DVSSIBLScene::A3DVSSIBLScene() : A3DIBLScene()
{
	m_iNumRoute		= NULL;
	m_iCurRoute		= 0;
	m_bRenderAll	= false;
	m_bNeedUpdate	= false;
}

/*	Initialize object

	Return true for success, otherwise return false.

	pA3DDevice: valid A3D device object
*/
bool A3DVSSIBLScene::Init(A3DDevice* pA3DDevice)
{
	if (!A3DIBLScene::Init(pA3DDevice))
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::Init, Failed to call A3DIBLScene::Init.");
		return false;
	}

	memset(m_aRoutes, 0, sizeof (m_aRoutes));

	m_iNumRoute	= 0;

	return true;
}

/*	Release resources

	Return true for success, otherwise return false.
*/
bool A3DVSSIBLScene::Release()
{
	RemoveAllRoutes();

	A3DIBLScene::Release();

	return true;
}

/*	Set current route

	Return true for success, otherwise return false.

	iIndex: route's index returned by A3DVSSIBLScene::LoadRoute().
	iSample: current sample of specified route
*/
bool A3DVSSIBLScene::SetCurrentRoute(int iIndex, int iSample)
{
	if (iIndex < 0 || iIndex >= MAXNUM_ROUTE)
		return false;

	if (!m_aRoutes[iIndex].pRoute)
		return false;

	m_iCurSample	= iSample;
	m_iCurRoute		= iIndex;
	m_bNeedUpdate	= true;

	return true;
}

/*	Set current section

	Return true for success, otherwise return false.

	iSection: section's index
*/
bool A3DVSSIBLScene::SetCurrentSection(int iSection)
{
	ROUTE* pRoute = &m_aRoutes[m_iCurRoute];
	if (!pRoute || !pRoute->pRoute)
		return false;

	if (iSection < 0 || iSection >= pRoute->pRoute->GetSectionNumber())
		return false;

	m_iCurSample = pRoute->pRoute->GetFirstSampleOfSection(iSection);

	return true;
}

/*	Update visible surface set

	Return true for success, otherwise return false.
*/
bool A3DVSSIBLScene::UpdateVisibleSets(A3DViewport* pCurrentViewport)
{
	if (!m_nNumTextures)
		return true;

	int i;

	for (i=0; i < m_nNumTextures; i++)
		m_pTextureRecords[i].nFaceVisible = 0;

	if (m_bRenderAll)
	{
		for (i=0; i < m_nNumFaces; i++)
		{
			int idTex = m_pFaceRecords[i].nTexID;
			int nFaceVisible = m_pTextureRecords[idTex].nFaceVisible++;

			memcpy(m_pTextureRecords[idTex].pVerts + nFaceVisible * 3, m_pAllFaces + i * 3, sizeof (A3DIBLVERTEX) * 3);
		}
	}
	else
	{
		if (!m_iNumRoute)
			return true;

		if (m_iCurRoute < 0 || m_iCurRoute >= m_iNumRoute ||
			!m_aRoutes[m_iCurRoute].pRoute)
			return false;
		
		ROUTE* pRoute = &m_aRoutes[m_iCurRoute];
		
		int iSection = pRoute->pRoute->GetSectionOfSample(m_iCurSample);
		if (iSection != pRoute->iCurSection || m_bNeedUpdate)
		{
			pRoute->pRoute->GetSectionVisSurf(iSection, pRoute->aVisSurfs, &pRoute->iNumVisSurf);
			pRoute->iCurSection = iSection;

			if (pRoute->aVisObjs)
				pRoute->pRoute->GetSectionVisObjs(iSection, pRoute->aVisObjs, &pRoute->iNumVisObj);

			m_bNeedUpdate = false;
		}
		
		for (i=0; i < pRoute->iNumVisSurf; i++)
		{
			int iFace = pRoute->aVisSurfs[i];
			int idTex = m_pFaceRecords[iFace].nTexID;
			int nFaceVisible = m_pTextureRecords[idTex].nFaceVisible++;
			
			memcpy(m_pTextureRecords[idTex].pVerts + nFaceVisible * 3, m_pAllFaces + iFace * 3, sizeof (A3DIBLVERTEX) * 3);
		}
	}

	m_nNumVisibleFaces = 0;

	for (i=0; i < m_nNumTextures; i++)
	{
#ifdef A3DSCENE_USESTREAM
		if (m_pTextureRecords[i].nFaceVisible > 0)
			m_pTextureRecords[i].pStream->SetVerts((LPBYTE)m_pTextureRecords[i].pVerts, m_pTextureRecords[i].nFaceVisible * 3);
#endif

		m_nNumVisibleFaces += m_pTextureRecords[i].nFaceVisible;
	}

	return true;
}

/*	Allocate a route slot

	Return route slot's address for success, otherwise return NULL.
*/
A3DVSSIBLScene::ROUTE* A3DVSSIBLScene::AllocRouteSlot()
{
	int i;

	//	Search a empty slot
	for (i=0; i < MAXNUM_ROUTE; i++)
	{
		if (!m_aRoutes[i].pRoute)
			break;
	}

	if (i >= MAXNUM_ROUTE)
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::AllocRouteSlot, too many routes.");
		return NULL;
	}
	
	ROUTE* pRoute = &m_aRoutes[i];
	memset(pRoute, 0, sizeof (ROUTE));

	return pRoute;
}

/*	Load one route

	Return route's index for success, otherwise return -1.

	szFileName: route file's name
	bAll: true, load all route at once.
*/
int A3DVSSIBLScene::LoadRoute(char* szFileName, bool bAll)
{
	int iNumFace, iNumObj;

	if (!(iNumFace = GetFaceCount()))
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::LoadRoute, scene data hasn't been loaded.");
		return -1;
	}

	ROUTE* pRoute = AllocRouteSlot();
	if (!pRoute)
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::Init, failed to allocate route slot!");
		return -1;
	}

	//	Create route object
	if (!(pRoute->pRoute = new A3DVSSRouteLoader))
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::Init, failed to create route object!");
		return -1;
	}

	if (!(pRoute->pRoute->Load(szFileName, bAll)))
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::LoadRoute, failed to load route.");
		goto Error;
	}

	//	Create visible surface index buffer
	if (!(pRoute->aVisSurfs = new int [iNumFace]))
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::LoadRoute, not enough memory.");
		goto Error;
	}

	memset(pRoute->aVisSurfs, 0, sizeof (int) * iNumFace);

	//	Create visible object ID buffer
	if ((iNumObj = pRoute->pRoute->GetObjectNum()))
	{
		if (!(pRoute->aVisObjs = new int [iNumObj]))
		{
			g_pA3DErrLog->ErrLog("A3DVSSIBLScene::LoadRoute, not enough memory.");
			goto Error;
		}

		memset(pRoute->aVisObjs, 0, iNumObj * sizeof (int));
	}

	pRoute->dwID		= pRoute->pRoute->GetRouteID();
	pRoute->iCurSection	= -1;
	pRoute->iNumVisSurf	= 0;
	pRoute->iNumVisObj	= 0;

	if (++m_iNumRoute == 1)		//	Is the first route
		m_iCurRoute = pRoute - m_aRoutes;
	
	return pRoute - m_aRoutes;

Error:

	if (pRoute->pRoute)
	{
		pRoute->pRoute->Release();
		delete pRoute->pRoute;
	}

	if (pRoute->aVisSurfs)
		delete [] pRoute->aVisSurfs;

	return -1;
}

/*	Load one route from memory file

	Return route's index for success, otherwise return -1.

	pBuf: memory file's address
*/
int	A3DVSSIBLScene::LoadRoute(BYTE* pBuf)
{
	int iNumFace, iNumObj;

	if (!(iNumFace = GetFaceCount()))
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::LoadRoute, scene data hasn't been loaded.");
		return -1;
	}

	ROUTE* pRoute = AllocRouteSlot();
	if (!pRoute)
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::Init, failed to allocate route slot!");
		return -1;
	}

	//	Create route object
	if (!(pRoute->pRoute = new A3DVSSRouteLoader))
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::Init, failed to create route object!");
		return -1;
	}

	if (!(pRoute->pRoute->Load(pBuf)))
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::LoadRoute, failed to load route.");
		goto Error;
	}

	//	Create visible surface index buffer
	if (!(pRoute->aVisSurfs = new int [iNumFace]))
	{
		g_pA3DErrLog->ErrLog("A3DVSSIBLScene::LoadRoute,  not enough memory.");
		goto Error;
	}

	memset(pRoute->aVisSurfs, 0, sizeof (int) * iNumFace);

	//	Create visible object ID buffer
	if ((iNumObj = pRoute->pRoute->GetObjectNum()))
	{
		if (!(pRoute->aVisObjs = new int [iNumObj]))
		{
			g_pA3DErrLog->ErrLog("A3DVSSIBLScene::LoadRoute, not enough memory.");
			goto Error;
		}

		memset(pRoute->aVisObjs, 0, iNumObj * sizeof (int));
	}

	pRoute->dwID		= pRoute->pRoute->GetRouteID();
	pRoute->iCurSection	= -1;
	pRoute->iNumVisSurf	= 0;
	pRoute->iNumVisObj	= 0;

	if (++m_iNumRoute == 1)		//	Is the first route
		m_iCurRoute = pRoute - m_aRoutes;
	
	return pRoute - m_aRoutes;

Error:

	if (pRoute->pRoute)
	{
		pRoute->pRoute->Release();
		delete pRoute->pRoute;
	}

	if (pRoute->aVisSurfs)
		delete [] pRoute->aVisSurfs;

	return -1;
}

/*	Remove one route

	Return true for success, otherwise return false.

	iIndex: route's index returned by A3DVSSIBLScene::LoadRoute().
*/
bool A3DVSSIBLScene::RemoveRoute(int iIndex)
{
	if (iIndex < 0 || iIndex >= MAXNUM_ROUTE)
		return false;

	ROUTE* pRoute = &m_aRoutes[iIndex];
			
	if (pRoute->pRoute)
	{
		pRoute->pRoute->Release();
		delete pRoute->pRoute;
		pRoute->pRoute = NULL;
	}
	
	if (pRoute->aVisSurfs)
	{
		delete [] pRoute->aVisSurfs;
		pRoute->aVisSurfs = NULL;
	}

	if (pRoute->aVisObjs)
	{
		delete [] pRoute->aVisObjs;
		pRoute->aVisObjs = NULL;
	}

	m_iNumRoute--;

	return true;
}

//	Remove all routes
void A3DVSSIBLScene::RemoveAllRoutes()
{
	for (int i=0; i < MAXNUM_ROUTE; i++)
	{
		if (m_aRoutes[i].pRoute)
			RemoveRoute(i);
	}

	m_iNumRoute = 0;
}

/*	Get route loader object

	Return route loader object's address for success, otherwise return NULL.

	iIndex: route's index returned by A3DVSSIBLScene::LoadRoute().
*/
A3DVSSRouteLoader* A3DVSSIBLScene::GetRouteLoader(int iIndex)
{
	if (iIndex < 0 || iIndex >= MAXNUM_ROUTE)
		return NULL;

	return m_aRoutes[iIndex].pRoute;
}

/*	Get scene object number. Call this function after at least one route has
	been loaded.

	Return scene object's number.
*/
int	A3DVSSIBLScene::GetSceneObjectNum()
{
	if (!m_iNumRoute)
		return 0;

	return m_aRoutes[m_iCurRoute].pRoute->GetObjectNum();
}

/*	Get visible object's ID of specified section

	Return true for success, otherwise return false.

	pBuf (out): buffer used to receive visible object's ID, the buffer must be
				length enough, cannot be NULL. You can through
				A3DVSSRouteLoader::GetObjectNum() to get the maximum possible 
				number of ID will be put in this buffer.
	piNumVisObj (out): used to receive the number of object ID that can be seen 
					   in this section, also it's the number of valid ID in pBuf.
					   This parameter can be NULL
*/
bool A3DVSSIBLScene::GetCurrentVisObjs(int* pBuf, int* piNumVisObj)
{
	if (!m_aRoutes[m_iCurRoute].aVisObjs)
	{
		if (piNumVisObj)
			*piNumVisObj = 0;

		return false;
	}

	memcpy(pBuf, m_aRoutes[m_iCurRoute].aVisObjs, m_aRoutes[m_iCurRoute].iNumVisObj * sizeof (int));

	if (piNumVisObj)
		*piNumVisObj = m_aRoutes[m_iCurRoute].iNumVisObj;

	return true;
}



