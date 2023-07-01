/*
 * FILE: A3DShader.cpp
 *
 * DESCRIPTION: Routines for shader
 *
 * CREATED BY: duyuxin, 2002/1/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DShader.h"
#include "A3DErrLog.h"
#include "A3DTime.h"
#include "A3DMathUtility.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DShaderMan.h"
#include "A3DConfig.h"

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

//	Default general properties
SHADERGENERAL A3DShader::m_DefGeneral = 
{
	0,						//	dwMask
	A3DCMP_LESSEQUAL,		//	dwZFunc
	true,					//	bZEnable
	true,					//	bZWriteEnable
	A3DCMP_ALWAYS,			//	dwAlphaFunc
	128,					//	iAlphaRef
	A3DCULL_CCW,			//	dwCullMode
	A3DBLEND_SRCALPHA,		//	dwSrcBlend
	A3DBLEND_INVSRCALPHA,	//	dwDstBlend
	NULL,					//	pPShader
	//	rgbGen
	//	AlphaGen
};

//	Default stage properties
SHADERSTAGE A3DShader::m_DefStage = 
{
	0,						//	dwMask
	0,						//	dwStates
	A3DTEX_NORMAL,			//	iTexType
//	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},	//	aTextures
	{NULL},					//	aTextures
	0,						//	iNumTex
	0,						//	dwFrameTime
	A3DTEXF_LINEAR,			//	dwFilter
	A3DTADDR_WRAP,			//	dwAddress
	A3DTOP_DISABLE,			//	dwColorOp (for stage 0, this should be A3DTOP_MODULATE)
	A3DTA_TEXTURE,			//	dwColorArg1
	A3DTA_CURRENT,			//	dwColorArg2
	A3DTOP_DISABLE,			//	dwAlphaOp (for stage 0, this should be A3DTOP_SELECTARG1)
	A3DTA_TEXTURE,			//	dwAlphaArg1
	A3DTA_CURRENT,			//	dwAlphaArg2
	0.0f, 0.0f, 0.0f,		//	r, g, b
	0.0f,					//	a
	A3DCMP_ALWAYS,			//	dwAlphaFunc
	128,					//	iAlphaRef
	0.0f, 0.0f, 0.0f, 0.0f,	//	fBumpEnvMat00, fBumpEnvMat01, fBumpEnvMat10, fBumpEnvMat11
	0.0f, 0.0f,				//	fBumpEnvScale, fBumpEnvOffset
	0,						//	iUVGenFunc
	0,						//	iProTexFunc
	0,						//	iNumtcMod
	//	atcMods[]
};

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

A3DShader::A3DShader() : A3DTexture()
{
	m_pA3DDevice	= NULL;
	m_iNumStage		= 0;
	m_szName[0]		= 0;
	m_dwStartTime	= 0;
	m_iMaxNumStage	= 0;
	m_bAlphaTexture	= false;
	m_bShaderTexture = true;

	memset(m_aStages, 0, sizeof (m_aStages));
	memset(&m_General, 0, sizeof (m_General));
	memset(m_aFuncVal, 0, sizeof (m_aFuncVal));
	memset(m_aCurFrame, 0, sizeof (m_aCurFrame));
}

A3DShader::~A3DShader()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pDevice: address of valid A3DDevice object
*/
bool A3DShader::Init(A3DDevice* pDevice)
{
	m_pA3DDevice	= pDevice;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWITexture = true;
		return true;
	}

	m_iNumStage		= 0;
	m_dwStartTime	= 0;
	m_bAlphaTexture	= false;
	m_bShaderTexture = true;

	m_iMaxNumStage = pDevice->GetMaxSimultaneousTextures();

	return true;
}

//	Release object
bool A3DShader::Release()
{
	if( m_bHWITexture ) return true;

	//	Clear general properties
	ClearGeneralProps();

	int i;
	for (i=0; i < m_iNumStage; i++)
	{
		//	Release texture
		ReleaseStageTexture(i);
	}

	memset(m_aStages, 0, sizeof (m_aStages));
	memset(&m_General, 0, sizeof (m_General));
	memset(m_aFuncVal, 0, sizeof (m_aFuncVal));
	memset(m_aCurFrame, 0, sizeof (m_aCurFrame));

	m_iNumStage = 0;
	m_szName[0]	= 0;

	m_pA3DDevice = NULL;

	return true;
}

/*	Apply shader.

	iLayer: parameter of CA3DTexture::Appear, we don't use it in shader
*/
bool A3DShader::Appear(int iLayer/* 0 */)
{
	if( m_bHWITexture ) return true;

	//	Apply general properties
	ApplyGeneral();

	if (!m_iNumStage)
	{
		m_pA3DDevice->ClearTexture(0);
		return true;
	}

	//	Apply stage properties
	int i, iNumStage = 0;

	for (i=0; i < m_iNumStage; i++)
	{
		if (StageIsVisible(i))
			ApplyStage(iNumStage++, i);

		if (iNumStage >= m_iMaxNumStage)
			break;
	}

	//	Disable unused stages
	if (iNumStage && iNumStage < MAXNUM_STAGE)
		m_pA3DDevice->SetTextureColorOP(iNumStage, A3DTOP_DISABLE);

	return true;
}

/*	Abolish shader.

	iLayer: parameter of CA3DTexture::Disappear, we don't use it in shader
*/
bool A3DShader::Disappear(int iLayer/* 0 */)
{
	if( m_bHWITexture ) return true;

	//	Apply default general properties
	ApplyDefGeneral();

	//	Apply default stage properties
	int i, iNumStage = 0;

	for (i=0; i < m_iNumStage; i++)
	{
		if (StageIsVisible(i))
			ApplyDefStage(iNumStage++, i);

		if (iNumStage >= m_iMaxNumStage)
			break;
	}

	return true;
}

//	Apply general properties
void A3DShader::ApplyGeneral()
{
	if (m_General.dwMask & SGMF_ZFUNC)
		m_pA3DDevice->SetZFunc(m_General.dwZFunc);

	if (m_General.dwMask & SGMF_ZENABLE)
		m_pA3DDevice->SetZTestEnable(m_General.bZEnable);

	if (m_General.dwMask & SGMF_ZWENABLE)
		m_pA3DDevice->SetZWriteEnable(m_General.bZWriteEnable);

	if (m_General.dwMask & SGMF_ALPHACOMP)
	{
		m_pA3DDevice->SetAlphaTestEnable(true);
		m_pA3DDevice->SetAlphaFunction((A3DCMPFUNC)m_General.dwAlphaFunc);
		m_pA3DDevice->SetAlphaRef(m_General.iAlphaRef);
	}

	if (m_General.dwMask & SGMF_CULLMODE)
		m_pA3DDevice->SetFaceCull((A3DCULLTYPE)m_General.dwCullMode);

	if (m_General.dwMask & SGMF_BLEND)
	{
		m_pA3DDevice->SetAlphaBlendEnable(true);
		m_pA3DDevice->SetSourceAlpha((A3DBLEND)m_General.dwSrcBlend);
		m_pA3DDevice->SetDestAlpha((A3DBLEND)m_General.dwDstBlend);
	}

	//	colorgen and alphagen aren't applied here
//	if (m_General.dwMask & SGMF_COLORGEN)
//	if (m_General.dwMask & SGMF_ALPHAGEN)

	if (m_General.pPShader)
		m_General.pPShader->Appear(false);
}

