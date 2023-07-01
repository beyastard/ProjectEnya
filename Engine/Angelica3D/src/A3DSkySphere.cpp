#include "A3DSkySphere.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "A3DErrLog.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DConfig.h"

A3DSkySphere::A3DSkySphere()
{
	m_pA3DDevice		= NULL;
	m_pCamera			= NULL;

	m_pStreamSurround	= NULL;
	m_pStreamCap		= NULL;

	m_pTextureSurroundF = NULL;
	m_pTextureSurroundB = NULL;
	m_pTextureCap		= NULL;

	m_nSegmentCount		= 6;
	m_vPitchRange		= DEG2RAD(17.5f);
	m_fRadius			= 100.0f;
	m_fFlySpeedU		= 0.018f;
	m_fFlySpeedV		= 0.018f;
	m_fTile				= 8.0f;
}

A3DSkySphere::~A3DSkySphere()
{
}

bool A3DSkySphere::Init(A3DDevice * pDevice, A3DCamera * pCamera, char * szCap, char * szSurroundF, char * szSurroundB)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;
	
	m_pA3DDevice = pDevice;
	m_pCamera = pCamera;

	// First Create Surround Cloud;
	if( !CreateSurround() )
		return false;

	if( !CreateCap() )
		return false;

	//Load the textures;
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szCap, "Textures\\Sky", &m_pTextureCap, A3DTF_MIPLEVEL, 1) )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szSurroundF, "Textures\\Sky", &m_pTextureSurroundF, A3DTF_MIPLEVEL, 1) )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szSurroundB, "Textures\\Sky", &m_pTextureSurroundB, A3DTF_MIPLEVEL, 1) )
		return false;

	m_WorldMatrix = IdentityMatrix();
	m_tmCapCloud = IdentityMatrix();
	return true;
}

bool A3DSkySphere::Release()
{
	if( m_pTextureCap )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureCap);
		m_pTextureCap = NULL;
	}
	if( m_pTextureSurroundF )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundF);
		m_pTextureSurroundF = NULL;
	}
	if( m_pTextureSurroundB )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundB);
		m_pTextureSurroundB = NULL;
	}
	if( m_pStreamSurround )
	{
		m_pStreamSurround->Release();
		delete m_pStreamSurround;
		m_pStreamSurround = NULL;
	}
	if( m_pStreamCap )
	{
		m_pStreamCap->Release();
		delete m_pStreamCap;
		m_pStreamCap = NULL;
	}
	return true;
}

bool A3DSkySphere::Render()
{	
	if( !m_pA3DDevice ) return true;

	//Update World Matrix First;
	if( m_pCamera )
	{
		A3DVECTOR3 vecPos = m_pCamera->GetPos();
		m_WorldMatrix = Translate(vecPos.x, vecPos.y, vecPos.z);
	}
	
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetAlphaTestEnable(false);
	
	//First of all show top cap of the surround cloud;
	if( m_pTextureCap )
	{
		A3DMATRIX4 matCap;
		matCap = m_WorldMatrix;
		matCap._42 -= m_fRadius * (float)cos(m_vPitchRange);
		m_pA3DDevice->SetWorldMatrix(matCap);
		m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT2);
		m_pA3DDevice->SetTextureMatrix(0, m_tmCapCloud);
		m_pTextureCap->Appear(0);
		m_pStreamCap->Appear();
		if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountC, 0, m_nIndexCountC / 3) )
		{
			g_pA3DErrLog->ErrLog("A3DSky::Render DrawIndexPrimitive Fail!");
			return false;
		}
		m_pTextureCap->Disappear(0);
		m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	}
	
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	//Then, we show the surround cloud front part first;
	if( m_pTextureSurroundF )
	{
		m_pA3DDevice->SetWorldMatrix(m_WorldMatrix);
		m_pTextureSurroundF->Appear(0);
		m_pStreamSurround->Appear();
		
		if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountS, 0, m_nIndexCountS / 3) )
		{
			g_pA3DErrLog->ErrLog("A3DSky::Render DrawIndexPrimitive Fail!");
			return false;
		}
		m_pTextureSurroundF->Disappear(0);
	}

	//Second show the surround cloud back part;
	if( m_pTextureSurroundB )
	{
		A3DMATRIX4 matRotate180 = IdentityMatrix();
		matRotate180._11 = -1.0f;
		matRotate180._33 = -1.0f;
		m_pA3DDevice->SetWorldMatrix(matRotate180 * m_WorldMatrix);
		m_pTextureSurroundB->Appear(0);
		m_pStreamSurround->Appear();
		
		if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCountS, 0, m_nIndexCountS / 3) )
		{
			g_pA3DErrLog->ErrLog("A3DSkySphere::Render DrawIndexPrimitive Fail!");
			return false;
		}
		m_pTextureSurroundB->Disappear(0);
	}

	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

	m_pA3DDevice->SetZWriteEnable(TRUE);
	m_pA3DDevice->SetZTestEnable(TRUE);
	m_pA3DDevice->SetLightingEnable(TRUE);
	m_pA3DDevice->SetFogEnable(TRUE);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	return true;
}

