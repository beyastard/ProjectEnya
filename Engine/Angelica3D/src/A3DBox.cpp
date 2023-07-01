#include "A3DBox.h"
#include "A3DFuncs.h"
#include "A3DErrLog.h"
#include "A3DEngine.h"
#include "A3DOBBFuncs.h"
#include "A3DConfig.h"

A3DBox::A3DBox()
{
	m_pA3DDevice = NULL;
	m_pA3DStream = NULL;
}

A3DBox::~A3DBox()
{
}

bool A3DBox::Init(A3DDevice * pA3DDevice, A3DCOLOR diffuse, A3DCOLOR specular)
{
//#ifdef _DEBUG
	if( !pA3DDevice || g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;
/*
#else
	return true;
#endif*/

	m_pA3DDevice = pA3DDevice;
	m_Texture.Init(pA3DDevice);

	m_pA3DStream = new A3DStream();
	if( NULL == m_pA3DStream )
	{
		g_pA3DErrLog->ErrLog("A3DBox::Init Not Enough Memory!");
		return false;
	}

	if( !m_pA3DStream->Init(pA3DDevice, A3DVT_LVERTEX, 8, 12 * 3, 0, true) )
	{
		g_pA3DErrLog->ErrLog("A3DBox::Init Init Mesh Fail!");
		return false;
	}

	// Use an identity obb;
	A3DOBB a3dOBB;

	a3dOBB.Center	= A3DVECTOR3(0.0f, 0.0f, 0.0f);
	a3dOBB.XAxis	= A3DVECTOR3(1.0f, 0.0f, 0.0f);
	a3dOBB.YAxis	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
	a3dOBB.ZAxis	= A3DVECTOR3(0.0f, 0.0f, 1.0f);
	a3dOBB.Extents	= A3DVECTOR3(1.0f, 1.0f, 1.0f);

	CompleteOBB(&a3dOBB);

	WORD index[24];
	if( !OBBToVertex(NULL, index, a3dOBB) )
		return false;

	if( !m_pA3DStream->SetIndices(index, 24) )
	{
		g_pA3DErrLog->ErrLog("A3DBox::Init m_pA3DStream Set Indices Fail!");
		return false;
	}

	if( !UpdateBox(a3dOBB, diffuse, specular) )
		return false;

	return true;
}

bool A3DBox::UpdateBox(const A3DAABB& a3dAABB, A3DCOLOR diffuse, A3DCOLOR specular)
{
	if( !m_pA3DDevice )
		return true;

	A3DOBB a3dOBB;

	a3dOBB.Center	= a3dAABB.Center;
	a3dOBB.XAxis	= A3DVECTOR3(1.0f, 0.0f, 0.0f);
	a3dOBB.YAxis	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
	a3dOBB.ZAxis	= A3DVECTOR3(0.0f, 0.0f, 1.0f);
	a3dOBB.Extents	= a3dAABB.Extents;

	CompleteOBB(&a3dOBB);

	return UpdateBox(a3dOBB, diffuse, specular);
}

bool A3DBox::UpdateBox(const A3DOBB& a3dOBB, A3DCOLOR diffuse, A3DCOLOR specular)
{
	if( !m_pA3DDevice )
		return true;

	if( m_pA3DDevice->GetA3DEngine()->GetBoxRenderFlag() )
	{
		A3DLVERTEX	verts[8];
		A3DVECTOR3  vertPos[8];
		
		m_a3dOBB = a3dOBB;

		OBBToVertex(vertPos, NULL, m_a3dOBB);
		for(int i=0; i<8; i++)
			verts[i] = A3DLVERTEX(vertPos[i], diffuse, specular, 0.0f, 0.0f);

		if( !m_pA3DStream->SetVerts((BYTE *)verts, 8) )
		{
			g_pA3DErrLog->ErrLog("A3DBox::UpdateBox m_pA3DStream Set Verts Fail!");
			return false;
		}
	}
	else
		return true;

	return true;
}

bool A3DBox::Release()
{
	if( !m_pA3DDevice )
		return true;

	if( m_pA3DStream )
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}
	m_Texture.Release();
	return true;
}

bool A3DBox::Render(A3DViewport * pCurrentViewport)
{
	if( !m_pA3DDevice )
		return true;

	if( m_pA3DDevice->GetA3DEngine()->GetBoxRenderFlag() )
	{
		m_Texture.Appear(0);
		m_pA3DStream->Appear();
		m_pA3DDevice->SetLightingEnable(false);
		m_pA3DDevice->SetFillMode(A3DFILL_WIREFRAME);
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_LINELIST, 0, 8, 0, 12) )
		{
			m_pA3DDevice->SetLightingEnable(true);
			m_pA3DDevice->SetFillMode(A3DFILL_SOLID);
			return false;
		}
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
		m_pA3DDevice->SetLightingEnable(true);
		m_pA3DDevice->SetFillMode(A3DFILL_SOLID);
		return true;
	}
	else
		return true;
}