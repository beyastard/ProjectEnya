/*
 * FILE: AList.h
 *
 * DESCRIPTION: A class which manage one dual-link list;
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ALIST_H_
#define _ALIST_H_

#include "A3DPlatform.h"
#include "A3DData.h"

typedef struct _ALISTELEMENT
{
	LPVOID			pData;
	_ALISTELEMENT * pNext;
	_ALISTELEMENT * pLast;
} ALISTELEMENT, * PALISTELEMENT;

class AList	: public A3DData
{
private:
	ALISTELEMENT *	m_pHead;
	ALISTELEMENT *	m_pTail;
	int				m_nSize;

public:
	AList();
	~AList();

	bool Init();
	bool Release();
	bool Reset();
	
	bool Append(LPVOID pDataToAppend, ALISTELEMENT ** ppElement = NULL);
	bool Insert(LPVOID pDataToInsert, ALISTELEMENT * pElement, ALISTELEMENT ** ppElement);
	bool Insert(LPVOID pDataToInsert, LPVOID pDataInsertBefore, ALISTELEMENT ** ppElement);
	bool Delete(ALISTELEMENT * pElement);
	bool Delete(LPVOID pData);

	bool IsValid(ALISTELEMENT * pElement);
	ALISTELEMENT * FindElementByData(LPVOID pData);
	ALISTELEMENT * GetElementByOrder(int nOrder);

	inline int GetSize() { return m_nSize; }
	inline ALISTELEMENT * GetHead() { return m_pHead; }
	inline ALISTELEMENT * GetTail() { return m_pTail; }
	inline ALISTELEMENT * GetFirst() { return m_pHead->pNext; }
};

typedef AList * PAList;

#endif