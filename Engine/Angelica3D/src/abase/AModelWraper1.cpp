#include "AModelWraper.h"
#ifndef USING_MODEL_UNIT2
#include "AModelUnit.h"
#include <assert.h>
#include "AConfig.h"
#include <mtime.h>
#include <A3DFileImage.h>
#include "AUnitInWorldInterface.h"
#include "ALexicalParse.h"
#include <string.h>
#include <stdio.h>
#include "jexception.h"

#pragma message( "old version with stl" )

struct _config_infor_cache
{
	map<string,string>									state_model_map;
	map<string,vector< pair<string,string> > >			state_gfx_map;
	map<pair<string,string> , vector < pair<string,string> > >		convert_gfx_map;
	map<string,list<AModelWraper::action_item> >		action_serial_map;
	vector<string>										state_queue;
	vector<string>										anim_queue;
	string												init_state;
	map<string,string>									state_init_anim;	
};

static map<string , _config_infor_cache> config_infor_cache;
static void get_cache(const AModelWraper& mw, _config_infor_cache& cnf)
{
	cnf.action_serial_map = mw.action_serial_map;
	cnf.anim_queue = mw.anim_queue;
	cnf.convert_gfx_map = mw.convert_gfx_map;
	cnf.init_state = mw.init_state;
	cnf.state_gfx_map = mw.state_gfx_map;
	cnf.state_model_map = mw.state_model_map;
	cnf.state_queue = mw.state_queue;
	cnf.state_init_anim = mw.state_init_anim;
}	
static void fill_cache(AModelWraper& mw, const _config_infor_cache& cnf)
{
	mw.action_serial_map = cnf.action_serial_map;
	mw.anim_queue = cnf.anim_queue;
	mw.convert_gfx_map = cnf.convert_gfx_map;
	mw.init_state = cnf.init_state;
	mw.state_gfx_map = cnf.state_gfx_map;
	mw.state_model_map = cnf.state_model_map;
	mw.state_queue = cnf.state_queue;
	mw.state_init_anim = cnf.state_init_anim;
}

MODEL_LEVEL AModelWraper::model_level = LEVEL_NORMAL;

void writeErrorInfor(const string& str)
{
	ADebug::Msg(0,str.c_str());
	assert(0);
}

AModelWraper::AModelWraper()
{
	play_anim_ready = true;
	pCurrentModel = 0;
	m_pHostListener = 0;
	m_pEngine = 0;
	next_call_delay = 0;
	current_state_index = 0;
	current_anim_privilege = -10;
	last_pos = A3DVECTOR3(0,0,0);
	last_dir = A3DVECTOR3(1,0,0);
	last_up	 = A3DVECTOR3(0,1,0);
}
AModelWraper::~AModelWraper()
{
	releaseModel();
}
void AModelWraper::End()
{

	J_EXCEPTION_RECORD(0)

	releaseModel();
	config_file_name = "";
	last_state = "";
	init_state = "";
	play_anim_ready = true;
	state_model_map.clear();
	state_gfx_map.clear();
	convert_gfx_map.clear();
	action_serial_map.clear();	
	current_action_queue.clear();
	assistant_model_map.clear();
	state_init_anim.clear();
	last_pos = A3DVECTOR3(0,0,0);
	last_dir = A3DVECTOR3(1,0,0);
	last_up	 = A3DVECTOR3(0,1,0);
	current_anim_privilege = -10;
}

bool AModelWraper::Begin()
{
	J_EXCEPTION_RECORD(0)
	if(init_state.empty())
	{
		writeErrorInfor("default model name is NULL! error occur on unit configed by file <" + config_file_name + ">.");
		return false;
	}
	return SetStateByName(init_state.c_str());
}

void AModelWraper::BindEngineAndListener(A3DEngine* pe,i_host_listener * pl)
{
	m_pHostListener = pl;
	m_pEngine = pe;
	if(!m_pEngine)
	{
		writeErrorInfor("Engine pointer is NULL!");
	}
}

A3DModel * AModelWraper::GetA3DModel()
{
	return pCurrentModel;
}
A3DVECTOR3 AModelWraper::GetPos()
{
	return last_pos;
}
A3DVECTOR3 AModelWraper::GetDir()
{
	return last_dir;
}
A3DVECTOR3 AModelWraper::GetUp()
{
	return last_up;
}
A3DVECTOR3 AModelWraper::GetRootFramePos()
{
	if(pCurrentModel&&pCurrentModel->FindChildFrameByName("HH_跨部"))
	{
		return pCurrentModel->FindChildFrameByName("HH_跨部")->GetAbsoluteTM().GetRow(0);
	}
	return A3DVECTOR3(0.0f,0.0f,0.0f);
}
A3DVECTOR3 AModelWraper::GetRootFrameDir()
{
	if(pCurrentModel&&pCurrentModel->FindChildFrameByName("HH_跨部"))
	{
		return pCurrentModel->FindChildFrameByName("HH_跨部")->GetAbsoluteTM().GetRow(2);
	}
	return A3DVECTOR3(1.0f,0.0f,0.0f);
}
A3DVECTOR3 AModelWraper::GetRootFrameUp()
{
	if(pCurrentModel&&pCurrentModel->FindChildFrameByName("HH_跨部"))
	{
		return pCurrentModel->FindChildFrameByName("HH_跨部")->GetAbsoluteTM().GetRow(1);
	}
	return A3DVECTOR3(0.0f,1.0f,0.0f);
}

void AModelWraper::SetPos(const A3DVECTOR3& p)
{

	J_EXCEPTION_RECORD(0)


	last_pos = p;
	if(pCurrentModel)
	{
		pCurrentModel->SetPos(last_pos);
	}

	map<string,A3DModel *>::iterator itor = assistant_model_map.begin(); 
	for(;itor != assistant_model_map.end();++itor)
	{
		A3DModel * pItem = (*itor).second;
		if(pItem && (pItem->GetParentFrame()==0))
		{
			pItem->SetPos(last_pos);
		}
	}
}
void AModelWraper::SetDirAndUp(const A3DVECTOR3& d,const A3DVECTOR3& u)
{

	J_EXCEPTION_RECORD(0)

	last_dir = d;
	last_up = u;
	if(pCurrentModel)
	{
		pCurrentModel->SetDirAndUp(last_dir,last_up);
	}
	map<string,A3DModel *>::iterator itor = assistant_model_map.begin(); 
	for(;itor != assistant_model_map.end();++itor)
	{	
		A3DModel * pItem = (*itor).second;
		if(pItem && (pItem->GetParentFrame()==0))
		{
			pItem->SetDirAndUp(last_dir,last_up);
		}
	}
}
A3DVECTOR3 AModelWraper::GetModelCenterPoint()
{	

	J_EXCEPTION_RECORD(0)


	if(pCurrentModel)
	{
		pCurrentModel->UpdateToFrame(-1);
		pCurrentModel->UpdateModelOBB();
		return pCurrentModel->GetModelAABB().Center;
	}
	return GetPos();
}
A3DVECTOR3 AModelWraper::GetModelSize()
{	

	J_EXCEPTION_RECORD(0)


	if(pCurrentModel)
	{
		pCurrentModel->UpdateToFrame(-1);
		pCurrentModel->UpdateModelOBB();
		return pCurrentModel->GetModelAABB().Extents;
	}
	return A3DVECTOR3(0.0f);
}

