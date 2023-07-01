/*
 * FILE: AWndConsole.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-5
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AWNDCONSOLE_H__
#define	__AWNDCONSOLE_H__

#include "AListBox.h"
#include "AEditPlus.h"
#include "AArray.h"
#include "AWC.h"

class AEditConsole : public AEditPlus
{
public:
	DECLARE_ASERIAL(AEditConsole)
protected:

	virtual LRESULT EditWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool	OnKeyboardMsg(AWndKeyboardMsg& Msg, bool& bAccept);
public:
	AEditConsole();
	virtual ~AEditConsole();

};

class AWndConsole : public AWnd
{
public:
	DECLARE_ASERIAL(AWndConsole)
protected:
	friend	class AEditConsole;

	AEditConsole	m_CommandInput;
	INT				m_nInputHeight;
	INT				m_nInputListSpace;
	AListBox		m_CommandList;
	AArrayString	m_CommandHistory;
	INT				m_nHistoryIndex;
	INT				m_nMarginHor;
	INT				m_nMarginVer;

	void	AdjustArea(ARECT& rInputArea, ARECT& rListArea);
	void	MoveHistory(int nStep);
	bool	AddLine(const char* pszStr);

	virtual bool	OnDraw();
	virtual bool	OnCreate();
	virtual void	OnDestroy();
	virtual bool	OnInputEnter();
	virtual bool	OnInputKey(AKeyboardButton akb, bool down);
	virtual bool	OnSetFocus(bool bFocus);
public:
	AWndConsole();
	virtual ~AWndConsole();


};


#endif	// __AWNDCONSOLE_H__