/*
 * FILE: A3DShader.h
 *
 * DESCRIPTION: Routines for shader
 *
 * CREATED BY: duyuxin, 2002/1/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSHADER_H_
#define _A3DSHADER_H_

#include "A3DFuncs.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DPixelShader.h"
#include "AScriptFile.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	General Properties Mask Flags
#define SGMF_ZFUNC			0x00000001		//	dwZFunc
#define SGMF_ZENABLE		0x00000002		//	bZEnable
#define SGMF_ZWENABLE		0x00000004		//	bWriteEnable
#define SGMF_ALPHACOMP		0x00000008		//	dwAlphaFunc and fAlphaRef
#define SGMF_CULLMODE		0x00000010		//	dwCullMode
#define SGMF_BLEND			0x00000020		//	dwSrcBlend and dwDstBlend
#define SGMF_COLORGEN		0x00000040		//	rgbGen
#define SGMF_ALPHAGEN		0x00000080		//	AlphaGen

//	Shader Stage Mask Flags
#define SSMF_FILTER			0x00000001		//	dwFilter
#define SSMF_ADDRESS		0x00000002		//	dwAddress
#define SSMF_COLOROP		0x00000004		//	dwColorOp, dwColorArg1 and dwColorArg2
#define SSMF_ALPHAOP		0x00000008		//	dwAlphaOp, dwAlphaArg1 and dwAlphaArg2
#define SSMF_COLORGEN		0x00000010		//	r, g, b
#define SSMF_ALPHAGEN		0x00000020		//	a
#define SSMF_ALPHACOMP		0x00000040		//	dwAlphaFunc and fAlphaRef
#define SSMF_BUMPMAT		0x00000080		//	fBumpEnvMat00, 01, 10, 11
#define SSMF_BUMPSCALE		0x00000100		//	fBumpEnvScale
#define SSMF_BUMPOFFSET		0x00000200		//	fBumpEnvOffset

//	Objects wave applied to (Shader Wave Object)
#define SWO_NONE			0		//	None
#define SWO_COLORGEN		1		//	Color generate
#define SWO_ALPHAGEN		2		//	Alpha generate
#define SWO_TCSCALE			3		//	Texture Coordinates scale
#define SWO_TCSCROLL		4		//	Texture Coordinates scroll
#define SWO_TCROTATE		5		//	Texture Coordinates rotate
#define SWO_TCSTRETCH		6		//	Texture Coordinates stretch
#define SWO_TCTURB			7		//	Texture Coordinates turbulence

//	Shader Wave Function Type
#define SWFT_NONE			0		//	None
#define SWFT_SIN			1		//	Sin wave
#define SWFT_TRIANGLE		2		//	Triangle wave
#define SWFT_SQUARE			3		//	Square wave
#define SWFT_SAWTOOTH		4		//	Sawtooth
#define SWFT_INVSAWTOOTH	5		//	Inverse sawtooth

//	Maxinum number of animation texture
#define MAXNUM_ANIMTEXTURE	32

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

typedef void (*LPFNUVGENERATE)(float* pu, float* pv);	//	U, V generate or transform function
typedef void (*LPFNPROCESSTEXTURE)();		//	Process texture function

//	Wave function used by shader
typedef struct _SHADERWAVE
{
	int			iType;			//	Wave applied for what ?
	int			iFunc;			//	Wave function
	float		fBase;			//	Wave base
	float		fAmp;			//	Amplitude
	float		fPhase;			//	Initial phase
	int			iPeriod;		//	Period (ticks)

} SHADERWAVE, *PSHADERWAVE;

//	General properties
typedef struct _SHADERGENERAL
{
	DWORD		dwMask;			//	Mask flags
	
	DWORD		dwZFunc;		//	Depth compare function
	bool		bZEnable;		//	Depth compare enable
	bool		bZWriteEnable;	//	Depth write enable
	DWORD		dwAlphaFunc;	//	Alpha compare function
	int			iAlphaRef;		//	Alpha reference value
	DWORD		dwCullMode;		//	Cull mode
	DWORD		dwSrcBlend;		//	Source blend factor
	DWORD		dwDstBlend;		//	Destination blend factor

	A3DPixelShader*	pPShader;	//	Pixel shader

	SHADERWAVE	rgbGen;			//	Color generator
	SHADERWAVE	AlphaGen;		//	Alpha generator
	
} SHADERGENERAL, *PSHADERGENERAL;

//	Stage properties
typedef struct _SHADERSTAGE
{
	DWORD		dwMask;			//	Mask flags
	DWORD		dwStates;		//	States
	
	//	Texture properties
	int			iTexType;		//	Texture type
	A3DTexture*	aTextures[MAXNUM_ANIMTEXTURE];	//	Texture pointers
	int			iNumTex;		//	Number of animation texture
	DWORD		dwFrameTime;	//	Frame time (ms)
	DWORD		dwFilter;		//	Magnification and minification filter
	DWORD		dwAddress;		//	Texture-addressing method for U and V
	
	//	Color and alpha
	DWORD		dwColorOp;		//	Color operation
	DWORD		dwColorArg1;	//	Color argument 1
	DWORD		dwColorArg2;	//	Color argument 2
	DWORD		dwAlphaOp;		//	Alpha operation
	DWORD		dwAlphaArg1;	//	Alpha argument 1
	DWORD		dwAlphaArg2;	//	Alpha argument 2
	float		r, g, b, a;		//	Color and alpha generator
	DWORD		dwAlphaFunc;	//	Alpha compare function
	int			iAlphaRef;		//	Alpha reference value
	
	//	Bump-mapping
	float		fBumpEnvMat00;	//	Bump-mapping matrix
	float		fBumpEnvMat01;
	float		fBumpEnvMat10;
	float		fBumpEnvMat11;
	float		fBumpEnvScale;	//	Scale value for bump-mapping
	float		fBumpEnvOffset;	//	Offset value for bump-mapping
	
	//	Process functions
	int			iUVGenFunc;		//	U, V generate or transform function's index, -1 means null
	int			iProTexFunc;	//	Process texture function's index, -1 means null

	int			iNumtcMod;		//	Number of TC modifier
	SHADERWAVE	atcMods[4];		//	Texture Coordinate modifier
	
} SHADERSTAGE, *PSHADERSTAGE;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DShader
//
///////////////////////////////////////////////////////////////////////////

class A3DShader : public A3DTexture
{
public:		//	Types

	enum
	{
		MAXNUM_STAGE		= 8,	//	Maximum number of stage
		MAXLEN_NAME			= 64,	//	Maximum number of shader name
		MAXNUM_TCMOD		= 5		//	Maximum number of TC modifiers
	};

	//	Dynamic function values
	enum
	{
		VAL_GENCOLORGEN		= 0,	//	General color generate
		VAL_GENALPHAGEN		= 1,	//	General alpha generate
		VAL_TCMODSTRETCH0	= 2,	//	tcmod stretch for stage 0 -- 8
		VAL_TCMODUSCROLL0	= 10,	//	tcmod u scroll for stage 0 -- 8
		VAL_TCMODVSCROLL0	= 18,	//	tcmod v scroll for stage 0 -- 8
		VAL_TCMODROTATE0	= 26,	//	tcmod rotate for stage 0 -- 8
		VAL_TCMODOUTURB0	= 34,	//	tcmod turbulence u offset for stage 0 -- 8
		VAL_TCMODOVTURB0	= 42,	//	tcmod turbulence v offset for stage 0 -- 8
		VAL_TCMODSUTURB0	= 50,
		VAL_TCMODSVTURB0	= 58,

		VAL_NUMBER			= 66	//	Number of value
	};

	//	State flags of stage
	enum
	{
		STGF_HIDE		= 0x0001	//	Stage is hidden
	};

public:		//	Constructors and Destructors

	A3DShader();
	virtual ~A3DShader();

public:		//	Attributes

public:		//	Operations

	virtual bool	Init(A3DDevice* pDevice);	//	Initialize object
	virtual bool	Release();					//	Release object

	virtual bool	Appear(int iLayer=0);		//	Apply shader
	virtual bool	Disappear(int iLayer=0);	//	Abolish shader
	virtual bool	TickAnimation();			//	Tick animation

	bool	Load(A3DDevice* pDevice, AFile* pFile);	//	Load shader from file

	void	SetName(char* szName)		{	strcpy(m_szName, szName);		}
	void	GetName(char* szNameBuf)	{	strcpy(szNameBuf, m_szName);	}
	int		GetStageNum()				{	return m_iNumStage;				}

	SHADERGENERAL	GetGeneralProps()		{	return m_General;			}
	SHADERSTAGE		GetStage(int iStage)	{	return m_aStages[iStage];	}

	void	SetGeneralProps(SHADERGENERAL& Props)	{	m_General = Props;	}
	void	ClearGeneralProps();							//	Clear general properties
	bool	SetStage(int iStage, SHADERSTAGE& Stage);		//	Set a stage's data
	bool	InsertStage(int iIndex, SHADERSTAGE& Stage);	//	Insert a stage
	void	RemoveStage(int iIndex);						//	Remove a stage
	bool	GetCurGenColorGenerate(float* pfVal);			//	Get general colorgen value
	bool	GetCurGenAlphaGenerate(float* pfVal);			//	Get general alphagen value

	bool	ChangeStageTexture(int iStage, int iType, DWORD dwTex);		//	Change a stage's texture
	bool	ChangePixelShader(char* szFileName);	//	Change pixel shader

	//	Stage states
	bool	StageIsVisible(int iStage)		{	return (m_aStages[iStage].dwStates & STGF_HIDE) ? false : true;	}

	inline void	ShowStage(int iStage, bool bShow);		//	Show or hide stage

protected:	//	Attributes

	static SHADERGENERAL	m_DefGeneral;		//	Default general properties
	static SHADERSTAGE		m_DefStage;			//	Default stage properties

	A3DDevice*		m_pA3DDevice;		//	A3D device object

	char			m_szName[MAXLEN_NAME];		//	Name of shader
	SHADERGENERAL	m_General;					//	General properties
	SHADERSTAGE		m_aStages[MAXNUM_STAGE];	//	Stages
	int				m_iNumStage;				//	Number of stage
	int				m_iMaxNumStage;				//	Maximum number of stage

	DWORD			m_dwStartTime;				//	Start time
	float			m_aFuncVal[VAL_NUMBER];		//	Dynamic function value
	int				m_aCurFrame[MAXNUM_STAGE];	//	Current frame of every stage

protected:	//	Operations

	bool	LoadStage(AScriptFile* pFile);			//	Load a stage
	bool	LoadStageProperty(AScriptFile* pFile);	//	Load a stage property
	bool	LoadGeneralProperty(AScriptFile* pFile);	//	Load a general property

	bool	LoadTCMod(AScriptFile* pFile, int iType, SHADERSTAGE* pStage);	//	Load a tc modifier property
	bool	LoadWave(AScriptFile* pFile, SHADERWAVE* pWave);	//	Load a wave function

	void	ApplyGeneral();							//	Apply general properties
	void	ApplyDefGeneral();						//	Apply default general properties
	void	ApplyStage(int iStage, int iIndex);		//	Apply one stage
	void	ApplyDefStage(int iStage, int iIndex);	//	Apply default stage properties for a stage

	float	CaluWaveValue(SHADERWAVE& Wave, DWORD dwTime);	//	Calculate wave function value
	void	CaluTurbulence(SHADERWAVE& Wave, DWORD dwTime, int iStage);	//	Calculate turbulence value

	void	ReleaseStageTexture(int iStage);		//	Release a stage's texture
	void	ReleasePixelShader();					//	Release pixel shader
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

/*	Show or hide specified stage.

	iStage: specified stage.
	bShow: true, show stage, false, hide stage.
*/
void A3DShader::ShowStage(int iStage, bool bShow)
{
	if (!bShow)
		m_aStages[iStage].dwStates |= STGF_HIDE;
	else
		m_aStages[iStage].dwStates &= ~STGF_HIDE;
}


#endif	//	_A3DSHADER_H_


