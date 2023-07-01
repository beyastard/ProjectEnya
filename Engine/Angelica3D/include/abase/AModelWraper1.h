#ifndef __AMODEL_WRAPER1_H__
#define __AMODEL_WRAPER1_H__


/***********************************************************************
 * FILE: AModelWraper1.H
 *
 * DESCRIPTION : wraper classes to introduce Engine Model class to 
 *				 application for easy using	
 *
 * CREATED BY: MengXin
 *
 * HISTORY:	2002 - 10 - 8
 *
 * Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
 ************************************************************************/

#include "ADebug.h"
#include "ABaseA3D.h"
#include "AUString.h"
#include "AUList.h"
#include "AUnitInWorldInterface.h"
#include "StlHeader.h"

struct i_host_listener;

class  AModelWraper
{
public:
	
	//得到支持的动画名
	static bool GetAnimationNameSet(const char * file_name,AListT<AUString>&res);
	//得到支持的挂点名
	static bool GetHHNameSet(const char * file_name,AListT<HH_infor>& res);
	//涉及的模型名
	static bool GetInvolvedModel(const char * file_name,AListT<AUString>& res);
	//涉及的效果名
	static bool GetInvolvedGFX(const char * file_name,AListT<AUString>& res);
	//逻辑事件回调
	static void call_back_logic_event(A3DMODEL_LOGIC_EVENT * pLogicEvent, LPVOID pArg);
	//低级动画结束回调
	static void call_back_action_end(A3DModel * pNoUsing, LPVOID pArg);
	//设置模型级别
	static void SetConfigFileModelLevel(MODEL_LEVEL level);
	//模型级别
	static MODEL_LEVEL model_level;

private:

	//内部使用的包含优先级的动画项目
	struct action_item
	{	
		string name;
		string loop_name;
		int	   loop_delay;
		int	   privilege;
		void   * p_outer_data;
		bool operator == (const action_item& v)
		{
			return v.name == name;
		}
		action_item():loop_delay(0),privilege(2),p_outer_data(0){}
	};

	int current_anim_privilege;
	//循环名
	string next_call_anim;
	//循环延迟
	int    next_call_delay;
	//放入引擎中的动画名
	string	anim_in_engine;
		
private:

	//反馈接口,可能为空
	i_host_listener * m_pHostListener;

	//引擎接口,必须可用
	A3DEngine* m_pEngine;

	//配置文件名
	string	config_file_name;
	
	//前一个状态名
	string	last_state;

	//当前经过分析后的要执行的低级动画序列
	list<action_item>	current_action_queue;

	//是否可以播放下一个动画的开关，由动画播放函数关闭
	//动画回调函数开放
	bool	play_anim_ready;
	//当前状态的索引
	int		current_state_index;
	
	//主模型
	A3DModel * pCurrentModel;

	//辅助模型
	map<string,A3DModel *>	assistant_model_map; 

	//辅助模型挂载信息
	map<string,pair<string,string> >	assistant_model_link_infor; 
	
	//逻辑姿态信息
	A3DVECTOR3 last_pos;
	A3DVECTOR3 last_dir;
	A3DVECTOR3 last_up;
	//挂点的的集合
	vector<string >		connect_name_vector;	
	vector<A3DFrame * >	connect_ptr_vector;	

public:
	//----------------------------------------------------------------------
	//状态名到该状态对应的模型名的映射
	map<string,string>									state_model_map;

	//状态名到该状态对应效果，及其挂点的映射
	map<string,vector<pair<string,string> > >			state_gfx_map;

	//前一个状态到后一个状态的变化 该变化对应的效果及其挂点
	map<pair<string,string> , vector<pair<string,string> > >	convert_gfx_map;

	//高级动画名到低级动画序列的映射
	map<string,list<action_item> >						action_serial_map;

	//根据配置文件读取的可选的状态序列
	vector<string>										state_queue;

	//根据配置文件读取的可选的动画序列
	vector<string>										anim_queue;

	//初始化时的状态名
	string												init_state;

	//状态的出场动画
	map<string,string>									state_init_anim;	
		
	//------------------------------------------------------------------------------
	
		
private:

