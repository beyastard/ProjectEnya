#include "A3DCamera.h"
#include "A3DFuncs.h"
#include "A3DMacros.h"
#include "A3DCollision.h"

A3DCamera::A3DCamera()
{
	m_bMirrored			= false;
	m_pA3DDevice		= NULL;
	m_pAM3DSoundDevice	= NULL;
}

A3DCamera::~A3DCamera()
{
}

// fov is in radian!
bool A3DCamera::Init(A3DDevice * pDevice, FLOAT vFov, FLOAT vFront, FLOAT vBack, FLOAT vRatio)
{
	D3DXMATRIX matProj;

	D3DXMatrixPerspectiveFovLH(&matProj, vFov, vRatio, vFront, vBack);
	m_matProjectTM = *(A3DMATRIX4 *)&matProj;
	m_matPostProjectTM = IdentityMatrix();
	m_matInvProjectTM = InverseTM(m_matProjectTM);

	m_pA3DDevice = pDevice;
	m_vFOVSet = m_vFOV = vFov;
	m_vRatio = vRatio;
	m_vFront = vFront;
	m_vBack = vBack;

	CreateViewFrustum();
	return true;
}

bool A3DCamera::Release()
{
	return true;
}

void A3DCamera::SetDirAndUp(A3DVECTOR3 vecDir, A3DVECTOR3 vecUp)
{
	m_vecDir = Normalize(vecDir);
	m_vecUp = Normalize(vecUp);
	m_vecRight = Normalize(CrossProduct(m_vecUp, m_vecDir));
	m_vecUp = Normalize(CrossProduct(m_vecDir, m_vecRight));
	m_vecLeft = -1.0f * m_vecRight;

	m_vecDirH = m_vecDir;
	m_vecDirH.y = 0.0f;
	m_vecDirH = Normalize(m_vecDirH);
	m_vecLeftH = m_vecLeft;
	m_vecLeftH.y = 0.0f;
	m_vecLeftH = Normalize(m_vecLeftH);

	m_vDeg = (FLOAT)RAD2DEG(atan2(-m_vecDir.z, m_vecDir.x));
	m_vPitch = (FLOAT)RAD2DEG(asin(m_vecDir.y));
	UpdateViewTM();
}

// Set a camera's view matrix using an existing view matrix
bool A3DCamera::SetViewTM(A3DMATRIX4 matView)
{
	A3DMATRIX4		matInv;

	m_matViewTM = matView;

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	m_matInvVPTM = InverseTM(m_matVPTM);

	A3DVECTOR3 vecDir, vecUp, vecPos;
	vecDir.x = matView.m[0][2];
	vecDir.y = matView.m[1][2];
	vecDir.z = matView.m[2][2];
	vecUp.x = matView.m[0][1];
	vecUp.y = matView.m[1][1];
	vecUp.z = matView.m[2][1];

	vecPos = matView.GetRow(3);

	matInv = matView;
	matInv._41 = matInv._42 = matInv._43 = 0.0f;
	matInv = Transpose(matInv);
	vecPos = matInv * (-1.0f * vecPos);
	
	SetDirAndUp(vecDir, vecUp);
	SetPos(vecPos);
	return true;
}

bool A3DCamera::UpdateViewTM()
{
	m_matViewTM = ViewMatrix(m_vecPos, m_vecDir, m_vecUp, 0.0f);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	m_matInvVPTM = InverseTM(m_matVPTM);

	UpdateViewFrustum();
	return true;
}

bool A3DCamera::UpdateProjectTM()
{
	D3DXMATRIX matProj;

	D3DXMatrixPerspectiveFovLH(&matProj, m_vFOV, m_vRatio, m_vFront, m_vBack);
	m_matProjectTM = *(A3DMATRIX4 *)&matProj;
	m_matInvProjectTM = InverseTM(m_matProjectTM);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	m_matInvVPTM = InverseTM(m_matVPTM);

	CreateViewFrustum();
	return true;
}

bool A3DCamera::SetProjectionTM(A3DMATRIX4 matProjection)
{
	m_matProjectTM = matProjection;
	m_matInvProjectTM = InverseTM(m_matProjectTM);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	m_matInvVPTM = InverseTM(m_matVPTM);

	CreateViewFrustum();
	return true;
}

bool A3DCamera::Active()
{
	Update();

	if( m_bMirrored )
		m_pA3DDevice->SetFaceCull(A3DCULL_CW);
	else
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	
	m_pA3DDevice->SetViewMatrix(GetViewTM());
	m_pA3DDevice->SetProjectionMatrix(m_matProjectTM * m_matPostProjectTM);

	return true;
}

