#include <assert.h>
#include "ACamera.h"
#include "AGame.h"

ACamera::ACamera()
{
	m_pAGame		= NULL;
	m_fSpeed		= 5.f;
	m_fTurnSpeed	= 8;
	m_fDamp			= 1.f / 8;
	m_turn			= 0.f;
	m_look			= 0.f;
	memset(&m_velocity, 0, sizeof(m_velocity));
	memset(&m_pos, 0, sizeof(m_pos));
	memset(&m_dir, 0, sizeof(m_dir));
}

ACamera::~ACamera(void)
{
}

bool ACamera::Init(AGame * __pAGame)
{
	m_pAGame		= __pAGame ;
	assert(m_pAGame);
	return true;
}

// __dir(0~360)
bool ACamera::Move(int __dir)
{
	A3DVECTOR3	vecDir, vecDir2, dV;
	float		a, sa, ca;
	// 沿着当前的朝向
	vecDir	= Normalize(GetA3DCamera()->GetDirH());
	vecDir2.x	= -vecDir.z;
	vecDir2.y	= 0;
	vecDir2.z	= vecDir.x;
	a			= __dir * A3D_PI / 180;
	sa			= float(-sin(a));
	ca			= float(cos(a));
	dV			= vecDir * ca + vecDir2 * sa;
	dV			= dV * m_fSpeed;
	m_velocity	= m_velocity + dV;
	return	true;
}
// __dir(正负)
bool ACamera::Lift(int __dir)
{
	m_velocity.y	+= float(__dir) * m_fSpeed;
	return	true;
}
// 停止
bool ACamera::Stop()
{
	memset(&m_velocity, 0, sizeof(m_velocity));
	return	true;
}
// 转向
bool ACamera::Turn(float h)
{
	m_turn = h*m_fTurnSpeed;
	//m_pA3DCamera->DegDelta(h*m_fTurnSpeed);
	return	true;
}
bool ACamera::Look(float v)
{
	m_look = v*m_fTurnSpeed;
	//m_pA3DCamera->PitchDelta(v*m_fTurnSpeed);
	return	true;
}

bool ACamera::TurnAroundAxis(float vDeltaRad)
{
	A3DVECTOR3 vecPos = GetA3DCamera()->GetPos();
	A3DVECTOR3 vecDir = GetA3DCamera()->GetDir();

	return GetA3DCamera()->TurnAroundAxis(vecPos,vecDir,vDeltaRad);
}

bool ACamera::LogicRun(float __TimeSpan)
{
	//change to second
	__TimeSpan = __TimeSpan/1000.f;
	A3DVECTOR3	vecPos, vecDir;
	// 物理规律
	vecPos	= GetA3DCamera()->GetPos();
	vecPos	= vecPos + m_velocity * __TimeSpan;

	GetA3DCamera()->DegDelta(m_turn);
	GetA3DCamera()->PitchDelta(m_look);
	GetA3DCamera()->SetPos(vecPos);
	
//	A3DVECTOR3 d = GetA3DCamera()->GetDir();
//	A3DVECTOR3 u = GetA3DCamera()->GetUp();
//	ADebug::Msg(0,"%f\n",DotProduct(d,u));	

	// 速度衰减
	m_velocity	= m_velocity * m_fDamp;

	m_look = 0.f;
	m_turn = 0.f;
	
	return true;
}

A3DCamera * ACamera::GetA3DCamera(void)
{
	return m_pAGame->GetA3DCamera();
}