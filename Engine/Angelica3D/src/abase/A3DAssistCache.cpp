// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: A3DAssistCache.cpp
// Creator: Wei Hua (魏华)
// 通用缓存

#include "A3DAssistCache.h"
#include <string.h>

unsigned int	A3DAssistCache::m_nCurTick	= 0;
////////////////////////////////////////////////////////////////////
// A3DAssistCache
////////////////////////////////////////////////////////////////////
A3DAssistCache::A3DAssistCache()
{
	m_nTotal	= 0;
	m_pUnits	= NULL;
}
A3DAssistCache::~A3DAssistCache()
{
}
bool	A3DAssistCache::Init(int nTotal)
{
	m_pUnits	= new A3DASSISTCACHEUNIT[nTotal];
	if( !m_pUnits )
	{
		return	false;
	}
	memset(m_pUnits, 0, sizeof(*m_pUnits)*nTotal);
	m_nTotal	= nTotal;
	return	true;
}
bool	A3DAssistCache::Release()
{
	if( m_pUnits )
	{
		delete []	m_pUnits;
		m_pUnits	= NULL;
		m_nTotal	= 0;
	}
	return	true;
}
void	A3DAssistCache::SetLastUsedTick(int nIdx)
{
	SetLastUsedTick(nIdx, m_nCurTick);
}
void	A3DAssistCache::SetLastUsedTick(int nIdx, int nTick)
{
	m_pUnits[nIdx].m_nLastUsedTick	= nTick;
	m_pUnits[nIdx].m_nUseCount		++;
}
void	A3DAssistCache::Reset(int nIdx)
{
	memset(&m_pUnits[nIdx], 0, sizeof(m_pUnits[nIdx]));
}
void	A3DAssistCache::ResetAll()
{
	memset(m_pUnits, 0, sizeof(*m_pUnits)*m_nTotal);
}
int		A3DAssistCache::GetMEUU(bool bForce)
{
	int		i;
	int		nMEUU;
	nMEUU	= -1;

	for(i=0;i<m_nTotal;i++)
	{
		if( !m_pUnits[i].m_bTaken )
		{
			return	i;
		}
		if( !bForce && StillInUse(i) )
		{
			continue;
		}
		if( nMEUU<0
		||  m_pUnits[i].Cmp(m_pUnits[nMEUU])<0 )
		{
			nMEUU	= i;
		}
	}
	return	nMEUU;
}

////////////////////////////////////////////////////////////////////
// A3DAssistCache_StrDup
////////////////////////////////////////////////////////////////////
A3DAssistCache_StrDup::A3DAssistCache_StrDup()
{
}
A3DAssistCache_StrDup::~A3DAssistCache_StrDup()
{
}
bool	A3DAssistCache_StrDup::SearchAndAct(char *szStr, int *pnMEUU, bool bFindOld)
{
	int		i;

	if( bFindOld )
	{
		for(i=0;i<m_nTotal;i++)
		{
			// 如果没有使用就不用查
			if( !m_pUnits[i].m_bTaken )
			{
				continue;
			}
			if( StillInUse(i) )	continue;
			if( strcmp(m_asa.GetUnit(i), szStr)==0 )
			{
				*pnMEUU	= i;
				return	true;
			}
		}
	}

	i	= GetMEUU();
	*pnMEUU = i;
	if( i >=0 )
	{
		m_asa.StrCpy(i, szStr);
	}

	SetLastUsedTick(i);
	m_pUnits[i].m_bTaken	= true;

	return	false;
}
void	A3DAssistCache_StrDup::Reset(int nIdx)
{
	m_asa.ClearUnit(nIdx);
	A3DAssistCache::Reset(nIdx);
}
bool	A3DAssistCache_StrDup::Init(int nTotal, int nStrLen)
{
	bool	bval;

	if( !m_asa.Init(nTotal, nStrLen) )
	{
		return	false;
	}
	
	bval	= A3DAssistCache::Init(nTotal);
	if( !bval )
	{
		return	bval;
	}

	return	true;
}
bool	A3DAssistCache_StrDup::Release()
{
	m_asa.Release();

	A3DAssistCache::Release();

	return	true;
}
bool	A3DAssistCache_StrDup::ResetAll()
{
	A3DAssistCache::ResetAll();

	return	true;
}
