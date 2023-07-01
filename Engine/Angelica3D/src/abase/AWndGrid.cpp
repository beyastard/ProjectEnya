#include "AWndGrid.h"
#include "AWC.h"

IMPLEMENT_ASERIAL(AWndGrid, AWnd, 0x0100)

AWndGrid::AWndGrid() : 
m_nRowFirstVisible	(0),
m_ptSel				(0, 0)
{
}

AWndGrid::~AWndGrid()
{
}
	
bool AWndGrid::OnDrawGrid(int nRow, int nCol, ARECT rGrid)
{
	UINT nTextColor;
	if(m_ptSel.x == nCol && m_ptSel.y == nRow)
	{
		nTextColor	= 0xFFFF0000;
	}
	else
	{
		nTextColor	= 0xFF000000;
	}
	AWC::DrawText(rGrid.GetLeftTop(), 
		m_GridData.GetItemText(nRow, nCol),
		-1, nTextColor);
	return true;
}

bool AWndGrid::OnDraw()
{
	ARECT rBack;
	GetAbsoluteArea(rBack);
	AWC::DrawRectangle(rBack, 0xFFFFFFFF);

	ARECT rAb;
	GetAbsoluteArea(rAb);
	ARECT rGrid(rAb.left, rAb.top, rAb.right, m_GridData.GetGridHeight());
	int r = m_nRowFirstVisible;
	for(; r < m_GridData.GetRowCount(); r ++)
	{
		if(rGrid.bottom > rAb.bottom)
			break;
		int c;
		rGrid.left	= rAb.left;
		for(c = 0; c < m_GridData.GetColCount(); c ++)
		{
			if(rGrid.left >= rAb.right)
				break;

			rGrid.SetWidth(m_GridData.GetGridWidth(c));

			if(rGrid.right >= rAb.right)
			{
				rGrid.right	= rAb.right;
			}
			OnDrawGrid(r, c, rGrid);
			rGrid.left	+= m_GridData.GetGridWidth(c);
		}
		rGrid.MoveVer(m_GridData.GetGridHeight());
	}
	return true;
}

bool AWndGrid::OnMouseDown(const APOINT& ptClick)
{
	if(!AWnd::OnMouseDown(ptClick))
	{
		return false;
	}

	int c, r;
	int x = ptClick.x;
	for(c = 0; c < m_GridData.GetColCount(); c ++)
	{
		x -= m_GridData.GetGridWidth(c);
		if(x < 0)
		{
			break;
		}
	}
	if(c >= m_GridData.GetColCount() && c > 0)
	{
		c = m_GridData.GetColCount() - 1;
	}

	if(m_GridData.GetGridHeight() != 0)
	{
		r	= m_nRowFirstVisible + ptClick.y / m_GridData.GetGridHeight();
	}
	else
	{
		r	= 0;
	}

	if(r < m_nRowFirstVisible)
	{
		r	= m_nRowFirstVisible;
	}
	if(r >= m_GridData.GetRowCount())
	{
		r = m_GridData.GetRowCount() - 1;
	}

	APOINT	ptNewSel(c, r);
	bool	bCanSel	= true;
	if(!OnSel(ptNewSel, bCanSel))
	{
		return false;
	}
	else
	{
		if(bCanSel)
		{
			m_ptSel.x	= c;
			m_ptSel.y	= r;
		}
	}

	return true;
}

bool AWndGrid::OnSel(const APOINT& ptNewSel, bool& bCanSel)
{
	bCanSel	= true;
	return true;
}

bool AWndGrid::LoadFromIni(AWnd* pParent, AWndIniFile& IniFile, const char* pszSection)
{
	if(!AWnd::LoadFromIni(pParent, IniFile, pszSection))
	{
		return false;
	}
	int nRowCount, nColCount;
	if(!IniFile.ReadInt(pszSection, "count_row", nRowCount))
	{
		ADEBUG_TRACE1("Cannot read count_row in section <%s>.", pszSection);
		nRowCount = 0;
	}
	if(!IniFile.ReadInt(pszSection, "count_col", nColCount))
	{
		ADEBUG_TRACE1("Cannot read count_col in section <%s>.", pszSection);
		nRowCount = 0;
	}
	m_GridData.SetGridCount(nRowCount, nColCount);

	int r, c;
	char szKeyBuf[64];
	for(r = 0; r < nRowCount; r ++)
	{
		for(c = 0; c < nColCount; c ++)
		{
			sprintf(szKeyBuf, "string_%d_%d", r, c);
			const char* pszStr	= NULL;
			if(IniFile.ReadString(pszSection, szKeyBuf, pszStr))
			{
				m_GridData.SetItemText(r, c, pszStr);
			}
		}
	}
	for(c = 0; c < nColCount; c ++)
	{
		int nWidth;
		sprintf(szKeyBuf, "width_col_%d", c);
		if(!IniFile.ReadInt(pszSection, szKeyBuf, nWidth))
		{
			nWidth	= 0;
		}
		m_GridData.SetGridWidth(c, nWidth);
	}

	int nHeight;
	if(!IniFile.ReadInt(pszSection, "height_row", nHeight))
	{
		nHeight	= 0;
	}
	m_GridData.SetGridHeight(nHeight);

	return true;
}

bool AWndGrid::LoadRes()
{
	if(!AWnd::LoadRes())
	{
		return false;
	}
	return true;
}

bool AWndGrid::FreeRes()
{
	bool bRet	= true;
	return bRet && AWnd::FreeRes();
}