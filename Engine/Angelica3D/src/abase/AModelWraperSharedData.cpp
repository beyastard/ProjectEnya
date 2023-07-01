#include "AModelWraperSharedData.h"
#include "ADebug.h"

config_file_c_imp g_ConfigFileCImp;

bool config_file_c_imp::pseudo_open()
{
	try
	{
		_matters_files();
	}
	catch(const char* sz)
	{
		ADebug::Msg(0,sz);
		assert(0);
	}
	catch(...)
	{
		ADebug::Msg(1,"unknow error at : %s %d error",__FILE__,__LINE__);
		assert(0);
	}
	return true;
}

void config_file_c_imp::_matters_files()
{
file_begin("data/model/手榴弹.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/手榴弹.mod","旋转");

state_end();
/*
state_begin("");

	model("");

state_end();
*/
action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/掷弹兵.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/掷弹兵_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();



//
action_begin("由蹲到站",false);

	anim("[由蹲到站]"		,2		);

action_end();

action_begin("由蹲到站投弹",false);

	anim("[由蹲到站投弹]"		,2		);

action_end();

action_begin("由站到蹲",false);

	anim("[由站到蹲]"		,2		);

action_end();

action_begin("由蹲到站投弹下蹲_循环",true);//需加延时

	anim("[由蹲到站]"		,2		);
	anim("[由蹲到站投弹]"		,2		);
	anim("[由站到蹲]"		,2		);

action_end();

action_begin("由蹲到站投弹_循环",true);//需加延时

	anim("[由蹲到站投弹]"		,2		);

action_end();

action_begin("左转身",false);

	anim("[左转身]"		,2		);

action_end();

action_begin("左转身投弹",false);

	anim("[左转身投弹]"		,2		);

action_end();

action_begin("向右闪避",false);

	anim("[向右闪避]"		,2		);

action_end();

action_begin("左转身投弹右闪避_循环",true);//需加循环

	anim("[左转身]"		,2		);
	anim("[左转身投弹]"		,2		);
	anim("[向右闪避]"		,2		);

action_end();

action_begin("左转身投弹_循环",true);//需加循环

	anim("[左转身投弹]"		,2		);

action_end();

action_begin("右转身",false);

	anim("[右转身]"		,2		);

action_end();

action_begin("右转身投弹",false);

	anim("[右转身投弹]"		,2		);

action_end();

action_begin("向左闪避",false);

	anim("[向左闪避]"		,2		);

action_end();

action_begin("右转身投弹左闪避_循环",true);//需加循环

	anim("[右转身]"		,2		);
	anim("[右转身投弹]"		,2		);
	anim("[向左闪避]"		,2		);

action_end();

action_begin("右转身投弹_循环",true);//需加循环

	anim("[右转身投弹]"		,2		);

action_end();

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//11-22
action_begin("胸部中弹撞墙死亡",false);
	
	anim("[胸部中弹撞墙死亡]"	,2		);

action_end();

//12-21 更新
action_begin("高处死亡2",false);
	
	anim("[高处死亡2]"	,4		);

action_end();

action_begin("高处死亡下落2_循环",true);
	
	anim("[高处死亡下落2]"	,4		);

action_end(); 

action_begin("高处死亡落地2",false);
	
	anim("[高处死亡落地2]"	,4		);

action_end();

file_end();
file_begin("data/model/消防栓.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/消防栓.mod");

state_end();

state_begin("");

	model("matters/消防栓损毁.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/铁丝网.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/铁丝网.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/收音机.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/收音机.mod");

state_end();

state_begin("");

	model("");

state_end();

convert_begin("寻常","");

	gfx("击毁_收音机.gfx","");

convert_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/木箱1.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/普通木箱.mod");

state_end();


//-------------------------------------------------------------
file_end();
file_begin("data/model/木箱2.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/普通木箱2.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/酒瓶01.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/酒瓶01.mod");

state_end();

state_begin("");

	model("");

state_end();

convert_begin("寻常","");

	gfx("击毁_酒瓶.gfx","");

convert_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/广告牌04.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/广告牌04.mod");

state_end();
state_begin("");

	model("");

state_end();

convert_begin("寻常","");

	gfx("击毁_牌匾.gfx","");

convert_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/弹药箱.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Ammo character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/弹药箱.mod");

state_end();

state_begin("");

	model("");

state_end();

convert_begin("寻常","");

	gfx("击毁_弹药箱.gfx","");

convert_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/街道沙包掩体01.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/街道沙包掩体01.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/街道沙包掩体02.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/街道沙包掩体02.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/街道沙包掩体03.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/街道沙包掩体03.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/花01.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/花01.mod");

state_end();

state_begin("");
	
	model("");

state_end();
convert_begin("寻常","");

	gfx("击毁_花盆.gfx","");

convert_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/花02.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/花02.mod");

state_end();

state_begin("");
	
	model("");

state_end();
convert_begin("寻常","");

	gfx("击毁_花盆.gfx","");

convert_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/浪人2.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/浪人2_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("举刀前跑_循环",true);
	
	anim("[举刀前跑]"	,2		);

action_end(); 

action_begin("左跑转身",false);

	anim("[左跑挥刀]"		,2		);

action_end();

action_begin("举刀",false);

	anim("[举刀]"		,2		);

action_end();

action_begin("前砍",false);

	anim("[前砍]"		,2		);

action_end();

action_begin("前砍_循环",true);//需加延时

	anim("[再砍]"		,2		);

action_end();

action_begin("高空跳下砍",false);

	anim("[跳起]"		,2		);
	anim("[跳在空中]"		,2		);
	anim("[跳下砍]"		,2		);

action_end();

action_begin("前翻滚_循环",true);
	
	anim("[前滚翻]"	,4		);

action_end(); 

action_begin("前刺",false);

	anim("[前刺]"		,2		);

action_end();

action_begin("前刺_循环",true);//需加循环

	anim("[前刺]"		,2		);

action_end();

action_begin("右转身挥刀",false);

	anim("[右转身挥刀]"		,2		);

action_end();

action_begin("右转身挥刀姿势",false);

	anim("[右转身挥刀姿势]"		,2		);

action_end();

action_begin("左转身挥刀",false);

	anim("[左转身挥刀]"		,2		);

action_end();

action_begin("左转身挥刀姿势",false);

	anim("[左转身挥刀姿势]"		,2		);

action_end();

action_begin("后退举刀",false);

	anim("[后退举刀]"		,2		);

action_end();

action_begin("后退循环",true);

	anim("[后退]"		,2		);

action_end();

action_begin("站立举刀",false);

	anim("[站立举刀]"		,2		);

action_end();

action_begin("再砍",false);

	anim("[再砍]"		,2		);

action_end();

action_begin("预备姿势",false);

	anim("[预备姿势]"		,2		);

action_end();

action_begin("左跑_循环",true);

	anim("[提刀向左跑]"		,2		);

action_end();

action_begin("高空跳下",false);

	anim("[跳起2]"		,2		);
	anim("[跳在空中2]"		,2		);
	anim("[落地]"		,2		);

action_end();

action_begin("跳起在空中",false);

	anim("[跳起2]"		,2		);
	anim("[跳在空中2]"		,2		);

action_end();

action_begin("跳起落地",false);

	anim("[落地]"		,2		);

action_end();

action_begin("前翻滚",false);
	
	anim("[前滚翻]"	,4		);

action_end(); 

action_begin("高空跳下起跳",false);

	anim("[跳起]"		,2		);

action_end();

action_begin("高空跳下在空中",true);

	anim("[跳在空中]"		,2		);

action_end();

action_begin("高空跳下跳下砍",false);

	anim("[跳下砍]"		,2		);

action_end();

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-13 新添动作
action_begin("空中死亡",false);
	
	anim("[空中死亡]"	,4		);

action_end();

action_begin("空中死亡下落_循环",true);
	
	anim("[空中死亡下落]"	,4		);

action_end(); 

action_begin("空中死亡落地",false);
	
	anim("[空中死亡落地]"	,4		);

action_end();

//11-13-2 新添动作
action_begin("提刀左跑_循环",true);
	
	anim("[提刀向坐跑]"	,4		);

action_end(); 

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//11-22
action_begin("胸部中弹撞墙死亡",false);
	
	anim("[胸部中弹撞墙死亡]"	,2		);

action_end();

//12-21 更新
action_begin("高处死亡2",false);
	
	anim("[高处死亡2]"	,4		);

action_end();

action_begin("高处死亡下落2_循环",true);
	
	anim("[高处死亡下落2]"	,4		);

action_end(); 

action_begin("高处死亡落地2",false);
	
	anim("[高处死亡落地2]"	,4		);

action_end();

file_end();
file_begin("data/model/巴祖卡火箭.txt");

state_begin("寻常");
	model("matters/巴祖卡火箭.mod","旋转");
state_end();

state_begin("");

	model("");

state_end();

action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/巴祖卡火箭弹.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/巴祖卡火箭弹.mod","旋转");

state_end();

state_begin("");

	model("");

state_end();

action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/捷克ZB26轻机枪.txt");

state_begin("寻常");

	model("matters/捷克ZB26轻机枪.mod","旋转");

state_end();



action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

file_end();

file_begin("data/model/毛瑟狙击步枪.txt");
state_begin("寻常");

	model("matters/毛瑟狙击步枪.mod","旋转");

state_end();

state_begin("");

	model("");

state_end();

action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/汤姆森冲锋枪.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/汤姆森冲锋枪.mod","旋转");

state_end();

state_begin("");

	model("");

state_end();

action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/主角用子弹夹.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主角用子弹夹.mod","旋转");

state_end();

state_begin("");

	model("");

state_end();
action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/马克沁重机枪.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/九二式重机枪.mod");

state_end();

state_begin("");

	model("");

state_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/战上海_老式汽车01.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/战上海_老式汽车01.mod");

state_end();

state_begin("击毁");

	model("matters/战上海_老式汽车损坏01.mod");

state_end();

convert_begin("寻常","击毁");

	gfx("车辆被击毁效果.gfx","");

convert_end();


//-------------------------------------------------------------
file_end();
file_begin("data/model/战上海_老式汽车03.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/战上海_老式汽车03.mod");

state_end();

state_begin("");

	model("matters/战上海_老式汽车损坏03.mod");

state_end();

convert_begin("寻常","击毁");

	gfx("车辆被击毁效果.gfx","");

convert_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/战上海_老式汽车04.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/战上海_老式汽车04.mod");

state_end();

state_begin("击毁");

	model("matters/战上海_老式汽车损坏04.mod");

state_end();

convert_begin("寻常","击毁");

	gfx("车辆被击毁效果.gfx","");

convert_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/重机枪手.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/重机枪手_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();



//
action_begin("向左提枪跑",false);

	anim("[向左提枪跑]"		,2		);

action_end();

action_begin("左跑急停转身",false);

	anim("[左跑急停转身]"		,2		);

action_end();

action_begin("向右提枪跑",false);

	anim("[向右提枪跑]"		,2		);

action_end();

action_begin("右跑急停转身",false);

	anim("[右跑急停转身]"		,2		);

action_end();

action_begin("蹲着瞄准_循环",true);

	anim("[左转身瞄准]"		,2		);

action_end();

action_begin("跳起",false);

	anim("[跳起]"		,2		);

action_end();

action_begin("跳在空中_循环",true);

	anim("[跳在空中]"		,2		);

action_end();

action_begin("落地",false);

	anim("[落地]"		,2		);

action_end();

action_begin("站着瞄准_循环",true);

	anim("[跳起瞄准]"		,2		);

action_end();

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-13 新添动作
action_begin("空中死亡",false);
	
	anim("[空中死亡]"	,4		);

action_end();

action_begin("空中死亡下落_循环",true);
	
	anim("[空中死亡下落]"	,4		);

action_end(); 

action_begin("空中死亡落地",false);
	
	anim("[空中死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//11-20
action_begin("向左提枪跑_循环",true);

	anim("[向左提枪跑]"		,2		);

action_end();

action_begin("向右提枪跑_循环",true);

	anim("[向右提枪跑]"		,2		);

action_end();

//11-22
action_begin("胸部中弹撞墙死亡",false);
	
	anim("[胸部中弹撞墙死亡]"	,2		);

action_end();

//12-21 更新
action_begin("高处死亡2",false);
	
	anim("[高处死亡2]"	,4		);

action_end();

action_begin("高处死亡下落2_循环",true);
	
	anim("[高处死亡下落2]"	,4		);

action_end(); 

action_begin("高处死亡落地2",false);
	
	anim("[高处死亡落地2]"	,4		);

action_end();file_end();
file_begin("data/model/生化兵.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/生化兵_半硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("站立_循环",true);
	
	anim("[预备姿势]"	,2		);

action_end(); 

action_begin("出场动作",false);
	
	anim("[出场动作]"	,2		);

action_end(); 

action_begin("走_循环",true);
	
	anim("[走]"	,2		);

action_end(); 

action_begin("前砍",false);
	
	anim("[前砍]"	,2		);

action_end(); 

action_begin("前砍_循环",true);
	
	anim("[前砍]"	,2		);

action_end(); 

action_begin("投掷",false);
	
	anim("[投掷]"	,2		);

action_end(); 

action_begin("投掷_循环",true);
	
	anim("[投掷]"	,2		);

action_end(); 
file_end();
file_begin("data/model/有轨电车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");
	model("characters/有轨电车.mod");
	gfx("","");
state_end();
state_begin("");
	model("");
	gfx("击毁_车辆.gfx","");
state_end();
convert_begin("寻常","");
	gfx("爆炸光影_大.gfx","");
convert_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/上海老头.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/上海老头_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("走_循环",true);
	
	anim("[走]"	,2		);

action_end(); 

action_begin("转身",false);
	
	anim("[转身]"	,2		);

action_end(); 

action_begin("惊惶逃跑_循环",true);
	
	anim("[惊慌逃跑]"	,2		);

action_end(); 

action_begin("抱头逃跑_循环",true);
	
	anim("[抱头逃跑]"	,2		);

action_end(); 

action_begin("由蹲到站",false);
	
	anim("[由蹲到站]"	,2		);

action_end(); 

action_begin("站起呼救_循环",true);
	
	anim("[站起呼救]"	,2		);

action_end(); 

action_begin("由站到蹲",false);
	
	anim("[由站到蹲]"	,2		);

action_end(); 

action_begin("跌坐在地",false);
	
	anim("[跌坐在地]"	,2		);

action_end(); 

action_begin("坐地后退_循环",true);
	
	anim("[后退]"	,2		);

action_end(); 

action_begin("坐地求饶_循环",true);
	
	anim("[坐地求饶]"	,2		);

action_end(); 

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//12-21 新增
action_begin("获救感激",false);
	
	anim("[获救感激]"	,2		);

action_end();

action_begin("普通跑_循环",true);
	
	anim("[普通跑]"	,2		);

action_end();

file_end();
file_begin("data/model/忍者.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/忍者_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("向左跑发镖",false);
	
	anim("[向左跑发镖]"	,2		);

action_end(); 

action_begin("向右跑发镖",false);
	
	anim("[向右跑发镖]"	,2		);

action_end(); 

action_begin("向左跑发镖_循环",true);
	
	anim("[向左跑发镖]"	,2		);

action_end(); 

action_begin("向右跑发镖_循环",true);
	
	anim("[向右跑发镖]"	,2		);

action_end(); 

action_begin("向前跑_循环",true);
	
	anim("[向前跑]"	,2		);

action_end(); 

action_begin("向前跑挥刀",false);
	
	anim("[向前跑挥刀]"	,2		);

action_end(); 

action_begin("向前跑挥刀_循环",true);
	
	anim("[向前跑挥刀]"	,2		);

action_end(); 

action_begin("站立跳起",false);
	
	anim("[站立跳起]"	,2		);

action_end(); 

action_begin("站立跳在空中_循环",true);
	
	anim("[站立跳在空中]"	,2		);

action_end(); 

action_begin("站立跳起后落地下蹲",false);
	
	anim("[落地]"	,2		);

action_end(); 

action_begin("保持下蹲_循环",true);
	
	anim("[落地准备姿势]"	,2		);

action_end(); 

action_begin("下蹲挥刀攻击",false);
	
	anim("[下蹲挥刀攻击]"	,2		);

action_end(); 

action_begin("下蹲挥刀攻击_循环",true);
	
	anim("[下蹲挥刀攻击]"	,2		);

action_end(); 

action_begin("下蹲跳起后退",false);
	
	anim("[下蹲跳起后退]"	,2		);

action_end(); 

action_begin("下蹲跳起后退在空中_循环",true);
	
	anim("[下蹲跳起后退空中]"	,2		);

action_end(); 

action_begin("下蹲跳起后退落地",false);
	
	anim("[下蹲跳起后退落地]"	,2		);

action_end(); 

action_begin("后空翻",false);
	
	anim("[后空翻]"	,2		);

action_end(); 

action_begin("后空翻_循环",true);
	
	anim("[后空翻]"	,2		);

action_end(); 


//11/08 14:25 更新
action_begin("左跑_循环",true);
	
	anim("[左跑]"	,2		);

action_end(); 

action_begin("右跑_循环",true);
	
	anim("[右跑]"	,2		);

action_end(); 


//11-13 新添动作
action_begin("空中死亡",false);
	
	anim("[空中死亡]"	,4		);

action_end();

action_begin("空中死亡下落_循环",true);
	
	anim("[空中死亡下落]"	,4		);

action_end(); 

action_begin("空中死亡落地",false);
	
	anim("[空中死亡落地]"	,4		);

action_end();

//12-17 新添动作
action_begin("站立后空翻",false);
	
	anim("[站立后空翻]"	,4		);

action_end();

action_begin("下蹲后空翻",false);
	
	anim("[下蹲后空翻]"	,4		);

action_end();

action_begin("前空翻",false);
	
	anim("[前空翻]"	,4		);

action_end();
file_end();
file_begin("data/model/主人公.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/主人公_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("喘气_循环",true);
	
	anim("[喘气]"	,2		);

action_end(); 

action_begin("跑_循环",true);
	
	anim("[向前跑]"	,2		);

action_end(); 

action_begin("立定举枪",false);
	
	anim("[立定举枪]"	,2		);

action_end(); 

action_begin("举枪立定_循环",true);
	
	anim("[立定举枪姿势]"	,2		);

action_end(); 

action_begin("前走拉枪栓",false);
	
	anim("[前走拉枪栓]"	,2		);

action_end(); 

action_begin("走_循环",true);
	
	anim("[提枪向前走]"	,2		);

action_end(); 

//12-21更新
action_begin("开枪后转身走",false);
	
	anim("[开枪后转身走]"	,2		);

action_end(); 


action_begin("拉枪栓立定",false);
	
	anim("[拉枪栓立定]"	,2		);

action_end(); 

action_begin("主角开枪",false);
	
	anim("[主角开枪]"	,2		);

action_end(); 

action_begin("主角开枪_循环",true);
	
	anim("[主角开枪]"	,2		);

action_end(); 

action_begin("跳跃起跳",false);
	
	anim("[跳跃]"	,2		);

action_end();

action_begin("跳跃在空中_循环",true);
	
	anim("[跳在空中]"	,2		);

action_end();

action_begin("跳跃落地",false);
	
	anim("[落地]"	,2		);

action_end();

//01-06 更新
action_begin("开枪动作",false);
	
	anim("[开枪动作]"	,2		);

action_end(); 

action_begin("开枪动作_循环",true);
	
	anim("[开枪动作]"	,2		);

action_end(); 

//01-25更新
action_begin("普通站立",false);
	
	anim("[站立]"	,2		);

action_end(); 

file_end();
file_begin("data/model/司令.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/司令_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("坐着_循环",true);
	
	anim("[坐着]"	,2		);

action_end(); 

action_begin("坐着讲话_循环",true);
	
	anim("[坐着讲话]"	,2		);

action_end(); 

action_begin("坐着挥手站起转身",false);
	
	anim("[挥手转身]"	,2		);

action_end(); 

action_begin("走_循环",true);
	
	anim("[走]"	,2		);

action_end(); 

action_begin("后退",false);
	
	anim("[后退]"	,2		);

action_end(); 

action_begin("拔枪",false);
	
	anim("[拔枪]"	,2		);

action_end(); 

action_begin("瞄准_循环",true);
	
	anim("[瞄准]"	,2		);

action_end(); 

action_begin("开火",false);
	
	anim("[开火]"	,2		);

action_end(); 

action_begin("得手狂笑",false);
	
	anim("[得手狂笑]"	,2		);

action_end(); 
file_end();
file_begin("data/model/日本敢死队员.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/日本敢死队员_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("左跳起",false);
	
	anim("[左跳]"	,2		);

action_end(); 

action_begin("左跳在空中_循环",true);
	
	anim("[左跳在空中]"	,2		);

action_end(); 

action_begin("左跳落地",false);
	
	anim("[左跳落地]"	,2		);

action_end(); 

action_begin("左跳落地姿势_循环",true);
	
	anim("[左跳落地姿势]"	,2		);

action_end(); 

action_begin("跑_循环",true);
	
	anim("[跑]"	,2		);

action_end(); 

action_begin("下蹲起跳",false);
	
	anim("[下蹲起跳]"	,2		);

action_end(); 

action_begin("下蹲起跳在空中_循环",true);
	
	anim("[下蹲起跳空中]"	,2		);

action_end(); 

action_begin("下蹲起跳落地",false);
	
	anim("[下蹲起跳落地]"	,2		);

action_end(); 

action_begin("投弹",false);
	
	anim("[投弹]"	,2		);

action_end(); 

action_begin("投弹_循环",true);
	
	anim("[投弹]"	,2		);

action_end(); 

action_begin("由站到蹲",false);
	
	anim("[下蹲]"	,2		);

action_end(); 

action_begin("右跳起",false);
	
	anim("[右跳]"	,2		);

action_end(); 

action_begin("右跳在空中_循环",true);
	
	anim("[右跳在空中]"	,2		);

action_end(); 

action_begin("右跳落地",false);
	
	anim("[右跳落地]"	,2		);

action_end(); 

action_begin("右跳落地姿势_循环",true);
	
	anim("[右跳落地姿势]"	,2		);

action_end(); 

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-13 新添动作
action_begin("空中死亡",false);
	
	anim("[空中死亡]"	,4		);

action_end();

action_begin("空中死亡下落_循环",true);
	
	anim("[空中死亡下落]"	,4		);

action_end(); 

action_begin("空中死亡落地",false);
	
	anim("[空中死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲姿势]"	,2		);

action_end();

//12-21 更新
action_begin("高处死亡2",false);
	
	anim("[高处死亡2]"	,4		);

action_end();

action_begin("高处死亡下落2_循环",true);
	
	anim("[高处死亡下落2]"	,4		);

action_end(); 

action_begin("高处死亡落地2",false);
	
	anim("[高处死亡落地2]"	,4		);

action_end();


//01-04
action_begin("炸死",false);
	
	anim("[炸死4]"	,2		);

action_end();
file_end();
file_begin("data/model/公共汽车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Bus character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	gfx("","");
	model("characters/战上海_公共汽车.mod");

state_end();

state_begin("");

	model("");
	gfx("击毁_车辆.gfx","");
state_end();

convert_begin("寻常","");

	gfx("爆炸光影_大.gfx","");
convert_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/洗澡鬼子.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/洗澡鬼子_硬动画01.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("转身",false);
	
	anim("[转身]"	,2		);

action_end(); 

action_begin("转身奔跑_循环",true);
	
	anim("[转身奔跑]"	,2		);

action_end(); 

action_begin("转身呼叫",false);
	
	anim("[转身呼叫]"	,2		);

action_end(); 

action_begin("转身呼叫逃跑_循环",true);
	
	anim("[转身呼叫逃跑]"	,2		);

action_end(); 

action_begin("跌坐在地",false);
	
	anim("[跌坐在地]"	,2		);

action_end(); 

action_begin("坐地后退_循环",true);
	
	anim("[后退]"	,2		);

action_end(); 

action_begin("坐地求饶_循环",true);
	
	anim("[坐地求饶]"	,2		);

action_end(); 
file_end();
file_begin("data/model/标记符.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("denote/历史学家.mod");

state_end();
file_end();
file_begin("data/model/老式汽车02.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	gfx("","");
	model("characters/战上海_老式汽车02.mod");

state_end();

state_begin("击毁");

	gfx("击毁_车辆.gfx","");
	model("");

state_end();
convert_begin("寻常","击毁");

	gfx("爆炸光影_大.gfx","");
convert_end();



//-------------------------------------------------------------
define_HH("HH_右边成员",0);
define_HH("HH_左边成员",1);
define_HH("HH_后座",2);
//-------------------------------------------------------------

action_begin("开车_循环",true);
	
	anim("[开车]"	,4		);

action_end(); 

action_begin("开火_循环",true,10.0);
	
	anim("[开火]"	,4		);

action_end(); 


action_begin("开火",false);
	
	anim("[开火]"	,4		);

action_end(); 

action_begin("停车",false);
	
	anim("[停车]"	,4		);

action_end(); 
file_end();
file_begin("data/model/上海女职员.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/上海女职员_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("走_循环",true);
	
	anim("[走]"	,2		);

action_end(); 

action_begin("转身",false);
	
	anim("[转身]"	,2		);

action_end(); 

action_begin("惊惶逃跑_循环",true);
	
	anim("[惊慌逃跑]"	,2		);

action_end(); 

action_begin("抱头逃跑_循环",true);
	
	anim("[抱头逃跑]"	,2		);

action_end(); 

action_begin("由蹲到站",false);
	
	anim("[由蹲到站]"	,2		);

action_end(); 

action_begin("站起呼救_循环",true);
	
	anim("[站起呼救]"	,2		);

action_end(); 

action_begin("由站到蹲",false);
	
	anim("[由站到蹲]"	,2		);

action_end(); 

action_begin("跌坐在地",false);
	
	anim("[跌坐在地]"	,2		);

action_end(); 

action_begin("坐地后退_循环",true);
	
	anim("[后退]"	,2		);

action_end(); 

action_begin("坐地求饶_循环",true);
	
	anim("[坐地求饶]"	,2		);

action_end(); 

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//12-21 新增
action_begin("获救感激",false);
	
	anim("[获救感激]"	,2		);

action_end();

action_begin("普通跑_循环",true);
	
	anim("[普通跑]"	,2		);

action_end();

//01-25 新增
action_begin("普通站起",false);
	
	anim("[普通站起]"	,2		);

action_end();

file_end();
file_begin("data/model/陆军.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/日本陆军_硬动画01.mod");

state_end();

state_begin("");

	model("");

state_end();




//++++++新增+++++
action_begin("蹲着移动_循环",true);
	
	anim("[下蹲移动]"	,2		);

action_end(); 

action_begin("向右提枪跑_循环",true);
	
	anim("[向右提枪跑]"	,2		);

action_end();

action_begin("向左提枪跑_循环",true);
	
	anim("[向左提枪跑]"	,2		);

action_end();

action_begin("追逐_循环",true);

	anim("[追逐]"		,2		);

action_end();

action_begin("由蹲到站瞄准_循环",true);

	anim("[由蹲到站瞄准]"		,2		);

action_end();

action_begin("站立端枪",false);

	anim("[站立端枪]"		,2		);

action_end();

action_begin("左转身开火_循环",true); //需加延时

	anim("[左转身开火]"		,2		);

action_end();

action_begin("右转身开火_循环",true); //需加延时

	anim("[右转身开火]"		,2		);

action_end();

action_begin("右翻滚_循环",true); //需加延时

	anim("[右滚翻]"		,4		);

action_end();

action_begin("左翻滚_循环",true); //需加延时

	anim("[左滚翻]"		,4		);

action_end();

action_begin("持续右侧滚翻下蹲瞄准射击_循环",true);

	anim("[右滚翻]"		,4		);
	anim("[右滚翻瞄准]"		,2		);
	anim("[右滚翻开火]"		,2		);
	anim("[右滚翻拉栓]"		,2		);

action_end();

action_begin("持续左侧滚翻下蹲瞄准射击_循环",true);

	anim("[左滚翻]"		,4			);
	anim("[左滚翻瞄准]"		,2		);
	anim("[左滚翻开火]"		,2		);
	anim("[左滚翻拉枪栓]"		,2		);

action_end();


//+++++++++

//-------------------------------------------------------------
action_begin("端枪缓慢巡逻_循环",true);
	
	anim("[巡逻走]"	,2		);

action_end(); 

action_begin("从车上跳下",false);

	anim("[跳起]"			,2		);
	anim("[跳在空中]"		,2		);
	anim("[落地]"			,2		);

action_end(); 

action_begin("走_循环",true);
	
	anim("[走]"	,2		);

action_end(); 

action_begin("跑_循环",true);
	
	anim("[端枪前跑]"	,2		);

action_end(); 

action_begin("提枪慢跑左转",false);

	anim("[右跑急停转身]"	,2		);

action_end(); 

action_begin("提枪慢跑右转",false);

	anim("[左跑急停转身]"	,2		);

action_end();

action_begin("端枪快跑",false);

	anim("[端枪前跑]"		,2		);

action_end();

action_begin("由蹲到站",false);

	anim("[由蹲到站]"		,2		);

action_end();

////


action_begin("由蹲到站开火",false);

	anim("[由蹲到站开火]"	,2		);

action_end();

action_begin("由蹲到站开火_循环",true);//需加延时

	anim("[由蹲到站开火]"	,2		);

action_end();

action_begin("由站到蹲",false);

	anim("[下蹲抬枪]"		,2		);

action_end();

action_begin("由蹲到站_循环",true);

	anim("[由蹲到站]"		,2		);
	anim("[由蹲到站瞄准]"	,2		);
	anim("[由蹲到站开火]"	,2		);
	anim("[由站到蹲]"		,2		);

action_end();

/////

action_begin("站立瞄准射击拉枪栓",false);//需加延时

	anim("[站立端枪]"		,2		);
	anim("[站立端枪瞄准]"	,2		);
	anim("[站立端枪开火]"	,2		);

action_end();

action_begin("站立瞄准射击拉枪栓_循环",true);//需加延时

	anim("[站立端枪瞄准]"	,2		);
	anim("[站立端枪开火]"	,2		);
	anim("[站立端枪拉枪栓]"	,2		);

action_end();

action_begin("左转身端枪瞄准射击",false);//需加延时

	anim("[左转身抬枪]"		,2		);
	anim("[左转身瞄准]"		,2		);
	anim("[左转身开火]"		,2		);

action_end();

action_begin("左转身端枪瞄准射击闪身躲避_循环",true);

	anim("[左转身抬枪]"		,2		);
	anim("[左转身瞄准]"		,2		);
	anim("[左转身开火]"		,2		);
	anim("[向右闪避]"		,2		);

action_end();

/////////

action_begin("右转身端枪瞄准射击",false);//需加延时

	anim("[右转身抬枪]"		,2		);
	anim("[右转身瞄准]"		,2		);
	anim("[右转身开火]"		,2		);

action_end();

action_begin("右转身端枪瞄准射击闪身躲避_循环",true);

	anim("[右转身抬枪]"		,2		);
	anim("[右转身瞄准]"		,2		);
	anim("[右转身开火]"		,2		);
	anim("[向左闪避]"		,2		);

action_end();

action_begin("下蹲瞄准射击拉枪栓",false);

	anim("[下蹲抬枪]"		,2		);
	anim("[下蹲瞄准]"		,2		);
	anim("[下蹲开火]"		,2		);

action_end();

action_begin("下蹲瞄准射击拉枪栓_循环",true);//需加延时

	anim("[下蹲瞄准]"		,2		);
	anim("[下蹲开火]"		,2		);
	anim("[下蹲拉枪栓]"		,2		);

action_end();

//////

action_begin("右侧滚翻下蹲瞄准射击",false);//需加延时

	anim("[右滚翻瞄准]"		,2		);
	anim("[右滚翻开火]"		,2		);

action_end();

action_begin("左侧滚翻下蹲瞄准射击",false);

	anim("[左滚翻瞄准]"		,2		);
	anim("[左滚翻开火]"		,2		);

action_end();

action_begin("右侧滚翻下蹲瞄准射击_循环",true,0.5);//需加延时

	anim("[右滚翻瞄准]"		,2		);
	anim("[右滚翻开火]"		,2		);
	anim("[右滚翻拉栓]"		,2		);

action_end();

action_begin("左侧滚翻下蹲瞄准射击_循环",true,0.5);//需加延时

	anim("[左滚翻瞄准]"		,2		);
	anim("[左滚翻开火]"		,2		);
	anim("[左滚翻拉枪栓]"		,2		);

action_end();

/////
action_begin("猫腰端枪移动_循环",true);

	anim("[下蹲移动]"		,2		);

action_end();


action_begin("准备追逐",false);

	anim("[准备追逐]"		,2		);

action_end();

action_begin("追逐",false);

	anim("[追逐]"		,2		);

action_end();

action_begin("跳起在空中",false);

	anim("[跳起]"			,2		);
	anim("[跳在空中]"		,2		);

action_end(); 

action_begin("跳起落地",false);

	anim("[落地]"			,2		);

action_end(); 

action_begin("右翻滚",false); 

	anim("[右滚翻]"		,4		);

action_end();

action_begin("左翻滚",false); 

	anim("[左滚翻]"		,4		);

action_end();

action_begin("左转身抬枪",false);

	anim("[左转身抬枪]"		,2		);

action_end();

action_begin("左转身瞄准",false);

	anim("[左转身瞄准]"		,2		);

action_end();

action_begin("左转身开火",false);

	anim("[左转身开火]"		,2		);

action_end();

action_begin("右转身抬枪",false);

	anim("[右转身抬枪]"		,2		);

action_end();

action_begin("右转身瞄准",false);

	anim("[左转身瞄准]"		,2		);

action_end();

action_begin("右转身开火",false);

	anim("[左转身开火]"		,2		);

action_end();

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11/08 15:44 新添动作
action_begin("巡逻转身",false);
	
	anim("[巡逻转身]"	,2		);

action_end();


//11-13 新添动作
action_begin("空中死亡",false);
	
	anim("[空中死亡]"	,4		);

action_end();

action_begin("空中死亡下落_循环",true);
	
	anim("[空中死亡下落]"	,4		);

action_end(); 

action_begin("空中死亡落地",false);
	
	anim("[空中死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//11-22
action_begin("胸部中弹撞墙死亡",false);
	
	anim("[胸部中弹撞墙死亡]"	,2		);

action_end();

//12-21 更新
action_begin("向右平移",false);
	
	anim("[向右平移射击]"	,2		);

action_end();

action_begin("向右平移_循环",true);
	
	anim("[向右平移射击]"	,2		);

action_end();

action_begin("向左平移",false);
	
	anim("[向左平移射击]"	,2		);

action_end();

action_begin("向左平移_循环",true);
	
	anim("[向左平移射击]"	,2		);

action_end();

action_begin("左转身",false);
	
	anim("[左转身]"	,2		);

action_end();

action_begin("右转身",false);
	
	anim("[右转身]"	,2		);

action_end();

action_begin("高处死亡2",false);
	
	anim("[高处死亡2]"	,4		);

action_end();

action_begin("高处死亡下落2_循环",true);
	
	anim("[高处死亡下落2]"	,4		);

action_end(); 

action_begin("高处死亡落地2",false);
	
	anim("[高处死亡落地2]"	,4		);

action_end();

action_begin("下蹲移动开火",false);
	
	anim("[下蹲移动开火]"	,4		);

action_end();

action_begin("下蹲移动开火_循环",true);
	
	anim("[下蹲移动开火]"	,4		);

action_end();

action_begin("普通跑_循环",true);
	
	anim("[普通跑]"	,4		);

action_end();file_end();
file_begin("data/model/军曹_指挥刀.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/军曹_指挥刀_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();




//
action_begin("提刀向左跑_循环",true);
	
	anim("[提刀向右跑]"	,2		);

action_end(); 

action_begin("左跑挥刀",false);
	
	anim("[左跑挥刀]"	,2		);

action_end(); 

action_begin("提刀向右跑_循环",true);
	
	anim("[提刀向左跑]"	,2		);

action_end(); 

action_begin("右跑挥刀",false);
	
	anim("[右跑挥刀]"	,2		);

action_end(); 

action_begin("标准挥刀",false);
	
	anim("[标准挥刀]"	,2		);

action_end(); 

action_begin("标准站立挥刀指挥_循环",true);
	
	anim("[标准站立挥刀指挥]"	,2		);

action_end(); 

action_begin("持刀站立_循环",true);
	
	anim("[持刀站立]"	,2		);

action_end(); 

action_begin("由蹲到站挥刀",false);
	
	anim("[由蹲到站]"	,2		);

action_end(); 

action_begin("由蹲到站挥刀指挥_循环",true);
	
	anim("[由蹲到站挥刀指挥]"	,2		);

action_end(); 

action_begin("由站到蹲",false);
	
	anim("[由站到蹲]"	,2		);

action_end(); 

action_begin("左转身挥刀",false);
	
	anim("[左转身挥刀]"	,2		);

action_end();

action_begin("左转身挥刀指挥_循环",true);
	
	anim("[左转身挥刀指挥]"	,2		);

action_end(); 

action_begin("向右闪避",false);
	
	anim("[向右闪避]"	,2		);

action_end();

action_begin("右转身挥刀",false);
	
	anim("[右转身挥刀]"	,2		);

action_end();

action_begin("右转身挥刀指挥_循环",true);
	
	anim("[右转身挥刀指挥]"	,2		);

action_end(); 

action_begin("向左闪避",false);
	
	anim("[向左闪避]"	,2		);

action_end();

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();


//11/08 14:25 更新
action_begin("收刀",false);
	
	anim("[收刀]"	,2		);

action_end();
 
action_begin("挥刀指挥收刀_循环",true);
	
	anim("[标准挥刀]"	,2		);
	anim("[收刀]"	,2		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();
 
//11-22
action_begin("胸部中弹撞墙死亡",false);
	
	anim("[胸部中弹撞墙死亡]"	,2		);

action_end();

//12-21更新
action_begin("正面跑_循环",true);
	
	anim("[正面跑]"	,2		);

action_end();

action_begin("高处死亡2",false);
	
	anim("[高处死亡2]"	,4		);

action_end();

action_begin("高处死亡下落2_循环",true);
	
	anim("[高处死亡下落2]"	,4		);

action_end(); 

action_begin("高处死亡落地2",false);
	
	anim("[高处死亡落地2]"	,4		);

action_end();

action_begin("空中死亡",false);
	
	anim("[空中死亡]"	,4		);

action_end();

action_begin("空中死亡下落_循环",true);
	
	anim("[空中死亡下落]"	,4		);

action_end(); 

action_begin("空中死亡落地",false);
	
	anim("[空中死亡落地]"	,4		);

action_end();

file_end();


file_begin("data/model/军曹_手枪.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/军曹_手枪_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();




//
action_begin("左转身抬枪",false);
	
	anim("[左转身抬枪]"	,2		);

action_end(); 

action_begin("左转身瞄准_循环",true);
	
	anim("[左转身瞄准]"	,2		);

action_end(); 

action_begin("左转身开火_循环",true); //需加延时
	
	anim("[左转身开火]"	,2		);

action_end(); 

action_begin("左转身抬枪开火躲避_循环",true); //需加延时

	anim("[左转身抬枪]"	,2		);	
	anim("[左转身瞄准]"	,2		);	
	anim("[左转身开火]"	,2		);
	anim("[向右闪避]"	,2		);	

action_end(); 

action_begin("右转身抬枪",false);
	
	anim("[右转身抬枪]"	,2		);

action_end(); 

action_begin("右转身瞄准_循环",true);
	
	anim("[右转身瞄准]"	,2		);

action_end(); 

action_begin("右转身开火_循环",true); //需加延时
	
	anim("[右转身开火]"	,2		);

action_end(); 

action_begin("右转身抬枪开火躲避_循环",true); //需加延时

	anim("[右转身抬枪]"	,2		);	
	anim("[右转身瞄准]"	,2		);	
	anim("[右转身开火]"	,2		);
	anim("[向左闪避]"	,2		);	

action_end(); 
file_end();
file_begin("data/model/消防车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	gfx("","");
	model("characters/战上海_消防车.mod");

state_end();

state_begin("");

	gfx("击毁_车辆.gfx","");
	model("");

state_end();
convert_begin("寻常","");

	gfx("爆炸光影_大.gfx","");
convert_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/上海女平民.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/上海女平民_半硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("走_循环",true);
	
	anim("[走]"	,2		);

action_end(); 

action_begin("转身",false);
	
	anim("[转身]"	,2		);

action_end(); 

action_begin("惊惶逃跑_循环",true);
	
	anim("[惊慌逃跑]"	,2		);

action_end(); 

action_begin("抱头逃跑_循环",true);
	
	anim("[抱头逃跑]"	,2		);

action_end(); 

action_begin("由蹲到站",false);
	
	anim("[由蹲到站]"	,2		);

action_end(); 

action_begin("站起呼救_循环",true);
	
	anim("[站起呼救]"	,2		);

action_end(); 

action_begin("由站到蹲",false);
	
	anim("[由站到蹲]"	,2		);

action_end(); 

action_begin("跌坐在地",false);
	
	anim("[跌坐在地]"	,2		);

action_end(); 

action_begin("坐地后退_循环",true);
	
	anim("[后退]"	,2		);

action_end(); 

action_begin("坐地求饶_循环",true);
	
	anim("[坐地求饶]"	,2		);

action_end(); 

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//12-21 新增
action_begin("获救感激",false);
	
	anim("[获救感激]"	,2		);

action_end();

action_begin("普通跑_循环",true);
	
	anim("[普通跑]"	,2		);

action_end();

//01-25 新增
action_begin("普通站起",false);
	
	anim("[普通站起]"	,2		);

action_end();

file_end();
file_begin("data/model/男平民.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/男平民_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("走_循环",true);
	
	anim("[走]"	,2		);

action_end(); 

action_begin("转身",false);
	
	anim("[转身]"	,2		);

action_end(); 

action_begin("惊惶逃跑_循环",true);
	
	anim("[惊慌逃跑]"	,2		);

action_end(); 

action_begin("抱头逃跑_循环",true);
	
	anim("[抱头逃跑]"	,2		);

action_end(); 

action_begin("由蹲到站",false);
	
	anim("[由蹲到站]"	,2		);

action_end(); 

action_begin("站起呼救_循环",true);
	
	anim("[站起呼救]"	,2		);

action_end(); 

action_begin("由站到蹲",false);
	
	anim("[由站到蹲]"	,2		);

action_end(); 

action_begin("跌坐在地",false);
	
	anim("[跌坐在地]"	,2		);

action_end(); 

action_begin("坐地后退_循环",true);
	
	anim("[后退]"	,2		);

action_end(); 

action_begin("坐地求饶_循环",true);
	
	anim("[坐地求饶]"	,2		);

action_end(); 

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//12-21 新增
action_begin("获救感激",false);
	
	anim("[获救感激]"	,2		);

action_end();

action_begin("普通跑_循环",true);
	
	anim("[普通跑]"	,2		);

action_end();file_end();
file_begin("data/model/上海青年男职员.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/上海青年男职员_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("走_循环",true);
	
	anim("[走]"	,2		);

action_end(); 

action_begin("转身",false);
	
	anim("[转身]"	,2		);

action_end(); 

action_begin("惊惶逃跑_循环",true);
	
	anim("[惊慌逃跑]"	,2		);

action_end(); 

action_begin("抱头逃跑_循环",true);
	
	anim("[抱头逃跑]"	,2		);

action_end(); 

action_begin("由蹲到站",false);
	
	anim("[由蹲到站]"	,2		);

action_end(); 

action_begin("站起呼救_循环",true);
	
	anim("[站起呼救]"	,2		);

action_end(); 

action_begin("由站到蹲",false);
	
	anim("[由站到蹲]"	,2		);

action_end(); 

action_begin("跌坐在地",false);
	
	anim("[跌坐在地]"	,2		);

action_end(); 

action_begin("坐地后退_循环",true);
	
	anim("[后退]"	,2		);

action_end(); 

action_begin("坐地求饶_循环",true);
	
	anim("[坐地求饶]"	,2		);

action_end(); 

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//12-21 新增
action_begin("获救感激",false);
	
	anim("[获救感激]"	,2		);

action_end();

action_begin("普通跑_循环",true);
	
	anim("[普通跑]"	,2		);

action_end();file_end();
file_begin("data/model/日本摩托车机枪兵.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/日军摩托车机枪兵_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("坐_循环",true);
	
	anim("[坐着]"	,2		);

action_end(); 

action_begin("向前开枪_循环",true);
	
	anim("[向前开枪]"	,2		);

action_end(); 

action_begin("向前开枪",false);
	
	anim("[向前开枪]"	,2		);

action_end(); 

action_begin("向后转",false);
	
	anim("[向后转]"	,2		);

action_end(); 

action_begin("向后瞄准",false);
	
	anim("[向后瞄准]"	,2		);

action_end(); 

action_begin("向后开枪_循环",true);
	
	anim("[向后开枪]"	,2		);

action_end(); 

action_begin("向后开枪",false);
	
	anim("[向后开枪]"	,2		);

action_end(); 

action_begin("向前转",false);
	
	anim("[向前转]"	,2		);

action_end(); 






file_end();
file_begin("data/model/日本摩托车驾驶员.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/日军摩托车驾驶员_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("开车_循环",true);
	
	anim("[开车姿势]"	,2		);

action_end(); 

action_begin("死亡",false);
	
	anim("[右边死]"	,2		);

action_end(); 
file_end();
file_begin("data/model/黄包车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/黄包车.mod");

state_end();


//-------------------------------------------------------------
file_end();
file_begin("data/model/九二式重机枪_站立式.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/九二式重机枪_站立式.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("开火_循环",true, 3.0);
	
	anim("[开火]"	,4		);

action_end(); 

action_begin("开火",false);
	
	anim("[开火]"	,4		);

action_end(); 

action_begin("停止",false);
	
	anim("[停止]"	,4		);

action_end(); 
file_end();
file_begin("data/model/交通岗亭.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/交通岗亭.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/九二式重机枪.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/九二式重机枪.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("开火_循环",true, 3.0);
	
	anim("[开火]"	,4		);

action_end(); 

action_begin("开火",false);
	
	anim("[开火]"	,4		);

action_end(); 

action_begin("停止",false);
	
	anim("[停止]"	,4		);

action_end(); 
file_end();
file_begin("data/model/虚拟物体.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/发射虚拟体.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/手推车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/手推车.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/主角用子弹夹3.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主角用子弹夹3.mod","旋转");

state_end();

state_begin("");

	model("");

state_end();
action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/主角用子弹夹2.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主角用子弹夹2.mod","旋转");

state_end();

state_begin("");

	model("");

state_end();
action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/小火轮.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/小火轮.mod");

state_end();

//-------------------------------------------------------------

define_HH("HH_士兵1",0);
define_HH("HH_士兵2",1);
define_HH("HH_士兵3",2);
define_HH("HH_士兵4",3);
define_HH("HH_士兵5",4);
define_HH("HH_士兵6",5);
define_HH("HH_士兵7",6);
file_end();
file_begin("data/model/手榴弹3.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/手榴弹3.mod","旋转");

state_end();
/*
state_begin("");

	model("");

state_end();
*/
action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/手榴弹2.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/手榴弹2.mod","旋转");

state_end();
/*
state_begin("");

	model("");

state_end();
*/
action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/血包01.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/血包01.mod","旋转");

state_end();

action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/破车3.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	damaged vehicle - NO.3 character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/破车3.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/破车2.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	damaged vehicle - NO.2 character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/破车2.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/血包02.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/血包02.mod","旋转");

state_end();

action_begin("旋转",true);
	anim("[旋转]"	,4		);
action_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/军用运兵车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	gfx("","");
	model("characters/军用运兵车.mod");

state_end();

convert_begin("寻常","击毁");

	gfx("爆炸光影_大.gfx","");
convert_end();

state_begin("击毁");
	gfx("击毁_车辆.gfx","");
	model("characters/军用运兵车击毁.mod");

state_end();

//-------------------------------------------------------------

define_HH("HH_军用运输车爆炸",0);
define_HH("HH_士兵05",1);
define_HH("HH_士兵04",2);
define_HH("HH_士兵01",3);
define_HH("HH_士兵03",4);
define_HH("HH_士兵02",5);
define_HH("HH_普通木箱01",6);
define_HH("HH_汽油桶",7);


//11-21
action_begin("开车_循环",true);
	
	anim("[开车]"	,2		);

action_end(); 

action_begin("停车",false);
	
	anim("[停车]"	,2		);

action_end(); 





file_end();
file_begin("data/model/浪人.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/浪人_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("举刀前跑_循环",true);
	
	anim("[举刀前跑]"	,2		);

action_end(); 

action_begin("左跑转身",false);

	anim("[左跑挥刀]"		,2		);

action_end();

action_begin("举刀",false);

	anim("[举刀]"		,2		);

action_end();

action_begin("前砍",false);

	anim("[前砍]"		,2		);

action_end();

action_begin("前砍_循环",true);//需加延时

	anim("[再砍]"		,2		);

action_end();

action_begin("高空跳下砍",false);

	anim("[跳起]"		,2		);
	anim("[跳在空中]"		,2		);
	anim("[跳下砍]"		,2		);

action_end();

action_begin("前翻滚_循环",true);
	
	anim("[前滚翻]"	,4		);

action_end(); 

action_begin("前刺",false);

	anim("[前刺]"		,2		);

action_end();

action_begin("前刺_循环",true);//需加循环

	anim("[前刺]"		,2		);

action_end();

action_begin("右转身挥刀",false);

	anim("[右转身挥刀]"		,2		);

action_end();

action_begin("右转身挥刀姿势",false);

	anim("[右转身挥刀姿势]"		,2		);

action_end();

action_begin("左转身挥刀",false);

	anim("[左转身挥刀]"		,2		);

action_end();

action_begin("左转身挥刀姿势",false);

	anim("[左转身挥刀姿势]"		,2		);

action_end();

action_begin("后退举刀",false);

	anim("[后退举刀]"		,2		);

action_end();

action_begin("后退循环",true);

	anim("[后退]"		,2		);

action_end();

action_begin("站立举刀",false);

	anim("[站立举刀]"		,2		);

action_end();

action_begin("再砍",false);

	anim("[再砍]"		,2		);

action_end();

action_begin("预备姿势",false);

	anim("[预备姿势]"		,2		);

action_end();

action_begin("左跑_循环",true);

	anim("[提刀向左跑]"		,2		);

action_end();

action_begin("高空跳下",false);

	anim("[跳起2]"		,2		);
	anim("[跳在空中2]"		,2		);
	anim("[落地]"		,2		);

action_end();

action_begin("跳起在空中",false);

	anim("[跳起2]"		,2		);
	anim("[跳在空中2]"		,2		);

action_end();

action_begin("跳起落地",false);

	anim("[落地]"		,2		);

action_end();

action_begin("前翻滚",false);
	
	anim("[前滚翻]"	,4		);

action_end(); 

action_begin("高空跳下起跳",false);

	anim("[跳起]"		,2		);

action_end();

action_begin("高空跳下在空中",true);

	anim("[跳在空中]"		,2		);

action_end();

action_begin("高空跳下跳下砍",false);

	anim("[跳下砍]"		,2		);

action_end();

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();

//11-13 新添动作
action_begin("空中死亡",false);
	
	anim("[空中死亡]"	,4		);

action_end();

action_begin("空中死亡下落_循环",true);
	
	anim("[空中死亡下落]"	,4		);

action_end(); 

action_begin("空中死亡落地",false);
	
	anim("[空中死亡落地]"	,4		);

action_end();

//11-13-2
action_begin("提刀左跑_循环",true);
	
	anim("[提刀向左跑]"	,4		);

action_end(); 

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//11-22
action_begin("胸部中弹撞墙死亡",false);
	
	anim("[胸部中弹撞墙死亡]"	,2		);

action_end();


//12-21 更新
action_begin("高处死亡2",false);
	
	anim("[高处死亡2]"	,4		);

action_end();

action_begin("高处死亡下落2_循环",true);
	
	anim("[高处死亡下落2]"	,4		);

action_end(); 

action_begin("高处死亡落地2",false);
	
	anim("[高处死亡落地2]"	,4		);

action_end();file_end();
file_begin("data/model/97式坦克.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	gfx("","");
	model("characters/97式坦克.mod");

state_end();

state_begin("");

	gfx("击毁_车辆.gfx","");
	model("characters/97式坦克击毁.mod");

state_end();
convert_begin("寻常","");

	gfx("爆炸光影_大.gfx","");
convert_end();

//
action_begin("开车_循环",true);
	
	anim("[开车]"	,4		);

action_end(); 

action_begin("开火_循环",true,10.0);
	
	anim("[开火]"	,4		);

action_end(); 


action_begin("开火",false);
	
	anim("[开火]"	,4		);

action_end(); 

action_begin("停车",false);
	
	anim("[停车]"	,4		);

action_end(); 

define_HH("HH_坦克虚拟体",2);

file_end();
file_begin("data/model/player1.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/t.mod");

state_end();

state_begin("");

	model("characters/t.mod");

state_end();


convert_begin("寻常","日军");

	gfx("手榴弹爆炸客观效果.gfx","");

convert_end();

//-------------------------------------------------------------

action_begin("开火",false);
	//	  动画名称	优先级	
	//anim("[站到跳]"	,1		);
	//anim("[跳到站]"	,2		);
	//anim("[上受袭]"	,2		);
action_end(); 

action_begin("走",false);
	//	  动画名称	优先级	
	anim("[巡逻]"	,1		);
action_end();file_end();
file_begin("data/model/轻机枪手.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/轻机枪手硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();




//
action_begin("走_循环",true);
	
	anim("[走]"	,2		);

action_end(); 

action_begin("高空跳下",false);

	anim("[跳起]"			,2		);
	anim("[跳在空中]"		,2		);
	anim("[落地]"			,2		);

action_end(); 

action_begin("向右提枪跑_循环",true);
	
	anim("[向右提枪跑]"	,2		);

action_end();

action_begin("提枪慢跑左转",false);

	anim("[右跑急停转身]"	,2		);

action_end(); 

action_begin("向左提枪跑_循环",true);
	
	anim("[向左提枪跑]"	,2		);

action_end();

action_begin("提枪慢跑右转",false);

	anim("[左跑急停转身]"	,2		);

action_end();

action_begin("由蹲到站",false);

	anim("[由蹲到站]"		,2		);

action_end();

action_begin("由蹲到站瞄准_循环",true);

	anim("[由蹲到站瞄准]"		,2		);

action_end();

action_begin("由蹲到站开火",false);

	anim("[由蹲到站开火]"	,2		);

action_end();

action_begin("由蹲到站开火_循环",true);//需加延时

	anim("[由蹲到站开火]"	,2		);

action_end();

action_begin("由站到蹲",false);

	anim("[由站到蹲]"		,2		);

action_end();

action_begin("由蹲到站_循环",true);//需加延时

	anim("[由蹲到站]"		,2		);
	anim("[由蹲到站瞄准]"	,2		);
	anim("[由蹲到站开火]"	,2		);
	anim("[由站到蹲]"		,2		);

action_end();

action_begin("站立端枪",false);

	anim("[站立端枪]"		,2		);

action_end();

action_begin("站立瞄准射击",false);

	anim("[站立端枪]"		,2		);
	anim("[站立端枪瞄准]"	,2		);
	anim("[站立端枪开火]"	,2		);

action_end();

action_begin("站立瞄准射击_循环",true);//需加延时

	anim("[站立端枪瞄准]"	,2		);
	anim("[站立端枪开火]"	,2		);

action_end();

action_begin("左转身端枪瞄准射击",false);

	anim("[左转身抬枪]"		,2		);
	anim("[左转身瞄准]"		,2		);
	anim("[左转身开火]"		,2		);

action_end();

action_begin("左转身端枪瞄准射击闪身躲避_循环",true);//需加延时

	anim("[左转身抬枪]"		,2		);
	anim("[左转身瞄准]"		,2		);
	anim("[左转身开火]"		,2		);
	anim("[向右闪避]"		,2		);

action_end();

action_begin("左转身开火_循环",true); //需加延时

	anim("[左转身开火]"		,2		);

action_end();

action_begin("右转身端枪瞄准射击",false);

	anim("[右转身抬枪]"		,2		);
	anim("[右转身瞄准]"		,2		);
	anim("[右转身开火]"		,2		);

action_end();

action_begin("右转身端枪瞄准射击闪身躲避_循环",true);//需加延时

	anim("[右转身抬枪]"		,2		);
	anim("[右转身瞄准]"		,2		);
	anim("[右转身开火]"		,2		);
	anim("[向左闪避]"		,2		);

action_end();


action_begin("右转身开火_循环",true); //需加延时

	anim("[右转身开火]"		,2		);

action_end();

action_begin("跳起在空中",false);

	anim("[跳起]"			,2		);
	anim("[跳在空中]"		,2		);

action_end(); 

action_begin("跳起落地",false);

	anim("[落地]"			,2		);

action_end(); 

action_begin("左转身抬枪",false);

	anim("[左转身抬枪]"		,2		);

action_end();

action_begin("左转身瞄准",false);

	anim("[左转身瞄准]"		,2		);

action_end();

action_begin("左转身开火",false);

	anim("[左转身开火]"		,2		);

action_end();

action_begin("右转身抬枪",false);

	anim("[右转身抬枪]"		,2		);

action_end();

action_begin("右转身瞄准",false);

	anim("[左转身瞄准]"		,2		);

action_end();

action_begin("右转身开火",false);

	anim("[左转身开火]"		,2		);

action_end();

action_begin("高处死亡",false);
	
	anim("[高处死亡]"	,4		);

action_end();

action_begin("高处死亡下落_循环",true);
	
	anim("[高处死亡下落]"	,4		);

action_end(); 

action_begin("高处死亡落地",false);
	
	anim("[高处死亡落地]"	,4		);

action_end();


//11-13 新添动作
action_begin("空中死亡",false);
	
	anim("[空中死亡]"	,4		);

action_end();

action_begin("空中死亡下落_循环",true);
	
	anim("[空中死亡下落]"	,4		);

action_end(); 

action_begin("空中死亡落地",false);
	
	anim("[空中死亡落地]"	,4		);

action_end();

//11-18
action_begin("下蹲",false);
	
	anim("[下蹲]"	,2		);

action_end();

//11-22
action_begin("胸部中弹撞墙死亡",false);
	
	anim("[胸部中弹撞墙死亡]"	,2		);

action_end();

//12-21 更新
action_begin("跑_循环",true);
	
	anim("[端枪前跑]"	,2		);

action_end();

action_begin("向右平移",false);
	
	anim("[向右平移]"	,2		);

action_end();

action_begin("向右平移_循环",true);
	
	anim("[向右平移]"	,2		);

action_end();

action_begin("向左平移",false);
	
	anim("[向左平移]"	,2		);

action_end();

action_begin("向左平移_循环",true);
	
	anim("[向左平移]"	,2		);

action_end();

action_begin("左转身",false);
	
	anim("[普通左转身]"	,2		);

action_end();

action_begin("右转身",false);
	
	anim("[普通右转身]"	,2		);

action_end();

action_begin("高处死亡2",false);
	
	anim("[高处死亡2]"	,4		);

action_end();

action_begin("高处死亡下落2_循环",true);
	
	anim("[高处死亡下落2]"	,4		);

action_end(); 

action_begin("高处死亡落地2",false);
	
	anim("[高处死亡落地2]"	,4		);

action_end();



file_end();
file_begin("data/model/破车1.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	damaged vehicle - NO.1 character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/破车1.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/电话亭.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/电话亭.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/吊灯.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/吊灯.mod");

state_end();

state_begin("");

	model("matters/吊灯损坏.mod");

state_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/路灯01.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/路灯01.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/主观手枪.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主观手枪模型_单手.mod");

state_end();
define_HH("HH_退膛",0);

//-------------------------------------------------------------
file_end();
file_begin("data/model/广告牌01.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/广告牌01.mod");

state_end();
state_begin("");

	model("");

state_end();

convert_begin("寻常","");

	gfx("击毁_牌匾.gfx","");

convert_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/路灯02.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/路灯02.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/主观手榴弹.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主观手榴弹模型.mod");

state_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/伤害体.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	damaged vehicle - NO.1 character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	
state_begin("寻常");

	model("matters/汽油桶.mod");

state_end();

convert_begin("寻常","日军");

	gfx("汽油桶爆炸效果.gfx","");

convert_end();

state_begin("日军");

	model("");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/摩托车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	gfx("","");
	model("characters/摩托车.mod");

state_end();

convert_begin("寻常","击毁");

	gfx("爆炸光影_大.gfx","");
convert_end();
state_begin("击毁");
	gfx("击毁_车辆.gfx","");
	model("characters/摩托车击毁.mod");

state_end();

//-------------------------------------------------------------
define_HH("HH_摩托车爆炸",0);
define_HH("HH_摩托兵机枪兵",1);
define_HH("HH_摩托车驾驶员",2);


//11-21
action_begin("开车_循环",true);
	
	anim("[开车]"	,2		);

action_end(); 

action_begin("停车",false);
	
	anim("[停车]"	,2		);

action_end(); file_end();
file_begin("data/model/汽油桶.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/汽油桶.mod");

state_end();

convert_begin("寻常","日军");
	gfx("击毁_汽油桶.gfx","");
	gfx("爆炸光影_大.gfx","");	
convert_end();

state_begin("日军");

	model("");

state_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/运兵车载物.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	gfx("","");
	model("characters/军用运兵车_栽物.mod");

state_end();

convert_begin("寻常","击毁");

	gfx("爆炸光影_大.gfx","");
convert_end();
state_begin("击毁");
	gfx("击毁_车辆.gfx","");
	model("characters/军用运兵车击毁.mod");

state_end();

//-------------------------------------------------------------

define_HH("HH_军用运输车爆炸",0);
define_HH("HH_士兵05",1);
define_HH("HH_士兵04",2);
define_HH("HH_士兵01",3);
define_HH("HH_士兵03",4);
define_HH("HH_士兵02",5);
define_HH("HH_普通木箱01",6);
define_HH("HH_汽油桶",7);

action_begin("开车_循环",true);
	
	anim("[开车]"	,2		);

action_end(); 

action_begin("停车",false);
	
	anim("[停车]"	,2		);

action_end(); 



file_end();
file_begin("data/model/沙包04.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	damaged vehicle - NO.2 character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/街道沙包掩体04.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/集装箱02.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	damaged vehicle - NO.2 character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/集装箱02.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/集装箱03.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	damaged vehicle - NO.2 character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/集装箱03.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/集装箱01.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	damaged vehicle - NO.2 character define file .
	Create By MengXin 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/集装箱01.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/忍者头目.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("characters/忍者头目_硬动画.mod");

state_end();

state_begin("");

	model("");

state_end();

//
action_begin("向左跑发镖",false);
	
	anim("[向左跑发镖]"	,2		);

action_end(); 

action_begin("向右跑发镖",false);
	
	anim("[向右跑发镖]"	,2		);

action_end(); 

action_begin("向左跑发镖_循环",true);
	
	anim("[向左跑发镖]"	,2		);

action_end(); 

action_begin("向右跑发镖_循环",true);
	
	anim("[向右跑发镖]"	,2		);

action_end(); 

action_begin("向前跑_循环",true);
	
	anim("[向前跑]"	,2		);

action_end(); 

action_begin("向前跑挥刀",false);
	
	anim("[向前跑挥刀]"	,2		);

action_end(); 

action_begin("向前跑挥刀_循环",true);
	
	anim("[向前跑挥刀]"	,2		);

action_end(); 

action_begin("站立跳起",false);
	
	anim("[站立跳起]"	,2		);

action_end(); 

action_begin("站立跳在空中_循环",true);
	
	anim("[站立跳在空中]"	,2		);

action_end(); 

action_begin("站立跳起后落地下蹲",false);
	
	anim("[落地]"	,2		);

action_end(); 

action_begin("保持下蹲_循环",true);
	
	anim("[落地准备姿势]"	,2		);

action_end(); 

action_begin("下蹲挥刀攻击",false);
	
	anim("[下蹲挥刀攻击]"	,2		);

action_end(); 

action_begin("下蹲挥刀攻击_循环",true);
	
	anim("[下蹲挥刀攻击]"	,2		);

action_end(); 

action_begin("下蹲跳起后退",false);
	
	anim("[下蹲跳起后退]"	,2		);

action_end(); 

action_begin("下蹲跳起后退在空中_循环",true);
	
	anim("[下蹲跳起后退空中]"	,2		);

action_end(); 

action_begin("下蹲跳起后退落地",false);
	
	anim("[下蹲跳起后退落地]"	,2		);

action_end(); 

action_begin("后空翻",false);
	
	anim("[后空翻]"	,2		);

action_end(); 

action_begin("后空翻_循环",true);
	
	anim("[后空翻]"	,2		);

action_end(); 


//11/08 14:25 更新
action_begin("左跑_循环",true);
	
	anim("[左跑]"	,2		);

action_end(); 

action_begin("右跑_循环",true);
	
	anim("[右跑]"	,2		);

action_end(); 


//11-13 新添动作
action_begin("空中死亡",false);
	
	anim("[空中死亡]"	,4		);

action_end();

action_begin("空中死亡下落_循环",true);
	
	anim("[空中死亡下落]"	,4		);

action_end(); 

action_begin("空中死亡落地",false);
	
	anim("[空中死亡落地]"	,4		);

action_end();

//12-17 新添动作
action_begin("站立后空翻",false);
	
	anim("[站立后空翻]"	,4		);

action_end();

action_begin("下蹲后空翻",false);
	
	anim("[下蹲后空翻]"	,4		);

action_end();

action_begin("前空翻",false);
	
	anim("[前空翻]"	,4		);

action_end();
file_end();
file_begin("data/model/主观巴祖卡火箭.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主观巴祖卡火箭.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/主观汤姆森冲锋枪.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主观汤姆森冲锋枪.mod");

state_end();
define_HH("HH_退膛",0);

//-------------------------------------------------------------
file_end();
file_begin("data/model/主观重机枪.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主观重机枪.mod");

state_end();
define_HH("HH_退膛",0);

//-------------------------------------------------------------
file_end();
file_begin("data/model/主观毛瑟步枪.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主观毛瑟步枪.mod");

state_end();
define_HH("HH_退膛",0);

//-------------------------------------------------------------
file_end();
file_begin("data/model/汽油桶2.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/汽油桶2.mod");

state_end();

convert_begin("寻常","日军");

	gfx("击毁_汽油桶.gfx","");
	gfx("爆炸光影_大.gfx","");
convert_end();

state_begin("日军");

	model("");

state_end();
//-------------------------------------------------------------
file_end();
file_begin("data/model/主观汽车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主观汽车.mod");

state_end();

//-------------------------------------------------------------
file_end();
file_begin("data/model/主观捷克轻机枪.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.7 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	model("matters/主观捷克轻机枪.mod");

state_end();
define_HH("HH_退膛",0);

//-------------------------------------------------------------
file_end();
file_begin("data/model/强化装甲车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	gfx("","");
	model("characters/强化装甲车.mod");

state_end();

state_begin("");

	gfx("击毁_车辆.gfx","");
	model("characters/强化装甲车击毁.mod");

state_end();
convert_begin("寻常","");

	gfx("爆炸光影_大.gfx","");
convert_end();
//-------------------------------------------------------------
define_HH("HH_强化装甲车爆炸",0);


//
action_begin("开车_循环",true);
	
	anim("[开车]"	,4		);

action_end(); 

action_begin("开火_循环",true,10.0);
	
	anim("[开火]"	,4		);

action_end(); 


action_begin("开火",false);
	
	anim("[开火]"	,4		);

action_end(); 

action_begin("停车",false);
	
	anim("[停车]"	,4		);

action_end(); 

define_HH("HH_强化装甲车_炮塔",1);
file_end();
file_begin("data/model/装甲车.txt");
/*--------------------------------------------------------------------------

	Copy Right EpieGame 
 	Japanese soldier - NO.1 character define file .
	Create By MengXin 
	This is sample file 

--------------------------------------------------------------------------*/	

state_begin("寻常");

	gfx("","");
	model("characters/装甲车.mod");

state_end();

convert_begin("寻常","击毁");

	gfx("爆炸光影_大.gfx","");
convert_end();
state_begin("击毁");
	gfx("击毁_车辆.gfx","");
	model("characters/装甲车击毁.mod");

state_end();

//
action_begin("开车_循环",true);
	
	anim("[开车]"	,4		);

action_end(); 

action_begin("开火_循环",true,10.0);
	
	anim("[开火]"	,4		);

action_end(); 


action_begin("开火",false);
	
	anim("[开火]"	,4		);

action_end(); 

action_begin("停车",false);
	
	anim("[停车]"	,4		);

action_end(); 

define_HH("HH_装甲车炮塔",0);

file_end();

file_begin("data/model/椅子.txt");
state_begin("寻常");

	model("matters/太师椅.mod");

state_end();

state_begin("");

	model("");

state_end();

//-------------------------------------------------------------
file_end();

}