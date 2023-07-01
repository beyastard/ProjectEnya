#include "AWorld.h"
#include "AGame.h"
#include "AEnvironment.h"
#include "AScene.h"
#include "ASky.h"
#include "ATerrain.h" 
#include "A3DCDS.h"
#include "AUnitInWorldInterface.h"
#include "AMemStatus.h"
#include "AModelUnit.h"
#include "jexception.h"


struct WorldCDServer : public A3DCDS
{
	friend class AWorld;
	AWorld * m_pHost;
	void SetHost(AWorld* pH){m_pHost = pH;}
	virtual bool RayTrace(A3DVECTOR3& p, A3DVECTOR3& v,
		FLOAT t, RAYTRACE * pRayTrace, A3DModel * pModelExclude)
	{

		J_EXCEPTION_RECORD(0)

		RAYTRACE Trace;
		pRayTrace->fFraction = 1.0f;
		i_shot_target * pTag = 0;
		if (m_pHost->RayTraceSence(p,v,t,&Trace) && Trace.fFraction < pRayTrace->fFraction)
			*pRayTrace = Trace;
		if(m_pHost->RayTraceAI(p,v,t,&Trace,0,0) && Trace.fFraction < pRayTrace->fFraction)
			*pRayTrace = Trace;
		if(m_pHost->m_bForceRender)
		{
			if (m_pHost->RayTraceMatter(p,v,t,&Trace,0,&pTag) && Trace.fFraction < pRayTrace->fFraction)
				*pRayTrace = Trace;
		}
		else if(m_pHost->m_pScn)
		{
			if (m_pHost->RayTraceMatterInHashTable(p,v,t,&Trace,
				m_pHost->m_pScn->PVSRenderPtrBegin(),
				m_pHost->m_pScn->PVSRenderPtrEnd())
				&& Trace.fFraction < pRayTrace->fFraction)
			{
				*pRayTrace = Trace;
			}
		}
		if (pRayTrace->fFraction < 1.0f)
			return true;
		return false;
	}
	virtual bool OBBTrace(A3DVECTOR3& vecStart, A3DVECTOR3& vecVelocity,
		FLOAT vTime, OBBSHAPE& obbShape, OBBTRACE * pOBBTrace, A3DModel * 
		pModelExclude, bool bFirstTime){return false;}
	virtual bool AABBTrace(A3DVECTOR3& vStart, A3DVECTOR3& vExts, A3DVECTOR3& vVelocity,
		FLOAT fTime, AABBTRACE* pTrace, A3DModel * pModelExclude){return false;}
	virtual A3DESP * GetA3DESP()
	{
		if(m_pHost->GetAScene()==0)
			return 0;
		return
			m_pHost->GetAScene()->GetA3DESP();
	}
	virtual A3DTerrain * GetA3DTerrain(){return 0;}
	virtual void DisableModelRayTrace(){}
	virtual void EnableModelRayTrace(){}
	virtual void SetModelRayTraceMask(DWORD dwMask){}
	virtual void SetModelAABBTraceMask(DWORD dwMask){}
	virtual void ClearModelRayTraceMask(DWORD dwMask){}
	virtual void ClearModelAABBTraceMask(DWORD dwMask){}
	virtual DWORD GetModelRayTraceMask(){return 0xffffffff;}
	virtual DWORD GetModelAABBTraceMask(){return 0xffffffff;}
};

struct PVSHashTableEnumFunctioner
{
	typedef i_render_target* VALUE;
	AWorld * m_pHost;
	A3DViewport * pCurrentViewport;
	PVSHashTableEnumFunctioner(AWorld * pHost , A3DViewport * pCurrentViewport):
	m_pHost(pHost),pCurrentViewport(pCurrentViewport){}
	void operator()(const VALUE & pItem)
	{
		if(pItem->GetVisibility())
		{
			if(!pItem->Render(pCurrentViewport))
			{
				throw("BAD RENDER");	
			}
		}
	}
};

