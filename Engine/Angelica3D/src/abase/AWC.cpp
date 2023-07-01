#include "a3dgdi.h"
#include "AWC.h"
#include "AGame.h"
#include "AGamePlay.h"
#include "ATrans.h"
#include "ADeviceKeyboard.h"
#include "ADeviceMouse.h"
#include "AWnd.h"
#include "ABaseA3D.h"
#include "A3DAssistTools.h"

class AWndTransKeyboard : public ATrans
{
protected:
	AWC*	m_pWC;
public:
	AWndTransKeyboard(AWC * pWC);
	virtual bool TranslateIn(ADevice *pADevice);
	virtual bool TranslateOut(ADevice *pADevice);
};

class AWndTransMouse : public ATrans
{
protected:
	AWC*	m_pWC;
public:
	AWndTransMouse(AWC * pWC);
	virtual bool TranslateIn(ADevice *pADevice);
	virtual bool TranslateOut(ADevice *pADevice);
};

class AWndMouseMsgFromDev : public AWndMouseMsg
{
protected:
	ADeviceMouse*	m_pDevMouse;
	AWC*	m_pWC;
public:
	AWndMouseMsgFromDev(AWC * pWC, ADeviceMouse* pDev = NULL);

	virtual void GetPos(APOINT& Pos);
	virtual void GetButtonStatus(AMouseButton amb, bool& bDown, bool& bChanged);
	virtual void SetDevice(ADeviceMouse* pDev);
};

class AWndKeyboardMsgFromDev : public AWndKeyboardMsg
{
protected:
	ADeviceKeyboard*	m_pDevKeyboard;
	AWC*	m_pWC;
public:
	AWndKeyboardMsgFromDev(AWC * pWC, ADeviceKeyboard* pDev = NULL);

	virtual void GetButtonStatus(AKeyboardButton akb, bool& bDown, bool& bChanged);
	virtual void GetImeChars(AUString& strIme);
	virtual void SetDevice(ADeviceKeyboard* pDev);
};

AWndKeyboardMsgFromDev::AWndKeyboardMsgFromDev(AWC * pWC, ADeviceKeyboard* pDev):
m_pDevKeyboard	(NULL),
m_pWC		(pWC)
{
}

void AWndKeyboardMsgFromDev::GetButtonStatus(
	AKeyboardButton akb, bool& bDown, bool& bChanged)
{
	if(m_pDevKeyboard == NULL)
	{
		bDown		= false;
		bChanged	= false;
		ADEBUG_TRACE("AWndKeyboardMsgFromDev::GetButtonStatus error!");
		return;
	}
	if(m_pDevKeyboard->VK_FirstPress(akb))
	{
		bDown		= true;
		bChanged	= true;
	}
	else if(m_pDevKeyboard->VK_LastPress(akb))
	{
		bDown		= false;
		bChanged	= true;
	}
	else if(m_pDevKeyboard->VK_Pressing(akb))
	{
		bDown		= true;
		bChanged	= false;
	}
	else
	{
		bDown		= false;
		bChanged	= false;
	}
}

void AWndKeyboardMsgFromDev::GetImeChars(AUString& strIme)
{
	if(m_pDevKeyboard == NULL)
	{
		strIme	= NULL;
		ADEBUG_TRACE("AWndKeyboardMsgFromDev::GetImeChars error!");
		return;
	}
	char buf[256];
	m_pDevKeyboard->GetChars(buf, sizeof(buf));
	strIme	= buf;
}

void AWndKeyboardMsgFromDev::SetDevice(ADeviceKeyboard* pDev)
{
	m_pDevKeyboard	= pDev;
}

// class AWndTransKeyboard

AWndTransKeyboard::AWndTransKeyboard(AWC * pWC):
m_pWC		(pWC)
{
}

bool AWndTransKeyboard::TranslateIn(ADevice *pADevice)
{
	ADeviceKeyboard* pDev = (ADeviceKeyboard*)pADevice;

	m_pWC->DispatchKeyboardMsg(pDev);

	return true;
}

bool AWndTransKeyboard::TranslateOut(ADevice *pADevice)
{
	return true;
}

// class AWndTransMouse

AWndTransMouse::AWndTransMouse(AWC * pWC):
m_pWC		(pWC)
{
}

bool AWndTransMouse::TranslateIn(ADevice *pADevice)
{
	return true;
}

