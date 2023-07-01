// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistSnd.cpp
// Creator: Wei Hua (魏华)
// 为音效做的缓存

#include "A3DAssistSnd.h"
#include "A3DAssistTools_Cmn.h"
#include "jexception.h"
#include "ADebug.h"
#include <am.h>

// 公共函数
bool	IsSndStopped(AMSoundBuffer *pAMSoundBuffer)
{
	pAMSoundBuffer->CheckEnd();
	return	pAMSoundBuffer->IsStopped();
}

////////////////////////////////////////////////////////////////////
// A3DAssist2DOSOP
////////////////////////////////////////////////////////////////////
A3DAssist2DOSOP::A3DAssist2DOSOP()
{
	m_ppAMSoundBuffer	= NULL;
	m_pA3DEngine		= NULL;
	m_nFadeInFinishTime	= 0;
	m_bJustLoad			= false;
}
A3DAssist2DOSOP::~A3DAssist2DOSOP()
{
}
bool	A3DAssist2DOSOP::StillInUse(int nIdx)
{
	J_EXCEPTION_RECORD(0);
	if( m_ppAMSoundBuffer[nIdx] )
	{
		return	true;
		//return	!IsSndStopped(m_ppAMSoundBuffer[nIdx]);
	}
	return	false;
}
bool	A3DAssist2DOSOP::ClrSnd(int i)
{
	J_EXCEPTION_RECORD(0);
	if( m_ppAMSoundBuffer[i] )
	{
		m_ppAMSoundBuffer[i]->UnloadImmEffect();
		m_pA3DEngine->GetAMSoundEngine()->GetSoundBufferMan()
			->ReleaseSoundBuffer(m_ppAMSoundBuffer[i]);
		m_ppAMSoundBuffer[i]	= NULL;
	}
	return	true;
}
AMSoundBuffer	* A3DAssist2DOSOP::SearchSndBuf(char *szSndFile)
{
	J_EXCEPTION_RECORD(0);
	int		nIdx;
	nIdx	= SearchString(szSndFile);
	if( nIdx<0 )
	{
		return	NULL;
	}
	return	m_ppAMSoundBuffer[nIdx];
}
int		A3DAssist2DOSOP::CheckAndSetSnd(char *szSndFile)
{
	J_EXCEPTION_RECORD(0);
	bool	bval;
	int		nIdx;

	nIdx	= SearchString(szSndFile);
	if( nIdx<0 )
	{
		// 如果所有声音都在播放，就算了
		nIdx	= GetMEUU();
		if( nIdx<0 )
		{
			return	-1;
		}
		// 为了保险先要释放一次
		ClrSnd(nIdx);
		//
		AMSoundBuffer	* pAMSoundBuffer;
		bval	= m_pA3DEngine->GetAMSoundEngine()->GetSoundBufferMan()
			->LoadSoundBufferFromFile(szSndFile, &pAMSoundBuffer);
		if( !bval || !pAMSoundBuffer )
		{
			A3DAssistCache_String::Reset(nIdx);
			ADebug::Msg(0,"!!!! Can not LoadSoundBufferFromFile for %s, %d"
				, szSndFile, nIdx);
			return	-1;
		}
		m_ppAMSoundBuffer[nIdx]	= pAMSoundBuffer;

		if( RegisterString(nIdx, szSndFile)<0 )
		{
			assert(0);
			ADebug::Msg(0,"!!!!Can not register sndfile:%s for idx:%d"
				, szSndFile
				, nIdx);
			return	-1;
		}
	}
	return	nIdx;
}
bool	A3DAssist2DOSOP::Init(A3DEngine *pA3DEngine, int nTotal, int nStrLen)
{
	J_EXCEPTION_RECORD(0);
	bool			bval;

	m_pA3DEngine	= pA3DEngine;

	bval			= A3DAssistCache_String::Init(nTotal, nStrLen);
	if( !bval )
	{
		return	bval;
	}

	m_ppAMSoundBuffer	= new AMSoundBuffer * [nTotal];
	if( !m_ppAMSoundBuffer )
	{
		return		false;
	}
	memset(m_ppAMSoundBuffer, 0, sizeof(*m_ppAMSoundBuffer)*nTotal);

	return			true;
}
bool	A3DAssist2DOSOP::Release()
{
	J_EXCEPTION_RECORD(0);
	ResetAll();
	if( m_ppAMSoundBuffer )
	{
		delete []	m_ppAMSoundBuffer;
		m_ppAMSoundBuffer	= NULL;
	}
	A3DAssistCache_String::Release();

	return	true;
}
bool	A3DAssist2DOSOP::LogicRun()
{
	J_EXCEPTION_RECORD(0);
	int		i;
	AMSoundBuffer	*pAMSoundBuffer;
	// 给所有需要fadetick的服务
	for(i=0;i<m_nTotal;i++)
	{
		if( !m_pUnits[i].m_bTaken )
		{
			continue;
		}
		//if( StillInUse(i)
		if( 1	// 不调用StillInUse免得中断了别的声音
		&&  m_pUnits[i].m_nProperty & AA_SND_PROPERTY_NEEDFADETICK )
		{
			pAMSoundBuffer	= m_ppAMSoundBuffer[i];
			pAMSoundBuffer->TickFade();
		}
	}
	return	true;
}
bool	A3DAssist2DOSOP::ResetAll()
{
	J_EXCEPTION_RECORD(0);
	int	i;
	if( m_ppAMSoundBuffer )
	{
		for(i=0;i<m_nTotal;i++)
		{
			ClrSnd(i);
		}
	}
	A3DAssistCache_String::ResetAll();
	return	true;
}
bool	A3DAssist2DOSOP::Play(char *szSndFile, bool bImmEffect, bool bLoop, bool bCanFade)
{
	J_EXCEPTION_RECORD(0);
	bool	bval;
	int		nIdx;
	AMSoundBuffer	*pAMSoundBuffer;

	nIdx	= CheckAndSetSnd(szSndFile);
	if( nIdx<0 )
	{
		return	false;
	}
	pAMSoundBuffer	= m_ppAMSoundBuffer[nIdx];
	if( !pAMSoundBuffer )
	{
		assert(0);
		ADebug::Msg(0,"!!!!NULL buf when play:%s idx:%d"
			, szSndFile
			, nIdx);
		// 打印所有元素的内容
		int	i;
		for(i=0;i<m_nTotal;i++)
		{
			if( !m_pUnits[i].m_bTaken )
			{
				continue;
			}
			ADebug::Msg(0,"No.%d InUse:%d %s LastTick:%d UseCount:%d Prop:%x"
				, i
				, StillInUse(i)
				, m_asa.GetUnit(i)
				, m_pUnits[i].m_nLastUsedTick
				, m_pUnits[i].m_nUseCount
				, m_pUnits[i].m_nProperty
				);
		}
		// 不返回，等着出错
		// 万一有错，就跳过去
		//return	false;
	}
	if( bImmEffect )
	{
		pAMSoundBuffer->LoadImmEffect();
	}
	else
	{
		pAMSoundBuffer->UnloadImmEffect();
	}

	if( bCanFade )
	{
		m_pUnits[nIdx].m_nProperty |= AA_SND_PROPERTY_CANFADE;
	}
	if( !m_bJustLoad )
	{
		// 如果在时间范围内则作淡入处理
		if( m_nFadeInFinishTime > g_AA_nSystemTickcount
		&&  bCanFade )
		{
			bval	= pAMSoundBuffer->FadeIn();
			m_pUnits[nIdx].m_nProperty	|= AA_SND_PROPERTY_NEEDFADETICK;
		}
		else
		{
			bval	= pAMSoundBuffer->Play(bLoop);
		}
	}
	SetLastUsedTick(nIdx);

	return	bval;
}
bool	A3DAssist2DOSOP::Stop(char *szSndFile)
{
	J_EXCEPTION_RECORD(0);
	bool	bval;

	AMSoundBuffer	*pAMSoundBuffer;
	pAMSoundBuffer	= SearchSndBuf(szSndFile);
	if( !pAMSoundBuffer || IsSndStopped(pAMSoundBuffer) )
	{
		return	false;
	}
	bval	= pAMSoundBuffer->Stop();

	return	true;
}
bool	A3DAssist2DOSOP::Pause(char *szSndFile)
{
	J_EXCEPTION_RECORD(0);
	bool	bval;

	AMSoundBuffer	*pAMSoundBuffer;
	pAMSoundBuffer	= SearchSndBuf(szSndFile);
	if( !pAMSoundBuffer || pAMSoundBuffer->IsPaused() )
	{
		return	false;
	}
	bval	= pAMSoundBuffer->Pause();

	return	true;
}
bool	A3DAssist2DOSOP::IsStopped(char *szSndFile)
{
	J_EXCEPTION_RECORD(0);
	AMSoundBuffer	*pAMSoundBuffer;
	pAMSoundBuffer	= SearchSndBuf(szSndFile);
	if( !pAMSoundBuffer )
	{
		return	false;
	}
	
	return	IsSndStopped(pAMSoundBuffer);
}
bool	A3DAssist2DOSOP::FadeIn(int ms)
{
	J_EXCEPTION_RECORD(0);
	// 设置时间
	m_nFadeInFinishTime	= g_AA_nSystemTickcount + ms;
	return	true;
}
bool	A3DAssist2DOSOP::FadeOut()
{
	J_EXCEPTION_RECORD(0);
	// 遍历当前所有正在播放的声音，如果需要fade就fadeout
	int		i;
	AMSoundBuffer	*pAMSoundBuffer;
	// 给所有需要fadetick的服务
	for(i=0;i<m_nTotal;i++)
	{
		if( !m_pUnits[i].m_bTaken )
		{
			continue;
		}
		if( StillInUse(i)
		&&  m_pUnits[i].m_nProperty & AA_SND_PROPERTY_CANFADE )
		{
			pAMSoundBuffer	= m_ppAMSoundBuffer[i];
			pAMSoundBuffer->FadeOut();
		}
	}
	return	true;
}

