/*
 * FILE: AProcessor.h
 *
 * DESCRIPTION: 所有处理器的基类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef __APROCESSOR__H__
#define __APROCESSOR__H__

#include "AUnit.h"

class AGame;

class AProcessor : public AUnit  
{
protected:
	int			m_nActive;
public:
	virtual bool Release();
	DECLARE_ASERIAL(AProcessor)

	inline int GetActiveStatus() { return m_nActive;}
	virtual int SetActiveStatus(int __nActive) 
	{
		int bPre = m_nActive;
		m_nActive = __nActive;
		return bPre;
	}
	virtual bool Draw();
	AProcessor();
	virtual ~AProcessor();

};

#endif // __APROCESSOR__H__