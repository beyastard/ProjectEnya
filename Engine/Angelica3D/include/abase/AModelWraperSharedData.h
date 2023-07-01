#ifndef _AMODELWRAPERSHAREDDATA_H_
#define _AMODELWRAPERSHAREDDATA_H_

#include "AUString.H"
#include "AUList.H"
#include "hash_table_wrapper.h"
using namespace abase;

struct _hash_function_austring
{
	_hash_function_austring() {}
	_hash_function_austring(const _hash_function_austring & hf) {}
	inline unsigned long operator()(const AUString& str) const
	{
		char* s = str.GetBuffer();
		unsigned long h = 0;
		for(;*s; s++)
		{
			h = h * 31 + tolower(*(unsigned char *)s);
		}
		return h;
	}
};

struct ll_action
{
	AUString	anim_name;
	AUString	loop_name;
	int			loop_delay;
	int			privilege;
	void*		p_outer_data;
	ll_action():
	loop_delay(0),
	privilege(2),
	p_outer_data(0){}
};

struct hl_action
{
	AListT<ll_action> act_queue;
	hl_action(){}
	hl_action(const hl_action&);
	hl_action operator = (const hl_action&);
};

struct gfx_item	
{
	AUString	gfx_name;
	AUString	hh_name;
};

struct hh_item	
{
	int			hh_index;
	AUString	hh_name;
};

struct model_item	
{
	AUString	init_anim;				
	AUString	model_name;
};

struct	shared_role_info
{
	AListT<hh_item>											SRI_list_ST_HH;//支持的挂载点
	AListT<model_item>										SRI_list_MOD;//依次出现的模型
	AListT<gfx_item>										SRI_list_GFX;//状态出现的效果
	AListT<gfx_item>										SRI_list_CNV_GFX;//状态出现的过渡效果
	hashtab<hl_action*,AUString,_hash_function_austring>	SRI_map_HL_LL;//高级动画名到低级动画序列的映射
	shared_role_info():SRI_map_HL_LL(128){}
};

struct	config_file_c_imp
{
	AUString  pseudo_file_name;
	enum DEFINE_FLAG
	{
		DEFINE_UNKNOW,
		DEFINE_STATE,
		DEFINE_CONVERT,
		DEFINE_ACTION
	} define_flag;

	shared_role_info * pcurrent_item;
	AUString	current_action;
	bool		current_loop_flag;
	float		current_loop_delay;
	gfx_item	cuttent_gfx;
	int			gfx_index;
	hl_action * pnew_hl_action;
	hashtab<shared_role_info*,AUString,_hash_function_austring>	all_config;
	AListT<shared_role_info*> dump1;
	AListT<hl_action*> dump2;

public:
	config_file_c_imp():all_config(128)
	{
		define_flag = DEFINE_UNKNOW;
		pcurrent_item = 0;
		pseudo_open();
	}

	~config_file_c_imp()
	{
		pseudo_close();
	}

private:
	bool file_begin(const char* file_name)
	{
		if(!pseudo_file_name.IsEmpty())
		{
			throw("miss < file_end >.");
			return false;
		}
		pseudo_file_name = file_name;
		pcurrent_item = new shared_role_info;
		dump1.AddTail(pcurrent_item);
		return true;
	}

