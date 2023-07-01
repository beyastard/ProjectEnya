#include "AGame.h"
#include "AbaseA3D.h"
#include "AProcessor.h"
#include "ADeviceMan.h"
#include "ADeviceMouse.h"
#include "ADeviceKeyboard.h"
#include "A3DAssistTools.h"
#include "AGamePlay.h"
#include "AWorld.h"
#include "ADebug.h"
#include <assert.h>
//#include <mtime.h>
#include "jexception.h"
#include <time.h>
#include <A3DMeshSorter.h>

IMPLEMENT_ASERIAL(AGame,AProcessor,1)

AGame * g_pAGame = NULL;

AGame::AGame()
{
	m_bIsWindow			= true;
	m_logfps			= 30;
	m_bFastPossible		= true;
	m_fRunSpan			= 1000.f/m_logfps;//毫秒
	m_bActive			= true;
	m_nWidth			= 800;
	m_nHeight			= 600;

	m_hWnd				= NULL;
	m_hInstance			= NULL;
	m_pA3DViewport		= NULL;
	m_pA3DLight			= NULL;
	m_pA3DCamera		= NULL;
	m_pA3DEngine		= NULL;
	m_pADeviceMan		= NULL;
	m_pAGamePlay		= NULL;
	m_pAMouse			= NULL;
	m_pAKeyBoard		= NULL;
	m_pA3DAssistTools	= NULL;
	m_pAWinMsgDevice	= NULL;
	m_pA3DRain			= NULL;
	m_pA3DSnow			= NULL;
	m_pA3DRainSplash	= NULL;
	m_pA3DDynamicLight	= NULL;
	m_bReadFromPck		= false;
	m_nDepth			= -1;
}

AGame::~AGame()
{

}

bool AGame::Init(PINITAGAMESTRUCT pInitStruct)
{
	srand( (unsigned)time( NULL ) );

	init_exception_stack(1);

	J_EXCEPTION_RECORD(0);

	g_pAGame = this;

	if(!InitA3DEngine(pInitStruct))
	{
		ADebug::Msg(0,"Init Engine error");
		return false;
	}

	if(!InitDevice(false))
	{
		ADebug::Msg(0,"Init Device error");
		return false;
	}

	if(!InitA3DTools())
	{
		ADebug::Msg(0,"Init A3DTools error");
		return false;
	}
	if(!InitRainSnow())
	{
		ADebug::Msg(0,"Init  RainSnow error");
		return false;
	}
	if(!InitGameCore())
	{
		ADebug::Msg(0,"Init  GameCore error");
		return false;
	}
		

	ADebug::Msg(0,"AGame Init ok");
	
	return true;
}

int AGame::Run()
{
	//显示片头动画等一次性信息
	long lBeginTime,lEndTime,lSpanTime;
	int nRet = EXIT_CODE_NULL;

	while(true)
	{
		lBeginTime = timeGetTime();
		if(!DispatchWndMsg())
		{
			nRet = EXIT_CODE_NORMAL;
			break;
		}	
		if(OnRunQueryActive())
		{
			if(!m_pAGamePlay)
			{
				ADebug::Msg(0,"AGamePlay object is NULL ");
				break;
			}
			else 
			{
				if(!m_pAGamePlay->Run(nRet,m_fRunSpan))
					break;
			}
		}	
		//限桢
		lEndTime = timeGetTime();
		lSpanTime = lEndTime - lBeginTime;
		//毫秒
		m_fRunSpan	= lSpanTime/1000.f;
		if(lSpanTime <= (1000000/m_logfps+1) && m_bFastPossible == false)
			Sleep((1000000/m_logfps - lSpanTime+1)/1000);
	}
	return nRet;
}

bool AGame::Release()
{
	ReleaseGameCore();
	ReleaseRainSnow();
	ReleaseA3DTools();
	ReleaseDevice();
	ReleaseA3DEngine();
	finalize_exception_stack();
	g_pAGame = NULL;	
	ADebug::Msg(0,"AGame Release Ok ");
	return true;
}

