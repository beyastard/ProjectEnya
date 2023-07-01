// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistSnd.h
// Creator: Wei Hua (魏华)
// 为音效做的缓存

#ifndef	__A3DASSISTSND_H__
#define	__A3DASSISTSND_H__

#include "A3DAssistCache.h"
#include <a3d.h>

// 定义
#define	AA_SND_PROPERTY_CANFADE			0x01	// 可以淡入淡出
#define	AA_SND_PROPERTY_NEEDFADETICK	0x02	// 需要调用tick淡入

// 公共函数
bool	IsSndStopped(AMSoundBuffer *pAMSoundBuffer);

// 每个wav只播同时一个(OneSoundOnePlay)
class	A3DAssist2DOSOP	: public A3DAssistCache_String
{
protected:
	bool			m_bJustLoad;				// 是否只load不不放
	AMSoundBuffer	**m_ppAMSoundBuffer;
	A3DEngine		*m_pA3DEngine;
	unsigned long	m_nFadeInFinishTime;		// 如果大于零则表示在次时刻前播放的所有需要fade的声音fadein
protected:
	bool	ClrSnd(int i);
	AMSoundBuffer	*SearchSndBuf(char *szSndFile);
	virtual bool	StillInUse(int nIdx);
protected:
	int		CheckAndSetSnd(char *szSndFile);
public:
	inline void	SetJustLoad(bool bSet)
	{ m_bJustLoad = bSet; }
public:
	A3DAssist2DOSOP();
	~A3DAssist2DOSOP();
	bool	Init(A3DEngine *pA3DEngine, int nTotal, int nStrLen);
	bool	Release();
	bool	LogicRun();							// 提供fade等功能
	bool	ResetAll();
	bool	Play(char *szSndFile, bool bImmEffect, bool bLoop, bool bCanFade);
	bool	Stop(char *szSndFile);
	bool	Pause(char *szSndFile);
	bool	IsStopped(char *szSndFile);
	bool	FadeIn(int ms);
	bool	FadeOut();
};

// wav不重用，只限制同时存在的个数
class	A3DAssist2DSnd		: public A3DAssistCache_StrDup
{
protected:
	bool			m_bJustLoad;				// 是否只load不不放
	AMSoundBuffer	**m_ppAMSoundBuffer;
	A3DEngine		*m_pA3DEngine;
	unsigned long	m_nFadeInFinishTime;		// 如果大于零则表示在次时刻前播放的所有需要fade的声音fadein
protected:
	bool	ClrSnd(int i);
	virtual bool	StillInUse(int nIdx);
public:
	inline void	SetJustLoad(bool bSet)
	{ m_bJustLoad = bSet; }
public:
	A3DAssist2DSnd();
	~A3DAssist2DSnd();
	bool	Init(A3DEngine *pA3DEngine, int nTotal, int nStrLen);
	bool	Release();
	bool	LogicRun();							// 提供fade等功能
	bool	ResetAll();
	bool	Play(char *szSndFile, bool bImmEffect, bool bCanFade);		// 因为不能停，所以不能有loop
	bool	FadeIn(int ms);
	bool	FadeOut();
};

class	A3DAssist3DSnd		: public A3DAssistCache_StrDup
{
protected:
	bool			m_bJustLoad;				// 是否只load不不放
	AM3DSoundBuffer	**m_ppAM3DSoundBuffer;
	A3DEngine		*m_pA3DEngine;
	unsigned long	m_nFadeInFinishTime;		// 如果大于零则表示在次时刻前播放的所有需要fade的声音fadein
protected:
	bool	ClrSnd(int i);
	virtual bool	StillInUse(int nIdx);
public:
	inline void	SetJustLoad(bool bSet)
	{ m_bJustLoad = bSet; }
public:
	A3DAssist3DSnd();
	~A3DAssist3DSnd();
	bool	Init(A3DEngine *pA3DEngine, int nTotal, int nStrLen);
	bool	Release();
	bool	LogicRun();							// 提供fade等功能
	bool	ResetAll();
	bool	Play(char *szSndFile, A3DVECTOR3 &vecPos, float fMinDist, float fMaxDist, bool bImmEffect, bool bForce2D, bool bCanFade);	// 引用是为了加快传递速度
	bool	FadeIn(int ms);
	bool	FadeOut();
};

#endif