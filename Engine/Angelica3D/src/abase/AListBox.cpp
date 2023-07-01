#include "AListBox.h"
#include "AWC.h"

IMPLEMENT_ASERIAL(AListBox, AWnd, 0x0100)

AListBox::AListBox():
m_nItemHeight	(20),
m_nMarginHor	(5),
m_nMarginVer	(5),
m_nScrollWidth	(15),
m_nFirstVisible	(0),
m_nSelected		(-1),
m_nItemSpace	(3),
m_uItemBackColor(0xFFCCCCCC),
m_uItemSelColor	(0xFFFFCCCC)
{
	/*AListBoxItem* pItem	= new AListBoxItem;
	m_Items.InsertAt(0, pItem);*/
	m_uBackColor	= 0;
	m_bShowSel		= false;
	m_bScrollBar	= true;
}

AListBox::~AListBox()
{
	Release();
	/*if(m_Items[0] != NULL)
	{
		delete m_Items[0];
	}*/
}

void AListBox::SetItemBackColor(UINT uColor)
{
	m_uItemBackColor	= uColor;
}

void AListBox::SetItemSelColor(UINT uColor)
{
	m_uItemSelColor	= uColor;
}

UINT AListBox::GetItemBackColor()
{
	return m_uItemBackColor;
}

UINT AListBox::GetItemSelColor()
{
	return m_uItemSelColor;
}

int	 AListBox::GetMaxVisibleCount()
{
	//return (m_rArea.GetHeight() - m_nMarginVer)/m_nItemHeight;
	APOINT	ptAb;
	GetAbsolutePos(ptAb);


	ARECT	rScroll;
	ARECT	rItems;
	AdjustRect(rScroll, rItems);
	rItems.Move(ptAb);

	INT nIndex;
	INT nItemHeight	= m_nItemHeight;
	ARECT	rItemArea(
		rItems.left,
		rItems.top,
		rItems.right,
		rItems.top + nItemHeight);
	for(nIndex = 0; nIndex < 99999; nIndex ++)
	{
		if(rItemArea.bottom > rItems.bottom)
		{
			break;
		}
		rItemArea.MoveVer(nItemHeight);
	}
	return nIndex;
}

int  AListBox::GetItemCount()
{
	return m_Items.GetLength();
}

bool AListBox::SetArea(const ARECT& rArea, int nScrollWidth)
{
	AWnd::SetArea(rArea);
	if(nScrollWidth != -1)
	{
		m_nScrollWidth	= nScrollWidth;
	}
	ARECT	rScroll;
	ARECT	rItems;
	AdjustRect(rScroll, rItems);

	if (m_bScrollBar)
		m_Scroll.SetArea(rScroll);

	return true;
}

void AListBox::SetScrollWidth(int nWidth)
{
	m_nScrollWidth = nWidth;
	ARECT	rScroll;
	ARECT	rItems;
	AdjustRect(rScroll, rItems);

	if (m_bScrollBar)
		m_Scroll.SetArea(rScroll);
}

void AListBox::SetFirstVisible(int nVisible)
{
	if(nVisible <= 0)
	{
		m_nFirstVisible	= 0;
	}
	else if(nVisible >= m_Items.GetLength())
	{
		m_nFirstVisible	= m_Items.GetLength() - 1;
	}
	else
	{
		m_nFirstVisible	= nVisible;
	}
	if(m_Items.GetLength() > 0 && m_bScrollBar)
	{
		m_Scroll.SetScrollPos(
			m_nFirstVisible*m_Scroll.GetScrollLength()/m_Items.GetLength());
	}
}

bool AListBox::Release()
{
	INT nIndex;
	for(nIndex = 0; nIndex < m_Items.GetLength(); nIndex ++)
	{
		delete m_Items[nIndex];
		m_Items[nIndex]	= NULL;
	}
	/*m_Items.SetLength(1);
	m_Items[0]	= new AListBoxItem;*/

	return AClassBase::Release();
}

