#include "AModelUnit.h"
#include "AModelWraper.h"
#ifdef USING_MODEL_UNIT2
#include "AUnitInWorldInterface.h"
//#include "mtime.h"

#pragma message( "new version without stl" )

//////////////////////////////////////////////////////////////
    
struct enum_fun_get_all_anim_key
{
	AListT<AUString>& res;
	enum_fun_get_all_anim_key(AListT<AUString>&r):res(r){}
	void operator()(const Hashtable_node<hl_action*,AUString>* b)
	{
		while(b)
		{
			res.AddTail(b->_key);
			b = b->_next;
		}
	}
};

bool AModelWraper::GetAnimationNameSet(const char * pseudo_file_name,AListT<AUString>&res)
{
	pair<shared_role_info**, bool> f = 
		g_ConfigFileCImp.all_config.get(pseudo_file_name);
	if(!f.second)
	{
		ADebug::Msg(0,"can not open pseudo file named < %s >.",pseudo_file_name);
		assert(0);
		return false;
	}
	shared_role_info*			pSharedInfo;
	pSharedInfo = *f.first;
	pSharedInfo->SRI_map_HL_LL.enum_buckets(
		enum_fun_get_all_anim_key(res));
	return true;	
}
bool AModelWraper::GetHHNameSet(const char * pseudo_file_name,AListT<HH_infor>& res)
{
	pair<shared_role_info**, bool> f = 
		g_ConfigFileCImp.all_config.get(pseudo_file_name);
	if(!f.second)
	{
		ADebug::Msg(0,"can not open pseudo file named < %s >.",pseudo_file_name);
		assert(0);
		return false;
	}
	shared_role_info*			pSharedInfo;
	pSharedInfo = *f.first;
	const AListT<hh_item>& hh_list = pSharedInfo->SRI_list_ST_HH;
	AListPosition pos = hh_list.GetHeadPosition();
	while(pos)
	{
		HH_infor item;
		item.index = hh_list.GetAt(pos).hh_index;
		item.name = hh_list.GetAt(pos).hh_name;
		hh_list.GetNext(pos);
		res.AddTail(item);
	}
	return true;
}
bool AModelWraper::GetInvolvedModel(const char * pseudo_file_name,AListT<AUString>& res)
{
	pair<shared_role_info**, bool> f = 
		g_ConfigFileCImp.all_config.get(pseudo_file_name);
	if(!f.second)
	{
		ADebug::Msg(0,"can not open pseudo file named < %s >.",pseudo_file_name);
		assert(0);
		return false;
	}
	shared_role_info*			pSharedInfo;
	pSharedInfo = *f.first;
	const AListT<model_item>& model_list = pSharedInfo->SRI_list_MOD;
	AListPosition pos = model_list.GetHeadPosition();
	while(pos)
	{
		AUString item;
		item = model_list.GetNext(pos).model_name;
		res.AddTail(item);
	}
	return true;
}
bool AModelWraper::GetInvolvedGFX(const char * pseudo_file_name,AListT<AUString>& res)
{
	pair<shared_role_info**, bool> f = 
		g_ConfigFileCImp.all_config.get(pseudo_file_name);
	if(!f.second)
	{
		ADebug::Msg(0,"can not open pseudo file named < %s >.",pseudo_file_name);
		assert(0);
		return false;
	}
	shared_role_info*			pSharedInfo;
	pSharedInfo = *f.first;
	{
		const AListT<gfx_item>& gfx_list = pSharedInfo->SRI_list_GFX;
		AListPosition pos = gfx_list.GetHeadPosition();
		while(pos)
		{
			AUString item;
			item = gfx_list.GetNext(pos).gfx_name;
			res.AddTail(item);
		}
	}
	{
		const AListT<gfx_item>& gfx_list = pSharedInfo->SRI_list_CNV_GFX;
		AListPosition pos = gfx_list.GetHeadPosition();
		while(pos)
		{
			AUString item;
			item = gfx_list.GetNext(pos).gfx_name;
			res.AddTail(item);
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////
namespace enumfun
{
	//由于 hashtable 没有实现begin\end所以将遍历的代码写在这里了
	
	//这个仿函负责将hash表中的所有模型释放掉
	struct enum_fun_release
	{
		A3DEngine* pEngine;
		enum_fun_release(A3DEngine* pE):pEngine(pE){}
		void operator()(A3DModel* pItem)
		{
			if(pItem)
			{
				pEngine->GetA3DModelMan()->ReleaseModel(pItem);
			}
		}
	};
	//这个仿函负责将hash表中的所有没有连接到父节点的模型设置到指定位置
	struct enum_fun_pos
	{
		A3DVECTOR3 pos;
		enum_fun_pos(const A3DVECTOR3& p):pos(p){}
		void operator()(A3DModel* pItem)
		{
			if(pItem && (pItem->GetParentFrame()==0))
			{
				pItem->SetPos(pos);
			}
		}
	};
	//这个仿函负责将hash表中的所有没有连接到父节点的模型设置到指定姿态
	struct enum_fun_dir_up
	{
		A3DVECTOR3 dir;
		A3DVECTOR3 up;
		enum_fun_dir_up(const A3DVECTOR3& d,const A3DVECTOR3& u):dir(d),up(u){}
		void operator()(A3DModel* pItem)
		{
			if(pItem && (pItem->GetParentFrame()==0))
			{
				pItem->SetDirAndUp(dir,up);
			}
		}
	};
	//这个仿函负责渲染hash表中的所有没有连接到父节点的模型
	struct enum_fun_render
	{
		A3DViewport * pV;
		enum_fun_render(A3DViewport * pv):pV(pv){}
		void operator()(A3DModel* pItem)// throw(const char*)
		{
			if(pItem && (pItem->GetParentFrame()==0))
			{
				if(!pItem->Render(pV))
				{
					throw(pItem->GetName());
				}
			}
		}
	};
	//这个仿函负责tick hash表中的所有的模型
	struct enum_fun_tickanim
	{
		void operator()(A3DModel* pItem)// throw(const char*)
		{
			if(pItem && (pItem->GetParentFrame()==0))
			{
				if(!pItem->TickAnimation())
				{
					throw(pItem->GetName());
				}
			}
		}
	};
	//这个仿函负责 ray trace hash表中的所有没有连接到父节点的模型
	struct enum_fun_raytrace
	{
		A3DVECTOR3 start;
		A3DVECTOR3 delt;
		RAYTRACE * pRT;

		enum_fun_raytrace(
			const A3DVECTOR3& vecStart,
			const A3DVECTOR3& vecDelta,
			RAYTRACE * pRayTrace):
			start(vecStart),
			delt(vecDelta),
			pRT(pRayTrace)
			{}

		void operator()(A3DModel* pItem)
		{	
			RAYTRACE ray_trace;
			ray_trace.fFraction = 1.0;
			if(pItem && (pItem->GetParentFrame()==0))
			{
				if(pItem->RayTrace(start,delt,&ray_trace,0xffffffff) &&
					ray_trace.fFraction<pRT->fFraction)
				{
					*pRT = ray_trace;
				}
			}                       
		}
	};
};
//构造函数，制定hash表的默认尺寸，没必要很大，否则浪费内存
AModelWraper::AModelWraper():
m_AssistantModelMap(16),
m_FrameMapInt(16),
m_FrameMapStr(16)
{
	m_pSharedInfo = 0;
	m_pListener = 0;
	m_pEngine = 0;
	m_pMainModel = 0;
	m_CurrentStateIndex = 0;
	m_ReadyToPlayAnimation = true;
	m_CurrentAnimationPrivilege = -10;
	m_LoopNextAnimationDelay = 0;
}
AModelWraper::~AModelWraper()
{
	releaseModel();
}
bool AModelWraper::AppendAssistantModel(const char* model_name,const char* hh_name, const char* cc_name)
{
	if(model_name==0||strcmp(model_name,"")==0)
	{
//		ADebug::Msg(0,"null argument.");
//		assert(0);
		return false;
	}
	if(m_AssistantModelMap.get(model_name).second)
	{
		ADebug::Msg(0,"model named < %s > has existed.",model_name);
		assert(0);
		return false;
	}
	A3DModel * pItem = 0;
	if(!m_pEngine->GetA3DModelMan()->LoadModelFile(const_cast<char*>(model_name),&pItem))
	{
		ADebug::Msg(0,"model named < %s > cannot be loaded.",model_name);
		assert(0);
		return false;
	}
	if(strcmp("",hh_name) != 0 && m_pMainModel)
	{
		char * pCCName = strcmp("",cc_name) == 0 ? 0 : const_cast<char*>(cc_name);
		if(!m_pMainModel->AddChildModel(pItem,const_cast<char*>(hh_name),const_cast<char*>(pCCName)))
		{
			ADebug::Msg(0,"model named < %s > cannot be link to father.",model_name);
			assert(0);
			return false;
		}
	}
	else
	{
		pItem->SetPos(GetPos());
	}
	pItem->SetLogicEventCallBack(call_back_logic_event,this);
	m_AssistantModelMap.put(model_name,pItem);
	return true; 		
}
bool AModelWraper::DeleteAssistantModel(const char* model_name)
{
	if(model_name==0||strcmp(model_name,"")==0)
	{
//		ADebug::Msg(0,"null argument.");
//		assert(0);
		return false;
	}
	if(!m_AssistantModelMap.get(model_name).second)
	{
//		ADebug::Msg(0,"model named < %s > not existed.",model_name);
//		assert(0);
		return false;
	}
	m_pEngine->GetA3DModelMan()->ReleaseModel(
		*(m_AssistantModelMap.get(model_name).first)
		);
	m_AssistantModelMap.erase(model_name);
	return true;
}
A3DModel * AModelWraper::GetAssistantModel(const char* model_name)
{
	if(model_name==0||strcmp(model_name,"")==0)
	{
//		ADebug::Msg(0,"null argument.");
//		assert(0);
		return 0;
	}
	if(!m_AssistantModelMap.get(model_name).second)
	{
//		ADebug::Msg(0,"model named < %s > not existed.",model_name);
//		assert(0);
		return 0;
	}
	return *m_AssistantModelMap.get(model_name).first;
}

bool AModelWraper::NoitfyAssistantModelPlayAnim(const char* model_name,const char * anim_name)
{
	if(	anim_name==0||strcmp(anim_name,"")==0)
	{
//		ADebug::Msg(0,"null argument.");
//		assert(0);
		return 0;
	}
	A3DModel * pItem = GetAssistantModel(model_name);
	if(pItem && ! pItem->PlayActionByName(const_cast<char*>(anim_name),true,false))
	{
		ADebug::Msg(0,"anim named < %s > can not be played.",anim_name);
		assert(0);
		return 0;
	}
	return true;
}

void AModelWraper::SetPos(const A3DVECTOR3& p)
{
	m_LogicPos = p;
	if(GetA3DModel())
	{
		GetA3DModel()->SetPos(p);
	}
	m_AssistantModelMap.enum_element(enumfun::enum_fun_pos(p));
}
void AModelWraper::SetDirAndUp(const A3DVECTOR3& d,const A3DVECTOR3& u)
{
	m_LogicDir = d;
	m_LogicUp = u;
	if(GetA3DModel())
	{
		GetA3DModel()->SetDirAndUp(d,u);
	}
	m_AssistantModelMap.enum_element(enumfun::enum_fun_dir_up(d,u));
}
A3DVECTOR3 AModelWraper::GetModelCenterPoint()
{
	if(GetA3DModel())
	{
		GetA3DModel()->UpdateToFrame(-1);
		GetA3DModel()->UpdateModelOBB();
		return GetA3DModel()->GetModelAABB().Center;
	}
	return GetPos();
}
A3DVECTOR3 AModelWraper::GetModelSize()
{
	if(GetA3DModel())
	{
		GetA3DModel()->UpdateToFrame(-1);
		GetA3DModel()->UpdateModelOBB();
		return GetA3DModel()->GetModelAABB().Extents;
	}
	return A3DVECTOR3(0.0f,0.0f,0.0f);
}
bool AModelWraper::Render(A3DViewport * pCurrentViewport)
{
	if(GetA3DModel() && !GetA3DModel()->Render(pCurrentViewport))
	{
		return false;
	}
	try
	{
		m_AssistantModelMap.enum_element(enumfun::enum_fun_render(pCurrentViewport));
	}
	catch(const char* psz)
	{
		ADebug::Msg(0,"anim named < %s > can not be render.",psz);
		assert(0);
		return false;	
	}
	return true;
}
bool AModelWraper::TickAnimation()
{
	if(GetA3DModel() && !GetA3DModel()->TickAnimation())
	{
		return false;
	}
	try
	{
		m_AssistantModelMap.enum_element(enumfun::enum_fun_tickanim());
	}
	catch(const char* psz)
	{
		ADebug::Msg(0,"anim named < %s > can not be tick.",psz);
		assert(0);
		return false;	
	}
	if(! handleActionQueue() )
	{
		return false;
	}
	return true;
}
bool AModelWraper::RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACE * pRayTrace)
{
	RAYTRACE ray_trace;
	ray_trace.fFraction = 1.0;
	pRayTrace->fFraction = 1.0;
	if(GetA3DModel())
	{
		if(GetA3DModel()->RayTrace(
			const_cast<A3DVECTOR3&>(vecStart),
			const_cast<A3DVECTOR3&>(vecDelta),
			&ray_trace,0xffffffff))
		{
			if(ray_trace.fFraction<pRayTrace->fFraction)
			{
				*pRayTrace = ray_trace;	
			}
		}
	}
	m_AssistantModelMap.enum_element(enumfun::enum_fun_raytrace(vecStart,vecDelta,pRayTrace));
	return pRayTrace->fFraction<1.0f;
}

bool AModelWraper::PlayGFX(const char* gfx_name)
{
	if(gfx_name==0||strcmp(gfx_name,"")==0)
	{
		ADebug::Msg(0,"null argument.");
		assert(0);
		return false;
	}
	A3DGraphicsFX*				m_pGFX;
	if(!m_pEngine->GetA3DGFXMan()->LoadGFXFromFile(const_cast<char*>(gfx_name),
		NULL, NULL, false, &m_pGFX,GetModelCenterPoint(),GetDir(),GetUp(),false))
	{	
		ADebug::Msg(0,"can not play GFX named < %s >.",gfx_name);
		assert(0);
		return false;
	}
	m_pGFX->Start(true);
	return true;			
}
bool AModelWraper::PlayGFX(const char* gfx_name,const A3DVECTOR3& pos)
{
	if(gfx_name==0||strcmp(gfx_name,"")==0)
	{
		ADebug::Msg(0,"null argument.");
		assert(0);
		return false;
	}
	A3DGraphicsFX*				m_pGFX;
	if(!m_pEngine->GetA3DGFXMan()->LoadGFXFromFile(const_cast<char*>(gfx_name),
		NULL, NULL, false, &m_pGFX,pos,GetDir(),GetUp(),false))
	{	
		ADebug::Msg(0,"can not play GFX named < %s >.",gfx_name);
		assert(0);
		return false;
	}
	m_pGFX->Start(true);
	return true;		
}
bool AModelWraper::PlayGFX(const char* gfx_name,const char* hh_name)
{
	if(gfx_name==0||strcmp(gfx_name,"")==0)
	{
		ADebug::Msg(0,"null argument.");
		assert(0);
		return false;
	}
	A3DGraphicsFX*				m_pGFX;
	if(!m_pEngine->GetA3DGFXMan()->LoadGFXFromFile(const_cast<char*>(gfx_name),
		GetA3DModel(),const_cast<char*>(hh_name), false, &m_pGFX,GetPos(),GetDir(),GetUp(),false))
	{	
		ADebug::Msg(0,"can not play GFX named < %s >.",hh_name);
		assert(0);
		return false;
	}
	m_pGFX->Start(true);
	return true;			
}
bool AModelWraper::Init(const char* pseudo_file_name)
{	
	pair<shared_role_info**, bool> res = 
		g_ConfigFileCImp.all_config.get(pseudo_file_name);
	if(!res.second)
	{
		ADebug::Msg(0,"can not open pseudo file named < %s >.",pseudo_file_name);
		assert(0);
		return false;
	}

	m_pSharedInfo = *res.first;
	AUString mn = m_pSharedInfo->SRI_list_MOD.GetHead().model_name;
	AUString an = m_pSharedInfo->SRI_list_MOD.GetHead().init_anim;
	if(!mn.IsEmpty() && !loadModel(mn,an))
	{
		return false;
	}

	AUString gfx_name;
	AUString gfx_hh;

	if(m_pSharedInfo->SRI_list_GFX.GetCount() != 0)
	{
		AListPosition pos = m_pSharedInfo->SRI_list_GFX.FindByIndex(m_CurrentStateIndex);
		if(pos)
		{
			gfx_name = m_pSharedInfo->SRI_list_GFX.GetAt(pos).gfx_name;
			gfx_hh = m_pSharedInfo->SRI_list_GFX.GetAt(pos).hh_name;
		}
		if(!gfx_name.IsEmpty())
		{
			if(!gfx_hh.IsEmpty())
			{
				PlayGFX(gfx_name,gfx_hh);
			}
			else
			{
				PlayGFX(gfx_name);
			}
		}
	}

	return true;
}
bool AModelWraper::NextState()
{
	AUString gfx_name;
	AUString gfx_hh;
	AUString mod_name;
	AUString init_anim;

	if(m_pSharedInfo->SRI_list_CNV_GFX.GetCount() != 0)
	{
		AListPosition pos = m_pSharedInfo->SRI_list_CNV_GFX.FindByIndex(m_CurrentStateIndex);
		if(pos)
		{
			gfx_name = m_pSharedInfo->SRI_list_CNV_GFX.GetAt(pos).gfx_name;
			gfx_hh = m_pSharedInfo->SRI_list_CNV_GFX.GetAt(pos).hh_name;
		}
		if(!gfx_name.IsEmpty())
		{
			if(!gfx_hh.IsEmpty())
			{
				PlayGFX(gfx_name,gfx_hh);
			}
			else
			{
				PlayGFX(gfx_name);
			}
		}
	}

	++m_CurrentStateIndex;
	releaseModel();

	if(m_pSharedInfo->SRI_list_MOD.GetCount() != 0)
	{
		AListPosition pos = m_pSharedInfo->SRI_list_MOD.FindByIndex(m_CurrentStateIndex);
		if(pos)
		{
			mod_name = m_pSharedInfo->SRI_list_MOD.GetAt(pos).model_name;
			init_anim = m_pSharedInfo->SRI_list_MOD.GetAt(pos).init_anim;
			loadModel(mod_name,init_anim);
		}
	}

	if(m_pSharedInfo->SRI_list_GFX.GetCount() != 0)
	{
		AListPosition pos = m_pSharedInfo->SRI_list_GFX.FindByIndex(m_CurrentStateIndex);
		if(pos)
		{
			gfx_name = m_pSharedInfo->SRI_list_GFX.GetAt(pos).gfx_name;
			gfx_hh = m_pSharedInfo->SRI_list_GFX.GetAt(pos).hh_name;
		}
		if(!gfx_name.IsEmpty())
		{
			if(!gfx_hh.IsEmpty())
			{
				PlayGFX(gfx_name,gfx_hh);
			}
			else
			{
				PlayGFX(gfx_name);
			}
		}
	}

	return true;
}
void AModelWraper::Release()
{
	releaseModel();
}
void AModelWraper::releaseModel()
{
	if(m_pMainModel)
	{
		m_pEngine->GetA3DModelMan()->ReleaseModel(m_pMainModel);
		m_pMainModel = 0;
	}
	m_AssistantModelMap.enum_element(enumfun::enum_fun_release(m_pEngine));
	m_AssistantModelMap.clear();
	m_FrameMapInt.clear();
	m_FrameMapStr.clear();
}
bool AModelWraper::loadModel(const char* model_name,const char* init_anim)
{
	releaseModel();
	ClearActionQueue();
	if(model_name==0||strcmp(model_name,"")==0)
	{
		return true;
	}
	if(!m_pEngine->GetA3DModelMan()->LoadModelFile(
		const_cast<char*>(model_name),&m_pMainModel))
	{
		ADebug::Msg(0,"load model named < %s > failure.",model_name);
		assert(0);
		return false;
	}
	m_pMainModel->SetActionLoopEndCallBack(call_back_action_end,this);
	m_pMainModel->SetLogicEventCallBack(call_back_logic_event,this);
	fillConnectPointSet();

	if(model_name!=0&&strcmp(init_anim,"")!=0)
	{
		if(strstr(init_anim,"["))
		{
			AppendLowLevelActionToQueueTail(init_anim);
		}
		else
		{
			AppendHighLevelActionToQueueTail(init_anim);
		}
	}
	return true; 
}
void AModelWraper::fillConnectPointSet()
{
	m_FrameMapStr.clear();
	AList * pChildFrameList = m_pMainModel->GetChildFrameList();
	ALISTELEMENT * pThisFrameElement = pChildFrameList->GetFirst();
	while( pThisFrameElement != pChildFrameList->GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pThisFrameElement->pData;
		recursionFindConnectPoint(pFrame);
		pThisFrameElement = pThisFrameElement->pNext;
	}
	m_FrameMapInt.clear();
	const AListT<hh_item>& hh_list = m_pSharedInfo->SRI_list_ST_HH;
	AListPosition pos = hh_list.GetHeadPosition();
	while(pos)
	{
		hh_item item = hh_list.GetNext(pos);
		bool bfound = m_FrameMapStr.get(item.hh_name).second;
		if(bfound)
		{
			A3DFrame * pFrame = *(m_FrameMapStr.get(item.hh_name).first);
			m_FrameMapInt.put(item.hh_index,pFrame);
		}
		else
		{
		}
	}
}
void AModelWraper::recursionFindConnectPoint(A3DFrame * pFrame)
{
	char szFrameUpper[256];
	strncpy(szFrameUpper, pFrame->GetName(), 256);
	_strupr(szFrameUpper);
	if( strstr(szFrameUpper, "HH_") == szFrameUpper )
	{
		m_FrameMapStr.put(szFrameUpper,pFrame);
	}
	A3DFrame * pChildFrame = pFrame->GetFirstChildFrame();
	while( pChildFrame )
	{
		recursionFindConnectPoint(pChildFrame);
		pChildFrame = pFrame->GetNextChildFrame();
	}
}

void AModelWraper::ClearActionQueue()
{
	m_CurrentActionQueue.RemoveAll();
	m_ReadyToPlayAnimation = true;
	m_LoopNextAnimationName.Empty();
	m_CurrentAnimationPrivilege = -10;
	m_LoopNextAnimationDelay = 0;
	m_AnimationInEngine.Empty();
	if(m_pMainModel)
	{
		m_pMainModel->PauseAnimation(true);
	}
}
bool AModelWraper::handleActionQueue()
{
	//首先检查控制开关是否是开放的
	if(m_ReadyToPlayAnimation)
	{	
		//当前执行队列非空时
		if(m_CurrentActionQueue.GetCount() != 0)
		{

			ll_action item = m_CurrentActionQueue.GetHead();
			//弹出并播放队列中的元素
			AUString anim_name = item.anim_name;
			//记录下优先级
			m_CurrentAnimationPrivilege = item.privilege;
			//如果这个元素有循环标记，就记录下来
			if(!item.loop_name.IsEmpty())
			{
				m_LoopNextAnimationDelay = item.loop_delay;
				m_LoopNextAnimationName = item.loop_name;
			}
			//播放动画了
			if(m_pMainModel && !m_pMainModel->PlayActionByName(anim_name,true,false))
			{
				ADebug::Msg(0,"play LL animation named < %s > failure.",anim_name.GetBuffer());
				assert(0);
				return false;
			}
			//把控制开关关上,等着回调函数将其打开
			m_ReadyToPlayAnimation = false;
			//通知宿主
			m_pListener->anim_change(anim_name,item.p_outer_data);
			m_AnimationInEngine = anim_name;
			m_CurrentActionQueue.RemoveHead();
		}
		else//当前队列为空时，检查是否有循环信息，有的话就递减延迟计数器减到零时进行循环
		{
			if(!m_LoopNextAnimationName.IsEmpty() && -- m_LoopNextAnimationDelay <= 0)
			{
				m_LoopNextAnimationDelay = 0;
				AppendHighLevelActionToQueueTail(m_LoopNextAnimationName);
			}
		}
	}
	return true;	
}

bool AModelWraper::AddHighLevelActionToQueueHead(const char * action_name,void * p_buffer)
{
	//not implement
	assert(0);
	return false;		
}
bool AModelWraper::AppendHighLevelActionToQueueTail(const char * action_name,void* p_buffer)
{
	if(action_name==0||strcmp(action_name,"")==0)
	{
		ADebug::Msg(0,"null argument.",action_name);
		assert(0);
		return false;
	}
	//高级动画名不存在就忽略
	pair<hl_action**,bool> find_res = m_pSharedInfo->SRI_map_HL_LL.get(action_name);
	if(!find_res.second)
	{
		ADebug::Msg(0,"play HL animation named < %s > failure.",action_name);
		assert(0);
		return false;
	}
	//得到高级动画名对应的低级动画序列
	hl_action* pHLAction = *find_res.first;
	//千万不要改动这里，它是共享的数据
	const AListT<ll_action>* act_list = &(pHLAction->act_queue);
	//将共享数据区的低级动作序列数据附加到当前执行队列的尾部
	bool bfirst = true;
	AListPosition pos = act_list->GetHeadPosition();
	while(pos)
	{
		m_CurrentActionQueue.AddTail(
			act_list->GetNext(pos)
			);
		//将用户数据加到第一个低级动作中去
		if(bfirst)
		{
			bfirst = false;
			m_CurrentActionQueue.GetTail().p_outer_data = p_buffer; 
		}
	}
	return true;
}

bool AModelWraper::AddLowLevelActionToQueueHead(const char * action_name,void * p_buffer)
{
	if(action_name==0||strcmp(action_name,"")==0)
	{
		ADebug::Msg(0,"null argument.",action_name);
		assert(0);
		return false;
	}
	//构造一个低级动画对象
	ll_action item;
	item.anim_name = action_name;	
	item.loop_name.Empty();
	item.loop_delay = 0;
	item.privilege = 3;
	item.p_outer_data = p_buffer;

	if(m_CurrentAnimationPrivilege <= item.privilege)
	{
		m_CurrentActionQueue.AddHead(item);
		//强行播放下一个动画
		m_ReadyToPlayAnimation = true;
		return true;
	}
	return true;		
}

bool AModelWraper::AppendLowLevelActionToQueueTail(const char * action_name,void* p_buffer)
{
	if(action_name==0||strcmp(action_name,"")==0)
	{
		ADebug::Msg(0,"null argument.",action_name);
		assert(0);
		return false;
	}

	//构造一个低级动画对象
	ll_action item;
	item.anim_name = action_name;	
	item.loop_name.Empty();
	item.loop_delay = 0;
	item.privilege = 3;
	item.p_outer_data = p_buffer;

	if(m_CurrentAnimationPrivilege <= item.privilege)
	{
		m_CurrentActionQueue.AddTail(item);
		//强行播放下一个动画
		m_ReadyToPlayAnimation = true;
		return true;
	}
	return true;		
}
bool AModelWraper::IsInLoopAction()
{
	return !m_LoopNextAnimationName.IsEmpty();
}
bool AModelWraper::HasLoopInAction()
{
	if(IsInLoopAction())
		return true;

	AListPosition pos = m_CurrentActionQueue.GetHeadPosition();
	while(pos)
	{
		if(!m_CurrentActionQueue.GetNext(pos).loop_name.IsEmpty())
		{
			return true;
		}
	}
	return false;	
}
bool AModelWraper::IsIdle()
{
	return IsEmptyActionQueue() && m_AnimationInEngine.IsEmpty();
}
bool AModelWraper::IsEmptyActionQueue()
{
	return !IsInLoopAction()&&(m_CurrentActionQueue.GetCount()==0);
}
bool AModelWraper::ForcePlayAnim(const char * anim_name)
{
	ClearActionQueue();
	if(m_pMainModel && !m_pMainModel->
		PlayActionByName(const_cast<char*>(anim_name),true,false))
	{
		ADebug::Msg(0,"can not play animation named < %s >.",anim_name);
		assert(0);
		return false;
	}
	m_AnimationInEngine = anim_name;
	return true;
}
void AModelWraper::call_back_logic_event(A3DMODEL_LOGIC_EVENT * pLogicEvent, LPVOID pArg)
{
	//调用反馈接口
	AModelWraper * pThis = static_cast<AModelWraper * >(pArg);
	if(pThis->m_pListener)
	{
		pThis->m_pListener->receive_feedback(pLogicEvent->szNotifyString);
	}
}
void AModelWraper::call_back_action_end(A3DModel * pNoUsing, LPVOID pArg)
{
	//动画项执行结束时打开开关
	AModelWraper * pThis = static_cast<AModelWraper * >(pArg);
	pThis->m_ReadyToPlayAnimation = true;
	pThis->m_CurrentAnimationPrivilege = -10;
	pThis->m_pListener->anim_end(pThis->m_AnimationInEngine);
	pThis->m_AnimationInEngine.Empty();
}

#endif