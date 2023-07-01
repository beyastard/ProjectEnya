#include "AMatter.h"
/*
// class AMatter

IMPLEMENT_ASERIAL(AMatter, AModelUnit, 0x0100)

AMatter::AMatter()
{
}

AMatter::~AMatter()
{
}

bool AMatter::OnShot(AModelUnit *pShotInfo)
{
	if(!AModelUnit::OnShot(pShotInfo))
	{
		return false;
	}
	return true;
}

bool AMatter::Release()
{
	return AModelUnit::Release();
}

bool AMatter::Render(A3DViewport * pCurrentViewport)
{
	if(!AModelUnit::Render(pCurrentViewport))
	{
		return false;
	}
	return true;
}

int AMatter::OnMsg(PAMSG pMsg)
{
	
	return AModelUnit::OnMsg(pMsg);
}

AModelUnitData* AMatter::CreateData()
{
	return new AMatterData;
}

bool AMatter::OnWriteData(AModelUnitData* pData)
{
	VERIFY_OBJECT_TYPE(pData, AMatterData);

	if(!AModelUnit::OnWriteData(pData))
	{
		return false;
	}
	((AMatterData*)pData)->m_matter_dat	= m_matter_dat;
	return true;
}

bool AMatter::OnReadData(AModelUnitData* pData)
{
	VERIFY_OBJECT_TYPE(pData, AMatterData);

	if(!AModelUnit::OnReadData(pData))
	{
		return false;
	}
	m_matter_dat	= ((AMatterData*)pData)->m_matter_dat;
	return true;
}

// class AMatterCanBeShoted

IMPLEMENT_ASERIAL(AMatterCanBeShoted, AMatter, 0x0100)

AMatterCanBeShoted::AMatterCanBeShoted()
{
	m_pShotGen	= NULL;
	m_uCountGen	= 0;
}

AMatterCanBeShoted::~AMatterCanBeShoted()
{
}

bool AMatterCanBeShoted::Release()
{
	if(m_pShotGen != NULL)
	{
		delete[] m_pShotGen;
		m_uCountGen	= 0;
	}
	return AMatter::Release();
}

bool AMatterCanBeShoted::OnShot(AModelUnit *pShotInfo)
{
	if(!AMatter::OnShot(pShotInfo))
	{
		return false;
	}
	// 产生物品（具有一定延迟）

	// 播放动画（在此期间，OnShot应该被禁用）

	// 发送终止自身“生命”的消息给AMatterMan.
	// 有可能不终止，在屏幕上保留一些效果。
	return true;
}

// class AMatterCanBePicked

IMPLEMENT_ASERIAL(AMatterCanBePicked, AMatter, 0x0100)

AMatterCanBePicked::AMatterCanBePicked()
{
}

AMatterCanBePicked::~AMatterCanBePicked()
{
}

bool AMatterCanBePicked::Release()
{
	return AMatter::Release();
}

bool AMatterCanBePicked::OnShot(AModelUnit *pShotInfo)
{
	if(!AMatter::OnShot(pShotInfo))
	{
		return false;
	}

	// 播放声音

	// 发送拾获物品的消息

	// 发送终止消息 

	return true;
}*/