/*
 * FILE: AStack.cpp
 *
 * DESCRIPTION: A class which manage one stack;
 *
 * CREATED BY: Hedi, 2002/3/27
 *
 * HISTORY:
 *				   
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AStack.h"

AStack::AStack()
{
	m_pStackBuffer	= NULL;
	m_nStackSize	= 0;
	m_nStackPointer = 0;
}

AStack::~AStack()
{
}

bool AStack::Init(int nElementSize, int nStackSize)
{
	m_nElementSize = nElementSize;
	m_nStackSize   = nStackSize;

	m_pStackBuffer = (LPBYTE) malloc(m_nStackSize * m_nElementSize);
	if( NULL == m_pStackBuffer )
		return false;
	
	m_nStackPointer = 0;
	return true;
}

bool AStack::Release()
{
	if( m_pStackBuffer )
	{
		free(m_pStackBuffer);
		m_pStackBuffer = NULL;
	}
	return true;
}

