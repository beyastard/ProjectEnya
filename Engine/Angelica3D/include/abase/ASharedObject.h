/*
 * FILE: ASharedObject.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__ASHAREDOBJECT_H__
#define	__ASHAREDOBJECT_H__

#include "AObject.h"
#include "AUList.h"

class ASharedObject : public AObject
{
public:
	DECLARE_ASERIAL(ASharedObject)
protected:
	INT		m_nRefCount;

protected:
	typedef AListT<ASharedObject*> ASharedList;
	static ASharedList m_DefSharedList;

	virtual ASharedList& GetSharedList();
public:
	ASharedObject();
	virtual ~ASharedObject();

	virtual bool Release(){return true;}

	VOID	IncRef() { m_nRefCount ++; }
	VOID	DecRef();
};

template<typename TYPE>
class ASharedPtrT
{
protected:
	TYPE	m_pSharedObject;
public:

	ASharedPtrT():
	m_pSharedObject	(NULL)
	{
	}

	ASharedPtrT(const ASharedPtrT& sptr)
	{
		if(sptr.m_pSharedObject != NULL)
		{
			sptr.m_pSharedObject->IncRef();
		}
		m_pSharedObject	= sptr.m_pSharedObject;
	}

	ASharedPtrT(TYPE pObj):
	m_pSharedObject	(pObj)
	{
		if(pObj != NULL)
		{
			pObj->IncRef();
		}
	}

	void operator = (TYPE pObj)
	{
		if(pObj != NULL)
		{
			pObj->IncRef();
		}
		if(m_pSharedObject != NULL)
		{
			m_pSharedObject->DecRef();
		}
		m_pSharedObject	= pObj;
	}

	void operator = (const ASharedPtrT& sptr)
	{
		if(sptr.m_pSharedObject != NULL)
		{
			sptr.m_pSharedObject->IncRef();
		}
		if(m_pSharedObject != NULL)
		{
			m_pSharedObject->DecRef();
		}
		m_pSharedObject	= sptr.m_pSharedObject;
	}

	operator TYPE ()
	{
		return m_pSharedObject;
	}

	operator const TYPE () const
	{
		return m_pSharedObject;
	}

	TYPE operator -> ()
	{
		return m_pSharedObject;
	}

};

#endif	// __ASHAREDOBJECT_H__