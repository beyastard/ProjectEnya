#include "AEditPlus.h"
#include "AWC.h"
#include "AFontDef.h"

class AEditDefCursor : public AEditPlusCursor
{
protected:
	UINT	m_uColor;
	UINT	m_uSelColor;
	int		m_nDelay;
	UINT	m_uLastShow;

public:
	AEditDefCursor();
	virtual bool OnDraw(const APOINT& AbPos, int nLineHeight, bool bSel = false);
	virtual void Update();
	virtual void SetColor(UINT nColor);
};

AEditDefCursor::AEditDefCursor() : 
m_uColor	(0xFFFFFF00),
m_uSelColor	(0xFFFFFFFF),
m_nDelay	(1000),
m_uLastShow	(0)
{
}

void AEditDefCursor::SetColor(UINT nColor)
{
	m_uColor	= nColor;
}

bool AEditDefCursor::OnDraw(const APOINT& AbPos, int nLineHeight, bool bSel)
{
	bool bShow;
	int  nDelta	= GetTickCount()-m_uLastShow;
	if(nDelta < (m_nDelay >> 1))
	{
		bShow	= true;
	}
	else if(nDelta > m_nDelay)
	{
		Update();
		bShow	= true;
	}
	else
	{
		bShow	= false;
	}

	if(bShow)
	{
		ARECT rCur;
		rCur.GetLeftTop()	= AbPos;
		rCur.SetHeight(nLineHeight);
		int nWidth	= nLineHeight >> 3;
		if(nWidth <= 0) nWidth ++;
		rCur.SetWidth(nWidth);
		AWC::DrawRectangle(rCur, m_uColor);
	}

	return true;
}

void AEditDefCursor::Update()
{
	m_uLastShow	= GetTickCount();
}


struct AEditVars
{
	WNDPROC pfOldEditProc;

	AEditVars()
	{
		pfOldEditProc	= NULL;
	}
}g_EditVars;

char g_szEditObject[] = "EditObject";
LRESULT CALLBACK EditWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AEditPlus* pEdit = (AEditPlus*)GetProp(hWnd, g_szEditObject);
	if(pEdit != NULL)
	{
		return pEdit->EditWindowProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		if(g_EditVars.pfOldEditProc != NULL)
		{
			return CallWindowProc(g_EditVars.pfOldEditProc, hWnd, uMsg, wParam, lParam);
		}
	}
	return 0;
}

IMPLEMENT_ASERIAL(AEditPlus, AWnd, 0x0100)

AEditPlus::AEditPlus():
m_uSelTextColor	(0xFFFFFFFF),
m_uSelBackColor	(0xFF6F6F6F),
m_nFont			(0),
m_hEdit			(NULL),
m_pCursor		(NULL),
m_hImc			(NULL),
m_nLastSelStart	(-1),
m_nLastSelEnd	(-1),
m_bEditLeftTop	(TRUE),
m_ptLastLButtonUp	(0, 0)
{
	m_bCanBeFocus	= true;
	m_pCursor		= new AEditDefCursor;
}

AEditPlus::~AEditPlus()
{
	if(m_pCursor != NULL)
	{
		delete m_pCursor;
	}
}

void AEditPlus::SetCursorColor(UINT uColor)
{
	if(m_pCursor != NULL)
	{
		m_pCursor->SetColor(uColor);
	}
}

void AEditPlus::ReplaceSel(const char* pszStr)
{
	SendMessage(m_hEdit, EM_REPLACESEL, 0, (LPARAM)pszStr);
}

