#include "A3DEngine.h"
#include "A3DErrLog.h"
#include "A3DCamera.h"
#include "A3DConfig.h"
#include "A3DCounter.h"
#include "A3DFont.h"
#include "A3DFontMan.h"
#include "A3DFrame.h"
#include "A3DGFXMan.h"
#include "A3DLightMan.h"
#include "A3DViewport.h"
#include "A3DRenderTarget.h"
#include "A3DMathUtility.h"
#include "A3DMoxMan.h"
#include "A3DTextureMan.h"
#include "A3DModelMan.h"
#include "A3DSurfaceMan.h"
#include "A3DShaderMan.h"
#include "A3DImgModelMan.h"
#include "A3DWorld.h"
#include "A3DGDI.h"
#include "A3DMeshSorter.h"
#include "A3DMeshCollector.h"
#include "A3DVertexCollector.h"
#include "A3DPlants.h"

A3DEngine::A3DEngine()
{
	m_pD3D				= NULL;
	m_pA3DDevice		= NULL;
	m_pA3DLightMan		= NULL;
	m_pA3DMeshCollector = NULL;
	m_pA3DVertexCollector = NULL;
	m_pA3DMeshSorter	= NULL;
	m_pA3DMoxMan		= NULL;
	m_pA3DModelMan		= NULL;
	m_pA3DSurfaceMan	= NULL;
	m_pA3DTextureMan	= NULL;
	m_pA3DImgModelMan	= NULL;

	m_pA3DFontMan		= NULL;

	m_hInstance			= NULL;
	m_hDeviceWnd		= NULL;
	
	m_pA3DCDS			= NULL;

	m_pA3DWorld			= NULL;
	m_pActiveViewport	= NULL;
	m_pActiveCamera		= NULL;
	
	m_pSky				= NULL;
	m_pTerrain			= NULL;

	m_pRFPSCounter		= NULL;
	m_pTFPSCounter		= NULL;
	m_pConsoleFont		= NULL;

	m_dwEngineTicks		= 0;
	m_bBoxRenderFlag	= false;
	m_bShowFPSFlag		= true;

	m_bBuildingRenderFlag = false;

	m_bFSMotionBlur	    = false;
	m_pMotionBlurRenderTarget = NULL;
	m_nMotionBlurValue	= 128; //0.5f;

	// Debug section;
	m_bShowPerformance	= false;
	strcpy(m_szPerfSectionName[ 0], "DRAWINDEXEDPRIMITIVE");
	strcpy(m_szPerfSectionName[ 1], "ENGINETICKANIMATION ");
	strcpy(m_szPerfSectionName[ 2], "ENGINERENDER        ");
	strcpy(m_szPerfSectionName[ 3], "MESHCOLLECTORRENDER ");
	strcpy(m_szPerfSectionName[ 4], "MESHUPDATELOD       ");
	strcpy(m_szPerfSectionName[ 5], "MESHRENDERTOBUFFER  ");
	strcpy(m_szPerfSectionName[ 6], "MESHRENDER          ");
	strcpy(m_szPerfSectionName[ 7], "ENGINETICK_WORLD    ");
	strcpy(m_szPerfSectionName[ 8], "ENGINETICK_LIST     ");
	strcpy(m_szPerfSectionName[ 9], "WORLD_GFXMAN_RENDER ");
	strcpy(m_szPerfSectionName[10], "WORLD_RAYTRACE      ");
	strcpy(m_szPerfSectionName[11], "WORLD_AABBTRACE     ");
	strcpy(m_szPerfSectionName[12], "ENGINETICK_MEDIA    ");

	strcpy(m_szObjSectionName[0],   "Total Model         ");	
	strcpy(m_szObjSectionName[1],   "Total GFX           "); 	
	strcpy(m_szObjSectionName[2],   "Total Texture       ");
	strcpy(m_szObjSectionName[3],   "Total Sprite        ");
	strcpy(m_szObjSectionName[4],   "Total String        ");	
	strcpy(m_szObjSectionName[5],   "Total Stream        ");	
	ZeroMemory(m_nObjectCount, sizeof(int) * A3DENGINE_MAX_OBJECT_SECTION);
	
	// Angelica Media Engine;
	m_pAMEngine			= NULL;
	m_pAMSoundEngine	= NULL;
	m_pAMVideoEngine	= NULL;

	m_pA3DPlants		= NULL;

	m_curIBLLightParam.dynamicLightParam.Type = A3DLIGHT_FORCE_DWORD;
}

