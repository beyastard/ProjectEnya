/*
 * FILE: ADarray.h
 *
 * DESCRIPTION: Dynamic array WITHOUT order.
 *
 * CREATED BY: Cui Ming 2002-1-21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/

#ifndef __ADARRAY_H__
#define __ADARRAY_H__


#include "A3DPlatform.h"
#include "A3DData.h"
#include "vector.h"
class ADArray :public A3DData
{
protected:
	abase::vector<void *> _array;
public:
	bool Append(LPVOID pDataToAppend) { _array.push_back(pDataToAppend);return true;}
	inline 	bool Delete(int idx) { _array.erase_noorder(_array.begin()+idx); return true;}
	inline LPVOID GetElement(int idx) { return _array[idx];}
	inline int GetSize() { return _array.size();}
	inline bool Delete(LPVOID pData);
	int  FindElementByData(LPVOID pData);

};

bool ADArray::Delete(LPVOID pData) 
{
	int tmp;
	if((tmp = FindElementByData(pData)) >= 0)
		return Delete(tmp);
	else
		return false;
}

#endif
