/*
 * FILE: AObjectIDListMan.h
 *
 * DESCRIPTION: 管理以连表方式管理的ID对象
 *
 * CREATED BY: 王远明, 2002/8/24
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */
#ifndef  _AOBJECTIDLISTMAN__H__
#define  _AOBJECTIDLISTMAN__H__

#include "AManager.h"
#include "AUList.h"

class AIDObject;

class AObjectIDListMan : public AManager  
{
protected:
	virtual bool OnRelease(AIDObject *p);
	virtual int OnGetID(AIDObject * __p);
	virtual bool OwnerDraw(AIDObject * pAIDObject);
	virtual bool OwnerProc(float __f,AIDObject * __pAIDObject,AListPosition __pos);
	virtual bool OwnerLoad(AIDObject * __p,AArchive& ar,AGame * __pAGame);
	AListT<AIDObject*>			m_list;

public:
	virtual bool DeleteObject(AIDObject *__p);
	virtual bool AddObject(AIDObject * __p);
	virtual AIDObject * GetPtrByIndex(int nIndex);
	virtual AIDObject * GetPtrByID(int id);
	virtual bool Clear();
	bool Draw();
	DECLARE_ASERIAL(AObjectIDListMan)

	virtual bool LogicRun(float __TimeSpan);
	virtual bool Release();
	bool Save(AArchive &ar);
	bool Load(AArchive &ar,AGame * __pAGame);
	AObjectIDListMan();
	virtual int GetCount();
	AListT<AIDObject*> *GetList() { return &m_list;}
	
	virtual ~AObjectIDListMan();

};

#endif // _AOBJECTIDLISTMAN__H__