A3DEngine::~A3DEngine()
{
}

bool A3DEngine::Init(HINSTANCE hInstance, HWND hDeviceWnd, A3DDEVFMT * pA3DDevFmt, A3DDEVTYPE devType, bool bAllowMultiThread, bool bForceWindowSwap)
{
#ifdef _DEBUG
	MessageBox(hDeviceWnd, "DEBUG", "", MB_OK);
#endif
	g_pA3DErrLog = new A3DErrLog();
	if (NULL == g_pA3DErrLog) return false;
	if (!g_pA3DErrLog->Init("A3D.log")) return false;

	g_pA3DConfig = new A3DConfig();
	if (NULL == g_pA3DConfig) return false;
	if (!g_pA3DConfig->Init()) return false;

	g_pA3DMathUtility = new A3DMathUtility();
	if (NULL == g_pA3DMathUtility)	return false;
	if (!g_pA3DMathUtility->Init()) return false;

	m_bUseOBBFlag = false;

	// By default we turn on optimization flag, for editors, we should turn off this flag;
	m_bOptimizedEngine = true;
	//m_bFSMotionBlur	   = false;
	//m_nMotionBlurValue = 128; // this is half fade out;

	m_hInstance = hInstance;
	m_hDeviceWnd = hDeviceWnd;
	m_devFmt = *pA3DDevFmt;

	// *****************************************************
	// First we should initialize the angelica media engine;
	// *****************************************************
	m_pAMEngine = new AMEngine();
	if( NULL == m_pAMEngine )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init, Enough memory!");
		return false;
	}

	if( !m_pAMEngine->Init(hDeviceWnd) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init, Init Angelica Media Engine Fail!");
		return false;
	}

	AMSOUNDDEVFORMAT devFormat;
	ZeroMemory(&devFormat, sizeof(devFormat));
	devFormat.nChannels = 2;
	devFormat.nSamplesPerSec = 44100;
	devFormat.wBitsPerSample = 16;
	devFormat.soundCL = AMSCL_EXCLUSIVE;
	if( !m_pAMEngine->InitSoundEngine(&devFormat, true) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init, Init Angelica Sound Engine Fail!");
		return false;
	}

	if( !m_pAMEngine->InitVideoEngine() )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init, Init Angelica Video Engine Fail!");
		return false;
	}

	if( !m_pAMEngine->InitImmWrapper("Default.ifr") )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init, Init Angelica Imm Wrapper Fail!");
		//return false;
	}

	m_pAMSoundEngine = m_pAMEngine->GetAMSoundEngine();
	m_pAMVideoEngine = m_pAMEngine->GetAMVideoEngine();

	// *****************************************
	// Now we initialize the Angelica 3D engine;
	// *****************************************
	m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
	if( NULL == m_pD3D )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init Error Create IDirect3D8 with version %d", D3D_SDK_VERSION);
		return false;
	}

	m_pA3DDevice = new A3DDevice();
	if( NULL == m_pA3DDevice )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DDevice->Init(this, m_hDeviceWnd, pA3DDevFmt, devType, bAllowMultiThread, bForceWindowSwap) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Init A3DDevice Fail!");
		return false;
	}

	// You must first turn on motion blur flag before you can switch motion blur in game;
	if( m_bFSMotionBlur )
	{
		m_pMotionBlurRenderTarget = new A3DRenderTarget();
		if( NULL == m_pMotionBlurRenderTarget )
		{
			g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
			return false;
		}
		if( !m_pMotionBlurRenderTarget->Init(m_pA3DDevice, m_pA3DDevice->GetDevFormat(), true, false) )
		{
			g_pA3DErrLog->ErrLog("A3DEngine::Init() Init motion blur render target fail!");
			return false;
		}
	}

	m_pA3DLightMan = new A3DLightMan();
	if( NULL == m_pA3DLightMan )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DLightMan->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() LightMan Init Fail!");
		return false;
	}

	m_pA3DTextureMan = new A3DTextureMan();
	if( NULL == m_pA3DTextureMan )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DTextureMan->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init TextureMan Init Fail!");
		return false;
	}

	m_pA3DMeshCollector = new A3DMeshCollector();
	if( NULL == m_pA3DMeshCollector )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init Not enough memory!");
		return false;
	}
	if( !m_pA3DMeshCollector->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init A3DMeshCollector Init fail!");
		return false;
	}

	m_pA3DVertexCollector = new A3DVertexCollector();
	if( NULL == m_pA3DMeshCollector )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init Not enough memory!");
		return false;
	}
	if( !m_pA3DVertexCollector->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init A3DVertexCollector Init fail!");
		return false;
	}

	m_pA3DMeshSorter = new A3DMeshSorter();
	if( NULL == m_pA3DMeshSorter )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DMeshSorter->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Init Mesh Sorter Fail!");
		return false;
	}

	m_pA3DFontMan = new A3DFontMan();
	if( NULL == m_pA3DFontMan )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init(), Not enough memory!");
		return false;
	}
	if( !m_pA3DFontMan->Init() )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init(), Init A3DFontMan fail!");
		return false;
	}

	m_pA3DGFXMan = new A3DGFXMan();
	if( NULL == m_pA3DGFXMan )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DGFXMan->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() A3DGFXMan Init Fail!");
		return false;
	}

	m_pA3DMoxMan = new A3DMoxMan();
	if( NULL == m_pA3DMoxMan )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DMoxMan->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init MoxMan Init Fail!");
		return false;
	}

	m_pA3DModelMan = new A3DModelMan();
	if( NULL == m_pA3DModelMan )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DModelMan->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init ModelMan Init Fail!");
		return false;
	}

	m_pA3DSurfaceMan = new A3DSurfaceMan();
	if( NULL == m_pA3DSurfaceMan )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DSurfaceMan->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init SurfaceMan Init Fail!");
		return false;
	}

	m_pA3DShaderMan = new A3DShaderMan();
	if( NULL == m_pA3DShaderMan )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !m_pA3DShaderMan->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init ShaderMan Init Fail!");
		return false;
	}

	m_pA3DImgModelMan = new A3DImageModelMan;
	if (!m_pA3DImgModelMan)
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}

	if (!m_pA3DImgModelMan->Init(m_pA3DDevice))
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init Image model man Init Fail!");
		return false;
	}

	m_ListViewport.Init();
	m_ListRenderTarget.Init();
	m_ListModel.Init();
	
	m_pA3DPlants = new A3DPlants();
	if( NULL == m_pA3DPlants )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init(), Not enough memory!");
		return false;
	}
	if( !m_pA3DPlants->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init(), Init A3DPlants Fail!");
		return false;
	}

	m_pRFPSCounter = new A3DCounter();
	if( NULL == m_pRFPSCounter )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init Not enough memory!");
		return false;
	}
	if( !m_pRFPSCounter->Init() )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init init FPSCounter fail!");
		return false;
	}
	m_pRFPSCounter->ResetFPSCounter();

	m_pTFPSCounter = new A3DCounter();
	if( NULL == m_pTFPSCounter )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init Not enough memory!");
		return false;
	}
	if( !m_pTFPSCounter->Init() )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init init FPSCounter fail!");
		return false;
	}
	m_pTFPSCounter->ResetFPSCounter();

	m_pConsoleFont = new A3DFont();
	if( NULL == m_pConsoleFont )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init Not enough memory!");
		return false;
	}
	HA3DFONT hA3DFont = GetA3DFontMan()->RegisterFont("Courier New", 12, NULL);
	if( !m_pConsoleFont->Init(GetA3DDevice(), hA3DFont, false) )
		return false;
	/*
	if( !m_pConsoleFont->Load(m_pA3DDevice, "ConsoleFont.bmp", 8, 16, 12,24) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init init console font fail!");
		return false;
	}*/

	g_pA3DGDI = new A3DGDI();
	if( NULL == g_pA3DGDI )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Not enough memory!");
		return false;
	}
	if( !g_pA3DGDI->Init(m_pA3DDevice) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init() Init A3DGDI Fail!");
		return false;
	}
	return true;
}

