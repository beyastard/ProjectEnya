/*
 * FILE: AUList.h
 *
 * DESCRIPTION: A class which manage one dual-link list;
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY: by jiangli, Rename AList as AUList.
 *			by Wei Hua, Add AUListS for simplified usage. 08/09/2002
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AULIST_H_
#define _AULIST_H_

#include "APlatform.h"

#pragma warning( disable : 4601 )
#pragma	push_macro("new")
#undef	new
#include <new.h>
#pragma	pop_macro("new")

#pragma warning ( disable : 4786 )

/*typedef struct _AULISTELEMENT
{
	LPVOID			pData;
	_AULISTELEMENT * pNext;
	_AULISTELEMENT * pLast;
} AULISTELEMENT, * PAULISTELEMENT;

class AUList
{
protected:
	AULISTELEMENT *	m_pHead;
	AULISTELEMENT *	m_pTail;
	int				m_nSize;

public:
	AUList();
	~AUList();

	bool Init();
	bool Release();
	bool Reset();
	
	bool Append(LPVOID pDataToAppend, AULISTELEMENT ** ppElement = NULL);
	bool Insert(LPVOID pDataToInsert, AULISTELEMENT * pElement, AULISTELEMENT ** ppElement);
	bool Insert(LPVOID pDataToInsert, LPVOID pDataInsertBefore, AULISTELEMENT ** ppElement);
	bool Delete(AULISTELEMENT * pElement);
	bool Delete(LPVOID pData);

	bool IsValid(AULISTELEMENT * pElement);
	AULISTELEMENT * FindElementByData(LPVOID pData);
	AULISTELEMENT * GetElementByOrder(int nOrder);

	inline int GetSize() { return m_nSize; }
	inline AULISTELEMENT * GetHead() { return m_pHead; }
	inline AULISTELEMENT * GetTail() { return m_pTail; }
	inline AULISTELEMENT * GetFirst() { return m_pHead->pNext; }
};

typedef AUList * PAUList;

// 只要保证加入的指针不为空即可
class	AUListS	: public AUList
{
private:
	AULISTELEMENT	* m_pCurElement;
public:
	AUListS();
	~AUListS();
	bool	Append(LPVOID pDataToAppend);
	void	BeginGet();
	LPVOID	GetNext();
};


typedef	struct tagAListPosition{} *AListPosition;

class AListBase
{
protected:
	
	class AListNode
	{
	public:
		AListNode*	m_pNext;
		AListNode*	m_pPrev;
		LPVOID		m_pData;
	};

#define ALIST_NODEBLOCK_SIZE	10

	class ANodeBlock
	{
	public:
		ANodeBlock*	m_pNext;
		AListNode	m_ListNodes[ALIST_NODEBLOCK_SIZE];
		ANodeBlock(ANodeBlock*& pBlocks, AListNode*& pFree);
	};

protected:

	AListNode*	m_pHead;
	AListNode*	m_pTail;
	AListNode*	m_pFree;
	ANodeBlock*	m_pBlocks;
	UINT		m_uCount;

	AListNode*	NewNode();
	VOID		FreeNode(AListNode* pNode);
	VOID		ReleaseAll();
public:
	AListBase();
	~AListBase();

	AListPosition	GetHeadPosition() const;
	AListPosition	GetTailPosition() const;
	AListPosition	AddHead		(LPVOID pData);
	AListPosition	AddTail		(LPVOID pData);
	AListPosition	InsertAfter	(AListPosition Pos, LPVOID pData);
	AListPosition	InsertBefore(AListPosition Pos, LPVOID pData);
	AListPosition	Find		(LPVOID pValue, AListPosition StartPos = NULL) const;
	AListPosition	FindByIndex	(INT nIndex) const;
	LPVOID			RemoveHead	();
	LPVOID			RemoveTail	();
	LPVOID			RemoveAt	(AListPosition Pos);
	VOID			RemoveAll	();
	UINT			GetCount	() const;
	VOID			SetByIndex	(INT nIndex, LPVOID pData);
	VOID			SetAt		(AListPosition Pos, LPVOID pData);
	LPCVOID			GetByIndex	(INT nIndex) const;
	LPVOID			GetByIndex	(INT nIndex);
	LPCVOID			GetAt		(AListPosition Pos) const;
	LPVOID			GetAt		(AListPosition Pos);
	LPCVOID			GetPrev		(AListPosition& Pos) const;
	LPVOID			GetPrev		(AListPosition& Pos);
	LPCVOID			GetNext		(AListPosition& Pos) const;
	LPVOID			GetNext		(AListPosition& Pos);
	LPCVOID			GetHead		() const;
	LPVOID			GetHead		();
	LPCVOID			GetTail		() const;
	LPVOID			GetTail		();
};

// inline methods of AListBase

__inline UINT AListBase::GetCount() const
{
	return m_uCount;
}

__inline LPCVOID AListBase::GetAt(AListPosition Pos) const
{
	return ((AListNode*)Pos)->m_pData;
}

__inline LPVOID AListBase::GetAt(AListPosition Pos)
{
	return ((AListNode*)Pos)->m_pData;
}

__inline VOID AListBase::SetAt(AListPosition Pos, LPVOID pData)
{
	((AListNode*)Pos)->m_pData	= pData;
}

__inline AListPosition AListBase::GetHeadPosition() const
{
	return (AListPosition)m_pHead;
}

__inline AListPosition AListBase::GetTailPosition() const
{
	return (AListPosition)m_pTail;
}

__inline LPCVOID AListBase::GetHead() const
{
	return (m_pHead == NULL) ? NULL : m_pHead->m_pData;
}

__inline LPVOID AListBase::GetHead()
{
	return (m_pHead == NULL) ? NULL : m_pHead->m_pData;
}

__inline LPCVOID AListBase::GetTail() const
{
	return (m_pTail == NULL) ? NULL : m_pTail->m_pData;
}

__inline LPVOID AListBase::GetTail()
{
	return (m_pTail == NULL) ? NULL : m_pTail->m_pData;
}

__inline LPCVOID AListBase::GetPrev(AListPosition& Pos) const
{
	LPVOID	pRet	= ((AListNode*)Pos)->m_pData;
	Pos		= (AListPosition)((AListNode*)Pos)->m_pPrev;
	return	pRet;
}

__inline LPCVOID AListBase::GetNext(AListPosition& Pos) const
{
	LPVOID	pRet	= ((AListNode*)Pos)->m_pData;
	Pos		= (AListPosition)((AListNode*)Pos)->m_pNext;
	return	pRet;
}

__inline LPVOID AListBase::GetPrev(AListPosition& Pos)
{
	LPVOID	pRet	= ((AListNode*)Pos)->m_pData;
	Pos		= (AListPosition)((AListNode*)Pos)->m_pPrev;
	return	pRet;
}

__inline LPVOID AListBase::GetNext(AListPosition& Pos)
{
	LPVOID	pRet	= ((AListNode*)Pos)->m_pData;
	Pos		= (AListPosition)((AListNode*)Pos)->m_pNext;
	return	pRet;
}

__inline LPCVOID AListBase::GetByIndex(INT nIndex) const
{
	return ((AListNode*)FindByIndex(nIndex))->m_pData;
}

__inline LPVOID AListBase::GetByIndex(INT nIndex)
{
	return ((AListNode*)FindByIndex(nIndex))->m_pData;
}

__inline VOID AListBase::SetByIndex(INT nIndex, LPVOID pData)
{
	AListNode* pNode = (AListNode*)FindByIndex(nIndex);
	if(pNode != NULL)
	{
		pNode->m_pData	= pData;
	}
}
*/