////////////////////////////////////////////////////////////////////
// A3DAssist2DSnd
////////////////////////////////////////////////////////////////////
A3DAssist2DSnd::A3DAssist2DSnd()
{
	m_ppAMSoundBuffer	= NULL;
	m_pA3DEngine		= NULL;
	m_nFadeInFinishTime	= 0;
	m_bJustLoad			= false;
}
A3DAssist2DSnd::~A3DAssist2DSnd()
{
}
bool	A3DAssist2DSnd::StillInUse(int nIdx)
{
	if( m_ppAMSoundBuffer[nIdx] )
	{
		return	true;
		//return	!IsSndStopped(m_ppAMSoundBuffer[nIdx]);
	}
	return	false;
}
bool	A3DAssist2DSnd::ClrSnd(int i)
{
	if( m_ppAMSoundBuffer[i] )
	{
		m_ppAMSoundBuffer[i]->UnloadImmEffect();
		m_pA3DEngine->GetAMSoundEngine()->GetSoundBufferMan()
			->ReleaseSoundBuffer(m_ppAMSoundBuffer[i]);
		m_ppAMSoundBuffer[i]	= NULL;
	}
	return	true;
}
bool	A3DAssist2DSnd::Init(A3DEngine *pA3DEngine, int nTotal, int nStrLen)
{
	bool			bval;

	m_pA3DEngine	= pA3DEngine;

	bval			= A3DAssistCache_StrDup::Init(nTotal, nStrLen);
	if( !bval )
	{
		return	bval;
	}

	m_ppAMSoundBuffer	= new AMSoundBuffer * [nTotal];
	if( !m_ppAMSoundBuffer )
	{
		return		false;
	}
	memset(m_ppAMSoundBuffer, 0, sizeof(*m_ppAMSoundBuffer)*nTotal);

	return			true;
}
bool	A3DAssist2DSnd::Release()
{
	ResetAll();
	if( m_ppAMSoundBuffer )
	{
		delete []	m_ppAMSoundBuffer;
		m_ppAMSoundBuffer	= NULL;
	}

	A3DAssistCache_StrDup::Release();

	return	true;
}
bool	A3DAssist2DSnd::LogicRun()
{
	int		i;
	AMSoundBuffer	*pAMSoundBuffer;
	// 给所有需要fadetick的服务
	for(i=0;i<m_nTotal;i++)
	{
		if( !m_pUnits[i].m_bTaken )
		{
			continue;
		}
		//if( StillInUse(i)
		if( 1	// 不调用StillInUse免得中断了别的声音
		&&  m_pUnits[i].m_nProperty & AA_SND_PROPERTY_NEEDFADETICK )
		{
			pAMSoundBuffer	= m_ppAMSoundBuffer[i];
			pAMSoundBuffer->TickFade();
		}
	}
	return	true;
}
bool	A3DAssist2DSnd::ResetAll()
{
	int	i;
	if( m_ppAMSoundBuffer )
	{
		for(i=0;i<m_nTotal;i++)
		{
			ClrSnd(i);
		}
	}
	A3DAssistCache_StrDup::ResetAll();
	return	true;
}
bool	A3DAssist2DSnd::Play(char *szSndFile, bool bImmEffect, bool bCanFade)
{
	int				nIdx;
	bool			bval;
	AMSoundBuffer	* pAMSoundBuffer;

	bval	= SearchAndAct(szSndFile, &nIdx, false);
	if( !bval )
	{
		// 重新初始化单元
		// 为了保险先要释放一次
		ClrSnd(nIdx);
		//
		bval	= m_pA3DEngine->GetAMSoundEngine()->GetSoundBufferMan()
			->LoadSoundBufferFromFile(szSndFile, &pAMSoundBuffer);
		if( !bval || !pAMSoundBuffer )
		{
			A3DAssistCache_StrDup::Reset(nIdx);
			return	false;
		}
		m_ppAMSoundBuffer[nIdx]	= pAMSoundBuffer;
	}
	else
	{
		pAMSoundBuffer	= m_ppAMSoundBuffer[nIdx];
	}

	if( bImmEffect )
	{
		pAMSoundBuffer->LoadImmEffect();
	}
	else
	{
		pAMSoundBuffer->UnloadImmEffect();
	}

	if( bCanFade )
	{
		m_pUnits[nIdx].m_nProperty |= AA_SND_PROPERTY_CANFADE;
	}
	if( !m_bJustLoad )
	{
		// 如果在时间范围内则作淡入处理
		if( m_nFadeInFinishTime > g_AA_nSystemTickcount
		&&  bCanFade )
		{
			bval	= pAMSoundBuffer->FadeIn();
			m_pUnits[nIdx].m_nProperty	|= AA_SND_PROPERTY_NEEDFADETICK;
		}
		else
		{
			bval	= pAMSoundBuffer->Play(false);
		}
	}

	return	true;
}
bool	A3DAssist2DSnd::FadeIn(int ms)
{
	// 设置时间
	m_nFadeInFinishTime	= g_AA_nSystemTickcount + ms;
	return	true;
}
bool	A3DAssist2DSnd::FadeOut()
{
	// 遍历当前所有正在播放的声音，如果需要fade就fadeout
	int		i;
	AMSoundBuffer	*pAMSoundBuffer;
	// 给所有需要fadetick的服务
	for(i=0;i<m_nTotal;i++)
	{
		if( !m_pUnits[i].m_bTaken )
		{
			continue;
		}
		if( StillInUse(i)
		&&  m_pUnits[i].m_nProperty & AA_SND_PROPERTY_CANFADE )
		{
			pAMSoundBuffer	= m_ppAMSoundBuffer[i];
			pAMSoundBuffer->FadeOut();
		}
	}
	return	true;
}


