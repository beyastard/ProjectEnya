/*
 * FILE: lAIDObject.h
 *
 * DESCRIPTION: 所有具有ID的对象的基类
 *
 * CREATED BY: 王远明 2002/08/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef __AIDOBJECT__H__
#define __AIDOBJECT__H__

#include "AObject.h"
#include "String.h"
#include "AUString.h"	// Added by ClassView
#pragma warning ( disable : 4786 )

class AData;
class AGame;
class AArchive;

#define INVALIDOBJECTID		-1
#define ISINVALIDID(id) ((id) < 0)

class AIDObject : public AObject  
{
public:

	virtual bool Release();
	virtual bool Save(AArchive &ar);
	virtual bool Load(AArchive &ar);

	DECLARE_ASERIAL(AIDObject)

	inline int GetId() {return m_nId;}
	inline void SetId(int __nId) {m_nId = __nId;}

	AIDObject();
	virtual ~AIDObject();

protected:

	int			m_nId;		// 对象ID
};

#endif // __AIDOBJECT__H__