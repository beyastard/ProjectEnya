/*
 * FILE: AVariant.h
 *
 * DESCRIPTION: 实现万用数据类AVariant
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY: By JiangLi。使用模板构造函数以及模板类型转换操作符应该是不可行的。
 *               因此重新定义了相关部分。
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */


#ifndef _AVARIANT_H__
#define _AVARIANT_H__

#include "windows.h"
#include "a3dtypes.h"
#include "aobject.h"
#include "abinary.h"
#include "austring.h"
#include <assert.h>

#define DEFINEOPERATOR(type) 	operator type() {assert(m_iType != AVT_INVALIDTYPE);return *(type *)(&m_avData);}

class ASet;

class AVariant  
{
public:

	enum AVARTYPE
	{
		AVT_INVALIDTYPE,	// 非法类型，用于默认构造,此时任何数据成员都不可用
		AVT_BOOL,			// 布尔量				m_avData.bV;
		AVT_CHAR,			// 字符					m_avData.cV;	
		AVT_UCHAR,			// 无符号字符			m_avData.ucV;	
		AVT_SHORT,			// 短整型				m_avData.sV;	
		AVT_USHORT,			// 无符号短整型			m_avData.usV;	
		AVT_INT,			// 整型					m_avData.iV;	
		AVT_UINT,			// 无符号整型			m_avData.uiV;	
		AVT_LONG,			// 长整型				m_avData.lV;	
		AVT_ULONG,			// 无符号长整型			m_avData.ulV;	
		AVT_INT64,			// 64Bit整型			m_avData.i64V;	
		AVT_UINT64,			// 无符号64Bit整型		m_avData.ui64V;
		AVT_FLOAT,			// 浮点数				m_avData.fV;	
		AVT_DOUBLE,			// 双精度浮点数			m_avData.dV;	
		AVT_POBJECT,		// 对象指针				m_avData.paV;
		AVT_A3DVECTOR3,		// 向量					m_avData.a3dvV;
		AVT_A3DMATRIX4,		// 矩阵					m_avData.pa3dmV;
		AVT_STRING,			// 字符串				m_avData.pstrV;
		AVT_PABINARY,		// 二进制块				m_avData.binV;
		AVT_PACLASSINFO,	// 类串行化信息指针		m_avData.paciV;
		AVT_PASET,			// 集合指针				m_avData.pasetV;
	};


public:

	AVariant();
	AVariant(const AVariant & v);
	AVariant(bool bV);
	AVariant(char cV);
	AVariant(unsigned char ucV);
	AVariant(short sV);
	AVariant(unsigned short usV);
	AVariant(int iV);
	AVariant(unsigned int uiV);
	AVariant(long lV);
	AVariant(unsigned long ulV);
	AVariant(__int64 i64V);
	AVariant(unsigned __int64 ui64V);
	AVariant(float fV);
	AVariant(double dV);
	AVariant(AObject & pObject);
	AVariant(A3DVECTOR3 avec3);
	AVariant(A3DMATRIX4 amat4);
	AVariant(AUString & str);
	AVariant(const ABinary & binV);
	AVariant(AClassInfo * paciV);
	AVariant(ASet * pSet);

	int GetType() const {return m_iType;}

	DEFINEOPERATOR(bool)
	DEFINEOPERATOR(char)
	DEFINEOPERATOR(unsigned char)
	DEFINEOPERATOR(short)
	DEFINEOPERATOR(unsigned short)
	DEFINEOPERATOR(int)
	DEFINEOPERATOR(unsigned int)
	DEFINEOPERATOR(long)
	DEFINEOPERATOR(unsigned long)
	DEFINEOPERATOR(__int64)
	DEFINEOPERATOR(unsigned __int64)

	// 对float的转换做特殊设计
	operator float()
	{
		assert(m_iType != AVT_INVALIDTYPE);
		if(m_iType == AVT_DOUBLE)
			return (float)m_avData.dV;
		if(m_iType == AVT_FLOAT)
			return m_avData.fV;
		return (float)(*(__int64 *)(&m_avData));
	}

	// 对double的转换做特殊设计
	operator double()
	{
		assert(m_iType != AVT_INVALIDTYPE);
		if(m_iType == AVT_DOUBLE)
			return m_avData.dV;
		if(m_iType == AVT_FLOAT)
			return (double)m_avData.fV;
		return (double)(*(__int64 *)(&m_avData));
	}

	DEFINEOPERATOR(AObject *)
	DEFINEOPERATOR(A3DVECTOR3)
	DEFINEOPERATOR(A3DMATRIX4)
	operator AUString() {assert(m_iType == AVT_STRING);return *(m_avData.pstrV);}
	operator ABinary() {assert(m_iType != AVT_INVALIDTYPE);return *(m_avData.pbinV);}
	DEFINEOPERATOR(AClassInfo *)
	DEFINEOPERATOR(ASet *)

private:

	union AVARIANT
	{
		bool					bV;			// 布尔量
		char					cV;			// 字符
		unsigned char			ucV;		// 无符号字符
		short					sV;			// 短整型
		unsigned short			usV;		// 无符号短整型
		int						iV;			// 整型
		unsigned int			uiV;		// 无符号整型
		long					lV;			// 长整型
		unsigned long			ulV;		// 无符号长整型
		__int64					i64V;		// 64Bit整型
		unsigned __int64		ui64V;		// 无符号64Bit整型
		float					fV;			// 浮点数
		double					dV;			// 双精度浮点数
		AObject *				paV;		// 对象指针
		char					a3dvV[sizeof(A3DVECTOR3)];	// 向量数据区
		char					a3dmV[sizeof(A3DMATRIX4)];	// 矩阵数据区
		AUString *				pstrV;		// 字符串 
		const ABinary *			pbinV;		// 二进制块对象指针
		AClassInfo *			paciV;		// 串行化信息指针
		ASet *					pasetV;		// 集合指针
	} m_avData;

	int m_iType;	// 数据类型
};

#endif