/*	Apply one stage

	iStage: destination stage level of D3D to which specified stage will applied to
	iIndex: index of stage in m_aStages.

	Note: usurally, iStage == iIndex, but when we hide one or more stages, they
		  may be not equal.
*/
void A3DShader::ApplyStage(int iStage, int iIndex)
{
	SHADERSTAGE* pStage = &m_aStages[iIndex];

	m_pA3DDevice->SetTextureCoordIndex(iStage, 0);

	if (pStage->iTexType == A3DTEX_NORMAL && pStage->aTextures[0])
		pStage->aTextures[0]->Appear(iStage);
	else if (pStage->iTexType == A3DTEX_ANIMATION)
	{
		if (pStage->aTextures[m_aCurFrame[iIndex]])
			pStage->aTextures[m_aCurFrame[iIndex]]->Appear(iStage);
	}

	if (pStage->dwMask & SSMF_FILTER)
		m_pA3DDevice->SetTextureFilterType(iStage, (A3DTEXTUREFILTERTYPE)pStage->dwFilter);

	if (pStage->dwMask & SSMF_ADDRESS)
		m_pA3DDevice->SetTextureAddress(iStage, (A3DTEXTUREADDR)pStage->dwAddress, (A3DTEXTUREADDR)pStage->dwAddress);

	if (pStage->dwMask & SSMF_BUMPMAT)
	{
		m_pA3DDevice->SetTextureBumpMat(iStage, pStage->fBumpEnvMat00, pStage->fBumpEnvMat01,
										pStage->fBumpEnvMat10, pStage->fBumpEnvMat11);
	}

	if (pStage->dwMask & SSMF_BUMPSCALE)
		m_pA3DDevice->SetTextureBumpScale(iStage, pStage->fBumpEnvScale);

	if (pStage->dwMask & SSMF_BUMPOFFSET)
		m_pA3DDevice->SetTextureBumpOffset(iStage, pStage->fBumpEnvOffset);

	//	TODO: handle these...
//	if (pStage->dwMask & SSMF_COLORGEN)
//	if (pStage->dwMask & SSMF_ALPHAGEN)
//	if (pStage->dwMask & SSMF_ALPHACOMP)
//	Stage.iProTexFunc
//	Stage.iUVGenFunc

	//	Texture Coordinates Modify
	if (pStage->iNumtcMod)
	{
		int i;
		SHADERWAVE* pWave;
		A3DMATRIX4 mat = IdentityMatrix();

		for (i=0; i < pStage->iNumtcMod; i++)
		{
			pWave = &pStage->atcMods[i];
			switch (pWave->iType)
			{
			case SWO_TCSCALE:

				//	Scale u and v
				mat._11 *= pWave->fBase;	mat._21 *= pWave->fBase;
				mat._31 *= pWave->fBase;	
				mat._12 *= pWave->fAmp;		mat._22 *= pWave->fAmp;
				mat._32 *= pWave->fAmp;
				break;

			case SWO_TCSCROLL:

				mat._31 += m_aFuncVal[VAL_TCMODUSCROLL0 + iIndex];
				mat._32 += m_aFuncVal[VAL_TCMODVSCROLL0 + iIndex];
				break;

			case SWO_TCROTATE:

				//	u -= 0.5f, v -= 0.5f
				mat._31 -= 0.5f;	mat._32 -= 0.5f;

				mat = RotateZ(mat, m_aFuncVal[VAL_TCMODROTATE0 + iIndex]);

				//	u += 0.5f, v += 0.5f
				mat._31 += 0.5f;	mat._32 += 0.5f;
				break;

			case SWO_TCSTRETCH:
			{
				//	u -= 0.5f, v -= 0.5f
				mat._31 -= 0.5f;	mat._32 -= 0.5f;

				//	Scale u and v
				float fVal = m_aFuncVal[VAL_TCMODSTRETCH0 + iIndex];

				mat._11 *= fVal;	mat._21 *= fVal;	mat._31 *= fVal;
				mat._12 *= fVal;	mat._22 *= fVal;	mat._32 *= fVal;

				//	u += 0.5f, v += 0.5f
				mat._31 += 0.5f;	mat._32 += 0.5f;
				break;
			}
			case SWO_TCTURB:
			{
				mat._31 += m_aFuncVal[VAL_TCMODOUTURB0 + iIndex];
				mat._32 += m_aFuncVal[VAL_TCMODOVTURB0 + iIndex];

				//	Scale u and v
				float fu = m_aFuncVal[VAL_TCMODSUTURB0 + iIndex];
				float fv = m_aFuncVal[VAL_TCMODSVTURB0 + iIndex];

				mat._11 *= fu;	mat._21 *= fu;	mat._31 *= fu;
				mat._12 *= fv;	mat._22 *= fv;	mat._32 *= fv;
				break;
			}
			}
		}

		m_pA3DDevice->SetTextureTransformFlags(iStage, A3DTTFF_COUNT2);
		m_pA3DDevice->SetTextureMatrix(iStage, mat);
	}

	if (!m_General.pPShader)
	{
		//	Use legacy pixel pipeline
		if (pStage->dwMask & SSMF_COLOROP)
		{
			m_pA3DDevice->SetTextureColorOP(iStage, (A3DTEXTUREOP)pStage->dwColorOp);
			m_pA3DDevice->SetTextureColorArgs(iStage, pStage->dwColorArg1, pStage->dwColorArg2);
		}
		else	//	Enable this stage
		{
			m_pA3DDevice->SetTextureColorOP(iStage, A3DTOP_MODULATE);
			m_pA3DDevice->SetTextureColorArgs(iStage, A3DTA_TEXTURE, A3DTA_CURRENT);
		}
		
		if (pStage->dwMask & SSMF_ALPHAOP)
		{
			m_pA3DDevice->SetTextureAlphaOP(iStage, (A3DTEXTUREOP)pStage->dwAlphaOp);
			m_pA3DDevice->SetTextureAlphaArgs(iStage, pStage->dwAlphaArg1, pStage->dwAlphaArg2);
		}
		else
			m_pA3DDevice->SetTextureAlphaOP(iStage, A3DTOP_DISABLE);
	}
}

