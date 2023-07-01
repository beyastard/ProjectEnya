/*
 * FILE: AWndGrid.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-4
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef __AWNDGRID_H__
#define __AWNDGRID_H__

#include "AWnd.h"
#include "AArray.h"

class AWndGrid : public AWnd
{
public:
	DECLARE_ASERIAL(AWndGrid)
protected:
	class AGridItem
	{
	public:
		AUString	m_strText;
		UINT		m_nGridData;
		AGridItem():
		m_nGridData(0)
		{
		}
	};

	class AItemRow
	{
	public:
		AArrayT<AGridItem*>	m_Items;
		~AItemRow()
		{
			for(int i = 0; i < m_Items.GetLength(); i ++)
			{
				if(m_Items[i] != NULL)
				{
					delete m_Items[i];
				}
			}
		}
		AGridItem& operator [] (int nIndex)
		{
			return *(m_Items[nIndex]);
		}
		void SetLength(int nLength)
		{
			if(nLength < m_Items.GetLength())
			{
				for(int i = nLength; i < m_Items.GetLength(); i ++)
				{
					if(m_Items[i] != NULL)
					{
						delete m_Items[i];
					}
				}
				m_Items.SetLength(nLength);
			}
			else if(nLength > m_Items.GetLength())
			{
				int i = m_Items.GetLength();
				m_Items.SetLength(nLength);
				for(; i < nLength; i ++)
				{
					m_Items[i]	= new AGridItem;
				}
			}
		}
		int GetLength()
		{
			return m_Items.GetLength();
		}
		void SetItemText(int nIndex, const char* pszText)
		{
			if(nIndex < 0 || nIndex >= m_Items.GetLength())
			{
				return;
			}
			m_Items[nIndex]->m_strText	= pszText;
		}
		void SetData(INT nIndex, UINT nData)
		{
			if(nIndex < 0 || nIndex >= m_Items.GetLength())
			{
				return;
			}
			m_Items[nIndex]->m_nGridData	= nData;
		}
		UINT GetData(INT nIndex)
		{
			if(nIndex < 0 || nIndex >= m_Items.GetLength())
			{
				return 0;
			}
			return m_Items[nIndex]->m_nGridData;
		}
	};

	class AGridData
	{
	public:
		AArrayT<AItemRow*>	m_Rows;
		AArrayT<int>		m_nGridWidth;
		int					m_nGridHeight;
		int					m_nColCount;
		AGridData():
		m_nColCount		(0),
		m_nGridHeight	(0)
		{
		}
		~AGridData()
		{
			for(int i = 0; i < m_Rows.GetLength(); i ++)
			{
				if(m_Rows[i] != NULL)
				{
					delete m_Rows[i];
				}
			}
		}
		AItemRow& operator [] (int nIndex)
		{
			return *(m_Rows[nIndex]);
		}
		int GetColCount()
		{
			return m_nColCount;
		}
		int GetRowCount()
		{
			return m_Rows.GetLength();
		}
		int GetGridWidth(int nColIndex)
		{
			return m_nGridWidth[nColIndex];
		}
		void SetGridWidth(int nColIndex, int nWidth)
		{
			if(nColIndex >= 0 && nColIndex < m_nColCount)
			{
				m_nGridWidth[nColIndex]	= nWidth;
			}
		}
		int GetGridHeight()
		{
			return m_nGridHeight;
		}
		void SetGridHeight(int nHeight)
		{
			m_nGridHeight	= nHeight;
		}
		void SetRowCount(int nCount)
		{
			if(nCount < m_Rows.GetLength())
			{
				for(int i = nCount; i < m_Rows.GetLength(); i ++)
				{
					if(m_Rows[i] != NULL)
					{
						delete m_Rows[i];
					}
				}
				m_Rows.SetLength(nCount);
			}
			else if(nCount > m_Rows.GetLength())
			{
				int i = m_Rows.GetLength();
				m_Rows.SetLength(nCount);
				for(; i < nCount; i ++)
				{
					m_Rows[i]	= new AItemRow;
				}
			}
		}
		void SetColCount(int nColCount)
		{
			int i;
			for(i = 0; i < m_Rows.GetLength(); i ++)
			{
				m_Rows[i]->SetLength(nColCount);
			}
			m_nGridWidth.SetLength(nColCount);
			if(m_nColCount < nColCount)
			{
				for(i = m_nColCount; i < nColCount; i ++)
				{
					m_nGridWidth[i] = 0;
				}
			}
			m_nColCount	= nColCount;
		}
		void SetGridCount(int nRowCount, int nColCount)
		{
			SetRowCount(nRowCount);
			SetColCount(nColCount);
		}
		void SetItemText(int nRowIndex, int nColIndex, const char* pszText)
		{
			if(nRowIndex < 0 || nRowIndex >= m_Rows.GetLength())
			{
				return;
			}
			m_Rows[nRowIndex]->SetItemText(nColIndex, pszText);
		}
		const char* GetItemText(int nRowIndex, int nColIndex)
		{
			if(nRowIndex < 0 || nRowIndex >= m_Rows.GetLength() ||
				nColIndex < 0 || nColIndex >= m_Rows[nRowIndex]->m_Items.GetLength())
			{
				return NULL;
			}
			return m_Rows[nRowIndex]->m_Items[nColIndex]->m_strText;
		}
		void SetData(int nRowIndex, int nColIndex, UINT nData)
		{
			if(nRowIndex < 0 || nRowIndex >= m_Rows.GetLength())
			{
				return;
			}
			m_Rows[nRowIndex]->SetData(nColIndex, nData);
		}
		UINT GetData(int nRowIndex, int nColIndex)
		{
			if(nRowIndex < 0 || nRowIndex >= m_Rows.GetLength())
			{
				return 0;
			}
			return m_Rows[nRowIndex]->GetData(nColIndex);
		}
	};

	int		m_nRowFirstVisible;
	AGridData	m_GridData;
	APOINT	m_ptSel;
	
	virtual bool	OnDraw();
	virtual bool	OnDrawGrid	(int nRow, int nCol, ARECT rGrid);
	virtual bool	OnMouseDown	(const APOINT& ptClick);
	virtual bool	OnSel(const APOINT& ptNewSel, bool& bCanSel);
public:
	AWndGrid();
	virtual ~AWndGrid();

	virtual bool LoadFromIni(AWnd* pParent, AWndIniFile& IniFile, const char* pszSection);
	virtual bool LoadRes	();
	virtual bool FreeRes	();
};

#endif	// __AWNDGRID_H__