#include "AWnd.h"
#include "A3DStringSet.h"
#include "AWC.h"
#include "ADeviceMouse.h"

IMPLEMENT_ASERIAL(AWnd, AVisualUnit, 0x0100)

AWnd::AWnd():
m_bCreated		(true),
m_bEnabled		(true),
m_bVisible		(true),
m_bCanBeFocus	(false),
m_bKeyPreview	(false),
m_bOwnerDraw	(false),
m_nTextFont		(-1),
m_uTextColor	(0xFF000000),
m_uBackColor	(0xFFFFFFFF),
m_rArea			(0, 0, 100, 100),
m_pParent		(NULL),
m_pActiveChild	(NULL),
m_pssToolTip	(NULL)
{

}

AWnd::~AWnd()
{
}

bool AWnd::SetArea(const ARECT& rArea)
{
	m_rArea	= rArea;
	return OnSize();
}

bool AWnd::PointIn(const APOINT& pt)
{
	return m_rArea && pt;
}

AWnd* AWnd::FindChild(const APOINT& pt)
{
	APOINT ptAb;
	GetAbsolutePos(ptAb);
	APOINT p = pt - ptAb;
	AListPosition Pos = m_lstChilds.GetTailPosition();
	while(Pos != NULL)
	{
		AWnd* pChild = m_lstChilds.GetPrev(Pos);
		assert(pChild != NULL);
		
		if(pChild->IsVisible() && pChild->PointIn(p))
		{
			return pChild;
		}
	}
	return NULL;
}

bool AWnd::Release()
{
	if(m_bCreated)
	{
		OnDestroy();
	}

	FreeRes();

	while(m_lstChilds.GetCount() > 0)
	{
		AWnd* pWnd = m_lstChilds.GetHead();
		pWnd->Release();
		// do not delete the pWnd, let user do it.
	}

	if(m_pssToolTip != NULL)
	{
		m_pssToolTip->Release();
		delete m_pssToolTip;
		m_pssToolTip	= NULL;
	}

	if(m_bCreated)
	{
		if(m_pParent)
		{
			m_pParent->RemoveChild(this);
		}

		AWC::GetWC()->OnWndRelease(this);
	}

	return AVisualUnit::Release();
}

