#include "AScrollbar.h"
#include "AWC.h"
//#include "mtime.h"

IMPLEMENT_ASERIAL(AScrollButton, AButton, 0x0100)

AScrollButton::AScrollButton():
m_uFirstDown		(0),
m_uFirstDownDelay	(500),
m_uLastDown			(0),
m_uDownDelay		(100),
m_sbsStyle			(ASBS_MID)
{
	m_uBackColor	= 0xFF7F7F7F;
	m_bCanBeFocus	= false;
}

AScrollButton::~AScrollButton()
{
}

bool AScrollButton::OnDraw()
{
	APOINT	ptAb;
	GetAbsolutePos(ptAb);

	ARECT	rArea(ptAb.x, ptAb.y, ptAb.x + m_rArea.GetWidth(), ptAb.y + m_rArea.GetHeight());
	AWC::DrawRectangle(rArea, m_uBackColor);
	
	return true;
}

bool AScrollButton::OnMouseMsg(AWndMouseMsg& Msg)
{
	if(!AButton::OnMouseMsg(Msg))
	{
		return false;
	}

	if(m_sbsStyle == ASBS_MID)
	{
		return ((AScrollButton*)m_pParent)->OnMouseMsg(Msg);
	}

	bool bDown, bChanged;
	Msg.GetButtonStatus(AMB_LEFT, bDown, bChanged);
	if(!bDown)
	{
		return true;
	}
/*		UINT				m_uFirstDown;
	UINT				m_uFirstDownDelay;
	UINT				m_uLastDown;
	UINT				m_uDownDelay;
*/
	if(bChanged)
	{
		m_uFirstDown	= (UINT)GetTickCount();
		return ((AScrollbar*)m_pParent)->OnButtonClick(this);
	}
	else
	{
		UINT uCurrent	= (UINT)GetTickCount();
		if(uCurrent - m_uFirstDown < m_uFirstDownDelay)
		{
			return true;
		}
		if(uCurrent - m_uLastDown > m_uDownDelay)
		{
			m_uLastDown	= uCurrent;
			return ((AScrollbar*)m_pParent)->OnButtonClick(this);
		}
	}

	return true;
}

IMPLEMENT_ASERIAL(AScrollbar, AWnd, 0x0100)

AScrollbar::AScrollbar():
m_ssStyle		(ASS_VER),
m_psbUp			(NULL),
m_psbMid		(NULL),
m_psbDown		(NULL),
m_nScrollLength	(0),
m_nCurrentPos	(0),
m_pOnScroll		(NULL)
{
	m_uBackColor	= 0xFFDFDFDF;
}

AScrollbar::~AScrollbar()
{
}

bool AScrollbar::SetArea(const ARECT& rArea)
{
	AWnd::SetArea(rArea);

	ARECT rUpArea;
	ARECT rMidArea;
	ARECT rDownArea;
	
	AdjustSize(rUpArea, rMidArea, rDownArea);

	m_psbUp->SetArea(rUpArea);
	m_psbMid->SetArea(rMidArea);
	m_psbDown->SetArea(rDownArea);

	return true;
}
void AScrollbar::SetScrollPos(int nPos)
{
	if(nPos	<= 0)
	{
		m_nCurrentPos	= 0;
	}
	else if(nPos >= m_nScrollLength)
	{
		m_nCurrentPos	= m_nScrollLength;
	}
	else
	{
		m_nCurrentPos	= nPos;
	}
	ARECT rMidArea;
	AdjustMid(rMidArea);
	m_psbMid->SetArea(rMidArea);

}

bool AScrollbar::OnMouseMsg(AWndMouseMsg& Msg)
{
	if(!AWnd::OnMouseMsg(Msg))
	{
		return false;
	}

	if(m_pOnScroll == NULL)
	{
		return true;
	}

	bool	bDown, bChanged;
	APOINT	ptMouse;
	APOINT	ptAb;
	APOINT	ptClick;
	Msg.GetButtonStatus(AMB_LEFT, bDown, bChanged);

	if(!bChanged || !bDown)
	{
		return true;
	}

	Msg.GetPos(ptMouse);
	GetAbsolutePos(ptAb);

	ptClick	= ptMouse - ptAb;
	if(m_ssStyle == ASS_VER)
	{
		INT nHeight		= m_psbUp->GetArea().GetHeight();
		m_nCurrentPos	= ptClick.y - nHeight - nHeight/2;
	}
	else
	{
		INT nWidth		= m_psbUp->GetArea().GetWidth();
		m_nCurrentPos	= ptClick.x - nWidth - nWidth/2;
	}
	if(m_nCurrentPos < 0)
	{
		m_nCurrentPos	= 0;
	}
	else if(m_nCurrentPos > m_nScrollLength)
	{
		m_nCurrentPos	= m_nScrollLength;
	}
	if(!(m_pParent->*m_pOnScroll)(ASM_SET_POSITION, m_nScrollLength, m_nCurrentPos))
	{
		ADEBUG_TRACE("OnScroll failed.");
		return false;
	}
	if(m_nCurrentPos < 0)
	{
		m_nCurrentPos	= 0;
	}
	else if(m_nCurrentPos > m_nScrollLength)
	{
		m_nCurrentPos	= m_nScrollLength;
	}
	
	ARECT rMidArea;
	AdjustMid(rMidArea);
	m_psbMid->SetArea(rMidArea);

	return true;
}

