/*
 * FILE: A3DIBLScene.cpp
 *
 * DESCRIPTION: Class representing the whole image-based-lighting scene in A3D Engine
 *
 * CREATED BY: Hedi, 2002/12/12
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DIBLScene.h"
#include "A3DErrLog.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DTextureMan.h"
#include "A3DConfig.h"
#include "A3DLamp.h"

A3DIBLLightGrid * A3DIBLScene::m_pGlobalLightGrid = NULL;
A3DIBLScene::A3DIBLScene()
{
	m_pA3DDevice		= NULL;
	
	m_szFileName[0]		= '\0';

	m_nNumFaces			= 0;
	m_pAllFaces			= NULL;
	m_pSortedFaces		= NULL;

	m_nNumTextures		= 0;
	m_pTextureRecords	= NULL;
	m_pFaceRecords		= NULL;

	m_nNumVisibleFaces	= 0;

	m_pIBLLightGrid		= NULL;
}

A3DIBLScene::~A3DIBLScene()
{
}

bool A3DIBLScene::Init(A3DDevice * pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;
	m_IBLLightRange = A3DIBLLR_1X;

	m_SceneAABB.Mins = A3DVECTOR3(99999.9f);
	m_SceneAABB.Maxs = A3DVECTOR3(-99999.9f);
	CompleteAABB(&m_SceneAABB);

	m_pIBLLightGrid = NULL;
	m_pGlobalLightGrid = NULL;
	
	return true;
}

bool A3DIBLScene::Release()
{
	for(int i=0; i<m_nNumTextures; i++)
	{
		if( m_pTextureRecords[i].pVerts )
		{
			free(m_pTextureRecords[i].pVerts);
		}
		if( m_pTextureRecords[i].pTexture )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureRecords[i].pTexture);
		}
		if( m_pTextureRecords[i].pLightMap )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureRecords[i].pLightMap);
		}
	}
	m_nNumTextures = 0;

	if( m_pTextureRecords )
	{
		free(m_pTextureRecords);
		m_pTextureRecords = NULL;
	}

	if( m_pFaceRecords )
	{
		free(m_pFaceRecords);
		m_pFaceRecords = NULL;
	}

	if( m_pSortedFaces )
	{
		free(m_pSortedFaces);
		m_pSortedFaces = NULL;
	}

	if( m_pAllFaces )
	{
		free(m_pAllFaces);
		m_pAllFaces = NULL;
	}

	m_nNumFaces			= 0;
	m_nNumTextures		= 0;
	m_nNumVisibleFaces	= 0;

	if( m_pIBLLightGrid )
	{
		m_pIBLLightGrid->Release();
		delete m_pIBLLightGrid;
		m_pIBLLightGrid = NULL;
		m_pGlobalLightGrid = NULL;
	}
	return true;
}

bool A3DIBLScene::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	if( !Init(pA3DDevice) )
		return false;

	strcpy(m_szFileName, pFileToLoad->GetRelativeName());
	if( pFileToLoad->IsText() )
	{
		return false;
	}
	else
	{
		int			i;
		DWORD		dwReadLength;

		pFileToLoad->Read(&m_Option, sizeof(A3DIBLSCENE_OPTION), &dwReadLength);
		pFileToLoad->Read(&m_IBLLightRange, sizeof(A3DIBLLIGHTRANGE), &dwReadLength);
		pFileToLoad->Read(&m_nNumTextures, sizeof(int), &dwReadLength);

		m_pTextureRecords = (A3DIBLSCENE_TEXTURE_RECORD *) malloc(sizeof(A3DIBLSCENE_TEXTURE_RECORD) * m_nNumTextures);
		if( NULL == m_pTextureRecords )
		{
			g_pA3DErrLog->ErrLog("A3DIBLScene::Load(), Not enough memory!");
			return false;
		}

		pFileToLoad->Read(m_pTextureRecords, sizeof(A3DIBLSCENE_TEXTURE_RECORD) * m_nNumTextures, &dwReadLength);
		for(i=0; i<m_nNumTextures; i++)
		{
			//Alloc needed resources;
			m_pTextureRecords[i].pVerts = (A3DIBLVERTEX *) malloc(sizeof(A3DIBLVERTEX) * m_pTextureRecords[i].nFaceNum * 3);
			if( NULL == m_pTextureRecords[i].pVerts )
			{
				g_pA3DErrLog->ErrLog("A3DIBLScene::Load(), Not enough memory!");
				return false;
			}
			m_pTextureRecords[i].nFaceVisible = 0;

			/*
			char * pszExt;
			if( strstr(m_pTextureRecords[i].szTextureName, ".sdr") == NULL )
			{
				pszExt = strstr(m_pTextureRecords[i].szTextureName, ".");
				if( pszExt )
				{
					pszExt[1] = 'd';
					pszExt[2] = 'd';
					pszExt[3] = 's';
				}
			}*/
		}

		pFileToLoad->Read(&m_nNumFaces, sizeof(int), &dwReadLength);
		m_pAllFaces = (A3DIBLVERTEX *) malloc(sizeof(A3DIBLVERTEX) * m_nNumFaces * 3);
		if( NULL == m_pAllFaces )
		{
			g_pA3DErrLog->ErrLog("A3DIBLScene::Load(), Not enough memory!");
			return false;
		}

		m_pSortedFaces = (A3DIBLSCENE_SORTEDFACE *) malloc(sizeof(A3DIBLSCENE_SORTEDFACE) * m_nNumFaces);
		if( NULL == m_pSortedFaces )
		{
			g_pA3DErrLog->ErrLog("A3DIBLScene::Load(), Not enough memory!");
			return false;
		}

		m_pFaceRecords = (A3DIBLSCENE_FACE_RECORD *) malloc(sizeof(A3DIBLSCENE_FACE_RECORD) * m_nNumFaces);
		if( NULL == m_pFaceRecords )
		{
			g_pA3DErrLog->ErrLog("A3DIBLScene::Load(), Not enough memory!");
			return false;
		}
		
		pFileToLoad->Read(m_pAllFaces, sizeof(A3DIBLVERTEX) * m_nNumFaces * 3, &dwReadLength);
		pFileToLoad->Read(m_pFaceRecords, sizeof(A3DIBLSCENE_FACE_RECORD) * m_nNumFaces, &dwReadLength);

		for(i=0; i<m_nNumFaces; i++)
			m_pFaceRecords[i].nRefTicks = -1;

		char szTag[5];
		szTag[4] = '\0';
		pFileToLoad->Read(szTag, 4, &dwReadLength);
		if( dwReadLength == 4 && 0 == _stricmp(szTag, "GRID") )
		{
			// Now load the light grid;
			m_pIBLLightGrid = new A3DIBLLightGrid();
			m_pIBLLightGrid->Load(m_pA3DDevice, pFileToLoad);
		}
	}	 

	if( !LoadAllMaps() )
		return false;

	m_pGlobalLightGrid = m_pIBLLightGrid;
	return true;
}

