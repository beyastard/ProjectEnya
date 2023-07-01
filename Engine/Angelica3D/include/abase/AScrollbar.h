/*
 * FILE: AScrollbar.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-3
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__ASCROLLBAR_H__
#define	__ASCROLLBAR_H__

#include "AWnd.h"
#include "AButton.h"

class AScrollButton : public AButton
{
public:
	DECLARE_ASERIAL(AScrollButton)
public:
	enum AScrollButtonStyle
	{
		ASBS_UP,
		ASBS_MID,
		ASBS_DOWN,
	};
protected:
	AScrollButtonStyle	m_sbsStyle;
	UINT				m_uFirstDown;
	UINT				m_uFirstDownDelay;
	UINT				m_uLastDown;
	UINT				m_uDownDelay;

	virtual bool	OnDraw();
	virtual bool	OnMouseMsg	(AWndMouseMsg& Msg);

	friend	class AScrollbar;
public:
	AScrollButton();
	virtual ~AScrollButton();
};


class AScrollbar : public AWnd
{
public:
	DECLARE_ASERIAL(AScrollbar)
public:
	enum AScrollStyle
	{
		ASS_HOR,
		ASS_VER,
	};
	enum AScrollMessage
	{
		ASM_SET_POSITION,
		ASM_PAGE_UP,
		ASM_PAGE_DOWN,
		ASM_STEP_UP,
		ASM_STEP_DOWN
	};

	typedef	bool (AObject::*AScrollHandler)(AScrollMessage ScrollMsg, INT nLength, INT& nCurPos);
protected:
	friend	class AScrollButton;
protected:
	AScrollButton*	m_psbUp;
	AScrollButton*	m_psbMid;
	AScrollButton*	m_psbDown;
	AScrollStyle	m_ssStyle;
	AScrollHandler	m_pOnScroll;
	INT				m_nScrollLength;
	INT				m_nCurrentPos;

	void	AdjustSize(ARECT& rUpArea, ARECT& rMidArea, ARECT& rDownArea);
	void	AdjustMid(ARECT& rMid);

	virtual	bool	Release();

	virtual bool	OnButtonClick(AScrollButton* pButton);
	virtual bool	OnCreate();
	virtual bool	OnDraw();
	virtual bool	OnMouseMsg	(AWndMouseMsg& Msg);
public:
	AScrollbar();
	virtual ~AScrollbar();

	virtual bool	SetArea(const ARECT& rArea);

	void SetScrollHandler(AScrollHandler pOnScroll) { m_pOnScroll = pOnScroll; }
	void SetScrollPos(int nPos);
	int  GetScrollPos();
	int  GetScrollLength() { return m_nScrollLength; }
};

#endif	// __ASCROLLBAR_H__