AWorld::AWorld() : m_PVSHashTable(4096,_hash_function())
{
	m_pA3DDevice	= NULL;
	m_pA3DEngine	= NULL;	
	m_pEnv			= NULL;
	m_pScn			= NULL;
	m_pSky			= NULL;
	m_pTrn			= NULL;
	m_bForceRender  = false;
	m_bSortAlpha	= true;
	m_fDirLightWeight = 1.0f;
	m_fAmbLightWeight = 1.0f;
	m_pCDServer = new WorldCDServer;
	//there is no RTTI 
	static_cast<WorldCDServer*>(m_pCDServer)->SetHost(this);
}
bool AWorld::CreateSky(const PAWORLD_INIT_SKY& __pDatum)
{
//	CHECK_CUP_COUNTER("AWorld::CreateSky")
	DestroySky();
	m_pSky = new ASky;
	m_pSky->BindDevice(m_pA3DDevice);
	return m_pSky->Create(__pDatum);
}
bool AWorld::CreateTerrain(const PAWORLD_INIT_TERRAIN& __pDatum)
{
//	CHECK_CUP_COUNTER("AWorld::CreateTerrain")
	MEMSTATUSBEGIN("CreateSky");
	DestroyTerrain();
	m_pTrn = new ATerrain;
	m_pTrn->BindDevice(m_pA3DDevice);
	bool bRet = m_pTrn->Create(__pDatum);
	MEMSTATUSEND();
	return bRet;
}
bool AWorld::CreateScene(const PAWORLD_INIT_SCENE& __pDatum)
{
	MEMSTATUSBEGIN("CreateScene");

//	CHECK_CUP_COUNTER("AWorld::CreateScene")
	DestroyScene();
	m_pScn = new AScene;
	m_pScn->BindDevice(m_pA3DDevice);
	bool bRet = m_pScn->Create(__pDatum);
	MEMSTATUSEND();
	return bRet;
}
bool AWorld::CreateEnvironment(const PAWORLD_INIT_ENVIRONMENT& __pDatum)
{
	MEMSTATUSBEGIN("CreateEvnironment");
//	CHECK_CUP_COUNTER("AWorld::CreateEnvironment")
	DestroyEnvironment();
	m_pEnv = new AEnvironment;
	if(!m_pEnv->Create(__pDatum))
	{
		return false;
	}
	m_pA3DDevice->SetAmbient(m_pEnv->GetKernelDatum()->colorAmbient);
	m_pA3DDevice->SetFogColor(m_pEnv->GetKernelDatum()->fogColor);
	m_pA3DDevice->SetFogStart(m_pEnv->GetKernelDatum()->vFogStart);
	m_pA3DDevice->SetFogEnd(m_pEnv->GetKernelDatum()->vFogEnd);
	m_pA3DDevice->SetFogDensity(m_pEnv->GetKernelDatum()->vFogDensity);
	MEMSTATUSEND();
	return true;
}
bool AWorld::DestroySky()
{
	if(m_pSky)
	{
		m_pSky->Release();
		delete m_pSky;
		m_pSky = NULL;
	}
	return true;
}
bool AWorld::DestroyTerrain()
{
	if(m_pTrn)
	{
		m_pTrn->Release();
		delete m_pTrn;
		m_pTrn = NULL;
	}
	return true;
}
bool AWorld::DestroyScene()
{
	if(m_pScn)
	{
		m_pScn->Release();
		delete m_pScn;
		m_pScn = NULL;
	}
	return true;
}
bool AWorld::DestroyEnvironment()
{
	if(m_pEnv)
	{
		m_pEnv->Release();
		delete m_pEnv;
		m_pEnv = NULL;
	}
	return true;
}
bool AWorld::Release()
{
	if(m_pEnv)
	{
		m_pA3DDevice->SetAmbient(m_pEnv->GetKernelDatum()->colorAmbient);
		m_pA3DDevice->SetFogColor(m_pEnv->GetKernelDatum()->fogColor);
		m_pA3DDevice->SetFogStart(m_pEnv->GetKernelDatum()->vFogStart);
		m_pA3DDevice->SetFogEnd(m_pEnv->GetKernelDatum()->vFogEnd);
		m_pA3DDevice->SetFogDensity(m_pEnv->GetKernelDatum()->vFogDensity);
	}
	DestroySky();
	DestroyTerrain();
	DestroyScene();
	DestroyEnvironment();
	m_PVSHashTable.clear();
	m_pA3DEngine->SetA3DCDS(0);
	return true;
}

