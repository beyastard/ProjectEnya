#ifndef __AMODELUNIT2__H__
#define __AMODELUNIT2__H__

#include "AModelUnitData.h"
#include "AVisualUnit.h"
#include "AUnitInWorldInterface.h"
#include "AModelWraper2.h"


class AModelUnit :	public AVisualUnit, 
					public i_shot_target, 
					public i_host_listener,
					public ASet
{

public:
	
	static bool GetHHNameSet(const char * file_name,AListT<HH_infor>& res)
	{
		return AModelWraper::GetHHNameSet(file_name,res);
	}
	static bool GetAnimationNameSet(const char * file_name,AListT<AUString>&res)
	{
		return AModelWraper::GetAnimationNameSet(file_name,res);
	}
	static bool GetInvolvedModel(const char * file_name,AListT<AUString>& res)
	{
		return AModelWraper::GetInvolvedModel(file_name,res);
	}
	static bool GetInvolvedGFX(const char * file_name,AListT<AUString>& res)
	{
		return AModelWraper::GetInvolvedGFX(file_name,res);
	}

protected:
	
	bool	m_bPVSFlag;
	bool	m_bUsingPVSFlag;

	bool	m_bSynFrame;
	float	m_fTickAnimationTime;
	AMODEL_KERNEL_DATUM	m_model_datum;

	struct light_infor
	{
		A3DIBLLIGHTPARAM LTParam;
		bool			 be_used;
	} m_light_infor;

	bool	m_bSubmited;
	AModelWraper* m_pModelWraper;

public://FOR ASET

	virtual int GetCount();
	virtual AUString GetNameByIndex(int index);
	virtual AVariant GetValueByIndex(int index);
	virtual AVariant GetValueByName(AUString szName);
	virtual AUString GetNameByValue(AVariant value);
	virtual int FindValue(AVariant value);
	virtual int FindName(AUString szName);;
	
public://FOR CALLBACK
	
	virtual int receive_feedback(const char * cmd){return OnActionCallBack(cmd);}
	virtual int anim_change(const char* name,void* pbuffer){return OnChangeCallBack(name,pbuffer);}
	virtual int anim_end(const char* name){return OnAnimEndCallBack(name);}

public://FOR i_SHORT
	
	virtual int	 OnShot(i_shot_attacker * pShotInfo);
	virtual bool RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACE * pRayTrace);
	virtual bool GetRayTraceEnable(){return m_model_datum.ray_trace_enable;}
	virtual void SetRayTraceEnable(bool b){m_model_datum.ray_trace_enable = b;}

public://FOR i_RENDER

	virtual bool Render(A3DViewport * pCurrentViewport);
	virtual bool GetVisibility(){return m_model_datum.visibility;}
	virtual void SetVisibility(bool b){m_model_datum.visibility = b;}
	virtual void UpdateLightInfor(const A3DVECTOR3& p);

protected://FOR UNIT
	
	virtual AData*		CreateData();
	virtual bool		OnWriteData(AData* pData);
	virtual bool		OnReadData(AData* pData);
	virtual int			OnActionCallBack(const char * szCmd);
	virtual int			OnChangeCallBack(const char * szName,void* pbuffer);
	virtual int			OnAnimEndCallBack(const char * szName);
	virtual bool		OnLogicRun(float __TimeSpan);
	virtual bool		UpdateFrame();

public:

	AModelUnit();
	virtual ~AModelUnit();
	virtual bool Release();

public:

	virtual bool LogicRun(float __TimeSpan);
	virtual bool SubmitModel();
	virtual bool WithdrawModel();
	virtual bool IsSubmited();
	virtual int	 OnMsg(PAMSG pMsg);
	virtual bool BindGame(AGame * m_pG);

