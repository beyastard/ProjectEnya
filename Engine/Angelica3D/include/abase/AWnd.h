/*
 * FILE: AWnd.h
 *
 * DESCRIPTION: 窗口类
 *
 * CREATED BY: JiangLi, 2002-8-26
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AWND_H__
#define	__AWND_H__

#include "AVisualUnit.h"
#include "AUList.h"
#include "AUString.h"
#include "AWndTypes.h"
#include "AMethodPtr.h"
#include "AWndIniFile.h"

// AVisualUnit
	class AWnd;

class A3DStringSet;
class ADeviceKeyboard;
class ADeviceMouse;
class AWndMouseMsg;
class AWndKeyboardMsg;

typedef AMethodPtr1T<bool, AWnd*> AWndNotify;

class AWnd : public AVisualUnit
{
public:
	DECLARE_ASERIAL(AWnd)
protected:
	friend class AWC;
protected:
	bool			m_bCreated;
	bool			m_bEnabled;
	bool			m_bVisible;
	bool			m_bCanBeFocus;
	bool			m_bKeyPreview;
	bool			m_bOwnerDraw;
	int				m_nTextFont;
	UINT			m_uTextColor;
	UINT			m_uBackColor;
	ARECT			m_rArea;
	AWnd*			m_pParent;
	AWnd*			m_pActiveChild;
	AListT<AWnd*>	m_lstChilds;
	AUString		m_strToolTip;
	A3DStringSet*	m_pssToolTip;

	bool AddChild		(AWnd* pWnd);
	bool RemoveChild	(AWnd* pWnd);
	void TabActiveChild	();

protected:

	// Message handlers

	virtual bool OnCreate	();
	virtual void OnDestroy	();
	virtual bool OnSize		();
	virtual bool OnDraw		();
	virtual bool OnSetFocus	(bool bFocus);
	virtual bool OnMouseMsg		(AWndMouseMsg& Msg);
	virtual bool OnKeyboardMsg	(AWndKeyboardMsg& Msg, bool& bAccept);
	virtual bool OnMouseHover	();
	virtual bool OnMouseLeave	();
	virtual bool OnMouseDown	(const APOINT& ptClick);
	virtual bool OnSetTextFont	(int nFont);

public:
	AWnd();
	virtual ~AWnd();

	bool	IsActived	();
	bool	IsAncestorOf(AWnd* pDescendant);
	bool	IsEnabled	() { return m_bEnabled; }
	bool	IsVisible	() { return m_bVisible; }
	bool	IsCanBeFocus() { return m_bCanBeFocus; }
	void	SetCanBeFocus(bool bCanBeFocus = true) { m_bCanBeFocus	= bCanBeFocus; }
	void	SetOwnerDraw(bool bOwnerDraw) { m_bOwnerDraw = bOwnerDraw; }
	bool	GetOwnerDraw() { return m_bOwnerDraw; }
	bool	EnableWnd	(bool bEnabled) { m_bEnabled = bEnabled; return true;}
	AWnd*	GetParent	() { return m_pParent; }
	bool	SetParent	(AWnd* pParent);
	bool	SetCapture	(bool bCapMouse);
	AWnd*	FindChild	(const APOINT& pt);
	int		GetTextFont	() { return m_nTextFont; }
	bool	SetTextFont	(int nFont) { return OnSetTextFont(nFont); }
	UINT	GetBackColor() { return m_uBackColor; }
	bool	SetBackColor(UINT uColor) { m_uBackColor = uColor; return true;}
	UINT	GetTextColor() { return m_uTextColor; }
	bool	SetTextColor(UINT uColor) { m_uTextColor = uColor; return true;}
	const	ARECT& GetArea	() { return m_rArea; }
	void	GetAbsoluteArea	(ARECT& rArea);
	void	GetAbsolutePos	(APOINT& ptPos);

	virtual bool TickAnimation(float fTimeSpan);		//	Tick animation

	// virtual methods.

	virtual bool Release	();
	virtual bool IsTabStoped();
	virtual bool Redraw		();
	virtual bool LoadFromIni(AWnd* pParent, AWndIniFile& IniFile, const char* pszSection);
	virtual bool LoadRes	();
	virtual bool FreeRes	();
	virtual bool ShowWindow	(bool bShow);
	virtual bool PointIn	(const APOINT& pt);
	virtual	bool MoveTo		(const APOINT& ptNewPos);
	virtual bool SetArea	(const ARECT& rArea);
	virtual bool SetAsActive(bool bActive);
	virtual bool Create		(AWnd* pParent, const ARECT& rArea);
	virtual bool ProcessMouseMsg(AWndMouseMsg& Msg);
	virtual bool ProcessKeyboardMsg(AWndKeyboardMsg& Msg, bool& bAccept);
};

#endif	// __AWND_H__