bool A3DEngine::Release()
{
	if( g_pA3DGDI )
	{
		g_pA3DGDI->Release();
		delete g_pA3DGDI;
		g_pA3DGDI = NULL;
	}
	if( m_pConsoleFont )
	{
		m_pConsoleFont->Release();
		delete m_pConsoleFont;
		m_pConsoleFont = NULL;
	}
	if( m_pRFPSCounter )
	{
		m_pRFPSCounter->Release();
		delete m_pRFPSCounter;
		m_pRFPSCounter = NULL;
	}
	if( m_pTFPSCounter )
	{
		m_pTFPSCounter->Release();
		delete m_pTFPSCounter;
		m_pTFPSCounter = NULL;
	}

	if( m_pA3DPlants )
	{
		m_pA3DPlants->Release();
		delete m_pA3DPlants;
		m_pA3DPlants = NULL;
	}

	if( m_pA3DModelMan )
	{
		m_pA3DModelMan->Release();
		delete m_pA3DModelMan;
		m_pA3DModelMan = NULL;
	}
	if( m_pA3DMoxMan )
	{
		m_pA3DMoxMan->Release();
		delete m_pA3DMoxMan;
		m_pA3DMoxMan = NULL;
	}
	if (m_pA3DImgModelMan)
	{
		m_pA3DImgModelMan->Release();
		delete m_pA3DImgModelMan;
		m_pA3DImgModelMan = NULL;
	}
	if( m_pA3DGFXMan )
	{
		m_pA3DGFXMan->Release();
		delete m_pA3DGFXMan;
		m_pA3DGFXMan = NULL;
	}
	if( m_pA3DFontMan )
	{
		m_pA3DFontMan->Release();
		delete m_pA3DFontMan;
		m_pA3DFontMan = NULL;
	}
	if( m_pA3DMeshSorter )
	{
		m_pA3DMeshSorter->Release();
		delete m_pA3DMeshSorter;
		m_pA3DMeshSorter = NULL;
	}
	if( m_pA3DMeshCollector )
	{
		m_pA3DMeshCollector->Release();
		delete m_pA3DMeshCollector;
		m_pA3DMeshCollector = NULL;
	}
	if( m_pA3DVertexCollector )
	{
		m_pA3DVertexCollector->Release();
		delete m_pA3DVertexCollector;
		m_pA3DVertexCollector = NULL;
	}
	if( m_pA3DLightMan )
	{
		m_pA3DLightMan->Release();
		delete m_pA3DLightMan;
		m_pA3DLightMan = NULL;
	}
	if( m_pA3DTextureMan )
	{
		m_pA3DTextureMan->Release();
		delete m_pA3DTextureMan;
		m_pA3DTextureMan = NULL;
	}
	if( m_pA3DSurfaceMan )
	{
		m_pA3DSurfaceMan->Release();
		delete m_pA3DSurfaceMan;
		m_pA3DSurfaceMan = NULL;
	}
	if( m_pA3DShaderMan )
	{
		m_pA3DShaderMan->Release();
		delete m_pA3DShaderMan;
		m_pA3DShaderMan = NULL;
	}
	if( m_pMotionBlurRenderTarget )
	{
		m_pMotionBlurRenderTarget->Release();
		delete m_pMotionBlurRenderTarget;
		m_pMotionBlurRenderTarget = NULL;
	}
	m_ListModel.Release();
	m_ListRenderTarget.Release();
	m_ListViewport.Release();
	if( m_pA3DDevice )
	{
		m_pA3DDevice->Release();
		delete m_pA3DDevice;
		m_pA3DDevice = NULL;
	}
	if( m_pD3D )
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}

	// *****************************************************
	// Now release angelica media engine;
	// *****************************************************
	if( m_pAMEngine )
	{
		m_pAMEngine->Release();
		delete m_pAMEngine;
		m_pAMEngine = NULL;
	}
	if( g_pA3DMathUtility )
	{
		g_pA3DMathUtility->Release();
		delete g_pA3DMathUtility;
		g_pA3DMathUtility = NULL;
	}
	if( g_pA3DConfig )
	{
		g_pA3DConfig->Release();
		delete g_pA3DConfig;
		g_pA3DConfig = NULL;
	}
	if( g_pA3DErrLog )
	{
		g_pA3DErrLog->Release();
		delete g_pA3DErrLog;
		g_pA3DErrLog = NULL;
	}
	return true;
}

