/*
 * FILE: AVariant.cpp
 *
 * DESCRIPTION: 定义AVariant类型的方法实现
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

// #include "stdafx.h"
#include "AVariant.h"
#include "AProperty.h"
#include "assert.h"

AVariant::AVariant()
{
	m_iType = AVT_INVALIDTYPE;		//初始化为非法类型
	memset(&m_avData, 0, sizeof(AVARIANT));
}

AVariant::AVariant(const AVariant & v)
{
	memcpy(&m_avData, &v.m_avData, sizeof(AVARIANT));
	m_iType = v.m_iType;
}

AVariant::AVariant(bool bV)
{
	m_iType = AVT_BOOL;
	m_avData.bV = bV;
}

AVariant::AVariant(char cV)
{
	m_iType = AVT_CHAR;
	m_avData.cV = cV;
}

AVariant::AVariant(unsigned char ucV)
{
	m_iType = AVT_UCHAR;
	m_avData.ucV = ucV;
}

AVariant::AVariant(short sV)
{
	m_iType = AVT_SHORT;
	m_avData.sV = sV;
}

AVariant::AVariant(unsigned short usV)
{
	m_iType = AVT_USHORT;
	m_avData.usV = usV;
}

AVariant::AVariant(int iV)
{
	m_iType = AVT_INT;
	m_avData.iV = iV;
}

AVariant::AVariant(unsigned int uiV)
{
	m_iType = AVT_UINT;
	m_avData.uiV = uiV;
}

AVariant::AVariant(long lV)
{
	m_iType = AVT_LONG;
	m_avData.lV = lV;
}

AVariant::AVariant(unsigned long ulV)
{
	m_iType = AVT_ULONG;
	m_avData.ulV = ulV;
}

AVariant::AVariant(__int64 i64V)
{
	m_iType = AVT_INT64;
	m_avData.i64V = i64V;
}

AVariant::AVariant(unsigned __int64 ui64V)
{
	m_iType = AVT_UINT64;
	m_avData.ui64V = ui64V;
}

AVariant::AVariant(float fV)
{
	m_iType = AVT_FLOAT;
	m_avData.fV = fV;
}

AVariant::AVariant(double dV)
{
	m_iType = AVT_DOUBLE;
	m_avData.dV = dV;
}

AVariant::AVariant(AObject & pObject)
{
	m_iType = AVT_POBJECT;
	m_avData.paV = &pObject;
}

AVariant::AVariant(A3DVECTOR3 avec3)
{
	m_iType = AVT_A3DVECTOR3;
	memcpy(m_avData.a3dvV, &avec3, sizeof(m_avData.a3dvV));
	/*
		因为m_avData.a3dvV的定义为“char [sizeof(A3DVECTOR3)]”，故用“memcpy”而非“=”。
	*/
}

AVariant::AVariant(A3DMATRIX4 amat4)
{
	m_iType = AVT_A3DMATRIX4;
	memcpy(m_avData.a3dmV, &amat4, sizeof(m_avData.a3dmV));
	/*
		因为m_avData.a3dmV的定义为“char [sizeof(A3DMATRIX4)]”，故用“memcpy”而非“=”。
	*/
}

AVariant::AVariant(AUString & str)
{
	m_iType = AVT_STRING;
	m_avData.pstrV = &str;
}

AVariant::AVariant(const ABinary & binV)
{
	m_iType = AVT_PABINARY;
	m_avData.pbinV = &binV;
}

AVariant::AVariant(AClassInfo * paciV)
{
	m_iType = AVT_PACLASSINFO;
	m_avData.paciV = paciV;
}

AVariant::AVariant(ASet * pSet)
{
	m_iType = AVT_PASET;
	m_avData.pasetV = pSet;
}