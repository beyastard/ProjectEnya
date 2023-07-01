#include "ASky.h"

ASky::ASky()
{
	m_pA3DDevice = NULL;
	m_pA3DBoxSky = NULL;
	m_pA3DGradientSky = NULL;
	m_pA3DSphereSky = NULL;
	m_fTickAnimationTime = 0.0f;
}

ASky::~ASky()
{

}

bool ASky::Create(const PAWORLD_INIT_SKY __pDatum)
{
	if(!BindFinished())
	{
		ADebug::Msg(0,"%s","not bind engine!");
		return false;
	}
	Release();


	switch(__pDatum->skyType)
	{
	case ASkyInit::SKY_BOX:
		{
			m_pA3DBoxSky = new A3DSkyBox;
			if( NULL == m_pA3DBoxSky )
			{
				ADebug::Msg(0,"%s","Not enough memory to create Sky!");
				return false;
			}

			if( !m_pA3DBoxSky->Init(
				m_pA3DDevice,
				NULL,
				__pDatum->szSkySurround0,
				__pDatum->szSkySurround1, 
				__pDatum->szSkySurround2,
				__pDatum->szSkySurround3,
				__pDatum->szSkySurround4,
				__pDatum->szSkySurround5
				) )
			{
				delete m_pA3DBoxSky;
				m_pA3DBoxSky = NULL;
				ADebug::Msg(0,"%s","Init A3DSky Fail!");
				return false;
			}
		}
		break;
	case ASkyInit::SKY_SPHERE:
		{
			m_pA3DSphereSky = new A3DSkySphere;
			if( NULL == m_pA3DSphereSky )
			{
				ADebug::Msg(0,"%s","Not enough memory to create Sky!");
				return false;
			}

			if( !m_pA3DSphereSky->Init(
				m_pA3DDevice,
				NULL,__pDatum->szSkyCap,
				__pDatum->szSkyFront,
				__pDatum->szSkyBack))
			{
				delete m_pA3DSphereSky;
				m_pA3DSphereSky = NULL;
				ADebug::Msg(0,"%s","Init A3DSky Fail!");
				return false;
			}
		}
		break;
	case ASkyInit::SKY_GRADIENT:
		{
			m_pA3DGradientSky = new A3DSkyGradient;
			if( NULL == m_pA3DGradientSky )
			{
				ADebug::Msg(0,"%s","Not enough memory to create Sky!");
				return false;
			}

			if( !m_pA3DGradientSky->Init(
				m_pA3DDevice,
				NULL,
				__pDatum->szSkyCloud,
				__pDatum->szSkyGradient))
			{
				delete m_pA3DGradientSky;
				m_pA3DGradientSky = NULL;
				ADebug::Msg(0,"%s","Init A3DSky Fail!");
				return false;
			}
		}
		break;
	}
	return true;
}
bool ASky::Create(ASkyInit* pInit)
{
	AWORLD_INIT_SKY dat;
	pInit->ToKernelDatum(&dat);
	return Create(&dat); 
}
bool ASky::Release()
{
	if(!BindFinished())
	{
		ADebug::Msg(0,"%s","not bind engine!");
		return false;
	}
	if( m_pA3DSphereSky )
	{
		m_pA3DSphereSky->Release();
		delete m_pA3DSphereSky;
		m_pA3DSphereSky = NULL;
	}
	if( m_pA3DBoxSky )
	{
		m_pA3DBoxSky->Release();
		delete m_pA3DBoxSky;
		m_pA3DBoxSky = NULL;
	}
	if( m_pA3DGradientSky )
	{
		m_pA3DGradientSky->Release();
		delete m_pA3DGradientSky;
		m_pA3DGradientSky = NULL;
	}
	return true;	
}
bool ASky::TickAnimation(float _ms)
{
	const float ENGINETICKTIME = 30.0f;
	m_fTickAnimationTime += _ms;
	while(m_fTickAnimationTime>=ENGINETICKTIME)
	{
		if( (m_pA3DSphereSky && m_pA3DSphereSky->TickAnimation()==false) ||
			(m_pA3DGradientSky && m_pA3DGradientSky->TickAnimation()==false) ||
			(m_pA3DBoxSky && m_pA3DBoxSky->TickAnimation()==false))
		{
			assert(0);
			return false;
		}
		m_fTickAnimationTime-= ENGINETICKTIME;
	}
	return true;
}


