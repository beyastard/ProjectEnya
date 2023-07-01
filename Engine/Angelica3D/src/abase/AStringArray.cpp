// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: AStringArray.cpp
// Creator: Wei Hua (魏华)
// 字传数组

#include "AStringArray.h"
#include "APlatform.h"
#include "ABinString.h"

////////////////////////////////////////////////////////////////////
// AStringArray
////////////////////////////////////////////////////////////////////
AStringArray::AStringArray()
{
	m_pBuffer		= NULL;
	m_nTotalUnit	= 0;
	m_nUnitLen		= 0;
}
AStringArray::~AStringArray()
{
}
bool	AStringArray::Init(int nTotalUnit, int nUnitLen)
{
	int	nTotalBytes;
	nTotalBytes	= nTotalUnit * nUnitLen;
	m_pBuffer	= new char [nTotalBytes];
	if( !m_pBuffer )
	{
		assert(0);
		return	false;
	}
	memset(m_pBuffer, 0, nTotalBytes);
	m_nTotalUnit	= nTotalUnit;
	m_nUnitLen		= nUnitLen;
	return	true;
}
bool	AStringArray::Release()
{
	if( m_pBuffer )
	{
		delete		m_pBuffer;
		m_pBuffer	= NULL;
	}
	return	true;
}
char *	AStringArray::GetUnit(int nIdx)
{
	if( nIdx<0 || nIdx>=m_nTotalUnit )
	{
		return	NULL;
	}
	return	m_pBuffer + nIdx * m_nUnitLen;
}
void	AStringArray::ClearUnit(int nIdx)
{
	char	* pBuf;
	pBuf	= GetUnit(nIdx);
	if( pBuf )
	{
		memset(pBuf, 0, m_nUnitLen);
	}
}
bool	AStringArray::StrCpy(int nIdx, char *pszStr)
{
	char	* pBuf;
	pBuf	= GetUnit(nIdx);
	if( pBuf )
	{
		strncpy(pBuf, pszStr, m_nUnitLen-1);
	}
	return	true;
}

////////////////////////////////////////////////////////////////////
// ABinStringArray
////////////////////////////////////////////////////////////////////
bool	ABinStringArray::StrCpy(int nIdx, char *pszStr)
{
	char	* pBuf;
	pBuf	= GetUnit(nIdx);
	if( pBuf )
	{
		if( BinStrCpy(pBuf, m_nUnitLen, pszStr)<0 )
		{
			return	false;
		}
		return	true;
	}
	return	false;
}