namespace AListUtil
{
#pragma warning( disable : 4601 )
#pragma	push_macro("new")
#undef	new
/*	template<typename TYPE>
	class AWConstructorWrapperT
	{
	public:
		AWConstructorWrapperT()
		{
		}
		AWConstructorWrapperT(const TYPE& t):
		m_tData	(t)
		{
		}

		~AWConstructorWrapperT()
		{
		}
		template<typename T>
		void* operator new (size_t nSize, T* p)
		{
			return p;
		}
		template<typename T>
		void operator delete (void* pMem, T* p)
		{
		}
		TYPE	m_tData;
	};

//#include <new.h>
*/
	template<typename T>
	__inline void Construct(T* p)
	{
		new(p) T;//AWConstructorWrapperT<T>;
	}

	/*template<typename T>
	__inline void Construct(T* p, const T& t)
	{
		new(p) AWConstructorWrapperT<T>(t);
	}*/

	template<typename T>
	__inline void Destruct(T* p)
	{
		p->~T();
	}
#pragma pop_macro("new")

};

typedef struct tagAListPosition { } *AListPosition;

template<typename TYPE>
class AListT
{
protected:

	class AListNode
	{
	public:
		AListNode*	m_pNext;
		AListNode*	m_pPrev;
		TYPE		m_tData;
	};

#define ALIST_NODEBLOCK_SIZE	10