bool AWndTransMouse::TranslateOut(ADevice *pADevice)
{
	ADeviceMouse* pDev = (ADeviceMouse*)pADevice;
	m_pWC->DispatchMouseMsg(pDev);
	return true;
}

// class AWndMouseMsgFromDev

AWndMouseMsgFromDev::AWndMouseMsgFromDev(AWC * pWC, ADeviceMouse* pDev):
m_pDevMouse	(pDev),
m_pWC		(pWC)
{

}

void AWndMouseMsgFromDev::GetPos(APOINT& Pos)
{
	if(m_pDevMouse == NULL)
	{
		::GetCursorPos(&Pos);
		::ScreenToClient(m_pWC->GetMainHandle(), &Pos);
		//ADEBUG_TRACE("AWndMouseMsgFromDev::GetPos error!");
		Pos.x	= (int)(Pos.x/AWC::GetWC()->GetScaleX());
		Pos.y	= (int)(Pos.y/AWC::GetWC()->GetScaleY());
		return;
	}

	m_pDevMouse->GetCursorPos((int*)&Pos.x, (int*)&Pos.y);

	// Used for test begin.
	::GetCursorPos(&Pos);
	//::SetCursorPos(Pos.x, Pos.y);
	//mouse_event(MOUSEEVENTF_ABSOLUTE, Pos.x-2, Pos.y-2, 0, 0);

	::ScreenToClient(m_pWC->GetMainHandle(), &Pos);
	// Used for test end.

	Pos.x	= (int)(Pos.x/AWC::GetWC()->GetScaleX());
	Pos.y	= (int)(Pos.y/AWC::GetWC()->GetScaleY());
}

void AWndMouseMsgFromDev::GetButtonStatus(AMouseButton amb, bool& bDown, bool& bChanged)
{
	if(m_pDevMouse == NULL)
	{
		bDown		= false;
		bChanged	= false;
		ADEBUG_TRACE("AWndMouseMsgFromDev::GetButtonStatus error!");
		return;
	}
	int btn = 0;
	switch(amb)
	{
	case AMB_LEFT:
		btn	= ADMOUSE_VK_LBUTTON;
		break;
	case AMB_RIGHT:
		btn	= ADMOUSE_VK_RBUTTON;
		break;
	case AMB_MIDDLE:
		btn	= ADMOUSE_VK_MBUTTON;
		break;
	default:
		ADEBUG_TRACE("Bad AMouseButton parameter!");
		bDown		= false;
		bChanged	= false;
	}
	if(m_pDevMouse->VK_FirstPress(btn))
	{
		bDown		= true;
		bChanged	= true;
	}
	else if(m_pDevMouse->VK_LastPress(btn))
	{
		bDown		= false;
		bChanged	= true;
	}
	else if(m_pDevMouse->VK_Pressing(btn))
	{
		bDown		= true;
		bChanged	= false;
	}
	else
	{
		bDown		= false;
		bChanged	= false;
	}
}

void AWndMouseMsgFromDev::SetDevice(ADeviceMouse* pDev)
{
	m_pDevMouse	= pDev;
}

// class AWC

IMPLEMENT_ASERIAL(AWC,AProcessor,0x0100)

AWC*	AWC::m_pWC				= NULL;

AWC::AWC():
m_pTransKeyboard	(NULL),
m_pTransMouse		(NULL),
m_pCurrMouseMsg		(NULL),
m_pCurrKeyboardMsg	(NULL),
m_pLastHover		(NULL),
m_pFocus			(NULL),
m_pMainWnd			(NULL),
m_pCapture			(NULL),
m_hMainWnd			(NULL),
m_pActiveInterface	(NULL),
m_nDefFont			(0),
m_fScaleX			(1.0f),
m_fScaleY			(1.0f)
{
	// Make sure to use only one AWC.
	//assert(m_pWC == NULL);
	m_pWC	= this;

	m_pWC = this;
	m_pTransKeyboard	= new AWndTransKeyboard(this);
	m_pTransMouse		= new AWndTransMouse(this);
	m_pCurrMouseMsg		= new AWndMouseMsgFromDev(this);
	m_pCurrKeyboardMsg	= new AWndKeyboardMsgFromDev(this);

}

