/*
 * FILE: ABinary.h
 *
 * DESCRIPTION: 二进制块管理类，块的解释方式由附带的GUID进行说明
 *
 * CREATED BY: 杨智盈, 2002/8/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef _ABINARY_H__
#define _ABINARY_H__

#include "AObject.h"
#include <memory.h>

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;
#endif /* GUID_DEFINED */

// {00000000-0000-0000-0000-0000000000000000} 空标识，用于初始化和判断初始化
static const GUID BT_EMPTY = 
{ 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };


class ABinary : public AObject  
{
public:

	DECLARE_ASERIAL(ABinary)

public:

	virtual bool Load(AArchive &ar);			// 存取函数
	virtual bool Save(AArchive &ar);

	int GetLength();							// 获取数据区长
	void * GetBufferSetLength(int length);		// 初始/重新 置数据区长度
	void SetType(GUID guidType);				// 设置解释类型
	GUID GetType();								// 获取解释类型

	const ABinary & operator = (const ABinary & bin);

	ABinary();
	ABinary(GUID guidType, int iLength, const void * pInitData = NULL);
	ABinary(const ABinary & bin);
	~ABinary();

	// 向任意类型指针的转换
	void * GetBuffer() {return m_pData;}

protected:

	GUID	m_guidType;				// 解释方式
	void *	m_pData;				// 数据区
	int		m_iLength;				// 数据区长度
};

#endif
