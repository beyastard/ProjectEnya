/*
 * FILE: A3DCamera.h
 *
 * DESCRIPTION: Class representing a camera
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCAMERA_H_
#define _A3DCAMERA_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DFrame.h"
#include "A3DDevice.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"

#include <AM3DSoundDevice.h>

class A3DCamera	: public A3DObject
{
public:		//	Types

	//	Clipping planes
	enum
	{
		CLIP_LEFT		= 0,
		CLIP_RIGHT		= 1,
		CLIP_TOP		= 2,
		CLIP_BOTTOM		= 3,
		CLIP_NEAR		= 4,
		CLIP_FAR		= 5,
		CLIP_PLANENUM	= 6
	};

private:
	A3DDevice *			m_pA3DDevice;
	AM3DSoundDevice *	m_pAM3DSoundDevice;

	A3DVECTOR3			m_vecPos;
	A3DVECTOR3			m_vecDir;
	A3DVECTOR3			m_vecUp;
	A3DVECTOR3			m_vecLeft;
	A3DVECTOR3			m_vecRight;
	A3DVECTOR3			m_vecDirH;
	A3DVECTOR3			m_vecLeftH;

	A3DMATRIX4			m_matViewTM;
	A3DMATRIX4			m_matProjectTM;
	A3DMATRIX4			m_matVPTM;
	A3DMATRIX4			m_matPostProjectTM;	// Transform matrix used for post project process

	A3DMATRIX4			m_matInvProjectTM;
	A3DMATRIX4			m_matInvVPTM;

	A3DCamera *			m_pMirrorCamera;
	A3DMATRIX4			m_matMirror;
	bool				m_bMirrored;

	FLOAT				m_vDeg;
	FLOAT				m_vPitch;

	FLOAT				m_vFOV;
	FLOAT				m_vFOVSet;
	FLOAT				m_vRatio;
	FLOAT				m_vFront;
	FLOAT				m_vBack;

	//	Frustum planes in view coordinates and world coordinates
	A3DPLANE			m_aVCPlanes[CLIP_PLANENUM];
	A3DPLANE			m_aWCPlanes[CLIP_PLANENUM];

protected:
	bool UpdateViewTM();
	bool UpdateProjectTM();
	bool UpdateDirAndUp();

	bool CreateViewFrustum();		//	Create view frustum
	bool UpdateViewFrustum();		//	Update view frustum

public:
	A3DCamera();
	~A3DCamera();

	bool Init(A3DDevice * pDevice, FLOAT vFov=DEG2RAD(65.0f), FLOAT vFront=1.0f, FLOAT vBack=2000.0f, FLOAT vRatio=1.3333333f);
	bool Release();

	void SetDirAndUp(A3DVECTOR3 vecDir, A3DVECTOR3 vecUp);

	bool Active();

	bool MoveFront(FLOAT vDistance);
	bool MoveBack(FLOAT vDistance);
	bool MoveLeft(FLOAT vDistance);
	bool MoveRight(FLOAT vDistance);
	bool Move(A3DVECTOR3 vecDelta);

	bool TurnAroundAxis(A3DVECTOR3 vecPos, A3DVECTOR3 vecAxis, FLOAT vDeltaRad);

	// Please don't call this function only once per tick, or maybe for several tick,
	// so we can save some cpu usage;
	bool UpdateEar();

	bool DegDelta(FLOAT vDelta);
	bool PitchDelta(FLOAT vDelta);
	bool SetDeg(FLOAT vDeg);
	bool SetPitch(FLOAT vPitch);
	
	bool AABBInViewFrustum(A3DAABB& aabb);		//	Check whether aabb is in view frustum
	bool SphereInViewFrustum(A3DVECTOR3& vCenter, float fRadius);	//	Check whether sphere is in view frustum

	A3DVECTOR3 GetPos();
	bool Transform(A3DVECTOR3& vecIn, A3DVECTOR3& vecOut);
	bool InvTransform(A3DVECTOR3& vecIn, A3DVECTOR3& vecOut);

	bool SetMirror(A3DCamera * pCamera, A3DVECTOR3 vecOrg, A3DVECTOR3 vecNormal);
	bool Update();
	bool Filter(A3DFrame * pFrame); //This is used to filter the objects when using different viewport;

	bool SetViewTM(A3DMATRIX4 matView);
	bool SetZBias(FLOAT vZBias);
	bool SetProjectionTM(A3DMATRIX4 matProjection);

	bool SetProjectionParam(FLOAT vFov, FLOAT vFront, FLOAT vBack, FLOAT vRatio);
	inline void GetProjectionParam(FLOAT * pvFov, FLOAT * pvFront, FLOAT * pvBack, FLOAT * pvRatio) { *pvFov = m_vFOV; *pvFront = m_vFront; *pvBack = m_vBack; *pvRatio = m_vRatio; }

	//UpdateViewTM should be called after set pos; but we should think
	//carefully about when to call it;
	inline void SetPos(A3DVECTOR3 vecPos) { m_vecPos = vecPos; UpdateViewTM(); }
	inline A3DMATRIX4 GetViewTM() { return m_matViewTM; }
	inline A3DMATRIX4 GetProjectionTM() { return m_matProjectTM; }
	inline A3DMATRIX4 GetVPTM() { return m_matVPTM; }
	inline A3DVECTOR3 GetDir() { return m_vecDir; }
	inline A3DVECTOR3 GetDirH() { return m_vecDirH; }
	inline A3DVECTOR3 GetUp() { return m_vecUp; }
	inline A3DVECTOR3 GetRight() { return m_vecRight; }
	inline A3DVECTOR3 GetRightH() { return -1.0f * m_vecLeftH; }
	inline A3DVECTOR3 GetLeft() { return m_vecLeft; }
	inline A3DVECTOR3 GetLeftH() { return m_vecLeftH; }
	inline void SetFOV(FLOAT vFOV) { m_vFOV = vFOV; UpdateProjectTM(); }
	inline FLOAT GetFOV() { return m_vFOV; }
	inline void SetFOVSet(FLOAT vFOVSet) { m_vFOVSet = vFOVSet; }
	inline FLOAT GetDeg() { return m_vDeg; }
	inline FLOAT GetPitch() { return m_vPitch; }

	inline A3DMATRIX4 GetPostProjectTM() { return m_matPostProjectTM; }
	inline void SetPostProjectTM(A3DMATRIX4 matPostProjectTM) { m_matPostProjectTM = matPostProjectTM; UpdateProjectTM(); }

	inline void SetAM3DSoundDevice(AM3DSoundDevice * pAM3DSoundDevice) { m_pAM3DSoundDevice = pAM3DSoundDevice; }
	inline AM3DSoundDevice * GetAM3DSoundDevice() { return m_pAM3DSoundDevice; }

	inline A3DPLANE * GetClipPlanePointer() { return m_aWCPlanes; }
};

typedef A3DCamera * PA3DCamera;
#endif
