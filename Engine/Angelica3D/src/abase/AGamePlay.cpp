#include "AGamePlay.h"
#include "AManager.h"
#include "assert.h"
#include "AGame.h"
#include  "ADeviceMan.h"
#include "ACamera.h"
#include "AWC.h"
#include "AWorld.h"
#include "ADeviceMouse.h"
#include "ADeviceKeyboard.h"
#include "AStat.h"
#include "ABaseTypes.h"
#include "A3DAssistTools.h"
#include "jexception.h"
#include "AFontDef.h"
//#include <mtime.h>

IMPLEMENT_ASERIAL(AGamePlay,AProcessor,1)

static float g_flashtime = 0.f;

AGamePlay::AGamePlay() : AProcessor()
{
	memset(m_managerList,0,sizeof(m_managerList));
	m_pACamera			= NULL;
	m_pAInterface		= NULL;
	m_pAGame			= NULL;
	m_pAWorld			= NULL;
	m_managerLastDarw	= NULL;
	m_exitCode			= EXIT_CODE_NULL;	
	m_bits				= 0xffffffff;
	m_fTickAnimationTime	= 0.f;
	m_bRain				= false;
	m_bSnow				= false;
	m_bResetAGameRes	= true;
	m_nEarIn			= 0;
	SetNoLogicRunTickCount(NOLOGICRUNCOUNT);
	m_bGamePlayRender	= true;
	m_bPrecach			= true;
}

AGamePlay::~AGamePlay()
{
}

bool AGamePlay::Init(AGame * __pAGame)
{
	m_pAGame		= __pAGame;
	assert(m_pAGame);

	ResetResource();
	m_pACamera = new ACamera;
	if(!m_pACamera)
	{
		ADebug::Msg(0,"new ACamera error");
		assert(0);
		return false;
	}

	if(!m_pACamera->Init(__pAGame))
	{
		ADebug::Msg(0,"init ACamera error");
		assert(0);
		return false;
	}
	return true;
}

bool AGamePlay::LogicRun(float __TimeSpan)
{
	if(m_nNoLogicRunCount>=0)
		m_nNoLogicRunCount--;
	if(GetNologicRunTickCount()>=0)
		return LogicRun_Begin(__TimeSpan);

	for(int i=0;i<MAX_MANAGER_NUM;i++)
	{
		if(m_managerList[i])
		{
			int id = m_managerList[i]->GetManagerId();
			if(GetBits() & (1<<id))
			{
				if(m_managerList[i]->LogicRun(__TimeSpan) == false)
					return false;
			}
		}
	}

	return true;
}

bool AGamePlay::Release()
{
	for(int i=0;i<MAX_MANAGER_NUM;i++)
	{
		if(m_managerList[i])
		{
			m_managerList[i]->Release();
			delete m_managerList[i];
			m_managerList[i] = NULL;
		}
	}
	RELEASE(m_pACamera);
	RELEASE(m_pAInterface);
	RELEASE(m_pAWorld);
	ResetResource();
	return true;
}

bool AGamePlay::Run(int& __nRet,float fTimeSpan)
{
	if(m_exitCode != EXIT_CODE_NULL)
	{
		__nRet = m_exitCode;
		m_exitCode = EXIT_CODE_NULL;
		return false;
	}
	if(fTimeSpan >500)
		fTimeSpan = 500;

	DeviceDealIn();
	
BEGIN_STAT("所有的逻辑");
	if(m_nEarIn%10 == 0)
	{
		if(m_pAGame->GetA3DCamera())
			m_pAGame->GetA3DCamera()->UpdateEar();
		m_nEarIn = 0;
	}
	m_nEarIn++;
//界面
	BEGIN_STAT("界面的LogicRun统计");
	if(GetBits() & INTERFACE_MASK && m_pAInterface && m_pAInterface->GetActiveStatus())
	{
		if( m_pAInterface->LogicRun(fTimeSpan) == false)
		{
			__nRet = 1;
			ADebug::Msg(0,"Interface LogicRun error");
			return false;
		}
	}
	END_STAT();
//自己	
	BEGIN_STAT("所有管理器的LogicRun统计");
	if(GetActiveStatus())
	{
		if( this->LogicRun(fTimeSpan) == false)
		{
			__nRet = 0;
			ADebug::Msg(0,"AEnyaGamePlay LogicRun error");
			return false;
		}
	}
	END_STAT();
	BEGIN_STAT("TickAnimation");
	m_fTickAnimationTime += fTimeSpan;
	while(m_fTickAnimationTime>=ENGINETICKTIME)
	{
		bool bval = TickAnimation(fTimeSpan);
		if( bval == false)
		{
			ADebug::Msg(0,"Engine tickanimation error");
			return false;
		}
		m_fTickAnimationTime-= ENGINETICKTIME;
	}
	assert(m_fTickAnimationTime<ENGINETICKTIME);
	END_STAT();
//绘制
	BEGIN_STAT("所有的Draw");
	if(Draw() == false )
	{
		ADebug::Msg(0,"AEnyaGamePlay draw error");
		return false;
	}
	END_STAT();
	DeviceDealOut();
	A3DAssistToolsLogicRun(fTimeSpan);
	g_flashtime += (fTimeSpan/1000.f);
	return true;
}


