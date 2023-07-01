// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ATrans.h
// Creator: Wei Hua (魏华)
// 翻译器基类

#ifndef	__ATRANS_H__
#define	__ATRANS_H__

#include "AUnit.h"

class	ADevice;

class	ATrans	: public AUnit
{
protected:
	int			m_nDestID;							// 目标游戏单元的id
public:
	ATrans()		{ m_nDestID = -1; }
	virtual bool	TranslateIn(ADevice *pADevice)	{ return true; }
	virtual bool	TranslateOut(ADevice *pADevice)	{ return true; }
};

#endif