bool A3DEngine::BeginRender()
{
	return m_pA3DDevice->BeginRender();
}

bool A3DEngine::Clear()
{
	return m_pA3DDevice->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, A3DCOLORRGB(0, 0, 0), 1.0f, 0);
}

bool A3DEngine::EndRender()
{
	if( m_pA3DMeshCollector )
		m_pA3DMeshCollector->RemoveUnrenderedMeshes();

	if( m_bShowPerformance || (m_bShowFPSFlag && (GetAsyncKeyState(VK_F12) & 0x8000)) )	
		DisplayPerformanceRecord();

	if( m_bShowFPSFlag )
	{
		char szFPS[32];

		sprintf(szFPS, "RFPS: %5.2f", m_pRFPSCounter->GetFPS());
		m_pConsoleFont->TextOut(m_devFmt.nWidth - 8 * strlen(szFPS), 0, szFPS, A3DCOLORRGBA(255, 0, 0, 255));
		sprintf(szFPS, "TFPS: %5.2f", m_pTFPSCounter->GetFPS());
		// Render Font multitimes is time consuming; !!!!!!!!!!!!
		m_pConsoleFont->TextOut(m_devFmt.nWidth - 8 * strlen(szFPS), 16, szFPS, A3DCOLORRGBA(255, 0, 0, 255));
	}

	return m_pA3DDevice->EndRender();
}

