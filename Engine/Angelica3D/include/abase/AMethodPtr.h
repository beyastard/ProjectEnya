/*
 * FILE: AMethodPtr.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-4
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AMETHODPTR_H__
#define	__AMETHODPTR_H__

#include "AObject.h"

class AMethodPtrBase
{
public:
	typedef	void (AObject::*AMethodTypeBase)();
protected:
	AObject*		m_pObject;
	AMethodTypeBase	m_pMethod;

	AMethodPtrBase():
	m_pObject	(NULL),
	m_pMethod	(NULL)
	{
	}

public:
	bool IsNil()
	{
		return m_pObject == NULL || m_pMethod == NULL;
	}

	bool operator == (const AMethodPtrBase& ptr)
	{
		return m_pObject == ptr.m_pObject && m_pMethod == ptr.m_pMethod;
	}
};

template<typename TYPE_RET>
class AMethodPtr0T : public AMethodPtrBase
{
public:
	typedef	TYPE_RET (AObject::*AMethodType)();
public:

	void SetValue(AObject* pObject, AMethodType pMethod)
	{
		m_pObject	= pObject;
		m_pMethod	= (AMethodTypeBase)pMethod;
	}

	TYPE_RET operator () ()
	{
		return (((AObject*)m_pObject)->*(AMethodType)(m_pMethod))();
	}
};

template<typename TYPE_RET, typename TYPE1>
class AMethodPtr1T : public AMethodPtrBase
{
public:
	typedef	TYPE_RET (AObject::*AMethodType)(TYPE1);
public:

	void SetValue(AObject* pObject, AMethodType pMethod)
	{
		m_pObject	= pObject;
		m_pMethod	= (AMethodTypeBase)pMethod;
	}

	TYPE_RET operator () (TYPE1 param1)
	{
		return (((AObject*)m_pObject)->*(AMethodType)(m_pMethod))(param1);
	}
};


#endif	// __AMETHODPTR_H__