//	Apply default general properties
void A3DShader::ApplyDefGeneral()
{
	if (m_General.dwMask & SGMF_ZFUNC)
		m_pA3DDevice->SetZFunc(m_DefGeneral.dwZFunc);

	if (m_General.dwMask & SGMF_ZENABLE)
		m_pA3DDevice->SetZTestEnable(m_DefGeneral.bZEnable);

	if (m_General.dwMask & SGMF_ZWENABLE)
		m_pA3DDevice->SetZWriteEnable(m_DefGeneral.bZWriteEnable);

	if (m_General.dwMask & SGMF_ALPHACOMP)
	{
		m_pA3DDevice->SetAlphaTestEnable(false);
	//	m_pA3DDevice->SetAlphaFunction((A3DCMPFUNC)m_DefGeneral.dwAlphaFunc);
	//	m_pA3DDevice->SetAlphaRef(m_DefGeneral.iAlphaRef);
	}

	if (m_General.dwMask & SGMF_CULLMODE)
		m_pA3DDevice->SetFaceCull((A3DCULLTYPE)m_DefGeneral.dwCullMode);

	if (m_General.dwMask & SGMF_BLEND)
	{
		m_pA3DDevice->SetAlphaBlendEnable(true);	//	Our default value is enable
		m_pA3DDevice->SetSourceAlpha((A3DBLEND)m_DefGeneral.dwSrcBlend);
		m_pA3DDevice->SetDestAlpha((A3DBLEND)m_DefGeneral.dwDstBlend);
	}

	if (m_General.pPShader)
		m_General.pPShader->Disappear();
}

/*	Apply default stage properties for a stage

	iStage: destination stage level of D3D which will be applied to default data
	iIndex: index of stage in m_aStages.

	Note: usurally, iStage == iIndex, but when we hide one or more stages, they
		  may be not equal.
*/
void A3DShader::ApplyDefStage(int iStage, int iIndex)
{
	SHADERSTAGE* pStage = &m_aStages[iIndex];

	if (pStage->iTexType == A3DTEX_NORMAL && pStage->aTextures[0])
		pStage->aTextures[0]->Disappear(iStage);
	
	if (pStage->dwMask & SSMF_FILTER)
		m_pA3DDevice->SetTextureFilterType(iStage, (A3DTEXTUREFILTERTYPE)m_DefStage.dwFilter);

	if (pStage->dwMask & SSMF_ADDRESS)
		m_pA3DDevice->SetTextureAddress(iStage, (A3DTEXTUREADDR)m_DefStage.dwAddress, (A3DTEXTUREADDR)m_DefStage.dwAddress);

	if (pStage->dwMask & SSMF_BUMPMAT)
	{
		m_pA3DDevice->SetTextureBumpMat(iStage, m_DefStage.fBumpEnvMat00, m_DefStage.fBumpEnvMat01,
										m_DefStage.fBumpEnvMat10, m_DefStage.fBumpEnvMat11);
	}

	if (pStage->dwMask & SSMF_BUMPSCALE)
		m_pA3DDevice->SetTextureBumpScale(iStage, m_DefStage.fBumpEnvScale);

	if (pStage->dwMask & SSMF_BUMPOFFSET)
		m_pA3DDevice->SetTextureBumpOffset(iStage, m_DefStage.fBumpEnvOffset);

	//	Reset texture matrix
	if (pStage->iNumtcMod)
		m_pA3DDevice->SetTextureTransformFlags(iStage, A3DTTFF_DISABLE);

	if (!m_General.pPShader)
	{
		//	Use legacy pixel pipeline
		if (pStage->dwMask & SSMF_COLOROP)
		{
			if (!iStage)
				m_pA3DDevice->SetTextureColorOP(iStage, A3DTOP_MODULATE);
			else
				m_pA3DDevice->SetTextureColorOP(iStage, (A3DTEXTUREOP)m_DefStage.dwColorOp);
			
			m_pA3DDevice->SetTextureColorArgs(iStage, m_DefStage.dwColorArg1, m_DefStage.dwColorArg2);
		}
		
		//	if (pStage->dwMask & SSMF_ALPHAOP)
		{
			if (!iStage)
				m_pA3DDevice->SetTextureAlphaOP(iStage, A3DTOP_MODULATE);
			else
				m_pA3DDevice->SetTextureAlphaOP(iStage, (A3DTEXTUREOP)m_DefStage.dwAlphaOp);
			
			m_pA3DDevice->SetTextureAlphaArgs(iStage, m_DefStage.dwAlphaArg1, m_DefStage.dwAlphaArg2);
		}
	}
}

/*	Insert a stage.

	Return true for success, otherwise return false.

	iIndex: position of new stage, -1 means add stage at tail
	Stage: stage data
*/
bool A3DShader::InsertStage(int iIndex, SHADERSTAGE& Stage)
{
	if (m_iNumStage >= MAXNUM_STAGE || iIndex < -1)
		return false;

	if (iIndex == -1 || iIndex >= m_iNumStage)	//	Add at tail
		m_aStages[m_iNumStage] = Stage;
	else
	{
		int i;
		for (i=m_iNumStage-1; i >= iIndex; i--)
			m_aStages[i+1] = m_aStages[i];

		m_aStages[iIndex] = Stage;
	}

	m_iNumStage++;

	return true;
}

/*	Remove a stage

	iIndex: 0 base position of stage which will be removed
*/
void A3DShader::RemoveStage(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_iNumStage)
		return;

	//	Release texture
	ReleaseStageTexture(iIndex);

	int i;
	for (i=iIndex; i < m_iNumStage-1; i++)
		m_aStages[i] = m_aStages[i+1];

	m_iNumStage--;
}

//	Release specified stage's texture
void A3DShader::ReleaseStageTexture(int iStage)
{
	if (!m_pA3DDevice)
		return;

	int i;
	A3DEngine* pEngine = m_pA3DDevice->GetA3DEngine();

	if (m_aStages[iStage].iTexType == A3DTEX_ANIMATION)
	{
		for (i=0; i < m_aStages[iStage].iNumTex; i++)
		{
			if (m_aStages[iStage].aTextures[i])
				pEngine->GetA3DTextureMan()->ReleaseTexture(m_aStages[iStage].aTextures[i]);
		}
	}
	else if (m_aStages[iStage].aTextures[0])
		pEngine->GetA3DTextureMan()->ReleaseTexture(m_aStages[iStage].aTextures[0]);
}

//	Release pixel shader
void A3DShader::ReleasePixelShader()
{
	if (!m_General.pPShader || !m_pA3DDevice)
		return;

	A3DEngine* pEngine = m_pA3DDevice->GetA3DEngine();
	pEngine->GetA3DShaderMan()->ReleasePixelShader(m_General.pPShader);

	m_General.pPShader = NULL;
}

//	Clear general properties
void A3DShader::ClearGeneralProps()
{
	ReleasePixelShader();

	memset(&m_General, 0, sizeof (m_General));
}

/*	Set a stage's data

	Return true for success, otherwise return false.

	iStage: stage's index
	Stage: set a stage's data
*/
bool A3DShader::SetStage(int iStage, SHADERSTAGE& Stage)
{
	if (iStage < 0 || iStage >= m_iNumStage)
		return false;

	m_aStages[iStage] = Stage;

	return true;
}