bool A3DIBLScene::Save(AFile * pFileToSave)
{
	if( pFileToSave->IsText() )
	{
		return false;
	}
	else
	{
		DWORD		dwWriteLength;
		
		pFileToSave->Write(&m_Option, sizeof(A3DIBLSCENE_OPTION), &dwWriteLength);
		pFileToSave->Write(&m_IBLLightRange, sizeof(A3DIBLLIGHTRANGE), &dwWriteLength);
		pFileToSave->Write(&m_nNumTextures, sizeof(int), &dwWriteLength);
		pFileToSave->Write(m_pTextureRecords, sizeof(A3DIBLSCENE_TEXTURE_RECORD) * m_nNumTextures, &dwWriteLength);

		pFileToSave->Write(&m_nNumFaces, sizeof(int), &dwWriteLength);
		pFileToSave->Write(m_pAllFaces, sizeof(A3DIBLVERTEX) * m_nNumFaces * 3, &dwWriteLength);
		pFileToSave->Write(m_pFaceRecords, sizeof(A3DIBLSCENE_FACE_RECORD) * m_nNumFaces, &dwWriteLength);

		// Now save the light grid;
		if( m_pIBLLightGrid )
		{
			pFileToSave->Write("GRID", 4, &dwWriteLength);
			m_pIBLLightGrid->Save(pFileToSave);
		}
		return true;
	}
	return true;
}

bool A3DIBLScene::SetLightGrid(A3DIBLLightGrid * pLightGrid )
{
	if( m_pIBLLightGrid )
	{
		m_pIBLLightGrid->Release();
		delete m_pIBLLightGrid;
		m_pIBLLightGrid = NULL;
		m_pGlobalLightGrid = NULL;
	}

	m_pIBLLightGrid = pLightGrid;
	m_pGlobalLightGrid = m_pIBLLightGrid;
	return true;
}

bool A3DIBLScene::AddTriFace(char * szTextureName, char * szLightMapName, bool b2Sided, A3DIBLVERTEX * v0, A3DIBLVERTEX * v1, A3DIBLVERTEX * v2)
{
	A3DMESH_PROP	prop;
	ZeroMemory(&prop, sizeof(prop));

	A3DIBLSCENE_TEXTURE_RECORD * pTextureRecord = FindTextureRecord(szTextureName, szLightMapName, b2Sided, prop);
	if( NULL == pTextureRecord )
	{
		// Create a new texture record;
		m_nNumTextures ++;
		m_pTextureRecords = (A3DIBLSCENE_TEXTURE_RECORD *) realloc(m_pTextureRecords, m_nNumTextures * sizeof(A3DIBLSCENE_TEXTURE_RECORD));
		if( NULL == m_pTextureRecords )
		{
			g_pA3DErrLog->ErrLog("A3DIBLScene::AddTriFace(), Not enough memory!");
			return false;
		}

		pTextureRecord = &m_pTextureRecords[m_nNumTextures - 1];
		ZeroMemory(pTextureRecord, sizeof(A3DIBLSCENE_TEXTURE_RECORD));
		
		// record this texture's base map and light map;
		strncpy(pTextureRecord->szTextureName, szTextureName, MAX_PATH);
		strncpy(pTextureRecord->szLightMapName, szLightMapName, MAX_PATH);
		
		pTextureRecord->b2Sided	 = b2Sided;
		pTextureRecord->prop	 = prop;
		pTextureRecord->nFaceNum = 0;
		pTextureRecord->nFaceVisible = 0;
		pTextureRecord->pVerts  = NULL;
		pTextureRecord->nTexID = m_nNumTextures - 1;
	}

	pTextureRecord->nFaceNum ++;
	pTextureRecord->pVerts = (A3DIBLVERTEX *) realloc(pTextureRecord->pVerts, pTextureRecord->nFaceNum * 3 * sizeof(A3DIBLVERTEX));
	if( NULL == pTextureRecord->pVerts )
	{
		g_pA3DErrLog->ErrLog("A3DIBLScene::AddTriFace(), Not enough memory!");
		return false;
	}

	m_nNumFaces ++;
	m_pAllFaces = (A3DIBLVERTEX *) realloc(m_pAllFaces, m_nNumFaces * 3 * sizeof(A3DIBLVERTEX));
	if( NULL == m_pAllFaces )
	{
		g_pA3DErrLog->ErrLog("A3DIBLScene::AddTriFace(), Not enough memory!");
		return false;
	}
	m_pSortedFaces = (A3DIBLSCENE_SORTEDFACE *) realloc(m_pSortedFaces, m_nNumFaces * sizeof(A3DIBLSCENE_SORTEDFACE));
	if( NULL == m_pSortedFaces )
	{
		g_pA3DErrLog->ErrLog("A3DIBLScene::AddTriFace(), Not enough memory!");
		return false;
	}
	m_pFaceRecords = (A3DIBLSCENE_FACE_RECORD *) realloc(m_pFaceRecords, m_nNumFaces * sizeof(A3DIBLSCENE_FACE_RECORD));
	if( NULL == m_pFaceRecords )
	{
		g_pA3DErrLog->ErrLog("A3DIBLScene::AddTriFace(), Not enough memory!");
		return false;
	}

	int idFace = m_nNumFaces - 1;
	m_pAllFaces[idFace * 3    ] = *v0;
	m_pAllFaces[idFace * 3 + 1] = *v1;
	m_pAllFaces[idFace * 3 + 2] = *v2;

	m_pFaceRecords[idFace].nTexID = pTextureRecord->nTexID;
	m_pFaceRecords[idFace].nRefTicks = -1;
	
	return true;
}

