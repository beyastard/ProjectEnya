#include "AModelUnit.h"
#include "AModelWraper.h"
#ifdef USING_MODEL_UNIT2
#include "AWorld.h"
#include "AGame.h"
#include "APointerAuto.h"
#include "AGamePlay.h"
#include "ABaseTypes.h"
#include "AScene.h"                                
#include "jexception.h"
 
#pragma message( "new version without stl" )

IMPLEMENT_ASERIAL(AModelUnit, AVisualUnit, 1)
 
void* VerifyObjectType(AObject* pObject,
	AClassInfo* pInfo, LPCSTR pszFile, int nLine)
{
	AClassInfo* p = pObject->GetClassInfo();
	while(p != NULL)
	{
		if(p == pInfo)
		{
			return (void*)pObject;
		}
		p	= p->GetBaseClass();
	}
	ADebug::Msg(0, "%s(%d): Invalid cast from \"%s\" to \"%s\" !",
		pszFile, nLine, pObject->GetClassInfo()->GetClassName(), pInfo->GetClassName());
	return NULL;
}

int AModelUnit::GetCount()
{
	return m_pModelWraper->GetSharedInfo()->SRI_list_ST_HH.GetCount();
}
AUString AModelUnit::GetNameByIndex(int index)
{
	const AListT<hh_item>& hh_list = m_pModelWraper->GetSharedInfo()->SRI_list_ST_HH;	
	AListPosition pos = hh_list.FindByIndex(index);
	if(pos)
	{
		const hh_item& item = hh_list.GetNext(pos);
		if(item.hh_index == index)
		{
			return item.hh_name;
		}
	}
	assert(0);
	return 	"";
}
AVariant AModelUnit::GetValueByIndex(int index)
{
	abase::pair<A3DFrame**,bool> p =	
	m_pModelWraper->m_FrameMapInt.get(index);
	A3DMATRIX4 res;
	if(p.second)
	{
		try
		{
			res = A3DMATRIX4((*(p.first))->GetAbsoluteTM());
		}
		catch(...)
		{
			assert(0);
		}
	}
	else
	{
		assert(0);
	}
	return res;  
}
AVariant AModelUnit::GetValueByName(AUString szName)
{
	abase::pair<A3DFrame**,bool> p =	
	m_pModelWraper->m_FrameMapStr.get(szName);
	A3DMATRIX4 res;
	if(p.second)
	{
		try
		{
			res = A3DMATRIX4((*(p.first))->GetAbsoluteTM());
		}
		catch(...)
		{
			assert(0);
		}
	}
	else
	{
		assert(0);
	}
	return res;  
}
bool AModelUnit::LogicRun(float __TimeSpan)
{
	if(!AVisualUnit::LogicRun(__TimeSpan))
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	if(!OnLogicRun(__TimeSpan))
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	m_pModelWraper->SetPos(GetPos());
	m_pModelWraper->SetDirAndUp(GetDir(),GetUp());
	return TickAnimation(__TimeSpan);
}
AUString AModelUnit::GetNameByValue(AVariant value)
{
	assert(0);
	return "";
}
int AModelUnit::FindValue(AVariant value)
{
	assert(0);
	return -1;
}
int AModelUnit::FindName(AUString szName)
{
	const AListT<hh_item>& hh_list = m_pModelWraper->GetSharedInfo()->SRI_list_ST_HH;	
	AListPosition pos = hh_list.GetHeadPosition();
	while(pos)
	{
		const hh_item& item = hh_list.GetNext(pos);
		if(item.hh_name == szName)
			return item.hh_index;
	}
	return -1;		
}
bool AModelUnit::RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACE * pRayTrace)
{
	if(GetRayTraceEnable())
	{
		return m_pModelWraper->RayTrace(vecStart,vecDelta,pRayTrace);
	}
	return false;
}
int	AModelUnit::OnShot(i_shot_attacker * pShotInfo)
{
	return 1;
}
bool AModelUnit::Render(A3DViewport * pCurrentViewport)
{
	if(!AVisualUnit::Render(pCurrentViewport))
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	if(GetVisibility())
	{
		A3DLight * pDynamicLight = g_pAGame->GetDynamicLight();
		A3DIBLLightGrid * pGrid;
		if(g_pAGame->GetAGamePlay()==NULL || g_pAGame->GetAGamePlay()->GetAWorld()==NULL)
			pGrid = NULL;
		else
			pGrid = g_pAGame->GetAGamePlay()->GetAWorld()->GetAScene()->GetLightGrid();
		if(pGrid)
		{
			if(m_iTicks%2==0)
			{
				UpdateLightInfor(GetPos());
			}
			
			g_pAGame->GetA3DEngine()->SetCurIBLLightParam(m_light_infor.LTParam);
			A3DLight * pLight = g_pAGame->GetA3DLight();
			A3DLIGHTPARAM param = pLight->GetLightparam();

			param.Diffuse = m_light_infor.LTParam.clLightDirect;
			param.Direction = m_light_infor.LTParam.vecLightDir;

			pLight->SetLightParam(&param);
			g_pAGame->GetA3DEngine()->GetA3DDevice()->SetAmbient(m_light_infor.LTParam.clLightAmbient);
			g_pAGame->GetA3DEngine()->GetA3DDevice()->SetTextureColorOP(0, pGrid->GetColorOP());
			//添加场景动态光源

			if( m_light_infor.LTParam.dynamicLightParam.Type != A3DLIGHT_FORCE_DWORD )
			{
				pDynamicLight->SetLightParam(&m_light_infor.LTParam.dynamicLightParam);
				pDynamicLight->TurnOn();
			}
			else
			{
				pDynamicLight->TurnOff();
			}
		}
		if(!m_pModelWraper->Render(pCurrentViewport))
		{
			ADebug::Msg(0,"Rander Err!");
			assert(0);
			return false;
		}
		if(pGrid)
		{
			g_pAGame->GetA3DEngine()->GetA3DDevice()->SetTextureColorOP(0, A3DTOP_MODULATE);
			pDynamicLight->TurnOff();
		}
	}
	return true;
}
void AModelUnit::UpdateLightInfor(const A3DVECTOR3& p)
{
	if(!m_pModelWraper->GetA3DModel())
	{
		return;
	}
	A3DVECTOR3 pp = p;
	pp.y += m_pModelWraper->GetA3DModel()->GetModelAABB().Extents.y;

	A3DIBLLightGrid * pGrid ;
	if(g_pAGame->GetAGamePlay()==NULL || g_pAGame->GetAGamePlay()->GetAWorld()==NULL)
		pGrid = NULL;
	else 
		pGrid = g_pAGame->GetAGamePlay()->GetAWorld()->GetAScene()->GetLightGrid();
	if(pGrid)
	{

		if(pGrid->GetEquivalentLightInfo(pp,
			&m_light_infor.LTParam.vecLightDir,
			&m_light_infor.LTParam.clLightDirect,
			&m_light_infor.LTParam.clLightAmbient,
			&m_light_infor.LTParam.dynamicLightParam))
		{
			m_light_infor.be_used = true;
		}
		else
		{
			m_light_infor.be_used = false;
		}
	}
	else
	{
		m_light_infor.be_used = false;
	}
}
int	AModelUnit::OnMsg(PAMSG pMsg)
{
	return 1;
}
int	AModelUnit::OnActionCallBack(const char * szCmd)
{
	return 1;
}
int	AModelUnit::OnChangeCallBack(const char * szName,void* buffer)
{
	return 1;
}
int	AModelUnit::OnAnimEndCallBack(const char * szName)
{
	return 1;
}
bool	AModelUnit::OnLogicRun(float __TimeSpan)
{
	return true;
}
bool AModelUnit::IsSubmited()
{
	return m_bSubmited;
}
bool AModelUnit::WithdrawModel()
{	
	assert(0);
	return false;
}
bool AModelUnit::SubmitModel()
{
	assert(0);
	return false;	
}
bool AModelUnit::Release()
{
	return true;
}
bool AModelUnit::editor_LogicRun()
{
	m_pModelWraper->SetPos(GetPos());
	m_pModelWraper->SetDirAndUp(GetDir(),GetUp());
	m_pModelWraper->TickAnimation();
	return true;
}
bool AModelUnit::TickAnimation(float fTimeSpan)
{
	if(m_bSynFrame)
	{
		return	m_pModelWraper->TickAnimation();
	}
	m_fTickAnimationTime += fTimeSpan;
	bool bTick = false;
	while(m_fTickAnimationTime>=ENGINETICKTIME)
	{
		if((m_bPVSFlag && m_bUsingPVSFlag)
			||(!m_bUsingPVSFlag))
		{
			if(m_pModelWraper->TickAnimation()==false)
			{
				ADebug::Msg(0,"TickAnimation error,%s",m_pModelWraper->GetA3DModel()->GetName());
				assert(0);
				return false;
			}
		}
		m_bPVSFlag = false;
		m_fTickAnimationTime-= ENGINETICKTIME;
		bTick = true;
	}
	assert(m_fTickAnimationTime<ENGINETICKTIME);
	if(!bTick)
	{
		//if((m_bPVSFlag && m_bUsingPVSFlag)
		//	||(!m_bUsingPVSFlag))
		{
			if(UpdateFrame()==false)
				return false;
		}
	}
	return true;
}

