/*
 * FILE: AWorld.H
 *
 * DESCRIPTION : Container class to link logic to the engine 
 *
 * CREATED BY: MengXin
 *
 * HISTORY:	2002 - 8 - 14
 *
 * Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
 */

#ifndef _AWORLD_H_
#define _AWORLD_H_

struct i_render_target;
struct i_shot_target;

class AScene;
class ASky;
class ATerrain;
class AEnvironment;
class A3DCDS;
class A3DViewport;
class A3DDevice;
class A3DEngine;
class AModelUnit;
#include "AWorldInit.h"

#include "hash_table_wrapper.h"
using namespace abase;

#include "AUList.h"
struct WorldCDServer;
class AWorld
{
public:
	enum
	{
		TYPE_TERRAIN,
		TYPE_SENCE,
		TYPE_MISSILE,
		TYPE_MATTER,
		TYPE_AI,
		TYPE_ANYTHING,
	};
protected:

	friend struct WorldCDServer;
	friend struct PVSHashTableEnumFunctioner;

	hashtab<AModelUnit*,int,_hash_function> m_PVSHashTable;

	AListT<i_render_target*>		m_RenderList;
	AListT<i_shot_target*>			m_MissileTraceList;
	AListT<i_shot_target*>			m_MatterTraceList;
	AListT<i_shot_target*>			m_AITraceList;

	A3DDevice *				m_pA3DDevice;
	A3DEngine *				m_pA3DEngine;
	AEnvironment*			m_pEnv;
	AScene*					m_pScn;
	ASky*					m_pSky;
	ATerrain*				m_pTrn;
	bool					m_bForceRender;
	bool					m_bSortAlpha;
	float					m_fDirLightWeight;
	float					m_fAmbLightWeight;
	A3DCDS*					m_pCDServer;	

public:

	inline void SetAmbLightWeight(float w){m_fAmbLightWeight = w;}
	inline void SetDirLightWeight(float w){m_fDirLightWeight = w;}
	inline float GetAmbLightWeight(){return m_fAmbLightWeight;}
	inline float GetDirLightWeight(){return m_fDirLightWeight;}
	inline void EanbleSortAlpha(bool e){m_bSortAlpha = e;}
	inline bool IsSortAlphaEnable(){return m_bSortAlpha;}

	inline AScene* GetAScene(){return m_pScn;}
	inline ASky* GetASky(){return m_pSky;}
	inline ATerrain* GetATerrain(){return m_pTrn;}
	inline AEnvironment* GetAEnvironment(){return m_pEnv;}

	virtual bool CreateSky(const PAWORLD_INIT_SKY& __pDatum);
	virtual bool CreateTerrain(const PAWORLD_INIT_TERRAIN& __pDatum);
	virtual bool CreateScene(const PAWORLD_INIT_SCENE& __pDatum);
	virtual bool CreateEnvironment(const PAWORLD_INIT_ENVIRONMENT& __pDatum);
	virtual void RefreshEnvironmentParam(AEnvironmentInit * pInit);

	virtual bool DestroySky();
	virtual bool DestroyTerrain();
	virtual bool DestroyScene();
	virtual bool DestroyEnvironment();

public:

	AWorld();
	virtual bool Release();
	virtual ~AWorld();

public:
	
	virtual bool Render(A3DViewport * pCurrentViewport);
	virtual bool RenderAlpha(A3DViewport * pCurrentViewport);

	bool RenderItemInHashTable(int* begin,int* end,A3DViewport * pCurrentViewport);
	bool RenderAllInHashTable(A3DViewport * pCurrentViewport);
	bool AddToRenderList(i_render_target * pTag);
	bool DeleteFromRenderList(i_render_target * pTag);

	bool RayTraceSence(const A3DVECTOR3& p,const A3DVECTOR3& v, float t,RAYTRACE * pRayTrace);
	bool RayTraceTerrain(const A3DVECTOR3& p, const A3DVECTOR3& v, float t,RAYTRACE * pRayTrace);
	bool RayTraceMissile(const A3DVECTOR3& p, const A3DVECTOR3& v, float t,RAYTRACE * pRayTrace,i_shot_target* pExcept,i_shot_target ** ppTaget);
	bool RayTraceMatter(const A3DVECTOR3& p, const A3DVECTOR3& v, float t,RAYTRACE * pRayTrace,i_shot_target* pExcept,i_shot_target ** ppTaget);
	bool RayTraceMatterInHashTable(const A3DVECTOR3& p, const A3DVECTOR3& v, float t,RAYTRACE * pRayTrace,int* begin,int*end);
	bool RayTraceAI(const A3DVECTOR3& p, const A3DVECTOR3& v, float t,RAYTRACE * pRayTrace,i_shot_target* pExcept,i_shot_target ** ppTaget);
	bool RayTraceAll(const A3DVECTOR3& p, const A3DVECTOR3& v, float t,RAYTRACE * pRayTrace, i_shot_target * pExcept, i_shot_target ** ppTarget,int * pType=0);
	bool inline RayTrace(const A3DVECTOR3& p, const A3DVECTOR3& v, float t,RAYTRACE * pRayTrace, i_shot_target * pExcept, i_shot_target ** ppTarget =0)
	{
		return RayTraceAll(p,v,t,pRayTrace,pExcept,ppTarget);
	}
	bool AddToMissileTraceList(i_shot_target* pTag);
	bool AddToMatterTraceList(i_shot_target* pTag);
	bool AddToAITraceList(i_shot_target* pTag);
	bool DeleteFromMissileTraceList(i_shot_target* pTag);
	bool DeleteFromMatterTraceList(i_shot_target* pTag);
	bool DeleteFromAITraceList(i_shot_target* pTag);
	unsigned int UpdateModelUnitFlagOfHashTable();

public:

	virtual void BindEngine(A3DEngine *  pE);
	virtual bool BindFinished();
	virtual bool TickAnimationExceptModels(float time_span);
	virtual void ForceRenderAll(bool b);
	virtual void RegisterToPVSHashTable(AModelUnit* _pItem,int _Index);
	virtual void UnregisterFromPVSHashTable(int _Index);
	virtual bool LoadNewRoute(BYTE* pBuf_Point,BYTE* pBuf_Route);
	virtual bool SetCurrentRoute(bool bRoute, int iSample);

};

#endif