bool A3DSkySphere::TickAnimation()
{
	if( !m_pA3DDevice ) return true;

	m_tmCapCloud._11 = m_fTile;
	m_tmCapCloud._22 = m_fTile;
	m_tmCapCloud._31 += m_fFlySpeedU / 30.0f;
	m_tmCapCloud._32 += m_fFlySpeedV / 30.0f;
	return true;
}

bool A3DSkySphere::SetCamera(A3DCamera * pCamera)
{
	if( !m_pA3DDevice ) return true;

	//If the new set camera is Null, then the old camera is retained;
	if( NULL == pCamera )
		return true;

	m_pCamera = pCamera;
	return true;
}

bool A3DSkySphere::SetTextureSurroundF(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureSurroundF )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundF);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureSurroundF, A3DTF_MIPLEVEL, 1) )
	{
		g_pA3DErrLog->ErrLog("A3DSkySphere::SetTextureSurroundF(), Can't load texture [%s]", szTextureFile);
		return false;
	}
	
	return true;
}

bool A3DSkySphere::SetTextureSurroundB(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureSurroundB )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureSurroundB);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureSurroundB, A3DTF_MIPLEVEL, 1) )
	{
		g_pA3DErrLog->ErrLog("A3DSkySphere::SetTextureSurroundB(), Can't load texture [%s]", szTextureFile);
		return false;
	}

	return true;
}

bool A3DSkySphere::SetTextureCap(char * szTextureFile)
{
	if( !m_pA3DDevice ) return true;

	if( m_pTextureCap )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextureCap);
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureFile, "Textures\\Sky", &m_pTextureCap, A3DTF_MIPLEVEL, 1) )
	{
		g_pA3DErrLog->ErrLog("A3DSkySphere::SetTextureSurroundB(), Can't load texture [%s]", szTextureFile);
		return false;
	}

	return true;
}