	class ANodeBlock
	{
	public:
		ANodeBlock*	m_pNext;
		AListNode	m_ListNodes[ALIST_NODEBLOCK_SIZE];
		void Create(ANodeBlock*& pBlocks, AListNode*& pFree)
		{
			for(int i = ALIST_NODEBLOCK_SIZE-2; i >= 0; i --)
			{
				m_ListNodes[i].m_pNext	= &m_ListNodes[i+1];
			}
			m_ListNodes[ALIST_NODEBLOCK_SIZE-1].m_pNext	= pFree;
			pFree	= &m_ListNodes[0];
			m_pNext	= pBlocks;
			pBlocks	= this;			
		}
	};

protected:

	AListNode*	m_pHead;
	AListNode*	m_pTail;
	AListNode*	m_pFree;
	ANodeBlock*	m_pBlocks;
	UINT		m_uCount;

	AListNode*	NewNode();
	VOID		FreeNode(AListNode* pNode);
	VOID		ReleaseAll();

	AListT<TYPE> operator = (const AListT<TYPE>& List) { return List; }
	AListT(const AListT<TYPE>& List) {}


public:

	AListT();
	~AListT();

	AListPosition	GetHeadPosition() const;
	AListPosition	GetTailPosition() const;
	AListPosition	AddHead		(const TYPE& tData);
	AListPosition	AddTail		(const TYPE& tData);
	AListPosition	AddTail		(AListT * pList);
	AListPosition	InsertAfter	(AListPosition Pos, const TYPE& tData);
	AListPosition	InsertBefore(AListPosition Pos, const TYPE& tData);
	AListPosition	Find		(const TYPE& tData, AListPosition StartPos = NULL) const;
	AListPosition	FindByIndex	(INT nIndex) const;
	TYPE			RemoveHead	();
	TYPE			RemoveTail	();
	TYPE			RemoveAt	(AListPosition Pos);
	VOID			RemoveAll	();
	UINT			GetCount	() const;
	VOID			SetByIndex	(INT nIndex, const TYPE& tData);
	VOID			SetAt		(AListPosition Pos, const TYPE& tData);
	const TYPE&		GetByIndex	(INT nIndex) const;
	TYPE&			GetByIndex	(INT nIndex);
	const TYPE&		GetAt		(AListPosition Pos) const;
	TYPE&			GetAt		(AListPosition Pos);
	const TYPE&		GetPrev		(AListPosition& Pos) const;
	TYPE&			GetPrev		(AListPosition& Pos);
	const TYPE&		GetNext		(AListPosition& Pos) const;
	TYPE&			GetNext		(AListPosition& Pos);
	const TYPE&		GetHead		() const;
	TYPE&			GetHead		();
	const TYPE&		GetTail		() const;
	TYPE&			GetTail		();
};

