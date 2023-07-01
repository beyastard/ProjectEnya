/*
 * FILE: AEdit.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-8-28
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AEDIT_H__
#define	__AEDIT_H__

#include "AWnd.h"
#include "AArray.h"

class AEditCursor
{
public:
	virtual bool OnDraw(const APOINT& AbPos, int nLineHeight, bool bSel = false) = 0;
	virtual void Update() = 0;
};

class AEdit : public AWnd
{
public:
	DECLARE_ASERIAL(AEdit)

public:

protected:
	bool			m_bMultiLine;
	int				m_nFirstVisibleLine;
	int				m_nLineHeight;
	int				m_nLineStep;
	int				m_nMarginLeft;
	int				m_nMarginTop;
	int				m_nMarginRight;
	int				m_nMarginBottom;
	UINT			m_uLastKeyDown;
	UINT			m_uLastKeyDownTime;
	UINT			m_uKeyDownEnumDelay;
	UINT			m_uLastKeyDownEnum;
	UINT			m_uKeyDownFirstDelay;

	UINT			m_uMarginColor;
	UINT			m_uSelTextColor;
	UINT			m_uSelBackColor;

	APOINT			m_ptSelStart;
	APOINT			m_ptSelEnd;
	APOINT			m_ptCurPos;
	
	AEditCursor*	m_pCursor;
	AArrayString	m_asStrings;

	void			CurMoveHor(int nStep);
	void			CurMoveVer(int nStep);
	void			CurMoveTo(const APOINT& ptNewPos);
	void			GetTextArea(const ARECT& rArea);
	const char*		GetCurrentLine();
	int				GetCurrentLineLength();
	bool			InsertChar(UINT uChar);
	bool			RemoveChar(int nDirection);
	bool			IsMultiByteChar(int nDirection);
	bool			NewLine();
	//int				GetCharCount(int nX, const char* pszStr, int nStrLen);

	virtual bool	OnEditPaste();
	virtual bool	OnEditKeyDown(UINT nKey);
	virtual bool	OnEditKeyPressing(UINT nKey, int nTimePassed);

	virtual bool	OnSetTextFont(int nFont);
	virtual	bool	OnCreate();
	virtual bool	OnKeyboardMsg	(AWndKeyboardMsg& Msg, bool& bAccept);
	virtual bool	OnDraw();
	virtual bool	OnDrawMargin(const APOINT& AbLeftTop);
	virtual bool	OnDrawBack(const APOINT& AbLeftTop);
	virtual bool	OnDrawText(const APOINT& AbPos, const char* pszText, int nText = -1, bool bSel = false);
public:
	AEdit();
	virtual ~AEdit();

	int		GetLineCount() { return m_asStrings.GetLength(); }
	bool	IsMultiLine() { return m_bMultiLine; }
	bool	SetMultiLine(bool bMultiLine) { m_bMultiLine	= bMultiLine; }
	void	AddString(const char* pszStr);
};

#endif	// __AEDIT_H__