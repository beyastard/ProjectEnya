#include "AMultiListBox.h"

IMPLEMENT_ASERIAL(AMultiListBox, AWnd, 0x0100)

AMultiListBox::AMultiListBox()
{
}

AMultiListBox::~AMultiListBox()
{
}

bool AMultiListBox::OnScroll(AScrollbar::AScrollMessage ScrollMsg, INT nLength, INT& nCurPos)
{
	return true;
}

bool AMultiListBox::OnCreate()
{
	if(!AWnd::OnCreate())
	{
		return false;
	}

	//m_Scroll.Create(

	return true;
}

bool AMultiListBox::LoadFromIni(AWnd* pParent, AWndIniFile& IniFile, const char* pszSection)
{
	if(!AWnd::LoadFromIni(pParent, IniFile, pszSection))
	{
		return false;
	}

	int nCount = 0;
	if(!IniFile.ReadInt(pszSection, "count_listbox", nCount))
	{
		return true;
	}

	return true;
}

void AMultiListBox::AdjustRect(ARECT& rArea, int nIndex, int nWidth)
{
	if(nIndex < 0 || nIndex > m_Lists.GetLength())
	{
		ADEBUG_TRACE("Invalid index parameter.");
		return;
	}
	int i;
	int nLeft	= 0;
	for(i = 0; i < nIndex; i ++)
	{
		nLeft	+= m_Lists[i]->GetArea().GetWidth();
	}
	rArea.SetValue(nLeft, 0, nLeft+nWidth, m_rArea.GetHeight());
}

void AMultiListBox::InsertCol(int nIndex, int nWidth)
{
	if(nIndex < 0 || nIndex > m_Lists.GetLength())
	{
		ADEBUG_TRACE("Invalid index parameter.");
		return;
	}

	AListBox* pList	= new AListBox;
	ARECT rArea;
	AdjustRect(rArea, nIndex, nWidth);
	pList->Create(this, rArea);
	m_Lists.InsertAt(nIndex, pList);
}

bool AMultiListBox::Release()
{
	int nCount = m_Lists.GetLength();
	for(int i = 0; i < nCount; i ++)
	{
		if(m_Lists[i] != NULL)
		{
			delete m_Lists[i];
		}
	}
	m_Lists.Release();

	return AWnd::Release();
}