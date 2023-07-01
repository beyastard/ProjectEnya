/*
 * FILE: AMultiListBox.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-16
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AMULTILISTBOX_H__
#define	__AMULTILISTBOX_H__

#include "AListBox.h"
#include "AArray.h"

class AMultiListBox : public AWnd
{
public:
	DECLARE_ASERIAL(AMultiListBox)
protected:
	AArrayT<AListBox*>	m_Lists;
	AScrollbar			m_Scroll;
	int					m_nScrollWidth;

	void AdjustRect(ARECT& rArea, int nIndex, int nWidth);

	virtual bool OnCreate();
	virtual bool OnScroll(AScrollbar::AScrollMessage ScrollMsg, INT nLength, INT& nCurPos);
public:
	AMultiListBox();
	virtual ~AMultiListBox();

	void InsertCol(int nIndex, int nWidth);

	virtual bool Release();
	virtual bool LoadFromIni(AWnd* pParent, AWndIniFile& IniFile, const char* pszSection);
};



#endif	// __AMULTILISTBOX_H__