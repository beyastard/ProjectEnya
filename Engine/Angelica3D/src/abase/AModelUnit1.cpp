#include "AModelUnit.h"
#include "AModelWraper.h"
#ifndef USING_MODEL_UNIT2
#include "AWorld.h"
#include "AGame.h"
#include "APointerAuto.h"
#include "AGamePlay.h"
#include "ABaseTypes.h"
#include "AScene.h"
#include "jexception.h"

#pragma message( "old version with stl" )

IMPLEMENT_ASERIAL(AModelUnit, AVisualUnit, 1)

bool AModelUnit::GetAnimationNameSet(const char * file_name,AListT<AUString>&res)
{
	return AModelWraper::GetAnimationNameSet(file_name,res);
}

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

//---------------------------------------------------------------------------------------

AModelUnit::AModelUnit()
{
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
	m_pModelWraper->BindEngineAndListener(m_pA3DEngine,this);
	return true;
}

AData* AModelUnit::CreateData()
{
	return new AModelUnitData;
}
bool AModelUnit::OnWriteData(AData* pData)
{

	J_EXCEPTION_RECORD(0);


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

	J_EXCEPTION_RECORD(0)


	VERIFY_OBJECT_TYPE(pData, AModelUnitData);
	if(!AVisualUnit::OnReadData(pData))
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	m_model_datum = ((AModelUnitData*)pData)->m_model_datum;
	m_pModelWraper->End();
	m_pModelWraper->LoadConfigFile(m_model_datum.config_file);
	
	if(m_model_datum.state == AUString(""))
	{
		m_pModelWraper->Begin();
		m_model_datum.state = m_pModelWraper->GetCurrentStateName().c_str();
	}
	else
	{
		m_pModelWraper->SetStateByName(m_model_datum.state);
	}
	UpdateLightInfor(GetPos());
	return true;
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
int	AModelUnit::OnShot(i_shot_attacker * pShotInfo)
{
	return 1;
}
bool	AModelUnit::OnLogicRun(float __TimeSpan)
{
	return true;
}
bool AModelUnit::RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACE * pRayTrace)
{

	J_EXCEPTION_RECORD(0)


	if(GetRayTraceEnable())
	{
		return m_pModelWraper->RayTrace(vecStart,vecDelta,pRayTrace);
	}
	return false;
}
bool AModelUnit::Render(A3DViewport * pCurrentViewport)
{

	J_EXCEPTION_RECORD(0)


	if(!AVisualUnit::Render(pCurrentViewport))
	{
		ADebug::Msg(0,"%s %d error",__FILE__,__LINE__);
		return false;
	}
	if(GetVisibility())
	{
		A3DLight * pDynamicLight = g_pAGame->GetDynamicLight();
		A3DIBLLightGrid * pGrid = g_pAGame->GetAGamePlay()->GetAWorld()->GetAScene()->GetLightGrid();
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
bool AModelUnit::LogicRun(float __TimeSpan)
{

	J_EXCEPTION_RECORD(0)


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
int  AModelUnit::GetStateIndex()
{
	return m_pModelWraper->GetState();
}
bool AModelUnit::ForcePlayAnim(const char * anim_name)
{
	return m_pModelWraper->ForcePlayAnim(anim_name);
}
bool AModelUnit::NextState()
{
	if(!m_pModelWraper->NextState())
	{
		return false;
	}
	SetState(m_pModelWraper->GetCurrentStateName().c_str());
	return true;
} 
A3DModel * AModelUnit::GetA3DModel()
{
	return m_pModelWraper->GetA3DModel();
}
A3DVECTOR3 AModelUnit::GetModelCenterPoint()
{
	return m_pModelWraper->GetModelCenterPoint();
}
A3DVECTOR3 AModelUnit::GetModelSize()
{
	return m_pModelWraper->GetModelSize();
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
	m_fTickAnimationTime += fTimeSpan;
	while(m_fTickAnimationTime>=ENGINETICKTIME)
	{
		if(m_pModelWraper->TickAnimation()==false)
		{
			assert(0);
			return false;
		}
		m_fTickAnimationTime-= ENGINETICKTIME;
	}
	assert(m_fTickAnimationTime<ENGINETICKTIME);
	return true;
}/*
unsigned long AModelUnit::GetSupportAnimCount()
{
	return m_pModelWraper->EnumSupportAnimName().size();
}
AUString AModelUnit::GetSupportAnimName(unsigned long index)
{
	if(index >= GetSupportAnimCount()) return "";
	return m_pModelWraper->EnumSupportAnimName()[index].c_str();
}
unsigned long AModelUnit::GetSupportSateCount()
{
	return m_pModelWraper->EnumSupportStateName().size();
}
AUString AModelUnit::GetSupportStateName(unsigned long index)
{
	if(index >= GetSupportSateCount()) return "";
	return m_pModelWraper->EnumSupportStateName()[index].c_str();
}*/
bool AModelUnit::GetHHPose(int index,A3DVECTOR3& pos,A3DVECTOR3& dir,A3DVECTOR3& up)
{

	J_EXCEPTION_RECORD(0)


	bool is_ok;
	try
	{
		A3DMATRIX4 matrix = A3DMATRIX4((m_pModelWraper->EnumHHPtr()[index])->GetAbsoluteTM());
		pos = matrix. GetRow(3);
		dir = matrix. GetRow(2);
		up  = matrix. GetRow(1);
		is_ok = true;
	}
	catch(...)
	{
		is_ok = false;
	}
	return is_ok;
}
bool AModelUnit::AppendAssistantModel(const char* model_name,const char* hh_name,const char* cc_name)
{
	return m_pModelWraper->AppendAssistantModel(model_name,hh_name,cc_name);
}
bool AModelUnit::DeleteAssistantModel(const char* model_name)
{
	return m_pModelWraper->DeleteAssistantModel(model_name);
}
bool AModelUnit::NoitfyAssistantModelPlayAnim(const char* model_name,const char * anim_name)
{
	return m_pModelWraper->NoitfyAssistantModelPlayAnim(model_name,anim_name);
}/*
bool AModelUnit::NoitfyAssistantModelPlayGFX(const char* model_name,const char * gfx_name)
{
	return m_pModelWraper->NoitfyAssistantModelPlayGFX(model_name,gfx_name);
}
bool AModelUnit::NoitfyAssistantModelPlayGFXOnModel(const char* model_name,const char * gfx_name,const char * hh_name)
{
	return m_pModelWraper->NoitfyAssistantModelPlayGFXOnModel(model_name,gfx_name,hh_name);
}*/
A3DModel * AModelUnit::GetAssistantModel(const char* model_name)
{
	return m_pModelWraper->GetAssistantModel(model_name);
}

bool AModelUnit::AddActionToHead(const char * state_name,void* buffer)
{
	return m_pModelWraper->AddActionToHead(state_name,buffer);
}
bool AModelUnit::AppendActionToTail(const char * state_name,void* buffer)
{
	return m_pModelWraper->AppendActionToTail(state_name,buffer);
}
bool AModelUnit::AppendLowLevelActionToTail(const char * state_name)
{
	return m_pModelWraper->AppendLowLevelActionToTail(state_name);
}
bool AModelUnit::AddLowLevelActionToHead(const char * state_name)
{
	return m_pModelWraper->AddLowLevelActionToHead(state_name);
}
bool AModelUnit::IsInLoopAction()
{
	return m_pModelWraper->IsInLoopAction();
}
void AModelUnit::ClearActionQueue()
{
	m_pModelWraper->ClearActionQueue();
}
bool AModelUnit::PlayGFX(const char* gfx_name)
{
	return m_pModelWraper->PlayGFX(gfx_name);
}
bool AModelUnit::PlayGFX(const char* gfx_name,const A3DVECTOR3& pos)
{
	return m_pModelWraper->PlayGFX(gfx_name,pos);
}
bool AModelUnit::PlayGFX(const char* gfx_name,const char* hh_name)
{
	return m_pModelWraper->PlayGFX(gfx_name,hh_name);
}
bool AModelUnit::HasLoopInAction()
{
	return m_pModelWraper->HasLoopInAction();
}

void AModelUnit::UpdateLightInfor(const A3DVECTOR3& p)
{

	J_EXCEPTION_RECORD(0)


	if(!m_pModelWraper->GetA3DModel())
	{
		return;
	}
	A3DVECTOR3 pp = p;
	pp.y += m_pModelWraper->GetA3DModel()->GetModelAABB().Extents.y;

	A3DIBLLightGrid * pGrid = g_pAGame->GetAGamePlay()->GetAWorld()->GetAScene()->GetLightGrid();
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

bool AModelUnit::IsEmptyActionQueue()
{
	return m_pModelWraper->IsEmptyActionQueue();
}
bool AModelUnit::IsIdle()
{
	return m_pModelWraper->IsIdle();
}

int AModelUnit::GetCount()
{
	return 	m_pModelWraper->EnumHHName().size();
}
AUString AModelUnit::GetNameByIndex(int index)
{
	return 	AUString(m_pModelWraper->EnumHHName()[index].c_str());
}
AVariant AModelUnit::GetValueByIndex(int index)
{

	J_EXCEPTION_RECORD(0)


	A3DMATRIX4 res;
	try
	{
		res = A3DMATRIX4((m_pModelWraper->EnumHHPtr()[index])->GetAbsoluteTM());
	}
	catch(...)
	{
		assert(0);
	}
	return res;  
}
AVariant AModelUnit::GetValueByName(AUString szName)
{
	A3DMATRIX4 res;
	int index = FindName(szName);
	if(index != -1)
	{
		res = GetValueByIndex(index);
	}
	return res;
}
AUString AModelUnit::GetNameByValue(AVariant value)
{
	assert(0);
	return "";	
}
int AModelUnit::FindValue(AVariant value)
{
	assert(0);
	return 0;	
}
int AModelUnit::FindName(AUString szName)
{

	J_EXCEPTION_RECORD(0)

	std::vector<string>& name_set = m_pModelWraper->EnumHHName();
	std::vector<string>::const_iterator itor = name_set.begin();
	int index = -1;
	for(;itor != name_set.end();++itor,++index)
	{
		string item = (*itor);
		if(item == szName.GetBuffer())
		{
			return index + 1;
		}
	}

	return -1;	
}
void AModelUnit::SetState(const char * s)
{
	m_model_datum.state = s;
	m_pModelWraper->SetStateByName(s);
}

bool AModelUnit::GetHHNameSet(const char * file_name,AListT<HH_infor>& res)
{
	return AModelWraper::GetHHNameSet(file_name,res);
}
bool AModelUnit::GetInvolvedModel(const char * file_name,AListT<AUString>& res)
{
	return AModelWraper::GetInvolvedModel(file_name,res);
}
bool AModelUnit::GetInvolvedGFX(const char * file_name,AListT<AUString>& res)
{
	return AModelWraper::GetInvolvedGFX(file_name,res);
}
#endif