template<typename TYPE>
AListT<TYPE>::AListT()
{
	m_pHead		= NULL;
	m_pTail		= NULL;
	m_pFree		= NULL;
	m_pBlocks	= NULL;
	m_uCount	= 0;
}

template<typename TYPE>
AListT<TYPE>::~AListT()
{
	ReleaseAll();
}

//template<typename TYPE> AListT<TYPE>::
template<typename TYPE>
typename AListT<TYPE>::AListNode*	AListT<TYPE>::NewNode()
{
	if(m_pFree == NULL)
	{
		((ANodeBlock*)(new BYTE[sizeof(ANodeBlock)]))->Create(m_pBlocks, m_pFree);
		//new ANodeBlock(m_pBlocks, m_pFree);
	}
	AListNode* pNode = m_pFree;
	m_pFree	= m_pFree->m_pNext;
	m_uCount ++;
	AListUtil::Construct(pNode);
	return pNode;
}

template<typename TYPE>
VOID AListT<TYPE>::FreeNode(AListNode* pNode)
{
	assert(pNode != NULL);

	AListUtil::Destruct(pNode);

	pNode->m_pNext	= m_pFree;
	m_pFree			= pNode;

	m_uCount --;
}

template<typename TYPE>
VOID AListT<TYPE>::ReleaseAll()
{
	RemoveAll();
	while(m_pBlocks != NULL)
	{
		ANodeBlock* pDelBlock	= m_pBlocks;
		m_pBlocks	= m_pBlocks->m_pNext;
		delete	(void*)pDelBlock;
	}
}

template<typename TYPE>
TYPE AListT<TYPE>::RemoveHead()
{
	assert(m_pHead != NULL);

	AListNode* pNode = m_pHead;
	TYPE tRet	= pNode->m_tData;
	m_pHead	= pNode->m_pNext;
	if(m_pHead != NULL)
	{
		m_pHead->m_pPrev	= NULL;
	}
	else
	{
		m_pTail	= NULL;
	}
	FreeNode(pNode);

	return tRet;
}

template<typename TYPE>
TYPE AListT<TYPE>::RemoveTail()
{
	assert(m_pTail != NULL);

	AListNode* pNode = m_pTail;
	TYPE tRet	= pNode->m_tData;
	m_pTail	= pNode->m_pPrev;
	if(m_pTail != NULL)
	{
		m_pTail->m_pNext	= NULL;
	}
	else
	{
		m_pHead	= NULL;
	}
	FreeNode(pNode);
	
	return tRet;
}

template<typename TYPE>
TYPE AListT<TYPE>::RemoveAt(AListPosition Pos)
{
	assert(Pos != NULL);

	TYPE tRet	= ((AListNode*)Pos)->m_tData;
	AListNode*	pNext	= ((AListNode*)Pos)->m_pNext;
	AListNode*	pPrev	= ((AListNode*)Pos)->m_pPrev;
	if(pNext != NULL)
	{
		pNext->m_pPrev	= pPrev;
	}
	else
	{
		m_pTail	= pPrev;
	}
	if(pPrev != NULL)
	{
		pPrev->m_pNext	= pNext;
	}
	else
	{
		m_pHead	= pNext;
	}
	FreeNode((AListNode*)Pos);
	return tRet;
}

template<typename TYPE>
VOID AListT<TYPE>::RemoveAll()
{
	while(m_pHead != NULL)
	{
		AListNode* pNode	= m_pHead;
		m_pHead	= pNode->m_pNext;
		FreeNode(pNode);
	}
	m_pTail		= NULL;
	m_uCount	= 0;
	/*if(m_pHead != NULL)
	{
		m_pTail->m_pNext	= m_pFree;
		m_pFree		= m_pHead;
		m_pHead		= NULL;
		m_pTail		= NULL;
		m_uCount	= 0;
	}*/
}