bool A3DEngine::RenderImageModels(A3DViewport * pViewport)
{
	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);

	if (m_pA3DImgModelMan && !m_pA3DImgModelMan->Render(pViewport))
		return false;

	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);

	return true;
}

bool A3DEngine::RenderGFX(A3DViewport * pCurrentViewport, int nCategoryMask)
{
	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());

	if( m_pA3DGFXMan && !m_pA3DGFXMan->Render(pCurrentViewport, nCategoryMask) )
		return false;
	
	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);

	if( !FlushVertexCollector(pCurrentViewport) )
		return false;
		
	return true;
}

bool A3DEngine::RenderWorld(A3DViewport * pCurrentViewport)
{
	// Now render the world;
	if( m_pA3DWorld && !m_pA3DWorld->Render(pCurrentViewport) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Render() Render Scene Frame Fail!");
		return false;
	}

	// Now Render Engine Models
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	ALISTELEMENT * pThisModelElement = m_ListModel.GetHead()->pNext;
	while( pThisModelElement != m_ListModel.GetTail() )
	{
		A3DModel * pModel = (A3DModel *) pThisModelElement->pData;
		if( !pModel->Render(pCurrentViewport) )
			return false;
		pThisModelElement = pThisModelElement->pNext;
	}

	return true;
}

bool A3DEngine::BeginCacheAlphaMesh(A3DViewport * pCurrentViewport)
{
	// Now set the current camera pos into mesh sorter;
	m_pA3DMeshSorter->SetCameraPos(pCurrentViewport->GetCamera()->GetPos());

	return true;
}

bool A3DEngine::FlushCachedAlphaMesh(A3DViewport * pCurrentViewport)
{
	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);

	if( m_pA3DMeshSorter && !m_pA3DMeshSorter->Render(pCurrentViewport) )
		return false;

	m_pA3DMeshSorter->RemoveAllMeshes();

	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);
	return true;
}

bool A3DEngine::FlushVertexCollector(A3DViewport * pCurrentViewport)
{
	// For effects, we will render into vertex collector, so we should flush the 
	// collector's buffer here;
	BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());

	if( m_pA3DVertexCollector && !m_pA3DVertexCollector->Flush(pCurrentViewport) )
		return false;

	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);

	EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);
	return true;
}

bool A3DEngine::FlushMeshCollector(A3DViewport * pCurrentViewport)
{
	BeginPerformanceRecord(A3DENGINE_PERFORMANCE_MESHCOLLECTORRENDER);

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());

	if( m_pA3DMeshCollector && !m_pA3DMeshCollector->Render(pCurrentViewport) )
		return false;

	EndPerformanceRecord(A3DENGINE_PERFORMANCE_MESHCOLLECTORRENDER);
	return true;
}

bool A3DEngine::RenderPlants(A3DViewport * pCurrentViewport)
{
	m_pA3DDevice->GetA3DEngine()->BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);

	if( m_pA3DPlants && !m_pA3DPlants->Render(pCurrentViewport) )
		return false;

	m_pA3DDevice->GetA3DEngine()->EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINERENDER);
	return true;
}

bool A3DEngine::RenderScene(A3DViewport * pCurrentViewport)
{
	BeginCacheAlphaMesh(pCurrentViewport);
	RenderWorld(pCurrentViewport);
	RenderPlants(pCurrentViewport);
	FlushCachedAlphaMesh(pCurrentViewport);

	RenderGFX(pCurrentViewport, 0xffffffff);
	return true;
}

bool A3DEngine::Present()
{
	m_pRFPSCounter->UpdateFPSCounter();
	return m_pA3DDevice->Present();
}

