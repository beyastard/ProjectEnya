#include "AEdit.h"
#include "AWC.h"

class AEditDefaultCursor : public AEditCursor
{
protected:
	UINT	m_uColor;
	UINT	m_uSelColor;
	int		m_nDelay;
	UINT	m_uLastShow;

public:
	AEditDefaultCursor();
	virtual bool OnDraw(const APOINT& AbPos, int nLineHeight, bool bSel = false);
	virtual void Update();
};

AEditDefaultCursor::AEditDefaultCursor() : 
m_uColor	(0xFF000000),
m_uSelColor	(0xFFFFFFFF),
m_nDelay	(1000),
m_uLastShow	(0)
{
}

bool AEditDefaultCursor::OnDraw(const APOINT& AbPos, int nLineHeight, bool bSel)
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

void AEditDefaultCursor::Update()
{
	m_uLastShow	= GetTickCount();
}

IMPLEMENT_ASERIAL(AEdit, AWnd, 1)

AEdit::AEdit():
m_bMultiLine		(false),
m_nFirstVisibleLine	(0),
m_ptSelStart		(0, 0),
m_ptSelEnd			(0, 0),
m_ptCurPos			(0, 0),
m_uSelTextColor		(0xFFFFFFFF),
m_uSelBackColor		(0xFF000000),
m_nLineHeight		(-1),
m_nLineStep			(1),
m_uMarginColor		(0xFF00FF00),
m_nMarginLeft		(5),
m_nMarginTop		(5),
m_nMarginRight		(5),
m_nMarginBottom		(5),
m_pCursor			(NULL),
m_uLastKeyDown		(0),
m_uLastKeyDownTime	(0),
m_uLastKeyDownEnum	(0),
m_uKeyDownEnumDelay	(200),
m_uKeyDownFirstDelay(500)
{
	m_bCanBeFocus	= true;
	m_asStrings.AddString("");
}

AEdit::~AEdit()
{
	if(m_pCursor != NULL)
	{
		delete m_pCursor;
	}
}

bool AEdit::OnSetTextFont(int nFont)
{
	if(!AWnd::OnSetTextFont(nFont))
	{
		return false;
	}

	return true;
}

bool AEdit::OnDraw()
{
	if(!AWnd::OnDraw())
	{
		return false;
	}

	APOINT AbLeftTop;
	GetAbsolutePos(AbLeftTop);

	if(!OnDrawBack(AbLeftTop))
	{
		return false;
	}

	if(!OnDrawMargin(AbLeftTop))
	{
		return false;
	}

	ARECT AbRect(AbLeftTop.x + m_nMarginLeft,
		AbLeftTop.y + m_nMarginTop,
		AbLeftTop.x + m_rArea.GetWidth() - m_nMarginRight,
		AbLeftTop.y + m_rArea.GetHeight() - m_nMarginBottom);

	if(AbRect.GetWidth() <= 0 && AbRect.GetHeight() <= 0)
	{
		return true;
	}
	LONG AbBottom	= AbLeftTop.y + m_rArea.GetHeight() - m_nMarginBottom;
	int	nCurLine	= m_nFirstVisibleLine;
	int nDeltaLine	= 0;
	int nLineHeight	= m_nLineHeight;

	if(nCurLine < m_asStrings.GetLength())
	{
		if(m_nLineHeight < 0)
		{
			APOINT Size;
			AWC::GetTextExtent("Ap", 2, Size);
			nLineHeight	= Size.y;
		}
	}

	nDeltaLine	= nLineHeight + m_nLineStep;
	AbRect.SetHeight(nDeltaLine);

	while(AbRect.bottom <= AbBottom &&
		nCurLine < m_asStrings.GetLength())
	{
		OnDrawText(AbRect.GetLeftTop(), m_asStrings.GetString(nCurLine));
		AbRect.MoveVer(nDeltaLine);
		nCurLine ++;
	}

	//AbLeftTop
	APOINT CurPoint;
	CurPoint.y	= AbLeftTop.y + m_nMarginTop + nDeltaLine * m_ptCurPos.y;
	CurPoint.x	= AbLeftTop.x + m_nMarginLeft;
	if(m_ptCurPos.x > 0)
	{
		const char* pszCur	= m_asStrings.GetString(m_ptCurPos.y);
		if(pszCur != NULL)
		{
			APOINT Size;
			AWC::GetTextExtent(pszCur, m_ptCurPos.x, Size);
			CurPoint.x	+= Size.x;
		}
	}
	if(AWC::GetWC()->GetFocus() == this)
	{
		m_pCursor->OnDraw(CurPoint, nLineHeight);
	}

	return true;
}

