#include "A3DStream.h"
#include "A3DTypes.h"
#include "A3DErrLog.h"
#include "A3DConfig.h"
#include "A3DEngine.h"

A3DStream::A3DStream()
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_pA3DDevice = NULL;
	m_nVertexSize = 0;
	m_dwFVFFlags = 0;
	m_nVertexType = 0;
	m_nVertCount = 0;
	m_nIndexCount = 0;
	m_nShowVertCount = 0;
	m_nShowIndexCount = 0;
	m_nStreamID = 0;

	m_bUseDynamicBuffer = false;
	m_bReferencePtr = false;

	m_pVerts = NULL;
	m_pIndices = NULL;

	m_bHWIStream = false;
}

A3DStream::~A3DStream()
{
}

bool A3DStream::InitAsHWI(A3DDevice * pDevice, int nVertexType, int nVertCount, int nIndexCount, int nStreamID, bool bUseDynamicBuffer, bool bCanReferencePtr)
{
	m_bHWIStream = true;

	m_pA3DDevice = pDevice;
	m_nVertCount = nVertCount;
	m_nIndexCount = nIndexCount;
	m_nShowVertCount = 0;
	m_nShowIndexCount = 0;
	m_nStreamID = nStreamID;

	m_bUseDynamicBuffer = false;

	switch(nVertexType)
	{
	case A3DVT_VERTEX:
		m_nVertexType = nVertexType;
		m_nVertexSize = sizeof(A3DVERTEX);
		m_dwFVFFlags = A3DFVF_A3DVERTEX;
		break;
	case A3DVT_LVERTEX:
		m_nVertexType = nVertexType;
		m_nVertexSize = sizeof(A3DLVERTEX);
		m_dwFVFFlags = A3DFVF_A3DLVERTEX;
		break;
	case A3DVT_TLVERTEX:
		m_nVertexType = nVertexType;
		m_nVertexSize = sizeof(A3DTLVERTEX);
		m_dwFVFFlags = A3DFVF_A3DTLVERTEX;
		break;
	case A3DVT_IBLVERTEX:
		m_nVertexType = nVertexType;
		m_nVertexSize = sizeof(A3DIBLVERTEX);
		m_dwFVFFlags = A3DFVF_A3DIBLVERTEX;
		break;
	default:
		g_pA3DErrLog->ErrLog("A3DStream::Init(), Unknown vertex type!");
		return false;
	}
	return true;
}

