#include "AWndSprite.h"
#include "A2DSpriteItem.h"
#include "A2DSprite.h"


IMPLEMENT_ASERIAL(ASharedSprite, ASharedObject, 0x0100)

AListT<ASharedSprite*>	ASharedSprite::m_SpriteList;

ASharedSprite::ASharedSprite():
m_pA2DSprite	(NULL)
{
}

ASharedSprite::~ASharedSprite()
{
}

ASharedSprite::ASharedList& ASharedSprite::GetSharedList()
{
	return *((ASharedList*)&m_SpriteList);
}

bool ASharedSprite::Release()
{
	ABASE_SAFE_RELEASE(m_pA2DSprite);

	return ASharedObject::Release();
}


IMPLEMENT_ASERIAL(AWndSprite, AObject, 0x0100)

AWndSprite::AWndSprite()
{
}

AWndSprite::~AWndSprite()
{
	Release();
}

bool AWndSprite::Release()
{

	return true;
}

bool AWndSprite::Init(const char* pszName, int nWidth, int nHeight, UINT colorKey)
{
	return true;
}


IMPLEMENT_ASERIAL(AWndSpriteItem, AObject, 0x0100)

AWndSpriteItem::AWndSpriteItem():
m_nIndex	(0)
{
}

AWndSpriteItem::~AWndSpriteItem()
{
}

bool AWndSpriteItem::Release()
{

	return true;
}

bool AWndSpriteItem::Init(AWndSprite* pParent, const ARECT& rItemRect)
{
	return true;
}


AListT<AWndSpriteMan::ASpriteNode*>	AWndSpriteMan::m_lstSpriteNodes;

A2DSprite* AWndSpriteMan::CreateSprite(const char* pszName, int nWidth, int nHeight, UINT colorKey, int nItemCount, ARECT * pRects)
{
	AListPosition	Pos = m_lstSpriteNodes.GetHeadPosition();
	while(Pos != NULL)
	{
		ASpriteNode* pNode	= m_lstSpriteNodes.GetNext(Pos);
		if(0 == strcmp(pNode->m_szName, pszName))
		{
			pNode->m_nRefCount ++;
			return pNode->m_pSprite;
		}
	}
	A2DSprite* pSprite	= new A2DSprite;
	float fScaleX	= AWC::GetWC()->GetScaleX();
	float fScaleY	= AWC::GetWC()->GetScaleY();

	int i;
	for(i = 0; i < nItemCount; i ++)
	{
		pRects[i].left		= (int)(pRects[i].left*fScaleX);
		pRects[i].top		= (int)(pRects[i].top*fScaleY);
		pRects[i].right		= (int)(pRects[i].right*fScaleX);
		pRects[i].bottom	= (int)(pRects[i].bottom*fScaleY);
	}
	if(!pSprite->Init(AWC::GetWC()->GetDevice(),
		const_cast<char*>(pszName), (int)(nWidth*fScaleX), (int)(nHeight*fScaleY),
		colorKey, nItemCount, (A3DRECT*)pRects, true, false))
	{
		pSprite->Release();
		delete pSprite;
		ADEBUG_TRACE1("Cannot load sprite <%s>.", pszName);
		return NULL;
	}
	for(i = 0; i < nItemCount; i ++)
	{
		pRects[i].left		= (int)(pRects[i].left/fScaleX);
		pRects[i].top		= (int)(pRects[i].top/fScaleY);
		pRects[i].right		= (int)(pRects[i].right/fScaleX);
		pRects[i].bottom	= (int)(pRects[i].bottom/fScaleY);
	}

	ASpriteNode* pNode	= new ASpriteNode;
	pNode->m_nRefCount	= 1;
	pNode->m_pSprite	= pSprite;
	strcpy(pNode->m_szName, pszName);
	m_lstSpriteNodes.AddTail(pNode);
	return pSprite;
}

VOID AWndSpriteMan::ReleaseSprite(A2DSprite* pSprite)
{
	assert(pSprite != NULL);

	AListPosition	Pos = m_lstSpriteNodes.GetHeadPosition();
	while(Pos != NULL)
	{
		AListPosition PosPre = Pos;
		ASpriteNode* pNode	= m_lstSpriteNodes.GetNext(Pos);
		if(pNode->m_pSprite == pSprite)
		{
			if(pNode->m_nRefCount <= 1)
			{
				m_lstSpriteNodes.RemoveAt(PosPre);
				delete pNode;
				pSprite->Release();
				delete pSprite;
			}
			else
			{
				pNode->m_nRefCount --;
			}
		}
	}
}