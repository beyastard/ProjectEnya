#include "AWndConsole.h"
#include "AWC.h"

IMPLEMENT_ASERIAL(AEditConsole, AEditPlus, 0x0100)

AEditConsole::AEditConsole()
{
}

AEditConsole::~AEditConsole()
{
}

LRESULT AEditConsole::EditWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(AWC::GetWC()->GetFocus() != (AWnd*)this)
	{
		return 0;
	}
	switch(uMsg)
	{
	case WM_CHAR:
		if(wParam == VK_RETURN)
		{
			AWndConsole* pWnd	= (AWndConsole*)m_pParent;
			pWnd->OnInputEnter();
			break;
		}
		else if(wParam == 96)
		{
			break;
		}
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			AWndConsole* pWnd	= (AWndConsole*)m_pParent;
			pWnd->OnInputKey((AKeyboardButton)wParam, uMsg == WM_KEYDOWN);
		}
	default:
		return AEditPlus::EditWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

bool AEditConsole::OnKeyboardMsg(AWndKeyboardMsg& Msg, bool& bAccept)
{
	if(!AEditPlus::OnKeyboardMsg(Msg, bAccept))
	{
		return false;
	}

	return true;
}

IMPLEMENT_ASERIAL(AWndConsole, AWnd, 0x0100)

AWndConsole::AWndConsole():
m_nInputHeight		(25),
m_nInputListSpace	(5),
m_nMarginHor		(5),
m_nMarginVer		(5),
m_nHistoryIndex		(0)
{
	m_uBackColor	= 0x8F0000FF;
	m_bCanBeFocus	= true;
}

AWndConsole::~AWndConsole()
{
}

void AWndConsole::AdjustArea(ARECT& rInputArea, ARECT& rListArea)
{
	rListArea.SetValue(
		m_nMarginHor,
		m_nMarginVer,
		m_rArea.GetWidth()-m_nMarginHor,
		m_rArea.GetHeight()-m_nMarginVer-m_nInputHeight-m_nInputListSpace);

	rInputArea.SetValue(
		m_nMarginHor,
		rListArea.bottom + m_nInputListSpace,
		m_rArea.GetWidth()-m_nMarginHor,
		rListArea.bottom + m_nInputListSpace + m_nInputHeight);
}

bool AWndConsole::OnInputKey(AKeyboardButton akb, bool down)
{
	return true;
}


bool AWndConsole::OnDraw()
{
	APOINT ptAb;
	GetAbsolutePos(ptAb);

	ARECT rMargin;

	// left,top
	rMargin.SetValue(
		ptAb.x, 
		ptAb.y,
		ptAb.x + m_nMarginHor,
		ptAb.y + m_rArea.GetHeight() - m_nMarginVer);
	AWC::DrawRectangle(rMargin, m_uBackColor);

	rMargin.SetValue(
		ptAb.x + m_nMarginHor, 
		ptAb.y,
		ptAb.x + m_rArea.GetWidth(),
		ptAb.y + m_nMarginVer);
	AWC::DrawRectangle(rMargin, m_uBackColor);

	rMargin.SetValue(
		ptAb.x + m_rArea.GetWidth() - m_nMarginHor, 
		ptAb.y + m_nMarginVer,
		ptAb.x + m_rArea.GetWidth(),
		ptAb.y + m_rArea.GetHeight());
	AWC::DrawRectangle(rMargin, m_uBackColor);

	rMargin.SetValue(
		ptAb.x, 
		ptAb.y + m_rArea.GetHeight() - m_nMarginVer,
		ptAb.x + m_rArea.GetWidth() - m_nMarginHor,
		ptAb.y + m_rArea.GetHeight());
	AWC::DrawRectangle(rMargin, m_uBackColor);

	ARECT	rInputArea;
	ARECT	rListArea;
	AdjustArea(rInputArea, rListArea);

	rMargin.SetValue(
		rListArea.left,
		rListArea.bottom,
		rInputArea.right,
		rInputArea.top);
	rMargin.Move(ptAb);
	AWC::DrawRectangle(rMargin, m_uBackColor);

	return true;
}

bool AWndConsole::OnInputEnter()
{
	char szBuf[1024];
	INT nStrLength = m_CommandInput.GetLine(0, szBuf, sizeof(szBuf));
	if(nStrLength > 0)
	{
		szBuf[nStrLength]	= '\0';
		AddLine(szBuf);
		m_CommandHistory.AddString(szBuf);
		if(m_CommandHistory.GetLength() > 256)
		{
			m_CommandHistory.RemoveAt(0);
		}
		m_nHistoryIndex	= m_CommandHistory.GetLength();
	}
	
	return true;
}

bool AWndConsole::AddLine(const char* pszStr)
{
	m_CommandList.AppendString(pszStr);
	if(m_CommandList.GetItemCount() > 256)
	{
		m_CommandList.RemoveString(0);
	}
	m_CommandInput.SetSel();
	INT nCount	= m_CommandList.GetMaxVisibleCount();
	if(m_CommandList.GetItemCount() > nCount)
	{
		m_CommandList.SetFirstVisible(m_CommandList.GetItemCount() - nCount);
	}

	return true;
}

void AWndConsole::MoveHistory(int nStep)
{
	if(m_CommandHistory.GetLength() <= 0)
	{
		return;
	}

	m_nHistoryIndex += nStep;

	if(m_nHistoryIndex < 0)
	{
		m_nHistoryIndex	= 0;
	}
	else if(m_nHistoryIndex >= m_CommandHistory.GetLength())
	{
		m_nHistoryIndex	= m_CommandHistory.GetLength() - 1;
	}
	m_CommandInput.SetSel();
	m_CommandInput.ReplaceSel(m_CommandHistory.GetString(m_nHistoryIndex));
}

bool AWndConsole::OnCreate()
{
	if(!AWnd::OnCreate())
	{
		return false;
	}

	ARECT	rInputArea;
	ARECT	rListArea;
	AdjustArea(rInputArea, rListArea);

	if(!m_CommandInput.Create(this, rInputArea))
	{
		ADEBUG_TRACE("Cannot create Command input.");
		return false;
	}
	m_CommandInput.SetBackColor(0x8F4F4FFF);
	m_CommandInput.SetTextColor(0xFF00FFFF);


	m_CommandList.SetMargin(0, 0);
	if(!m_CommandList.Create(this, rListArea))
	{
		ADEBUG_TRACE("Cannot create Command list.");
		return false;
	}
	m_CommandList.SetBackColor(0x8F4F4FFF);
	//m_CommandList.SetBackColor(0);
	m_CommandList.SetTextColor(0xFF00FFFF);


	return true;
}

bool AWndConsole::OnSetFocus(bool bFocus)
{
	if(!AWnd::OnSetFocus(bFocus))
	{
		return false;
	}

	if(bFocus)
	{
		AWC::GetWC()->SetFocus(&m_CommandInput);
	}

	return true;
}

void AWndConsole::OnDestroy()
{
}