bool AEditPlus::OnCreate()
{
	if(!AWnd::OnCreate())
	{
		return false;
	}

	if(g_EditVars.pfOldEditProc == NULL)
	{
		HWND hWnd = CreateWindowEx(
			WS_EX_TRANSPARENT,
			"EDIT",
			NULL,
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE,
			0, 0, 10, 10,
			AWC::GetWC()->GetMainHandle(),
			NULL, 
			GetModuleHandle(NULL),
			NULL);
		
		if(hWnd == NULL)
		{
			ADEBUG_TRACE("Cannot create temp edit!");
			return false;
		}
		g_EditVars.pfOldEditProc = (WNDPROC)SetClassLong(hWnd,
			GCL_WNDPROC, (LONG)::EditWindowProc);
		::DestroyWindow(hWnd);
		if(g_EditVars.pfOldEditProc == NULL)
		{
			ADEBUG_TRACE("Cannot get the window procedure of the temp edit.");
			return FALSE;
		}	
	}

	INT nWinLeft, nWinTop;
	if(m_bEditLeftTop)
	{
		nWinLeft	= 0;
		nWinTop		= 0;
	}
	else
	{
		nWinLeft	= m_rArea.left;
		nWinTop		= m_rArea.top;
	}

	m_hEdit = CreateWindowEx(
		WS_EX_TRANSPARENT, 
		"EDIT",
		NULL, 
		ES_WANTRETURN | ES_NOHIDESEL | WS_CHILD | ES_LEFT | ES_MULTILINE,
		//0, 0,
		nWinLeft, nWinTop, 
		//100, 100,
		m_rArea.GetWidth(), m_rArea.GetHeight(),
		AWC::GetWC()->GetMainHandle(),
		NULL,
		GetModuleHandle(NULL),
		NULL);
	if(m_hEdit == NULL)
	{
		ADEBUG_TRACE("Cannot create edit.");
		return false;
	}
	SetProp(m_hEdit, g_szEditObject, (HANDLE)this);
	
	m_nFont	= AWC::GetDefFont();
	AWC::SetCurrentFont(m_nFont);
	HFONT	hFont	= AWC::GetCurrentFontHandle();
	SendMessage(m_hEdit, WM_SETFONT, (WPARAM)hFont, 0);

	if(AWC::GetWC()->GetFocus() != this)
	{
		::SetFocus(AWC::GetWC()->GetMainHandle());
	}

	m_hImc	= ImmGetContext(AWC::GetWC()->GetMainHandle());

	return true;
}

LRESULT AEditPlus::EditWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(g_EditVars.pfOldEditProc != NULL)
	{
		if(m_bEditLeftTop)
		{
			//if(::GetCapture() == m_hEdit)
			{
				if(uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONUP || uMsg == WM_LBUTTONDOWN)
				{
					APOINT ptMouse(LOWORD(lParam), HIWORD(lParam));
					APOINT ptAb;
					GetAbsolutePos(ptAb);
					ptMouse	-= ptAb;
					ptMouse.x	= (int)(ptMouse.x*AWC::GetWC()->GetScaleX());
					ptMouse.y	= (int)(ptMouse.y*AWC::GetWC()->GetScaleY());
					lParam	= ptMouse;
				}
			}
		}
		HRESULT hr = CallWindowProc(g_EditVars.pfOldEditProc, hWnd, uMsg, wParam, lParam);
		if(uMsg != 512)
		{
			int i =0;
		}
		switch(uMsg)
		{
		case WM_KILLFOCUS:
		case WM_SETFOCUS:
			if(AWC::GetWC()->GetFocus() != this)
			{
				::SetFocus(AWC::GetWC()->GetMainHandle());
			}
			break;
		case WM_KEYDOWN:
			if(m_pCursor != NULL)
			{
				m_pCursor->Update();
			}
			break;
		case IMN_OPENCANDIDATE:
			{
				CANDIDATEFORM cf;
				cf.dwIndex		= 0;
				cf.dwStyle		= CFS_EXCLUDE;
				cf.ptCurrentPos	= APOINT(0, 0);
				cf.rcArea		= ARECT(0, 0, 0, 0);
				ImmSetCandidateWindow(m_hImc, &cf);
			}
			break;
		case WM_NCDESTROY:
			if(m_hEdit != NULL)
			{
				m_hEdit	= NULL;
			}
			break;
		}
		return hr;
	}
	return 0;
}