bool AEdit::OnDrawMargin(const APOINT& AbLeftTop)
{
	ARECT rMargin;

	// lefttop
	rMargin.SetValue(
		AbLeftTop.x,
		AbLeftTop.y,
		AbLeftTop.x + m_nMarginLeft,
		AbLeftTop.y + m_rArea.GetHeight() - m_nMarginBottom);
	AWC::DrawRectangle(rMargin, m_uMarginColor);

	// topright
	rMargin.SetValue(
		AbLeftTop.x + m_nMarginLeft,
		AbLeftTop.y,
		AbLeftTop.x + m_rArea.GetWidth(),
		AbLeftTop.y + m_nMarginTop);
	AWC::DrawRectangle(rMargin, m_uMarginColor);

	// rightbottom
	rMargin.SetValue(
		AbLeftTop.x + m_rArea.GetWidth() - m_nMarginRight,
		AbLeftTop.y + m_nMarginTop,
		AbLeftTop.x + m_rArea.GetWidth(),
		AbLeftTop.y + m_rArea.GetHeight());
	AWC::DrawRectangle(rMargin, m_uMarginColor);

	// bottomleft
	rMargin.SetValue(
		AbLeftTop.x,
		AbLeftTop.y + m_rArea.GetHeight() - m_nMarginBottom,
		AbLeftTop.x + m_rArea.GetWidth() - m_nMarginRight,
		AbLeftTop.y + m_rArea.GetHeight());
	AWC::DrawRectangle(rMargin, m_uMarginColor);

	return true;
}

bool AEdit::OnDrawBack(const APOINT& AbLeftTop)
{
	ARECT rBack(
		AbLeftTop.x + m_nMarginLeft,
		AbLeftTop.y + m_nMarginTop,
		AbLeftTop.x + m_rArea.GetWidth() - m_nMarginRight,
		AbLeftTop.y + m_rArea.GetHeight() - m_nMarginBottom);
	AWC::DrawRectangle(rBack, m_uBackColor);
	return true;
}

bool AEdit::OnCreate()
{
	if(!AWnd::OnCreate())
	{
		return false;
	}
	m_pCursor	= new AEditDefaultCursor;

	return true;
}

bool AEdit::OnDrawText(const APOINT& AbPos, const char* pszText, int nText, bool bSel)
{
	AWC::DrawText(AbPos, pszText, nText, m_uTextColor);
	return true;
}

bool AEdit::OnEditPaste()
{
	char* pszStr = (char*)GetClipboardData(CF_OWNERDISPLAY );
	if(pszStr != NULL)
	{
		while(pszStr[0])
		{
			InsertChar(pszStr[0]);
			pszStr++;
		}
	}
	return true;
}

bool AEdit::OnEditKeyDown(UINT nKey)
{
	switch((AKeyboardButton)nKey)
	{
	case AKB_LEFT:
		if(IsMultiByteChar(-1))
		{
			CurMoveHor(-2);
		}
		else
		{
			CurMoveHor(-1);
		}
		break;
	case AKB_TOP:
		CurMoveVer(-1);
		break;
	case AKB_RIGHT:
		if(IsMultiByteChar(1))
		{
			CurMoveHor(2);
		}
		else
		{
			CurMoveHor(1);
		}
		break;
	case AKB_BOTTOM:
		CurMoveVer(1);
		break;
	case AKB_HOME:
		CurMoveHor(-m_ptCurPos.x);
		break;
	case AKB_END:
		CurMoveHor(GetCurrentLineLength());
	case AKB_DELETE:
		RemoveChar(1);
		break;
	case AKB_BACK:
		RemoveChar(-1);
		break;
	case AKB_V:
		if(AWC::GetWC()->IsKeyPressing(AKB_CONTROL))
		{
			OnEditPaste();
		}
		break;
	}
	return true;
}