AWorld::~AWorld()
{
	try
	{
		if(m_pCDServer)
		{
			delete m_pCDServer;
			m_pCDServer = 0;
		}
	}
	catch(...)
	{
		ADebug::Msg(1,"Warning : You should delete world before engine is invalid.");
		assert(0&&"Warning : You should delete world before engine is invalid.");
	}
}
void AWorld::RefreshEnvironmentParam(AEnvironmentInit * pInit)
{

	J_EXCEPTION_RECORD(0)

	if(m_pEnv)
	{
		m_pEnv->Create(pInit);
		m_pA3DDevice->SetAmbient(m_pEnv->GetKernelDatum()->colorAmbient);
		m_pA3DDevice->SetFogColor(m_pEnv->GetKernelDatum()->fogColor);
		m_pA3DDevice->SetFogStart(m_pEnv->GetKernelDatum()->vFogStart);
		m_pA3DDevice->SetFogEnd(m_pEnv->GetKernelDatum()->vFogEnd);
		m_pA3DDevice->SetFogDensity(m_pEnv->GetKernelDatum()->vFogDensity);
	
		A3DLight * pL = g_pAGame->GetA3DLight();
		if(pL)
		{
			A3DLIGHTPARAM param = pL->GetLightparam();	
			param.Diffuse = ColorRGBAToColorValue(m_pEnv->GetKernelDatum()->colorDirectional);
			param.Direction	= Normalize(m_pEnv->GetKernelDatum()->vecLightDir);
			pL->SetLightParam(&param);
		}
		else
		{
			assert(0);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////

bool AWorld::Render(A3DViewport * pCurrentViewport)
{
	J_EXCEPTION_RECORD(0)
	if(m_pSky)
	{
		if( m_pSky->GetA3DSkySphere() && (!m_pSky->GetA3DSkySphere()->SetCamera(pCurrentViewport->GetCamera()) || !m_pSky->GetA3DSkySphere()->Render()) )
		{
			ADebug::Msg(0,"%s","Render Sky Fail!");
			return false;
		}

		if( m_pSky->GetA3DSkyBox() && (!m_pSky->GetA3DSkyBox()->SetCamera(pCurrentViewport->GetCamera()) || !m_pSky->GetA3DSkyBox()->Render()) )
		{
			ADebug::Msg(0,"%s","Render Sky Fail!");
			return false;
		}

		if( m_pSky->GetA3DSkyGradient() && (!m_pSky->GetA3DSkyGradient()->SetCamera(pCurrentViewport->GetCamera()) || !m_pSky->GetA3DSkyGradient()->Render()) )
		{
			ADebug::Msg(0,"%s","Render Sky Fail!");
			return false;
		}
	}
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	if(m_pTrn)
	{

		if(m_pTrn->GetTerrain())
		{
			m_pTrn->GetTerrain()->SetCamera(pCurrentViewport->GetCamera());
			m_pTrn->GetTerrain()->Render(pCurrentViewport);
		}
	}
	m_pA3DDevice->GetA3DEngine()->SetBuildingRenderFlag(true);
	if(m_pScn)
	{	
		A3DIBLLightGrid * pGrid = m_pScn->GetLightGrid();
		if(pGrid)
		{
			pGrid->SetAmbientBrightness(m_fAmbLightWeight);
			pGrid->SetDirectBrightness(m_fDirLightWeight);
		}
		m_pScn->UpdateVisibleSets(pCurrentViewport);
		if(m_pScn->GetA3DScene())
		{
			m_pScn->GetA3DScene()->Render(pCurrentViewport, A3DSCENE_RENDER_SOLID);
		}
	}
	m_pA3DEngine->SetBuildingRenderFlag(false);
	AListPosition pos = m_RenderList.GetHeadPosition();
	while(pos)
	{
		i_render_target * pItem = m_RenderList.GetNext(pos);
		if(!pItem || !pItem->GetVisibility())
		{
			continue;
		}
		if( !pItem->Render(pCurrentViewport) )
		{
			assert(0);
			ADebug::Msg(0,"%s","Render Model Fail!");
			return false;
		}
	}
	if(m_bForceRender)
	{
		if(!RenderAllInHashTable(pCurrentViewport))
		{
			return false;
		}
	}
	else if(m_pScn)
	{
		if(!RenderItemInHashTable(m_pScn->PVSRenderPtrBegin(),m_pScn->PVSRenderPtrEnd(),pCurrentViewport))
		{
			return false;
		}
	}
	return true;
}

bool AWorld::RenderAlpha(A3DViewport * pCurrentViewport)
{

	J_EXCEPTION_RECORD(0)

	if(m_pScn)
	{
		if(m_pScn->GetA3DScene())
		{
			if(m_bSortAlpha)
				m_pScn->GetA3DScene()->RenderSort(pCurrentViewport, A3DSCENE_RENDER_ALPHA);
			else
				m_pScn->GetA3DScene()->Render(pCurrentViewport, A3DSCENE_RENDER_ALPHA);
		}
	}
	return true;
}

bool AWorld::TickAnimationExceptModels(float time_span)
{
	if(m_pSky && !m_pSky->TickAnimation(time_span))
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////
bool AWorld::AddToMissileTraceList(i_shot_target* pTag)
{
	J_EXCEPTION_RECORD(0)
	if(!pTag || m_MissileTraceList.Find(pTag))
		return false;
	m_MissileTraceList.AddTail(pTag);
	return true;		
}
bool AWorld::AddToMatterTraceList(i_shot_target* pTag)
{
	J_EXCEPTION_RECORD(0)

	if(!pTag || m_MatterTraceList.Find(pTag))
		return false;
	m_MatterTraceList.AddTail(pTag);
	return true;
}
bool AWorld::AddToAITraceList(i_shot_target* pTag)
{

	J_EXCEPTION_RECORD(0)

	if(!pTag || m_AITraceList.Find(pTag))
		return false;
	m_AITraceList.AddTail(pTag);
	return true;
}
bool AWorld::DeleteFromMissileTraceList(i_shot_target* pTag)
{

	J_EXCEPTION_RECORD(0)


	AListPosition pos = m_MissileTraceList.Find(pTag);
	if(pos)
	{
		m_MissileTraceList.RemoveAt(pos);
		return true;
	}
	return false;
}
bool AWorld::DeleteFromMatterTraceList(i_shot_target* pTag)
{
	J_EXCEPTION_RECORD(0)

	AListPosition pos = m_MatterTraceList.Find(pTag);
	if(pos)
	{
		m_MatterTraceList.RemoveAt(pos);
		return true;
	}
	return false;
}
bool AWorld::DeleteFromAITraceList(i_shot_target* pTag)
{

	J_EXCEPTION_RECORD(0)

	AListPosition pos = m_AITraceList.Find(pTag);
	if(pos)
	{
		m_AITraceList.RemoveAt(pos);
		return true;
	}
	return false;
}
bool AWorld::RayTraceMissile(
							 const A3DVECTOR3& p,
							 const A3DVECTOR3& v,
							 float t,RAYTRACE *
							 pRayTrace,
							 i_shot_target* pExcept,
							 i_shot_target ** ppTaget)
{

	J_EXCEPTION_RECORD(0)

	RAYTRACE rayTrace;
	float fMinFrac = 1.0f;
	AListPosition pos = m_MissileTraceList.GetHeadPosition();
	while (pos)
	{
		i_shot_target * pItem = m_MissileTraceList.GetNext(pos);
		if((pItem->GetRayTraceEnable())&&
			(pItem != pExcept) &&
			(pItem->RayTrace(p, v * t , &rayTrace)) )
		{
			if( rayTrace.fFraction < fMinFrac)
			{
				fMinFrac	= rayTrace.fFraction;
				*pRayTrace  = rayTrace;
				if(ppTaget)
				{
					*ppTaget = pItem;
				}
			}
		}
	}

	if( fMinFrac < 1.0f )
		return true;
	
	return false;
}

bool AWorld::RayTraceMatterInHashTable(
									   const A3DVECTOR3& p,
									   const A3DVECTOR3& v,
									   float t,
									   RAYTRACE * pRayTrace,
									   int* begin,
									   int* end)
{

	J_EXCEPTION_RECORD(0)

	RAYTRACE rayTrace;
	float fMinFrac = 1.0f;
	for(int * itor = begin;itor!=end;++itor)
	{
		pair<AModelUnit **, bool> res = m_PVSHashTable.get(*itor);
		if(res.second &&(*(res.first))->GetVisibility() )
		{
			try
			{
				//应该再做一个针对碰撞检测的散列表，但是没时间改
				//现有的所有加入到表里的对象都是AModelUnit所以这样写了
				AModelUnit * pItem = *(res.first);
				if(pItem)
				{
					if((pItem ->GetRayTraceEnable()) && (pItem->RayTrace(p, v * t , &rayTrace)) )
					{
						if( rayTrace.fFraction < fMinFrac )
						{
							fMinFrac	= rayTrace.fFraction;
							*pRayTrace	= rayTrace;
						}
					}
				}
			}
			catch(...)
			{
				assert(0);
			}
		}
	}
	if( fMinFrac < 1.0f )
		return true;
	else
		return false;
}

bool AWorld::RayTraceMatter(const A3DVECTOR3& p,
							const A3DVECTOR3& v,
							float t,
							RAYTRACE * pRayTrace,
							i_shot_target* pExcept,
							i_shot_target ** ppTaget)
{

	J_EXCEPTION_RECORD(0)


	RAYTRACE rayTrace;
	float fMinFrac = 1.0f;
	AListPosition pos = m_MatterTraceList.GetHeadPosition();
	while(pos)
	{
		i_shot_target * pItem = m_MatterTraceList.GetNext(pos);
		if( (pItem->GetRayTraceEnable()) && 
			(pItem != pExcept) &&
			(pItem->RayTrace(p, v * t , &rayTrace)) )
		{
			if( rayTrace.fFraction < fMinFrac )
			{
				fMinFrac	= rayTrace.fFraction;
				*pRayTrace	= rayTrace;
				if(ppTaget)
				{
					*ppTaget = pItem;
				}
			}
		}
	}
	if( fMinFrac < 1.0f )
		return true;
	else
		return false;
}
bool AWorld::RayTraceAI(const A3DVECTOR3& p,
						const A3DVECTOR3& v,
						float t,
						RAYTRACE * pRayTrace,
						i_shot_target* pExcept,
						i_shot_target ** ppTaget)
{

	J_EXCEPTION_RECORD(0)


	RAYTRACE rayTrace;
	float fMinFrac = 1.0f;
	AListPosition pos = m_AITraceList.GetHeadPosition();
	while(pos)
	{
		i_shot_target * pItem = m_AITraceList.GetNext(pos);
		if(	(pItem->GetRayTraceEnable()) &&
			(pItem != pExcept) &&
			(pItem->RayTrace(p, v * t , &rayTrace)) )
		{
			if( rayTrace.fFraction < fMinFrac )
			{
				fMinFrac	= rayTrace.fFraction;
				*pRayTrace	= rayTrace;
				if(ppTaget)
				{
					*ppTaget = pItem;
				}
			}
		}
	}

	if( fMinFrac < 1.0f )
	{
		return true;
	}
	return false;
}

bool AWorld::RayTraceSence(const A3DVECTOR3& p,
						   const A3DVECTOR3& v,
						   float t,
						   RAYTRACE * pRayTrace)
{

	J_EXCEPTION_RECORD(0)


	RAYTRACE rayTrace;
	if(m_pScn)
	{
		if( m_pScn->GetA3DESP() && m_pScn->GetA3DESP()->RayTrace(&rayTrace, const_cast<A3DVECTOR3&>(p), const_cast<A3DVECTOR3&>(v), t) )
		{
			*pRayTrace = rayTrace;
			return true;
		}
	}

	return false;
}

bool AWorld::RayTraceTerrain(const A3DVECTOR3& p,
							 const A3DVECTOR3& v,
							 float t,
							 RAYTRACE * pRayTrace)
{

	J_EXCEPTION_RECORD(0)

	RAYTRACE	rayTrace;
	if( m_pTrn)
	{
		if( m_pTrn->GetTerrain() && m_pTrn->GetTerrain()->RayTrace(const_cast<A3DVECTOR3&>(p), const_cast<A3DVECTOR3&>(v), t, &rayTrace) )
		{
			*pRayTrace = rayTrace;
			return true;
		}
	}

	return false;
}

bool AWorld::RayTraceAll(const A3DVECTOR3& p,
						 const A3DVECTOR3& v,
						 float t,
						 RAYTRACE * pRayTrace,
						 i_shot_target * pExcept,
						 i_shot_target ** ppTarget,
						 int * pType)
{

	J_EXCEPTION_RECORD(0)


	RAYTRACE Trace;

	pRayTrace->fFraction = 1.0f;

	i_shot_target * pTag = 0;

	if(pType)
		*pType = TYPE_ANYTHING;

	if (RayTraceTerrain(p,v,t,&Trace) && Trace.fFraction < pRayTrace->fFraction)
	{
		*pRayTrace = Trace;
		if(pType)
			*pType = TYPE_TERRAIN;
	}

	if (RayTraceSence(p,v,t,&Trace) && Trace.fFraction < pRayTrace->fFraction)
	{
		*pRayTrace = Trace;
		if(pType)
			*pType = TYPE_SENCE;
	}

	if (RayTraceMissile(p,v,t,&Trace,pExcept,&pTag) && Trace.fFraction < pRayTrace->fFraction)
	{
		*pRayTrace = Trace;
		if(ppTarget)
			*ppTarget = pTag;
		if(pType)
			*pType = TYPE_MISSILE;
	}
	if (RayTraceMatter(p,v,t,&Trace,pExcept,&pTag) && Trace.fFraction < pRayTrace->fFraction)
	{
		*pRayTrace = Trace;
		if(ppTarget)
			*ppTarget = pTag;
		if(pType)
			*pType = TYPE_MATTER;
	}
	if (RayTraceAI(p,v,t,&Trace,pExcept,&pTag) && Trace.fFraction < pRayTrace->fFraction)
	{
		*pRayTrace = Trace;
		if(ppTarget)
			*ppTarget = pTag;
		if(pType)
			*pType = TYPE_AI;
	}
	if (pRayTrace->fFraction < 1.0f)
		return true;
	
	return false;
}

bool AWorld::AddToRenderList(i_render_target * pTag)
{

	J_EXCEPTION_RECORD(0)

	if(!pTag || m_RenderList.Find(pTag))
		return false;
	m_RenderList.AddTail(pTag);
		return true;	
}

bool AWorld::DeleteFromRenderList(i_render_target * pTag)
{

	J_EXCEPTION_RECORD(0)

	AListPosition pos = m_RenderList.Find(pTag);
	if(pos)
	{
		m_RenderList.RemoveAt(pos);
		return true;
	}
	return false;
}

bool AWorld::RenderItemInHashTable(int* begin,int* end,A3DViewport * pCurrentViewport)
{

	J_EXCEPTION_RECORD(0)

	int * itor = begin;
	for(;itor!=end;++itor)
	{
		pair<AModelUnit**, bool> res = m_PVSHashTable.get(*itor);
		if(res.second &&(*(res.first))->GetVisibility() )
		{
			if(!(*(res.first))->Render(pCurrentViewport))
			{
				ADebug::Msg(0,"Render Item In Hash Table Error - ID = &d .",*itor);
				assert(0);
				return false;
			}
		}
	}
	return true;
}

bool AWorld::RenderAllInHashTable(A3DViewport * pCurrentViewport)
{

	J_EXCEPTION_RECORD(0)

	try
	{
		m_PVSHashTable.enum_element(PVSHashTableEnumFunctioner(this,pCurrentViewport));
	}
	catch(...)
	{
		return false;
	}
	return true;
}

void AWorld::BindEngine(A3DEngine *  pE)
{
	m_pA3DEngine	= pE;
	m_pA3DDevice	= m_pA3DEngine->GetA3DDevice();
	m_pA3DEngine->SetA3DCDS(m_pCDServer);
}
bool AWorld::BindFinished()
{
	return m_pA3DEngine&&m_pA3DDevice;
}
void AWorld::ForceRenderAll(bool b)
{
	if(m_pScn==NULL)
		return;
	m_bForceRender = b;
	m_pScn->ForceRenderAll(b);
}
void AWorld::RegisterToPVSHashTable(AModelUnit* _pItem,int _Index)
{
	m_PVSHashTable.put(_Index,_pItem);
}
void AWorld::UnregisterFromPVSHashTable(int _Index)
{
	m_PVSHashTable.erase(_Index);
}
bool AWorld::LoadNewRoute(BYTE* pBuf_Point,BYTE* pBuf_Route)
{
	return m_pScn->LoadNewRoute(pBuf_Point,pBuf_Route);
}
bool AWorld::SetCurrentRoute(bool bRoute, int iSample)
{
	return m_pScn->SetCurrentRoute(bRoute,iSample);
}

unsigned int AWorld::UpdateModelUnitFlagOfHashTable()
{	
	J_EXCEPTION_RECORD(0)
	if(!m_pScn)
	{
		return 0;
	}
	int cnt;
	int* begin = m_pScn->PVSRenderPtrBegin();
	int* end = m_pScn->PVSRenderPtrEnd();
	int* itor = begin;
	for(cnt = 0;itor!=end;++itor)
	{
		pair<AModelUnit**, bool> res = m_PVSHashTable.get(*itor);
		if(res.second &&(*(res.first))->GetVisibility() )
		{
			++cnt;
			(*(res.first))->SetPVSFlag(true);
		}
	}
	return cnt;
}
