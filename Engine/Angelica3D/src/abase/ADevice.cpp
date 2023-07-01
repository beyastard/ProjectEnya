// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ADevice.cpp
// Creator: Wei Hua (魏华)
// 设备基类

#include "AUList.h"
#include "ATrans.h"
#include "ADevice.h"

////////////////////////////////////////////////////////////////////
// ADevice1vs1
////////////////////////////////////////////////////////////////////
ADevice1vs1::ADevice1vs1()
{
	m_pTrans	= NULL;
}
ADevice1vs1::~ADevice1vs1()
{
}
void	ADevice1vs1::SetTrans(ATrans *pTrans)
{
	m_pTrans	= pTrans;
}
bool	ADevice1vs1::TranslateIn()
{
	bool	bval;
	bval	= true;
	if( m_pTrans )
	{
		bval	= m_pTrans->TranslateIn(this);
	}
	return	bval;
}
bool	ADevice1vs1::TranslateOut()
{
	bool	bval;
	bval	= true;
	if( m_pTrans )
	{
		bval	= m_pTrans->TranslateOut(this);
	}
	return	bval;
}

////////////////////////////////////////////////////////////////////
// ADevice1vsN
////////////////////////////////////////////////////////////////////
ADevice1vsN::ADevice1vsN()
{
	m_pTransList	= new AListT<ATrans *>;
}
ADevice1vsN::~ADevice1vsN()
{
	if( m_pTransList )
	{
		delete	m_pTransList;
		m_pTransList	= NULL;
	}
}
bool	ADevice1vsN::AddTrans(ATrans *pTrans)
{
	m_pTransList->AddTail(pTrans);
	return true;
}
bool	ADevice1vsN::RemoveTrans(ATrans *pTrans)
{
	AListPosition	pos = m_pTransList->Find(pTrans);
	if(pos != NULL)
	m_pTransList->RemoveAt(pos);
	return true;
}
bool	ADevice1vsN::TranslateIn()
{
	bool			bval;
	ATrans			* pATrans;

	AListPosition pos = m_pTransList->GetHeadPosition();
	while(pos)
	{
		pATrans = m_pTransList->GetNext(pos);
		bval	= pATrans->TranslateIn(this);		// 不过一环出错也不应该影响后面的翻译
	}
	return	true;
}
bool	ADevice1vsN::TranslateOut()
{
	bool			bval;
	ATrans			* pATrans;

	AListPosition pos = m_pTransList->GetHeadPosition();
	while(pos)
	{
		pATrans = m_pTransList->GetNext(pos);
		bval	= pATrans->TranslateOut(this);		// 不过一环出错也不应该影响后面的翻译
	}
	return	true;
}