bool A3DSkySphere::CreateSurround()
{
	bool bval;

	m_nVertCountS = (m_nSegmentCount + 1) * 4 * 2;
	m_nIndexCountS = m_nSegmentCount * 3 * 6 * 2;

	A3DLVERTEX *	pVerts;
	WORD *			pIndices;

	m_pStreamSurround = new A3DStream();
	if( NULL == m_pStreamSurround )
	{
		g_pA3DErrLog->ErrLog("A3DSkySphere::Init(), Not enough memory!");
		return false;
	}
	bval = m_pStreamSurround->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nVertCountS, m_nIndexCountS, 0);
	if( !bval )
	{
		g_pA3DErrLog->ErrLog("A3DSkySphere::Init(), Stream Surround Init Fail!");
		return false;
	}
	bval = m_pStreamSurround->LockVertexBuffer(0, 0, (LPBYTE *)&pVerts, NULL);
	if( !bval )
	{
		g_pA3DErrLog->ErrLog("A3DSkySphere::Init(), Stream Surround Lock Verts Fail!");
		return false;
	}
	bval = m_pStreamSurround->LockIndexBuffer(0, 0, (LPBYTE *)&pIndices, NULL);
	if( !bval )
	{
		g_pA3DErrLog->ErrLog("A3DSkySphere::Init(), Stream Surround Lock Indices Fail!");
		return false;
	}

	// Create a half-angle cylinder to be used as surround(front and back) cloud
	WORD i, j;
	FLOAT vHeight;
	FLOAT alpha;
	int   nVertIndex = 0;
	int   nFaceIndex = 0;
	int   is = (m_nSegmentCount + 1) * 2;
	float h = m_fRadius * 0.7854f;

	for(j=0; j<=3; j++)
	{
		if( j == 0 )
			vHeight = h;
		else if( j == 1 )
			vHeight = h / 6.0f;
		else if( j == 2 )
			vHeight = 0.0f;
		else if( j == 3 )
			vHeight = -m_fRadius / 3.0f;

		//Left Quadro Cynlinde,r;
		for(i=0; i<=m_nSegmentCount; i++)
		{
			alpha = A3D_PI / 2.0f + A3D_PI / 2.0f * i / m_nSegmentCount;

			pVerts[nVertIndex].x = m_fRadius * (float)cos(alpha);
			pVerts[nVertIndex].y = vHeight;
			pVerts[nVertIndex].z = m_fRadius * (float)sin(alpha);
			if( j != 0 )
				pVerts[nVertIndex].diffuse = 0xffffffff;
			else
				pVerts[nVertIndex].diffuse = 0x00ffffff;
			pVerts[nVertIndex].specular = 0xff000000;
			pVerts[nVertIndex].tu = 1.0f - 1.0f * i / m_nSegmentCount; 
			if( j != 3 )
				pVerts[nVertIndex].tv = (1.0f - vHeight / h) * 0.5f * 0.9f + 0.05f;
			else
				pVerts[nVertIndex].tv = 0.5f;

			
			if( i != m_nSegmentCount && j != 3 )
			{
				pIndices[nFaceIndex * 3] = nVertIndex;
				pIndices[nFaceIndex * 3 + 1] = nVertIndex + is;
				pIndices[nFaceIndex * 3 + 2] = nVertIndex + is + 1;

				pIndices[nFaceIndex * 3 + 3] = nVertIndex;
				pIndices[nFaceIndex * 3 + 4] = nVertIndex + is + 1;
				pIndices[nFaceIndex * 3 + 5] = nVertIndex + 1;

				nFaceIndex += 2;
			}

			nVertIndex ++;
		}

		//Right Quadro Sphere;
		for( i=0; i<=m_nSegmentCount; i++ )
		{
			alpha = A3D_PI / 2.0f * i / m_nSegmentCount;

			pVerts[nVertIndex].x = m_fRadius * (float)cos(alpha);
			pVerts[nVertIndex].y = vHeight;
			pVerts[nVertIndex].z = m_fRadius * (float)sin(alpha);
			if( j != 0 )
				pVerts[nVertIndex].diffuse = 0xffffffff;
			else
				pVerts[nVertIndex].diffuse = 0x00ffffff;
			pVerts[nVertIndex].specular = 0xff000000;
			pVerts[nVertIndex].tu = 1.0f - 1.0f * i / m_nSegmentCount; 
			if( j != 3 )
				pVerts[nVertIndex].tv = (0.5f + vHeight / h * 0.5f) * 0.9f + 0.05f;
			else
				pVerts[nVertIndex].tv = 0.5f;
			
			if( i != m_nSegmentCount && j != 3 )
			{
				pIndices[nFaceIndex * 3] = nVertIndex;
				pIndices[nFaceIndex * 3 + 1] = nVertIndex + is;
				pIndices[nFaceIndex * 3 + 2] = nVertIndex + is + 1;

				pIndices[nFaceIndex * 3 + 3] = nVertIndex;
				pIndices[nFaceIndex * 3 + 4] = nVertIndex + is + 1;
				pIndices[nFaceIndex * 3 + 5] = nVertIndex + 1;

				nFaceIndex += 2;
			}

			nVertIndex ++;
		}
	}

	m_pStreamSurround->UnlockVertexBuffer();
	m_pStreamSurround->UnlockIndexBuffer();

	return true;
}