bool A3DEngine::RenderAllViewport()
{
	if( m_bFSMotionBlur && m_pMotionBlurRenderTarget )
	{
		if( !m_pA3DDevice->SetRenderTarget(m_pMotionBlurRenderTarget) )
		{
			g_pA3DErrLog->ErrLog("A3DEngine::RenderAllViewport(), m_pA3DDevice->SetRenderTarget Fail!");
			return false;
		}
	}

	ALISTELEMENT * pThis = m_ListViewport.GetFirst();

	while( pThis != m_ListViewport.GetTail() )
	{
		A3DViewport * pViewport = (A3DViewport *)(pThis->pData);
		pViewport->Active();

		if( !pViewport->ClearDevice() )
			return false;
		
		if( !RenderScene(pViewport) )
			return false;

		pThis = pThis->pNext;
	}

	if( m_bFSMotionBlur && m_pMotionBlurRenderTarget )
	{
		if( !m_pA3DDevice->RestoreRenderTarget() )
		{
			g_pA3DErrLog->ErrLog("A3DEngine::RenderAllViewport(), m_pA3DDevice->RestoreRenderTarget Fail!");
			return false;
		}

		//We should use some specified viewport instead of current active viewport;
		GetActiveViewport()->Active();
		m_pMotionBlurRenderTarget->PresentToBack(0, 0);
	}
	
	return true;
}

bool A3DEngine::RenderAllRenderTarget()
{
	if( m_ListRenderTarget.GetSize() == 0 )
		return true;

	ALISTELEMENT * pThis = m_ListRenderTarget.GetFirst();

	while( pThis != m_ListRenderTarget.GetTail() )
	{
		A3DRenderTarget * pRenderTarget = (A3DRenderTarget *)(pThis->pData);
		
		if( !m_pA3DDevice->SetRenderTarget(pRenderTarget) )
		{
			g_pA3DErrLog->ErrLog("A3DEngine::RenderAllRenderTarget() m_pA3DDevice->SetRenderTarget Fail!");
			return false;
		}

		if( pRenderTarget->GetViewNum() > 0 )
		{
			if( !pRenderTarget->RenderAllViewport() )
				goto Failure;
		}
		pThis = pThis->pNext;
	}

	if( !m_pA3DDevice->RestoreRenderTarget() )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::RenderAllRenderTarget() m_pA3DDevice->RestoreRenderTarget Fail!");
		return false;
	}
	return true;

Failure:
	if( !m_pA3DDevice->RestoreRenderTarget() )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::RenderAllRenderTarget() m_pA3DDevice->RestoreRenderTarget Fail!");
		return false;
	}
	return false;
}

bool A3DEngine::TickAnimation()
{
	memset(m_dwTimeUsed, 0, sizeof(DWORD) * A3DENGINE_MAX_PERFORMANCE_SECTION);

	BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICKANIMATION);

	if( m_pSky && !m_pSky->TickAnimation() )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::TickAnimation() tick sky fail!");
		return false;
	}

	BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICK_WORLD);
	if( m_pA3DWorld && !m_pA3DWorld->TickAnimation() )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::TickAnimation() tick frame scene fail!");
		return false;
	}
	EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICK_WORLD);

	//Now update The models	
	BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICK_LIST);
	ALISTELEMENT * pThisModelElement = m_ListModel.GetHead()->pNext;
	while( pThisModelElement != m_ListModel.GetTail() )
	{
		A3DModel * pModel = (A3DModel *) pThisModelElement->pData;
		if( !pModel->TickAnimation() )
			return false;
		pThisModelElement = pThisModelElement->pNext;
	}
	EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICK_LIST);

	if( m_pA3DGFXMan && !m_pA3DGFXMan->TickAnimation() )
		return false;

	if( m_pA3DTextureMan && !m_pA3DTextureMan->TickAnimation() )
		return false;

	//if( m_pA3DShaderMan && !m_pA3DShaderMan->TickAnimation() )
	//	return false;

	m_pTFPSCounter->UpdateFPSCounter();
	m_dwEngineTicks ++;

	EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICKANIMATION);

	BeginPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICK_MEDIA);
	if( m_pAMEngine )
		m_pAMEngine->Tick();
	EndPerformanceRecord(A3DENGINE_PERFORMANCE_ENGINETICK_MEDIA);

	return true;
}

bool A3DEngine::AddViewport(A3DViewport * pViewport, ALISTELEMENT ** ppElement)
{
	if( m_ListViewport.GetSize() == 0 )
		pViewport->Active();
	return m_ListViewport.Append((LPVOID)pViewport, ppElement);
}

bool A3DEngine::RemoveViewport(A3DViewport * pViewport)
{
	return m_ListViewport.Delete((LPVOID) pViewport);
}