template<typename TYPE>
AListPosition AListT<TYPE>::GetHeadPosition() const
{
	return (AListPosition)m_pHead;
}

template<typename TYPE>
AListPosition AListT<TYPE>::GetTailPosition() const
{
	return (AListPosition)m_pTail;
}

template<typename TYPE>
AListPosition AListT<TYPE>::AddHead(const TYPE& tData)
{
	AListNode* pNode	= NewNode();
	pNode->m_tData		= tData;
	pNode->m_pPrev		= NULL;
	pNode->m_pNext		= m_pHead;
	if(m_pHead != NULL)
	{
		m_pHead->m_pPrev	= pNode;
	}
	else
	{
		m_pTail				= pNode;
	}
	m_pHead		= pNode;
	return (AListPosition)pNode;
}

template<typename TYPE>
AListPosition AListT<TYPE>::AddTail(const TYPE& tData)
{
	AListNode* pNode	= NewNode();
	pNode->m_tData		= tData;
	pNode->m_pPrev		= m_pTail;
	pNode->m_pNext		= NULL;
	if(m_pTail != NULL)
	{
		m_pTail->m_pNext	= pNode;
	}
	else
	{
		m_pHead				= pNode;
	}
	m_pTail		= pNode;
	return (AListPosition)pNode;
}

template<typename TYPE>
AListPosition AListT<TYPE>::InsertAfter(AListPosition Pos, const TYPE& tData)
{
	if(Pos == NULL)
	{
		return AddTail(tData);
	}

	AListNode* pNode	= NewNode();
	pNode->m_tData		= tData;
	pNode->m_pPrev		= (AListNode*)Pos;
	pNode->m_pNext		= ((AListNode*)Pos)->m_pNext;
	if(((AListNode*)Pos)->m_pNext != NULL)
	{
		((AListNode*)Pos)->m_pNext->m_pPrev	= pNode;
	}
	else
	{
		m_pTail			= pNode;
	}
	((AListNode*)Pos)->m_pNext	= pNode;

	return (AListPosition)pNode;
}

template<typename TYPE>
AListPosition AListT<TYPE>::InsertBefore(AListPosition Pos, const TYPE& tData)
{
	if(Pos == NULL)
	{
		return AddHead(tData);
	}

	AListNode* pNode	= NewNode();
	pNode->m_tData		= tData;
	pNode->m_pNext		= (AListNode*)Pos;
	pNode->m_pPrev		= ((AListNode*)Pos)->m_pPrev;
	if(((AListNode*)Pos)->m_pPrev != NULL)
	{
		((AListNode*)Pos)->m_pPrev->m_pNext	= pNode;
	}
	else
	{
		m_pHead			= pNode;
	}
	((AListNode*)Pos)->m_pPrev	= pNode;

	return (AListPosition)pNode;
}

template<typename TYPE>
AListPosition AListT<TYPE>::Find(const TYPE& tData, AListPosition StartPos) const
{
	AListNode* pNode;
	if(StartPos != NULL)
	{
		pNode	= (AListNode*) StartPos;
	}
	else
	{
		pNode	= m_pHead;
	}
	while(pNode != NULL)
	{
		if(pNode->m_tData == tData)
		{
			break;
		}
		pNode	= pNode->m_pNext;
	}
	return (AListPosition)pNode;
}

template<typename TYPE>
AListPosition AListT<TYPE>::FindByIndex	(INT nIndex) const
{
	if(nIndex < 0 || nIndex >= (INT)m_uCount)
	{
		return NULL;
	}
	if(nIndex > ((int)m_uCount >> 1))
	{
		AListNode* pNode	= m_pTail;
		for(INT i = (INT)m_uCount-nIndex-1; i > 0; i --)
		{
			pNode	= pNode->m_pPrev;
		}
		return (AListPosition)pNode;
	}
	else
	{
		AListNode* pNode	= m_pHead;
		for(int i = nIndex; i > 0; i --)
		{
			pNode	= pNode->m_pNext;
		}
		return (AListPosition)pNode;
	}
}