bool AGame::DispatchWndMsg()
{
	MSG msg;
	while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		if(msg.message == WM_QUIT)
		{
			//Release();
			return false;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			OnDispatchWndMsg(&msg);
		}
	}
	return true;
}

int AGame::OnDispatchWndMsg(MSG * pMSG)
{
	if(m_pAWinMsgDevice)
		m_pAWinMsgDevice->FilterWinMsg(pMSG);
	return 0;
}

bool AGame::InitA3DEngine(PINITAGAMESTRUCT pInitStruct)
{
	AFileMod_Initialize();
	AUString strPckFileName;
	QueryPckFileName(strPckFileName);
	if(!strPckFileName.IsEmpty())
	{
		ADebug::Msg(0,"Use Pck File: %s ",strPckFileName);
	//	AFileMod_SetBaseDir("E:\\dev\\Product\\bin");
		bool bRet = OpenFilePackage(strPckFileName);
		if(!bRet)
		{
			ADebug::Msg(0,"Open Pck File error: %s ",strPckFileName);
			return false;
		}
		m_bReadFromPck = true;
	}
	m_pA3DEngine = new A3DEngine();
	if( NULL == m_pA3DEngine ) 
	{
		ADebug::Msg(0,"new A3DEngine error");
		return false;
	}
	
	A3DDEVFMT devFmt;
	ZeroMemory(&devFmt, sizeof(devFmt));
	devFmt.bWindowed	= pInitStruct->InitA3DEngine.bIsWindow;
	devFmt.nWidth		= pInitStruct->InitA3DEngine.nWidth;
	devFmt.nHeight		= pInitStruct->InitA3DEngine.nHeight;
	m_hWnd				= pInitStruct->InitA3DEngine.hWnd;
	m_hInstance			= pInitStruct->InitA3DEngine.hInstance;

	SetClientWidth(devFmt.nWidth);
	SetClientHeight(devFmt.nHeight);

	if( !m_pA3DEngine->Init(pInitStruct->InitA3DEngine.hInstance ,
							pInitStruct->InitA3DEngine.hWnd , 
							&devFmt, 
							A3DDEVTYPE_HAL, true, false) )
	{
		ADebug::Msg(0,"m_pA3DEngine->Init() error");
		return false;
	}
	ADebug::Msg(0,"A3DEngine has been Inited");

	//init texture
	g_pA3DConfig->SetFlagHQTexture(false);

	//Set Render Parameters;
	m_pA3DCamera = new A3DCamera();
	if( NULL == m_pA3DCamera ) 
	{
		ADebug::Msg(0,"new A3DCamera error");
		return false;
	}

	if( !m_pA3DCamera->Init(m_pA3DEngine->GetA3DDevice()
		, DEG2RAD(50.0f), 0.1f, 3000.0f) )
	{
		ADebug::Msg(0,"Init A3DCamera error");
		return false;
	}

	m_pA3DCamera->SetPos(A3DVECTOR3(0.0f, 4.0f, 0.0f));
	m_pA3DCamera->SetDirAndUp(A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pA3DCamera->SetFOV(DEG2RAD(70.0f));
	m_pA3DCamera->SetFOVSet(DEG2RAD(70.0f));
	m_pA3DCamera->SetAM3DSoundDevice(m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice());
	
	//Main View;
	static	const A3DCOLOR	DFT_AMB_COLOR		= (A3DCOLORRGB(126, 88, 67));
	static	const A3DCOLOR	DFT_FOG_COLOR		= (A3DCOLORRGB(147, 159, 186));
	static	const A3DCOLOR	DFT_CLEAR_COLOR		= (A3DCOLORRGB(0, 0, 128));

	if( !m_pA3DEngine->GetA3DDevice()->CreateViewport(&m_pA3DViewport,
		0, 0, GetClientWidth(),GetClientHeight(), 0.0f, 1.0f,
		true, true, DFT_CLEAR_COLOR) )
	{
		ADebug::Msg(0,"CreateViewport() error");
		return false;
	}
	m_pA3DViewport->SetCamera(m_pA3DCamera);
	m_pA3DEngine->AddViewport(m_pA3DViewport, NULL);
	m_pA3DEngine->GetA3DDevice()->SetFogColor(DFT_FOG_COLOR);

	//Add a directional light;
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(
		1, &m_pA3DLight, A3DVECTOR3(0.0f, 0.0f, 0.0f), A3DVECTOR3(-1.0f, -1.0f, 1.0f),
		A3DCOLORVALUE(255.0f/255.0f, 242.0f/255.0f, 220.0f/255.0f, 1.0f),	// Diffuse
		A3DCOLORVALUE(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f, 1.0f),	// Specular
		A3DCOLORVALUE(0.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 1.0f));		// Ambient
	if(!m_pA3DLight)
	{
		ADebug::Msg(0,"Create DirectionLight error");
		return false;
	}
	m_pA3DEngine->GetA3DDevice()->SetAmbient(DFT_AMB_COLOR);

/*	m_pA3DEngine->GetA3DLightMan()->CreatePointLight(
		2,&m_pA3DDynamicLight,A3DVECTOR3(0.0f, 0.0f, 0.0f),A3DCOLORVALUE(1.0f,1.0f,1.0f,1.0f),
		A3DCOLORVALUE(1.0f,1.0f,1.0f,1.0f),A3DCOLORVALUE(1.0f,1.0f,1.0f,1.0f),10.f,1.0f);

	if(!m_pA3DDynamicLight)
	{
		ADebug::Msg(0,"Create DynamicLight error");
		return false;
	}

	m_pA3DDynamicLight->TurnOff();
*/
	m_pA3DEngine->GetMeshSorter()->SetIBLLight(m_pA3DLight, NULL/*m_pA3DDynamicLight*/);
	m_bIsWindow = pInitStruct->InitA3DEngine.bIsWindow;

#ifdef _DEBUG
	m_pA3DEngine->SetShowFPSFlag(true);
#else
	m_pA3DEngine->SetShowFPSFlag(false);
#endif	

	ADebug::Msg(0,"AGame Init Ok");
	return true;
}

bool AGame::InitDevice(bool bWindow)
{
	//产生ADeviceMan和ADevice并挂在ADeviceMan上
	//new 它们的派生类
 	m_pADeviceMan = new ADeviceMan;

	if(!m_pADeviceMan)
	{
		assert(0);
		ADebug::Msg(0,"new ADeviceMan error");
		return false;
	}

	ADEVICEMOUSE_INFO_T mouseInfo;
	memset(&mouseInfo,0,sizeof(ADEVICEMOUSE_INFO_T));
	mouseInfo.hInst			= m_hInstance;
	mouseInfo.hWnd			= m_hWnd;
	mouseInfo.nLeft			= 0;
	mouseInfo.nTop			= 0;
	mouseInfo.nBottom		= m_nHeight;
	mouseInfo.nRight		= m_nWidth;
	mouseInfo.bWindow		= bWindow;
	
	m_pAMouse = NewADeviceMouse(&mouseInfo);
	if(!m_pAMouse)
	{
		ADebug::Msg(0,"new AMouse error");
		assert(0);
		return false;
	}

	m_pAKeyBoard = NewADeviceKeyboard();
	if(!m_pAKeyBoard)
	{
		ADebug::Msg( 0,"new AkeyBoard error");
		assert(0);
		return false;
	}

	//挂设备
	m_pADeviceMan->AddDevice(m_pAMouse);
	m_pADeviceMan->AddDevice(m_pAKeyBoard);
	
	m_pAWinMsgDevice = new AWinMsgDeviceGrp;
	if(!m_pAWinMsgDevice)
	{
		ADebug::Msg(0,"new AWinMsgDeviceGrp error");
		return true;
	}
	//挂设备
	m_pAWinMsgDevice->AddDevice(m_pAMouse);
	m_pAWinMsgDevice->AddDevice(m_pAKeyBoard);
	return true;
}

bool AGame::InitGameCore()
{
	//new AGamePlay object
	m_pAGamePlay = new AGamePlay ;
	if(!m_pAGamePlay)
	{
		ADebug::Msg(0,"new AGamePlay error");
		return false;
	}
	if(!m_pAGamePlay->Init(this))
	{
		ADebug::Msg(0,"Init AGamePlay error");
		return false;
	}
	return true;
}

bool AGame::ReleaseA3DEngine()
{
	RELEASE(m_pA3DViewport);
	m_pA3DEngine->GetA3DLightMan()->ReleaseLight(m_pA3DLight);
	m_pA3DLight = NULL;
//	m_pA3DEngine->GetA3DLightMan()->ReleaseLight(m_pA3DDynamicLight);
//	m_pA3DDynamicLight = NULL;
	RELEASE(m_pA3DCamera);
	RELEASE(m_pA3DEngine);
	CloseFilePackage();
	AFileMod_Finalize();
	return true;
}

bool AGame::ReleaseDevice()
{
	if(m_pADeviceMan)
	{
		m_pADeviceMan->RemoveAllDevice();
		delete m_pADeviceMan;
		m_pADeviceMan = NULL;
	}
	if(m_pAMouse)
	{
		delete m_pAMouse;
		m_pAMouse = NULL;
	}
	if(m_pAKeyBoard)
	{
		delete m_pAKeyBoard;
		m_pAKeyBoard = NULL;
	}
	if(m_pAWinMsgDevice)
	{
		delete m_pAWinMsgDevice;
		m_pAWinMsgDevice = NULL;
	}
	return true;
}

bool AGame::ReleaseGameCore()
{
	RELEASE(m_pAGamePlay)
	return true;
}

bool AGame::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_ACTIVATEAPP:
			m_bActive = (wParam ? true : false);
			// 对于全屏模式才需要下面的操作
			if(!m_bIsWindow)
			{
				if(m_bActive)
				{
					if(GetA3DEngine())
					{
						GetA3DEngine()->GetA3DDevice()->RestoreCurrentDisplayMode();
					}
				}
				else
				{
					if(GetA3DEngine())
					{
						//ShowWindow(hWnd, SW_MINIMIZE);
						//GetA3DEngine()->GetA3DDevice()->RestoreDesktopDisplayMode();
					}
				}
			}
			break;
		case WM_DESTROY:
		case WM_QUIT:
		case WM_CLOSE:
			//::PostQuitMessage(0);
			return true;
		case WM_CREATE:
		case WM_PAINT:	  
			return false;
		case WM_SIZE:
			if(m_hWnd)
			{
			//	RECT rect;
			//	::GetClientRect(m_hWnd,&rect);
			//	ResizeDevice(rect.right-rect.left,rect.bottom-rect.top);
			}
			break;
		default:
			return false;
	}
	return true;
}

