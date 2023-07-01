/*
 * FILE: AObject.h
 *
 * DESCRIPTION: AObject的方法实现
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#include "AObject.h"
#include <assert.h>

AClassInfo AObject::m_classAObject(CLASSNAME(AObject), AObject::CreateObject, NULL, 1);

AObject::AObject()
{
}

AObject::~AObject()
{
}

AObject * AObject::CreateObject()
{
	return new AObject();
}

AClassInfo * AObject::GetClassInfo()
{
	return &m_classAObject;
}

bool AObject::Load(AArchive &ar)
{
	int nVersion;
	if(ar.Read(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}
	//利用这个版本号进行版本控制
	return true;
}

bool AObject::Save(AArchive &ar)
{
	int nVersion = AObject::GetClassInfo()->GetVersion();
	if(ar.Write(&nVersion,sizeof(nVersion))==false)
	{
		assert(0);
		return false;
	}
	return true;
}

// 对象是否派生自某类
bool AObject::IsKindOf(AClassInfo * pClassInfo)
{
	if(!pClassInfo) return false;
	AClassInfo * pInfo = GetClassInfo();

	while(pInfo)
	{
		if(pInfo == pClassInfo) return true;
		pInfo = pInfo->GetBaseClass();
	}

	return false;
}