/*
	This function must be called after calling AddTriFace into this IBL scene to
	let all texture loaded, for when you call AddTriFace, none texture has been loaded
*/
bool A3DIBLScene::LoadAllMaps()
{
	if( !m_pA3DDevice ) // If is used for preprocessing, m_pA3DDevice will be NULL;
	{
		// Update texture information
		for(int i=0; i<m_nNumTextures; i++)
		{
			bool bAlpha = false;
			char szTextureName[MAX_PATH];
			strcpy(szTextureName, m_pTextureRecords[i].szTextureName);
			_strlwr(szTextureName);
			if( strstr(szTextureName, ".tga") || strstr(szTextureName, ".sdr") )
				bAlpha = true;
			
			// Now determine the texture's property
			if( bAlpha )
				m_pTextureRecords[i].dwRenderFlag = A3DSCENE_RENDER_ALPHA;
			else
				m_pTextureRecords[i].dwRenderFlag = A3DSCENE_RENDER_SOLID;
		}
		return true;
	}

	char szSceneTextureFolder[MAX_PATH];
	AFileMod_GetFilePath(m_szFileName, szSceneTextureFolder, MAX_PATH);

	if( szSceneTextureFolder[0] )
		strcat(szSceneTextureFolder, "\\Textures");
	else
		strcpy(szSceneTextureFolder, "Textures");

	for(int i=0; i<m_nNumTextures; i++)
	{
		if( strlen(m_pTextureRecords[i].szTextureName) )
		{
			if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_pTextureRecords[i].szTextureName, szSceneTextureFolder, &m_pTextureRecords[i].pTexture) )
			{
				g_pA3DErrLog->ErrLog("A3DIBLScene::AddMesh(), Can't load texture [%s]!", m_pTextureRecords[i].szTextureName);
			}
		}
		if( strlen(m_pTextureRecords[i].szLightMapName) && g_pA3DConfig->GetFlagLightMapScene() )
		{
			if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_pTextureRecords[i].szLightMapName, szSceneTextureFolder, &m_pTextureRecords[i].pLightMap, A3DTF_LIGHTMAP) )
			{
				g_pA3DErrLog->ErrLog("A3DIBLScene::AddTriFace(), Can't load texture [%s]!", m_pTextureRecords[i].szLightMapName);
			}
		}

		// Now determine the texture's property
		if( m_pTextureRecords[i].pTexture && m_pTextureRecords[i].pTexture->IsAlphaTexture() )
			m_pTextureRecords[i].dwRenderFlag = A3DSCENE_RENDER_ALPHA;
		else
			m_pTextureRecords[i].dwRenderFlag = A3DSCENE_RENDER_SOLID;
	}

	if( !PrepareFirstRender() )
		return false;

	return true;
}

A3DIBLSCENE_TEXTURE_RECORD * A3DIBLScene::FindTextureRecord(char * szTextureName, char * szLightMapName, bool b2Sided, A3DMESH_PROP prop)
{
	for(int i=0; i<m_nNumTextures; i++)
	{
		if( _stricmp(szTextureName, m_pTextureRecords[i].szTextureName) )
			continue;

		if( _stricmp(szLightMapName, m_pTextureRecords[i].szLightMapName) )
			continue;

		if( m_pTextureRecords[i].b2Sided ^ b2Sided )
			continue;

		return &m_pTextureRecords[i];
	}

	return NULL;
}

int IBLFaceSortCompare(const void *arg1, const void *arg2)
{
	return ((A3DIBLScene::A3DIBLSCENE_SORTEDFACE *)arg1)->vDisToCam - ((A3DIBLScene::A3DIBLSCENE_SORTEDFACE *)arg2)->vDisToCam > 0.0f ? 1 : -1;
}

// Render current visible sets sorting by distance to camera;
bool A3DIBLScene::RenderSort(A3DViewport * pCurrentViewport, DWORD dwFlag, bool bNear2Far)
{
	if( dwFlag & A3DSCENE_RENDER_ALPHA )
		RenderLamps(pCurrentViewport);

	// Because we have used alpha compare function so no need to sort alpha faces.
	return true;

	A3DVECTOR3		vecCamPos = pCurrentViewport->GetCamera()->GetPos();

	// First construct sorting buffer;
	int				i, n;
	int				nSortedFaceNum = 0;

	for(i=0; i<m_nNumTextures; i++)
	{
		if( !(dwFlag & m_pTextureRecords[i].dwRenderFlag) )
			continue;
	
		A3DIBLVERTEX *		pVerts;
		pVerts = m_pTextureRecords[i].pVerts;
		for(n=0; n<m_pTextureRecords[i].nFaceVisible; n++)
		{
			A3DVECTOR3 vecCenter;
			vecCenter.x = (pVerts[n * 3].x + pVerts[n * 3 + 1].x + pVerts[n * 3 + 2].x) / 3.0f;
			vecCenter.y = (pVerts[n * 3].y + pVerts[n * 3 + 1].y + pVerts[n * 3 + 2].y) / 3.0f;
			vecCenter.z = (pVerts[n * 3].z + pVerts[n * 3 + 1].z + pVerts[n * 3 + 2].z) / 3.0f;
			m_pSortedFaces[nSortedFaceNum].vDisToCam = Magnitude(vecCamPos - vecCenter) * (bNear2Far ? 1.0f : -1.0f);
			m_pSortedFaces[nSortedFaceNum].pTexRecord = &m_pTextureRecords[i];
			m_pSortedFaces[nSortedFaceNum].wIndexInTexVisible = n;

			nSortedFaceNum ++;
		}
	}

	qsort(m_pSortedFaces, nSortedFaceNum, sizeof(A3DIBLSCENE_SORTEDFACE), IBLFaceSortCompare);

	// Now render the sorted faces;
	SetDeviceState();

#define MAX_RENDER_FACE			256
	A3DIBLVERTEX				pRenderVerts[MAX_RENDER_FACE * 3];
	int							nRenderFaceNum = 0;
	A3DIBLSCENE_TEXTURE_RECORD *pLastTex = NULL;

	for(i=0; i<nSortedFaceNum; i++)
	{
		A3DIBLSCENE_TEXTURE_RECORD *	pThisTex = m_pSortedFaces[i].pTexRecord;

		if( pThisTex != pLastTex )
		{
			if( pLastTex )
			{
				// Flush the render buffer;
				m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DIBLVERTEX);
				m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, nRenderFaceNum, pRenderVerts, sizeof(A3DIBLVERTEX));
				nRenderFaceNum = 0;

				if( pLastTex->pTexture )
				{
					pLastTex->pTexture->Disappear(0);
					if( pLastTex->pTexture->IsShaderTexture() )
						SetDeviceState();
				}
				if( pLastTex->pLightMap && g_pA3DConfig->GetFlagLightMapScene() )
					pLastTex->pLightMap->Disappear(1);
			}

			if( pThisTex->pLightMap && g_pA3DConfig->GetFlagLightMapScene() )
				pThisTex->pLightMap->Appear(1);
			if( pThisTex->pTexture )
				pThisTex->pTexture->Appear(0);

			if( pThisTex->b2Sided )
				m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
			else
				m_pA3DDevice->SetFaceCull(A3DCULL_CCW);

			pLastTex = pThisTex;
		}

		pRenderVerts[nRenderFaceNum * 3    ] = pThisTex->pVerts[m_pSortedFaces[i].wIndexInTexVisible * 3    ];
		pRenderVerts[nRenderFaceNum * 3 + 1] = pThisTex->pVerts[m_pSortedFaces[i].wIndexInTexVisible * 3 + 1];
		pRenderVerts[nRenderFaceNum * 3 + 2] = pThisTex->pVerts[m_pSortedFaces[i].wIndexInTexVisible * 3 + 2];
		nRenderFaceNum ++;

		if( nRenderFaceNum >= MAX_RENDER_FACE )
		{
			m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DIBLVERTEX);
			m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, nRenderFaceNum, pRenderVerts, sizeof(A3DIBLVERTEX));
			nRenderFaceNum = 0;
		}
	}

	if( nRenderFaceNum > 0 )
	{
		m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, nRenderFaceNum, pRenderVerts, sizeof(A3DIBLVERTEX));
	}

	if( pLastTex )
	{
		if( pLastTex->pTexture )
			pLastTex->pTexture->Disappear(0);
		if( pLastTex->pLightMap && g_pA3DConfig->GetFlagLightMapScene() )
			pLastTex->pLightMap->Disappear(1);
	}

	RestoreDeviceState();

	if( dwFlag & A3DSCENE_RENDER_ALPHA )
		RenderLamps(pCurrentViewport);

	return true;
}

