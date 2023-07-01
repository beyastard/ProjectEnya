/*
 * FILE: AIDRec.h
 *
 * DESCRIPTION: ID信息记录;
 *
 * CREATED BY: wangyuanming, 2001/8/20
 *
 * HISTORY: 
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef __AIDREC__H__
#define __AIDREC__H__

#include "ABinary.h"

class AObject;


//大小的单位为指针个数
class AIDRec : public ABinary  
{
	int		m_nCaheSize;		//缓冲大小
public:
	bool Malloc(int __newSize, int __newValue=NULL);
	bool ReMalloc(int __newSize,int __newValue= NULL);
	bool SetDataPtr(int __nID,AObject * __p);
	AObject * GetDataPtrByID(int __nID);
	AIDRec();
	virtual ~AIDRec();
	void * GetDataPtr() { return m_pData;}

	bool SetDataPtr(AObject * __p,int nID);
protected:
	int GetSize();
};

#endif // __AENYAIDREC__H__