bool A3DEngine::AddRenderTarget(A3DRenderTarget * pRenderTarget, ALISTELEMENT ** ppElement)
{
	return m_ListRenderTarget.Append((LPVOID) pRenderTarget, ppElement);
}

bool A3DEngine::RemoveRenderTarget(A3DRenderTarget * pRenderTarget)
{
	return m_ListRenderTarget.Delete((LPVOID) pRenderTarget);
}

bool A3DEngine::AddModel(A3DModel * pModel, ALISTELEMENT ** ppElement)
{
	assert(pModel->GetContainer() == A3DCONTAINER_NULL);
	pModel->SetContainer(A3DCONTAINER_ENGINE);

	return m_ListModel.Append((LPVOID) pModel, ppElement);
}

bool A3DEngine::RemoveModel(A3DModel * pModel)
{
	assert(pModel->GetContainer() == A3DCONTAINER_ENGINE);
	pModel->SetContainer(A3DCONTAINER_NULL);

	return m_ListModel.Delete((LPVOID) pModel);
}

bool A3DEngine::SetWorld(A3DWorld * pWorld)
{
	m_pA3DWorld = pWorld;
	return true;
}

bool A3DEngine::SetSky(A3DSky * pSky)
{
	m_pSky = pSky;
	return true;
}

bool A3DEngine::SetTerrain(A3DTerrain * pTerrain)
{
	m_pTerrain = pTerrain;
	return true;
}

bool A3DEngine::Reset()
{
	ResetWorldRelated();

	//This function is used to release all resources but keep the hardware device unchanged;
	m_ListModel.Reset();
	m_ListRenderTarget.Reset();
	m_ListViewport.Reset();

	m_pA3DPlants->Reset();

	if( m_pA3DModelMan )
	{
		m_pA3DModelMan->Reset();
	}
	if( m_pA3DMoxMan )
	{
		m_pA3DMoxMan->Reset();
	}
	if( m_pA3DLightMan )
	{
		m_pA3DLightMan->Reset();
	}
	if( m_pA3DGFXMan )
	{
		m_pA3DGFXMan->Reset();
	}
	if( m_pA3DTextureMan )
	{
		m_pA3DTextureMan->Reset();
	}
	if( m_pA3DSurfaceMan )
	{
		m_pA3DSurfaceMan->Reset();
	}
	if (m_pA3DImgModelMan)
	{
		m_pA3DImgModelMan->Reset();
	}
	m_pConsoleFont->Release();
	delete m_pConsoleFont;
	m_pConsoleFont = NULL;

	m_pConsoleFont = new A3DFont();
	if( NULL == m_pConsoleFont )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init Not enough memory!");
		return false;
	}
	HA3DFONT hA3DFont = GetA3DFontMan()->RegisterFont("Courier New", 12, NULL);
	if( !m_pConsoleFont->Init(GetA3DDevice(), hA3DFont, false) )
		return false;

	/*
	if( !m_pConsoleFont->Load(m_pA3DDevice, "ConsoleFont.bmp", 8, 16, 12,24) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init init console font fail!");
		return false;
	}*/

	if( m_pAMEngine )
	{
		m_pAMEngine->Reset();
	}
	m_pRFPSCounter->ResetFPSCounter();
	m_pTFPSCounter->ResetFPSCounter();
	m_dwEngineTicks = 0;
	return true;
}

// Reset world related objects;
bool A3DEngine::ResetWorldRelated()
{
	// This function should not be used any more;
	if( m_pA3DMeshCollector )
		m_pA3DMeshCollector->Reset();

	if( m_pA3DVertexCollector )
		m_pA3DVertexCollector->Reset();

	return true;
}									   

