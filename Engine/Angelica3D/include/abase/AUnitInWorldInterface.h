#ifndef __AUWINTERFACE_H__
#define __AUWINTERFACE_H__

class AUString;
class A3DViewport;

struct i_shot_attacker
{
	virtual float	GetBaseDamage()=0;
	virtual long	GetWarHeadID()=0;
	virtual float	GetFlyDistance()=0;
	virtual AUString GetBBName() = 0;
	virtual int	GetShotOwner()=0;
	virtual int GetMaterial() = 0;
	virtual A3DVECTOR3 GetHitPos() = 0;
	virtual A3DVECTOR3 GetNormal() = 0;
	virtual A3DVECTOR3 GetImpulseVelocity() = 0;
};

struct i_shot_target
{
	virtual int  OnShot(i_shot_attacker * ) = 0;
	virtual bool RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACE * pRayTrace) = 0;
	virtual bool GetRayTraceEnable() = 0;
	virtual void SetRayTraceEnable(bool b) = 0;
};

struct i_render_target
{
	virtual bool Render(A3DViewport * pCurrentViewport) = 0;	
	virtual bool GetVisibility() = 0;
	virtual void SetVisibility(bool b) = 0;
};

struct HH_infor
{
	AUString name;
	int		 index;	
};

enum MODEL_LEVEL
{
	LEVEL_NORMAL,
	LEVEL_BLOOD,
	LEVEL_URBANITY,
	LEVEL_SEXY
};

//逻辑外围类的动作事件反馈接口，
//通常情况下，包含 ModelWraper 类实例的对象应该将该接口
//实现并传给 ModelWraper。

struct i_host_listener
{
	virtual int receive_feedback(const char * cmd) = 0;
	virtual int anim_change(const char* name,void* buffer) = 0;
	virtual int anim_end(const char* name) = 0;
};
#endif //__AUWINTERFACE_H__ 