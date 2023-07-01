/*
 * FILE: AWndSprite.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-9-9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AWNDSPRITE_H__
#define	__AWNDSPRITE_H__

#include "AWC.h"
#include "AUList.h"
#include "AUString.h"
#include "ASharedObject.h"

class A2DSprite;
class A2DSpriteItem;

class ASharedSprite : public ASharedObject
{
public:
	DECLARE_ASERIAL(ASharedSprite)
protected:
	static AListT<ASharedSprite*>	m_SpriteList;
	virtual ASharedList& GetSharedList();
public:
	A2DSprite*	m_pA2DSprite;
	AUString	m_strName;

public:
	ASharedSprite();
	virtual ~ASharedSprite();

	virtual bool Release();

	static ASharedSprite* CreateSprite();
};

class AWndSprite : public AObject
{
public:
	DECLARE_ASERIAL(AWndSprite)
protected:

public:
	AWndSprite();
	virtual ~AWndSprite();

	virtual bool Release();

	virtual bool Init(const char* pszName, int nWidth, int nHeight, UINT colorKey);
	//bool	Draw(const APOINT& ptPos, const ARECT& rSubRect);
};

class AWndSpriteItem : public AObject
{
public:
	DECLARE_ASERIAL(AWndSpriteItem)
protected:
	INT		m_nIndex;
public:
	AWndSpriteItem();
	virtual ~AWndSpriteItem();

	virtual bool Release();

	bool Init(AWndSprite* pParent, const ARECT& rItemRect);
};

class AWndSpriteMan
{
protected:
	class ASpriteNode
	{
	public:
		char		m_szName[128];
		int			m_nRefCount;
		A2DSprite*	m_pSprite;
	};
	static AListT<ASpriteNode*>	m_lstSpriteNodes;
public:
	static A2DSprite* CreateSprite(const char* pszName, int nWidth, int nHeight, UINT colorKey, int nItemCount, ARECT * pRects);
	static VOID	ReleaseSprite(A2DSprite* pSprite);
};

#endif	// __AWNDSPRITE_H__