	//分析动作队列做出相应的判断,这是该类唯一需要每个 TICK 都被调用的函数
	//（被 TickAnimation 调用）
	bool handleActionQueue();
	//装载模型，该函数在没有进行预装载的情况下会有可观的时间开销
	bool loadModel(const char* model_name);
	//卸载模型
	void releaseModel();
	//播放效果，该函数在没有进行预装载的情况下会有可观的时间开销
	bool playGFX(const char* gfx_name);
	//播放效果，该函数在没有进行预装载的情况下会有可观的时间开销
	bool playGFX(const char* gfx_name,const A3DVECTOR3& pos);
	//播放效果（带挂点），该函数在没有进行预装载的情况下会有可观的时间开销
	bool playGFXonModel(const char* gfx_name,const char* link_point_name);
	//递归寻找挂点
	void recursionFindConnectPoint(A3DFrame * pFrame);
	//填写挂点集合
	void fillConnectPointSet();

public:
	//安装辅助模型
	bool AppendAssistantModel(const char* model_name,const char* hh_name = "" , const char* cc_name = "" );
	//删除辅助模型
	bool DeleteAssistantModel(const char* model_name);
	//通知辅助模型播放动画
	bool NoitfyAssistantModelPlayAnim(const char* model_name,const char * anim_name);
	//通知辅助模型播放效果
	bool NoitfyAssistantModelPlayGFX(const char* model_name,const char * gfx_name);
	//通知辅助模型播放效果（带挂点）
	bool NoitfyAssistantModelPlayGFXOnModel(const char* model_name,const char * gfx_name,const char * hh_name);
	//得到辅助模型
	A3DModel * GetAssistantModel(const char* model_name);
	//播放动画
	bool PlayGFX(const char* gfx_name);
	bool PlayGFX(const char* gfx_name,const A3DVECTOR3& pos);
	bool PlayGFX(const char* gfx_name,const char* hh_name);

public:

	//常规控制
	A3DModel * GetA3DModel();
	A3DVECTOR3 GetPos();
	A3DVECTOR3 GetDir();
	A3DVECTOR3 GetUp();

	A3DVECTOR3 GetRootFramePos();
	A3DVECTOR3 GetRootFrameDir();
	A3DVECTOR3 GetRootFrameUp();

	void SetPos(const A3DVECTOR3& p);
	void SetDirAndUp(const A3DVECTOR3& d,const A3DVECTOR3& u);
	A3DVECTOR3 GetModelCenterPoint();
	A3DVECTOR3 GetModelSize();
	bool Render(A3DViewport * pCurrentViewport);
	bool TickAnimation();
//	bool AABBTrace(AABBTRACEINFO* pInfo, AABBTRACE* pTrace);
	bool RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACE * pRayTrace);

public:

	AModelWraper();
	~AModelWraper();
	//避免被当作值类型使用，只定义而不去实现
	AModelWraper operator = (const AModelWraper&);
	AModelWraper (const AModelWraper&);

public:

	//设置引擎接口和反馈接口
	void BindEngineAndListener(A3DEngine* pe,i_host_listener * pl = 0);

	//加载配置文件，该函数时间开销较大
	bool LoadConfigFile(const char * file_name);

	//加载配置字符串，该函数时间开销较大
	bool LoadConfigString(const char * config_string);


	vector<string >&		EnumHHName();
	vector<A3DFrame * >&	EnumHHPtr();
	vector<string>&		EnumSupportAnimName();
	vector<string>&		EnumSupportStateName();

public:

	//前向状态序列控制

	//进入状态序列，并将状态制于第一个状态，加载资源
	bool Begin();

	//跃迁致下一个状态，更换资源，清空当前执行队列，播放效果
	bool NextState();

	//离开状态序列，将状态制为空，释放资源
	void End();

	//得到状态序列元素的个数（类似于数组元素个数）
	int  GetStateCount();

	//得到当前状态是第几个（类似于数组元素索引）//index based 0
	int	 GetState();

	//随机状态控制，建议避免使用因为对应的变换可能因没有定义而被忽略

	//设置到指定的状态
	bool SetStateByName(const char * state_name);

	//得到当前状态名称
	string GetCurrentStateName();

public:
	
	//动画控制

	//向执行队列中压如高级动画名
	//调用方不要在每个TICK都推入相同的高级动画，
	//由于模型只可能依次消化掉他们，所以这样做只会使执行队列
	//无休止的膨胀，对于循环性动画推入一个即可，并且不会被清除
	//非循环的动画每当执行完毕都会被从队列中清掉

	bool AddActionToHead(const char * state_name,void * p_buffer = 0);
	//一般的情况下与前一个函数组合强行播放某动画
	bool AppendActionToTail(const char * state_name,void * p_buffer = 0);
	//清空执行队列
	void ClearActionQueue();
	//队列是否为空
	bool IsEmptyActionQueue();
	//强行播放某动画
	bool ForcePlayAnim(const char * anim_name);
	//将低级动画放入播放队列
	bool AppendLowLevelActionToTail(const char * state_name);
	//将低级动画放入队列头部
	bool AddLowLevelActionToHead(const char * state_name);
	//当前动作是否处在循环中
	bool IsInLoopAction();
	//当前队列中是否包含循环动画
	bool HasLoopInAction();
	//是否无事可做
	bool IsIdle();
};

#endif//__AMODEL_WRAPER1_H__