/*	Change a stage's texture.

	Return true for success, otherwise return false.

	iStage: stage's index
	iType: texture type
	dwTex: for different texture type, this parameter has different means

		A3DTEX_NORMAL: dwTex is address of texture map file string.
		A3DTEX_LIGHTMAP:
		A3DTEX_BUMPMAP:
		A3DTEX_PROCESS:
		A3DTEX_DETAIL:
*/
bool A3DShader::ChangeStageTexture(int iStage, int iType, DWORD dwTex)
{
	if (iStage < 0)
		return false;

	SHADERSTAGE* pStage = &m_aStages[iStage];
	A3DEngine* pEngine  = m_pA3DDevice->GetA3DEngine();

	//	Release old texture
	ReleaseStageTexture(iStage);

	pStage->iTexType = iType;

	//	Load new map file
	if (iType == A3DTEX_NORMAL)
	{
		char* szMapFile = (char*)dwTex;

		A3DTexture* pTexture;

		if (!pEngine->GetA3DTextureMan()->LoadTextureFromFileInFolder(szMapFile, "Shaders\\Textures", &pTexture))
		{
			g_pA3DErrLog->ErrLog("A3DShader::ChangeStageTexture, failed to load map file %s", szMapFile);
			return false;
		}

		pStage->aTextures[0] = pTexture;
	}
	else if (iType == A3DTEX_ANIMATION)
	{
		char szPre[MAX_PATH], szSuf[16], szMapFile[MAX_PATH];
		char* pTemp, *szFile = (char*)dwTex;

		if (!(pTemp = strrchr(szFile, '.')))
		{
			g_pA3DErrLog->ErrLog("A3DShader::ChangeStageTexture, invalid map file %s", szFile);
			return false;
		}

		*pTemp = '\0';
		strcpy(szPre, szFile);
		sprintf(szSuf, ".%s", pTemp+1);

		int i;
		A3DTexture* pTexture;

		for (i=0; i < pStage->iNumTex; i++)
		{
			sprintf(szMapFile, "%s%02d%s", szPre, i, szSuf);

			if (!pEngine->GetA3DTextureMan()->LoadTextureFromFileInFolder(szMapFile, "Shaders\\Textures", &pTexture))
			{
				g_pA3DErrLog->ErrLog("A3DShader::ChangeStageTexture, failed to load map file %s", szMapFile);
				return false;
			}
			
			pStage->aTextures[i] = pTexture;
		}
	}

	return true;
}

/*	Change pixel shader

	Return true for success, otherwise return false.

	szFileName: shader's file name. NULL means clear pixel shader
*/
bool A3DShader::ChangePixelShader(char* szFileName)
{
	//	Release old pixel shader if it exists
	if (m_General.pPShader)
		ReleasePixelShader();

	if (!szFileName)
		return true;

	A3DPixelShader* pNewShader;
	A3DEngine* pEngine = m_pA3DDevice->GetA3DEngine();

	if (!pEngine->GetA3DShaderMan()->LoadPixelShader(szFileName, &pNewShader))
	{
		g_pA3DErrLog->ErrLog("A3DShader::ChangePixelShader, failed to load pixel shader feom file %s", szFileName);
		return false;
	}

	m_General.pPShader = pNewShader;

	return true;
}

/*	Load shader from file

	Return true for success, otherwise return false.

	pDevice: A3DDevice object's address, if this parameter is NULL, it will be ignored.
	pFile: file which contain shader's information. This file should be open as
		   script file.
*/
bool A3DShader::Load(A3DDevice* pDevice, AFile* pFile)
{
	if( m_bHWITexture ) return true;

	AScriptFile ScriptFile;

	if (!ScriptFile.Open(pFile))
	{
		g_pA3DErrLog->ErrLog("A3DShader::Load, Failed to open script file.");
		return false;
	}

	//	Clear all stage information
	if (m_General.dwMask || m_iNumStage)
		Release();

	if (pDevice)
		m_pA3DDevice = pDevice;

	//	Clear all properties
	memset(m_aStages, 0, sizeof (m_aStages));
	memset(&m_General, 0, sizeof (m_General));

	m_iNumStage	= 0;

	char* szToken = ScriptFile.m_szToken;

	ScriptFile.ResetScriptFile();

	//	Get shader's name
	if (!ScriptFile.GetNextToken(true))
	{
		g_pA3DErrLog->ErrLog("A3DShader::Load, Failed to read shader's name");
		goto Error;
	}

	strcpy(m_szName, szToken);

	//	Search token "{"
	if (!ScriptFile.MatchToken("{", false))
	{
		g_pA3DErrLog->ErrLog("A3DShader::Load, Failed get shader's body");
		goto Error;
	}

	while (ScriptFile.PeekNextToken(true))
	{
		if (!_stricmp(szToken, "}"))		//	End of shader body
		{
			//	Skip '}'
			ScriptFile.GetNextToken(true);
			break;
		}

		if (!_stricmp(szToken, "{"))		//	Is a stage
		{
			if (!LoadStage(&ScriptFile))
			{
				g_pA3DErrLog->ErrLog("A3DShader::Load, Failed parse %d stage", m_iNumStage);
				goto Error;
			}
		}
		else 	//	Is a general properties
		{
			if (!LoadGeneralProperty(&ScriptFile))
				goto Error;
		}
	}

	ScriptFile.Close();
	return true;

Error:

	ScriptFile.Close();
	return false;
}

/*	Load a stage.

	Return true for success, otherwise return false

	pFile: shader script file
*/
bool A3DShader::LoadStage(AScriptFile* pFile)
{
	if (m_iNumStage >= MAXNUM_STAGE)
	{
		g_pA3DErrLog->ErrLog("A3DShader::LoadStage, Too many stages");
		return false;
	}

	m_aStages[m_iNumStage].dwMask = 0;

	//	Search '{'
	if (!pFile->MatchToken("{", false))
		return false;

	while (pFile->PeekNextToken(true))
	{
		if (!_stricmp(pFile->m_szToken, "}"))
		{
			//	Skip '}'
			pFile->GetNextToken(true);
			break;
		}

		if (!LoadStageProperty(pFile))
			return false;
	}

	m_iNumStage++;

	return true;
}

/*	Load a stage property.

	Return true for success, otherwise return false.

	pFile: shader script file
*/
bool A3DShader::LoadStageProperty(AScriptFile* pFile)
{
	char* szToken = pFile->m_szToken;
	bool bValueOK = true;
	char szKey[100];

	SHADERSTAGE* pStage = &m_aStages[m_iNumStage];

	//	Get property keyword
	if (!pFile->GetNextToken(true))
		return false;

	strcpy(szKey, szToken);

	if (!_stricmp(szKey, "texture"))
	{
		//	Get texture type
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			if (!_stricmp(szToken, "TEX_NORMAL"))
				pStage->iTexType = A3DTEX_NORMAL;
			else if (!_stricmp(szToken, "TEX_LIGHTMAP"))
				pStage->iTexType = A3DTEX_LIGHTMAP;
			else if (!_stricmp(szToken, "TEX_BUMPMAP"))
				pStage->iTexType = A3DTEX_BUMPMAP;
			else if (!_stricmp(szToken, "TEX_PROCESS"))
				pStage->iTexType = A3DTEX_PROCESS;
			else if (!_stricmp(szToken, "TEX_DETAIL"))
				pStage->iTexType = A3DTEX_DETAIL;
			else if (!_stricmp(szToken, "TEX_ANIMATION"))
				pStage->iTexType = A3DTEX_ANIMATION;
			else
				bValueOK = false;
		}

		if (!bValueOK)
			goto EndTexture;

		//	Get texture parameter
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			//	Load map file here
			if (pStage->iTexType == A3DTEX_NORMAL)
				ChangeStageTexture(m_iNumStage, pStage->iTexType, (DWORD)&szToken[0]);
			else if (pStage->iTexType == A3DTEX_ANIMATION)
			{
				pStage->iNumTex = atoi(szToken);

				//	Frame time
				pFile->GetNextToken(false);
				pStage->dwFrameTime = atoi(szToken);

				//	File name
				pFile->GetNextToken(false);
				ChangeStageTexture(m_iNumStage, pStage->iTexType, (DWORD)&szToken[0]);
			}
			else
			{
				//	TODO: handle parameter of other type texture
			}
		}