bool A3DStream::Init(A3DDevice * pDevice, int nVertexType, int nVertCount, int nIndexCount, int nStreamID, bool bUseDynamicBuffer, bool bCanReferencePtr)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return InitAsHWI(pDevice, nVertexType, nVertCount, nIndexCount, nStreamID, bUseDynamicBuffer, bCanReferencePtr);

	HRESULT			hval;

	m_pA3DDevice = pDevice;
	if( NULL == m_pA3DDevice ) return true;

	m_nVertCount = nVertCount;
	m_nIndexCount = nIndexCount;
	m_nShowVertCount = 0;
	m_nShowIndexCount = 0;
	m_nStreamID = nStreamID;

	if( g_pA3DConfig->GetFlagUseDynamicStream() )
		m_bUseDynamicBuffer = bUseDynamicBuffer;
	else
		m_bUseDynamicBuffer = false;

	switch(nVertexType)
	{
	case A3DVT_VERTEX:
		m_nVertexType = nVertexType;
		m_nVertexSize = sizeof(A3DVERTEX);
		m_dwFVFFlags = A3DFVF_A3DVERTEX;
		break;
	case A3DVT_LVERTEX:
		m_nVertexType = nVertexType;
		m_nVertexSize = sizeof(A3DLVERTEX);
		m_dwFVFFlags = A3DFVF_A3DLVERTEX;
		break;
	case A3DVT_TLVERTEX:
		m_nVertexType = nVertexType;
		m_nVertexSize = sizeof(A3DTLVERTEX);
		m_dwFVFFlags = A3DFVF_A3DTLVERTEX;
		break;
	case A3DVT_IBLVERTEX:
		m_nVertexType = nVertexType;
		m_nVertexSize = sizeof(A3DIBLVERTEX);
		m_dwFVFFlags = A3DFVF_A3DIBLVERTEX;
		break;
	default:
		g_pA3DErrLog->ErrLog("A3DStream::Init(), Unknown vertex type!");
		return false;
	}

	if( m_bUseDynamicBuffer )
	{
		hval = m_pA3DDevice->GetD3DDevice()->CreateVertexBuffer(nVertCount * m_nVertexSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &m_pVertexBuffer);
		if( D3D_OK != hval ) 
		{
			g_pA3DErrLog->ErrLog("A3DStream::Init(), CreateVertexBuffer Fail, with [%d]	vertex", nVertCount);
			return false;
		}

		if( nIndexCount > 0 )
		{
			hval = m_pA3DDevice->GetD3DDevice()->CreateIndexBuffer(nIndexCount * sizeof(WORD), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer);
			if( D3D_OK != hval )
			{
				g_pA3DErrLog->ErrLog("A3DStream::Init(), CreateIndexBuffer Fail, with [%d] index", nIndexCount);
				return false;
			}
		}

		m_bReferencePtr = bCanReferencePtr;
		if( !m_bReferencePtr )
		{
			m_pVerts = (BYTE *) malloc(nVertCount * m_nVertexSize);
			if( NULL == m_pVerts )
			{
				g_pA3DErrLog->ErrLog("A3DStream::Init(), Not enough memory!");
				return false;
			}
			if( nIndexCount > 0 )
			{
				m_pIndices = (WORD *) malloc(nIndexCount * sizeof(WORD));
				if( NULL == m_pIndices )
				{
					g_pA3DErrLog->ErrLog("A3DStream::Init(), Not enough memory!");
					return false;
				}
			}
		}

		m_pA3DDevice->AddDynamicStreams(this, &m_pStoredElement);
	}
	else
	{
		hval = m_pA3DDevice->GetD3DDevice()->CreateVertexBuffer(nVertCount * m_nVertexSize, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED, &m_pVertexBuffer);
		if( D3D_OK != hval ) return false;

		if( nIndexCount > 0 )
		{
			hval = m_pA3DDevice->GetD3DDevice()->CreateIndexBuffer(nIndexCount * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer);
			if( D3D_OK != hval ) return false;
		}
	}

	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DENGINE_OBJECT_STREAM);
	return true;
}

bool A3DStream::Release()
{
	if( m_pA3DDevice )
		m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DENGINE_OBJECT_STREAM);

	if( m_pVertexBuffer )
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
	if( m_pIndexBuffer )
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}
	
	if( m_bUseDynamicBuffer )
	{
		m_pA3DDevice->RemoveDynamicStreams(m_pStoredElement);

		if( !m_bReferencePtr )
		{
			if( m_pVerts ) free(m_pVerts);
			if( m_pIndices ) free(m_pIndices);
		}
		m_pVerts = NULL;
		m_pIndices = NULL;
	}

	m_pA3DDevice = NULL;
	return true;
}

bool A3DStream::SetVerts(BYTE * pVerts, int nVertCount)
{
	if( m_bHWIStream ) return true;

	HRESULT			hval;

	if( nVertCount > m_nVertCount )
	{
		g_pA3DErrLog->ErrLog("Vert Count Beyond m_VertCount!");
		return false;
	}

	LPBYTE	pVBVerts;
	DWORD	dwFlags = NULL;

	if( m_bUseDynamicBuffer )
		dwFlags = D3DLOCK_DISCARD;

	hval = m_pVertexBuffer->Lock(0, 0, &pVBVerts, dwFlags);
	if( D3D_OK != hval ) return false;

	memcpy(pVBVerts, pVerts, nVertCount * m_nVertexSize);

	hval = m_pVertexBuffer->Unlock();
	if( D3D_OK != hval ) return false;

	if( m_bUseDynamicBuffer )
	{
		if( m_bReferencePtr )
			m_pVerts = pVerts;
		else
			memcpy(m_pVerts, pVerts, nVertCount * m_nVertexSize);
	}

	m_nShowVertCount = nVertCount;
	return true;
}