bool A3DIBLScene::SetDeviceState()
{
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);

	A3DTEXTUREOP textureOP;
	switch(m_IBLLightRange)
	{
	case A3DIBLLR_1X:
		textureOP = A3DTOP_MODULATE;
		break;
	case A3DIBLLR_2X:
		textureOP = A3DTOP_MODULATE2X;
		break;
	case A3DIBLLR_4X:
		textureOP = A3DTOP_MODULATE4X;
		break;
	default:
		textureOP = A3DTOP_MODULATE;
		break;
	}

	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);

	m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureColorOP(1, textureOP);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_MODULATE);

	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DIBLVERTEX);

	return true;
}

bool A3DIBLScene::RestoreDeviceState()
{
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);

	m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);

	m_pA3DDevice->SetLightingEnable(true);
	return true;
}

bool A3DIBLScene::Render(A3DViewport * pCurrentViewport, DWORD dwFlag)
{
	if( dwFlag & A3DSCENE_RENDER_SOLID )
	{
		SetDeviceState();

		for(int i=0; i<m_nNumTextures; i++)
		{
			//if( !(dwFlag & m_pTextureRecords[i].dwRenderFlag) )
			//	continue;
			if( m_pTextureRecords[i].pTexture && (m_pTextureRecords[i].pTexture->IsAlphaTexture() && !m_pTextureRecords[i].pTexture->IsShaderTexture()) )
			{
				m_pA3DDevice->SetAlphaTestEnable(true);
				m_pA3DDevice->SetAlphaRef(0x80);
				m_pA3DDevice->SetAlphaFunction(A3DCMP_GREATER);
			}

			if( m_pTextureRecords[i].nFaceVisible > 0 )
			{
				if( m_pTextureRecords[i].pLightMap && g_pA3DConfig->GetFlagLightMapScene() )
					m_pTextureRecords[i].pLightMap->Appear(1);
				if( m_pTextureRecords[i].pTexture )
					m_pTextureRecords[i].pTexture->Appear(0);

				if( m_pTextureRecords[i].b2Sided )
					m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
				else
					m_pA3DDevice->SetFaceCull(A3DCULL_CCW);

				m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, m_pTextureRecords[i].nFaceVisible, m_pTextureRecords[i].pVerts, sizeof(A3DIBLVERTEX));

				if( m_pTextureRecords[i].pTexture )
				{
					m_pTextureRecords[i].pTexture->Disappear(0);
					if( m_pTextureRecords[i].pTexture->IsShaderTexture() )
						SetDeviceState();
				}
				if( m_pTextureRecords[i].pLightMap && g_pA3DConfig->GetFlagLightMapScene() )
					m_pTextureRecords[i].pLightMap->Disappear(1);
			}

			m_pA3DDevice->SetAlphaTestEnable(false);
		}

		RestoreDeviceState();
	}

	if( dwFlag & A3DSCENE_RENDER_ALPHA )
		RenderLamps(pCurrentViewport);
	return true;
}

bool A3DIBLScene::RenderLamps(A3DViewport * pCurrentViewport)
{
	if( m_pIBLLightGrid )
	{
		m_pIBLLightGrid->RenderLamps(pCurrentViewport);
	}
	return true;
}

bool A3DIBLScene::PrepareFirstRender()
{
	int i;

	if( g_pA3DConfig->GetFlagLightMapScene() )
	{
		// Now we should turn off all vertex colors, because we just use light maps to shade the scene;
		for(i=0; i<m_nNumFaces * 3; i++)
		{
			m_pAllFaces[i].diffuse = A3DCOLORRGB(255, 255, 255);
		}
	}

	for(i=0; i<m_nNumTextures; i++)
		m_pTextureRecords[i].nFaceVisible = 0;

	for(i=0; i<m_nNumFaces; i++)
	{
		int idTex = m_pFaceRecords[i].nTexID;
		int nFaceVisible = m_pTextureRecords[idTex].nFaceVisible ++;

		memcpy(m_pTextureRecords[idTex].pVerts + nFaceVisible * 3, m_pAllFaces + i * 3, 3 * sizeof(A3DIBLVERTEX));
	}

	m_nNumVisibleFaces = m_nNumFaces;
	
	A3DTEXTUREOP textureOP;
	switch(m_IBLLightRange)
	{
	case A3DIBLLR_1X:
		textureOP = A3DTOP_MODULATE;
		break;
	case A3DIBLLR_2X:
		textureOP = A3DTOP_MODULATE2X;
		break;
	case A3DIBLLR_4X:
		textureOP = A3DTOP_MODULATE4X;
		break;
	default:
		textureOP = A3DTOP_MODULATE;
		break;
	}

	if( m_pIBLLightGrid )
	{
		m_pIBLLightGrid->SetColorOP(textureOP);
		m_pIBLLightGrid->PrepareFirstRender();
	}

	return true;
}

// This function should be called only once when all verts are rendered without 
// pre-visible detections.
bool A3DIBLScene::UpdateVisibleSets(A3DViewport * pCurrentViewport)
{
	return true;
}

bool A3DIBLScene::GetAllFaceVerts(A3DVECTOR3 * pVertsBuffer, int * pnBufferSize)
{
	if( !pVertsBuffer || *pnBufferSize < int(m_nNumFaces * 3 * sizeof(A3DVECTOR3)))
	{
		*pnBufferSize = m_nNumFaces * 3 * sizeof(A3DVECTOR3);
		return false;
	}

	for(int i=0; i<m_nNumFaces; i++)
	{
		pVertsBuffer[i * 3 + 0].x = m_pAllFaces[i * 3 + 0].x;
		pVertsBuffer[i * 3 + 0].y = m_pAllFaces[i * 3 + 0].y;
		pVertsBuffer[i * 3 + 0].z = m_pAllFaces[i * 3 + 0].z;

		pVertsBuffer[i * 3 + 1].x = m_pAllFaces[i * 3 + 1].x;
		pVertsBuffer[i * 3 + 1].y = m_pAllFaces[i * 3 + 1].y;
		pVertsBuffer[i * 3 + 1].z = m_pAllFaces[i * 3 + 1].z;

		pVertsBuffer[i * 3 + 2].x = m_pAllFaces[i * 3 + 2].x;
		pVertsBuffer[i * 3 + 2].y = m_pAllFaces[i * 3 + 2].y;
		pVertsBuffer[i * 3 + 2].z = m_pAllFaces[i * 3 + 2].z;
	}

	return true;
}