EndTexture:;

	}
	else if (!_stricmp(szKey, "filter"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			if (!_stricmp(szToken, "TEXF_NONE"))
				pStage->dwFilter = A3DTEXF_NONE;
			else if (!_stricmp(szToken, "TEXF_POINT"))
				pStage->dwFilter = A3DTEXF_POINT;
			else if (!_stricmp(szToken, "TEXF_LINEAR"))
				pStage->dwFilter = A3DTEXF_LINEAR;
			else if (!_stricmp(szToken, "TEXF_ANISOTROPIC"))
				pStage->dwFilter = A3DTEXF_ANISOTROPIC;
			else if (!_stricmp(szToken, "TEXF_FLATCUBIC"))
				pStage->dwFilter = A3DTEXF_FLATCUBIC;
			else if (!_stricmp(szToken, "TEXF_GAUSSIANCUBIC"))
				pStage->dwFilter = A3DTEXF_GAUSSIANCUBIC;
			else
				bValueOK = false;

			if (bValueOK)
				pStage->dwMask |= SSMF_FILTER;
		}
	}
	else if (!_stricmp(szKey, "address"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			if (!_stricmp(szToken, "TADDR_WRAP"))
				pStage->dwAddress = A3DTADDR_WRAP;
			else if (!_stricmp(szToken, "TADDR_MIRROR"))
				pStage->dwAddress = A3DTADDR_MIRROR;
			else if (!_stricmp(szToken, "TADDR_CLAMP"))
				pStage->dwAddress = A3DTADDR_CLAMP;
			else if (!_stricmp(szToken, "TADDR_BORDER"))
				pStage->dwAddress = A3DTADDR_BORDER;
			else if (!_stricmp(szToken, "TADDR_MIRRORONCE"))
				pStage->dwAddress = A3DTADDR_MIRRORONCE;
			else
				bValueOK = false;

			if (bValueOK)
				pStage->dwMask |= SSMF_ADDRESS;
		}
	}
	else if (!_stricmp(szKey, "colorop") || !_stricmp(szKey, "alphaop"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			DWORD* pValue, dwMask;
			if (!_stricmp(szKey, "colorop"))
			{
				pValue = &pStage->dwColorOp;
				dwMask = SSMF_COLOROP;
			}
			else
			{
				pValue = &pStage->dwAlphaOp;
				dwMask = SSMF_ALPHAOP;
			}

			if (!_stricmp(szToken, "TOP_DISABLE"))
				*pValue = A3DTOP_DISABLE;
			else if (!_stricmp(szToken, "TOP_SELECTARG1"))
				*pValue = A3DTOP_SELECTARG1;
			else if (!_stricmp(szToken, "TOP_SELECTARG2"))
				*pValue = A3DTOP_SELECTARG2;
			else if (!_stricmp(szToken, "TOP_MODULATE"))
				*pValue = A3DTOP_MODULATE;
			else if (!_stricmp(szToken, "TOP_MODULATE2X"))
				*pValue = A3DTOP_MODULATE2X;
			else if (!_stricmp(szToken, "TOP_MODULATE4X"))
				*pValue = A3DTOP_MODULATE4X;
			else if (!_stricmp(szToken, "TOP_ADD"))
				*pValue = A3DTOP_ADD;
			else if (!_stricmp(szToken, "TOP_ADDSIGNED"))
				*pValue = A3DTOP_ADDSIGNED;
			else if (!_stricmp(szToken, "TOP_ADDSIGNED2X"))
				*pValue = A3DTOP_ADDSIGNED2X;
			else if (!_stricmp(szToken, "TOP_SUBTRACT"))
				*pValue = A3DTOP_SUBTRACT;
			else if (!_stricmp(szToken, "TOP_ADDSMOOTH"))
				*pValue = A3DTOP_ADDSMOOTH;
			else if (!_stricmp(szToken, "TOP_BLENDDIFFUSEALPHA"))
				*pValue = A3DTOP_BLENDDIFFUSEALPHA;
			else if (!_stricmp(szToken, "TOP_BLENDTEXTUREALPHA"))
				*pValue = A3DTOP_BLENDTEXTUREALPHA;
			else if (!_stricmp(szToken, "TOP_BLENDFACTORALPHA"))
				*pValue = A3DTOP_BLENDFACTORALPHA;
			else if (!_stricmp(szToken, "TOP_BLENDTEXTUREALPHAPM"))
				*pValue = A3DTOP_BLENDTEXTUREALPHAPM;
			else if (!_stricmp(szToken, "TOP_BLENDCURRENTALPHA"))
				*pValue = A3DTOP_BLENDCURRENTALPHA;
			else if (!_stricmp(szToken, "TOP_PREMODULATE"))
				*pValue = A3DTOP_PREMODULATE;
			else if (!_stricmp(szToken, "TOP_MODULATEALPHA_ADDCOLOR"))
				*pValue = A3DTOP_MODULATEALPHA_ADDCOLOR;
			else if (!_stricmp(szToken, "TOP_MODULATECOLOR_ADDALPHA"))
				*pValue = A3DTOP_MODULATECOLOR_ADDALPHA;
			else if (!_stricmp(szToken, "TOP_MODULATEINVALPHA_ADDCOLOR"))
				*pValue = A3DTOP_MODULATEINVALPHA_ADDCOLOR;
			else if (!_stricmp(szToken, "TOP_MODULATEINVCOLOR_ADDALPHA"))
				*pValue = A3DTOP_MODULATEINVCOLOR_ADDALPHA;
			else if (!_stricmp(szToken, "TOP_BUMPENVMAP"))
				*pValue = A3DTOP_BUMPENVMAP;
			else if (!_stricmp(szToken, "TOP_BUMPENVMAPLUMINANCE"))
				*pValue = A3DTOP_BUMPENVMAPLUMINANCE;
			else if (!_stricmp(szToken, "TOP_DOTPRODUCT3"))
				*pValue = A3DTOP_DOTPRODUCT3;
			else if (!_stricmp(szToken, "TOP_MULTIPLYADD"))
				*pValue = A3DTOP_MULTIPLYADD;
			else if (!_stricmp(szToken, "TOP_LERP"))
				*pValue = A3DTOP_LERP;
			else
				bValueOK = false;

			if (bValueOK)
				pStage->dwMask |= dwMask;
		}
	}
	else if (!_stricmp(szKey, "colorarg1") || !_stricmp(szKey, "colorarg2") ||
			 !_stricmp(szKey, "alphaarg1") || !_stricmp(szKey, "alphaarg2"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			DWORD* pValue;
			if (!_stricmp(szKey, "colorarg1"))
				pValue = &pStage->dwColorArg1;
			else if (!_stricmp(szKey, "colorarg2"))
				pValue = &pStage->dwColorArg2;
			else if (!_stricmp(szKey, "alphaarg1"))
				pValue = &pStage->dwAlphaArg1;
			else
				pValue = &pStage->dwAlphaArg2;

			//	Argument
			if (!_stricmp(szToken, "TA_CURRENT"))
				*pValue = A3DTA_CURRENT;
			else if (!_stricmp(szToken, "TA_DIFFUSE"))
				*pValue = A3DTA_DIFFUSE;
			else if (!_stricmp(szToken, "TA_SELECTMASK"))
				*pValue = A3DTA_SELECTMASK;
			else if (!_stricmp(szToken, "TA_SPECULAR"))
				*pValue = A3DTA_SPECULAR;
			else if (!_stricmp(szToken, "TA_TEMP"))
				*pValue = A3DTA_TEMP;
			else if (!_stricmp(szToken, "TA_TEXTURE"))
				*pValue = A3DTA_TEXTURE;
			else if (!_stricmp(szToken, "TA_TFACTOR"))
				*pValue = A3DTA_TFACTOR;
			else
				bValueOK = false;

			//	Modifier is optional
			if (pFile->GetNextToken(false))
			{
				if (!_stricmp(szToken, "TA_COMPLEMENT"))
					*pValue |= A3DTA_COMPLEMENT;
				else if (!_stricmp(szToken, "TA_ALPHAREPLICATE"))
					*pValue |= A3DTA_ALPHAREPLICATE;
			}
		}
	}
	else if (!_stricmp(szKey, "colorgen") || !_stricmp(szKey, "alphagen"))
	{
		//	This property is not supported now, just ignore them
	}
	else if (!_stricmp(szKey, "alphacomp"))
	{
		//	This property is not supported now, just ignore them
	}
	else if (!_stricmp(szKey, "bumpmat"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			pStage->fBumpEnvMat00 = (float)atof(szToken);

			pFile->GetNextToken(false);
			pStage->fBumpEnvMat01 = (float)atof(szToken);

			pFile->GetNextToken(false);
			pStage->fBumpEnvMat10 = (float)atof(szToken);

			pFile->GetNextToken(false);
			pStage->fBumpEnvMat11 = (float)atof(szToken);

			pStage->dwMask |= SSMF_BUMPMAT;
		}
	}
	else if (!_stricmp(szKey, "bumpscale"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			pStage->fBumpEnvScale = (float)atof(szToken);
			pStage->dwMask |= SSMF_BUMPSCALE;
		}
	}
	else if (!_stricmp(szKey, "bumpoff"))
	{
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			pStage->fBumpEnvOffset = (float)atof(szToken);
			pStage->dwMask |= SSMF_BUMPOFFSET;
		}
	}
	else if (!_stricmp(szKey, "uvgenfunc"))
	{
		//	This property is not supported now, just ignore them
	}
	else if (!_stricmp(szKey, "protexfunc"))
	{
		//	This property is not supported now, just ignore them
	}
	else if (!_stricmp(szKey, "tcscale"))
	{
		if (!LoadTCMod(pFile, SWO_TCSCALE, pStage))
			bValueOK = false;
	}
	else if (!_stricmp(szKey, "tcscroll"))
	{
		if (!LoadTCMod(pFile, SWO_TCSCROLL, pStage))
			bValueOK = false;
	}
	else if (!_stricmp(szKey, "tcrotate"))
	{
		if (!LoadTCMod(pFile, SWO_TCROTATE, pStage))
			bValueOK = false;
	}
	else if (!_stricmp(szKey, "tcstretch"))
	{
		if (!LoadTCMod(pFile, SWO_TCSTRETCH, pStage))
			bValueOK = false;
	}
	else if (!_stricmp(szKey, "tcturb"))
	{
		if (!LoadTCMod(pFile, SWO_TCTURB, pStage))
			bValueOK = false;
	}
	else	//	Unknown property keyword
	{
		g_pA3DErrLog->ErrLog("A3DShader::LoadStageProperty, shader '%s': unknown stage property: '%s' was ignored", m_szName, szKey);
	}

	if (!bValueOK)
		g_pA3DErrLog->ErrLog("A3DShader::LoadStageProperty, shader '%s': invalid value for stage property '%s' ", m_szName, szKey);

	//	Skip this line
	pFile->SkipLine();

	return true;
}