bool AModelWraper::LoadConfigFile(const char * file_name)
{

	J_EXCEPTION_RECORD(0)


	config_file_name = file_name;
	if(config_infor_cache.find(file_name) != config_infor_cache.end())
	{
		fill_cache(*this,config_infor_cache[file_name]);
	}
	else
	{
		AFileImage file;
		if(!file.Open(const_cast<char*>(file_name),A3DFILE_TYPE_BINARY))
		{
			writeErrorInfor("can not open config file named : < " +
				string(file_name) + " >.");
			return false;
		}
		file.Seek(0,SEEK_END);
		size_t f_size = file.GetPos();
		file.Seek(0,SEEK_SET);
		char* cpp = new char[f_size + 1];
		memset(cpp,0,f_size + 1);
		DWORD read_size;
		file.Read(cpp,f_size,&read_size);
		file.Close();
		if(!LoadConfigString(cpp))
		{
			delete [] cpp;
			return false;
		}
		delete [] cpp;

		_config_infor_cache new_item;
		get_cache(*this,new_item);
		config_infor_cache[file_name] = new_item;

	}
	current_state_index = 0;
	return true;
}


static void skipBlankComment(LexicalParse& lp)
{

	J_EXCEPTION_RECORD(0)


	while(true)
	{
		string token;
		long token_type = lp.lookToken(token);
		if( token_type  == TOKEN_TYPE_BLANK || 
			token_type == TOKEN_TYPE_COMMENT)
		{
			lp.goForward();
			continue;
		}
		break;
	}
}
static bool extractFunction(LexicalParse& lp,
					 string& fun_name,
					 vector<_variant>& arg_list)
{

	J_EXCEPTION_RECORD(0)


	arg_list.clear();
	skipBlankComment(lp);
	string token;
	long token_type;
	
	//取得函数名
	token_type = lp.lookToken(token);
	lp.goForward();
	if(token_type  == TOKEN_TYPE_IDENTITY)
	{
		fun_name = token;
	}
	else
	{	
		writeErrorInfor("miss function name!");
		return false;
	}

	skipBlankComment(lp);
	token_type = lp.lookToken(token);
	lp.goForward();
	if(token_type == TOKEN_TYPE_OPERATOR &&	token == "(")
	{
	}
	else
	{
		writeErrorInfor("miss '('");
		return false;
	}

	while(true)
	{
		skipBlankComment(lp);
		token_type = lp.lookToken(token);
		lp.goForward();
		
		bool is_neg = false;

		if(token_type == TOKEN_TYPE_OPERATOR && token == ")")
		{
			break;
		}
		else if(token_type == TOKEN_TYPE_STRING)
		{   
			arg_list.push_back(token.c_str());
			is_neg = false;
			skipBlankComment(lp);
			token_type = lp.lookToken(token);
			lp.goForward();
			if(token_type == TOKEN_TYPE_OPERATOR && token == ")")
			{
				break;
			}
			else if(token_type == TOKEN_TYPE_OPERATOR && token == ",")
			{
				continue;
			}
			else
			{
				writeErrorInfor("incomplete function argument list,miss ',' or ')'.");
				return false;
			}

		}
		else if(token_type == TOKEN_TYPE_BOOL)
		{
			arg_list.push_back(bool(token == "true" || token == "TRUE" ));
			is_neg = false;
			skipBlankComment(lp);
			token_type = lp.lookToken(token);
			lp.goForward();
			if(token_type == TOKEN_TYPE_OPERATOR && token == ")")
			{
				break;
			}
			else if(token_type == TOKEN_TYPE_OPERATOR && token == ",")
			{
				continue;
			}
			else
			{
				writeErrorInfor("incomplete function argument list,miss ',' or ')'.");
				return false;
			}

		}
		else if(token_type == TOKEN_TYPE_DIGITIAL_DEC)
		{
			int i = 0;
			sscanf(token.c_str(),"%d",&i);
			if(is_neg) i = -i;
			is_neg = false;
			arg_list.push_back(i);
			skipBlankComment(lp);
			token_type = lp.lookToken(token);
			lp.goForward();
			if(token_type == TOKEN_TYPE_OPERATOR && token == ")")
			{
				break;
			}
			else if(token_type == TOKEN_TYPE_OPERATOR && token == ",")
			{
				continue;
			}
			else
			{
				writeErrorInfor("incomplete function argument list,miss ',' or ')'.");
				return false;
			}

		}
		else if(token_type == TOKEN_TYPE_DIGITIAL_FLOAT)
		{
			float f = 0;
			sscanf(token.c_str(),"%f",&f);
			if(is_neg) f = -f;
			is_neg = false;
			arg_list.push_back(f);

			skipBlankComment(lp);
			token_type = lp.lookToken(token);
			lp.goForward();
			if(token_type == TOKEN_TYPE_OPERATOR && token == ")")
			{
				break;
			}
			else if(token_type == TOKEN_TYPE_OPERATOR && token == ",")
			{
				continue;
			}
			else
			{
				writeErrorInfor("incomplete function argument list,miss ',' or ')'.");
				return false;
			}
		}
		else if(token_type == TOKEN_TYPE_OPERATOR && token == "-")
		{
			is_neg = true;
		}
	}
	skipBlankComment(lp);
	token_type = lp.lookToken(token);
	lp.goForward();
	if(token_type == TOKEN_TYPE_OPERATOR &&	token == ";")
	{
	}
	else
	{
		writeErrorInfor("function state miss ';'");
		return false;
	}
	return true;
}