bool A3DIBLScene::Import(char * szMoxFileName)
{
	AFileImage fileImage;
	if( !fileImage.Open(szMoxFileName, AFILE_OPENEXIST) )
		return false;

	if( !Load(m_pA3DDevice, &fileImage) )
	{
		g_pA3DErrLog->ErrLog("A3DIBLScene::Import(), Call Load fail!");
		fileImage.Close();
		return false;
	}

	fileImage.Close();
	return true;
}


/**********************************************************/
/*				A3DIBLSceneLightGrd section		          */
/**********************************************************/

A3DIBLLightGrid::A3DIBLLightGrid()
{
	m_pA3DDevice		= NULL;

	m_nLightSourceCount = 0;
	m_pLightSources		= NULL;

	m_nLampCount		= 0;
	m_pLampEffects		= NULL;

	m_pLightSamples		= NULL;

	m_nLength			= 0;
	m_nWidth			= 0;
	m_nHeight			= 0;

	m_nXDim				= 0;
	m_nZDim				= 0;
	m_nYDim				= 0;

	m_vXDF				= 1.0f;
	m_vZDF				= 1.0f;
	m_vYDF				= 1.0f;

	m_vDirBrightness	= 1.0f;
	m_vAmbientBrightness= 1.0f;

	m_vecMin = A3DVECTOR3(-1.0f);
	m_vecMax = A3DVECTOR3(1.0f);
	
	m_bOptimized		= false;
	m_colorOP			= A3DTOP_MODULATE;

	// Dynamic Lights Section
	for(int i=0; i<A3DLIGHTGRID_MAX_DLIGHT; i++)
		m_aDLights[i] = new A3DLightGridDLight();

	m_nDLightCount		= 0;
	m_nNextDLightIndex	= 0;
}

A3DIBLLightGrid::~A3DIBLLightGrid()
{
}

bool A3DIBLLightGrid::Init(A3DDevice * pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;
	return true;
}

bool A3DIBLLightGrid::Release()
{
	int		i;

	for(i=0; i<A3DLIGHTGRID_MAX_DLIGHT; i++)
		delete m_aDLights[i];

	if( m_pLampEffects )
	{
		for(i=0; i<m_nLampCount; i++)
		{
			if( m_pLampEffects[i] )
			{
				m_pLampEffects[i]->Release();
				delete m_pLampEffects[i];
				m_pLampEffects[i] = NULL;
			}
		}
		free(m_pLampEffects);
	}

	if( m_pLightSources )
	{
		free(m_pLightSources);
		m_pLightSources = NULL;
	}

	if( m_pLightSamples )
	{
		free(m_pLightSamples);
		m_pLightSamples = NULL;
	}

	return true;
}

bool A3DIBLLightGrid::Optimize()
{
	return true;
}

bool A3DIBLLightGrid::AddLightSource(const A3DIBLLIGHTSOURCE& source)
{
	m_nLightSourceCount ++;

	m_pLightSources = (A3DIBLLIGHTSOURCE *) realloc(m_pLightSources, m_nLightSourceCount * sizeof(A3DIBLLIGHTSOURCE));
	if( NULL == m_pLightSources )
		return false;

	m_pLampEffects = (A3DLamp **) realloc(m_pLampEffects, m_nLightSourceCount * sizeof(A3DLamp *));
	if( NULL == m_pLampEffects )
		return false;

	int nLightIndex = m_nLightSourceCount - 1;
	m_pLightSources[nLightIndex] = source;
	return true;
}

bool A3DIBLLightGrid::GetLightSource(int nIndex, A3DIBLLIGHTSOURCE * pSource)
{
	if( nIndex >= m_nLightSourceCount )
		return false;

	*pSource = m_pLightSources[nIndex];
	return true;
}

bool A3DIBLLightGrid::SetDimension(int nLength, int nWidth, int nHeight, const A3DVECTOR3& vecMin, const A3DVECTOR3& vecMax)
{
	if( m_pLightSamples )
	{
		free(m_pLightSamples);
		m_pLightSamples = NULL;
	}

	m_nLength = nLength;
	m_nWidth  = nWidth;
	m_nHeight = nHeight;

	m_nXDim = m_nLength + 1;
	m_nZDim = m_nWidth + 1;
	m_nYDim = m_nHeight + 1;

	m_vecMin = vecMin;
	m_vecMax = vecMax;
	
	m_vXDF = m_nLength / (m_vecMax.x - m_vecMin.x);
	m_vZDF = m_nWidth  / (m_vecMax.z - m_vecMin.z);
	m_vYDF = m_nHeight / (m_vecMax.y - m_vecMin.y);

	m_pLightSamples = (A3DIBLLIGHTSAMPLE *) malloc(m_nXDim * m_nZDim * m_nYDim * sizeof(A3DIBLLIGHTSAMPLE));
	if( NULL == m_pLightSamples )
	{
		g_pA3DErrLog->ErrLog("A3DIBLLightGrid::SetGridDimension(), Not enough memory!");
		return false;
	}

	ZeroMemory(m_pLightSamples, m_nXDim * m_nZDim * m_nYDim * sizeof(A3DIBLLIGHTSAMPLE));
	return true;
}

bool A3DIBLLightGrid::GetDimension(int * pnLength, int * pnWidth, int * pnHeight, A3DVECTOR3 * pvecMin, A3DVECTOR3 * pvecMax)
{
	*pnLength = m_nLength;
	*pnWidth  = m_nWidth;
	*pnHeight = m_nHeight;

	*pvecMin = m_vecMin;
	*pvecMax = m_vecMax;
	return true;
}

