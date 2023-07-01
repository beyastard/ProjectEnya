#include "A3DTextureMan.h"
#include "A3DErrLog.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"
#include "A3DShaderMan.h"
#include "A3DGDI.h"

A3DTextureMan::A3DTextureMan()
{
	m_pA3DDevice = NULL;
}

A3DTextureMan::~A3DTextureMan()
{
}

bool A3DTextureMan::Init(A3DDevice * pDevice)
{
	m_pA3DDevice = pDevice;
	m_ListTexture.Init();
	return true;
}

bool A3DTextureMan::Release()
{
	ALISTELEMENT * pThisTextureElement = m_ListTexture.GetFirst();

	// We should release shader texture first, or it will cause access violation
	while( pThisTextureElement != m_ListTexture.GetTail() )
	{
		TEXTURERECORD * pTextureRecord = (TEXTURERECORD *) pThisTextureElement->pData;
		
		A3DTexture * pA3DTexture = pTextureRecord->pA3DTexture;

		if( pA3DTexture && pA3DTexture->IsShaderTexture() )
		{
			pA3DTexture->Release();
			delete pA3DTexture;
			pA3DTexture = NULL;
			pTextureRecord->pA3DTexture = NULL;
		}

		pThisTextureElement = pThisTextureElement->pNext;
	}

	while( pThisTextureElement != m_ListTexture.GetTail() )
	{
		TEXTURERECORD * pTextureRecord = (TEXTURERECORD *) pThisTextureElement->pData;
		
		A3DTexture * pA3DTexture = pTextureRecord->pA3DTexture;

		if( pA3DTexture )
		{
			pA3DTexture->Release();
			delete pA3DTexture;
			pA3DTexture = NULL;
		}

		free(pTextureRecord);
		pThisTextureElement = pThisTextureElement->pNext;
	}

	m_ListTexture.Release();
	return true;
}

bool A3DTextureMan::FindTexture(char * szFilename, A3DTexture ** ppA3DTexture)
{
	ALISTELEMENT * pThisTextureElement = m_ListTexture.GetFirst();

	while( pThisTextureElement != m_ListTexture.GetTail() )
	{
		TEXTURERECORD * pTextureRecord = (TEXTURERECORD *) pThisTextureElement->pData;

		if( 0 == _stricmp(szFilename, pTextureRecord->szFilename) )
		{
			pTextureRecord->nRefCount ++;
			*ppA3DTexture = pTextureRecord->pA3DTexture;
		}

		pThisTextureElement = pThisTextureElement->pNext;
	}
	return true;
}

bool A3DTextureMan::LoadTextureFromFileInFolder(char * pszFilename, char * szFolder, A3DTexture ** ppA3DTexture, DWORD dwTextureFlags, int nMipLevel)
{
	char szRelativePath[MAX_PATH];

	APath_GetFullPath(szRelativePath, szFolder, pszFilename);
	return LoadTextureFromFile(szRelativePath, ppA3DTexture, dwTextureFlags, nMipLevel);
}

bool A3DTextureMan::LoadTextureFromFile(char * pszFilename, A3DTexture ** ppA3DTexture, DWORD dwTextureFlags, int nMipLevel)
{
	// We don't need to load an empty texture;
	if( !pszFilename )
		return true;

	A3DTexture * pNewA3DTexture;
	*ppA3DTexture = NULL;

	if( !FindTexture(pszFilename, ppA3DTexture) )
		return false;
	
	if( *ppA3DTexture )
		return true;

	char  szNameLwr[MAX_PATH];
	char  *pChar;
	strncpy(szNameLwr, pszFilename, MAX_PATH);
	_strlwr(szNameLwr);
	if( pChar = strstr(szNameLwr, ".sdr") )
	{
		// It is a shader file;
		*(pChar + 4) = '\0';
		pNewA3DTexture = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(szNameLwr);
		if( !pNewA3DTexture )
			return false;
		
		goto RECORD;
	}

	pNewA3DTexture = new A3DTexture();
	if( NULL == pNewA3DTexture )
	{
		g_pA3DErrLog->ErrLog("A3DTextureMan::LoadTextureFromFile(), Not enough memory!");
		return false;
	}

	char szFullpath[MAX_PATH];
	if( strlen(pszFilename) == 0 )
		AFileMod_GetFullPath(szFullpath, "Textures\\MissingTex.bmp");
	else
		AFileMod_GetFullPath(szFullpath, pszFilename);
	
	if( dwTextureFlags & A3DTF_DETAILMAP )
		pNewA3DTexture->SetIsDetailTexture(true);
	if( dwTextureFlags & A3DTF_LIGHTMAP )
		pNewA3DTexture->SetIsLightMap(true);
	if( dwTextureFlags & A3DTF_MIPLEVEL )
		pNewA3DTexture->SetDesciredMipLevel(nMipLevel);

	if( !pNewA3DTexture->LoadFromFile(m_pA3DDevice, szFullpath, pszFilename) )
	{
		g_pA3DErrLog->ErrLog("A3DTextureMan::LoadTextureFromFile(), Create new A3DTexture fail!");
		return false;
	}

RECORD:
	TEXTURERECORD * pNewTextureRecord = (TEXTURERECORD *) malloc(sizeof(TEXTURERECORD));
	if( NULL == pNewTextureRecord ) 
	{
		g_pA3DErrLog->ErrLog("A3DTextureMan::LoadTextureFile Not enough Memory!");
		return false;
	}
	strncpy(pNewTextureRecord->szFilename, pszFilename, MAX_PATH);
	pNewTextureRecord->pA3DTexture = pNewA3DTexture;
	pNewTextureRecord->nRefCount = 1;
	
	if( !m_ListTexture.Append((LPVOID)pNewTextureRecord) )
		return false;

	pNewA3DTexture->TickAnimation();
	*ppA3DTexture = pNewA3DTexture;
	return true;
}

