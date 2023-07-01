#include "AStat.h"
//#include <mtime.h>

AStat g_stat;

AStat::AStat()	: AObjectIDMan()
{
	Init(NULL);
	m_num		= -1;
}

AStat::~AStat()
{
	m_itemStack.RemoveAll();
	Release();
}

void AStat::BeginStatItem(TU_S * pItem)
{
	if(pItem->bReset)
	{
		pItem->totalTime	= 0;
		pItem->maxTime		= 0;
		pItem->minTime		= 0;
	}
	pItem->preTime	= timeGetTime();
	//Ñ¹Õ»
	m_itemStack.AddTail(pItem);
}

void AStat::EndStatItem()
{
	//³öÕ»
	AListPosition pos = m_itemStack.GetTailPosition();
	TU_S * pItem = m_itemStack.GetAt(pos);
	m_itemStack.RemoveAt(pos);

	if(pItem==0)
	{
		ADebug::Msg(0,"EndStatItem error");
		assert(0);
		return ;
	}
	unsigned long nTime = timeGetTime();
	unsigned long nUseTime = nTime - pItem->preTime ;

	pItem->totalTime += nUseTime;

	if(nUseTime > pItem->maxTime )
		pItem->maxTime = nUseTime;

	if(nUseTime < pItem->minTime )
		pItem->minTime = nUseTime;

	pItem->useTime	= nUseTime;
}
	
int AStat::OnGetID(AIDObject * __pAIDObject)
{
	TU_S * pItem = (TU_S*)__pAIDObject;
	return pItem->id ;
}

bool AStat::OnRelease(AIDObject *__pAIDObject)
{
	TU_S * pItem = (TU_S*)__pAIDObject;
	delete pItem;

	return true;
}
