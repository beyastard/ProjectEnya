/*
 * FILE: AStream.h
 *
 * DESCRIPTION: 定义串行化文件数据接口AStream
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef _ASTREAM_H__
#define _ASTREAM_H__

class AStream  
{
public:
	virtual bool Write(const void * buff, int size) = 0;	// 写数据
	virtual bool Read(void * buff, int size) = 0;			// 读数据
};

#endif