/*	Load a general property.

	Return true for success, otherwise return false

	pFile: shader script file
*/
bool A3DShader::LoadGeneralProperty(AScriptFile* pFile)
{
	char* szToken = pFile->m_szToken;
	bool bValueOK = true;
	char szKey[100];

	//	Get property keyword
	if (!pFile->GetNextToken(true))
		return false;

	strcpy(szKey, szToken);

	if (!_stricmp(szToken, "depthfunc") || !_stricmp(szToken, "alphacomp"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			DWORD* pValue, dwMask;
			if (!_stricmp(szKey, "depthfunc"))
			{
				pValue = &m_General.dwZFunc;
				dwMask = SGMF_ZFUNC;
			}
			else
			{
				pValue = &m_General.dwAlphaFunc;
				dwMask = SGMF_ALPHACOMP;
			}

			if (!_stricmp(szToken, "CMP_NEVER"))
				*pValue = A3DCMP_NEVER;
			else if (!_stricmp(szToken, "CMP_LESS"))
				*pValue = A3DCMP_LESS;
			else if (!_stricmp(szToken, "CMP_EQUAL"))
				*pValue = A3DCMP_EQUAL;
			else if (!_stricmp(szToken, "CMP_LESSEQUAL"))
				*pValue = A3DCMP_LESSEQUAL;
			else if (!_stricmp(szToken, "CMP_GREATER"))
				*pValue = A3DCMP_GREATER;
			else if (!_stricmp(szToken, "CMP_NOTEQUAL"))
				*pValue = A3DCMP_NOTEQUAL;
			else if (!_stricmp(szToken, "CMP_GREATEREQUAL"))
				*pValue = A3DCMP_GREATEREQUAL;
			else if (!_stricmp(szToken, "CMP_ALWAYS"))
				*pValue = A3DCMP_ALWAYS;
			else
				bValueOK = false;

			if (dwMask == SGMF_ALPHACOMP)
			{
				//	Read reference value
				if (!pFile->GetNextToken(false))
					m_General.iAlphaRef = 128;	//	Default value
				else
					m_General.iAlphaRef = atoi(szToken);
			}

			if (bValueOK)
				m_General.dwMask |= dwMask;
		}
	}
	else if (!_stricmp(szToken, "zenable"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			int iValue = atoi(szToken);
			m_General.bZEnable = iValue ? true : false;
			m_General.dwMask  |= SGMF_ZENABLE;	
		}
	}
	else if (!_stricmp(szToken, "zwriteenable"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			int iValue = atoi(szToken);
			m_General.bZWriteEnable = iValue ? true : false;
			m_General.dwMask |= SGMF_ZWENABLE;
		}
	}
	else if (!_stricmp(szToken, "cullmode"))
	{
		//	Get value
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else
		{
			if (!_stricmp(szToken, "CULL_NONE"))
				m_General.dwCullMode = A3DCULL_NONE;
			else if (!_stricmp(szToken, "CULL_CW"))
				m_General.dwCullMode = A3DCULL_CW;
			else if (!_stricmp(szToken, "CULL_CCW"))
				m_General.dwCullMode = A3DCULL_CCW;
			else
				bValueOK = false;

			if (bValueOK)
				m_General.dwMask |= SGMF_CULLMODE;
		}
	}
	else if (!_stricmp(szToken, "blend"))
	{
		//	Get value
		for (int i=0; i < 2; i++)
		{
			if (!pFile->GetNextToken(false))
			{
				bValueOK = false;
				break;
			}

			DWORD* pValue;
			if (!i)
				pValue = &m_General.dwSrcBlend;
			else
				pValue = &m_General.dwDstBlend;

			if (!_stricmp(szToken, "BLEND_ZERO"))
				*pValue = A3DBLEND_ZERO;
			else if (!_stricmp(szToken, "BLEND_ONE"))
				*pValue = A3DBLEND_ONE;
			else if (!_stricmp(szToken, "BLEND_SRCCOLOR"))
				*pValue = A3DBLEND_SRCCOLOR;
			else if (!_stricmp(szToken, "BLEND_INVSRCCOLOR"))
				*pValue = A3DBLEND_INVSRCCOLOR;
			else if (!_stricmp(szToken, "BLEND_SRCALPHA"))
				*pValue = A3DBLEND_SRCALPHA;
			else if (!_stricmp(szToken, "BLEND_INVSRCALPHA"))
				*pValue = A3DBLEND_INVSRCALPHA;
			else if (!_stricmp(szToken, "BLEND_DESTALPHA"))
				*pValue = A3DBLEND_DESTALPHA;
			else if (!_stricmp(szToken, "BLEND_INVDESTALPHA"))
				*pValue = A3DBLEND_INVDESTALPHA;
			else if (!_stricmp(szToken, "BLEND_DESTCOLOR"))
				*pValue = A3DBLEND_DESTCOLOR;
			else if (!_stricmp(szToken, "BLEND_INVDESTCOLOR"))
				*pValue = A3DBLEND_INVDESTCOLOR;
			else if (!_stricmp(szToken, "BLEND_SRCALPHASAT"))
				*pValue = A3DBLEND_SRCALPHASAT;
			else if (!_stricmp(szToken, "BLEND_BOTHSRCALPHA"))
				*pValue = A3DBLEND_BOTHSRCALPHA;
			else if (!_stricmp(szToken, "BLEND_BOTHINVSRCALPHA"))
				*pValue = A3DBLEND_BOTHINVSRCALPHA;
			else
				bValueOK = false;
		}

		if (bValueOK)
		{
			m_General.dwMask |= SGMF_BLEND;
			m_bAlphaTexture	  = true;
		}
	}
	else if (!_stricmp(szToken, "colorgen"))
	{
		//	Load wave
		if (!LoadWave(pFile, &m_General.rgbGen))
			bValueOK = false;
		else
		{
			m_General.rgbGen.iType = SWO_COLORGEN;
			m_General.dwMask |= SGMF_COLORGEN;
		}
	}
	else if (!_stricmp(szToken, "alphagen"))
	{
		//	Load wave
		if (!LoadWave(pFile, &m_General.AlphaGen))
			bValueOK = false;
		else
		{
			m_General.rgbGen.iType = SWO_ALPHAGEN;
			m_General.dwMask |= SGMF_ALPHAGEN;
		}
	}
	else if (!_stricmp(szToken, "pixelshader"))
	{
		//	Get pixel shader's file name
		if (!pFile->GetNextToken(false))
			bValueOK = false;
		else if (!ChangePixelShader(szToken))
			bValueOK = false;
	}
	else	//	Unknown property keyword
	{
		g_pA3DErrLog->ErrLog("A3DShader::LoadGeneralProperty, shader '%s': unknown general property: '%s' was ignored", m_szName, szKey);
	}

	if (!bValueOK)
		g_pA3DErrLog->ErrLog("A3DShader::LoadGeneralProperty, shader '%s': invalid value for general property '%s' ", m_szName, szKey);

	//	Skip this line
	pFile->SkipLine();

	return true;
}