bool AWnd::SetAsActive(bool bActive)
{
	if(m_pParent == NULL)
	{
		return true;
	}

	if(!bActive)
	{
		if(IsActived())
		{
			m_pParent->TabActiveChild();
		}
	}
	else
	{
		if(IsTabStoped())
		{
			m_pParent->m_pActiveChild	= this;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void AWnd::TabActiveChild()
{
	AListPosition Pos = NULL;
	if(m_pActiveChild == NULL)
	{
		Pos	= m_lstChilds.GetHeadPosition();
	}
	else
	{
		Pos = m_lstChilds.Find(m_pActiveChild);
	}

	while(Pos != NULL)
	{
		AWnd* pChild	= m_lstChilds.GetNext(Pos);
		if(pChild->SetAsActive(true))
		{
			break;
		}
	}
}

bool AWnd::IsActived()
{
	if(m_pParent != NULL)
	{
		if(m_pParent->m_pActiveChild == this)
		{
			return true;
		}
	}
	return false;
}

bool AWnd::IsAncestorOf(AWnd* pDescendant)
{
	if(this == pDescendant)
	{
		return false;
	}

	AWnd* pWnd	= pDescendant;
	while(pWnd != NULL)
	{
		if(pWnd == this)
		{
			return true;
		}
		pWnd	= pWnd->GetParent();
	}

	return false;
}

bool AWnd::AddChild(AWnd* pWnd)
{
	assert(pWnd != NULL);

	if(m_lstChilds.Find(pWnd) != NULL)
	{
		return true;
	}

	m_lstChilds.AddTail(pWnd);
	pWnd->m_pParent	= this;

	if(m_pActiveChild == NULL)
	{
		pWnd->SetAsActive(true);
	}

	return true;
}

bool AWnd::RemoveChild(AWnd* pWnd)
{
	assert(pWnd != NULL);

	if(m_pActiveChild == pWnd)
	{
		TabActiveChild();
		if(m_pActiveChild == pWnd)
		{
			m_pActiveChild	= NULL;
		}
	}

	AListPosition Pos = m_lstChilds.Find(pWnd);
	if(Pos != NULL)
	{
		m_lstChilds.RemoveAt(Pos);
	}

	pWnd->m_pParent	= NULL;

	return true;
}

bool AWnd::OnCreate()
{
	return true;
}

void AWnd::OnDestroy()
{

}

bool AWnd::OnSize()
{
	return true;
}

bool AWnd::OnSetFocus(bool bFocus)
{
	return true;
}

bool AWnd::OnMouseMsg(AWndMouseMsg& Msg)
{
	bool bDown, bChanged;
	Msg.GetButtonStatus(AMB_LEFT, bDown, bChanged);
	if(bDown && bChanged)
	{
		APOINT ptAb, ptMouse;
		GetAbsolutePos(ptAb);
		Msg.GetPos(ptMouse);
		APOINT ptClick(ptMouse.x-ptAb.x, ptMouse.y-ptAb.y);
		return OnMouseDown(ptClick);
	}
	return true;
}

bool AWnd::OnMouseHover()
{
	return true;
}

bool AWnd::OnMouseLeave()
{
	return true;
}

bool AWnd::OnDraw()
{
	return true;
}

bool AWnd::OnSetTextFont(int nFont)
{
	m_nTextFont	= nFont;
	return true;
}

bool AWnd::OnKeyboardMsg(AWndKeyboardMsg& Msg, bool& bAccept)
{
	return true;
}

bool AWnd::OnMouseDown(const APOINT& ptClick)
{
	return true;
}

bool AWnd::IsTabStoped()
{
	return m_bEnabled && m_bVisible && m_bCanBeFocus;
}

bool AWnd::Create(AWnd* pParent, const ARECT& rArea)
{
	BindGame(AWC::GetWC()->GetGame());
	if(pParent != NULL)
	{
		if(!pParent->AddChild(this))
		{
			return false;
		}
	}

	m_rArea	= rArea;

	if(!OnCreate())
	{
		if(pParent != NULL)
		{
			pParent->RemoveChild(this);
		}
		OnDestroy();
		return false;
	}

	m_bCreated	= true;

	return true;
}

bool AWnd::LoadFromIni(AWnd* pParent, AWndIniFile& IniFile, const char* pszSection)
{
	ARECT rArea;
	if(!IniFile.ReadRect(pszSection, "rect_window", rArea))
	{
		ADEBUG_TRACE("Cannot load window rect.");
		return false;
	}

	if(!Create(pParent, rArea))
	{
		ADEBUG_TRACE1("Cannot create window. %s", pszSection);
		return false;
	}

	return true;
}

bool AWnd::ShowWindow(bool bShow)
{
	m_bVisible	= bShow;
	return true;
}

bool AWnd::SetParent(AWnd* pParent)
{
	if(m_pParent != NULL)
	{
		m_pParent->RemoveChild(this);
	}
	m_pParent = pParent;
	if(m_pParent != NULL)
	{
		m_pParent->AddChild(this);
	}
	return true;
}

void AWnd::GetAbsoluteArea(ARECT& rArea)
{
	GetAbsolutePos(rArea.GetLeftTop());

	rArea.SetWidth(m_rArea.GetWidth());
	rArea.SetHeight(m_rArea.GetHeight());
}

void AWnd::GetAbsolutePos(APOINT& ptPos)
{
	ptPos	= m_rArea.GetLeftTop();
	AWnd* pParent = m_pParent;
	while(pParent != NULL)
	{
		ptPos	+= pParent->m_rArea.GetLeftTop();
		pParent	= pParent->m_pParent;
	}
}

bool AWnd::TickAnimation(float fTimeSpan)
{
	AListPosition Pos = m_lstChilds.GetHeadPosition();
	while (Pos)
	{
		AWnd* pChild = m_lstChilds.GetNext(Pos);
		assert(pChild != NULL);
		pChild->TickAnimation(fTimeSpan);
	}

	return true;
}

bool AWnd::MoveTo(const APOINT& ptNewPos)
{
	m_rArea.MoveTo(ptNewPos);
	return true;
}

bool AWnd::SetCapture(bool bCapMouse)
{
	if(IsVisible())
	{
		if(bCapMouse)
		{
			AWC::GetWC()->SetCapture(this);
		}
		else
		{
			AWC::GetWC()->SetCapture(NULL);
		}
		return true;
	}
	return false;
}

bool AWnd::LoadRes()
{
	AListPosition Pos = m_lstChilds.GetHeadPosition();
	bool bResult	= true;
	while(Pos != NULL)
	{
		AWnd* pChild = m_lstChilds.GetNext(Pos);
		assert(pChild != NULL);
		if(!pChild->LoadRes())
		{
			bResult	= false;
		}
	}
	return bResult;
}

bool AWnd::FreeRes()
{
	AListPosition Pos = m_lstChilds.GetHeadPosition();
	while(Pos != NULL)
	{
		AWnd* pChild = m_lstChilds.GetNext(Pos);
		assert(pChild != NULL);
		if(!pChild->FreeRes())
		{
			return false;
		}
	}
	return true;
}

bool AWnd::Redraw()
{
	if(!IsVisible())
	{
		return true;
	}
	if(m_bOwnerDraw)
	{
		return true;
	}

	if(!OnDraw())
	{
		ADebug::Msg(0, "Cannot redraw AWnd class : %s", this->GetClassInfo()->GetClassName());
		return false;
	}

	AListPosition Pos = m_lstChilds.GetHeadPosition();
	while(Pos != NULL)
	{
		AWnd* pChild = m_lstChilds.GetNext(Pos);
		assert(pChild != NULL);
		if(!pChild->Redraw())
		{
			return false;
		}
	}

	if(strcmp(this->GetClassInfo()->GetBaseClass()->GetClassName(), "AWndConsole")==0)
	{
		//strcpy(NULL, NULL);
		return true;
	}
	return true;
}

bool AWnd::ProcessKeyboardMsg(AWndKeyboardMsg& Msg, bool& bAccept)
{
	AWnd* pParent	= m_pParent;
	if(m_bKeyPreview)
	{
		OnKeyboardMsg(Msg, bAccept);
		if(bAccept)
		{
			return true;
		}
	}
	while(pParent != NULL)
	{
		if(pParent->m_bKeyPreview)
		{
			if(!pParent->ProcessKeyboardMsg(Msg, bAccept))
			{
				return false;
			}
			if(bAccept)
			{
				return true;
			}
			break;
		}
		pParent = pParent->m_pParent;
	}
	if(m_bKeyPreview)
	{
		return true;
	}
	return OnKeyboardMsg(Msg, bAccept);
}

bool AWnd::ProcessMouseMsg(AWndMouseMsg& Msg)
{
	AWnd* pCap = AWC::GetWC()->GetCapture();
	if(pCap != NULL)
	{
		if(pCap != this)
		{
			if(!pCap->IsAncestorOf(this))
			{
				return pCap->ProcessMouseMsg(Msg);
			}
		}
	}

	APOINT Pos;
	Msg.GetPos(Pos);

	//ADEBUG_TRACE2("pos : %d , %d ", Pos.x, Pos.y);
	
	AWnd* pWnd = FindChild(Pos);
	if(pWnd == NULL)
	{
		pWnd	= this;
	}

	//
	if(pWnd == this)
	{
		AWC* pWC	= AWC::GetWC();
		if(pWnd != pWC->GetLastHover())
		{
			if(!pWC->SetLastHover(pWnd))
			{
				ADebug::Msg(0, "Cannot set hover window.");
			}
		}
		bool bDown, bChanged;
		Msg.GetButtonStatus(AMB_LEFT, bDown, bChanged);
		if(bDown && bChanged)
		{
			if(pWnd->IsCanBeFocus())
			{
				AWC::GetWC()->SetFocus(pWnd);
			}
		}
		if(!pWnd->OnMouseMsg(Msg))
		{
			return false;
		}
	}
	else
	{
		if(!pWnd->ProcessMouseMsg(Msg))
		{
			return false;
		}
	}

	return true;
}