bool A3DSkySphere::CreateCap()
{
	// Now calculate the face count and vert count of the hemisphere sky cone part;
	m_nVertCountC = 1 + m_nSegmentCount * (4 * m_nSegmentCount);
	m_nIndexCountC = (4 * m_nSegmentCount + (m_nSegmentCount - 1) * (4 * m_nSegmentCount) * 2) * 3;

	// Then create stream for rendering usage
	m_pStreamCap = new A3DStream();
	if( NULL == m_pStreamCap )
	{
		g_pA3DErrLog->ErrLog("A3DSkySphere::Init(), Not enough memory!");
		return false;
	}
	if( !m_pStreamCap->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nVertCountC, m_nIndexCountC, 0) )
	{
		g_pA3DErrLog->ErrLog("A3DSkySphere::Init(), Init the A3DStream fail!");
		return false;
	}

	A3DLVERTEX *	pVerts;
	WORD *			pIndices;
	
	// Fill verts;
	if( !m_pStreamCap->LockVertexBuffer(0, 0, (LPBYTE *)&pVerts, NULL) )
		return false;
	
	float r = m_fRadius * (float)sin(m_vPitchRange);
	float h = m_fRadius - m_fRadius * (float)cos(m_vPitchRange);

	// First it is my cap center vertex;
	pVerts[0] = A3DLVERTEX(A3DVECTOR3(0.0f, m_fRadius, 0.0f), 0xffffffff, 0xff000000, 0.0f, 0.0f);

	// Then hemisphere middle part;
	int i, ix, iy;
	int is = m_nSegmentCount * 4; // vert count of each segment
	int nVertIndex = 1;
	for(iy=0; iy<m_nSegmentCount; iy++)
	{
		for(ix=0; ix<4 * m_nSegmentCount; ix++)
		{
			float alpha = A3D_PI * 2.0f * ix / is;
			float pitch = A3D_PI / 2.0f - (iy + 1) * m_vPitchRange / m_nSegmentCount;
			
			float vX = float(m_fRadius * cos(alpha) * cos(pitch));
			float vY = float(m_fRadius * sin(pitch));
			float vZ = float(m_fRadius * sin(alpha) * cos(pitch));

			pVerts[nVertIndex] = A3DLVERTEX(A3DVECTOR3(vX, vY, vZ), 0xffffffff, 0xff000000, 0.0f, 0.0f);
			
			nVertIndex ++;
		}
	}

	// Now we apply a plain texture maping to it;
	for(i=0; i<m_nVertCountC; i++)
	{
		pVerts[i].tu = (pVerts[i].x / r + 1.0f) * 0.5f;
		pVerts[i].tv = (pVerts[i].z / r + 1.0f) * 0.5f;
	}

	m_pStreamCap->UnlockVertexBuffer();

	// Fill the indcies;
	if( !m_pStreamCap->LockIndexBuffer(0, 0, (LPBYTE *)&pIndices, NULL) )
		return false;
	
	// First conic cap;
	int nFaceIndex = 0;
	for(i=0; i<is; i++)
	{
		pIndices[nFaceIndex * 3] = 0;
		pIndices[nFaceIndex * 3 + 1] = 1 + i;
		pIndices[nFaceIndex * 3 + 2] = 1 + ((i + 1) % is);

		nFaceIndex ++;
	}

	// Then middle part;
	for(iy=0; iy<m_nSegmentCount-1; iy++)
	{
		for(ix=0; ix<is; ix++)
		{
			int index0 = 1 + iy * is + ix;
			int index1 = 1 + iy * is + ((ix + 1) % is);
			int index2 = index0 + is;
			int index3 = index1 + is;

			pIndices[nFaceIndex * 3] = index0;
			pIndices[nFaceIndex * 3 + 1] = index2;
			pIndices[nFaceIndex * 3 + 2] = index3;

			pIndices[nFaceIndex * 3 + 3] = index0;
			pIndices[nFaceIndex * 3 + 4] = index3;
			pIndices[nFaceIndex * 3 + 5] = index1;																

			nFaceIndex += 2;
		}
	}
	m_pStreamCap->UnlockIndexBuffer();

	return true;
}