bool A3DIBLLightGrid::GetEquivalentLightInfo(const A3DVECTOR3& vecPoint, A3DVECTOR3 * pvecLightDir, A3DCOLORVALUE * pclLightDirect, A3DCOLOR * pclAmbient, A3DLIGHTPARAM * pDLightParam)
{
	float vX = (vecPoint.x - m_vecMin.x) * m_vXDF;
	float vZ = (vecPoint.z - m_vecMin.z) * m_vZDF;
	float vY = (vecPoint.y - m_vecMin.y) * m_vYDF;

	// shrink a little to make sure nX >= 0 && nX < m_nLength and so with nZ, nY
	int nX = int(vX);
	int nZ = int(vZ);
	int nY = int(vY);

	float fx[2], fz[2], fy[2], f;
	fx[0] = 1.0f + nX - vX;
	fx[1] = 1.0f - fx[0];

	fz[0] = 1.0f + nZ - vZ;
	fz[1] = 1.0f - fz[0];

	fy[0] = 1.0f + nY - vY;
	fy[1] = 1.0f - fy[0];

	// make index within valid range;
	if( nX < 0 ) nX = 0;
	if( nX >= m_nLength ) nX = m_nLength - 1;

	if( nZ < 0 ) nZ = 0;
	if( nZ >= m_nWidth ) nZ = m_nWidth - 1;

	if( nY < 0 ) nY = 0;
	if( nY >= m_nHeight ) nY = m_nHeight - 1;

	A3DVECTOR3		vecLightDir = A3DVECTOR3(0.0f);
	A3DCOLORVALUE 	clLightDirect;
	A3DCOLORVALUE 	clLightAmbient;
	clLightDirect.r = 0.0f;
	clLightDirect.g = 0.0f;
	clLightDirect.b = 0.0f;
	clLightDirect.a = 1.0f;
	clLightAmbient.r = 0.0f;
	clLightAmbient.g = 0.0f;
	clLightAmbient.b = 0.0f;
	clLightAmbient.a = 1.0f;

	int nIndex = 0;
	A3DIBLLIGHTSAMPLE sample;

	float vSumBright = 0.0f;
	for(int i=0; i<8; i++)
	{
		int sx, sz, sy;
		sx = i % 2;
		sz = (i / 2) % 2;
		sy = i / 4;
		f = fx[sx] * fz[sz] * fy[sy]; 
		
		nIndex = (nY + sy) * m_nXDim * m_nZDim + (nZ + sz) * m_nXDim + nX + sx;
		sample = m_pLightSamples[nIndex];
		A3DCOLORVALUE clDirect = ColorRGBAToColorValue(sample.colorDirect);
		A3DCOLORVALUE clAmbient = ColorRGBAToColorValue(sample.colorAmbient);

		clLightDirect.r += clDirect.r * f;
		clLightDirect.g += clDirect.g * f;
		clLightDirect.b += clDirect.b * f;
		
		clLightAmbient.r += clAmbient.r * f;
		clLightAmbient.g += clAmbient.g * f;
		clLightAmbient.b += clAmbient.b * f;
		
		f *= clDirect.a + 1e-4f;
		if( m_pLightSources[sample.wLightIndex].type == A3DIBLLIGHTTYPE_DIRECT )
			vecLightDir = vecLightDir + m_pLightSources[sample.wLightIndex].vecDir * f;
		else
			vecLightDir = vecLightDir + Normalize(vecPoint - m_pLightSources[sample.wLightIndex].vecPos) * f;
	}

	clLightDirect.r *= m_vDirBrightness;
	clLightDirect.g *= m_vDirBrightness;
	clLightDirect.b *= m_vDirBrightness;

	clLightAmbient.r *= m_vAmbientBrightness;
	clLightAmbient.g *= m_vAmbientBrightness;
	clLightAmbient.b *= m_vAmbientBrightness;

	* pvecLightDir = Normalize(vecLightDir);
	* pclLightDirect = clLightDirect;
	* pclAmbient = ColorValueToColorRGBA(clLightAmbient);

	// Get Dynamic light if need;
	if( pDLightParam  )
	{
		// We should retrieve the Dynamic light parameter for it;
		ZeroMemory(pDLightParam, sizeof(A3DLIGHTPARAM));
		pDLightParam->Type = A3DLIGHT_FORCE_DWORD; // nothing

		if( g_pA3DConfig->GetFlagDynamicLight() )
		{
			// Retrieve neareset sample point
			if( fx[1] > 0.5f ) nX ++;
			if( fz[1] > 0.5f ) nZ ++;
			if( fy[1] > 0.5f ) nY ++;
			nIndex = nY * m_nXDim * m_nZDim + nZ * m_nXDim + nX;
			sample = m_pLightSamples[nIndex];

			int nDLightID = sample.wDLightIndex;
			if( nDLightID >= A3DLIGHTGRID_MAX_DLIGHT || !m_aDLights[nDLightID]->IsUsed() )
			{
			}
			else
			{
				pDLightParam->Type = A3DLIGHT_POINT;
				pDLightParam->Diffuse = m_aDLights[nDLightID]->GetColor();
				pDLightParam->Ambient = pDLightParam->Diffuse * 0.5f;
				pDLightParam->Ambient.a = 1.0f;
				pDLightParam->Specular = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
				pDLightParam->Position = m_aDLights[nDLightID]->GetPos();
				pDLightParam->Range = m_aDLights[nDLightID]->GetRange() * 2.0f;
				pDLightParam->Attenuation0 = 1.0f;
				pDLightParam->Attenuation1 = 4.0f / pDLightParam->Range;
				pDLightParam->Attenuation2 = 0.0f;
			}
		}
	}
	return true;
}

bool A3DIBLLightGrid::GetNearestLightInfo(const A3DVECTOR3& vecPoint, A3DVECTOR3 * pvecLightDir, A3DCOLORVALUE * pclLightDirect, A3DCOLOR * pclAmbient, A3DLIGHTPARAM * pDLightParam)
{
	int nIndex;

	if( !GetNearestLightSampleIndex(vecPoint, &nIndex) )
		return false;

	A3DIBLLIGHTSAMPLE sample;
	GetLightSample(nIndex, &sample);

	if( m_pLightSources[sample.wLightIndex].type == A3DIBLLIGHTTYPE_DIRECT )
		*pvecLightDir = m_pLightSources[sample.wLightIndex].vecDir;
	else
		*pvecLightDir = Normalize(vecPoint - m_pLightSources[sample.wLightIndex].vecPos);

	A3DCOLORVALUE	colDirect;
	A3DCOLOR		colAmbient;

	colDirect = ColorRGBAToColorValue(sample.colorDirect);
	colDirect.r *= m_vDirBrightness;
	colDirect.g *= m_vDirBrightness;
	colDirect.b *= m_vDirBrightness;
	colDirect.a = 1.0f;
	*pclLightDirect = colDirect;

	colAmbient = sample.colorAmbient;
	BYTE r, g, b;
	r = int(A3DCOLOR_GETRED(colAmbient) * m_vAmbientBrightness);
	g = int(A3DCOLOR_GETGREEN(colAmbient) * m_vAmbientBrightness);
	b = int(A3DCOLOR_GETBLUE(colAmbient) * m_vAmbientBrightness);
	if( r > 255 ) r = 255;
	if( g > 255 ) g = 255;
	if( b > 255 ) b = 255;
	*pclAmbient = A3DCOLORRGB(r, g, b);

	// Get Dynamic light if need;
	if( pDLightParam )
	{
		// We should retrieve the Dynamic light parameter for it;
		ZeroMemory(pDLightParam, sizeof(A3DLIGHTPARAM));
		pDLightParam->Type = A3DLIGHT_FORCE_DWORD; // nothing

		if( g_pA3DConfig->GetFlagDynamicLight() )
		{
			int nDLightID = sample.wDLightIndex;
			if( nDLightID >= A3DLIGHTGRID_MAX_DLIGHT || !m_aDLights[nDLightID]->IsUsed() )
			{
			}
			else
			{
				pDLightParam->Type = A3DLIGHT_POINT;
				pDLightParam->Diffuse = m_aDLights[nDLightID]->GetColor();
				pDLightParam->Ambient = pDLightParam->Diffuse * 0.5f;
				pDLightParam->Ambient.a = 1.0f;
				pDLightParam->Specular = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
				pDLightParam->Position = m_aDLights[nDLightID]->GetPos();
				pDLightParam->Range = m_aDLights[nDLightID]->GetRange() * 2.0f;
				pDLightParam->Attenuation0 = 1.0f;
				pDLightParam->Attenuation1 = 4.0f / pDLightParam->Range;
				pDLightParam->Attenuation2 = 0.0f;
			}
		}
	}

	return true;
}