void AListBox::PageUp()
{
	m_nFirstVisible -= GetMaxVisibleCount();
	if(m_nFirstVisible < 0)
	{
		m_nFirstVisible	= 0;
	}
}

void AListBox::PageDown()
{
	m_nFirstVisible += GetMaxVisibleCount();
	if(m_nFirstVisible >= m_Items.GetLength())
	{
		m_nFirstVisible	= m_Items.GetLength() - 1;
	}
}

void AListBox::AdjustRect(ARECT& rScroll, ARECT& rItems)
{
	INT nsw	= m_nScrollWidth;
	INT	w	= m_rArea.GetWidth() - m_nMarginHor*2;
	if(w < nsw)
	{
		rItems.SetValue(0, 0, 0, 0);
		rScroll.SetValue(0, 0, 0, 0);
		return;
	}
	rItems.SetValue(
		m_nMarginHor,
		m_nMarginVer,
		m_nMarginHor + w - nsw,
		m_rArea.GetHeight() - m_nMarginVer);
	rScroll.SetValue(
		rItems.right,
		rItems.top, 
		m_nMarginHor + w,
		rItems.bottom);
}

bool AListBox::LoadFromIni(AWnd* pParent, AWndIniFile& IniFile, const char* pszSection)
{
	if(!AWnd::LoadFromIni(pParent, IniFile, pszSection))
	{
		return false;
	}

	int nLineCount = 0;
	if(!IniFile.ReadInt(pszSection, "count_line", nLineCount))
	{
		return true;
	}

	int nIndex;
	char buf[64];
	char nilstr[]	= "";
	for(nIndex = 0; nIndex < nLineCount; nIndex ++)
	{
		sprintf(buf, "line_%d", nIndex);
		const char* str	= NULL;
		if(!IniFile.ReadString(pszSection, buf, str))
		{
			str	= nilstr;
		}
		AppendString(str);
	}
	return true;
}

bool AListBox::OnMouseDown(const APOINT& ptClick)
{
	if(!AWnd::OnMouseDown(ptClick))
	{
		return false;
	}

	SetSel(ptClick);

	return true;
}

void AListBox::SetSel(const APOINT& ptSel)
{
	ARECT	rScroll;
	ARECT	rItems;
	AdjustRect(rScroll, rItems);
	int nOldSel	= m_nSelected;
	m_nSelected = m_nFirstVisible + (ptSel.y-rItems.top)/m_nItemHeight;
	if(m_nSelected >= m_Items.GetLength())
	{
		m_nSelected	= m_Items.GetLength()-1;
	}
	if(nOldSel != m_nSelected)
	{
		OnSel();
	}
}

bool AListBox::OnSel()
{
	return true;
}

bool AListBox::OnScroll(AScrollbar::AScrollMessage ScrollMsg, INT nLength, INT& nCurPos)
{
	if(m_Items.GetLength() <= 1 || nLength <= 0)
	{
		nCurPos	= 0;
		return true;
	}
	switch(ScrollMsg)
	{
	case AScrollbar::ASM_STEP_UP:
		if(m_nFirstVisible > 0)
		{
			m_nFirstVisible	--;
		}
		nCurPos	= m_nFirstVisible*nLength/(m_Items.GetLength()-1);
		break;
	case AScrollbar::ASM_STEP_DOWN:
		if(m_nFirstVisible < (m_Items.GetLength()-1))
		{
			m_nFirstVisible ++;
		}
		nCurPos	= m_nFirstVisible*nLength/(m_Items.GetLength()-1);
		break;
	case AScrollbar::ASM_SET_POSITION:
		m_nFirstVisible	= nCurPos*(m_Items.GetLength())/nLength;
		if(m_nFirstVisible >= m_Items.GetLength())
		{
			m_nFirstVisible = m_Items.GetLength()-1;
		}
		break;
	default:
		break;
	}

	return true;
}