////////////////////////////////////////////////////////////////////
// A3DAssist3DSnd
////////////////////////////////////////////////////////////////////
A3DAssist3DSnd::A3DAssist3DSnd()
{
	m_ppAM3DSoundBuffer	= NULL;
	m_pA3DEngine		= NULL;
	m_nFadeInFinishTime	= 0;
	m_bJustLoad			= false;
}
A3DAssist3DSnd::~A3DAssist3DSnd()
{
}
bool	A3DAssist3DSnd::ClrSnd(int i)
{
	if( m_ppAM3DSoundBuffer[i] )
	{
		m_ppAM3DSoundBuffer[i]->UnloadImmEffect();
		m_pA3DEngine->GetAMSoundEngine()->Get3DSoundBufferMan()
			->Release3DSoundBuffer(m_ppAM3DSoundBuffer[i]);
		m_ppAM3DSoundBuffer[i]	= NULL;
	}
	return	true;
}
bool	A3DAssist3DSnd::StillInUse(int nIdx)
{
	if( m_ppAM3DSoundBuffer[nIdx] )
	{
		//return	true;
		return	!IsSndStopped(m_ppAM3DSoundBuffer[nIdx]);
	}
	return	false;
}
bool	A3DAssist3DSnd::Init(A3DEngine *pA3DEngine, int nTotal, int nStrLen)
{
	bool			bval;

	m_pA3DEngine	= pA3DEngine;

	bval			= A3DAssistCache_StrDup::Init(nTotal, nStrLen);
	if( !bval )
	{
		return	bval;
	}

	m_ppAM3DSoundBuffer	= new AM3DSoundBuffer * [nTotal];
	if( !m_ppAM3DSoundBuffer )
	{
		return		false;
	}
	memset(m_ppAM3DSoundBuffer, 0, sizeof(*m_ppAM3DSoundBuffer)*nTotal);

	return			true;
}
bool	A3DAssist3DSnd::Release()
{
	ResetAll();
	if( m_ppAM3DSoundBuffer )
	{
		delete []	m_ppAM3DSoundBuffer;
		m_ppAM3DSoundBuffer	= NULL;
	}

	A3DAssistCache_StrDup::Release();

	return	true;
}
bool	A3DAssist3DSnd::LogicRun()
{
	int		i;
	AM3DSoundBuffer	*pAM3DSoundBuffer;
	// 给所有需要fadetick的服务
	for(i=0;i<m_nTotal;i++)
	{
		if( !m_pUnits[i].m_bTaken )
		{
			continue;
		}
		//if( StillInUse(i)
		if( 1	// 不调用StillInUse免得中断了别的声音
		&&  m_pUnits[i].m_nProperty & AA_SND_PROPERTY_NEEDFADETICK )
		{
			pAM3DSoundBuffer	= m_ppAM3DSoundBuffer[i];
			pAM3DSoundBuffer->TickFade();
		}
	}
	return	true;
}
bool	A3DAssist3DSnd::ResetAll()
{
	int	i;
	if( m_ppAM3DSoundBuffer )
	{
		for(i=0;i<m_nTotal;i++)
		{
			ClrSnd(i);
		}
	}
	A3DAssistCache_StrDup::ResetAll();
	return	true;
}
bool	A3DAssist3DSnd::Play(char *szSndFile, A3DVECTOR3 &vecPos, float fMinDist, float fMaxDist, bool bImmEffect, bool bForce2D, bool bCanFade)
{
	int				nIdx;
	bool			bval;
	AM3DSoundBuffer	* pAM3DSoundBuffer;

	bval	= SearchAndAct(szSndFile, &nIdx, true);
	if( !bval )
	{
		// 重新初始化单元
		// 为了保险先要释放一次
		ClrSnd(nIdx);
		//
		bval	= m_pA3DEngine->GetAMSoundEngine()->Get3DSoundBufferMan()
			->Load3DSoundBufferFromFile(szSndFile, &pAM3DSoundBuffer);
		if( !bval || !pAM3DSoundBuffer )
		{
			A3DAssistCache_StrDup::Reset(nIdx);
			return	false;
		}
		m_ppAM3DSoundBuffer[nIdx]	= pAM3DSoundBuffer;

	}
	else
	{
		pAM3DSoundBuffer	= m_ppAM3DSoundBuffer[nIdx];
	}

	pAM3DSoundBuffer->SetPosition(vecPos);
	pAM3DSoundBuffer->SetMinDistance(fMinDist);
	pAM3DSoundBuffer->SetMaxDistance(fMaxDist);
	pAM3DSoundBuffer->SetForce2D(bForce2D);
	pAM3DSoundBuffer->UpdateChanges();

	if( bImmEffect )
	{
		pAM3DSoundBuffer->LoadImmEffect();
	}
	else
	{
		pAM3DSoundBuffer->UnloadImmEffect();
	}

	if( bCanFade )
	{
		m_pUnits[nIdx].m_nProperty |= AA_SND_PROPERTY_CANFADE;
	}
	// 如果在时间范围内则作淡入处理
	if( !m_bJustLoad )
	{
		if( m_nFadeInFinishTime > g_AA_nSystemTickcount
		&&  bCanFade )
		{
			bval	= pAM3DSoundBuffer->FadeIn();
			m_pUnits[nIdx].m_nProperty	|= AA_SND_PROPERTY_NEEDFADETICK;
		}
		else
		{
			bval	= pAM3DSoundBuffer->Play(false);
		}
	}

	return	true;
}
bool	A3DAssist3DSnd::FadeIn(int ms)
{
	// 设置时间
	m_nFadeInFinishTime	= g_AA_nSystemTickcount + ms;
	return	true;
}
bool	A3DAssist3DSnd::FadeOut()
{
	// 遍历当前所有正在播放的声音，如果需要fade就fadeout
	int		i;
	AM3DSoundBuffer	* pAM3DSoundBuffer;
	// 给所有需要fadetick的服务
	for(i=0;i<m_nTotal;i++)
	{
		if( !m_pUnits[i].m_bTaken )
		{
			continue;
		}
		if( StillInUse(i)
		&&  m_pUnits[i].m_nProperty & AA_SND_PROPERTY_CANFADE )
		{
			pAM3DSoundBuffer	= m_ppAM3DSoundBuffer[i];
			pAM3DSoundBuffer->FadeOut();
		}
	}
	return	true;
}