bool AEdit::OnEditKeyPressing(UINT nKey, int nTimePassed)
{
	if(!OnEditKeyDown(nKey))
	{
		return false;
	}
	return true;
}

bool AEdit::OnKeyboardMsg(AWndKeyboardMsg& Msg, bool& bAccept)
{
	if(!AWnd::OnKeyboardMsg(Msg, bAccept))
	{
		return false;
	}
	bAccept	= true;
	bool bDown[256], bChanged[256];
	int i;
	for(i = 0; i < 256; i ++)
	{
		Msg.GetButtonStatus((AKeyboardButton)i, bDown[i], bChanged[i]);
		if(bDown[i] && bChanged[i])
		{
			m_uLastKeyDown		= (UINT)i;
			m_uLastKeyDownTime	= GetTickCount();
			if(!OnEditKeyDown((UINT)i))
			{
				return false;
			}
		//	return true;
		}
	}

	if(bDown[m_uLastKeyDown] && !bChanged[m_uLastKeyDown])
	{
		if(GetTickCount() - m_uLastKeyDownTime > m_uKeyDownFirstDelay)
		{
			if(GetTickCount() - m_uLastKeyDownEnum > m_uKeyDownEnumDelay)
			{
				if(!OnEditKeyPressing(
					m_uLastKeyDown,
					GetTickCount() - m_uLastKeyDownTime))
				{
					return false;
				}
			}
		}
	}

	AUString strIme;
	Msg.GetImeChars(strIme);
	int nImeLen	= strIme.GetLength();
	for(i = 0; i < nImeLen; i ++)
	{
		UINT c = (unsigned char)strIme[i];
		if(c & 0x80)
		{
			if(i < nImeLen-1)
			{
				c |= ((UINT)((unsigned char)strIme[i+1])) << 8;
				i ++;
			}
		}
		InsertChar(c);
	}
	//Msg.GetButtonStatus(AKB_LEFT, );

	return true;
}

void AEdit::AddString(const char* pszStr)
{
	m_asStrings.AddString(pszStr);
}

const char*	AEdit::GetCurrentLine()
{
	if(GetLineCount() <= 0)
	{
		return NULL;
	}
	return m_asStrings.GetString(m_ptCurPos.y);
}

int	 AEdit::GetCurrentLineLength()
{
	const char* pszStr = GetCurrentLine();
	if(pszStr == NULL)
	{
		return 0;
	}
	return strlen(pszStr);
}

void AEdit::CurMoveHor(int nStep)
{
	int	nLength	= GetCurrentLineLength();
	if(m_ptCurPos.x > nLength)
	{
		m_ptCurPos.x = (LONG)nLength;
	}
	int nPos	= m_ptCurPos.x + nStep;

	if(nPos > nLength)
	{
		nPos	= nLength;
	}
	else if(nPos < 0)
	{
		nPos	= 0;
	}

	m_ptCurPos.x	= nPos;
	m_pCursor->Update();
}

void AEdit::CurMoveVer(int nStep)
{
	int	nLineCount	= GetLineCount();
	if(m_ptCurPos.y >= nLineCount)
	{
		m_ptCurPos.y = nLineCount;
	}
	int nPos		= m_ptCurPos.y + nStep;
	if(nPos >= nLineCount)
	{
		nPos	= nLineCount -1;
	}
	if(nPos < 0)
	{
		nPos	= 0;
	}
	m_ptCurPos.y	= nPos;
	m_pCursor->Update();
}

void AEdit::GetTextArea(const ARECT& rArea)
{

}

void AEdit::CurMoveTo(const APOINT& ptNewPos)
{
	APOINT ptDelta	= ptNewPos - m_ptCurPos;
	CurMoveHor(ptDelta.x);
	CurMoveVer(ptDelta.y);
}