bool AListBox::OnCreate()
{
	if(!AClassBase::OnCreate())
	{
		return false;
	}

	ARECT	rItems;

	if (m_bScrollBar)
	{
		ARECT	rScroll;
		AdjustRect(rScroll, rItems);
		if(!m_Scroll.Create(this, rScroll))
		{
			ADEBUG_TRACE("Cannot create scrollbar for list box.");
			return false;
		}

		m_Scroll.SetScrollHandler((AScrollbar::AScrollHandler)&AListBox::OnScroll);
	}

	//m_Items[0]->m_strText = "asdf";
	/*m_Items.InsertAt(0, new AListBoxItem("qewrewrt"));
	m_Items.InsertAt(1, new AListBoxItem("qewrewrt"));
	m_Items.InsertAt(2, new AListBoxItem("asdf"));
	m_Items.InsertAt(3, new AListBoxItem("dfgfg"));
	m_Items.InsertAt(4, new AListBoxItem("gjhdj"));
	m_Items.InsertAt(5, new AListBoxItem("kl;';"));
	m_Items.InsertAt(6, new AListBoxItem("rtyry"));
	m_Items.InsertAt(7, new AListBoxItem("hjkjh"));
	m_Items.InsertAt(8, new AListBoxItem("860909"));
	m_Items.InsertAt(9, new AListBoxItem("90-0=0-="));
	m_Items.InsertAt(10, new AListBoxItem("2362626"));
	m_Items.InsertAt(11, new AListBoxItem("qewurtrewrt"));
	m_Items.InsertAt(12, new AListBoxItem("er"));
	m_Items.InsertAt(13, new AListBoxItem("werytwre"));
	m_Items.InsertAt(14, new AListBoxItem("poipoioipoipoip"));*/

	return true;
}

void AListBox::OnDestroy()
{
	AClassBase::OnDestroy();
}

bool AListBox::OnDraw()
{
	APOINT	ptAb;
	GetAbsolutePos(ptAb);

	ARECT	rBack(
		ptAb.x, ptAb.y,
		ptAb.x + m_rArea.GetWidth(),
		ptAb.y + m_rArea.GetHeight());
	if(m_uBackColor != 0)
	{
		AWC::DrawRectangle(rBack, m_uBackColor);
	}

	ARECT	rScroll;
	ARECT	rItems;
	AdjustRect(rScroll, rItems);
	rItems.Move(ptAb);

	INT nIndex;
	INT nItemHeight	= m_nItemHeight;
	ARECT	rItemArea(
		rItems.left,
		rItems.top,
		rItems.right,
		rItems.top + nItemHeight);
	for(nIndex = m_nFirstVisible; nIndex < m_Items.GetLength(); nIndex ++)
	{
		if(rItemArea.bottom > rItems.bottom)
		{
			break;
		}
		OnDrawItem(rItemArea, nIndex);
		rItemArea.MoveVer(nItemHeight);
	}

	return true;
}