AData* AModelUnit::CreateData()
{
	return new AModelUnitData;
}
bool AModelUnit::OnWriteData(AData* pData)
{
	VERIFY_OBJECT_TYPE(pData, AModelUnitData);
	if(!AVisualUnit::OnWriteData(pData))
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	((AModelUnitData*)pData)->m_model_datum	= m_model_datum;
	return true;
}
bool AModelUnit::OnReadData(AData* pData)
{
	VERIFY_OBJECT_TYPE(pData, AModelUnitData);
	if(!AVisualUnit::OnReadData(pData))
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	m_model_datum = ((AModelUnitData*)pData)->m_model_datum;
	
	if(!m_pModelWraper->Init(m_model_datum.config_file))
	{
		return false;
	}

	UpdateLightInfor(GetPos());
	return true;
}
AModelUnit::AModelUnit()
{
	m_bUsingPVSFlag = false;
	m_bPVSFlag = false;
	m_bSynFrame = false;
	m_fTickAnimationTime = 0.f;
	m_bSubmited = false;
	m_light_infor.be_used = false;
	m_light_infor.LTParam.dynamicLightParam.Type = A3DLIGHT_FORCE_DWORD;
	m_pModelWraper = new AModelWraper;
}
AModelUnit::~AModelUnit()
{
	delete m_pModelWraper;
}

bool AModelUnit::BindGame(AGame * m_pG)
{
	if(!AVisualUnit::BindGame(m_pG))
	{
		assert(0);
		return false;
	}
	m_pModelWraper->BindEngine(m_pA3DEngine);
	m_pModelWraper->BindListener(this);
	return true;
}
bool AModelUnit::GetHHPose(int index,A3DVECTOR3& pos,A3DVECTOR3& dir,A3DVECTOR3& up)
{
	A3DMATRIX4 matrix = GetValueByIndex(index);
	pos = matrix. GetRow(3);
	dir = matrix. GetRow(2);
	up  = matrix. GetRow(1);
	return true;
}

bool AModelUnit::UpdateFrame()
{
	return true;
}
#endif