bool AGame::InitA3DTools()
{
	A3DASSISTTOOLS_INFO_T	aati;
	memset(&aati, 0, sizeof(aati));
	aati.pA3DEngine		= m_pA3DEngine;

	m_pA3DAssistTools	= NewA3DAssistTools(&aati);
	if( !m_pA3DAssistTools )
	{
		ADebug::Msg(0,"NewA3DAssistTools error");
		return	false;
	}

	return true;
}

bool AGame::ReleaseA3DTools()
{
	if( m_pA3DAssistTools )
	{
		delete	m_pA3DAssistTools;
		m_pA3DAssistTools	= NULL;
	}

	return true;
}

bool AGame::logic_run(float fTimeSpan)
{
	int nRet=-1;
	if(m_bActive)
	{
		if(!m_pAGamePlay->Run(nRet,fTimeSpan))
			return false;
	}
	return true;
}

bool AGame::InitExtraGamePlay(AGamePlay *__pGamePlay)
{
	if(!__pGamePlay)
	{
		ADebug::Msg(0,"extra AGamePlay is NULL ");
		return false;
	}
	if(!__pGamePlay->Init(this))
	{
		ADebug::Msg(0,"Init AGamePlay error");
		return false;
	}

	return true;
}

bool AGame::ResizeDevice(int __cx, int __cy)
{
	if( !m_pA3DEngine || !m_pA3DViewport )
		return false;

	A3DVIEWPORTPARAM param;
	param.X = 0;
	param.Y = 0;
	param.MinZ = 0.0f;
	param.MaxZ = 1.0f;
	param.Width = __cx;
	param.Height = __cy;
	m_pA3DViewport->SetParam(&param);
	m_pA3DEngine->SetDisplayMode(__cx, __cy, (A3DFORMAT) NULL, 0, SDM_WIDTH | SDM_HEIGHT);

	SetClientWidth(__cx);
	SetClientHeight(__cy);

	return true;
}
A3DCamera * AGame::SetA3DCamera(A3DCamera * p)
{
	assert(p);

	if(p==m_pA3DCamera)
		return p;
	A3DCamera * pPre = m_pA3DCamera;
	pPre->SetAM3DSoundDevice(NULL);
	m_pA3DCamera = p;
	m_pA3DCamera->SetAM3DSoundDevice(m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice());
	m_pA3DViewport->SetCamera(m_pA3DCamera);
	if(GetA3DSnow())
		GetA3DSnow()->SetHostCamera(GetA3DCamera());
	if(GetA3DRain())
		GetA3DRain()->SetHostCamera(GetA3DCamera());
	m_pA3DCamera->Active();

	return pPre;
}

