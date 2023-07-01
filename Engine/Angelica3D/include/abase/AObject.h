/*
 * FILE: AObject.h
 *
 * DESCRIPTION: 派生动作和接口描述AObject
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef _AOBJECT_H__
#define _AOBJECT_H__

#include "AArchive.h"
#include "AClassInfo.h"
#include "ADebug.h"

//取得类名
#define CLASSNAME(name) #name


//-------Add by wangym------
#define CLASS_INFO(name)	&name::m_class##name
//--------------------------


/////格式: DECLARE_ASERIAL(classname) in .h
#define DECLARE_ASERIAL(name) \
	static AClassInfo m_class##name;\
	virtual AClassInfo * GetClassInfo(); \
	static name * CreateObject();

/////格式：IMPLEMENT_ASERIAL(classname, baseclassname, integer) in .cpp
#define IMPLEMENT_ASERIAL(name, base, version) \
	AClassInfo name::m_class##name(CLASSNAME(name), (AObject *(*)(void))name::CreateObject, &base::m_class##base, version);\
	name * name::CreateObject()\
	{\
		return new name();\
	}\
	AClassInfo * name::GetClassInfo()\
	{\
	return &name::m_class##name;\
	}

class AObject  
{
public:

	DECLARE_ASERIAL(AObject)
	virtual bool Load(AArchive &ar);
	virtual bool Save(AArchive &ar);
	bool IsKindOf(AClassInfo * pClassInfo);
	AObject();
	virtual ~AObject();
};

#endif
