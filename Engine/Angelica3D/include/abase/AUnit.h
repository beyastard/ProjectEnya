/*
 * FILE: AUnit.h
 *
 * DESCRIPTION: 所有具有逻辑动作的对象的基类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef __AUNIT__H__
#define __AUNIT__H__

#include "AIDObject.h"
#include "AData.h"
#include "ABaseA3D.h"
#include "AUnitInWorldInterface.h"

struct AMSG;
typedef AMSG *PAMSG;
class AEvent;
class AGame;
//class AListT;

#define VERIFY_OBJECT_TYPE(pObject, typeClass) \
	(typeClass*)VerifyObjectType(pObject, CLASS_INFO(typeClass), __FILE__, __LINE__)

void* VerifyObjectType(AObject* pObject,
	AClassInfo* pInfo, LPCSTR pszFile, int nLine);

class AUnit : public AIDObject  
{
public:
	DECLARE_ASERIAL(AUnit)
protected:

	AGame *			m_pAGame;
	unsigned long	m_iTicks;

	// By JiangLi
	// 创建一个派生于AData的对象，如果子类进行读写数据
	// 的操作，就必须重载这个函数创建合适的数据对象。
	virtual AData* CreateData();
	// 把数据写入到数据对象里。通常保存一个对象的时候需要执行这
	// 个函数。保存的数据应该是能够用来完全恢复自身的。
	virtual bool OnWriteData(AData* pData);
	// 从数据对象中读取数据。这个数据有可能来源于流，或者来源于
	// 内存中的数据；也有可能是更新数据的过程。所以最好不要把这
	// 个函数仅仅当做初始化的过程。
	virtual bool OnReadData(AData* pData);
public:
	virtual bool IsDel();
	virtual bool IsDead();

	AUnit();
	virtual ~AUnit();
	virtual bool Init(AData *_pData,AGame * _pAGame);
	virtual bool Load(AArchive &ar,AGame * __pAGame);
	virtual bool Save(AArchive &ar);
	virtual int  OnMsg(PAMSG pMsg);
	virtual bool LogicRun(float __TimeSpan);
	virtual bool Release();
	virtual bool BindGame(AGame * m_pG);
	virtual bool BindFinished();	
	unsigned long& AccessTickCount() { return m_iTicks;}
	inline AGame* GetAGame() { return m_pAGame;}
};

#endif // __AIDOBJECT__H__