/*	Load a tc modifier property.

	Return true for success, otherwise return false.

	pFile: script file pointer.
	iType: tc modifier type
	pStage: current stage.	
*/
bool A3DShader::LoadTCMod(AScriptFile* pFile, int iType, SHADERSTAGE* pStage)
{
	char* szToken = pFile->m_szToken;
	int i;

	//	Check whether this property has existed
	for (i=0; i < pStage->iNumtcMod; i++)
	{
		if (pStage->atcMods[i].iType == iType)
			break;
	}

	if (i < pStage->iNumtcMod)	//	The property has existed, overwrite it
		g_pA3DErrLog->ErrLog("A3DShader::LoadTCMod, shader '%s': tc modifier overwrite.", m_szName);
	else if (pStage->iNumtcMod >= MAXNUM_TCMOD)
	{
		g_pA3DErrLog->ErrLog("A3DShader::LoadTCMod, shader '%s': too much tcmods.", m_szName);
		return false;
	}

	SHADERWAVE* pWave = &pStage->atcMods[i];

	switch (iType)
	{
	case SWO_TCSCALE:
	case SWO_TCSCROLL:

		//	u scale or scroll
		if (!pFile->GetNextToken(false))
			return false;

		pWave->fBase = (float)atof(szToken);

		//	v scale or scroll
		if (!pFile->GetNextToken(false))
			return false;

		pWave->fAmp	= (float)atof(szToken);
		break;

	case SWO_TCROTATE:

		//	Rotate value (degree per second)
		if (!pFile->GetNextToken(false))
			return false;

		//	Convert degree to radian
		pWave->fBase = DEG2RAD((float)atof(szToken));
		break;

	case SWO_TCSTRETCH:

		if (!LoadWave(pFile, &pStage->atcMods[pStage->iNumtcMod]))
			return false;
		
		break;

	case SWO_TCTURB:

		//	Amplitude
		if (!pFile->GetNextToken(false))
			return false;

		pWave->fAmp = (float)atof(szToken);

		//	Phase
		if (!pFile->GetNextToken(false))
			return false;

		pWave->fPhase = (float)atof(szToken);

		//	Period
		if (!pFile->GetNextToken(false))
			return false;

		pWave->iPeriod = atoi(szToken);
		break;

	default:
		return false;
	}

	pWave->iType = iType;

	if (i == pStage->iNumtcMod)
		pStage->iNumtcMod++;

	return true;
}