bool A3DStream::SetIndices(WORD * pIndices, int nIndexCount)
{
	if( m_bHWIStream ) return true;

	HRESULT hval;

	if( !m_pIndexBuffer )
		return false;

	if( nIndexCount > m_nIndexCount )
	{
		g_pA3DErrLog->ErrLog("Index Count Beyond m_nIndexCount!");
		return false;
	}

	DWORD	dwFlags = NULL;

	if( m_bUseDynamicBuffer )
		dwFlags = D3DLOCK_DISCARD;
	
	//Update the d3d index buffer;
	WORD * pVBIndices;
	hval = m_pIndexBuffer->Lock(0, 0, (LPBYTE *)&pVBIndices, dwFlags);
	if( D3D_OK != hval ) return false;

	memcpy(pVBIndices, pIndices, nIndexCount * sizeof(WORD));

	hval = m_pIndexBuffer->Unlock();
	if( D3D_OK != hval ) return false;

	if( m_bUseDynamicBuffer )
	{
		if( m_bReferencePtr )
			m_pIndices = pIndices;
		else
			memcpy(m_pIndices, pIndices, nIndexCount * sizeof(WORD));
	}

	m_nShowIndexCount = nIndexCount;
	return true;
}

bool A3DStream::Appear()
{
	if( m_bHWIStream ) return true;

	if( NULL == m_pA3DDevice )
		return true;

	m_pA3DDevice->GetD3DDevice()->SetStreamSource(m_nStreamID, m_pVertexBuffer, m_nVertexSize);
	m_pA3DDevice->GetD3DDevice()->SetVertexShader(m_dwFVFFlags);
	if( m_pIndexBuffer )
		m_pA3DDevice->GetD3DDevice()->SetIndices(m_pIndexBuffer, 0);

	if( m_bReferencePtr && !m_pVerts )
	{
		int a = 0;
	}
	return true;
}

bool A3DStream::LockVertexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD Flags)
{
	// For a hardware independent stream, we have to let lock fail;
	if( m_bHWIStream ) return false;

	HRESULT hval;
	
	if( !m_bUseDynamicBuffer )
	{
		hval = m_pVertexBuffer->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
		if( D3D_OK != hval )
		{
			g_pA3DErrLog->ErrLog("A3DStream::LockVertexBuffer m_pVertexBuffer->Lock() Fail");
			return false;
		}
	}
	else
	{
		if( m_bReferencePtr )
		{
			g_pA3DErrLog->ErrLog("A3DStream::LockVertexBuffer(), Dynamic Stream use LockVertexBuffer with reference ptr is forbidden!");
			return false;
		}

		m_VertLockInfo.dwOffset = OffsetToLock;
		m_VertLockInfo.dwSize = SizeToLock ? SizeToLock : m_nVertCount * m_nVertexSize - OffsetToLock;
		//m_VertLockInfo.pBuffer = *ppbData;
		*ppbData = m_pVerts + OffsetToLock;
	}

	return true;
}

bool A3DStream::UnlockVertexBuffer()
{
	if( m_bHWIStream ) return true;

	HRESULT hval;

	if( !m_bUseDynamicBuffer )
	{
		hval = m_pVertexBuffer->Unlock();
		if( D3D_OK != hval )
		{
			g_pA3DErrLog->ErrLog("A3DStream::UnlockVertexBuffer m_pVertexBuffer->Unlock() Fail");
			return false;
		}
	}
	else
	{
		if( m_bReferencePtr )
		{
			g_pA3DErrLog->ErrLog("A3DStream::UnlockVertexBuffer(), Dynamic Stream use LockVertexBuffer with reference ptr is forbidden!");
			return false;
		}

		LPBYTE	pVBVerts;
		DWORD	dwFlags = D3DLOCK_DISCARD;

		hval = m_pVertexBuffer->Lock(0, 0, &pVBVerts, dwFlags);
		if( D3D_OK != hval ) return false;

		memcpy(pVBVerts, m_pVerts, m_nVertCount * m_nVertexSize);

		hval = m_pVertexBuffer->Unlock();
		if( D3D_OK != hval ) return false;
	}

	return true;
}

