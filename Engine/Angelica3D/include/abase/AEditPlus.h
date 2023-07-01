/*
 * FILE: AEditPlus.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-2
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AEDITPLUS_H__
#define	__AEDITPLUS_H__

#include "AWnd.h"

class AEditPlusCursor
{
public:
	virtual bool OnDraw(const APOINT& AbPos, int nLineHeight, bool bSel = false) = 0;
	virtual void Update() = 0;
	virtual void SetColor(UINT nColor) = 0;
};

class AEditPlus : public AWnd
{
public:
	DECLARE_ASERIAL(AEditPlus)
protected:
	UINT			m_uSelTextColor;
	UINT			m_uSelBackColor;
	HWND			m_hEdit;
	int				m_nFont;
	APOINT			m_ptLastLButtonUp;
	APOINT			m_ptCursorPos;
	AEditPlusCursor*	m_pCursor;
	HIMC			m_hImc;
	INT				m_nLastSelStart;
	INT				m_nLastSelEnd;
	BOOL			m_bEditLeftTop;

	friend  LRESULT CALLBACK EditWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT EditWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual bool	DrawLines	(const ARECT& AbArea);
	virtual	bool	OnCreate	();
	virtual	void	OnDestroy	();
	virtual bool	OnSetFocus	(bool bFocus);
	virtual bool	OnDraw		();
	virtual bool	OnMouseMsg	(AWndMouseMsg& Msg);
	virtual bool	OnKeyboardMsg(AWndKeyboardMsg& Msg, bool& bAccept);

public:
	AEditPlus();
	virtual ~AEditPlus();

	INT	GetLine(INT nIndex, char* pszBuf, INT nBufLength);
	INT GetLineCount();
	void SetSel(INT nSelStart = 0, INT nSelEnd = -1);
	void ReplaceSel(const char* pszStr);
	void SetCursorColor(UINT uColor);
};


#endif	// __AEDITPLUS_H__