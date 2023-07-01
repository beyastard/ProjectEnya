/*
 * FILE: AProperty.cpp
 *
 * DESCRIPTION: Implementation of class APropertyArray
  *
 * CREATED BY: JiangLi, 2002/08/08
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#include "AProperty.h"
/*
#define APROP_ARRAY_BLOCK	8

APropertyArray::APropertyArray()
{
	m_pProperties	= NULL;
	m_uCount		= 0;
}

APropertyArray::~APropertyArray()
{
	for(int i = 0; i < (int)m_uCount; i ++)
	{
		if(m_pProperties[i] != NULL)
		{
			delete m_pProperties[i];
		}
	}
	if(m_pProperties != NULL)
	{
		delete m_pProperties;
	}
}

void APropertyArray::AddProperty(AProperty* pProp)
{
	assert(pProp != NULL);
	if( (m_uCount % APROP_ARRAY_BLOCK) == 0 )
	{
		AProperty** pTemp	= new AProperty*[m_uCount + APROP_ARRAY_BLOCK];
		if(m_pProperties != NULL)
		{
			for(int i = 0; i < (int)m_uCount; i ++)
			{
				pTemp[i]	= m_pProperties[i];
			}
			delete m_pProperties;
		}
		m_pProperties	= pTemp;
	}
	m_pProperties[m_uCount]	= pProp;
	m_uCount ++;
}

AProperty* APropertyArray::GetPropByName(const char* pszName)
{
	for(int i = 0; i < (int)m_uCount; i ++)
	{
		if(lstrcmp(pszName, m_pProperties[i]->GetName()) == 0)
		{
			return m_pProperties[i];
		}
	}
	return NULL;
}

AProperty* APropertyArray::GetPropByIndex(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_uCount)
	{
		return NULL;
	}
	return m_pProperties[nIndex];
}

namespace APropGlobals
{
	ARangeTemplate<int> g_rngPercent(0, 100);
}
*/