bool A3DStream::LockIndexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD Flags)
{
	if( m_bHWIStream ) return true;

	HRESULT hval;

	if( !m_pIndexBuffer )
		return false;

	if( !m_bUseDynamicBuffer )
	{
		hval = m_pIndexBuffer->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
		if( D3D_OK != hval )
		{				 
			g_pA3DErrLog->ErrLog("A3DStream::LockIndexBuffer m_pIndexBuffer->Lock() Fail");
			return false;
		}
	}
	else
	{
		if( m_bReferencePtr )
		{
			g_pA3DErrLog->ErrLog("A3DStream::LockIndexBuffer(), Dyanmic Stream use LockIndexBuffer with a reference ptr is forbidden!");
			return false;
		}
		m_IndexLockInfo.dwOffset = OffsetToLock;
		m_IndexLockInfo.dwSize   = SizeToLock ? SizeToLock : m_nIndexCount * sizeof(WORD) - OffsetToLock;
		//m_IndexLockInfo.pBuffer  = *ppbData;
		*ppbData = (LPBYTE)m_pIndices + OffsetToLock;
	}

	return true;
}

bool A3DStream::UnlockIndexBuffer()
{
	if( m_bHWIStream ) return true;

	HRESULT hval;

	if( !m_pIndexBuffer )
		return false;

	if( !m_bUseDynamicBuffer )
	{
		hval = m_pIndexBuffer->Unlock();
		if( D3D_OK != hval )
		{
			g_pA3DErrLog->ErrLog("A3DStream::UnlockIndexBuffer m_pIndexBuffer->Unlock() Fail");
			return false;
		}
	}
	else
	{
		if( m_bReferencePtr )
		{
			g_pA3DErrLog->ErrLog("A3DStream::UnlockIndexBuffer(), Dyanmic Stream use LockIndexBuffer with a reference ptr is forbidden!");
			return false;
		}

		WORD * pVBIndices;
		DWORD  dwFlags = D3DLOCK_DISCARD;

		hval = m_pIndexBuffer->Lock(0, 0, (LPBYTE *)&pVBIndices, dwFlags);
		if( D3D_OK != hval ) return false;

		memcpy(pVBIndices, m_pIndices, m_nIndexCount * sizeof(WORD));

		hval = m_pIndexBuffer->Unlock();
		if( D3D_OK != hval ) return false;
	}

	return true;
}

bool A3DStream::ReleaseResource()
{
	if( m_bHWIStream ) return true;

	if( m_bUseDynamicBuffer )
	{
		if( m_pVertexBuffer )
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = NULL;
		}
		if( m_pIndexBuffer )
		{
			m_pIndexBuffer->Release();
			m_pIndexBuffer = NULL;
		}
	}
	return true;
}

bool A3DStream::RebuildResource()
{
	if( m_bHWIStream ) return true;

	HRESULT	hval;

	if( m_bUseDynamicBuffer )
	{
		hval = m_pA3DDevice->GetD3DDevice()->CreateVertexBuffer(m_nVertCount * m_nVertexSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, NULL, D3DPOOL_DEFAULT, &m_pVertexBuffer);
		if( D3D_OK != hval ) return false;

		if( m_pVerts )
			SetVerts(m_pVerts, m_nVertCount);

		if( m_nIndexCount > 0 )
		{
			hval = m_pA3DDevice->GetD3DDevice()->CreateIndexBuffer(m_nIndexCount * sizeof(WORD), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer);
			if( D3D_OK != hval ) return false;

			if( m_pIndices )
				SetIndices(m_pIndices, m_nIndexCount);
		}
	}
	return true;
}