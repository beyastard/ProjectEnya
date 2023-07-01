// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ADeviceMan.h
// Creator: Wei Hua (魏华)
// 设备管理类

#ifndef	__ADEVICEMAN_H__
#define	__ADEVICEMAN_H__
#include "Aulist.h"

class	ADevice;
class	AWinMsgDevice;

class	ADeviceMan
{
protected:
	AListT<ADevice *>	* m_pDeviceList;						// 管理的设备列表
public:
	ADeviceMan();
	~ADeviceMan();
	bool	AddDevice(ADevice *pADevice);			// 所有设备由AGameFrame创建、删除。也可以直接是成员变量（因为一般不用动态生成）。
	bool	RemoveDevice(ADevice *pADevice);
	bool	RemoveAllDevice();
	bool	DealIn();								// 分成In/Out写的原因是程序总流程是先检查输入、进行逻辑、然后输出
	bool	DealOut();
};

class	AWinMsgDeviceGrp
{
protected:
	AListT<AWinMsgDevice *>	* m_pDeviceList;						// 管理的设备列表
public:
	AWinMsgDeviceGrp();
	~AWinMsgDeviceGrp();
	bool	AddDevice(AWinMsgDevice *pAWMDevice);
	bool	RemoveDevice(AWinMsgDevice *pAWMDevice);
	bool	RemoveAllDevice();
	bool	FilterWinMsg(void *pWinMsg);
};

#endif