bool AScrollbar::OnButtonClick(AScrollButton* pButton)
{
	if(m_pOnScroll == NULL)
	{
		return true;
	}
	AScrollMessage	msg;
	if(pButton == m_psbUp)
	{
		msg	= ASM_STEP_UP;
	}
	else if(pButton == m_psbDown)
	{
		msg	= ASM_STEP_DOWN;
	}
	else
	{
		ADEBUG_TRACE("Unknown button pointer!");
		return false;
	}
	if(!(m_pParent->*m_pOnScroll)(msg, m_nScrollLength, m_nCurrentPos))
	{
		ADEBUG_TRACE("OnScroll failed.");
		return false;
	}
	if(m_nCurrentPos < 0)
	{
		m_nCurrentPos	= 0;
	}
	else if(m_nCurrentPos > m_nScrollLength)
	{
		m_nCurrentPos	= m_nScrollLength;
	}
	ARECT rMidArea;
	AdjustMid(rMidArea);
	m_psbMid->SetArea(rMidArea);
	return true;
}

void AScrollbar::AdjustMid(ARECT& rMid)
{
	if(m_nScrollLength <= 0)
	{
		rMid.SetValue(0, 0, 0, 0);
		return;
	}
	switch(m_ssStyle)
	{
	case ASS_VER:
		{
			INT	nWidth	= m_rArea.GetWidth();
			INT nHeight	= nWidth;
			INT nTop	= nHeight+m_nCurrentPos;
			rMid.SetValue(0, nTop, nWidth, nTop + nHeight);
		}
		break;
	case ASS_HOR:
		{
			INT	nHeight	= m_rArea.GetHeight();
			INT nWidth	= nHeight;
			INT nLeft	= nWidth+m_nCurrentPos;
			rMid.SetValue(nLeft, 0, nLeft + nWidth, nHeight);
		}
		break;
	default:
		ADEBUG_TRACE("Invalid scrollbar style.");
		break;
	}
}

void AScrollbar::AdjustSize(ARECT& rUpArea, ARECT& rMidArea, ARECT& rDownArea)
{
	switch(m_ssStyle)
	{
	case ASS_VER:
		{
			INT	nWidth	= m_rArea.GetWidth();
			INT nHeight	= nWidth;
			if(nHeight > (m_rArea.GetHeight() >> 1))
			{
				nHeight	= m_rArea.GetHeight() >> 1;
			}
			rUpArea.SetValue(0, 0, nWidth, nHeight);
			rDownArea.SetValue(0, m_rArea.GetHeight()-nHeight, nWidth, m_rArea.GetHeight());

			m_nScrollLength	= m_rArea.GetHeight() - nHeight*3;
		}
		break;
	case ASS_HOR:
		{
			INT	nHeight	= m_rArea.GetHeight();
			INT nWidth	= nHeight;
			if(nWidth > (m_rArea.GetWidth() >> 1))
			{
				nWidth	= m_rArea.GetWidth() >> 1;
			}
			rUpArea.SetValue(0, 0, nWidth, nHeight);
			rDownArea.SetValue(0, m_rArea.GetWidth()-nWidth, nWidth, m_rArea.GetWidth());

			if(m_rArea.GetWidth() > 3*nWidth)
			{
				rMidArea.SetValue(nWidth, 0, nWidth + nWidth, nHeight);
			}
			else
			{
				rMidArea.SetValue(0, 0, 0, 0);
			}
			m_nScrollLength	= m_rArea.GetWidth() - nWidth*3;
		}
		break;
	default:
		ADEBUG_TRACE("Invalid scrollbar style.");
		return;
		break;
	}
	AdjustMid(rMidArea);
}

bool AScrollbar::Release()
{
	ABASE_SAFE_RELEASE(m_psbUp);
	ABASE_SAFE_RELEASE(m_psbMid);
	ABASE_SAFE_RELEASE(m_psbDown);

	return AWnd::Release();
}

bool AScrollbar::OnDraw()
{
	APOINT	ptAb;
	GetAbsolutePos(ptAb);

	ARECT	rArea(ptAb.x, ptAb.y, ptAb.x + m_rArea.GetWidth(), ptAb.y + m_rArea.GetHeight());
	AWC::DrawRectangle(rArea, m_uBackColor);

	return true;
}

bool AScrollbar::OnCreate()
{
	assert(m_pParent != NULL);

	if(!AWnd::OnCreate())
	{
		return false;
	}

	ARECT rUpArea;
	ARECT rMidArea;
	ARECT rDownArea;
	
	AdjustSize(rUpArea, rMidArea, rDownArea);

	m_psbUp		= new AScrollButton;
	m_psbUp->m_sbsStyle	= AScrollButton::ASBS_UP;
	if(!m_psbUp->Create(this, rUpArea))
	{
		ADEBUG_TRACE("Cannot create up button.");
		return false;
	}

	m_psbMid	= new AScrollButton;
	m_psbMid->m_sbsStyle	= AScrollButton::ASBS_MID;
	if(!m_psbMid->Create(this, rMidArea))
	{
		ADEBUG_TRACE("Cannot create mid button.");
		return false;
	}

	m_psbDown	= new AScrollButton;
	m_psbDown->m_sbsStyle	= AScrollButton::ASBS_DOWN;
	if(!m_psbDown->Create(this, rDownArea))
	{
		ADEBUG_TRACE("Cannot create down button.");
		return false;
	}

	return true;
}