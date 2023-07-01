/*
 * FILE: AModelUnitTypes.h
 *
 * DESCRIPTION: 所有派生于AModelUnit的类型都会在这里有一个唯一的类型
 *          标志。每添加一个新的类型也需要在这里的枚举类型中添加一个
 *          新的枚举常量。
 *
 * CREATED BY: JiangLi, 2002-8-15
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AMODELUNITTYPES_H__
#define	__AMODELUNITTYPES_H__

enum AMODELUNIT_TYPE
{
	// AMatter 分类：By JiangLi
	// 通用类的枚举量有可能用不上，写在这里用来表达其层次。
	AMATTER_CANBESHOTED,	// 这是一个通用类，通指可以被打碎的物品。
		AMATTER_CHEST,		// 箱子，可以被打碎。派生于AMATTER_CANBESHOTED
	AMATTER_CANBEPICKED,	// 这是一个通用类，通指可以被拾取的物品。
		AMATTER_WEAPON,		// 武器，包括枪械，手榴弹，子弹，防弹衣等。
		AMATTER_MEDICINE,	// 药品，包括生命恢复，解毒（如果能中毒）等。
};

#endif	// __AMODELUNITTYPES_H__