bool A3DIBLLightGrid::GetNearestLightSampleIndex(const A3DVECTOR3& vecPoint, int * pnIndex)
{
	int nX = int((vecPoint.x - m_vecMin.x) * m_vXDF + 0.5f);
	int nZ = int((vecPoint.z - m_vecMin.z) * m_vZDF + 0.5f);
	int nY = int((vecPoint.y - m_vecMin.y) * m_vYDF + 0.5f);
	
	if( nX < 0 ) nX = 0;
	if( nX > m_nLength ) nX = m_nLength;

	if( nZ < 0 ) nZ = 0;
	if( nZ > m_nWidth ) nZ = m_nWidth;

	if( nY < 0 ) nY = 0;
	if( nY > m_nHeight ) nY = m_nHeight;

	*pnIndex = nY * m_nXDim * m_nZDim + nZ * m_nXDim + nX;
	return true;
}

bool A3DIBLLightGrid::GetSamplePosition(int nIndex, A3DVECTOR3 * pvecPoint)
{
	int nX = nIndex % m_nXDim;
	int nZ = (nIndex / m_nXDim) % m_nZDim;
	int nY = (nIndex / (m_nXDim * m_nZDim)) % m_nYDim;

	pvecPoint->x = m_vecMin.x + nX / m_vXDF;
	pvecPoint->z = m_vecMin.z + nZ / m_vZDF;
	pvecPoint->y = m_vecMin.y + nY / m_vYDF;
	return true;
}

bool A3DIBLLightGrid::SetLightSample(int nIndex, const A3DIBLLIGHTSAMPLE& sample)
{
	m_pLightSamples[nIndex] = sample;
	return true;
}

bool A3DIBLLightGrid::GetLightSample(int nIndex, A3DIBLLIGHTSAMPLE * pSample)
{
	*pSample = m_pLightSamples[nIndex];
	return true;
}

bool A3DIBLLightGrid::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	if( !Init(pA3DDevice) )
		return false;

	if( pFileToLoad->IsText() )
	{
		return false;
	}
	else
	{
		DWORD		dwReadLength;

		pFileToLoad->Read(&m_nLightSourceCount, sizeof(m_nLightSourceCount), &dwReadLength);
		m_pLightSources = (A3DIBLLIGHTSOURCE *) malloc(sizeof(A3DIBLLIGHTSOURCE) * m_nLightSourceCount);
		if( NULL == m_pLightSources )
			return false;
		pFileToLoad->Read(m_pLightSources, sizeof(A3DIBLLIGHTSOURCE) * m_nLightSourceCount, &dwReadLength);

		pFileToLoad->Read(&m_nLength, sizeof(m_nLength), &dwReadLength);
		pFileToLoad->Read(&m_nWidth, sizeof(m_nWidth), &dwReadLength);
		pFileToLoad->Read(&m_nHeight, sizeof(m_nHeight), &dwReadLength);

		m_nXDim = m_nLength + 1;
		m_nZDim = m_nWidth + 1;
		m_nYDim = m_nHeight + 1;

		m_pLightSamples = (A3DIBLLIGHTSAMPLE *) malloc(sizeof(A3DIBLLIGHTSAMPLE) * m_nXDim * m_nZDim * m_nYDim);
		if( NULL == m_pLightSamples )
			return false;

		pFileToLoad->Read(m_pLightSamples, sizeof(A3DIBLLIGHTSAMPLE) * m_nXDim * m_nZDim * m_nYDim, &dwReadLength);

		pFileToLoad->Read(&m_vecMin, sizeof(A3DVECTOR3), &dwReadLength);
		pFileToLoad->Read(&m_vecMax, sizeof(A3DVECTOR3), &dwReadLength);

		m_vXDF = m_nLength / (m_vecMax.x - m_vecMin.x);
		m_vZDF = m_nWidth  / (m_vecMax.z - m_vecMin.z);
		m_vYDF = m_nHeight / (m_vecMax.y - m_vecMin.y);

		pFileToLoad->Read(&m_bOptimized, sizeof(m_bOptimized), &dwReadLength);
	}

	return true;
}

bool A3DIBLLightGrid::Save(AFile * pFileToSave)
{
	if( pFileToSave->IsText() )
	{
		return false;
	}
	else
	{
		DWORD		dwWriteLength;
		
		pFileToSave->Write(&m_nLightSourceCount, sizeof(m_nLightSourceCount), &dwWriteLength);
		pFileToSave->Write(m_pLightSources, sizeof(A3DIBLLIGHTSOURCE) * m_nLightSourceCount, &dwWriteLength);

		pFileToSave->Write(&m_nLength, sizeof(m_nLength), &dwWriteLength);
		pFileToSave->Write(&m_nWidth, sizeof(m_nWidth), &dwWriteLength);
		pFileToSave->Write(&m_nHeight, sizeof(m_nHeight), &dwWriteLength);

		pFileToSave->Write(m_pLightSamples, sizeof(A3DIBLLIGHTSAMPLE) * m_nXDim * m_nZDim * m_nYDim, &dwWriteLength);

		pFileToSave->Write(&m_vecMin, sizeof(A3DVECTOR3), &dwWriteLength);
		pFileToSave->Write(&m_vecMax, sizeof(A3DVECTOR3), &dwWriteLength);

		pFileToSave->Write(&m_bOptimized, sizeof(m_bOptimized), &dwWriteLength);
	}

	return true;
}

