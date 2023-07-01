/*
 * FILE: AStack.h
 *
 * DESCRIPTION: A class which manage one stack;
 *
 * CREATED BY: Hedi, 2002/3/27
 *
 * HISTORY:
 *				   
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ASTACK_H_
#define _ASTACK_H_

#include "A3DPlatform.h"

class AStack
{
private:
	int		m_nElementSize;		// Each element's size;
	int		m_nStackSize;		// How many elements can be held by the pool;

	LPBYTE	m_pStackBuffer;		// Pool Buffer;
	int		m_nStackPointer;	// A SP reference position;
protected:
public:
	AStack();
	~AStack();

	inline bool Push(LPVOID pElement);
	inline bool Pop(LPVOID pElement);

	bool Init(int nElementSize, int nStackSize=1024);
	bool Release();
	inline int GetStackSize() { return m_nStackSize; }
};

bool AStack::Push(LPVOID pElement)
{
	if( m_nStackPointer >= m_nStackSize )
	{
		m_nStackSize *= 2;
		m_pStackBuffer = (LPBYTE) realloc(m_pStackBuffer, m_nStackSize * m_nElementSize);
		if( NULL == m_pStackBuffer )
			return false;
	}

	memcpy(m_pStackBuffer + (m_nStackPointer ++) * m_nElementSize, pElement, m_nElementSize);
	return true;
}

bool AStack::Pop(LPVOID pElement)
{
	if( m_nStackPointer )
		memcpy(pElement, m_pStackBuffer + (-- m_nStackPointer) * m_nElementSize, m_nElementSize);
	else
		return false;

	return true;
}
typedef class AStack * PAStack;

#endif