AWC::~AWC()
{
	m_pWC	= this;
	if(m_pTransKeyboard != NULL)
	{
		m_pTransKeyboard->Release();
		delete m_pTransKeyboard;
	}
	if(m_pTransMouse != NULL)
	{
		m_pTransMouse->Release();
		delete m_pTransMouse;
	}
	if(m_pCurrMouseMsg != NULL)
	{
		delete m_pCurrMouseMsg;
	}
	if(m_pCurrKeyboardMsg != NULL)
	{
		delete m_pCurrKeyboardMsg;
	}
	m_pWC	= NULL;
}

bool AWC::SetDisplayMode(int w, int h, int d)
{
	m_pWC	= this;

	A3DFORMAT formatTarget	= (d > 16) ? A3DFMT_X8R8G8B8 : A3DFMT_R5G6B5;
	if(!m_pAGame->GetA3DEngine()->SetDisplayMode(
		w, h, formatTarget, false, SDM_WIDTH | SDM_HEIGHT | SDM_FORMAT))
	{
		ADEBUG_TRACE3("Cannot set display mode %dx%d %dbit.", w, h, d);
		return false;
	}

	m_fScaleX	= (float)w / 800.0f;
	m_fScaleY	= (float)h / 600.0f;

	return true;
}

A3DDevice* AWC::GetDevice()
{
	return m_pAGame->GetA3DEngine()->GetA3DDevice();
}

bool AWC::CreateMainWnd()
{
	m_pWC	= this;
	assert(m_pMainWnd == NULL);

	m_pMainWnd	= new AWnd;

	assert(m_pMainWnd != NULL);

	ARECT r(0, 0, 800, 600);

	return m_pMainWnd->Create(NULL, r);
}

bool AWC::Init(const AWC_INIT& wci)
{
	m_pWC	= this;
	TRY_METHOD(
		BindGame(wci.pGame),
		"Cannot bind game in AWC.");

	m_hMainWnd	= m_pAGame->GetA3DEngine()->GetA3DDevice()->GetDeviceWnd();//::GetForegroundWindow();
	assert(m_hMainWnd != NULL);

	if(!CreateMainWnd())
	{
		ADEBUG_TRACE("Cannot create MainWnd.");
		return false;
	}

	Resize();
	return true;
}

void AWC::Resize()
{
	m_pWC	= this;
	ARECT r(0, 0, 800, 600);
	m_pMainWnd->SetArea(r);
}

bool AWC::Draw()
{
	m_pWC	= this;
	if(m_pMainWnd != NULL)
	{
		AWC::SetCurrentFont(AWC::GetDefFont());

		//if(GetActiveStatus())
		//ADEBUG_TIME_TEST(m_pMainWnd->Redraw());
		return m_pMainWnd->Redraw();
	}
	return true;
}

bool AWC::Release()
{
	m_pWC	= this;
	if(m_pMainWnd != NULL)
	{
		m_pMainWnd->Release();
		delete m_pMainWnd;
		m_pMainWnd	= NULL;
	}
	return true;
}

bool AWC::BindGame(AGame* pG)
{
	m_pWC	= this;
	return AProcessor::BindGame(pG);
}

bool AWC::BindFinished()
{
	m_pWC	= this;
	return AProcessor::BindFinished();
}

void AWC::OnWndRelease(AWnd* pWnd)
{
	m_pWC	= this;
	if(m_pFocus == pWnd)
	{
		m_pFocus		= NULL;
	}
	if(m_pLastHover == pWnd)
	{
		m_pLastHover	= pWnd;
	}
	if(m_pCapture == pWnd)
	{
		m_pCapture		= pWnd;
	}
}

int AWC::SetActiveStatus(int nActive)
{
	m_pWC	= this;
	int nPre = AProcessor::SetActiveStatus(nActive);
	if(nPre == nActive)
	{
		return nPre;
	}

	return nPre;
}

bool AWC::TickAnimation(float fTimeSpan)
{
	if (m_pActiveInterface)
		m_pActiveInterface->TickAnimation(fTimeSpan);

	return true;
}

bool AWC::SetLastHover(AWnd* pWnd)
{
	m_pWC	= this;
	if(pWnd == m_pLastHover)
	{
		return true;
	}
	if(m_pLastHover != NULL)
	{
		if(!m_pLastHover->OnMouseLeave())
		{
			return false;
		}
	}
	m_pLastHover	= pWnd;
	if(m_pLastHover != NULL)
	{
		if(!m_pLastHover->OnMouseHover())
		{
			return false;
		}
	}
	return true;
}

