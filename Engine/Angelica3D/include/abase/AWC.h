/*
 * FILE: AWC.h
 *
 * DESCRIPTION: 窗口管理类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:	JiangLi, 2002-08-26. 实现窗口管理类的各项功能。
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef __AWC__H__
#define __AWC__H__

#include "AProcessor.h"
#include "ATrans.h"
#include "AWndTypes.h"


class ADeviceKeyboard;
class ADeviceMouse;
class AWnd;
class A3DDevice;
class A2DSprite;

struct AWC_INIT
{
	AGame*	pGame;
	HWND	hMainWnd;

	AWC_INIT():
	pGame(NULL),
	hMainWnd(NULL)
	{
	};
};

enum AMouseButton
{
	AMB_LEFT,
	AMB_RIGHT,
	AMB_MIDDLE,
};

enum AKeyboardButton
{
	AKB_ESC			= 0x1B,
	AKB_ENTER		= 0x0D,
	AKB_LEFT		= 0x25,
	AKB_TOP			= 0x26,
	AKB_RIGHT		= 0x27,
	AKB_BOTTOM		= 0x28,
	AKB_CONTROL		= VK_CONTROL,
	AKB_BACK		= VK_BACK,
	AKB_HOME		= VK_HOME,
	AKB_END			= VK_END,
	AKB_INSERT		= VK_INSERT,
	AKB_DELETE		= VK_DELETE,
	AKB_RETURN		= VK_RETURN,
	AKB_UP			= VK_UP,
	AKB_DOWN		= VK_DOWN,

	AKB_A			= 'A',
	AKB_B			= 'B',
	AKB_C			= 'C',
	AKB_D			= 'D',
	AKB_E			= 'E',
	AKB_F			= 'F',
	AKB_G			= 'G',
	AKB_H			= 'H',
	AKB_I			= 'I',
	AKB_J			= 'J',
	AKB_K			= 'K',
	AKB_L			= 'L',
	AKB_V			= 'V',
};

class AWndMouseMsg
{
public:
	virtual void GetPos(APOINT& Pos) = 0;
	virtual void GetButtonStatus(AMouseButton amb, bool& bDown, bool& bChanged) = 0;
};

class AWndKeyboardMsg
{
public:
	virtual void GetButtonStatus(AKeyboardButton akb, bool& bDown, bool& bChanged) = 0;
	virtual void GetImeChars(AUString& strIme) = 0;
};

class AWC : public AProcessor  
{
public:
	DECLARE_ASERIAL(AWC)

protected:
	friend class AWndTransKeyboard;
	friend class AWndTransMouse;
	friend class AWnd;
protected:
	AWnd*	m_pLastHover;
	AWnd*	m_pFocus;
	AWnd*	m_pMainWnd;
	AWnd*	m_pActiveInterface;
	AWnd*	m_pCapture;
	ATrans*	m_pTransKeyboard;
	ATrans*	m_pTransMouse;
	AWndMouseMsg*		m_pCurrMouseMsg;
	AWndKeyboardMsg*	m_pCurrKeyboardMsg;
	HWND	m_hMainWnd;
	INT		m_nDefFont;
	float	m_fScaleX;
	float	m_fScaleY;

	static	AWC*	m_pWC;

	virtual bool DispatchKeyboardMsg(ADeviceKeyboard* pKeyboard);
	virtual bool DispatchMouseMsg(ADeviceMouse* pMouse);
	virtual void OnWndRelease(AWnd* pWnd);
	virtual bool CreateMainWnd();

	AWnd*	GetLastHover() { return m_pLastHover; }
	bool	SetLastHover(AWnd* pWnd);
	void	Resize		();
	AGame*	GetGame		() { return m_pAGame; }
public:
	static void GetFontSize(int nFont,int *pW,int *pH);
	AWC();
	virtual ~AWC();

	static AWC*	GetWC() { return m_pWC; }

	AWnd*			GetCapture	() { return m_pCapture; }
	void			SetCapture	(AWnd* pWnd) { m_pCapture = pWnd; }
	AWnd*			GetFocus	() { return m_pFocus; }
	bool			SetFocus	(AWnd* pWnd);
	AWndMouseMsg*	GetMouseMsg	() { return m_pCurrMouseMsg; }
	bool			IsKeyPressing	(AKeyboardButton akb);
	HWND			GetMainHandle() { return m_hMainWnd; }
	A3DDevice*		GetDevice();
	AWnd*			GetMainWnd	()	{ return m_pMainWnd; }
	float			GetScaleX()	{ return m_fScaleX; }
	float			GetScaleY()	{ return m_fScaleY; }

	virtual bool SetDisplayMode(int w, int h, int d);
	virtual bool Init			(const AWC_INIT& wci);
	virtual bool Draw			();
	virtual bool BindGame		(AGame * m_pG);
	virtual bool BindFinished	();
	virtual bool Release		();
	virtual int SetActiveStatus(int __bActive);

	virtual bool TickAnimation(float fTimeSpan);	//	Tick animation

	// Graph interfaces.

	static bool DrawSprite		(A2DSprite* pSprite, const APOINT& ptPos);
	static bool DrawText		(const APOINT& Pos, const char* pszStr, int nStrLen, UINT uColor, bool bShade = false);
	static bool DrawRectangle	(const ARECT& rRect, UINT uColor);
	static bool DrawTriangle	(const APOINT& p1, const APOINT& p2, const APOINT p3, UINT uColor);
	static bool DrawLine		(const APOINT& p1, const APOINT& p2, UINT uColor);
	static bool DrawPoint		(const APOINT& p , UINT uColor);

	static bool GetTextExtent	(const char* szText, int nTextLen, APOINT& Pos);
	static HFONT	GetCurrentFontHandle	();
	static int	GetCurrentFont	();
	static int	GetDefFont		() { return	GetWC()->m_nDefFont; }
	static bool	SetCurrentFont(int nFont);
};

#endif // __AWC__H__