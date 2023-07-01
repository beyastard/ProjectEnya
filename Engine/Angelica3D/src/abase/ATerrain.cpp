#include "ATerrain.h"

ATerrain::ATerrain()
{
	m_pA3DDevice		= NULL;
	m_pA3DTerrain		= NULL;
}

ATerrain::~ATerrain()
{

}

bool ATerrain::Create(const PAWORLD_INIT_TERRAIN __pDatum)
{
	if(!BindFinished())
	{
		ADebug::Msg(0,"%s","not bind device!");
		return false;
	}
	Release();

	m_pA3DTerrain = new A3DTerrain();
	if( NULL == m_pA3DTerrain )
	{
		ADebug::Msg(0,"%s","Not enough memory to create Terrain !");
		return false;
	}
	if( !m_pA3DTerrain->Init(
		m_pA3DDevice,
		&__pDatum->terrainParam,
		__pDatum->szTexBase,
		__pDatum->szHeightMap,
		__pDatum->szTexMap) )
	{
		delete m_pA3DTerrain;
		m_pA3DTerrain = NULL;
		ADebug::Msg(0,"%s","Init A3DTerrain Fail!");
		return false;
	}
	return true;
}
bool ATerrain::Create(ATerrainInit* pInit)
{
	AWORLD_INIT_TERRAIN dat;
	pInit->ToKernelDatum(&dat);
	return Create(&dat);
}

bool ATerrain::Release()
{
	if( m_pA3DTerrain )
	{
		m_pA3DTerrain->Release();
		delete m_pA3DTerrain;
		m_pA3DTerrain = NULL;
	}
	return true;
}

