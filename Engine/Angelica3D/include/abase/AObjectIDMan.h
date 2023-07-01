/*
 * FILE: AIDObject.h
 *
 * DESCRIPTION: 管理具有ID的对象的管理器的基类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef __AOBJECTIDMAN__H__
#define __AOBJECTIDMAN__H__

#include "AManager.h"
#include "AUList.h"

class AIDObject;
class AIDRec;
class AArchive;
class AClassInfo;
class AGame;


//ID <0 invalide >=0 valid
#define ID_OK(id)  (IsValidID(id))

class AObjectIDMan : public AManager  
{
protected:
	virtual bool OwnerLoad(AIDObject *__p, AArchive &ar, AGame *__pAGame);
	virtual bool OwnerProc(AIDObject * pAIDObject,float fSpanTime);
	virtual bool OnRelease(AIDObject * __pAIDObject);
	int					m_nNum;
	AIDRec	*			m_IDRec;
	int					m_objectID;

public:
	virtual int DeleteObject(AIDObject *__p);
	virtual bool Clear();
	bool LogicRun(float __fSpanTime);
	DECLARE_ASERIAL(AObjectIDMan)

	virtual int OnGetID(AIDObject * pAObject);
		
	bool Release();
	bool Init(AGame *__pAGame);

	bool Save(AArchive &ar);
	bool Load(AArchive &ar,AGame * __pAGame);

	AIDObject * GetPtrByID(int __nID);
	inline int GetTakedMaxID(){ return m_objectID;}

	bool IsValidID(int __nID);
	bool IsValidPtr(AIDObject * __ptr);

	
	int AddObject( AIDObject * pObject);
	bool DelObject(AIDObject * pObject);

	AObjectIDMan();
	virtual ~AObjectIDMan();
	
	inline int GetCount() { return m_nNum; }
};

#endif // __AOBJECTIDMAN__H__