void AEditPlus::OnDestroy()
{
	if(m_hEdit != NULL)
	{
		::RemoveProp(m_hEdit, g_szEditObject);
		::DestroyWindow(m_hEdit);
		m_hEdit	= NULL;
	}
	if(m_hImc != NULL)
	{
		ImmReleaseContext(AWC::GetWC()->GetMainHandle(), m_hImc);
		m_hImc	= NULL;
	}
}

bool AEditPlus::OnDraw()
{
	if(!AWnd::OnDraw())
	{
		return false;
	}
	APOINT AbPos;
	GetAbsolutePos(AbPos);

	ARECT AbArea(AbPos.x, AbPos.y, AbPos.x+m_rArea.GetWidth(), AbPos.y+m_rArea.GetHeight());
	
	AWC::DrawRectangle(AbArea, m_uBackColor);

	if(!DrawLines(AbArea))
	{
		return false;
	}

	return true;
}

void AEditPlus::SetSel(INT nSelStart, INT nSelEnd)
{
	SendMessage(m_hEdit, EM_SETSEL, nSelStart, (LPARAM)nSelEnd);
}

INT	AEditPlus::GetLine(INT nIndex, char* pszBuf, INT nBufLength)
{
	*((WORD*)pszBuf)	= nBufLength;
	return (LONG)SendMessage(m_hEdit, EM_GETLINE, nIndex, (LPARAM)pszBuf);
}

INT AEditPlus::GetLineCount()
{
	return (LONG)SendMessage(m_hEdit, EM_GETLINECOUNT, 0, 0);
}

