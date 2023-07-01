// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ADeviceKeyboard.h
// Creator: Wei Hua (魏华)
// 键盘设备

#ifndef	___ADEVICEKEYBOARD_H__
#define	___ADEVICEKEYBOARD_H__

#include "ADevice.h"

#define	MYVK_UNDO				0x1A
#define	MYVK_REDO				0x19

class	ADeviceKeyboard		: public ADevice1vs1, public AWinMsgDevice
{
public:
	// ~ 由于父类中是虚的析构，所以字类中只要实现虚的析构就会被调到
	// 返回指定虚拟键的按下状态
	virtual bool	VK_FirstPress(int nVK)					= 0;
	virtual bool	VK_LastPress(int nVK)					= 0;
	virtual bool	VK_Pressing(int nVK)					= 0;
	// 获得WM_CHAR的可见字串(返回字串长度)
	virtual int		GetChars(char *pszChars, int nSize)		= 0;
	// 获得WM_KEYDOWN的功能键字串(返回字串长度)
	virtual int		GetKeys(char *pszChars, int nSize)		= 0;
};
ADeviceKeyboard	* NewADeviceKeyboard();

#endif