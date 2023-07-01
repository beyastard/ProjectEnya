#ifndef __AMODEL_WRAPER2_H__
#define __AMODEL_WRAPER2_H__

#include "ADebug.h"
#include "ABaseA3D.h"
#include "AUString.h"
#include "AUList.h"
#include "hash_table_wrapper.h"
#include "AModelWraperSharedData.h"
#include "AUnitInWorldInterface.h"

using namespace abase;

struct i_host_listener;

class  AModelWraper
{

public:

	static bool GetAnimationNameSet(const char * file_name,AListT<AUString>&res);
	static bool GetHHNameSet(const char * file_name,AListT<HH_infor>& res);
	static bool GetInvolvedModel(const char * file_name,AListT<AUString>& res);
	static bool GetInvolvedGFX(const char * file_name,AListT<AUString>& res);

private:
	
	shared_role_info*			m_pSharedInfo;
	i_host_listener*			m_pListener;
	A3DEngine*					m_pEngine;
	A3DModel*					m_pMainModel;
	A3DVECTOR3					m_LogicPos;
	A3DVECTOR3					m_LogicDir;
	A3DVECTOR3					m_LogicUp;
	int							m_CurrentStateIndex;
	bool						m_ReadyToPlayAnimation;
	AUString					m_LoopNextAnimationName;
	int							m_CurrentAnimationPrivilege;
	int							m_LoopNextAnimationDelay;
	AUString					m_AnimationInEngine;
	AListT<ll_action>			m_CurrentActionQueue;						

public:

	hashtab<A3DModel*,AUString,_hash_function_austring>	m_AssistantModelMap;
	hashtab<A3DFrame*,AUString,_hash_function_austring>	m_FrameMapStr;
	hashtab<A3DFrame*,int,_hash_function>	m_FrameMapInt;

private:

	static void call_back_logic_event(A3DMODEL_LOGIC_EVENT * pLogicEvent, LPVOID pArg);
	static void call_back_action_end(A3DModel * pNoUsing, LPVOID pArg);
	void releaseModel();
	bool loadModel(const char* str,const char* an);
	void fillConnectPointSet();
	void recursionFindConnectPoint(A3DFrame * pFrame);
	bool handleActionQueue();

public:

	bool AppendAssistantModel(const char* model_name,const char* hh_name = "" , const char* cc_name = "" );
	bool DeleteAssistantModel(const char* model_name);
	bool NoitfyAssistantModelPlayAnim(const char* model_name,const char * anim_name);
	A3DModel * GetAssistantModel(const char* model_name);

public:

	inline 	shared_role_info * GetSharedInfo() const {return m_pSharedInfo;}
	inline void BindEngine(A3DEngine* pe){m_pEngine = pe;}
	inline void BindListener(i_host_listener * pl){m_pListener = pl;}
	inline A3DModel * GetA3DModel(){return m_pMainModel;}
	inline A3DVECTOR3 GetPos() const {return m_LogicPos;}
	inline A3DVECTOR3 GetDir() const {return m_LogicDir;}
	inline A3DVECTOR3 GetUp() const {return m_LogicUp;}

	void SetPos(const A3DVECTOR3& p);
	void SetDirAndUp(const A3DVECTOR3& d,const A3DVECTOR3& u);

	A3DVECTOR3 GetModelCenterPoint();
	A3DVECTOR3 GetModelSize();
	
	bool Render(A3DViewport * pCurrentViewport);
	bool TickAnimation();
	bool RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACE * pRayTrace);

public:

	AModelWraper();
	~AModelWraper();

public:

	bool PlayGFX(const char* gfx_name);
	bool PlayGFX(const char* gfx_name,const A3DVECTOR3& pos);
	bool PlayGFX(const char* gfx_name,const char* hh_name);

public:

	bool Init(const char* pseudo_file_name);
	bool NextState();
	inline int GetStateIndex(){return m_CurrentStateIndex;}
	void Release();

public:
	
	bool AddHighLevelActionToQueueHead(const char * state_name,void * p_buffer = 0);
	bool AppendHighLevelActionToQueueTail(const char * state_name,void * p_buffer = 0);
	bool AddLowLevelActionToQueueHead(const char * state_name,void * p_buffer = 0);
	bool AppendLowLevelActionToQueueTail(const char * state_name,void * p_buffer = 0);
	bool IsInLoopAction();
	bool HasLoopInAction();
	bool IsIdle();
	void ClearActionQueue();
	bool IsEmptyActionQueue();
	bool ForcePlayAnim(const char * anim_name);
};

#endif//__AMODEL_WRAPER2_H__