bool AEditPlus::DrawLines(const ARECT& AbArea)
{
	INT	nLineCount	= (LONG)SendMessage(m_hEdit, EM_GETLINECOUNT, 0, 0);
	if(nLineCount <= 0)
	{
		return true;
	}

	//if(AWC::GetCurrentFont() != m_nFont)
	{
		AWC::SetCurrentFont(m_nFont);
		HFONT	hFont	= AWC::GetCurrentFontHandle();
		SendMessage(m_hEdit, WM_SETFONT, (WPARAM)hFont, 0);
	}
	ARECT rFmtArea;
	SendMessage(m_hEdit, EM_GETRECT, 0, (LPARAM)&rFmtArea);
	float fScaleX	= AWC::GetWC()->GetScaleX();
	float fScaleY	= AWC::GetWC()->GetScaleY();
	rFmtArea.left	= (int)(rFmtArea.left/fScaleX);
	rFmtArea.top	= (int)(rFmtArea.top/fScaleY);
	rFmtArea.right	= (int)(rFmtArea.right/fScaleX);
	rFmtArea.bottom	= (int)(rFmtArea.bottom/fScaleY);
	if(rFmtArea.GetWidth() <= 0 || rFmtArea.GetHeight() <= 0)
	{
		return true;
	}
	rFmtArea.Move(AbArea.GetLeftTop());


	ARECT	rTextArea(rFmtArea);
	APOINT	ptTextSize;
	APOINT	ptSelStartPos, ptSelEndPos;
	INT		nDefTextHeight;
	INT		nLineHeight;
	INT		nCurLine;
	INT		nSelStart, nSelEnd;
	CHAR	szLineBuf[1024];

	SendMessage(m_hEdit, EM_GETSEL, (WPARAM)&nSelStart, (LPARAM)&nSelEnd);
	AWC::GetTextExtent("Ap", 2, ptTextSize);
	nDefTextHeight	= ptTextSize.y;
	nLineHeight		= nDefTextHeight;

	for(nCurLine = (LONG)SendMessage(m_hEdit, EM_GETFIRSTVISIBLELINE, 0, 0);
		nCurLine < nLineCount;
		nCurLine ++)
	{
		INT		nLineIndex;
		INT		nLineLength;
		INT		nLineSelStart;
		INT		nLineSelEnd;
		BOOL	bDrawSel;
		BOOL	bComputeStart;
		BOOL	bComputeEnd;

		nLineIndex	= (LONG)SendMessage(m_hEdit, EM_LINEINDEX, nCurLine, 0);
		*((WORD*)szLineBuf)	= sizeof(szLineBuf);
		nLineLength	= (LONG)SendMessage(m_hEdit, EM_GETLINE, nCurLine, (LPARAM)szLineBuf);

		if(nLineLength <= 0)
		{
			nLineHeight	= nDefTextHeight;
			if(nSelStart == nLineIndex)
			{
				ptSelStartPos	= rTextArea.GetLeftTop();
			}
			if(nSelEnd == nLineIndex)
			{
				ptSelEndPos		= rTextArea.GetLeftTop();
			}
			goto	Label_Next_Loop;
		}

		if(nSelStart > (nLineIndex+nLineLength) || nSelEnd < nLineIndex)
		{
			bDrawSel	= FALSE;
		}
		else
		{
			bDrawSel	= TRUE;
			if(nSelStart < nLineIndex)
			{
				nLineSelStart	= 0;
				bComputeStart	= FALSE;
			}
			else
			{
				nLineSelStart	= nSelStart - nLineIndex;
				bComputeStart	= TRUE;
			}
			if(nSelEnd > (nLineIndex + nLineLength))
			{
				nLineSelEnd		= nLineLength;
				bComputeEnd		= FALSE;
			}
			else
			{
				nLineSelEnd		= nSelEnd - nLineIndex;
				bComputeEnd		= TRUE;
			}
		}
		if(bDrawSel)
		{
			INT nSelLength	= nLineSelEnd - nLineSelStart;

			// draw none selected text.
			if(nLineSelStart > 0)
			{
				AWC::GetTextExtent(szLineBuf, nLineSelStart, ptTextSize);
				rTextArea.SetHeight(ptTextSize.y);
				if(rTextArea.bottom <= rFmtArea.bottom && ptTextSize.x > 0)
				{
					AWC::DrawText(rTextArea.GetLeftTop(),
						szLineBuf, nLineSelStart, m_uTextColor);
					rTextArea.left	+= ptTextSize.x;
				}
			}
			if(bComputeStart)
			{
				ptSelStartPos	= rTextArea.GetLeftTop();
			}
			if(nSelLength > 0)
			{
				AWC::GetTextExtent(&szLineBuf[nLineSelStart], nSelLength, ptTextSize);
				rTextArea.SetHeight(ptTextSize.y);
				if(rTextArea.bottom <= rFmtArea.bottom && ptTextSize.x > 0)
				{
					ARECT rSelArea(
						rTextArea.left,
						rTextArea.top, 
						rTextArea.left + ptTextSize.x,
						rTextArea.bottom);
					AWC::DrawRectangle(rSelArea, m_uSelBackColor);
					AWC::DrawText(rTextArea.GetLeftTop(),
						&szLineBuf[nLineSelStart], nSelLength, m_uSelTextColor);
					rTextArea.left	+= ptTextSize.x;
				}
			}
			if(bComputeEnd)
			{
				ptSelEndPos		= rTextArea.GetLeftTop();
			}
			INT	nTailLength	= nLineLength - nLineSelEnd;
			if(nTailLength > 0)
			{
				AWC::GetTextExtent(&szLineBuf[nLineSelEnd], nTailLength, ptTextSize);
				rTextArea.SetHeight(ptTextSize.y);
				if(rTextArea.bottom <= rFmtArea.bottom && ptTextSize.x > 0)
				{
					AWC::DrawText(rTextArea.GetLeftTop(),
						&szLineBuf[nLineSelEnd], nTailLength, m_uTextColor);
					//rTextArea.left	+= ptTextSize.x;
				}
			}
		}// if(bDrawSel)
		else// if(bDrawSel)
		{
			if(nLineLength > 0)
			{
				AWC::GetTextExtent(szLineBuf, nLineLength, ptTextSize);
				rTextArea.SetHeight(ptTextSize.y);
				if(rTextArea.bottom <= rFmtArea.bottom && ptTextSize.x > 0)
				{
					AWC::DrawText(rTextArea.GetLeftTop(),
						szLineBuf, nLineLength, m_uTextColor);
					//rTextArea.left	+= ptTextSize.x;
				}
			}
		}// if(bDrawSel)/else

Label_Next_Loop:

		rTextArea.left	= rFmtArea.left;
		rTextArea.top  += nLineHeight;
		if(rTextArea.bottom > rFmtArea.bottom)
		{
			break;
		}
	}

	//m_ptLastLButtonUpm_pCursor->OnDraw(

	if(ptSelStartPos.x > rFmtArea.right)
	{
		ptSelStartPos.x	= rFmtArea.right;
	}
	if(ptSelEndPos.x > rFmtArea.right)
	{
		ptSelEndPos.x	= rFmtArea.right;
	}

	/*INT nStartDis = 
		(ptSelStartPos.x-m_ptLastLButtonUp.x)*(ptSelStartPos.x-m_ptLastLButtonUp.x)+
		(ptSelStartPos.y-m_ptLastLButtonUp.y)*(ptSelStartPos.y-m_ptLastLButtonUp.y);

	INT nEndDis = 
		(ptSelEndPos.x-m_ptLastLButtonUp.x)*(ptSelEndPos.x-m_ptLastLButtonUp.x)+
		(ptSelEndPos.y-m_ptLastLButtonUp.y)*(ptSelEndPos.y-m_ptLastLButtonUp.y);
	*/
	if(m_nLastSelStart != nSelStart)
	{
		m_ptCursorPos = ptSelStartPos;
	}
	else if(m_nLastSelEnd != nSelEnd)
	{
		m_ptCursorPos = ptSelEndPos;
	}
	m_nLastSelStart	= nSelStart;
	m_nLastSelEnd	= nSelEnd;

	APOINT ptAbCurPos	= m_ptCursorPos;

	if(m_pCursor != NULL)
	{
		if(/*nSelStart == nSelEnd &&*/ AWC::GetWC()->GetFocus() == this)
		{
			m_pCursor->OnDraw(ptAbCurPos, nLineHeight);
		}
	}

	return true;
}

