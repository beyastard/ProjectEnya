/*
 * FILE: AVisualUnit.h
 *
 * DESCRIPTION: 具有可视属性的对象的基类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef __AVISUALUNIT__H__
#define __AVISUALUNIT__H__

#include "AUnit.h"

class A3DViewport;
class A3DEngine;
class AVisualUnit : public AUnit  , public i_render_target
{
public:

	A3DEngine *		m_pA3DEngine;

	DECLARE_ASERIAL(AVisualUnit)

	virtual bool LogicRun(float __TimeSpan);	
	virtual bool Render(A3DViewport * pCurrentViewport);
	virtual bool GetVisibility();
	virtual void SetVisibility(bool b);
	virtual int  OnMsg(PAMSG pMsg);
	virtual bool BindGame(AGame * m_pG);
	virtual bool BindFinished();
	virtual bool Release();
	virtual void EnableZBufferTest(bool b){}
	virtual bool IsZBufferTestEnabled(){return true;}
	AVisualUnit();
	virtual ~AVisualUnit();
};

#endif // __AVISUALUNIT__H__