/*	Load a wave function

	Return true for success, otherwise return false.

	pFile: script file pointer.
	pWave: used to receive wave data.
*/
bool A3DShader::LoadWave(AScriptFile* pFile, SHADERWAVE* pWave)
{
	char* szToken = pFile->m_szToken;

	//	Wave function -------------------------
	if (!pFile->GetNextToken(false))
		return false;

	if (!_stricmp(szToken, "W_SIN"))
		pWave->iFunc = SWFT_SIN;
	else if (!_stricmp(szToken, "W_TRIANGLE"))
		pWave->iFunc = SWFT_TRIANGLE;
	else if (!_stricmp(szToken, "W_SQUARE"))
		pWave->iFunc = SWFT_SQUARE;
	else if (!_stricmp(szToken, "W_SAWTOOTH"))
		pWave->iFunc = SWFT_SAWTOOTH;
	else if (!_stricmp(szToken, "W_INVSAWTOOTH"))
		pWave->iFunc = SWFT_INVSAWTOOTH;
	else
	{
		g_pA3DErrLog->ErrLog("A3DShader::LoadWave, unknown wave function: %s", szToken);
		return false;
	}

	//	Baseline value ------------------------
	if (!pFile->GetNextToken(false))
		return false;

	pWave->fBase = (float)atof(szToken);

	//	Amplitude -----------------------------
	if (!pFile->GetNextToken(false))
		return false;

	pWave->fAmp = (float)atof(szToken);

	//	Initial phase -------------------------
	if (!pFile->GetNextToken(false))
		return false;

	pWave->fPhase = (float)atof(szToken);

	//	Frequency -----------------------------
	if (!pFile->GetNextToken(false))
		return false;

	pWave->iPeriod = atoi(szToken);

	return true;
}

//	Tick animation
bool A3DShader::TickAnimation()
{
	if( m_bHWITexture ) return true;

	DWORD dwTime;

	if (!m_dwStartTime)
	{
		m_dwStartTime = A3DGetTime();
		dwTime = 0;
	}
	else
		dwTime = A3DGetTime() - m_dwStartTime;

	//	Color generate
	if (m_General.dwMask & SGMF_COLORGEN)
		m_aFuncVal[VAL_GENCOLORGEN] = CaluWaveValue(m_General.rgbGen, dwTime);

	//	Alpha generate
	if (m_General.dwMask & SGMF_ALPHAGEN)
		m_aFuncVal[VAL_GENALPHAGEN] = CaluWaveValue(m_General.AlphaGen, dwTime);

	int i, j;
	SHADERWAVE* pWave;
	float x = dwTime * 0.001f;

	for (i=0; i < m_iNumStage; i++)
	{
		//	Texture animation
		if (m_aStages[i].iTexType == A3DTEX_ANIMATION)
			m_aCurFrame[i] = (dwTime / m_aStages[i].dwFrameTime) % m_aStages[i].iNumTex;
			
		for (j=0; j < m_aStages[i].iNumtcMod; j++)
		{
			pWave = &m_aStages[i].atcMods[j];

			switch (pWave->iType)
			{
			case SWO_TCSTRETCH:		//	tcmod stretch for every stage

				m_aFuncVal[VAL_TCMODSTRETCH0 + i] = CaluWaveValue(*pWave, dwTime);
				break;

			case SWO_TCSCROLL:

				m_aFuncVal[VAL_TCMODUSCROLL0 + i] = x * pWave->fBase;
				m_aFuncVal[VAL_TCMODVSCROLL0 + i] = x * pWave->fAmp;
				break;

			case SWO_TCROTATE:

				m_aFuncVal[VAL_TCMODROTATE0 + i] = x * pWave->fBase;
				break;

			case SWO_TCTURB:

				CaluTurbulence(*pWave, dwTime, i);
				break; 

			default:
				continue;
			}
		}
	}

	return true;
}

/*	Calculate wave function's value

	Return result.

	Wave: wave function parameters
	dwTime: time in ms
*/
float A3DShader::CaluWaveValue(SHADERWAVE& Wave, DWORD dwTime)
{
	float x, y;

	x  = (float)(dwTime % Wave.iPeriod) / Wave.iPeriod + Wave.fPhase;
	x -= (float)floor(x);		//	Normalize x to 0.0f -- 1.0f

	switch (Wave.iFunc)
	{
	case SWFT_SIN:

		y = g_pA3DMathUtility->SIN(x * 360.0f);
		break;

	case SWFT_TRIANGLE:

		y = (x <= 0.5f) ? x * 2.0f : (1.0f - x) * 2.0f;
		break;

	case SWFT_SQUARE:

		y = (x <= 0.5f) ? 1.0f : -1.0f;
		break;

	case SWFT_INVSAWTOOTH:

		y = 1.0f - x;
		break;

	case SWFT_SAWTOOTH:
	default:

		y = x;
		break;
	}

	return y * Wave.fAmp + Wave.fBase;
}

/*	Calculate turbulence value

	Wave: wave function parameters
	dwTime: time in ms
	iStage: index of stage
*/
void A3DShader::CaluTurbulence(SHADERWAVE& Wave, DWORD dwTime, int iStage)
{
	float x1, fSin1, x2, fSin2;

	x1  = (float)(dwTime % Wave.iPeriod) / Wave.iPeriod + Wave.fPhase;
	x1 -= (float)floor(x1);		//	Normalize x to 0.0f -- 1.0f

	fSin1 = g_pA3DMathUtility->SIN(x1 * 360.0f) * Wave.fAmp;

	int iPeriod = (int)(Wave.iPeriod * 0.99f);
	x2  = (float)(dwTime % iPeriod) / iPeriod + Wave.fPhase;
	x2 -= (float)floor(x2);		//	Normalize x to 0.0f -- 1.0f

	fSin2 = g_pA3DMathUtility->SIN(x2 * 360.0f) * Wave.fAmp;

	m_aFuncVal[VAL_TCMODOUTURB0 + iStage] = -fSin1;
	m_aFuncVal[VAL_TCMODOVTURB0 + iStage] = fSin2;
	m_aFuncVal[VAL_TCMODSUTURB0 + iStage] = 1.0f - fSin1;
	m_aFuncVal[VAL_TCMODSVTURB0 + iStage] = 1.0f + fSin1;
}

/*	Get current value of general color generate.

	Return true if value in pfVal is valid, otherwise return false.

	pfVal (out): used to receive general colorgen value if true is returned.
*/
bool A3DShader::GetCurGenColorGenerate(float* pfVal)
{
	if (m_General.dwMask & SGMF_COLORGEN)
	{
		*pfVal = m_aFuncVal[VAL_GENCOLORGEN];
		return true;
	}

	return false;
}

/*	Get current value of general alpha generate.

	Return true if value in pfVal is valid, otherwise return false.

	pfVal (out): used to receive general alphagen value if true is returned.
*/
bool A3DShader::GetCurGenAlphaGenerate(float* pfVal)
{
	//	Alpha generate
	if (m_General.dwMask & SGMF_ALPHAGEN)
	{
		*pfVal = m_aFuncVal[VAL_GENALPHAGEN];
		return true;
	}

	return false;
}