bool A3DCamera::MoveFront(FLOAT vDistance)
{
	m_vecPos = m_vecPos + m_vecDirH * vDistance;

	UpdateViewTM();
	return true;
}

bool A3DCamera::MoveBack(FLOAT vDistance)
{
	m_vecPos = m_vecPos - m_vecDirH * vDistance;

	UpdateViewTM();
	return true;
}

bool A3DCamera::MoveLeft(FLOAT vDistance)
{
	m_vecPos = m_vecPos + m_vecLeftH * vDistance;

	UpdateViewTM();
	return true;
}

bool A3DCamera::MoveRight(FLOAT vDistance)
{
	m_vecPos = m_vecPos - m_vecLeftH * vDistance;

	UpdateViewTM();
	return true;
}

bool A3DCamera::DegDelta(FLOAT vDelta)
{
	m_vDeg += vDelta;
	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

bool A3DCamera::PitchDelta(FLOAT vDelta)
{
	m_vPitch += vDelta;
	if( m_vPitch > 89.0f )
		m_vPitch = 89.0f;
	else if( m_vPitch < -89.0f )
		m_vPitch = -89.0f;
	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

bool A3DCamera::Move(A3DVECTOR3 vecDelta)
{
	m_vecPos = m_vecPos + vecDelta;

	UpdateViewTM();
	return true;
}

bool A3DCamera::Filter(A3DFrame * pFrame)
{
	return true;
}

bool A3DCamera::SetMirror(A3DCamera * pCamera, A3DVECTOR3 vecOrg, A3DVECTOR3 vecNormal)
{
	m_matMirror = MirrorMatrix(vecOrg, vecNormal);
	m_pMirrorCamera = pCamera;
	m_bMirrored = true;

	return true;
}

bool A3DCamera::Update()
{
	if( m_bMirrored )
	{
		SetViewTM(m_matMirror * m_pMirrorCamera->GetViewTM());
	}

	if( m_vFOVSet != m_vFOV )
	{
		if( m_vFOV > m_vFOVSet )
		{
			m_vFOV -= 0.01f;//Zoom in;
			if( m_vFOV < m_vFOVSet )
				m_vFOV = m_vFOVSet;
		}
		else
		{
			m_vFOV += 0.01f;//Zoom out;
			if( m_vFOV > m_vFOVSet )
				m_vFOV = m_vFOVSet;
		}

		UpdateProjectTM();
	}
	return true;
}

A3DVECTOR3 A3DCamera::GetPos()
{
	if( m_bMirrored )
		return m_pMirrorCamera->GetPos();
	else
		return m_vecPos;
	return true;
}

bool A3DCamera::UpdateDirAndUp()
{
	A3DVECTOR3 vecYAxis = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_vecDir.x = (FLOAT)(cos(DEG2RAD(m_vDeg)) * cos(DEG2RAD(m_vPitch)));
	m_vecDir.y = (FLOAT)sin(DEG2RAD(m_vPitch));
	m_vecDir.z = (FLOAT)(-sin(DEG2RAD(m_vDeg)) * cos(DEG2RAD(m_vPitch)));

	m_vecRight = Normalize(CrossProduct(vecYAxis, m_vecDir));
	m_vecUp = Normalize(CrossProduct(m_vecDir, m_vecRight));
	m_vecLeft = -1.0f * m_vecRight;

	m_vecDirH = m_vecDir;
	m_vecDirH.y = 0.0f;
	m_vecDirH = Normalize(m_vecDirH);
	m_vecLeftH = m_vecLeft;
	m_vecLeftH.y = 0.0f;
	m_vecLeftH = Normalize(m_vecLeftH);
	return true;
}

/*
	Transform one 3D world space vector into projection plane coordinates;
	the coordinated ranges are 
	return true if this point should be clipped;
	return false if not;
*/
bool A3DCamera::Transform(A3DVECTOR3& vecIn, A3DVECTOR3& vecOut)
{
	vecOut = vecIn * m_matVPTM;

	if( vecOut.x < -1.0f || vecOut.y < -1.0f ||
		vecOut.x > 1.0f || vecOut.y > 1.0f )
		return true;
	else
		return false;
}

/*
	Transform one projection plane coordinates into world space coordinates;
*/
bool A3DCamera::InvTransform(A3DVECTOR3& vecIn, A3DVECTOR3& vecOut)
{
	vecOut = vecIn * m_matInvVPTM;

	return true;
}

/* 
	Rotate the camera around an axis in world space;
*/
bool A3DCamera::TurnAroundAxis(A3DVECTOR3 vecPos, A3DVECTOR3 vecAxis, FLOAT vDeltaRad)
{
	A3DMATRIX4 matRotate = RotateAxis(vecPos, vecAxis, vDeltaRad);
	A3DVECTOR3 vecOrigin = A3DVECTOR3(0.0f) * matRotate;
	m_vecPos = m_vecPos * matRotate;
	m_vecDir = m_vecDir * matRotate - vecOrigin;
	m_vecUp  = m_vecUp  * matRotate - vecOrigin;

	SetDirAndUp(m_vecDir, m_vecUp);
	return true;
}

//	Create view frustum in view coordinates
bool A3DCamera::CreateViewFrustum()
{
	//	Construct view frustum in view space
	float c, s, fAngle = m_vFOV * 0.5f;

	c = (float)cos(fAngle);
	s = (float)sin(fAngle);

	//	Top and bottom clip planes
	m_aVCPlanes[CLIP_TOP].vNormal	 = A3DVECTOR3(0.0f, -c, s);
	m_aVCPlanes[CLIP_BOTTOM].vNormal = A3DVECTOR3(0.0f, c, s);

	m_aVCPlanes[CLIP_TOP].fDist = m_aVCPlanes[CLIP_BOTTOM].fDist = 0.0f;

	fAngle = (float)(tan(fAngle) * m_vRatio);

	c = (float)cos(atan(fAngle));
	s = (float)sin(atan(fAngle));

	//	Left and right clip planes
	m_aVCPlanes[CLIP_LEFT].vNormal	 = A3DVECTOR3(c, 0.0f, s);
	m_aVCPlanes[CLIP_RIGHT].vNormal	 = A3DVECTOR3(-c, 0.0f, s);

	m_aVCPlanes[CLIP_LEFT].fDist = m_aVCPlanes[CLIP_RIGHT].fDist = 0.0f;

	//	Near and Far clipping plane
	m_aVCPlanes[CLIP_NEAR].vNormal	 = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_aVCPlanes[CLIP_FAR].vNormal	 = A3DVECTOR3(0.0f, 0.0f, -1.0f);

	m_aVCPlanes[CLIP_NEAR].fDist = m_aVCPlanes[CLIP_FAR].fDist = 0.0f;

	return true;
}

//	Update view frustum and translate it into world coordinates
bool A3DCamera::UpdateViewFrustum()
{
	//	Translate left plane
	m_aWCPlanes[CLIP_LEFT].vNormal	= ViewToWorld(m_aVCPlanes[CLIP_LEFT].vNormal, m_matViewTM);
	m_aWCPlanes[CLIP_LEFT].fDist	= DotProduct(m_aWCPlanes[CLIP_LEFT].vNormal, m_vecPos);
	m_aWCPlanes[CLIP_LEFT].byType	= A3DPLANE_UNKNOWN;
	m_aWCPlanes[CLIP_LEFT].bySignBits = GetA3DPlaneSignBits(m_aWCPlanes[CLIP_LEFT].vNormal);

	//	Translate right plane
	m_aWCPlanes[CLIP_RIGHT].vNormal	= ViewToWorld(m_aVCPlanes[CLIP_RIGHT].vNormal, m_matViewTM);
	m_aWCPlanes[CLIP_RIGHT].fDist	= DotProduct(m_aWCPlanes[CLIP_RIGHT].vNormal, m_vecPos);
	m_aWCPlanes[CLIP_RIGHT].byType	= A3DPLANE_UNKNOWN;
	m_aWCPlanes[CLIP_RIGHT].bySignBits = GetA3DPlaneSignBits(m_aWCPlanes[CLIP_RIGHT].vNormal);

	//	Translate top plane
	m_aWCPlanes[CLIP_TOP].vNormal	= ViewToWorld(m_aVCPlanes[CLIP_TOP].vNormal, m_matViewTM);
	m_aWCPlanes[CLIP_TOP].fDist		= DotProduct(m_aWCPlanes[CLIP_TOP].vNormal, m_vecPos);
	m_aWCPlanes[CLIP_TOP].byType	= A3DPLANE_UNKNOWN;
	m_aWCPlanes[CLIP_TOP].bySignBits = GetA3DPlaneSignBits(m_aWCPlanes[CLIP_TOP].vNormal);

	//	Translate bottom plane
	m_aWCPlanes[CLIP_BOTTOM].vNormal = ViewToWorld(m_aVCPlanes[CLIP_BOTTOM].vNormal, m_matViewTM);
	m_aWCPlanes[CLIP_BOTTOM].fDist	 = DotProduct(m_aWCPlanes[CLIP_BOTTOM].vNormal, m_vecPos);
	m_aWCPlanes[CLIP_BOTTOM].byType	 = A3DPLANE_UNKNOWN;
	m_aWCPlanes[CLIP_BOTTOM].bySignBits = GetA3DPlaneSignBits(m_aWCPlanes[CLIP_BOTTOM].vNormal);

	//	Translate near plane
	A3DVECTOR3 vPos = m_vecPos + m_vecDir * m_vFront;

	m_aWCPlanes[CLIP_NEAR].vNormal	= ViewToWorld(m_aVCPlanes[CLIP_NEAR].vNormal, m_matViewTM);
	m_aWCPlanes[CLIP_NEAR].fDist	= DotProduct(m_aWCPlanes[CLIP_NEAR].vNormal, vPos);
	m_aWCPlanes[CLIP_NEAR].byType	= A3DPLANE_UNKNOWN;
	m_aWCPlanes[CLIP_NEAR].bySignBits = GetA3DPlaneSignBits(m_aWCPlanes[CLIP_NEAR].vNormal);

	//	Transform far plane
	vPos = m_vecPos + m_vecDir * m_vBack;

	m_aWCPlanes[CLIP_FAR].vNormal	= ViewToWorld(m_aVCPlanes[CLIP_FAR].vNormal, m_matViewTM);
	m_aWCPlanes[CLIP_FAR].fDist		= DotProduct(m_aWCPlanes[CLIP_FAR].vNormal, vPos);
	m_aWCPlanes[CLIP_FAR].byType	= A3DPLANE_UNKNOWN;
	m_aWCPlanes[CLIP_FAR].bySignBits = GetA3DPlaneSignBits(m_aWCPlanes[CLIP_FAR].vNormal);

	return true;
}

/*	Check whether aabb is completely in view frustum.

	Return true if aabb is completely in view frustum.

	aabb: A3D aabb object, only Mins and Maxs components will be used
*/
bool A3DCamera::AABBInViewFrustum(A3DAABB& aabb)
{
	//	Don't check front and back plane
	for (int i=0; i < CLIP_PLANENUM-2; i++)
	{
		if (CLS_PlaneToAABB(m_aWCPlanes[i], aabb.Mins, aabb.Maxs) < 0)
			return false;
	}
	
	return true;
}

//	Check whether sphere is in view frustum
bool A3DCamera::SphereInViewFrustum(A3DVECTOR3& vCenter, float fRadius)
{
	//	Don't check front and back plane
	for (int i=0; i < CLIP_PLANENUM-2; i++)
	{
		if (CLS_PlaneToSphere(m_aWCPlanes[i], vCenter, fRadius) < 0)
			return false;
	}

	return true;
}

bool A3DCamera::SetDeg(FLOAT vDeg)
{
	m_vDeg = vDeg;

	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

bool A3DCamera::SetPitch(FLOAT vPitch)
{
	m_vPitch = vPitch;

	if( m_vPitch > 89.0f )
		m_vPitch = 89.0f;
	else if( m_vPitch < -89.0f )
		m_vPitch = -89.0f;

	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

// Please don't call this function only once per tick, or maybe for several tick,
// so we can save some cpu usage;
bool A3DCamera::UpdateEar()
{
	// Now we should adjust the 3d sound device's pos and orientation;
	if( m_pAM3DSoundDevice )
	{
		m_pAM3DSoundDevice->SetPosition(m_vecPos);
		m_pAM3DSoundDevice->SetOrientation(m_vecDir, m_vecUp);
		m_pAM3DSoundDevice->UpdateChanges();
	}
	return true;
}

bool A3DCamera::SetZBias(FLOAT vZBias)
{
	A3DMATRIX4 matProjectTM;

	matProjectTM = m_matProjectTM;
	//matProjectTM._43 += vZBias;
	matProjectTM._33 *= vZBias;
	
	m_pA3DDevice->SetProjectionMatrix(matProjectTM * m_matPostProjectTM);
	return true;
}

bool A3DCamera::SetProjectionParam(FLOAT vFov, FLOAT vFront, FLOAT vBack, FLOAT vRatio)
{
	if( vFov > 0.0f )
		m_vFOV = vFov;
	if( vFront > 0.0f )
		m_vFront = vFront;
	if( vBack > 0.0f )
		m_vBack = vBack;
	if( vRatio > 0.0f )
		m_vRatio = vRatio;

	return UpdateProjectTM();
}