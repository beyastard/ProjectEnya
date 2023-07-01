// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ADeviceMouse.h
// Creator: Wei Hua (魏华)
// 鼠标设备

#ifndef	__ADEVICEMOUSE_H__
#define	__ADEVICEMOUSE_H__

#include "ADevice.h"

#define	ADMOUSE_VK_LBUTTON		1
#define	ADMOUSE_VK_RBUTTON		2
#define	ADMOUSE_VK_MBUTTON		3

class	ADeviceMouse		: public ADevice1vs1, public AWinMsgDevice
{
public:
	// ~ 由于父类中是虚的析构，所以字类中只要实现虚的析构就会被调到
	// 返回指定虚拟键的按下状态
	virtual bool	VK_FirstPress(int nVK)					= 0;
	virtual bool	VK_LastPress(int nVK)					= 0;
	virtual bool	VK_Pressing(int nVK)					= 0;
	virtual void	GetCursorPos(int *pnX, int *pnY)		= 0;
	virtual void	GetCursorDeltaPos(int *pnDX, int *pnDY)	= 0;
	virtual void	SetCursorRange(int nL, int nT, int nR, int nB)	= 0;
	virtual int		GetWheelRoll()							= 0;
};
#include "APlatform.h"
typedef struct	__ADEVICEMOUSE_INFO_T
{
	HWND		hWnd;
	HINSTANCE	hInst;
	bool		bWindow;			// 窗口模式不设置控制级别
	// CursorRange
	int			nLeft;
	int			nTop;
	int			nRight;
	int			nBottom;
}ADEVICEMOUSE_INFO_T;
ADeviceMouse	* NewADeviceMouse(ADEVICEMOUSE_INFO_T *pInfo);

#endif