bool AWC::SetFocus(AWnd* pWnd)
{
	m_pWC	= this;
	if(pWnd == m_pFocus)
	{
		return true;
	}
	AWnd*	pOldFocus	= m_pFocus;
	m_pFocus	= pWnd;
	if(pWnd != NULL)
	{
		if(!pWnd->IsCanBeFocus())
		{
			m_pFocus = pOldFocus;
			return false;
		}
		if(pOldFocus != NULL)
		{
			if(!pOldFocus->OnSetFocus(false))
			{
				m_pFocus	= pOldFocus;
				return false;
			}
		}
		if(!pWnd->OnSetFocus(true))
		{
			m_pFocus = NULL;
			return false;
		}
	}
	else
	{
		if(pOldFocus != NULL)
		{
			pOldFocus->OnSetFocus(false);
		}
	}

	return true;
}

bool AWC::DrawRectangle(const ARECT& rRect, UINT uColor)
{
	ARECT r(
		(int)(rRect.left*AWC::GetWC()->GetScaleX()),
		(int)(rRect.top*AWC::GetWC()->GetScaleY()),
		(int)(rRect.right*AWC::GetWC()->GetScaleX()),
		(int)(rRect.bottom*AWC::GetWC()->GetScaleY()));
	return g_pA3DGDI->Draw2DRectangle(*((A3DRECT*)&r), (A3DCOLOR)uColor);
}

bool AWC::DrawTriangle(const APOINT& p1, const APOINT& p2, const APOINT p3, UINT uColor)
{
	A3DPOINT2	v1((int)(p1.x*AWC::GetWC()->GetScaleX()),(int)(p1.y*AWC::GetWC()->GetScaleY()));
	A3DPOINT2	v2((int)(p2.x*AWC::GetWC()->GetScaleX()),(int)(p2.y*AWC::GetWC()->GetScaleY()));
	A3DPOINT2	v3((int)(p3.x*AWC::GetWC()->GetScaleX()),(int)(p3.y*AWC::GetWC()->GetScaleY()));

	return g_pA3DGDI->Draw2DTriangle(v1, v2, v3, (A3DCOLOR)uColor);
//	return g_pA3DGDI->Draw2DTriangle(
//		*((A3DPOINT2*)&p1),
//		*((A3DPOINT2*)&p2),
//		*((A3DPOINT2*)&p3),
//		(A3DCOLOR)uColor);
}

bool AWC::DrawLine(const APOINT& p1, const APOINT& p2, UINT uColor)
{
	A3DPOINT2	v1((int)(p1.x*AWC::GetWC()->GetScaleX()),(int)(p1.y*AWC::GetWC()->GetScaleY()));
	A3DPOINT2	v2((int)(p2.x*AWC::GetWC()->GetScaleX()),(int)(p2.y*AWC::GetWC()->GetScaleY()));

	return g_pA3DGDI->Draw2DLine(v1, v2, (A3DCOLOR)uColor);

//	return g_pA3DGDI->Draw2DLine(
//		*((A3DPOINT2*)&p1),
//		*((A3DPOINT2*)&p2),
//		(A3DCOLOR)uColor);
}

bool AWC::DrawPoint(const APOINT& p , UINT uColor)
{
	APOINT pt((int)(p.x*AWC::GetWC()->GetScaleX()),
		(int)(p.y*AWC::GetWC()->GetScaleY()));
	return g_pA3DGDI->Draw2DPoint(
		*((A3DPOINT2*)&pt),
		(A3DCOLOR)uColor,
		1.0f);
}

bool AWC::DrawSprite(A2DSprite* pSprite, const APOINT& ptPos)
{
//	ARECT rRect(0, 0, 800, 600);
//	::Sleep(5);
//	ADEBUG_TIME_TEST(g_pA3DGDI->Draw2DRectangle(*((A3DRECT*)&rRect), (A3DCOLOR)0xFF000000));
//	::Sleep(5);
//	ADEBUG_TIME_TEST(pSprite->DrawToBack(ptPos.x, ptPos.y));
//	return true;
	APOINT pt(
		(int)(ptPos.x*AWC::GetWC()->GetScaleX()),
		(int)(ptPos.y*AWC::GetWC()->GetScaleY()));
//	pSprite->SetScaleX(AWC::GetWC()->GetScaleX());
//	pSprite->SetScaleY(AWC::GetWC()->GetScaleY());
	return pSprite->DrawToBack(pt.x, pt.y);
}