bool AEditPlus::OnMouseMsg(AWndMouseMsg& Msg)
{
	if(!AWnd::OnMouseMsg(Msg))
	{
		return false;
	}
	APOINT ptAbPos, ptMousePos;

	GetAbsolutePos(ptAbPos);
	Msg.GetPos(ptMousePos);
	//ptMousePos	-= ptAbPos;
	LPARAM	lParam	= ptMousePos;
	WPARAM	wParam	= 0;
	UINT	uMessage;

	bool bDown, bChanged;
	Msg.GetButtonStatus(AMB_LEFT, bDown, bChanged);

	if(bChanged)
	{
		if(bDown)
		{
			uMessage	= WM_LBUTTONDOWN;
		}
		else
		{
			uMessage	= WM_LBUTTONUP;
		}
		if(m_pCursor != NULL)
		{
			m_pCursor->Update();
		}
	}
	else
	{
		uMessage	= WM_MOUSEMOVE;
	}

	EditWindowProc(m_hEdit, uMessage, wParam, lParam);

	return true;
}

bool AEditPlus::OnKeyboardMsg(AWndKeyboardMsg& Msg, bool& bAccept)
{
	if(!AWnd::OnKeyboardMsg(Msg, bAccept))
	{
		return false;
	}

	bAccept	= true;
	if(::GetFocus() != m_hEdit)
	{
		::SetFocus(m_hEdit);
	}

	return true;
}


bool AEditPlus::OnSetFocus(bool bFocus)
{
	if(!AWnd::OnSetFocus(bFocus))
	{
		return false;
	}

	if(m_hEdit != NULL)
	{
		if(bFocus)
		{
			::SetFocus(m_hEdit);
		}
		else
		{
			::SetFocus(AWC::GetWC()->GetMainHandle());
		}
	}

	return true;
}