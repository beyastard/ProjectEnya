/*
 * FILE: AMatter.h
 *
 * DESCRIPTION: AMatter
 *
 * CREATED BY: JiangLi, 2002/08/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.	
 */

#ifndef	__AMATTER_H__
#define	__AMATTER_H__
/*
#include "AModelUnit.h"
#include "AMatterData.h"

// AObject
	// AIDObject
		// AUnit
			// AVisualUnit
				// AModelUnit
					class AMatter;
						class AMatterCanBeShoted;
						class AMatterCanBePicked;

class AMatter : public AModelUnit
{
public:
	DECLARE_ASERIAL(AMatter)
public:
	AMatter();
	virtual ~AMatter();

protected:
	AMATTER_KERNEL_DATUM	m_matter_dat;
protected:

	virtual AModelUnitData* CreateData();
	virtual bool OnWriteData(AModelUnitData* pData);
	virtual bool OnReadData(AModelUnitData* pData);
	
public:
	virtual bool OnShot(AModelUnit *pShotInfo);
	virtual bool Release();
	virtual bool Render(A3DViewport * pCurrentViewport);
	virtual int  OnMsg(PAMSG pMsg);
};

// 能够被击中的物品，被打碎以后播放动画，产生其它
// 的物品。产生物品具有一定的几率，能够从多个候选
// 物品中随机选取。被打中以后应该会有一定的分数，
// 并且有可能需要发送信息给射击者。
class AMatterCanBeShoted : public AMatter
{
public:
	DECLARE_ASERIAL(AMatterCanBeShoted)

protected:

	// 被打中以后产生的物品
	class AShotGenerate
	{
	public:
		//AMODELUNIT_TYPE	m_Type;		// 物品类型
		int				m_nProb;	// 产生几率
		int				m_nDelay;	// 产生物品的延迟
		///INITMODELUNIT	m_Init;		// 初始参数
		union	// 不同类型物品的初始参数。
		{
			int		m_nReserve;
		};
	};

	AShotGenerate*	m_pShotGen;
	UINT			m_uCountGen;

public:
	AMatterCanBeShoted();
	virtual ~AMatterCanBeShoted();

	virtual bool Release();
	virtual bool OnShot(AModelUnit *pShotInfo);
};

// 能够被拾取的物品，拾取以后可能需要播放声音。
// 被拾取以后，把物品类别发送给ShotInfo的关联对象。
class AMatterCanBePicked : public AMatter
{
public:
	DECLARE_ASERIAL(AMatterCanBePicked)
public:
	AMatterCanBePicked();
	virtual ~AMatterCanBePicked();

	virtual bool Release();
	virtual bool OnShot(AModelUnit *pShotInfo);
};
*/
#endif	// __AMATTER_H__