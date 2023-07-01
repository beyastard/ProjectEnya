#include "AUList.h"
#include <assert.h>
/*
AUList::AUList()
{
	m_pHead = m_pTail = NULL;
	m_nSize = 0;
}

AUList::~AUList()
{
	
}

bool AUList::Init()
{
	m_pHead = (AULISTELEMENT *) malloc(sizeof(AULISTELEMENT));
	if( NULL == m_pHead )
		return false;

	m_pTail = (AULISTELEMENT *) malloc(sizeof(AULISTELEMENT));
	if( NULL == m_pTail )
		return false;

	m_pHead->pData = m_pTail->pData = NULL;
	m_pHead->pLast = m_pTail->pNext = NULL;
	m_pHead->pNext = m_pTail;
	m_pTail->pLast = m_pHead;
	m_nSize = 0;
	return true;
}

bool AUList::Release()
{
	AULISTELEMENT * pThisElement = m_pHead;
	AULISTELEMENT * pElementToDel;

	//We do not carry out the work to release Element.pData, 
	//That is the caller's task :(
	while( pThisElement )
	{
		pElementToDel = pThisElement;
		pThisElement = pThisElement->pNext;

		free(pElementToDel);
	}

	m_pHead = m_pTail = NULL;
	m_nSize = 0;
	return true;
}

bool AUList::Append(LPVOID pDataToAppend, AULISTELEMENT ** ppElement)
{
	return Insert(pDataToAppend, m_pTail, ppElement);
}
	
bool AUList::Insert(LPVOID pDataToInsert, AULISTELEMENT * pElement, AULISTELEMENT ** ppElement)
{
	AULISTELEMENT * pNewElement;
	if( NULL == pElement )
		return false;

	pNewElement = (AULISTELEMENT *) malloc(sizeof(AULISTELEMENT));
	if( NULL == pNewElement )
		return false;

	pNewElement->pData = pDataToInsert;

	pElement->pLast->pNext = pNewElement;
	pNewElement->pLast = pElement->pLast;
	pNewElement->pNext = pElement;
	pElement->pLast = pNewElement;
	
	if( ppElement )
		*ppElement = pNewElement;

	m_nSize ++;
	return true;
}
	
bool AUList::Insert(LPVOID pDataToInsert, LPVOID pDataInsertBefore, AULISTELEMENT ** ppElement)
{
	if( NULL == pDataInsertBefore )
		return false;

	AULISTELEMENT * pElement = FindElementByData(pDataInsertBefore);
	if( NULL == pElement )
		return false;

	return Insert(pDataToInsert, pElement, ppElement);
}
	
bool AUList::Delete(AULISTELEMENT * pElement)
{
	if( NULL == pElement )
		return false;

	pElement->pLast->pNext = pElement->pNext;
	pElement->pNext->pLast = pElement->pLast;

	free(pElement);
	m_nSize --;
	return true;
}
	
bool AUList::Delete(LPVOID pData)
{
	AULISTELEMENT * pElement;
	if( NULL == pData )
		return false;

	pElement = FindElementByData(pData);
	if( NULL == pElement )
		return false;

	return Delete(pElement);
}

AULISTELEMENT * AUList::FindElementByData(LPVOID pData)
{
	AULISTELEMENT * pThisElement = m_pHead->pNext;

	while(pThisElement != m_pTail)
	{
		if( pThisElement->pData == pData )
			return pThisElement;
		else
			pThisElement = pThisElement->pNext;
	}

	//Nothing be found;
	return NULL;
}

bool AUList::IsValid(AULISTELEMENT * pElement)
{
	AULISTELEMENT * pThisElement = m_pHead->pNext;

	while( pThisElement != m_pTail )
	{
		if( pThisElement == pElement )
			return true;
		else
			pThisElement = pThisElement->pNext;
	}

	return false;
}

bool AUList::Reset()
{
	AULISTELEMENT * pThisElement = m_pHead->pNext;
	AULISTELEMENT * pElementToDel;

	//We do not carry out the work to release Element.pData, 
	//That is the caller's task :(
	while( pThisElement != m_pTail )
	{
		pElementToDel = pThisElement;
		pThisElement = pThisElement->pNext;

		free(pElementToDel);
	}

	m_pHead->pData = m_pTail->pData = NULL;
	m_pHead->pLast = m_pTail->pNext = NULL;
	m_pHead->pNext = m_pTail;
	m_pTail->pLast = m_pHead;
	m_nSize = 0;
	return true;
}

AULISTELEMENT * AUList::GetElementByOrder(int nOrder)
{
	if( nOrder >= m_nSize )
		return NULL;

	AULISTELEMENT * pThisElement = m_pHead->pNext;

	int nCount = 0;
	while(pThisElement != m_pTail)
	{
		if( nCount == nOrder )
			return pThisElement;
		else
			pThisElement = pThisElement->pNext;

		nCount ++;
	}

	return NULL;
}

AUListS::AUListS()
{
	Init();
	m_pCurElement	= NULL;
}
AUListS::~AUListS()
{
	Release();
}
bool	AUListS::Append(LPVOID pDataToAppend)
{
	return	AUList::Append(pDataToAppend);
}
void	AUListS::BeginGet()
{
	m_pCurElement	= m_pHead;
}
LPVOID	AUListS::GetNext()
{
	m_pCurElement	= m_pCurElement->pNext;
	if( m_pCurElement == m_pTail )
	{
		return	NULL;
	}
	return	m_pCurElement->pData;
}

// class AListBase

AListBase::ANodeBlock::ANodeBlock(AListBase::ANodeBlock*& pBlocks, AListBase::AListNode*& pFree)
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

AListBase::AListBase()
{
	m_pHead		= NULL;
	m_pTail		= NULL;
	m_pFree		= NULL;
	m_pBlocks	= NULL;
	m_uCount	= 0;
}

AListBase::~AListBase()
{
	ReleaseAll();
}

AListBase::AListNode* AListBase::NewNode()
{
	if(m_pFree == NULL)
	{
		new ANodeBlock(m_pBlocks, m_pFree);
	}
	AListNode* pNode = m_pFree;
	m_pFree	= m_pFree->m_pNext;
	m_uCount ++;
	return pNode;
}

VOID AListBase::FreeNode(AListNode* pNode)
{
	assert(pNode != NULL);

	pNode->m_pNext	= m_pFree;
	m_pFree			= pNode;

	m_uCount --;
}

AListPosition AListBase::AddHead(LPVOID pData)
{
	AListNode* pNode	= NewNode();
	pNode->m_pData		= pData;
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

AListPosition AListBase::AddTail(LPVOID pData)
{
	AListNode* pNode	= NewNode();
	pNode->m_pData		= pData;
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

AListPosition AListBase::InsertAfter(AListPosition Pos, LPVOID pData)
{
	if(Pos == NULL)
	{
		return AddTail(pData);
	}

	AListNode* pNode	= NewNode();
	pNode->m_pData		= pData;
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

AListPosition AListBase::InsertBefore(AListPosition Pos, LPVOID pData)
{
	if(Pos == NULL)
	{
		return AddHead(pData);
	}

	AListNode* pNode	= NewNode();
	pNode->m_pData		= pData;
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

AListPosition AListBase::Find(LPVOID pValue, AListPosition StartPos) const
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
		if(pNode->m_pData == pValue)
		{
			break;
		}
		pNode	= pNode->m_pNext;
	}
	return (AListPosition)pNode;
}

AListPosition AListBase::FindByIndex(INT nIndex) const
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

LPVOID AListBase::RemoveHead()
{
	LPVOID pRet	= NULL;
	if(m_pHead != NULL)
	{
		AListNode* pNode = m_pHead;
		pRet	= pNode->m_pData;
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
	}
	return pRet;
}

LPVOID AListBase::RemoveTail()
{
	LPVOID pRet	= NULL;
	if(m_pTail != NULL)
	{
		AListNode* pNode = m_pTail;
		pRet	= pNode->m_pData;
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
	}
	return pRet;
}

LPVOID AListBase::RemoveAt(AListPosition Pos)
{
	assert(Pos != NULL);

	LPVOID pRet	= ((AListNode*)Pos)->m_pData;
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
	return pRet;
}

VOID AListBase::RemoveAll()
{
	if(m_pHead != NULL)
	{
		m_pTail->m_pNext	= m_pFree;
		m_pFree		= m_pHead;
		m_pHead		= NULL;
		m_pTail		= NULL;
		m_uCount	= 0;
	}
}

VOID AListBase::ReleaseAll()
{
	RemoveAll();
	while(m_pBlocks != NULL)
	{
		ANodeBlock* pDelBlock	= m_pBlocks;
		m_pBlocks	= m_pBlocks->m_pNext;
		delete	pDelBlock;
	}
}
*/