bool AListBox::OnDrawItem(const ARECT& rAbArea, INT nIndex)
{
	assert(nIndex < m_Items.GetLength());

	UINT uBackColor;
	if (m_bShowSel && m_nSelected == nIndex)
	{
		uBackColor	= m_uItemSelColor;
	}
	else
	{
		uBackColor	= m_uBackColor;
	}

	if (uBackColor != 0)
	{
		AWC::DrawRectangle(rAbArea, uBackColor);
	}
	
	int nFont = AWC::GetCurrentFont();
	AWC::SetCurrentFont(GetTextFont());
	APOINT ptText(rAbArea.left, rAbArea.top + m_nItemSpace);
	UINT color;
	if(m_Items[nIndex]->m_bOwnColor)
		color = m_Items[nIndex]->m_nStrColor;
	else
		color = m_uTextColor;
	if(m_Items[nIndex]->m_bFilledArea)
	{
		APOINT pt;
		AWC::GetTextExtent("A",1,pt);
		if(m_Items[nIndex]->m_strText.GetLength()*pt.x>=rAbArea.GetWidth() || m_Items[nIndex]->m_strText.GetLength()==0)
			AWC::DrawText(ptText, m_Items[nIndex]->m_strText, -1, color);
		else
		{
			int nWidth = rAbArea.GetWidth()/m_Items[nIndex]->m_strText.GetLength();
			int nLen = m_Items[nIndex]->m_strText.GetLength();
			AUString str = m_Items[nIndex]->m_strText;
			char ch[4096];
			for(int i=0;i<nLen;i++)
			{
				if(str.GetAt(i,ch[0])==false) return true;
				if(ch[0]<0)
				{
					str.GetAt(++i,ch[1]);
					ch[2] = 0;
					AWC::DrawText(ptText,ch, -1, color);
					ptText.x += (2*nWidth);
				}
				else if((ch[0]>='a' && ch[0]<='z') || (ch[0]>='A' && ch[0]<='Z'))
				{
					char c;
					int j;
					for(j=1;i+j<nLen;j++)
					{
						str.GetAt(i+j,c);
						if((c>='a' && c<='z') || (c>='A' && c<='Z'))
						{
							ch[j] = c;
						}
						else
						{
							i = i+j-1;
							break;
						}
					}
					ch[j] = 0;
					AWC::DrawText(ptText,ch, -1, color);
					ptText.x += (j*nWidth);
				}
				else
				{
					ch[1] = 0;
					AWC::DrawText(ptText,ch, -1, color);
					ptText.x += nWidth;
				}
			}
		}
	}
	else
	{
		AWC::DrawText(ptText, m_Items[nIndex]->m_strText, -1, color);
	}

	AWC::SetCurrentFont(nFont);
	return true;
}

void AListBox::AppendString(const char* pszStr)
{
	m_Items.InsertAt(m_Items.GetLength(), new AListBoxItem(pszStr));
}

void AListBox::InsertString(INT nIndex, const char* pszStr)
{
	assert(nIndex >= 0 && nIndex < m_Items.GetLength());

	m_Items.InsertAt(nIndex, new AListBoxItem(pszStr));
}

void AListBox::ReplaceString(INT nIndex, const char* pszStr)
{
	assert(nIndex >= 0 && nIndex < m_Items.GetLength());

	if(m_Items[nIndex] != NULL)
	{
		delete m_Items[nIndex];
	}
	m_Items[nIndex]	= new AListBoxItem(pszStr);
}

void AListBox::RemoveString(INT nIndex)
{
	assert(nIndex >= 0 && nIndex < m_Items.GetLength());

	if(m_Items[nIndex] != NULL)
	{
		delete m_Items[nIndex];
	}
	m_Items.RemoveAt(nIndex);
}

void AListBox::SetMargin(INT nHor, INT nVer)
{
	m_nMarginHor	= nHor;
	m_nMarginVer	= nVer;
}

void AListBox::GetMargin(INT& nHor, INT& nVer)
{
	nHor	= m_nMarginHor;
	nVer	= m_nMarginVer;
}

void AListBox::SetItemHeight(INT nHeight)
{
	m_nItemHeight	= nHeight;
}

INT	 AListBox::GetItemHeight()
{
	return m_nItemHeight;
}

void AListBox::AppendString(const char *pszStr, UINT color)
{
	m_Items.InsertAt(m_Items.GetLength(), new AListBoxItem(pszStr,color));
}

void AListBox::LineDown()
{
	m_nFirstVisible++;
	if(m_nFirstVisible > m_Items.GetLength()-GetMaxVisibleCount())
	{
		m_nFirstVisible	= m_Items.GetLength() - GetMaxVisibleCount();
	}
}

void AListBox::LineUp()
{
	m_nFirstVisible--;
	if(m_nFirstVisible < 0)
	{
		m_nFirstVisible	= 0;
	}
}

bool AListBox::RemoveAll()
{
	INT nIndex;
	for(nIndex = 0; nIndex < m_Items.GetLength(); nIndex ++)
	{
		delete m_Items[nIndex];
		m_Items[nIndex]	= NULL;
	}
	m_Items.SetLength(0);
	m_nFirstVisible = 0;
	return true;
}

void AListBox::AppendString(const char *pszStr, UINT color, bool bFill)
{
	m_Items.InsertAt(m_Items.GetLength(), new AListBoxItem(pszStr,color,bFill));
}