bool AGamePlay::Draw()
{
	J_EXCEPTION_RECORD(0);
	A3DViewport * pA3DViewport = m_pAGame->GetA3DViewport();
	A3DEngine * pA3DEngine = m_pAGame->GetA3DEngine();
	assert(pA3DViewport && pA3DEngine);
	bool bval;

	if(GetPrecach() && GetActiveStatus() && GetNologicRunTickCount()>=0)
	{
		ADebug::Msg(0,"Precaching...");
		return pA3DEngine->PrecacheAllTextures(pA3DViewport);
	}

	BEGIN_STAT("BeginRender");
	bval = pA3DEngine->BeginRender();
	if(!bval)
	{
		ADebug::Msg(0,"BeginRender error");
		assert(0);
		return false;
	}
	pA3DViewport->Active();
	pA3DViewport->ClearDevice();
	END_STAT();
	
	pA3DEngine->BeginCacheAlphaMesh(pA3DViewport);

	BEGIN_STAT("AWorld的Render统计");

	if(m_bGamePlayRender && m_pAWorld && !m_pAWorld->Render(pA3DViewport))
	{
		ADebug::Msg(0,"AWorld Render error");
		assert(0);
		return false;
	}
	END_STAT();

	BEGIN_STAT("Manager的Draw统计");
	for(int i=0;i<MAX_MANAGER_NUM;i++)
	{
		if(m_bGamePlayRender && m_managerList[i])
		{
			if(m_managerList[i] != GetLastDrawMan())
			{
				if(m_managerList[i]->Draw() == false)
					return false;
			}
		}
	}
	END_STAT();
	BEGIN_STAT("Rain Snow的LogicRun统计");
	if(m_bGamePlayRender && m_bRain)
	{
		if(m_pAGame->GetA3DRain())
			m_pAGame->GetA3DRain()->RenderParticles(m_pAGame->GetA3DViewport());
	}
	if(m_bGamePlayRender && m_bSnow)
	{
		if(m_pAGame->GetA3DSnow())
			m_pAGame->GetA3DSnow()->RenderParticles(m_pAGame->GetA3DViewport());
	}
	END_STAT();
	BEGIN_STAT("RenderAlpha");
	if(m_bGamePlayRender && m_pAWorld && !m_pAWorld->RenderAlpha(pA3DViewport))
	{
		ADebug::Msg(0,"AWorld Render error");
		assert(0);
		return false;
	}
	END_STAT();

	BEGIN_STAT("RenderScene");
	if(m_bGamePlayRender)
	{
		pA3DEngine->RenderWorld(pA3DViewport);
		pA3DEngine->RenderImageModels(pA3DViewport);
		pA3DEngine->FlushCachedAlphaMesh(pA3DViewport);
		pA3DEngine->RenderGFX(pA3DViewport, 0xffffffff);	
	}
	END_STAT();

	BEGIN_STAT("LastMan Draw");
	if(m_bGamePlayRender && GetLastDrawMan())
	{
		if(GetLastDrawMan()->Draw() == false)
			return false;
	}
	END_STAT();
	BEGIN_STAT("InterFace Draw");
	if(m_pAInterface /*&& m_pAInterface->GetActiveStatus()*/)
	{
		if(m_pAInterface->Draw()== false)
		{
			ADebug::Msg(0,"Interface Draw error");
			return false;
		}
	}
	END_STAT();
	BEGIN_STAT("AfterRender");
	AfterRender();
	END_STAT();
	BEGIN_STAT("EndRender");
	bval = pA3DEngine->EndRender();
	END_STAT();
	if(!bval)
	{
		ADebug::Msg(0,"A3DEngine EndRender error");
		assert(0);
		return false;
	}
	BEGIN_STAT("Present");
	if(GetActiveStatus())
	{
		if(GetNologicRunTickCount()<=0)
		{
			bval = pA3DEngine->Present();
			if(!bval)
			{
				ADebug::Msg(0,"A3DEngine Present error");
				assert(0);
				return false;
			}
		}
	}
	else
	{
		bval = pA3DEngine->Present();
		if(!bval)
		{
			ADebug::Msg(0,"A3DEngine Present error");
			assert(0);
			return false;
		}
	}
	END_STAT();
	return true;
}