bool AEdit::NewLine()
{
	// Not implement because of not suport.
	if(!m_bMultiLine)
	{
		return false;
	}
	if(GetLineCount() == 0)
	{
		//m_s
	}
	return true;
}

bool AEdit::RemoveChar(int nDirection)
{
	char* pszLine	= const_cast<char*>(GetCurrentLine());
	int nLength = pszLine ? strlen(pszLine) : 0;
	int nCurPos	= m_ptCurPos.x;
	if(nCurPos < 0)
	{
		nCurPos = 0;
	}
	else if(nCurPos > nLength)
	{
		nCurPos	= nLength;
	}

	if(nDirection < 0)
	{
		if(nCurPos > 0)
		{
			int nCharSize = 1;
			if(IsMultiByteChar(-1))
			{
				nCharSize	= 2;
			}
			MoveMemory(&pszLine[nCurPos-nCharSize], &pszLine[nCurPos], nLength - nCurPos + 1);
			m_ptCurPos.x	= nCurPos - nCharSize;
		}
	}
	else
	{
		if(nCurPos < nLength)
		{
			int nCharSize = 1;
			if(IsMultiByteChar(1))
			{
				nCharSize	= 2;
			}
			MoveMemory(&pszLine[nCurPos], &pszLine[nCurPos+nCharSize], nLength - nCurPos -nCharSize+1);
			m_ptCurPos.x	= nCurPos;
		}
	}
	return true;
}

bool AEdit::IsMultiByteChar(int nDirection)
{
	const char* pszLine	= GetCurrentLine();
	if(pszLine == NULL)
	{
		return false;
	}
	int nLength	= GetCurrentLineLength();
	if(nLength <= 1)
	{
		return false;
	}
	int nCurPos	= m_ptCurPos.x;
	if(nCurPos < 0)
	{
		nCurPos = 0;
	}
	else if(nCurPos > nLength)
	{
		nCurPos	= nLength;
	}
	int nCharSize = 1;
	if(nDirection < 0)
	{
		if(nCurPos > 0)
		{
			if(pszLine[nCurPos-1] < 0)
			{
				nCharSize	= 2;
			}
			/*else
			{
				if(nCurPos > 1)
				{
					if(pszLine[nCurPos-2] < 0)
					{
						nCharSize	= 2;
					}
				}
			}*/
		}
	}
	else
	{
		if(nCurPos < nLength)
		{
			if(pszLine[nCurPos] < 0)
			{
				nCharSize	= 2;
			}
			/*else
			{
				if(nCurPos < nLength - 1)
				{
					if(pszLine[nCurPos+1] < 0)
					{
						nCharSize	= 2;
					}
				}
			}*/
		}
	}
	return nCharSize > 1;
}

bool AEdit::InsertChar(UINT uChar)
{
	APOINT SizeChar, SizeLine;
	char  szBuf[512];
	const char* pszLine = GetCurrentLine();
	int nCharSize	= uChar > 255 ? 2 : 1;
	
	AWC::GetTextExtent((char*)&uChar, nCharSize, SizeChar);
	AWC::GetTextExtent(pszLine, -1, SizeLine);
	if((SizeChar.x + SizeLine.x) > 
		m_rArea.GetWidth() - m_nMarginLeft - m_nMarginRight)
	{
		return false;
	}

	int nLength = pszLine ? strlen(pszLine) : 0;
	int nCurPos	= m_ptCurPos.x;
	if(nCurPos < 0)
	{
		nCurPos = 0;
	}
	else if(nCurPos > nLength)
	{
		nCurPos	= nLength;
	}
	strncpy(szBuf, pszLine, nCurPos);
	if(nCurPos < nLength)
	{
		strncpy(&szBuf[nCurPos+nCharSize], &pszLine[nCurPos], nLength - nCurPos);
	}
	strncpy(&szBuf[nCurPos], (char*)&uChar, nCharSize);
	
	szBuf[nLength+nCharSize]= '\0';

	m_asStrings.ReplaceString(m_ptCurPos.y, szBuf);

	m_ptCurPos.x	= nCurPos + nCharSize;
	
	return true;
}