	bool state_begin(const char* st_name)
	{
		(void)st_name;
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag != DEFINE_UNKNOW)
		{
			throw("miss < end_XXX >.");
			return false;
		}
		define_flag = DEFINE_STATE;
		return true;
	}

	bool convert_begin(const char* name1,const char* name2)
	{
		(void)name1;
		(void)name2;
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag != DEFINE_UNKNOW)
		{
			throw("miss < end_XXX >.");
			return false;
		}
		define_flag = DEFINE_CONVERT;
		return true;
	}

	bool action_begin(const char* act_name,bool is_loop,float loop_delay = 0.0)
	{	
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag != DEFINE_UNKNOW)
		{
			throw("miss < end_XXX >.");
			return false;
		}
		define_flag = DEFINE_ACTION;
		current_action = act_name;
		current_loop_flag = is_loop;
		current_loop_delay = loop_delay * 30;
		pnew_hl_action = new hl_action;
		dump2.AddTail(pnew_hl_action);
		return true;
	}

	bool state_end()
	{
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag != DEFINE_STATE)
		{
			throw("miss < state_begin >.");
			return false;
		}
		define_flag = DEFINE_UNKNOW;
		return true; 
	}

	bool convert_end()
	{
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag != DEFINE_CONVERT)
		{
			throw("miss < convert_begin >.");
			return false;
		}
		define_flag = DEFINE_UNKNOW;
		return true;
	}

	bool action_end()
	{
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag != DEFINE_ACTION)
		{
			throw("miss < action_begin >.");
			return false;
		}
		if(current_loop_flag)
		{
			if(pnew_hl_action->act_queue.GetCount() != 0)
			{
				pnew_hl_action->act_queue.GetTail().loop_delay = (int)current_loop_delay;
				pnew_hl_action->act_queue.GetTail().loop_name = current_action;
			}
		}
		pcurrent_item->SRI_map_HL_LL.put(current_action,pnew_hl_action);
		define_flag = DEFINE_UNKNOW;	
		current_action = "";
		current_loop_flag = false;
		current_loop_delay = 0;
		return true;
	}

	bool gfx(const char* gn,const char* hn)
	{
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag == DEFINE_STATE)
		{
			gfx_item item;
			item.gfx_name = gn;
			item.hh_name = hn;
			pcurrent_item->SRI_list_GFX.AddTail(item);		
		}
		else if(define_flag == DEFINE_CONVERT)
		{
			gfx_item item;
			item.gfx_name = gn;
			item.hh_name = hn;
			pcurrent_item->SRI_list_CNV_GFX.AddTail(item);
		}
		else
		{
			throw("miss < state_begin > or < convert_begin >.");
			return false;
		}
		return true;
	}

	bool anim(const char* an,int pr = 2)
	{
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag != DEFINE_ACTION || !pnew_hl_action)
		{
			throw("miss < action_begin >.");
			return false;
		}
		ll_action item;
		item.anim_name = an;
		item.privilege = pr;
		pnew_hl_action->act_queue.AddTail(item);
		return true;
	}

	bool define_HH(const char* hn,int id)
	{
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag != DEFINE_UNKNOW)
		{
			throw("miss < end_XXX >.");
			return false;
		}
		hh_item item;
		item.hh_name = hn;
		item.hh_index = id;
		pcurrent_item->SRI_list_ST_HH.AddTail(item);
		return true;
	}

	bool model(const char* mn,const char* ia = "")
	{
		if(!pcurrent_item)
		{
			throw("miss < file_begin >.");
			return false;
		}
		if(define_flag != DEFINE_STATE)
		{
			throw("miss < state_begin >.");
			return false;
		}		
		model_item item;
		item.init_anim = ia;
		item.model_name = mn;
		pcurrent_item->SRI_list_MOD.AddTail(item);	
		return true;
	}

	bool file_end()
	{
		if(pseudo_file_name.IsEmpty())
		{
			throw("miss < file_begin >.");
			return false;
		}
		all_config.put(pseudo_file_name,pcurrent_item);
		pcurrent_item = 0;
		pseudo_file_name = "";
		return true;
	}

public:
	void pseudo_close()
	{
		{
			AListPosition pos = dump1.GetHeadPosition();
			while(pos)
			{
				delete dump1.GetNext(pos);
			}
		}
		{
			AListPosition pos = dump2.GetHeadPosition();
			while(pos)
			{
				delete dump2.GetNext(pos);
			}
		}
	}

	bool pseudo_open();

	bool getdata(const char* pseudo_file_nmae,shared_role_info** ppData)
	{
		abase::pair<shared_role_info**, bool> res = all_config.get(pseudo_file_nmae);
		if(res.second)
		{
			*ppData = *res.first;
			return true;
		}
		return false;
	}

private:
	void _matters_files();
};

extern config_file_c_imp g_ConfigFileCImp;

#endif
