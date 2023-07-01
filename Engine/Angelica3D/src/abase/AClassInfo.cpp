/*
 * FILE: AClassInfo.cpp
 *
 * DESCRIPTION: 实现AClassInfo类的方法
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#include "AClassInfo.h"
#include "AObject.h"
#include <assert.h>
#include <string.h>

static AClassInfo * global_classinfos = NULL;

AClassInfo::AClassInfo(const char * pcszName, CREATEMETHOD pfCreate, AClassInfo * pBase , int iVersion)
{
	assert(pcszName);
	assert(pfCreate);

	m_pcszName	= pcszName;
	m_pfCreate	= pfCreate;
	m_pciBase	= pBase;
	m_iVersion	= iVersion;

	m_pciPrev	= global_classinfos;
	if(global_classinfos)
	{
		global_classinfos->m_pciNext = this;
	}
	m_pciNext	= NULL;
	global_classinfos = this;
}

AClassInfo::~AClassInfo()
{

}

AClassInfo * AClassInfo::GetClassInfo(const char * pszClass)
{
	assert(pszClass);
	AClassInfo * pInfo = global_classinfos;
	while(pInfo)
	{
		if(strcmp(pInfo->m_pcszName, pszClass) == 0)
		{
			return pInfo;
		}
		pInfo = pInfo->m_pciPrev;
	}

	return NULL;
}

const char * AClassInfo::GetClassName()
{
	return m_pcszName;
}

AClassInfo * AClassInfo::GetBaseClass()
{
	return m_pciBase;
}

CREATEMETHOD AClassInfo::GetCreateMethod()
{
	return m_pfCreate;
}

int AClassInfo::GetVersion()
{
	return m_iVersion;
}