template<typename TYPE>
UINT AListT<TYPE>::GetCount() const
{
	return m_uCount;
}

template<typename TYPE>
VOID AListT<TYPE>::SetByIndex(INT nIndex, const TYPE& tData)
{
	AListNode* pNode = (AListNode*)FindByIndex(nIndex);
	if(pNode != NULL)
	{
		pNode->m_tData	= tData;
	}
}

template<typename TYPE>
VOID AListT<TYPE>::SetAt(AListPosition Pos, const TYPE& tData)
{
	assert(Pos != NULL);
	((AListNode*)Pos)->m_tData	= tData;
}

template<typename TYPE>
const TYPE& AListT<TYPE>::GetByIndex(INT nIndex) const
{
	return ((AListNode*)FindByIndex(nIndex))->m_tData;
}

template<typename TYPE>
TYPE& AListT<TYPE>::GetByIndex(INT nIndex)
{
	return ((AListNode*)FindByIndex(nIndex))->m_tData;
}

template<typename TYPE>
const TYPE& AListT<TYPE>::GetAt(AListPosition Pos) const
{
	assert(Pos != NULL);
	return ((AListNode*)Pos)->m_tData;
}

template<typename TYPE>
TYPE& AListT<TYPE>::GetAt(AListPosition Pos)
{
	assert(Pos != NULL);
	return ((AListNode*)Pos)->m_tData;
}

template<typename TYPE>
const TYPE& AListT<TYPE>::GetPrev(AListPosition& Pos) const
{
	assert(Pos != NULL);
	TYPE&	tRet	= ((AListNode*)Pos)->m_tData;
	Pos		= (AListPosition)((AListNode*)Pos)->m_pPrev;
	return	tRet;
}

template<typename TYPE>
TYPE& AListT<TYPE>::GetPrev(AListPosition& Pos)
{
	assert(Pos != NULL);
	TYPE&	tRet	= ((AListNode*)Pos)->m_tData;
	Pos		= (AListPosition)((AListNode*)Pos)->m_pPrev;
	return	tRet;
}

template<typename TYPE>
const TYPE& AListT<TYPE>::GetNext(AListPosition& Pos) const
{
	assert(Pos != NULL);
	TYPE&	tRet	= ((AListNode*)Pos)->m_tData;
	Pos		= (AListPosition)((AListNode*)Pos)->m_pNext;
	return	tRet;
}

template<typename TYPE>
TYPE& AListT<TYPE>::GetNext(AListPosition& Pos)
{
	assert(Pos != NULL);
	TYPE&	tRet	= ((AListNode*)Pos)->m_tData;
	Pos		= (AListPosition)((AListNode*)Pos)->m_pNext;
	return	tRet;
}

template<typename TYPE>
const TYPE& AListT<TYPE>::GetHead() const
{
	assert(m_pHead != NULL);
	return m_pHead->m_tData;
}

template<typename TYPE>
TYPE& AListT<TYPE>::GetHead()
{
	assert(m_pHead != NULL);
	return m_pHead->m_tData;
}

template<typename TYPE>
const TYPE& AListT<TYPE>::GetTail() const
{
	assert(m_pTail != NULL);
	return m_pTail->m_tData;
}

template<typename TYPE>
TYPE& AListT<TYPE>::GetTail()
{
	assert(m_pTail != NULL);
	return m_pTail->m_tData;
}

template<typename TYPE>
AListPosition AListT<TYPE>::AddTail (AListT * pList)
{
	assert(pList);
	AListPosition pRetVal = NULL;
	AListPosition pos = pList->GetHeadPosition();
	while(pos)
	{
		if(!pRetVal)
		{
			pRetVal = AddTail(pList->GetNext(pos));
		}
		else
		{
			AddTail(pList->GetNext(pos));
		}
	}

	pList->RemoveAll();
	return pRetVal;
}

#endif
