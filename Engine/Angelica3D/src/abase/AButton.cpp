#include "AButton.h"
#include "AWC.h"


IMPLEMENT_ASERIAL(AButton, AWnd, 0x0100)

AButton::AButton():
m_bPushed			(false),
m_bHovered			(false),
m_bCheckedButton	(false),
m_bContinuousClick	(false),
m_nLastClick		(0),
m_nFirstDelay		(300),
m_nClickDelay		(100),
m_nDblDelay			(400),
m_nDblFirst			(0)
{
	m_bCanBeFocus	= true;

}

AButton::~AButton()
{
}

bool AButton::OnMouseMsg(AWndMouseMsg& msg)
{
	if(!AWnd::OnMouseMsg(msg))
	{
		return false;
	}

	if(!m_bEnabled)
	{
		return true;
	}

	bool bDown		= false;
	bool bChanged	= false;
	msg.GetButtonStatus(AMB_LEFT, bDown, bChanged);

	if(bChanged)
	{
		if(bDown)
		{
			if(!m_bCheckedButton)
			{
				m_bPushed	= true;
			}
			if(m_bContinuousClick)
			{
				m_nLastClick	= GetTickCount();
			}
		}
		else
		{
			if(!m_bCheckedButton)
			{
				if(m_bPushed)
				{
					m_bPushed	= false;
					OnClick();
				}
			}
			else
			{
				m_bPushed	= !m_bPushed;
				OnClick();
			}
			UINT nCurr	= ::GetTickCount();
			if(nCurr - m_nDblFirst < m_nDblDelay)
			{
				OnDblClick();
			}
			else
			{
				m_nDblFirst	= nCurr;
			}
		}
	}
	else if(bDown)
	{
		if(m_bContinuousClick && IsPushed())
		{
			int nDelay	= GetTickCount() - m_nLastClick - m_nFirstDelay;
			if(nDelay > (int)m_nClickDelay)
			{
				m_nLastClick += m_nClickDelay;
				OnClick();
			}
		}
	}

	return true;
}

bool AButton::OnClick()
{
	if(!m_wnOnClick.IsNil())
	{
		return m_wnOnClick(this);
	}
	return true;
}

bool AButton::OnDblClick()
{
	if(!m_wnOnDblClick.IsNil())
	{
		return m_wnOnDblClick(this);
	}
	return true;
}

bool AButton::OnDraw()
{
	if(!AWnd::OnDraw())
	{
		return false;
	}
	return true;
}

bool AButton::OnMouseHover()
{
	if(!AWnd::OnMouseHover())
	{
		return false;
	}

	m_bHovered	= true;

	return true;
}

bool AButton::OnMouseLeave()
{
	if(!AWnd::OnMouseLeave())
	{
		return false;
	}

	m_bHovered	= false;
	if(!m_bCheckedButton)
	{
		m_bPushed	= false;
	}

	return true;
}