bool A3DTextureMan::ReleaseTexture(PA3DTexture& pA3DTexture)
{
	ALISTELEMENT * pThisTextureElement = m_ListTexture.GetFirst();

	while( pThisTextureElement != m_ListTexture.GetTail() )
	{
		TEXTURERECORD * pTextureRecord = (TEXTURERECORD *) pThisTextureElement->pData;

		if( pTextureRecord->pA3DTexture == pA3DTexture )
		{
			pTextureRecord->nRefCount --;
			if( 0 == pTextureRecord->nRefCount )
			{
				pA3DTexture->Release();
				delete pA3DTexture;
				pA3DTexture = NULL;

				free(pTextureRecord);
				pTextureRecord = NULL;
				m_ListTexture.Delete(pThisTextureElement);
			}
			else
				pA3DTexture = NULL;
			return true;
		}

		pThisTextureElement = pThisTextureElement->pNext;
	}

	g_pA3DErrLog->ErrLog("A3DTextureMan::ReleaseTexture(), Can not find texture [%x]", pA3DTexture);
	return true;
}

bool A3DTextureMan::Reset()
{
	ALISTELEMENT * pThisTextureElement = m_ListTexture.GetFirst();

	while( pThisTextureElement != m_ListTexture.GetTail() )
	{
		TEXTURERECORD * pTextureRecord = (TEXTURERECORD *) pThisTextureElement->pData;
		
		A3DTexture * pA3DTexture = pTextureRecord->pA3DTexture;
		pA3DTexture->Release();
		delete pA3DTexture;
		pA3DTexture = NULL;

		free(pTextureRecord);
		pThisTextureElement = pThisTextureElement->pNext;
	}

	m_ListTexture.Reset();
	return true;
}

bool A3DTextureMan::TickAnimation()
{
	ALISTELEMENT * pThisTextureElement = m_ListTexture.GetFirst();

	while( pThisTextureElement != m_ListTexture.GetTail() )
	{
		TEXTURERECORD * pTextureRecord = (TEXTURERECORD *) pThisTextureElement->pData;
		A3DTexture * pA3DTexture = pTextureRecord->pA3DTexture;
		
		pA3DTexture->TickAnimation();
		
		pThisTextureElement = pThisTextureElement->pNext;
	}

	return true;
}

bool A3DTextureMan::PrecacheAllTexture()
{
	if( !m_pA3DDevice->GetD3DDevice() )
		return true;

	ALISTELEMENT * pElement = m_ListTexture.GetFirst();

	// We use a L vertex rectange to make all MIP-MAP level rendered;
	A3DLVERTEX		verts[6];
	verts[0] = A3DLVERTEX(A3DVECTOR3(-100.0f, -10.0f, 0.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);
	verts[1] = A3DLVERTEX(A3DVECTOR3(-100.0f, -10.0f, 2000.0f), 0xffffffff, 0xff000000, 1.0f, 0.0f);
	verts[2] = A3DLVERTEX(A3DVECTOR3( 100.0f, -10.0f, 2000.0f), 0xffffffff, 0xff000000, 0.0f, 1.0f);

	verts[3] = verts[2];
	verts[4] = A3DLVERTEX(A3DVECTOR3( 100.0f, -10.0f, 0.0f), 0xffffffff, 0xff000000, 1.0f, 1.0f);
	verts[5] = verts[0];
	
	D3DXMATRIX matProj;
	A3DMATRIX4 matProject;

	D3DXMatrixPerspectiveFovLH(&matProj, DEG2RAD(60.0f), 1.333f, 1.0f, 2000.0f);
	matProject = *(A3DMATRIX4 *)&matProj;

	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetProjectionMatrix(matProject);
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetViewMatrix(IdentityMatrix());

	m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DLVERTEX);
	while( pElement != m_ListTexture.GetTail() )
	{
		TEXTURERECORD * pTextureRecord = (TEXTURERECORD *) pElement->pData;
		A3DTexture * pTexture = pTextureRecord->pA3DTexture;

		m_pA3DDevice->BeginRender();

		pTexture->Appear(0);
		
		m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, 2, verts, sizeof(A3DLVERTEX));

		pTexture->Disappear(0);

		A3DRECT rectScreen;
		rectScreen.left = 0;
		rectScreen.top = 0;
		rectScreen.right = m_pA3DDevice->GetDevFormat().nWidth;
		rectScreen.bottom = m_pA3DDevice->GetDevFormat().nHeight;
		g_pA3DGDI->Draw2DRectangle(rectScreen, 0xff000000);
		
		m_pA3DDevice->EndRender();

		m_pA3DDevice->Present();

		pElement = pElement->pNext;
	}

	m_pA3DDevice->SetLightingEnable(true);
	return true;
}