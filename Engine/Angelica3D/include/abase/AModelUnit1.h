#ifndef __AMODELUNIT1__H__
#define __AMODELUNIT1__H__

#include "AModelUnitData.h"
#include "AVisualUnit.h"
#include "AUnitInWorldInterface.h"
#include "AModelWraper.h"

class AModelUnit :	public AVisualUnit, 
					public i_shot_target, 
					public i_host_listener,
					public ASet
{

public:
	
	static bool GetHHNameSet(const char * file_name,AListT<HH_infor>& res);
	static bool GetAnimationNameSet(const char * file_name,AListT<AUString>&res);
	static bool GetInvolvedModel(const char * file_name,AListT<AUString>& res);
	static bool GetInvolvedGFX(const char * file_name,AListT<AUString>& res);

protected:

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
	virtual int anim_change(const char* name,void* pbuffer)
	{return OnChangeCallBack(name,pbuffer);}
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

	A3DVECTOR3 GetPos(){return m_model_datum.pos;}
	void SetPos(const A3DVECTOR3& p){m_model_datum.pos = p;}

	A3DVECTOR3 GetDir(){return m_model_datum.dir;}
	void SetDir(const A3DVECTOR3& d){m_model_datum.dir = d;}

	A3DVECTOR3 GetUp(){return m_model_datum.up;}
	void SetUp(const A3DVECTOR3& u){m_model_datum.up = u;}
	
	inline void SetDirAndUp(const A3DVECTOR3 & __vecDir,const A3DVECTOR3&  __vecUp){m_model_datum.dir = __vecDir;m_model_datum.up  = __vecUp;}

	AUString GetConfigFileName(){return m_model_datum.config_file;}

	AUString GetState(){return m_model_datum.state;}
	void SetState(const char * s);

	int  GetStateIndex();
	bool NextState();
	
	bool GetHHPose(int index,A3DVECTOR3& pos,A3DVECTOR3& dir,A3DVECTOR3& up);

//	unsigned long GetSupportAnimCount();
//	AUString GetSupportAnimName(unsigned long index);

//	unsigned long GetSupportSateCount();
//	AUString GetSupportStateName(unsigned long index);

public:

	A3DModel * GetA3DModel();
	A3DVECTOR3 GetModelCenterPoint();
	A3DVECTOR3 GetModelSize();

public:

	bool AddActionToHead(const char * state_name,void* buffer = 0);
	bool AppendActionToTail(const char * state_name,void* buffer = 0);
	void ClearActionQueue();
	bool ForcePlayAnim(const char * anim_name);
	bool AppendLowLevelActionToTail(const char * state_name);
	bool AddLowLevelActionToHead(const char * state_name);
	bool IsInLoopAction();
	bool HasLoopInAction();
	bool IsEmptyActionQueue();
	bool IsIdle();

public:

	bool AppendAssistantModel(const char* model_name,const char* hh_name = "",const char* cc_name = "");
	bool DeleteAssistantModel(const char* model_name);
	bool NoitfyAssistantModelPlayAnim(const char* model_name,const char * anim_name);
//	bool NoitfyAssistantModelPlayGFX(const char* model_name,const char * gfx_name);
//	bool NoitfyAssistantModelPlayGFXOnModel(const char* model_name,const char * gfx_name,const char * hh_name);
	A3DModel * GetAssistantModel(const char* model_name);
	bool PlayGFX(const char* gfx_name);
	bool PlayGFX(const char* gfx_name,const A3DVECTOR3& pos);
	bool PlayGFX(const char* gfx_name,const char* hh_name);

public:

#ifndef USING_MODEL_UNIT2

	DECLARE_ASERIAL(AModelUnit)

#endif
		
	bool TickAnimation(float fTimeSpan);
	bool editor_LogicRun();
};
#endif // __AMODELUNIT1__H__