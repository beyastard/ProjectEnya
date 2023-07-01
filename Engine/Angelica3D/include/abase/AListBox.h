/*
 * FILE: AListBox.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-4
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__ALISTBOX_H__
#define	__ALISTBOX_H__

#include "AWnd.h"
#include "AArray.h"
#include "AScrollbar.h"

class AListBox : public AWnd
{
public:
	DECLARE_ASERIAL(AListBox)
private:
	typedef AWnd AClassBase;
public:
	class AListBoxItem
	{
	public:
		AUString	m_strText;
		UINT		m_nStrColor;
		bool		m_bOwnColor;
		bool		m_bFilledArea;//如果文字比一个ITEM的区域小，则自动调整间距来适应这个区域的宽
		AListBoxItem()
		{
			m_bOwnColor = false;
			m_bFilledArea = false;
		}
		AListBoxItem(const char* pszText,UINT strColor):
		m_strText	(pszText) , m_nStrColor(strColor)
		{
			m_bOwnColor = true;
			m_bFilledArea = false;
		}
		AListBoxItem(const char* pszText):
		m_strText	(pszText)
		{
			m_bOwnColor = false;
			m_bFilledArea = false;
		}
		AListBoxItem(const char* pszText,UINT strColor,bool bFilledAread):
		m_strText	(pszText) , m_nStrColor(strColor) ,m_bFilledArea(bFilledAread)
		{
			m_bOwnColor = true;
		}
	};

	AArrayT<AListBoxItem*> m_Items;
	INT		m_nItemHeight;
	INT		m_nItemSpace;
	INT		m_nMarginHor;
	INT		m_nMarginVer;
	INT		m_nScrollWidth;
	INT		m_nFirstVisible;
	INT		m_nSelected;
	UINT	m_uItemBackColor;
	UINT	m_uItemSelColor;
	bool	m_bShowSel;
	bool	m_bScrollBar;

	AScrollbar	m_Scroll;

	void	AdjustRect(ARECT& rScroll, ARECT& rItems);

	virtual bool Release();

	virtual bool OnScroll	(AScrollbar::AScrollMessage ScrollMsg, INT nLength, INT& nCurPos);
	virtual bool OnCreate	();
	virtual void OnDestroy	();
	virtual bool OnDraw		();
	virtual bool OnDrawItem	(const ARECT& rAbArea, INT nIndex);
	virtual bool OnMouseDown(const APOINT& ptClick);
	virtual bool OnSel		();

	void	ShowSelectedItem(bool bShow)	{	m_bShowSel = bShow;		}

public:
	virtual void AppendString(const char *pszStr, UINT color,bool bFill);
	virtual void AppendString(const char* pszStr,UINT color);
	virtual void AppendString(const char* pszStr);
	bool RemoveAll();
	void LineUp();
	void LineDown();
	AListBox();
	virtual ~AListBox();

	void InsertString(INT nIndex, const char* pszStr);
	void ReplaceString(INT nIndex, const char* pszStr);
	void RemoveString(INT nIndex);
	void SetMargin(INT nHor, INT nVer);
	void GetMargin(INT& nHor, INT& nVer);
	void SetItemHeight(INT nHeight);
	int	 GetItemHeight();
	int	 GetMaxVisibleCount();
	int  GetItemCount();
	void SetFirstVisible(int nVisible);
	void SetScrollWidth(int nWidth);
	void SetItemBackColor(UINT uColor);
	void SetItemSelColor(UINT uColor);
	UINT GetItemBackColor();
	UINT GetItemSelColor();
	void PageUp();
	void PageDown();
	void SetSel(const APOINT& ptSel);
	AScrollbar& GetScroll() { return m_Scroll; }

	virtual bool SetArea(const ARECT& rArea, int nScrollWidth = -1);
	virtual bool LoadFromIni(AWnd* pParent, AWndIniFile& IniFile, const char* pszSection);
};


#endif	// __ALISTBOX_H__