bool AWC::DrawText(const APOINT& Pos, const char* pszStr,
	int nStrLen, UINT uColor, bool bShade)
{
	char szOut[512];
	if(pszStr == NULL)
	{
		return true;
	}
	if(nStrLen > 0)
	{
		strncpy(szOut, pszStr, nStrLen);
		szOut[nStrLen] = '\0';
	}
	else
	{
		strcpy(szOut, pszStr);
	}
	APOINT pt(
		(int)(Pos.x*AWC::GetWC()->GetScaleX()),
		(int)(Pos.y*AWC::GetWC()->GetScaleY()));
	return GetWC()->m_pAGame->GetA3DAssistTools()->F_TextOut(
		pt.x, pt.y, szOut, (A3DCOLOR) uColor, bShade);
}

bool AWC::GetTextExtent(const char* szText, int nTextLen, APOINT& Pos)
{
	char szOut[512];
	if(szText == NULL)
	{
		Pos.x	= 0;
		Pos.y	= 0;
		return true;
	}
	if(nTextLen > 0)
	{
		strncpy(szOut, szText, nTextLen);
		szOut[nTextLen]	= '\0';
	}
	else
	{
		strncpy(szOut, szText, 512);
	}
	szOut[511]	= '\0';

	if(!GetWC()->m_pAGame->GetA3DAssistTools()
		->F_GetTextExtent(szOut, (int*)&Pos.x, (int*)&Pos.y))
	{
		return false;
	}
	else
	{
		Pos.x	= (int)(Pos.x/AWC::GetWC()->GetScaleX());
		Pos.y	= (int)(Pos.y/AWC::GetWC()->GetScaleY());
	}
	return true;
}

int	AWC::GetCurrentFont()
{
	return GetWC()->m_pAGame->GetA3DAssistTools()->F_GetCurFont();
}

bool AWC::SetCurrentFont(int nFont)
{
	return GetWC()->m_pAGame->GetA3DAssistTools()->F_SetCurFont(nFont);
}

HFONT AWC::GetCurrentFontHandle()
{
	return (HFONT) GetWC()->m_pAGame->GetA3DAssistTools()
		->F_GetFont(GetWC()->m_pAGame->GetA3DAssistTools()->F_GetCurFont());
}

bool AWC::IsKeyPressing(AKeyboardButton akb)
{
	m_pWC	= this;
	bool bDown, bChanged;
	m_pCurrKeyboardMsg->GetButtonStatus(akb, bDown, bChanged);
	return bDown;
}

bool AWC::DispatchKeyboardMsg(ADeviceKeyboard* pKeyboard)
{
	// m_pAGame->GetA3DViewport()->
	/*if(pKeyboard->VK_FirstPress(AKB_ESC))
	{
		this->SetActiveStatus(false);
		m_pAGame->GetAGamePlay()->SetActiveStatus(true);
	}*/

	m_pWC	= this;
	if(m_pFocus != NULL)
	{
		((AWndKeyboardMsgFromDev*)m_pCurrKeyboardMsg)->SetDevice(pKeyboard);
		bool bAccept	= false;
		m_pFocus->ProcessKeyboardMsg(*m_pCurrKeyboardMsg, bAccept);
	}

	return true;
}

bool AWC::DispatchMouseMsg(ADeviceMouse* pMouse)
{
	m_pWC	= this;
	if(m_pMainWnd != NULL)
	{
		((AWndMouseMsgFromDev*)m_pCurrMouseMsg)->SetDevice(pMouse);
		m_pMainWnd->ProcessMouseMsg(*m_pCurrMouseMsg);
	}
	return true;
}

void AWC::GetFontSize(int nFont, int *pW, int * pH)
{
	int nCurrentFont = 	AWC::GetCurrentFont();
	AWC::SetCurrentFont(nFont);
	GetWC()->m_pAGame->GetA3DAssistTools()->F_GetTextExtent("A",pW,pH);
	AWC::SetCurrentFont(nCurrentFont);
}
