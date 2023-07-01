/*
 * FILE: AButton.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-8-27
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__ABUTTON_H__
#define	__ABUTTON_H__

#include "AWnd.h"
#include "AUString.h"

class AButton : public AWnd
{
public:
	DECLARE_ASERIAL(AButton)
protected:
	bool	m_bPushed;
	bool	m_bHovered;
	bool	m_bCheckedButton;
	bool	m_bContinuousClick;
	UINT	m_nLastClick;
	UINT	m_nFirstDelay;
	UINT	m_nClickDelay;
	UINT	m_nDblDelay;
	UINT	m_nDblFirst;


	virtual bool OnMouseMsg		(AWndMouseMsg& msg);
	virtual bool OnMouseHover	();
	virtual bool OnMouseLeave	();
	virtual bool OnClick		();
	virtual bool OnDblClick		();
	virtual bool OnDraw			();

public:
	AWndNotify	m_wnOnClick;
	AWndNotify	m_wnOnDblClick;

public:
	AButton();
	virtual ~AButton();

	void SetCheckedButton(bool b) { m_bCheckedButton	= b; }
	bool IsCheckedButton() { return m_bCheckedButton; }
	void Push(bool b) { m_bPushed = b; }
	bool IsPushed() { return m_bPushed; }
	void SetContinuousClick(bool b) { m_bContinuousClick = b; }
	bool IsContinuousClick() { return m_bContinuousClick; }
};


#endif	// __ABUTTON_H__