bool A3DIBLLightGrid::PrepareFirstRender()
{
	int		i;
	int		* nIDMap = new int[m_nLightSourceCount];

	for(i=0; i<m_nXDim * m_nZDim * m_nYDim; i++)
		m_pLightSamples[i].wDLightIndex = 0xffff;
	
	// First find out how many lamp effects needed;
	m_nLampCount = 0;
	for(i=0; i<m_nLightSourceCount; i++)
	{
		if( m_pLightSources[i].nLampStyle == -1 )
			continue;

		nIDMap[m_nLampCount] = i;
		m_nLampCount ++;
	}

	m_pLampEffects = (A3DLamp **) malloc(sizeof(A3DLamp *) * m_nLampCount);
	if( NULL == m_pLampEffects )
		return false;

	ZeroMemory(m_pLampEffects, sizeof(A3DLamp *) * m_nLampCount);
	for(i=0; i<m_nLampCount; i++)
	{
		A3DLamp * pLamp;
		A3DIBLLIGHTSOURCE source = m_pLightSources[nIDMap[i]];
		A3DCOLORVALUE lampColor;

		switch(source.nLampStyle)
		{
		case LAMPSTYLE_SUN:
			pLamp = new A3DLamp();
			if( !pLamp->Init(m_pA3DDevice, "Shine.bmp", LAMPSTYLE_SUN) )
			{
				g_pA3DErrLog->ErrLog("A3DIBLLightGrid::PrepareFirstRender(), create a sun lamp fail!");
				delete pLamp;
				break;
			}
			lampColor = source.color;
			lampColor.r *= source.vLampIntensity;
			lampColor.g *= source.vLampIntensity;
			lampColor.b *= source.vLampIntensity;
			pLamp->SetLampParams(200, 200, -source.vecDir * 1000.0f, A3DVECTOR3(0.0f), ColorValueToColorRGBA(lampColor));
			pLamp->SetViewConeParams(50.0f, 120.0f);
			pLamp->SetLensFlareParams("Flare.bmp");

			m_pLampEffects[i] = pLamp;
			break;

		case LAMPSTYLE_UNREAL:
			pLamp = new A3DLamp();
			if( !pLamp->Init(m_pA3DDevice, "Lamp.bmp", LAMPSTYLE_UNREAL) )
			{
				g_pA3DErrLog->ErrLog("A3DIBLLightGrid::AddLightSource(), create a unreal lamp fail!");
				delete pLamp;
				break;
			}
			lampColor = source.color;
			lampColor.r *= source.vLampIntensity;
			lampColor.g *= source.vLampIntensity;
			lampColor.b *= source.vLampIntensity;
			pLamp->SetLampParams(float(source.nLampSize), float(source.nLampSize), source.vecPos, source.vecDir, ColorValueToColorRGBA(lampColor));
			pLamp->SetDiatanceParams(0.0f, source.vLampRange);
			pLamp->SetViewConeParams(50.0f, 120.0f);
			
			m_pLampEffects[i] = pLamp;
			break;

		default:
			m_pLampEffects[i] = NULL;
			break;
		}
	}

	delete [] nIDMap;
	return true;
}

bool A3DIBLLightGrid::RenderLamps(A3DViewport * pCurrentViewport)
{
	for(int i=0; i<m_nLampCount; i++)
	{
		A3DLamp * pLamp = m_pLampEffects[i];
		if( !pLamp )
			continue;

		pLamp->Render(pCurrentViewport);
	}

	return true;
}

bool A3DIBLLightGrid::AnimateLamps()
{
	return true;
}

bool A3DIBLLightGrid::GetNextDLight(int * pDLightID)
{
	*pDLightID = -1;

	if( m_nDLightCount >= A3DLIGHTGRID_MAX_DLIGHT )
		return true;

	// Now find a unused dynamic light slot;
	int i;
	for(i=0; i<A3DLIGHTGRID_MAX_DLIGHT; i++)
	{
		if( !m_aDLights[i]->IsUsed() )
		{
			m_aDLights[i]->SetUsed(true);
			*pDLightID = i;
			return true;
		}
	}

	return true;
}

bool A3DIBLLightGrid::SetFreeDLight(int nDLightID)
{
	m_aDLights[nDLightID]->SetUsed(false);

	return true;
}

bool A3DIBLLightGrid::UpdateDLight(int nDLightID, const A3DVECTOR3& vecPos, const A3DCOLORVALUE& color, const FLOAT& vRange)
{
	if( !m_aDLights[nDLightID]->IsUsed() )
		return false;

	// First update color;
	UpdateDLightColorOnly(nDLightID, color);

	// Then update position related information;
	m_aDLights[nDLightID]->SetPos(vecPos);
	m_aDLights[nDLightID]->SetRange(vRange);

	A3DVECTOR3 vecMins, vecMaxs;
	vecMins = vecPos - A3DVECTOR3(vRange, vRange, vRange);
	vecMaxs = vecPos + A3DVECTOR3(vRange, vRange, vRange);

	// Now update the light sample grid to index this dynamic light;
	int nStartX, nStartZ, nStartY;
	int nEndX, nEndZ, nEndY;
	nStartX = int((vecMins.x - m_vecMin.x) * m_vXDF);
	nStartZ = int((vecMins.z - m_vecMin.z) * m_vZDF);
	nStartY = int((vecMins.y - m_vecMin.y) * m_vYDF);
	
	if( nStartX < 0 ) nStartX = 0;
	if( nStartX > m_nLength ) nStartX = m_nLength;

	if( nStartZ < 0 ) nStartZ = 0;
	if( nStartZ > m_nWidth ) nStartZ = m_nWidth;

	if( nStartY < 0 ) nStartY = 0;
	if( nStartY > m_nHeight ) nStartY = m_nHeight;

	nEndX = int((vecMaxs.x - m_vecMin.x) * m_vXDF) + 1;
	nEndZ = int((vecMaxs.z - m_vecMin.z) * m_vZDF) + 1;
	nEndY = int((vecMaxs.y - m_vecMin.y) * m_vYDF) + 1;
	
	if( nEndX < 0 ) nEndX = 0;
	if( nEndX > m_nLength ) nEndX = m_nLength;

	if( nEndZ < 0 ) nEndZ = 0;
	if( nEndZ > m_nWidth ) nEndZ = m_nWidth;

	if( nEndY < 0 ) nEndY = 0;
	if( nEndY > m_nHeight ) nEndY = m_nHeight;

	for(int nY=nStartY; nY<=nEndY; nY++)
	{
		for(int nZ=nStartZ; nZ<=nEndZ; nZ++)
		{
			for(int nX=nStartX; nX<=nEndX; nX++)
			{
				int nIndex = nY * m_nXDim * m_nZDim + nZ * m_nXDim + nX;

				m_pLightSamples[nIndex].wDLightIndex = nDLightID;
			}
		}
	}
	return true;
}

bool A3DIBLLightGrid::UpdateDLightColorOnly(int nDLightID, const A3DCOLORVALUE& color)
{
	if( !m_aDLights[nDLightID]->IsUsed() )
		return false;

	A3DCOLORVALUE colorReal = color;
	if( m_colorOP == A3DTOP_MODULATE2X )
	{
		colorReal.r /= 2.0f; colorReal.g /= 2.0f; colorReal.b /= 2.0f; colorReal.a /= 2.0f;
	}
	else if( m_colorOP == A3DTOP_MODULATE4X )
	{
		colorReal.r /= 4.0f; colorReal.g /= 4.0f; colorReal.b /= 4.0f; colorReal.a /= 4.0f;
	}

	m_aDLights[nDLightID]->SetColor(colorReal);

	// We need not to modify the sample grid index of this dynamic light, for
	// only color has changed;
	return true;
}

/*********************************************/
/*      Light Grid Dynamic Light Class       */
/*********************************************/
A3DLightGridDLight::A3DLightGridDLight()
{
	m_bUsed = false;
}

A3DLightGridDLight::~A3DLightGridDLight()
{
}