bool AGame::SetHWND(HWND hWnd)
{
	assert(hWnd);
	if(m_pA3DEngine==NULL)
	{
		assert(0);
		return false;
	}
	m_pA3DEngine->GetA3DDevice()->SetDeviceWnd(hWnd);
	
	RECT rect;
	::GetClientRect(hWnd,&rect);
	
	if(ResizeDevice(rect.right-rect.left ,rect.bottom - rect.top)==false)
	{
		return false;
	}
	m_hWnd = hWnd;
	return true;
}

bool AGame::CreateDevice(bool bWindow)
{
	return AGame::InitDevice(bWindow);
}

bool AGame::DeleteDevice()
{
	return AGame::ReleaseDevice();
}

bool AGame::InitRainSnow()
{
	ReleaseRainSnow();
	assert(!m_pA3DSnow);
	m_pA3DSnow = new A3DSnow;
	if(!m_pA3DSnow)
	{
		assert(0);
		return false;
	}
	m_pA3DSnow->SetHostCamera(GetA3DCamera());
	m_pA3DSnow->Init(m_pA3DEngine->GetA3DDevice(), 0.8f);
	m_pA3DSnow->SetEmitterWidth(100.0f);
	m_pA3DSnow->SetEmitterLength(100.0f);
	m_pA3DSnow->SetEmitterHeight(40.0f);
	m_pA3DSnow->SetUseRateMode(8, true);
	m_pA3DSnow->SetSize(0.5f);
	m_pA3DSnow->SetSizeVariation(20.0f);
	m_pA3DSnow->SetSpeed(0.9f);
	m_pA3DSnow->SetSpeedVariation(50.0f);
	m_pA3DSnow->SetLife(100);
	m_pA3DSnow->SetTextureMap("Snow.bmp");
	m_pA3DSnow->SetSrcBlend(A3DBLEND_ONE);
	m_pA3DSnow->SetDestBlend(A3DBLEND_INVSRCCOLOR);
	m_pA3DSnow->SetSpinTime(100);
	m_pA3DSnow->SetSpinTimeVariation(50);
	m_pA3DSnow->CreateSnow();
	m_pA3DSnow->SetColor(0, A3DCOLORRGBA(160, 160, 160, 255));

	m_pA3DRain = new A3DRain;
	if(!m_pA3DRain)
	{
		assert(0);
		return false;
	}
	m_pA3DRain->SetHostCamera(m_pA3DCamera);
	m_pA3DRain->Init(m_pA3DEngine->GetA3DDevice(), 0.5f);
	m_pA3DRain->SetEmitterWidth(20.0f);
	m_pA3DRain->SetEmitterLength(20.0f);
	m_pA3DRain->SetEmitterHeight(15.0f);
	m_pA3DRain->SetUseRateMode(40, true);
	m_pA3DRain->SetSize(0.03f);
	m_pA3DRain->SetSizeVariation(30.0f);
	m_pA3DRain->SetSpeed(1.2f);
	m_pA3DRain->SetSpeedVariation(50.0f);
	m_pA3DRain->SetLife(20);
	m_pA3DRain->SetTextureMap("rain.tga");
	m_pA3DRain->SetSrcBlend(A3DBLEND_SRCALPHA);
	m_pA3DRain->SetDestBlend(A3DBLEND_INVSRCALPHA);
	m_pA3DRain->CreateRain();
	m_pA3DRain->SetColorNum(1);
	m_pA3DRain->SetColor(0, A3DCOLORRGBA(180, 180, 180, 180));

	m_pA3DRain->StartRain();
	m_pA3DSnow->StartSnow();

	m_pA3DRainSplash = new A3DRainSplash;
	if(!m_pA3DRainSplash)
	{
		assert(0);
		return false;
	}
	m_pA3DRainSplash->Init(m_pA3DEngine->GetA3DDevice());
	m_pA3DRainSplash->SetSpawnNumber(6);
	m_pA3DRainSplash->SetSplashScatter(7.0f);
	m_pA3DRainSplash->SetUseRateMode(30, true);
	m_pA3DRainSplash->SetOffAxis(-89.0f);
	m_pA3DRainSplash->SetOffAxisSpread(75.0f);
	m_pA3DRainSplash->SetSize(0.35f);
	m_pA3DRainSplash->SetSizeVariation(100.0f);
	m_pA3DRainSplash->SetSpeed(0.0025f);
	m_pA3DRainSplash->SetSpeedVariation(50.0f);
	m_pA3DRainSplash->SetLife(30);
	m_pA3DRainSplash->SetSpinPhase(180);
	m_pA3DRainSplash->SetSpinPhaseVariation(100.0f);
	m_pA3DRainSplash->SetTextureMap("rainsplash.tga");
	m_pA3DRainSplash->SetSrcBlend(A3DBLEND_SRCALPHA);
	m_pA3DRainSplash->SetDestBlend(A3DBLEND_INVSRCALPHA);
	m_pA3DRainSplash->SetColorNum(2);
	m_pA3DRainSplash->SetColor(0, A3DCOLORRGBA(160, 160, 160, 180));
	m_pA3DRainSplash->SetColor(1, A3DCOLORRGBA(160, 160, 160, 20));
	m_pA3DRainSplash->SetColorMapRange(30);
	m_pA3DRainSplash->CreateStandard();
	m_pA3DRainSplash->Start();
	m_pA3DRainSplash->SetGravity(1.8f);
		
	m_pA3DRain->SetRainSplash(m_pA3DRainSplash);
	return true;
}

bool AGame::ReleaseRainSnow()
{
	RELEASE(m_pA3DRainSplash);
	RELEASE(m_pA3DRain);
	RELEASE(m_pA3DSnow);
	return true;
}

void AGame::SetLogicfps(int logicfps)
{
	m_logfps			= logicfps;
	m_fRunSpan			= 1000.f/m_logfps;//毫秒
}

void AGame::SetForceRender(bool bForceRender)
{
	m_bFastPossible = bForceRender;
}

bool AGame::Reset()
{
	bool bRet;
	if(GetA3DAssistTools())
	{
		bRet = GetA3DAssistTools()->ResetResource();
		if(!bRet)
		{
			ADebug::Msg(0,"Reset A3DAssistTools error");
			assert(0);
			return false;
		}
	}
	if(GetA3DEngine())
	{
		bRet = GetA3DEngine()->ResetResource();
		if(!bRet)
		{
			ADebug::Msg(0,"Reset A3DEngine error");
			assert(0);
			return false;
		}
	}
	return true;
}

void AGame::QueryPckFileName(AUString &strPckFileName)
{
	strPckFileName = "";
}

bool AGame::OnRunQueryActive()
{
	return m_bActive;
}
