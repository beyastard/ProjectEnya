#include "AScene.h"
#include "A3DVSSIBLScene.h"
#include "A3DVSSScene.h"
#include "jexception.h"

AScene::AScene()
{
	m_pA3DDevice		= NULL;
	m_pA3DESP			= NULL;
	m_pA3DIBLLightGrid  = NULL;
	m_pA3DScene			= NULL;
	m_pPVSBuffer_Begin	= NULL;
	m_pPVSBuffer_End	= NULL;
	m_iCurrentRouteID	= -1;
	m_iCurrentPointID	= -1;
	m_iLastRouteID		= -1;
	m_MaxObjCnt			= 0;
	m_bForceRenderAll	= true;
	m_bIBL				= false;
	m_pOldScene			= NULL;
	m_pIBLScene			= NULL;
}
AScene::~AScene()
{

}

bool AScene::Create(const PAWORLD_INIT_SCENE __pDatum)
{
	if(!BindFinished())
	{
		ADebug::Msg(0,"%s","not bind device!");
		return false;
	}
	Release();

	if(strstr(__pDatum->szModFile,".mox"))
	{
		m_pOldScene= new A3DVSSScene;
		m_pA3DScene = m_pOldScene;
		m_bIBL = false;
	}
	else if(strstr(__pDatum->szModFile,".ils"))
	{
		m_pIBLScene= new A3DVSSIBLScene;
		m_pA3DScene = m_pIBLScene;
		m_bIBL = true;
	}

	if( NULL == m_pA3DScene )
	{
		ADebug::Msg(0,"%s","Not enough memory to create Scene!");
		return false;
	}
	if( !m_pA3DScene->Init(m_pA3DDevice) )
	{
		delete m_pA3DScene;
		m_pA3DScene = NULL;
		ADebug::Msg(0,"%s","Error init A3DScene!");
		return false;
	}

	if(strcmp(__pDatum->szModFile,"")!=0)
	{
		if(!m_pA3DScene->Import(__pDatum->szModFile))
		{	
			delete m_pA3DScene;
			m_pA3DScene = NULL;
			ADebug::Msg(0,"%s","Error load A3DScene!");
			return false;
		}
	}
	if(strcmp(__pDatum->szBSPFile,"")!=0)
	{
		if(!m_pA3DScene->SetBSPFile(__pDatum->szBSPFile))
		{	
			delete m_pA3DScene;
			m_pA3DScene = NULL;
			ADebug::Msg(0,"%s","Error load BSP!");
			return false;
		}
	}
	if(strcmp(__pDatum->szESPFile,"")!=0)
	{
		m_pA3DESP = new A3DESP();
		if( NULL == m_pA3DESP )
		{
			ADebug::Msg(0,"%s","Not enough memory to create ESP!");
			return false;
		}
		if( !m_pA3DESP->Load(__pDatum->szESPFile) )
		{
			delete m_pA3DESP;
			m_pA3DESP = NULL;
			ADebug::Msg(0,"%s","Load ESP File Fail!");
			return false;
		}
	}

	if( m_bIBL )
	{
		m_pA3DIBLLightGrid = m_pIBLScene->GetLightGrid();
		m_pIBLScene->ForceToRenderAll(true);
		m_pIBLScene->UpdateVisibleSets(m_pA3DDevice->GetA3DEngine()->GetActiveViewport());
	}
	else
	{
		m_pOldScene->ForceToRenderAll(true);
		m_pOldScene->UpdateVisibleSets(m_pA3DDevice->GetA3DEngine()->GetActiveViewport());
	}
	return true;
}

bool AScene::Create(ASceneInit* pInit)
{
	AWORLD_INIT_SCENE dat;
	pInit->ToKernelDatum(&dat);
	return Create(&dat);
}

bool AScene::Release()
{
	if(!m_pA3DScene)
	{
		return false;
	}
	if( m_pA3DScene )
	{
		m_pA3DScene->Release();
		delete m_pA3DScene;
		m_pA3DScene = NULL;
	}
	if(m_pA3DESP)
	{
		m_pA3DESP->Release();
		delete m_pA3DESP;
		m_pA3DESP = NULL;
	}
	if(m_pPVSBuffer_Begin)
	{
		delete[] m_pPVSBuffer_Begin;
		m_pPVSBuffer_Begin = NULL;
	}
	return true;
}