static bool GetInvolvedModelFromTXT(const char * txt,
									AListT<AUString>& res)
{

	J_EXCEPTION_RECORD(0)


	string source_string = txt;
	LexicalParse lexical_parse;
	lexical_parse.setSourceString(source_string);
	string mode_name;
	while(true)
	{
		string token;
		skipBlankComment(lexical_parse);
		long token_type = lexical_parse.lookToken(token);
		if(token_type == TOKEN_TYPE_ERROR)
		{
			return false;
		}
		else if(token_type == TOKEN_TYPE_EOF )
		{
			break;
		}
		else if(token_type  == TOKEN_TYPE_IDENTITY)
		{
			string fun_name;
			vector<_variant> arg_list;
			if(!extractFunction(lexical_parse,fun_name,arg_list))
			{
				return false;
			}
			else if(
				(fun_name == "model_sexy" && AModelWraper::model_level == LEVEL_SEXY)||
				(fun_name == "model_blood" && AModelWraper::model_level == LEVEL_BLOOD)||
				(fun_name == "model_urbanity" && AModelWraper::model_level == LEVEL_URBANITY)||
				(fun_name == "model"/* && model_level == LEVEL_NORMAL*/)
				//缺省配置，在模型级别匹配失败或未指定的时使用
				)
			{
				if(arg_list.size() != 1)
				{
					return	false; 
				}
				if(arg_list[0].type != VALUE_TYPE_STRING)
				{
					return	false; 
				}
				mode_name = strlwr((char*)(arg_list[0]));
				res.AddTail(mode_name.c_str());
				continue;
			}
		}
	}
	return true;
}

bool AModelWraper::GetInvolvedModel(const char * file_name,
									AListT<AUString>& res)
{

	J_EXCEPTION_RECORD(0)


	AFileImage file;
	if(!file.Open(const_cast<char*>(file_name),A3DFILE_TYPE_BINARY))
	{
		writeErrorInfor("can not open config file named : < " +
			string(file_name) + " >.");
		return false;
	}
	file.Seek(0,SEEK_END);
	size_t f_size = file.GetPos();
	file.Seek(0,SEEK_SET);
	char* cpp = new char[f_size + 1];
	memset(cpp,0,f_size + 1);
	DWORD read_size;
	file.Read(cpp,f_size,&read_size);
	file.Close();
	if(!GetInvolvedModelFromTXT(cpp,res))
	{
		delete [] cpp;
		return false;
	}
	delete [] cpp;
	return true;
}

static bool GetInvolvedGFXFromTXT(const char * txt,AListT<AUString>& res)
{	

	J_EXCEPTION_RECORD(0)


	string source_string = txt;
	LexicalParse lexical_parse;
	lexical_parse.setSourceString(source_string);

	while(true)
	{
		string token;
		skipBlankComment(lexical_parse);
		long token_type = lexical_parse.lookToken(token);
		if(token_type == TOKEN_TYPE_ERROR)
		{
			return false;
		}
		else if(token_type == TOKEN_TYPE_EOF )
		{
			break;
		}
		else if(token_type  == TOKEN_TYPE_IDENTITY)
		{
			string fun_name;
			vector<_variant> arg_list;
			if(!extractFunction(lexical_parse,fun_name,arg_list))
			{
				return false;
			}
			if(fun_name == "gfx")
			{
				if(arg_list.size() < 1)
				{
					return false;
				}
				if(arg_list[0].type != VALUE_TYPE_STRING)
				{
					return false;
				}
				string gfx_name = strlwr((char*)(arg_list[0]));
				res.AddTail(gfx_name.c_str());
			}
		}
	}
	return true;
}

bool AModelWraper::GetInvolvedGFX(const char * file_name,AListT<AUString>& res)
{

	J_EXCEPTION_RECORD(0)


	AFileImage file;
	if(!file.Open(const_cast<char*>(file_name),A3DFILE_TYPE_BINARY))
	{
		writeErrorInfor("can not open config file named : < " +
			string(file_name) + " >.");
		return false;
	}
	file.Seek(0,SEEK_END);
	size_t f_size = file.GetPos();
	file.Seek(0,SEEK_SET);
	char* cpp = new char[f_size + 1];
	memset(cpp,0,f_size + 1);
	DWORD read_size;
	file.Read(cpp,f_size,&read_size);
	file.Close();
	if(!GetInvolvedGFXFromTXT(cpp,res))
	{
		delete [] cpp;
		return false;
	}
	delete [] cpp;
	return true;
}

