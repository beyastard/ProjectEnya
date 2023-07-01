/*
 * FILE: AGamePlay.h
 *
 * DESCRIPTION: 游戏核心类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef __AGAMEPLAY__H__
#define __AGAMEPLAY__H__

#include "AProcessor.h"

class AManager;
class AClassInfo;
class ADeviceMan;
class ACamera;
class AWC;
class AGameLoading;
class AIDRec;
class AWorld;
class AGame;

#define MAX_MANAGER_NUM 32
#define NOLOGICRUNCOUNT		3		//tick count

class AGamePlay : public AProcessor  
{
protected:
	int				m_exitCode;
	//管理器列表
	AManager *		m_managerList[MAX_MANAGER_NUM];
	AManager *		m_managerLastDarw;
	ACamera *		m_pACamera;
	AWC *			m_pAInterface;
	DWORD			m_bits;
	AWorld *		m_pAWorld;
	bool			m_bRain;
	bool			m_bSnow;
	float			m_fTickAnimationTime;
	bool			m_bResetAGameRes;
	int				m_nEarIn;
	int				m_nNoLogicRunCount;
	bool			m_bGamePlayRender;
	bool			m_bPrecach;
public:
	DECLARE_ASERIAL(AGamePlay)

protected:
	virtual bool A3DAssistToolsLogicRun(float time);
	virtual void DeviceDealOut();
	virtual void DeviceDealIn();
	virtual void AfterRender();
	virtual bool Draw();
	virtual bool LogicRun(float __TimeSpan);	

public:
	virtual bool ResetResource();
	void ShowCursor(bool bShow);
	virtual bool LogicRun_Begin(float time);
	virtual bool Reset();
	virtual bool Save(AArchive &ar);
	virtual bool Run(int& __nRet,float fTimeSpan);
	virtual bool Init(AGame * __pAGame);
	virtual bool Release();
	inline void RainStart(bool bStart) { m_bRain = bStart;}
	inline void SnowStart(bool bStart){ m_bSnow = bStart;}
	inline bool GetRainStart() { return m_bRain;}
	inline bool GetSnowStart() { return m_bSnow;}
	inline bool GetIsResetAGameRes() { return m_bResetAGameRes;}
	inline void SetIsResetAGameRes(bool bResetRes) { m_bResetAGameRes = bResetRes;}

	virtual bool TickAnimation(float fTimeSpan);
	inline void SetAWorld( AWorld * pAWorld) { m_pAWorld = pAWorld;}
	inline AWorld * GetAWorld() { return m_pAWorld;}
	
	AManager* GetManagerById(int __nId) { return m_managerList[__nId];}
	bool AddManPtr(AManager * __pAManager);

	inline ACamera * GetACamera() { return m_pACamera;}
	AGamePlay();
	virtual ~AGamePlay();

	inline void SetBits(DWORD add,DWORD remove) {m_bits |= add;m_bits &= ~remove; }
	inline AManager * GetLastDrawMan() { return m_managerLastDarw;}
	inline AManager * SetLastDrawMan(AManager * pMan)
	{
		AManager * pFirst = m_managerLastDarw;
		m_managerLastDarw = pMan;
		return pFirst;
	}
	inline DWORD GetBits() { return m_bits;}
	void SetNoLogicRunTickCount(int nCount);
	inline int GetNologicRunTickCount() { return m_nNoLogicRunCount;}
	inline bool GetGamePlayRender() { return m_bGamePlayRender;}
	inline void SetGamePlayRender(bool bGamePlayRender) { m_bGamePlayRender = bGamePlayRender;}
	inline void SetPrecach(bool b) { m_bPrecach = b;;}
	inline bool GetPrecach() { return m_bPrecach;}

};

#endif // __AGAMEPLAY__H__