void AScene::ForceRenderAll(bool b)
{
	if(!m_pA3DScene)
	{		
		return ;
	}	
	if(m_pOldScene)
		m_pOldScene->ForceToRenderAll(b);
	if(m_pIBLScene)
		m_pIBLScene->ForceToRenderAll(b);
	m_bForceRenderAll = b;
}
bool AScene::LoadNewRoute(BYTE* pBuf_Point,BYTE* pBuf_Route)
{

	J_EXCEPTION_RECORD(0)

	if(!m_pA3DScene)
	{
		return true;
	}

	if(m_pOldScene)
	{
		m_pOldScene->RemoveAllRoutes();
		m_iCurrentRouteID = m_pOldScene->LoadRoute(pBuf_Route);
		m_iCurrentPointID = m_pOldScene->LoadRoute(pBuf_Point);
		m_pOldScene->SetCurrentRoute(m_iCurrentRouteID,0);
		m_pOldScene->SetCurrentRoute(m_iCurrentPointID,0);

		if(m_pPVSBuffer_Begin == NULL)
		{
			m_MaxObjCnt = m_pOldScene->GetSceneObjectNum();
			if(m_MaxObjCnt < 0 || m_MaxObjCnt > 100000)
			{
				assert(0);
				return false;
			}
			m_pPVSBuffer_Begin = new int[m_MaxObjCnt];
			m_pPVSBuffer_End = m_pPVSBuffer_Begin;
		}
	}
	if(m_pIBLScene)
	{
		m_pIBLScene->RemoveAllRoutes();
		m_iCurrentRouteID = m_pIBLScene->LoadRoute(pBuf_Route);
		m_iCurrentPointID = m_pIBLScene->LoadRoute(pBuf_Point);
		m_pIBLScene->SetCurrentRoute(m_iCurrentRouteID,0);
		m_pIBLScene->SetCurrentRoute(m_iCurrentPointID,0);

		if(m_pPVSBuffer_Begin == NULL)
		{
			m_MaxObjCnt = m_pIBLScene->GetSceneObjectNum();
			if(m_MaxObjCnt < 0 || m_MaxObjCnt > 100000)
			{
				assert(0);
				return false;
			}
			m_pPVSBuffer_Begin = new int[m_MaxObjCnt];
			m_pPVSBuffer_End = m_pPVSBuffer_Begin;
		}
	}
	m_iLastRouteID		= -1;
	return true;
}
bool AScene::SetCurrentRoute(bool bRoute, int iSample)
{

	J_EXCEPTION_RECORD(0)

	if(!m_pA3DScene)
	{
		assert(0);
		return false;
	}

	int iRouteID = bRoute ? 1 : 0;

	if(m_pOldScene)
	{
		if (m_iLastRouteID != iRouteID)
		{
			m_iLastRouteID = iRouteID;

			if(bRoute)
				m_pOldScene->SetCurrentRoute(m_iCurrentRouteID,iSample);
			else
				m_pOldScene->SetCurrentRoute(m_iCurrentPointID,0);
		}

		if(bRoute)
			m_pOldScene->SetCurrentSample(iSample);
		else
			m_pOldScene->SetCurrentSection(iSample);
	}

	if(m_pIBLScene)
	{
		if (m_iLastRouteID != iRouteID)
		{
			m_iLastRouteID = iRouteID;

			if(bRoute)
				m_pIBLScene->SetCurrentRoute(m_iCurrentRouteID,iSample);
			else
				m_pIBLScene->SetCurrentRoute(m_iCurrentPointID,0);
		}

		if(bRoute)
			m_pIBLScene->SetCurrentSample(iSample);
		else
			m_pIBLScene->SetCurrentSection(iSample);
	}
	return true;
}
bool AScene::UpdateVisibleSets(A3DViewport * pCurrentViewport)
{

	J_EXCEPTION_RECORD(0)

	if(m_pOldScene)
	{
		if(!m_bForceRenderAll)
		{
			m_pOldScene->UpdateVisibleSets(pCurrentViewport);
			int cnt;
			m_pOldScene->GetCurrentVisObjs(m_pPVSBuffer_Begin,&cnt);		//	Get visible object's ID of specified section
			m_pPVSBuffer_End = m_pPVSBuffer_Begin + cnt;
			return true;
		}
	}
	if(m_pIBLScene)
	{
		if(!m_bForceRenderAll)
		{
			m_pIBLScene->UpdateVisibleSets(pCurrentViewport);
			int cnt;
			m_pIBLScene->GetCurrentVisObjs(m_pPVSBuffer_Begin,&cnt);		//	Get visible object's ID of specified section
			m_pPVSBuffer_End = m_pPVSBuffer_Begin + cnt;
			return true;
		}
	}
	return false;
}