//愚蠢但有效的分析方法
bool AModelWraper::LoadConfigString(const char * config_string)
{

	J_EXCEPTION_RECORD(0)


	string source_string = config_string;

	enum DEFINE_FLAG
	{
		DEFINE_UNKNOW,
		DEFINE_STATE,
		DEFINE_CONVERT,
		DEFINE_ACTION	
	} define_flag = DEFINE_UNKNOW;
	LexicalParse lexical_parse;

	lexical_parse.setSourceString(source_string);
	list<action_item> act_list;
	list<pair< string , string > > gfx_list;
	string str_key;
	int	   int_param;
	pair<string,string> pair_key;
	bool is_loop_anim = false;
	while(true)
	{
		string token;
		skipBlankComment(lexical_parse);
		long token_type = lexical_parse.lookToken(token);
		if(token_type == TOKEN_TYPE_ERROR)
		{
			return false;
		}
		else if(token_type == TOKEN_TYPE_EOF )
		{
			if(define_flag != DEFINE_UNKNOW)
			{
				writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
				return false;
			}
			return true;
		}
		else if(token_type  == TOKEN_TYPE_IDENTITY)
		{
			string fun_name;
			vector<_variant> arg_list;

			if(!extractFunction(lexical_parse,fun_name,arg_list))
			{
				return false;
			}
			if(fun_name == "state_begin")
			{
				if(define_flag != DEFINE_UNKNOW)
				{
					writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				define_flag = DEFINE_STATE;
			
				if(arg_list.size() != 1)
				{
					writeErrorInfor("function < state_begin > require 1 arg(s)error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list[0].type != VALUE_TYPE_STRING)
				{
					writeErrorInfor(
						"function state_begin require string argument!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				str_key = (char*)(arg_list[0]);
				if(init_state.empty())
				{
					init_state = str_key;
				}
				state_queue.push_back(str_key);

				continue;
			}
			else if(fun_name == "convert_begin")
			{
				if(define_flag != DEFINE_UNKNOW)
				{
					writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				define_flag = DEFINE_CONVERT;

				if(arg_list.size() != 2)
				{
					writeErrorInfor("function < convert_begin > require 2 arg(s)error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list[0].type != VALUE_TYPE_STRING)
				{
					writeErrorInfor(
						"function state_begin require string argument 2!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list[1].type != VALUE_TYPE_STRING)
				{
					writeErrorInfor(
						"function state_begin require string argument 1!");
					return false;
				}
				pair_key.first =( char * )(arg_list[0]);
				pair_key.second = ( char * )(arg_list[1]);

				continue;
			}
			else if(fun_name == "action_begin")
			{
				if(define_flag != DEFINE_UNKNOW)
				{
					writeErrorInfor("bad function invoke sequenceerror occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				define_flag = DEFINE_ACTION;

				if(arg_list.size() < 2)
				{
					writeErrorInfor("function < convert_begin > require at least 2 arg(s)error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list[0].type != VALUE_TYPE_STRING)
				{
					writeErrorInfor(
						"function state_begin require string argument 1!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				str_key = ( char * )(arg_list[0]);

				if(arg_list[1].type != VALUE_TYPE_BOOL)
				{
					writeErrorInfor(
						"function state_begin require bool argument 3!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				is_loop_anim = ( bool )(arg_list[1]);
				int_param = 1;
				if(arg_list.size() == 3 && is_loop_anim)
				{
					if(arg_list[2].type == VALUE_TYPE_INT)
					{
						int_param = int(int(arg_list[2]) * 30);
					}
					else if(arg_list[2].type == VALUE_TYPE_FLOAT)
					{
						int_param = int(float(arg_list[2]) * 30);
					}
					else
					{
						writeErrorInfor(
							"function state_begin require float argument 4!error occur on unit configed by file <" + config_file_name + ">.");
						return false;
					}
				}
				continue;
			}
			else if(fun_name == "state_end")
			{
				if(define_flag != DEFINE_STATE)
				{
					writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				define_flag = DEFINE_UNKNOW;
				continue;
			}
			else if(fun_name == "convert_end")
			{
				if(define_flag != DEFINE_CONVERT)
				{
					writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				gfx_list.clear();
				define_flag = DEFINE_UNKNOW;
				continue;
			}
			else if(fun_name == "action_end")
			{
				if(define_flag != DEFINE_ACTION)
				{
					writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				define_flag = DEFINE_UNKNOW;
				action_serial_map[str_key] = act_list;
				anim_queue.push_back(str_key);
				act_list.clear();
				continue;
			}
			else if(fun_name == "gfx")
			{
				if(define_flag != DEFINE_STATE && 
					define_flag != DEFINE_CONVERT)
				{
					writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list.size() < 1)
				{
					writeErrorInfor("function < convert_begin > require at least 1 arg(s)error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list[0].type != VALUE_TYPE_STRING)
				{
					writeErrorInfor(
						"function gfx require string argument 1!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				string gfx_name = (char*)(arg_list[0]);
				string hh_name;
				if(arg_list.size() == 2)
				{
					if(arg_list[1].type != VALUE_TYPE_STRING)
					{
						writeErrorInfor(
							"function gfx require string argument 2!error occur on unit configed by file <" + config_file_name + ">.");
						return false;
					}
					hh_name = (char*)(arg_list[1]);
				}
				if(define_flag == DEFINE_STATE)
				{
					state_gfx_map[str_key].push_back(make_pair(gfx_name,hh_name));
				}
				else if(define_flag == DEFINE_CONVERT)
				{
					convert_gfx_map[pair_key].push_back(make_pair(gfx_name,hh_name));
				}
				continue;
			}
			else if(fun_name == "anim")
			{
				action_item val;
				if(define_flag != DEFINE_ACTION)
				{
					writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				
				if(arg_list.size() < 2)
				{
					writeErrorInfor("function < convert_begin > require at least 2 arg(s)error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list[0].type != VALUE_TYPE_STRING)
				{
					writeErrorInfor(
						"function anim require string argument 1!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				val.name = (char*)(arg_list[0]);

				if(arg_list[1].type != VALUE_TYPE_INT)
				{
					writeErrorInfor(
						"function anim require string argument 1!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				val.privilege = (int)(arg_list[1]);

				if(is_loop_anim)
				{
					val.loop_delay = int_param;
					val.loop_name = str_key;
				}
				act_list.push_back(val);
				continue;
			}
			else if(fun_name == "define_HH")
			{

			}

			else if(fun_name == "init_anim")
			{
				if(define_flag != DEFINE_STATE)
				{
					writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list.size() != 1)
				{
					writeErrorInfor("function < init_anim > require 1 arg(s)error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list[0].type != VALUE_TYPE_STRING)
				{
					writeErrorInfor(
						"function init_anim require string argument!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				string init_anim_name = (char*)(arg_list[0]);
				state_init_anim[str_key] = init_anim_name;
				continue;
			}
			else if(
				(fun_name == "model_sexy" && model_level == LEVEL_SEXY)||
				(fun_name == "model_blood" && model_level == LEVEL_BLOOD)||
				(fun_name == "model_urbanity" && model_level == LEVEL_URBANITY)||
				(fun_name == "model"/* && model_level == LEVEL_NORMAL*/)//缺省配置，在模型级别匹配失败或未指定的时使用
				)
			{
				if(define_flag != DEFINE_STATE)
				{
					writeErrorInfor("bad function invoke sequence!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list.size() != 1)
				{
					writeErrorInfor("function < state_begin > require 1 arg(s)error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}

				if(arg_list[0].type != VALUE_TYPE_STRING)
				{
					writeErrorInfor(
						"function state_begin require string argument!error occur on unit configed by file <" + config_file_name + ">.");
					return false;
				}
				string mod_name = (char*)(arg_list[0]);
				state_model_map[str_key] = mod_name;
				continue;
			}
			else 
			{
				writeErrorInfor("unknow function " + token+"error occur on unit configed by file <" + config_file_name + ">.");
				return false;
			}
		}
	}
	return true;
}

bool AModelWraper::playGFX(const char * gfx_name)
{

	J_EXCEPTION_RECORD(0)


	if(string(gfx_name).empty())
	{
		return true;
	}
	A3DGraphicsFX*				m_pGFX;
	if(!m_pEngine->GetA3DGFXMan()->LoadGFXFromFile(const_cast<char*>(gfx_name),
		NULL, NULL, false, &m_pGFX,
		GetModelCenterPoint(),GetDir(),GetUp(),false))
	{
		writeErrorInfor("can not open gfx file named : < " +
			string(gfx_name) + " >." + "Please ensure file : \"" + config_file_name +
					" \" contain correct gfx define.");
		return false;
	}
	m_pGFX->Start(true);
	return true;
}

bool AModelWraper::playGFX(const char* gfx_name,const A3DVECTOR3& pos)
{

	J_EXCEPTION_RECORD(0)


	if(string(gfx_name).empty())
	{
		return true;
	}
	A3DGraphicsFX*				m_pGFX;
	if(!m_pEngine->GetA3DGFXMan()->LoadGFXFromFile(const_cast<char*>(gfx_name),
		NULL, NULL, false, &m_pGFX,
		pos,GetDir(),GetUp(),false))
	{
		writeErrorInfor("can not open gfx file named : < " +
			string(gfx_name) + " >." + "Please ensure file : \"" + config_file_name +
					" \" contain correct gfx define.");
		return false;
	}
	m_pGFX->Start(true);
	return true;	
}

bool AModelWraper::PlayGFX(const char* gfx_name)
{
	return playGFX(gfx_name);
}
bool AModelWraper::PlayGFX(const char* gfx_name,const A3DVECTOR3& pos)
{
	return playGFX(gfx_name,pos);
}
bool AModelWraper::PlayGFX(const char* gfx_name,const char* hh_name)
{
	return playGFXonModel(gfx_name,hh_name);
}
bool AModelWraper::playGFXonModel(const char * gfx_name,
								   const char * link_point_name)
{

	J_EXCEPTION_RECORD(0)


	if(string(gfx_name).empty())
	{
		return true;
	}
	if(!pCurrentModel)
	{
		return playGFX(gfx_name);
	}
	A3DGraphicsFX*				m_pGFX;
	if(!m_pEngine->GetA3DGFXMan()->LoadGFXFromFile(const_cast<char*>(gfx_name),
		pCurrentModel,const_cast<char*>(link_point_name),true,&m_pGFX,
		GetPos(),GetDir(),GetUp(),false))
	{
		writeErrorInfor("can not open gfx file named : < " + string(gfx_name) +
			" >, or bad connection." + "Please ensure file : \"" + config_file_name +
					" \" contain correct gfx define.");
		return false;
	}
	m_pGFX->Start(true);
	return true;
}
bool AModelWraper::loadModel(const char * model_name)
{

	J_EXCEPTION_RECORD(0)
	releaseModel();
	ClearActionQueue();
	if(string(model_name).empty())
	{
		return true;
	}

#ifdef _DEBUG
	static bool bFirst = true;
	FILE * fp=NULL;
	if(bFirst)
	{
		fp = fopen("data/preload/preloadinfo.txt","wt");
		bFirst = false;
	}
	else
	{
		fp = fopen("data/preload/preloadinfo.txt","at");
	}
	if(!fp)
	{
		assert(0);
		return false;
	}
	fprintf(fp,"%s\n",model_name);
	fclose(fp);
#endif

	if(!m_pEngine->GetA3DModelMan()->LoadModelFile(
		const_cast<char*>(model_name),&pCurrentModel))
	{
		writeErrorInfor("Engine can not open model file named : < " +
			string(model_name) + " >." + "Please ensure file : \"" + config_file_name +
					" \" contain correct model define.");

		return false;
	}
	
	pCurrentModel->TickAnimation();
	pCurrentModel->SetActionLoopEndCallBack(call_back_action_end,this);
	pCurrentModel->SetLogicEventCallBack(call_back_logic_event,this);
	fillConnectPointSet();
	return true; 
}
void AModelWraper::releaseModel()
{

	J_EXCEPTION_RECORD(0)


	if(pCurrentModel)
	{
		m_pEngine->GetA3DModelMan()->ReleaseModel(pCurrentModel);
		pCurrentModel = 0;
	}
	map<string,A3DModel *>::iterator itor = assistant_model_map.begin();
	for(;itor != assistant_model_map.end();++itor)
	{
		if((*itor).second)
		{
			m_pEngine->GetA3DModelMan()->ReleaseModel((*itor).second);
			(*itor).second = 0;
		}
	}
	assistant_model_map.clear();
}

bool AModelWraper::Render(A3DViewport * pCurrentViewport)
{

	J_EXCEPTION_RECORD(0)

	if(pCurrentModel && !pCurrentModel->Render(pCurrentViewport))
	{
		return false;
	}	

	map<string,A3DModel *>::iterator itor = assistant_model_map.begin();
	for(;itor != assistant_model_map.end();++itor)
	{
		A3DModel * pItem = (*itor).second;
		if(pItem && (pItem->GetParentFrame()==0))
		{
			pItem->Render(pCurrentViewport);
		}
	}

	return true;
}

bool AModelWraper::TickAnimation()
{

	J_EXCEPTION_RECORD(0)


	if(pCurrentModel)
	{
		if(! pCurrentModel->TickAnimation() )
		{
			return false;
		}

		map<string,A3DModel *>::iterator itor = assistant_model_map.begin();
		for(;itor != assistant_model_map.end();++itor)
		{
			A3DModel * pItem = (*itor).second;
			if(pItem && (pItem->GetParentFrame()==0))
			{
				pItem->TickAnimation();
			}
		}

		if(! handleActionQueue() )
		{
			return false;
		}
	}
	return true;
}/*
bool AModelWraper::AABBTrace(AABBTRACEINFO* pInfo, AABBTRACE* pTrace)
{
	if(pCurrentModel)
	{
		return pCurrentModel->AABBTrace(pInfo,pTrace);
	}
	return false;
}*/
bool AModelWraper::RayTrace(const A3DVECTOR3& vecStart,
							const A3DVECTOR3& vecDelta, RAYTRACE * pRayTrace)
{
	
	J_EXCEPTION_RECORD(0)


	RAYTRACE ray_trace;
	ray_trace.fFraction = 1.0;
	pRayTrace->fFraction = 1.0;
	//主模型的碰撞检测
	if(pCurrentModel)
	{
		if(pCurrentModel->RayTrace(
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
	//辅助模型的碰撞检测
	map<string,A3DModel *>::iterator itor = assistant_model_map.begin();
	for(;itor!=assistant_model_map.end();++itor)
	{	
		if(
			(*itor).second->RayTrace(
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
	return pRayTrace->fFraction<1.0f;
}
bool AModelWraper::SetStateByName(const char * state_name)
{

	J_EXCEPTION_RECORD(0)


	//设置的和以前相同就忽略
	if(last_state == state_name)
	{
		return true;
	}
	//如果状态是不可识别的就忽略
	if(state_model_map.find(state_name) == state_model_map.end())
	{
		return true;
	}

	if(convert_gfx_map.find(make_pair(last_state,state_name)) 
		!= convert_gfx_map.end())
	{
		vector<pair<string , string > > & gfx_list = convert_gfx_map[make_pair(last_state,state_name)]; 
		vector<pair<string , string > >::iterator itor = gfx_list.begin();
		for(;itor != gfx_list.end();++itor)
		{
			string nn = (*itor).first;
			string hh = (*itor).second;
			if(!hh.empty())
			{
				if(!playGFXonModel(nn.c_str(),hh.c_str()))
				{
					return false;
				}
			}
			else
			{
				if(!playGFX(nn.c_str()))
				{
					return false;
				}
			}
		}
	}

	//如果相应的容器找到了要表现的内容但是无法表现时才返回false

	if(!loadModel(state_model_map[state_name].c_str()))
	{
		return false;
	}

	play_anim_ready = true;
	current_action_queue.clear();
	current_anim_privilege = -10;
	next_call_anim = "";
	next_call_delay = 0;

	if(state_init_anim.find(state_name) != state_init_anim.end())
	{
		const string& anim_name = state_init_anim[state_name];
		if(!anim_name.empty())
		{
			if(anim_name[0] == '[' && anim_name[anim_name.length() - 1] == ']')
			{
				AppendLowLevelActionToTail(anim_name.c_str());
			}
			else
			{
				AppendActionToTail(anim_name.c_str());
			}
		}
	}

	if(state_gfx_map.find(state_name) != state_gfx_map.end())
	{
		vector<pair<string , string > > & gfx_list = state_gfx_map[state_name]; 
		vector<pair<string , string > >::iterator itor = gfx_list.begin();
		for(;itor != gfx_list.end();++itor)
		{
			string nn = (*itor).first;
			string hh = (*itor).second;
			if(!hh.empty())
			{
				if(!playGFXonModel(nn.c_str(),hh.c_str()))
				{
					return false;
				}
			}
			else
			{
				if(!playGFX(nn.c_str()))
				{
					return false;
				}
			}
		}
	}

	last_state = state_name;
	return true;
}


string AModelWraper::GetCurrentStateName()
{
	return last_state;
}
int  AModelWraper::GetStateCount()
{
	return state_queue.size();
}
int	 AModelWraper::GetState()
{
	return current_state_index;
}
bool AModelWraper::NextState()
{

	J_EXCEPTION_RECORD(0)


	if(current_state_index > state_queue.size() - 1 )
	{
		return false;
	}

	string new_state_name;
	++current_state_index;
	if(current_state_index == state_queue.size())
	{
		new_state_name = "";
	}
	else
	{
		new_state_name = state_queue[current_state_index];
	}
	if(SetStateByName(new_state_name.c_str()))
	{
		return true;
	}
	return false;
}
void AModelWraper::recursionFindConnectPoint(A3DFrame * pFrame)
{

	J_EXCEPTION_RECORD(0)


	char szFrameUpper[256];
	strncpy(szFrameUpper, pFrame->GetName(), 256);
	strupr(szFrameUpper);
	if( strstr(szFrameUpper, "HH_") == szFrameUpper )
	{
		connect_name_vector.push_back(string(szFrameUpper));
		connect_ptr_vector.push_back(pFrame);
	}
	//Then see if it is my child frame's child frame;
	A3DFrame * pChildFrame = pFrame->GetFirstChildFrame();
	while( pChildFrame )
	{
		recursionFindConnectPoint(pChildFrame);
		pChildFrame = pFrame->GetNextChildFrame();
	}
}
void AModelWraper::fillConnectPointSet()
{

	J_EXCEPTION_RECORD(0)


	connect_name_vector.clear();
	connect_ptr_vector.clear();
	AList * pChildFrameList = pCurrentModel->GetChildFrameList();
	ALISTELEMENT * pThisFrameElement = pChildFrameList->GetFirst();
	while( pThisFrameElement != pChildFrameList->GetTail() )
	{
		A3DFrame * pFrame = (A3DFrame *) pThisFrameElement->pData;
		recursionFindConnectPoint(pFrame);
		pThisFrameElement = pThisFrameElement->pNext;
	}
}
vector<string >&AModelWraper::EnumHHName()
{
	return connect_name_vector;
}
vector<A3DFrame * >&AModelWraper::EnumHHPtr()
{
	return connect_ptr_vector;
}
vector<string>&	AModelWraper::EnumSupportAnimName()
{
	return anim_queue;
}
vector<string>&	AModelWraper::EnumSupportStateName()
{
	return state_queue;
}
bool AModelWraper::ForcePlayAnim(const char * anim_name)
{

	J_EXCEPTION_RECORD(0)


	ClearActionQueue();
	if(pCurrentModel && !pCurrentModel->
		PlayActionByName(const_cast<char*>(anim_name),true,false))
	{
		writeErrorInfor(string("can not play animation named : < ") 
			+ anim_name + " >.error occur on unit configed by file <" + config_file_name + ">.");
		return false;
	}
	anim_in_engine = anim_name;
	return true;
}
bool AModelWraper::AppendAssistantModel(const char* model_name,const char* hh_name, const char* cc_name)
{

	J_EXCEPTION_RECORD(0)


	A3DModel * pItem = 0;
	if(string(model_name).empty())
	{
		return true;
	}

	if(!m_pEngine->GetA3DModelMan()->LoadModelFile(
		const_cast<char*>(model_name),&pItem))
	{
		writeErrorInfor("can not open model file named : < " +
			string(model_name) + " >.error occur on unit configed by file <" + config_file_name + ">.");
		return false;
	}



	pItem->TickAnimation();


	if(assistant_model_map.find(model_name) != assistant_model_map.end())
	{
		if(assistant_model_map[model_name]) 
		{
			m_pEngine->GetA3DModelMan()->ReleaseModel(assistant_model_map[model_name]);
			delete assistant_model_map[model_name];
		}
	}
	assistant_model_map[model_name] = pItem;
	if(strcmp("",hh_name) != 0 && pCurrentModel)
	{
		char * pCCName = strcmp("",cc_name) == 0 ? NULL : const_cast<char*>(cc_name);

		if(pCurrentModel->AddChildModel(pItem,const_cast<char*>(hh_name),const_cast<char*>(pCCName)))
		{
			assistant_model_link_infor[model_name] = make_pair(hh_name,cc_name);
		}
		else
		{
			writeErrorInfor("can not link model : < " +	string(model_name) + " >. to main model HH - Name is < " + string(hh_name) + " > ;  CC - Name is < " + string(cc_name) + " >.");
		}
	}
	else
	{
		pItem->SetPos(GetPos());
	}
	pItem->SetLogicEventCallBack(call_back_logic_event,this);
	return true; 
}
bool AModelWraper::DeleteAssistantModel(const char* model_name)
{

	J_EXCEPTION_RECORD(0)


	if(string(model_name).empty())
	{
		return true;
	}
	if(assistant_model_map.find(model_name) != assistant_model_map.end())
	{
		A3DModel* pModel	= assistant_model_map[model_name];
		
		if(pModel != NULL)
		{
			if(assistant_model_link_infor.find(model_name) != assistant_model_link_infor.end())
			{
				string hh_name = assistant_model_link_infor[model_name].first;
				if(hh_name != "" )
				{
					pCurrentModel->DeleteChildModel(pModel);
				}
			}
			m_pEngine->GetA3DModelMan()->ReleaseModel(pModel);
			delete pModel;
		}
	}
	assistant_model_link_infor.erase(model_name);
	assistant_model_map.erase(model_name);
	return true; 
}
A3DModel * AModelWraper::GetAssistantModel(const char* model_name)
{

	J_EXCEPTION_RECORD(0)


	if(assistant_model_map.find(model_name) != assistant_model_map.end())
	{
		return assistant_model_map[model_name];
	}
	else return 0;
}
bool AModelWraper::NoitfyAssistantModelPlayAnim(const char* model_name,const char * anim_name)
{

	J_EXCEPTION_RECORD(0)


	A3DModel * pItem = GetAssistantModel(model_name);
	if(pItem && !pItem->
		PlayActionByName(const_cast<char*>(anim_name),true,false))
	{
		writeErrorInfor(string("can not play animation named : < ") 
			+ anim_name + " >." + "Please ensure file : \"" + config_file_name +
					" \" contain correct model define.");
		return false;
	}
	return true;
}
bool AModelWraper::NoitfyAssistantModelPlayGFX(const char* model_name,const char * gfx_name)
{

	J_EXCEPTION_RECORD(0)


	A3DModel * pItem = GetAssistantModel(model_name);
	if(pItem)
	{
		A3DGraphicsFX*				m_pGFX;
		pItem->UpdateToFrame(-1);
		pItem->UpdateModelOBB();
		A3DVECTOR3 pos = pItem->GetModelAABB().Center;
		if(pItem && m_pEngine->GetA3DGFXMan()->LoadGFXFromFile(const_cast<char*>(gfx_name),
			NULL, NULL, false, &m_pGFX,
			GetModelCenterPoint(),GetDir(),GetUp(),false))
		{
			writeErrorInfor("can not open gfx file named : < " + string(gfx_name) +
				" >, or bad connection." + "Please ensure file : \"" + config_file_name +
				" \" contain correct model define.");
			return false;
		}
		m_pGFX->Start(true);
	}
	return true;	
}
bool AModelWraper::NoitfyAssistantModelPlayGFXOnModel(const char* model_name,const char * gfx_name,const char * hh_name)
{

	J_EXCEPTION_RECORD(0)


	A3DModel * pItem = GetAssistantModel(model_name);
	if(pItem)
	{
		A3DGraphicsFX*				m_pGFX;
		pItem->UpdateToFrame(-1);
		pItem->UpdateModelOBB();
		A3DVECTOR3 pos = pItem->GetModelAABB().Center;
		if(pItem && !m_pEngine->GetA3DGFXMan()->LoadGFXFromFile(const_cast<char*>(gfx_name),
			pCurrentModel,const_cast<char*>(hh_name),true,&m_pGFX,
			pos,pItem->GetDir(),pItem->GetUp(),false))
		{
			writeErrorInfor("can not open gfx file named : < " + string(gfx_name) +
				" >, or bad connection." + "Please ensure file : \"" + config_file_name +
				" \" contain correct gfx define.");
			return false;
		}
		m_pGFX->Start(true);
	}
	return true;	
}

bool AModelWraper::GetAnimationNameSet(const char * file_name,AListT<AUString>& res)
{

	J_EXCEPTION_RECORD(0)


	LexicalParse lexical_parse;

	AFileImage file;
	if(!file.Open(const_cast<char*>(file_name),A3DFILE_TYPE_BINARY))
	{
		writeErrorInfor("can not open config file named : < " +
			string(file_name) + " >.");
		return false;
	}
	file.Seek(0,SEEK_END);
	size_t f_size = file.GetPos();
	file.Seek(0,SEEK_SET);
	char* cpp = new char[f_size + 1];
	memset(cpp,0,f_size + 1);
	DWORD read_size;
	file.Read(cpp,f_size + 1,&read_size);
	file.Close();
	lexical_parse.setSourceString(cpp);
	delete [] cpp;

	res.RemoveAll();
	while(true)
	{
		string token;
		skipBlankComment(lexical_parse);
		long token_type = lexical_parse.lookToken(token);
		if(token_type == TOKEN_TYPE_ERROR)
		{
			return false;
		}
		else if(token_type == TOKEN_TYPE_EOF )
		{
			return true;
		}
		else if(token_type == TOKEN_TYPE_IDENTITY)
		{
			string fun_name;
			vector<_variant> arg_list;
			if(extractFunction(lexical_parse,fun_name,arg_list))
			{
				if(fun_name == "action_begin")
				{
					if(arg_list.size()<1)
					{
						writeErrorInfor("Miss Function Name!");
						return false;
					}
					if(arg_list[0].type != VALUE_TYPE_STRING)
					{
						writeErrorInfor("Miss Function Name!");
						return false;
					}
					res.AddTail((char*)(arg_list[0]));
				}
			}
			else
			{
				return false;	
			}
		}
		else 
		{
			return false;
		}
	}
	return true;
}

bool AModelWraper::GetHHNameSet(const char * file_name,AListT<HH_infor>& res)
{
	J_EXCEPTION_RECORD(0)

	LexicalParse lexical_parse;

	AFileImage file;
	if(!file.Open(const_cast<char*>(file_name),A3DFILE_TYPE_BINARY))
	{
		writeErrorInfor("can not open config file named : < " +
			string(file_name) + " >.");
		return false;
	}
	file.Seek(0,SEEK_END);
	size_t f_size = file.GetPos();
	file.Seek(0,SEEK_SET);
	char* cpp = new char[f_size + 1];
	memset(cpp,0,f_size + 1);
	DWORD read_size;
	file.Read(cpp,f_size + 1,&read_size);
	file.Close();
	lexical_parse.setSourceString(cpp);
	delete [] cpp;

	res.RemoveAll();
	while(true)
	{
		string token;
		skipBlankComment(lexical_parse);
		long token_type = lexical_parse.lookToken(token);
		if(token_type == TOKEN_TYPE_ERROR)
		{
			return false;
		}
		else if(token_type == TOKEN_TYPE_EOF )
		{
			return true;
		}
		else if(token_type == TOKEN_TYPE_IDENTITY)
		{
			string fun_name;
			vector<_variant> arg_list;
			if(extractFunction(lexical_parse,fun_name,arg_list))
			{
				if(fun_name == "define_HH")
				{
					if(arg_list.size()<2)
					{
						writeErrorInfor("2 args!");
						return false;
					}
					if(arg_list[0].type != VALUE_TYPE_STRING)
					{
						writeErrorInfor("arg1 need str!");
						return false;
					}
					if(arg_list[1].type != VALUE_TYPE_INT)
					{
						writeErrorInfor("arg2 need int!");
						return false;
					}
					HH_infor hh_infor;
					hh_infor.index = int(arg_list[1]);
					hh_infor.name  = static_cast<const char*>((char*)arg_list[0]);
					res.AddTail(hh_infor);
				}
			}
			else
			{
				return false;	
			}
		}
		else 
		{
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------

bool AModelWraper::handleActionQueue()
{

	J_EXCEPTION_RECORD(0)


	//首先检查控制开关是否是开放的
	if(play_anim_ready)
	{	
		//当前执行队列非空时
		if(!current_action_queue.empty())
		{
			const action_item& item = current_action_queue.front();
			//弹出并播放队列中的元素
			string anim_name = item.name;
			current_anim_privilege = item.privilege;
			//如果这个元素有循环标记，就记录下来
			if(!item.loop_name.empty())
			{
				next_call_anim = item.loop_name;
				next_call_delay = item.loop_delay;
			}
			if(pCurrentModel && !pCurrentModel->
				PlayActionByName(const_cast<char*>(anim_name.c_str()),true,false))
			{
				writeErrorInfor("Model can not play animation named : < " 
					+ anim_name + " >." + "Please ensure file : \"" + config_file_name +
					" \" contain correct action define.");
				return false;
			}
			//把控制开关关上
			play_anim_ready = false;
			//通知宿主
			m_pHostListener ->anim_change(anim_name.c_str(),item.p_outer_data);
			anim_in_engine = anim_name;
			current_action_queue.pop_front();
		}
		else//当前队列为空时，检查是否有循环信息，有的话就递减延迟计数器减到零时进行循环
		{
			if(!next_call_anim.empty() && -- next_call_delay <= 0)
			{
				next_call_delay = 0;
				AddActionToHead(next_call_anim.c_str());
			}
		}
	}
	return true;
}

bool AModelWraper::AppendActionToTail(const char * act_name,void* p_buffer)
{

	J_EXCEPTION_RECORD(0)


	//高级动画名不存在就忽略
	if(action_serial_map.find(act_name) == action_serial_map.end())
	{
		writeErrorInfor("unit can not execute action named : < " 
			+ string(act_name) + " >. please ensure file < " + config_file_name + " > contain this action define.");

		return false;
	}
	//得到高级动画名对应的低级动画序列
	list<action_item>& act_list = action_serial_map[act_name];
	//插入用户指定的信息
	if(p_buffer != 0 && !act_list.empty())
	{
		act_list.front().p_outer_data = p_buffer;
	}
	current_action_queue.insert(current_action_queue.end(),act_list.begin(),act_list.end());
	return true;
}
bool AModelWraper::AddActionToHead(const char * act_name,void * p_buffer)
{

	J_EXCEPTION_RECORD(0)


	//高级动画名不存在就忽略
	if(action_serial_map.find(act_name) == action_serial_map.end())
	{
		writeErrorInfor("unit can not execute action named : < " 
			+ string(act_name) + " >. please ensure file < " + config_file_name + " > contain this action define.");
		return false;
	}
	//得到高级动画名对应的低级动画列
	list<action_item>& act_list = action_serial_map[act_name];
	//插入用户指定的信息
	if(p_buffer != 0 && !act_list.empty())
	{
		act_list.front().p_outer_data = p_buffer;
	}
	if(	current_anim_privilege <= act_list.front().privilege)
	{
		current_action_queue.insert(current_action_queue.begin(),act_list.begin(),act_list.end());
		//强行播放下一个动画
		play_anim_ready = true;
		return true;
	}
	return false;	
}
void AModelWraper::ClearActionQueue()
{

	J_EXCEPTION_RECORD(0)


	current_action_queue.clear();
	play_anim_ready = true;
	next_call_anim = "";
	current_anim_privilege = -10;
	next_call_delay = 0;
	anim_in_engine = "";
	if(pCurrentModel)
	{
		pCurrentModel->PauseAnimation(true);
	}
}
void AModelWraper::call_back_logic_event(A3DMODEL_LOGIC_EVENT * pLogicEvent, LPVOID pArg)
{

	J_EXCEPTION_RECORD(0)


	//调用反馈接口
	AModelWraper * pThis = static_cast<AModelWraper * >(pArg);
	if(pThis->m_pHostListener)
	{
		pThis->m_pHostListener->receive_feedback(pLogicEvent->szNotifyString);
	}
}
void AModelWraper::call_back_action_end(A3DModel * pNoUsing, LPVOID pArg)
{

	J_EXCEPTION_RECORD(0)


	//动画项执行结束时打开开关
	AModelWraper * pThis = static_cast<AModelWraper * >(pArg);
	pThis->play_anim_ready = true;
	pThis->current_anim_privilege = -10;
	pThis->m_pHostListener->anim_end(pThis->anim_in_engine.c_str());
	pThis->anim_in_engine = "";
}
bool AModelWraper::AddLowLevelActionToHead(const char * state_name)
{

	J_EXCEPTION_RECORD(0)


	action_item item;
	item.name = state_name;	
	item.loop_name = "";
	item.loop_delay = 0;
	item.privilege = 3;
	if(	current_anim_privilege <= item.privilege)
	{
		current_action_queue.push_front(item);
		//强行播放下一个动画
		play_anim_ready = true;
		return true;
	}
	return false;
}
bool AModelWraper::AppendLowLevelActionToTail(const char * state_name)
{

	J_EXCEPTION_RECORD(0)


	action_item item;
	item.name = state_name;	
	item.loop_name = "";
	item.loop_delay = 0;
	item.privilege = 3;
	current_action_queue.push_back(item);
	play_anim_ready = true;
	return true;
}
bool AModelWraper::IsInLoopAction()
{
	J_EXCEPTION_RECORD(0)


	return !next_call_anim.empty();
}

bool AModelWraper::HasLoopInAction()
{

	J_EXCEPTION_RECORD(0)


	if(IsInLoopAction())
		return true;
	list<action_item>::iterator itor = current_action_queue.begin();
	for(;itor != current_action_queue.end();++itor)
	{
		const action_item& item = *itor;
		if(!item.loop_name.empty())
		{
			return true;
		}
	}
	return false;
}

void AModelWraper::SetConfigFileModelLevel(MODEL_LEVEL level)
{
	model_level = level;
}

bool AModelWraper::IsEmptyActionQueue()
{
	return !IsInLoopAction()&&current_action_queue.empty();
}

bool AModelWraper::IsIdle()
{
	return IsEmptyActionQueue() && anim_in_engine.empty();
}
#endif