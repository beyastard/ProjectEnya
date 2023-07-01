/*
 * FILE: AClassInfo.h
 *
 * DESCRIPTION: 定义所有依靠AObject实现串行化的描述类AClassInfo
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef _ACLASSINFO_H__
#define _ACLASSINFO_H__

#include "AList.h"

class AObject;
typedef AObject * (* CREATEMETHOD)(void);

class AClassInfo  
{
public:
	int GetVersion();
	CREATEMETHOD GetCreateMethod();
	AClassInfo * GetBaseClass();
	const char * GetClassName();
	AClassInfo(const char * pcszName, CREATEMETHOD pfCreate, AClassInfo * pBase, int iVersion);
	~AClassInfo();
	static AClassInfo * GetClassInfo(const char * pszClass);

private:

	const char *	m_pcszName;		//类名
	AClassInfo *	m_pciBase;		//基类信息
	int				m_iVersion;		//类串行化版本
	CREATEMETHOD	m_pfCreate;		//创建对象的函数指针

	AClassInfo *	m_pciPrev;		//上一个类信息
	AClassInfo *	m_pciNext;		//下一个类信息
};

#endif
