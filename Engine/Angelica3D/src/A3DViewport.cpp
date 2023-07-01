#include "A3DViewport.h"
#include "A3DErrLog.h"
#include "A3DMacros.h"
#include "A3DEngine.h"

A3DViewport::A3DViewport()
{
	m_pA3DDevice = NULL;
	m_pA3DCamera = NULL;
	m_bClearColor = false;
	m_bClearZ = false;
	m_colorClear = A3DCOLORRGB(0, 0, 0);
}

A3DViewport::~A3DViewport()
{
}

bool A3DViewport::Init(A3DDevice * pA3DDevice, A3DVIEWPORTPARAM * pParam, bool bClearColor, bool bClearZ, A3DCOLOR colorClear)
{
	m_pA3DDevice = pA3DDevice;
	m_pA3DCamera = NULL;
	
	m_bClearColor = bClearColor;
	m_bClearZ = bClearZ;
	m_colorClear = colorClear;

	return SetParam(pParam);
}

bool A3DViewport::SetParam(A3DVIEWPORTPARAM * pParam)
{
	m_ViewportParam = *pParam;

	//Construct the view scale transform matrix;
	m_matViewScale = IdentityMatrix();
	m_matViewScale._11 = 1.0f * m_ViewportParam.Width / 2;
	m_matViewScale._22 = -1.0f * m_ViewportParam.Height / 2;
	m_matViewScale._33 = m_ViewportParam.MaxZ - m_ViewportParam.MinZ;
	m_matViewScale._41 = 1.0f * m_ViewportParam.X + m_ViewportParam.Width / 2;
	m_matViewScale._42 = 1.0f * m_ViewportParam.Y + m_ViewportParam.Height / 2;
	m_matViewScale._43 = m_ViewportParam.MinZ;
	
	m_matInvViewScale = InverseTM(m_matViewScale);
	return true;
}

bool A3DViewport::Release()
{
	return true;
}

bool A3DViewport::Active()
{
	//Viewport that was not associated with any camera will not active at all;
	if( NULL == m_pA3DCamera )
	{
		g_pA3DErrLog->ErrLog("A3DViewport::Active() m_pA3DCamera is NULL!");
		return false;
	}

	m_pA3DDevice->GetA3DEngine()->SetActiveViewport(this);
	m_pA3DDevice->GetA3DEngine()->SetActiveCamera(m_pA3DCamera);
	m_pA3DDevice->SetViewport(&m_ViewportParam);
	m_pA3DCamera->Active();
	return true;
}

bool A3DViewport::SetCamera(A3DCamera * pA3DCamera)
{
	m_pA3DCamera = pA3DCamera;
	return true;
}

bool A3DViewport::ClearDevice()
{
	DWORD	dwFlags = 0;
	
	if( m_bClearColor )
	{
		dwFlags |= D3DCLEAR_TARGET;
	}

	if( m_bClearZ )
	{
		dwFlags |= D3DCLEAR_ZBUFFER;
	}

	if( NULL == dwFlags )
		return true;

	if FAILED(m_pA3DDevice->Clear(dwFlags, m_colorClear, 1.0f, 0) )
		return false;

	return true;
}

/*
	Transform a 3D point from world to screen coordinates;
	return true if that point should be clipped;
	else return false;

	the vecOut is x, y, z, w, and the real x', y', z' should be x/w, y/w, z/w;
*/
bool A3DViewport::Transform(A3DVECTOR3& vecIn, A3DVECTOR3& vecOut)
{
	bool		bClipped;
	A3DVECTOR3	vecView;
								 
	bClipped = m_pA3DCamera->Transform(vecIn, vecView);

	if( vecView.z < m_ViewportParam.MinZ || vecView.z > m_ViewportParam.MaxZ )
		bClipped = true;

	vecOut = vecView * m_matViewScale;
	return bClipped;
}

/*
	Transform a screen point to world space coordinates;
*/
bool A3DViewport::InvTransform(A3DVECTOR3& vecIn, A3DVECTOR3& vecOut)
{
	A3DVECTOR3	vecView;
			
	vecView = vecIn * m_matInvViewScale;

	m_pA3DCamera->InvTransform(vecView, vecOut);
	return true;
}