bool A3DEngine::DisplayPerformanceRecord()
{
	char			szInfo[64];
	int i;
	int				nDrawVertCount = m_pA3DDevice->GetDrawVertCount();
	int				nDrawFaceCount = m_pA3DDevice->GetDrawFaceCount();
	int				nDrawCount	   = m_pA3DDevice->GetDrawCount();
	for(i=0; i<A3DENGINE_PERFORMANCE_LASTTAG; i++)
	{
		sprintf(szInfo, "%s:%6d", m_szPerfSectionName[i], m_dwTimeUsed[i] / (A3DCounter::GetCPUFrequency() / 1000000));
		m_pConsoleFont->TextOut(400, 100 + 16 * i, szInfo, A3DCOLORRGBA(255, 0, 0, 255));
		m_dwTimeUsed[i] = 0;
	}

	sprintf(szInfo, "%s:%6d/%6d", "Vertex/Face Count   ", nDrawVertCount, nDrawFaceCount);
	m_pConsoleFont->TextOut(400, 100 + 16 * i, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
	i++;

	sprintf(szInfo, "%s:%6d",     "Draw Count          ", nDrawCount);
	m_pConsoleFont->TextOut(400, 100 + 16 * i, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
	i++;

	sprintf(szInfo, "%s:%6d", "GraphicsFX Count    ", m_pA3DGFXMan->GetGFXCount());
	m_pConsoleFont->TextOut(400, 100 + 16 * i, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
	i++;

	if( m_pA3DWorld )
	{
		sprintf(szInfo, "%s:%6d", "World Obj-Mod Count ", m_pA3DWorld->GetObjectModelCount());
		m_pConsoleFont->TextOut(400, 100 + 16 * i, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
		i++;
	}

	sprintf(szInfo, "%s:%6d", "Engine Model Count  ", m_ListModel.GetSize());
	m_pConsoleFont->TextOut(400, 100 + 16 * i, szInfo, A3DCOLORRGBA(0, 255, 0, 255));
	i++;

	int j = i;
	for(j=0; j<A3DENGINE_OBJECT_LASTTAG; j++)
	{
		sprintf(szInfo, "%s:%6d", m_szObjSectionName[j], m_nObjectCount[j]);
		m_pConsoleFont->TextOut(400, 100 + 16 * (j + i), szInfo, A3DCOLORRGBA(0, 255, 255, 255));
	}
	return true;	
}

bool A3DEngine::ResetResource()
{
	ResetWorldRelated();

	// Release all resources but keep some important interface such as viewport, rendertarget and so on...
	// This function is used to release all resources but keep the hardware device unchanged;
	m_ListModel.Reset();
	
	if( m_pA3DPlants )
		m_pA3DPlants->Reset();

	if( m_pA3DModelMan )
	{
		m_pA3DModelMan->Reset();
	}
	if( m_pA3DMoxMan )
	{
		m_pA3DMoxMan->Reset();
	}
	if( m_pA3DGFXMan )
	{
		m_pA3DGFXMan->Reset();
	}
	if( m_pA3DTextureMan )
	{
		m_pA3DTextureMan->Reset();
	}
	if( m_pA3DSurfaceMan )
	{
		m_pA3DSurfaceMan->Reset();
	}
	if (m_pA3DImgModelMan)
	{
		m_pA3DImgModelMan->Reset();
	}
	m_pConsoleFont->Release();
	delete m_pConsoleFont;
	m_pConsoleFont = NULL;

	m_pConsoleFont = new A3DFont();
	if( NULL == m_pConsoleFont )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init Not enough memory!");
		return false;
	}

	HA3DFONT hA3DFont = GetA3DFontMan()->RegisterFont("Courier New", 12, NULL);
	if( !m_pConsoleFont->Init(GetA3DDevice(), hA3DFont, false) )
		return false;

	/*
	if( !m_pConsoleFont->Load(m_pA3DDevice, "ConsoleFont.bmp", 8, 16, 12,24) )
	{
		g_pA3DErrLog->ErrLog("A3DEngine::Init init console font fail!");
		return false;
	}
	*/

	if( m_pAMEngine )
	{
		m_pAMEngine->Reset();
	}
	return true;
}

bool A3DEngine::SetDisplayMode(int nWidth, int nHeight, A3DFORMAT formatTarget, bool bWindowed, DWORD dwFlags)
{
	if( !m_pA3DDevice->SetDisplayMode(nWidth, nHeight, formatTarget, bWindowed, dwFlags) )
		return false;

	if( dwFlags & SDM_WIDTH )
		m_devFmt.nWidth = nWidth;
	if( dwFlags & SDM_HEIGHT )
		m_devFmt.nHeight = nHeight;
	if( (dwFlags & SDM_FORMAT) && !m_devFmt.bWindowed )
		m_devFmt.fmtTarget = formatTarget;
	if( dwFlags & SDM_WINDOW )
		m_devFmt.bWindowed = bWindowed;
	return true;
}

bool A3DEngine::PrecacheAllTextures(A3DViewport * pViewport)
{
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);

	pViewport->Active();

	pViewport->ClearDevice();

	if( m_pA3DTextureMan )
	{
		m_pA3DTextureMan->PrecacheAllTexture();
	}

	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);

	return true;
}