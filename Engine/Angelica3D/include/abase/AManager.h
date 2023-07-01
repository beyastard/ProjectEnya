/*
 * FILE: AUnit.h
 *
 * DESCRIPTION: 所有管理器的基类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#include "AUnit.h"
#include <assert.h>

#ifndef __AMANAGER__H__
#define __AMANAGER__H__


#define		MAN_SELF	-1
//manager ID,请从ID_MAN_GAME的后面添加
enum
{
	//这些值决定了写存盘文件和读存盘文件的顺序，所以不应该改变
	ID_MAN_BEGIN = 0,
	ID_MAN_CAMERA		= ID_MAN_BEGIN,
	ID_MAN_EVENT		= 1,
	ID_MAN_BULLET		= 2,
	ID_MAN_PLAY			= 3,
	ID_MAN_AI			= 4,
	ID_MAN_MATTER		= 5,
	ID_MAN_MSG			= 6,
	ID_MAN_END			= ID_MAN_MSG,

};

enum
{
	EVENT_MAN_MASK	= (1<<ID_MAN_EVENT),
	AI_MAN_MASK		= (1<<ID_MAN_AI),
	MATTER_MAN_MASK	= (1<<ID_MAN_MATTER),
	MSG_MAN_MASK	= (1<<ID_MAN_MSG), // Carefully use it 
	PLAY_MAN_MASK	= (1<<ID_MAN_PLAY),
	CAMERA_MAN_MASK	= (1<<ID_MAN_CAMERA),
	BULLET_MAN_MASK = (1<<ID_MAN_BULLET),
	INTERFACE_MASK	= (1<<31),
};

class AGame;
struct AMSG;
class AArchive;

class AManager : public AUnit  
{
protected:
	virtual bool OnLogicRun(float fTimeSpan);
	int		m_managerId;
public:
	virtual bool LogicRun_Begin(float time);
	virtual bool Save(AArchive& ar);
	virtual bool Load(AArchive& ar,AGame * p);
	virtual bool Clear();
	DECLARE_ASERIAL(AManager)
	AManager();
	virtual ~AManager();

	inline void SetManagerId(int __nId)
	{
		assert(__nId>=0 && __nId <= 255);
		m_managerId = __nId;
	}
	inline int GetManagerId()
	{
		return m_managerId;
	}

	//virtual 
	virtual int OnMsg(AMSG * pMSG);
	virtual bool LogicRun(float __TimeSpan);
	virtual bool Draw();
	virtual bool Release();
	virtual bool Init(AGame* __pAGame );

};

#endif // __AMANAGER__H__