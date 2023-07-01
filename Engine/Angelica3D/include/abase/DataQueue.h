// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: DataQueue.h
// Creator: Wei Hua (魏华)
// 数据队列

#ifndef	__DATAQUEUE_H__
#define	__DATAQUEUE_H__

#define	DATAQUEUE_ERROR_OVERFLOW				-1	// 队列满
#define	DATAQUEUE_ERROR_EMPTY					-2	// 队列空
#define	DATAQUEUE_ERROR_DATATOOBIG				-3	// 数据太大或者用于接受数据的缓冲区太小

#define	DATAQUEUE_DFT_MAXDATASIZE				255

class	DataQueue
{
protected:
	char	*m_pBuffer;								// 总的内存起始
	int		m_nTotalBytes;							// 物理上总共可以存放的字节数
	int		m_nLogicBytes;							// 逻辑上总共可以存放的字节数(用于最后有空闲的情况)
	int		m_nHead;								// 有效数据的起始位置
	int		m_nTail;								// 有效数据的结束位置
	int		m_nNum;									// 数据总条数
	int		m_nMaxDataSize;							// 最大允许的数据长度
protected:
	bool	TestIncTail(int nDelta);				// 测试增加到尾部是否还在容许范围内
													// 返回true时，m_nDataTail变为新的可增加的尾部位置(可能已经回绕变成0了)
													// 然后可以在这个尾部增加nDelta的数据一定不会超界
													// 返回false表示数据溢出
public:
	inline void	SetMaxDataSize(int nMax)			{ m_nMaxDataSize = nMax; }
public:
	DataQueue();
	~DataQueue();
	int		Init(int nTotalBytes);
	int		Release();
	int		Push(void *pData, int nSize);			// 压入
	int		PopQueue(void *pData, int *pnSize);
	int		PeekQueue(void **ppData, int *pnSize);
	int		DelQueueHead();
	int		ClearAll();
	int		GetNum();								// 获得目前队列中的数据条数
};

#endif