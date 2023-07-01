/*
 * FILE: A3DEngine.h
 *
 * DESCRIPTION: The class standing for the A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DENGINE_H_
#define _A3DENGINE_H_

#include "A3DPlatform.h"
#include "A3DControl.h"
#include "A3DDevice.h"
#include "AList.h"
#include "A3DSky.h"
#include "A3DTerrain.h"
#include "A3DModel.h"
#include "A3DCounter.h"

#include <AMEngine.h>
#include <AMSoundEngine.h>
#include <AMVideoEngine.h>

typedef class A3DCamera * PA3DCamera;
typedef class A3DCDS * PA3DCDS;
typedef class A3DFrame * PA3DFrame;
typedef class A3DFontMan * PA3DFontMan;
typedef class A3DGFXMan * PA3DGFXlMan;
typedef class A3DLightMan * PA3DLightMan;
typedef class A3DMoxMan * PA3DMoxMan;
typedef class A3DTextureMan * PA3DTextureMan;
typedef class A3DModelMan * PA3DModelMan;
typedef class A3DSurfaceMan * PA3DSurfaceMan;
typedef class A3DShaderMan * PA3DShaderMan;
typedef class A3DImageModelMan * PA3DImageModelMan;
typedef class A3DViewport * PA3DViewport;
typedef class A3DRenderTarget * PA3DRenderTarget;
typedef class A3DWorld * PA3DWorld;
typedef class A3DFont * PA3DFont;
typedef class A3DCounter * PA3DCounter;
typedef class A3DMeshCollector * PA3DMeshCollector;
typedef class A3DMeshSorter * PA3DMeshSorter;
typedef class A3DVertexCollector * PA3DVertexCollector;
typedef class A3DPlants * PA3DPlants;

#define A3DENGINE_MAX_PERFORMANCE_SECTION 32

enum A3DENGINE_PERFORMANCE_SECTION_TAG
{
	A3DENGINE_PERFORMANCE_DRAWINDEXEDPRIMITIVE = 0,
	A3DENGINE_PERFORMANCE_ENGINETICKANIMATION = 1,
	A3DENGINE_PERFORMANCE_ENGINERENDER = 2,
	A3DENGINE_PERFORMANCE_MESHCOLLECTORRENDER = 3,
	A3DENGINE_PERFORMANCE_MESHUPDATELOD = 4,
	A3DENGINE_PERFORMANCE_MESHRENDERTOBUFFER = 5,
	A3DENGINE_PERFORMANCE_MESHRENDER = 6,
	A3DENGINE_PERFORMANCE_ENGINETICK_WORLD = 7,
	A3DENGINE_PERFORMANCE_ENGINETICK_LIST = 8,
	A3DENGINE_PERFORMANCE_WORLD_GFXMAN_RENDER = 9,
	A3DENGINE_PERFORMANCE_WORLD_RAYTRACE = 10,
	A3DENGINE_PERFORMANCE_WORLD_AABBTRACE = 11,
	A3DENGINE_PERFORMANCE_ENGINETICK_MEDIA = 12,
	A3DENGINE_PERFORMANCE_LASTTAG = 13
};

#define A3DENGINE_MAX_OBJECT_SECTION	32
enum A3DENGINE_OBJECT_STAT_TAG
{
	A3DENGINE_OBJECT_MODEL			= 0,
	A3DENGINE_OBJECT_GFX			= 1,
	A3DENGINE_OBJECT_TEXTURE		= 2,
	A3DENGINE_OBJECT_SPRITE			= 3, 
	A3DENGINE_OBJECT_STRING			= 4,
	A3DENGINE_OBJECT_STREAM			= 5,
	A3DENGINE_OBJECT_LASTTAG		= 6
};

//This class is the tool class to contain all a3d specified classes;
class A3DEngine : public A3DControl
{
private:
	HWND						m_hDeviceWnd;
	HINSTANCE					m_hInstance;

	//Device Format	record;
	A3DDEVFMT					m_devFmt;

	//Viewports and Cameras List;
	AList						m_ListViewport;

	//Rendertargets List;
	AList						m_ListRenderTarget;

	//Although here is a model list, but this is used for model editor only;
	//For game, we must add all models into A3DWorld;
	//Model List;
	AList						m_ListModel;
	
	//Objects created in this module;
	IDirect3D8 *				m_pD3D;

	A3DDevice *					m_pA3DDevice;
	A3DFontMan *				m_pA3DFontMan;
	A3DGFXMan *					m_pA3DGFXMan;
	A3DLightMan *				m_pA3DLightMan;
	A3DMeshCollector *			m_pA3DMeshCollector;
	A3DVertexCollector *		m_pA3DVertexCollector;
	A3DMeshSorter *				m_pA3DMeshSorter;
	A3DMoxMan *					m_pA3DMoxMan;
	A3DModelMan *				m_pA3DModelMan;
	A3DSurfaceMan *				m_pA3DSurfaceMan;
	A3DTextureMan *				m_pA3DTextureMan;
	A3DShaderMan *				m_pA3DShaderMan;
	A3DImageModelMan *			m_pA3DImgModelMan;

	A3DCDS *					m_pA3DCDS;
	//Object Pointer from other module;
	A3DWorld *					m_pA3DWorld;
	A3DViewport *				m_pActiveViewport;
	A3DCamera *					m_pActiveCamera;

	//Elements of the scene;
	A3DSky *					m_pSky;
	A3DTerrain *				m_pTerrain;

	//Heart Beat control;
	DWORD						m_dwEngineTicks;
	A3DCounter *				m_pRFPSCounter;
	A3DCounter *				m_pTFPSCounter;

	//Whether we should use frame obb also;
	bool						m_bUseOBBFlag;

	//Box render flag;
	bool						m_bBoxRenderFlag;
	bool						m_bShowFPSFlag;

	//whether we should use optimize algorithm for improve the speed;
	bool						m_bOptimizedEngine;

	//Building flag; when rendering a building, this flag will be true; and 
	//then mesh collector will stop working;
	bool						m_bBuildingRenderFlag;

	//motion blur flag;
	A3DRenderTarget *			m_pMotionBlurRenderTarget;
	bool						m_bFSMotionBlur;
	int							m_nMotionBlurValue; // 0~255, corresponding to alpha value remains for each frame;

	//Font object used to show something;
	A3DFont	*					m_pConsoleFont;

	//Debug Section;
	bool						m_bShowPerformance;
	__int64						m_i64TimeStart[A3DENGINE_MAX_PERFORMANCE_SECTION];
	DWORD						m_dwTimeUsed[A3DENGINE_MAX_PERFORMANCE_SECTION];
	char						m_szPerfSectionName[32][A3DENGINE_MAX_PERFORMANCE_SECTION];

	char						m_szObjSectionName[32][A3DENGINE_MAX_OBJECT_SECTION];
	int							m_nObjectCount[A3DENGINE_MAX_OBJECT_SECTION];

	// Angelica Media Engine;
	AMEngine *					m_pAMEngine;
	AMSoundEngine *				m_pAMSoundEngine;
	AMVideoEngine *				m_pAMVideoEngine;

	// Angelica Plants Class;
	A3DPlants *					m_pA3DPlants;

	// Some engine state information
	A3DIBLLIGHTPARAM			m_curIBLLightParam;

protected:
public:
	A3DEngine();
	~A3DEngine();

	bool Init(HINSTANCE hInstance, HWND hDeviceWnd, A3DDEVFMT * pA3DDevFmt, A3DDEVTYPE devType=A3DDEVTYPE_HAL, bool bAllowMultiThread=false, bool bForceWindowSwap=false);
	bool Release();
	bool Reset();
	bool ResetResource();

	// Reset some objects that related with world;
	bool ResetWorldRelated();

	bool SetDisplayMode(int nWidth, int nHeight, A3DFORMAT formatTarget, bool bWindowed, DWORD dwFlags);

	bool BeginRender();
	bool Clear();
	bool EndRender();
	bool RenderScene(A3DViewport * pCurrentViewport);
	bool Present();

	bool PrecacheAllTextures(A3DViewport * pViewport);

	bool RenderAllViewport();
	bool RenderAllRenderTarget();
	
	bool RenderWorld(A3DViewport * pViewport);
	bool RenderPlants(A3DViewport * pViewport);
	bool BeginCacheAlphaMesh(A3DViewport * pViewport);
	bool FlushCachedAlphaMesh(A3DViewport * pViewport);
	bool RenderGFX(A3DViewport * pViewport, int nCategoryMask);
	bool RenderImageModels(A3DViewport * pViewport);
	bool FlushVertexCollector(A3DViewport * pViewport);
	bool FlushMeshCollector(A3DViewport * pViewport);

	bool TickAnimation();

	bool SetWorld(A3DWorld * pWorld);
	bool SetSky(A3DSky * pSky);
	bool SetTerrain(A3DTerrain * pTerrain);

	bool AddViewport(A3DViewport * pViewport, ALISTELEMENT ** ppElement);
	bool RemoveViewport(A3DViewport * pViewport);
	bool AddRenderTarget(A3DRenderTarget * pRenderTarget, ALISTELEMENT ** ppElement);
	bool RemoveRenderTarget(A3DRenderTarget * pRenderTarget);
	bool AddModel(A3DModel * pModel, ALISTELEMENT ** ppElement);
	bool RemoveModel(A3DModel * pModel);
	
	// Debug used performance section;
	inline void BeginPerformanceRecord(A3DENGINE_PERFORMANCE_SECTION_TAG sectionTag);
	inline void EndPerformanceRecord(A3DENGINE_PERFORMANCE_SECTION_TAG sectoinTag);
	inline void IncObjectCount(A3DENGINE_OBJECT_STAT_TAG sectionTag);
	inline void DecObjectCount(A3DENGINE_OBJECT_STAT_TAG sectionTag);
	bool DisplayPerformanceRecord();

	inline IDirect3D8 * GetD3D() { return m_pD3D; }
	inline A3DDevice * GetA3DDevice() { return m_pA3DDevice; }
	inline A3DFontMan * GetA3DFontMan() { return m_pA3DFontMan; }
	inline A3DLightMan * GetA3DLightMan() { return m_pA3DLightMan; }
	inline void	SetActiveViewport(A3DViewport * pViewport) { m_pActiveViewport = pViewport; }
	inline A3DViewport * GetActiveViewport() { return m_pActiveViewport; }
	inline void SetActiveCamera(A3DCamera * pCamera) { m_pActiveCamera = pCamera; }
	inline A3DCamera * GetActiveCamera() { return m_pActiveCamera; }
	inline A3DMeshCollector * GetMeshCollector() { return m_pA3DMeshCollector; }
	inline A3DVertexCollector * GetVertexCollector() { return m_pA3DVertexCollector; }
	inline A3DMeshSorter * GetMeshSorter() { return m_pA3DMeshSorter; }
	inline A3DMoxMan * GetA3DMoxMan() { return m_pA3DMoxMan; };
	inline A3DModelMan * GetA3DModelMan() { return m_pA3DModelMan; }
	inline A3DSurfaceMan * GetA3DSurfaceMan() { return m_pA3DSurfaceMan; }
	inline A3DTextureMan * GetA3DTextureMan() { return m_pA3DTextureMan; }
	inline A3DGFXMan * GetA3DGFXMan() { return m_pA3DGFXMan; }
	inline A3DShaderMan * GetA3DShaderMan() { return m_pA3DShaderMan; }
	inline A3DImageModelMan * GetA3DImageModelMan() { return m_pA3DImgModelMan; }
	inline A3DWorld * GetA3DWorld() { return m_pA3DWorld; }

	inline A3DCDS * GetA3DCDS() { return m_pA3DCDS; }
	inline void SetA3DCDS(A3DCDS * pA3DCDS) { m_pA3DCDS = pA3DCDS; }

	inline DWORD GetEngineTicks() { return m_dwEngineTicks; }
	inline void SetShowFPSFlag(bool bFlag) { m_bShowFPSFlag = bFlag; }
	inline bool GetShowFPSFlag() { return m_bShowFPSFlag; }
	inline bool GetUseOBBFlag() { return m_bUseOBBFlag; }
	inline void SetBoxRenderFlag(bool bFlag) { m_bBoxRenderFlag = bFlag; }
	inline bool GetBoxRenderFlag() { return m_bBoxRenderFlag; }
	inline bool IsOptimizedEngine() { return m_bOptimizedEngine; }
	inline void SetOptimizedEngine(bool bOptimized) { m_bOptimizedEngine = bOptimized; }
	inline bool IsFSMotionBlur() { return m_bFSMotionBlur; }
	inline void SetFSMotionBlur(bool bFSMotionBlur) { m_bFSMotionBlur = bFSMotionBlur; }
	inline int GetMotionBlurValue() { return m_nMotionBlurValue; }
	inline void SetMotionBlurValue(int nMotionBlurValue) { m_nMotionBlurValue = min(255, nMotionBlurValue); }
	inline FLOAT GetFPS() { return m_pTFPSCounter->GetFPS(); }
	inline void SetBuildingRenderFlag(bool bFlag) { m_bBuildingRenderFlag = bFlag; }
	inline bool GetBuildingRenderFlag() { return m_bBuildingRenderFlag; }

	// Angelica Media Engine;
	inline AMEngine * GetAMEngine() { return m_pAMEngine; }
	inline AMSoundEngine * GetAMSoundEngine() { return m_pAMSoundEngine; }
	inline AMVideoEngine * GetAMVideoEngine() { return m_pAMVideoEngine; }

	inline A3DPlants * GetA3DPlants() { return m_pA3DPlants; }

	inline A3DIBLLIGHTPARAM GetCurIBLLightParam() { return m_curIBLLightParam; }
	inline void SetCurIBLLightParam(A3DIBLLIGHTPARAM param) { m_curIBLLightParam = param; }
};

inline void A3DEngine::BeginPerformanceRecord(A3DENGINE_PERFORMANCE_SECTION_TAG sectionTag)
{
	m_i64TimeStart[sectionTag] = A3DCounter::GetCPUCycle();
}

inline void A3DEngine::EndPerformanceRecord(A3DENGINE_PERFORMANCE_SECTION_TAG sectionTag)
{
	m_dwTimeUsed[sectionTag] += (DWORD)(A3DCounter::GetCPUCycle() - (DWORD)m_i64TimeStart[sectionTag]);
}

inline void A3DEngine::IncObjectCount(A3DENGINE_OBJECT_STAT_TAG sectionTag)
{
	m_nObjectCount[sectionTag] ++;
}

inline void A3DEngine::DecObjectCount(A3DENGINE_OBJECT_STAT_TAG sectionTag)
{
	m_nObjectCount[sectionTag] --;
}

typedef A3DEngine * PA3DEngine;

#endif
