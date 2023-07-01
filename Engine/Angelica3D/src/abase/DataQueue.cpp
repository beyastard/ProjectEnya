// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: DataQueue.cpp
// Creator: Wei Hua (魏华)
// 数据队列

#include "DataQueue.h"
#include <string.h>

typedef		int			DQDS_T;				// 用于表示一条数据大小的数据类型
#define		DQDS_SIZE	sizeof(DQDS_T)

DataQueue::DataQueue()
{
	m_pBuffer		= NULL;
	m_nTotalBytes	= 0;
	m_nLogicBytes	= 0;
	m_nHead			= 0;
	m_nTail			= 0;
	m_nNum			= 0;
	m_nMaxDataSize	= DATAQUEUE_DFT_MAXDATASIZE;
}
DataQueue::~DataQueue()
{
	Release();
}
bool	DataQueue::TestIncTail(int nDelta)
{
	int		nTail;
	nTail	= m_nTail + nDelta;
	if( nTail >= m_nTotalBytes )
	{
		if( nDelta >= m_nHead )
		{
			// 无法加入了
			return	false;
		}
		m_nLogicBytes	= m_nTail;
		m_nTail			= 0;
	}
	else
	{
		if( m_nTail<m_nHead && nTail>=m_nHead )
		{
			// 无法加入了
			return	false;
		}
	}
	return	true;
}
int		DataQueue::Init(int nTotalBytes)
{
	m_pBuffer		= new char [nTotalBytes];
	if( !m_pBuffer )
	{
		return	-1;
	}

	m_nTotalBytes	= nTotalBytes;
	m_nLogicBytes	= m_nTotalBytes;
	m_nHead			= 0;
	m_nTail			= 0;
	m_nNum			= 0;

	return	0;
}
int		DataQueue::Release()
{
	if( m_pBuffer )
	{
		delete		m_pBuffer;
		m_pBuffer	= NULL;
	}
	return	0;
}
int		DataQueue::Push(void *pData, int nSize)
{
	if( nSize>m_nMaxDataSize )
	{
		return	DATAQUEUE_ERROR_DATATOOBIG;
	}
	if( !TestIncTail(nSize + DQDS_SIZE) )
	{
		return	DATAQUEUE_ERROR_OVERFLOW;
	}
	memcpy(m_pBuffer + m_nTail, &nSize, DQDS_SIZE);
	m_nTail	+= DQDS_SIZE;
	memcpy(m_pBuffer + m_nTail, pData, nSize);
	m_nTail	+= nSize;

	m_nNum	++;

	return	0;
}
int		DataQueue::PopQueue(void *pData, int *pnSize)
{
	int		rst;
	int		nSize;
	void	*pTmp;

	rst		= PeekQueue(&pTmp, &nSize);
	if( rst!=0 )
	{
		return	rst;
	}
	if( nSize>*pnSize )
	{
		return	DATAQUEUE_ERROR_DATATOOBIG;
	}
	if( pData )
	{
		memcpy(pData, pTmp, nSize);
	}
	*pnSize	= nSize;

	DelQueueHead();

	return	0;
}
int		DataQueue::PeekQueue(void **ppData, int *pnSize)
{
	if( m_nHead == m_nTail )
	{
		return	DATAQUEUE_ERROR_EMPTY;
	}

	*pnSize	= *(DQDS_T *)(m_pBuffer + m_nHead);
	*ppData	= m_pBuffer + m_nHead + DQDS_SIZE;

	return	0;
}
int		DataQueue::DelQueueHead()
{
	if( m_nHead == m_nTail )
	{
		return	DATAQUEUE_ERROR_EMPTY;
	}

	int		nSize;
	nSize	= *(DQDS_T *)(m_pBuffer + m_nHead);
	m_nHead	+= DQDS_SIZE + nSize;

	if( m_nHead == m_nTail )
	{
		m_nHead	= 0;
		m_nTail	= 0;
	}
	else if( m_nHead == m_nLogicBytes )
	{
		m_nHead			= 0;
		m_nLogicBytes	= m_nTotalBytes;
	}

	m_nNum	--;

	return	0;
}
int		DataQueue::ClearAll()
{
	m_nHead	= 0;
	m_nTail	= 0;
	return	0;
}
int		DataQueue::GetNum()
{
	return	m_nNum;
}