bool AGamePlay::AddManPtr(AManager * __pAManager)
{
	if(NULL == __pAManager)
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		assert(0);
		return true;
	}
	int id = __pAManager->GetManagerId();
	if(m_managerList[id])
	{
		ADebug::Msg(0,"AddManPtr have exist the pointer");
		assert(0);
		return false;
	}
	
	m_managerList[id] = __pAManager;
	return true;
}


bool AGamePlay::Save(AArchive &ar)
{
	return true;
}


bool AGamePlay::Reset()
{
	for(int i=0;i<MAX_MANAGER_NUM;i++)
	{
		if(m_managerList[i])
		{
			if(m_managerList[i]->Clear() == false)
				return false;
		}
	}
	if(m_pAWorld)
		m_pAWorld->Release();
	return true;
}

void AGamePlay::AfterRender()
{
/*	if(g_flashtime> 1.f)
	{
		int nFont = AWC::GetCurrentFont();
		AWC::SetCurrentFont(ARIAL18);
		m_pAGame->GetA3DAssistTools()->F_TextOut(30,20,"Evaluation",A3DCOLORRGB(255,255,0),false);
		AWC::SetCurrentFont(nFont);

		if(g_flashtime >= 1.5f)
			g_flashtime = 0.f;
	}
*/
}

bool AGamePlay::TickAnimation(float fTimeSpan)
{
	if(m_pAGame->GetA3DRain())
		m_pAGame->GetA3DRain()->TickEmitting();
	if(m_pAGame->GetA3DSnow())
		m_pAGame->GetA3DSnow()->TickEmitting();
	
	if(!m_pAWorld->TickAnimationExceptModels(fTimeSpan))
	{
		assert(0);
		return false;
	}
	if(m_pAGame->GetA3DEngine()->TickAnimation()==false)
	{
		assert(0);
		return false;
	}
	return true;
}

bool AGamePlay::LogicRun_Begin(float time)
{
	for(int i=0;i<MAX_MANAGER_NUM;i++)
	{
		if(m_managerList[i])
		{
			int id = m_managerList[i]->GetManagerId();
			if(GetBits() & (1<<id))
			{
				if(m_managerList[i]->LogicRun_Begin(time) == false)
					return false;
			}
		}
	}
	//if(m_pAWorld)
	//	m_pAWorld->ForceRenderAll(true);
	//g_pAGame->GetA3DEngine()->PrecacheAllTextures(g_pAGame->GetA3DViewport());
	return true;

}

void AGamePlay::DeviceDealIn()
{
	if(m_pAGame->GetADeviceMan())
		m_pAGame->GetADeviceMan()->DealIn();
}

void AGamePlay::DeviceDealOut()
{
	if(m_pAGame->GetADeviceMan())
		m_pAGame->GetADeviceMan()->DealOut();
}

bool AGamePlay::A3DAssistToolsLogicRun(float time)
{
	// 保证A3DAssistTools可以正常运行
	if(m_pAGame->GetA3DAssistTools())
	{
		m_pAGame->GetA3DAssistTools()->LogicRun();
		m_pAGame->GetA3DAssistTools()->AddSystemTickcount((int)time);
	}
	return true;
}

void AGamePlay::ShowCursor(bool bShow)
{
	int i=0;
	if(bShow)
	{
		while(::ShowCursor(true)<0)
		{
			i++;
			if(i>100)
			{
				assert(false);
				return;
			}
		}
		return ;
	}
	if(!bShow)
	{
		while(::ShowCursor(false)>=0)
		{
			i++;
			if(i>100)
			{
				assert(false);
				return;
			}
		}
	}
	return ;
}

bool AGamePlay::ResetResource()
{
	if(GetIsResetAGameRes())
	{
		if(m_pAGame)
		{
			bool bRet = m_pAGame->ReleaseRainSnow();
			if(bRet==false)
			{
				ADebug::Msg(0,"Release RainSnow error: resetresource");
				assert(0);
				return false;
			}
			bRet = m_pAGame->Reset();
			if(bRet==false)
			{
				ADebug::Msg(0,"Reset  error: resetresource");
				assert(0);
				return false;
			}
			return true;//m_pAGame->InitRainSnow();
		}
	}
	return true;
}

void AGamePlay::SetNoLogicRunTickCount(int nCount)
{
	m_nNoLogicRunCount = nCount<0 ? -1 : nCount;
}