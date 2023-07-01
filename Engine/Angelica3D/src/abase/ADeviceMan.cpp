// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ADeviceMan.cpp
// Creator: Wei Hua (魏华)
// 设备管理类

#include "AUList.h"
#include "ADeviceMan.h"
#include "ADevice.h"

////////////////////////////////////////////////////////////////////
// ADeviceMan
////////////////////////////////////////////////////////////////////
ADeviceMan::ADeviceMan()
{
	m_pDeviceList	= new AListT<ADevice *>;
}
ADeviceMan::~ADeviceMan()
{
	if( m_pDeviceList )
	{
		delete	m_pDeviceList;
		m_pDeviceList	= NULL;
	}
}
bool	ADeviceMan::AddDevice(ADevice *pADevice)
{
	m_pDeviceList->AddTail(pADevice);
	return  true;
}
bool	ADeviceMan::RemoveDevice(ADevice *pADevice)
{
	AListPosition	pos = m_pDeviceList->Find(pADevice);
	if(pos != NULL)
	m_pDeviceList->RemoveAt(pos);
	return true;
}
bool	ADeviceMan::RemoveAllDevice()
{
	// 清空列表中的索引
	m_pDeviceList->RemoveAll();

	return	true;
}
bool	ADeviceMan::DealIn()
{
	bool	bval;
	ADevice	*pADevice;

	AListPosition pos = m_pDeviceList->GetHeadPosition();
	while(pos)
	{
		pADevice = m_pDeviceList->GetNext(pos);
		bval	= pADevice->UpdateIn();
		bval	= pADevice->TranslateIn();
	}

	return	true;
}
bool	ADeviceMan::DealOut()
{
	bool	bval;
	ADevice	*pADevice;

	AListPosition pos = m_pDeviceList->GetHeadPosition();
	while(pos)
	{
		pADevice = m_pDeviceList->GetNext(pos);
		bval	= pADevice->TranslateOut();
		bval	= pADevice->UpdateOut();
	}

	return	true;
}

////////////////////////////////////////////////////////////////////
// AWinMsgDeviceGrp
////////////////////////////////////////////////////////////////////
AWinMsgDeviceGrp::AWinMsgDeviceGrp()
{
	m_pDeviceList	= new AListT<AWinMsgDevice *>;
}
AWinMsgDeviceGrp::~AWinMsgDeviceGrp()
{
	if( m_pDeviceList )
	{
		delete	m_pDeviceList;
		m_pDeviceList	= NULL;
	}
}
bool	AWinMsgDeviceGrp::AddDevice(AWinMsgDevice *pAWMDevice)
{
	m_pDeviceList->AddTail(pAWMDevice);
	return true;
}
bool	AWinMsgDeviceGrp::RemoveDevice(AWinMsgDevice *pAWMDevice)
{
	AListPosition	pos = m_pDeviceList->Find(pAWMDevice);
	if(pos != NULL)
		m_pDeviceList->RemoveAt(pos);
	return	true;
}
bool	AWinMsgDeviceGrp::RemoveAllDevice()
{
	m_pDeviceList->RemoveAll();
	return true;
}
bool	AWinMsgDeviceGrp::FilterWinMsg(void *pWinMsg)
{
	bool			bval;
	AWinMsgDevice	*pAWMDevice;

	AListPosition pos = m_pDeviceList->GetHeadPosition();
	while(pos)
	{
		pAWMDevice = m_pDeviceList->GetNext(pos);
		bval	= pAWMDevice->FilterWinMsg(pWinMsg);
	}

	return	true;
}
