/*
 * FILE: AArchive.cpp
 *
 * DESCRIPTION: 串行化动作类AArchive的方法实现
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#include "AArchive.h"
#include "AObject.h"
#include <assert.h>
#include <string.h>

AArchive::AArchive(AStream * pStream, bool bSave)
{
	assert(pStream);
	m_pStream = pStream;
	m_bMode = bSave;
}

AArchive::~AArchive()
{

}

bool AArchive::IsLoading()
{
	return !m_bMode;
}

bool AArchive::IsStoring()
{
	return m_bMode;
}

bool AArchive::Read(void *buff, int size)
{
	assert(m_pStream);
	return m_pStream->Read(buff, size);
}

bool AArchive::Write(const void *buff, int size)
{
	assert(m_pStream);
	return m_pStream->Write(buff, size);
}

AObject * AArchive::CreateObject()
{
	AObject * pObject = NULL;

	AClassInfo * pClassInfo = ReadClassInfo();
	assert(pClassInfo);

	// 从类信息创建类对象
	CREATEMETHOD pfCreateObject = pClassInfo->GetCreateMethod();
	assert(pfCreateObject);
	if(!pfCreateObject)
	{
		return NULL;
	}

	// 创建新对象
	pObject = pfCreateObject();
	if(!pObject)
	{
		return NULL;
	}

	return pObject;
}

bool AArchive::WriteClassInfo(AObject *pObject)
{
	assert(pObject);
	if(!pObject)
	{
		return false;
	}

	AClassInfo * pClassInfo = pObject->GetClassInfo();
	assert(pObject);
	if(!pClassInfo)
	{
		return false;
	}

	return WriteClassInfo(pClassInfo);
}

bool AArchive::WriteClassInfo(AClassInfo *pClassInfo)
{
	assert(pClassInfo);

	const char * pszClassName = pClassInfo->GetClassName();
	assert(pszClassName);
	if(!pszClassName)
	{
		return false;
	}

	AClassInfo * pValidClass = AClassInfo::GetClassInfo(pszClassName);
	assert(pValidClass);

	int namelen = strlen(pszClassName);
	assert(namelen > 0);
	if(namelen <= 0)
	{
		return false;
	}

	// 类名长度
	if(!Write(&namelen, sizeof(namelen)))
	{
		return false;
	}

	// 类名
	if(!Write(pszClassName, namelen))
	{
		return false;
	}

	// 版本
	int iVersion = pClassInfo->GetVersion();
	if(!Write(&iVersion, sizeof(iVersion)))
	{
		return false;
	}

	return true;
}

AClassInfo * AArchive::ReadClassInfo()
{
	// 类名称长度
	char * pszName = NULL;

	int namelen = 0;
	if(!Read(&namelen, sizeof(namelen)))
	{
		return NULL;
	}

	// 长度必须大于零
	if(namelen <= 0)
	{
		return NULL;
	}

	// 类名称
	pszName = new char[namelen + 1];
	if(!Read(pszName, namelen))
	{
		delete []pszName;
		return NULL;
	}
	pszName[namelen] = 0;

	// 版本
	int iVersion = 0;
	if(!Read(&iVersion, sizeof(iVersion)))
	{
		delete []pszName;
		return NULL;
	}

	// 根据类名取得类信息
	AClassInfo * pClassInfo = AClassInfo::GetClassInfo(pszName);
	assert(pClassInfo);
	if(!pClassInfo)
	{
		delete []pszName;
		return NULL;
	}
	delete []pszName;

	return pClassInfo;
}

bool AArchive::WriteString(AUString &str)
{
	int len = str.GetLength();
	
	Write(&len, sizeof(len));
	if(len > 0)
	{
		Write(str.GetBuffer(), len);
	}
	return true;
}

bool AArchive::ReadString(AUString &str)
{
	int len = 0;
	Read(&len, sizeof(len));
	if(len > 0)
	{
		char * buff = str.GetBufferSetLength(len);
		assert(buff);
		Read(buff, len);
	}

	return true;
}