public:

	inline void SetPVSFlag(bool b)
	{
		if(m_bUsingPVSFlag)
			m_bPVSFlag = b;
		else
			m_bPVSFlag = true;
	}

	inline A3DVECTOR3 GetPos(){return m_model_datum.pos;}
	void SetPos(const A3DVECTOR3& p){m_model_datum.pos = p;}

	inline A3DVECTOR3 GetDir(){return m_model_datum.dir;}
	inline void SetDir(const A3DVECTOR3& d){m_model_datum.dir = d;}

	inline A3DVECTOR3 GetUp(){return m_model_datum.up;}
	inline void SetUp(const A3DVECTOR3& u){m_model_datum.up = u;}
	
	inline void SetDirAndUp(const A3DVECTOR3 & __vecDir,const A3DVECTOR3&  __vecUp){m_model_datum.dir = __vecDir;m_model_datum.up  = __vecUp;}

	inline AUString GetConfigFileName(){return m_model_datum.config_file;}

	inline int  GetStateIndex(){return m_pModelWraper->GetStateIndex();}
	inline bool NextState(){return m_pModelWraper->NextState();}
	
	bool GetHHPose(int index,A3DVECTOR3& pos,A3DVECTOR3& dir,A3DVECTOR3& up);

public:

	inline A3DModel * GetA3DModel()
	{
		return m_pModelWraper->GetA3DModel();
	}
	inline A3DVECTOR3 GetModelCenterPoint()
	{
		return m_pModelWraper->GetModelCenterPoint();
	}
	inline A3DVECTOR3 GetModelSize()
	{
		return m_pModelWraper->GetModelSize();
	}

public:

	inline bool AddActionToHead(const char * state_name,void* buffer = 0)
	{
		return m_pModelWraper->AddHighLevelActionToQueueHead(state_name,buffer);
	}
	inline bool AppendActionToTail(const char * state_name,void* buffer = 0)
	{
		return m_pModelWraper->AppendHighLevelActionToQueueTail(state_name,buffer);
	}
	inline void ClearActionQueue()
	{
		m_pModelWraper->ClearActionQueue();
	}
	inline bool ForcePlayAnim(const char * anim_name)
	{
		return m_pModelWraper->ForcePlayAnim(anim_name);
	}
	inline bool AppendLowLevelActionToTail(const char * state_name)
	{
		return m_pModelWraper->AppendLowLevelActionToQueueTail(state_name);
	}
	inline bool AddLowLevelActionToHead(const char * state_name)
	{
		return m_pModelWraper->AddLowLevelActionToQueueHead(state_name);
	}
	inline bool IsInLoopAction()
	{
		return m_pModelWraper->IsInLoopAction();
	}
	inline bool HasLoopInAction()
	{
		return m_pModelWraper->HasLoopInAction();
	}
	inline bool IsEmptyActionQueue()
	{
		return m_pModelWraper->IsEmptyActionQueue();
	}
	inline bool IsIdle()
	{
		return m_pModelWraper->IsIdle();
	}

public:

	inline bool AppendAssistantModel(const char* model_name,const char* hh_name = "",const char* cc_name = "")
	{
		return m_pModelWraper->AppendAssistantModel(model_name,hh_name,cc_name);
	}
	inline bool DeleteAssistantModel(const char* model_name)
	{
		return m_pModelWraper->DeleteAssistantModel(model_name);
	}
	inline bool NoitfyAssistantModelPlayAnim(const char* model_name,const char * anim_name)
	{
		return m_pModelWraper->NoitfyAssistantModelPlayAnim(model_name,anim_name);
	}
	inline A3DModel * GetAssistantModel(const char* model_name)
	{
		return m_pModelWraper->GetAssistantModel(model_name);
	}
	inline bool PlayGFX(const char* gfx_name)
	{
		return m_pModelWraper->PlayGFX(gfx_name);
	}
	inline bool PlayGFX(const char* gfx_name,const A3DVECTOR3& pos)
	{
		return m_pModelWraper->PlayGFX(gfx_name,pos);
	}
	inline bool PlayGFX(const char* gfx_name,const char* hh_name)
	{
		return m_pModelWraper->PlayGFX(gfx_name,hh_name);
	}

public:

#ifdef USING_MODEL_UNIT2

	DECLARE_ASERIAL(AModelUnit)

#endif

	inline void SetTickSyn(bool s){m_bSynFrame = s;}
	bool TickAnimation(float fTimeSpan);
	bool editor_LogicRun();
};

#endif // __AMODELUNIT2__H__