/*
 * FILE: A3DStream.h
 *
 * DESCRIPTION: Class that standing for the vertex stream in A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSTREAM_H_
#define _A3DSTREAM_H_

#include "A3DPlatform.h"
#include "A3DObject.h"
#include "A3DDevice.h"
#include "A3DTypes.h"
#include "A3DVertex.h"
#include "AList.h"

typedef struct _STREAM_LOCK_INFO
{
	DWORD			dwOffset;
	DWORD			dwSize;
	LPBYTE			pBuffer;
}STREAM_LOCK_INFO, * PSTREAM_LOCK_INFO;

class A3DStream
{
private:
	bool						m_bHWIStream;
	IDirect3DVertexBuffer8 *	m_pVertexBuffer;
	IDirect3DIndexBuffer8 *		m_pIndexBuffer;

	bool						m_bReferencePtr;
	BYTE *						m_pVerts;
	WORD *						m_pIndices;

	STREAM_LOCK_INFO			m_VertLockInfo;
	STREAM_LOCK_INFO			m_IndexLockInfo;

	A3DDevice *					m_pA3DDevice;
	
	int							m_nVertexType;
	int							m_nVertexSize;
	DWORD						m_dwFVFFlags;

	int							m_nVertCount;
	int							m_nIndexCount;
	int							m_nShowVertCount;
	int							m_nShowIndexCount;
	int							m_nStreamID;

	bool						m_bUseDynamicBuffer;

	ALISTELEMENT				*m_pStoredElement;
protected:
public:
	A3DStream();
	~A3DStream();

	bool InitAsHWI(A3DDevice * pDevice, int nVertexType, int nVertCount, int nIndexCount, int nStreamID, bool bUseDynamicBuffer=true, bool bCanReferencePtr=false);
	bool Init(A3DDevice * pDevice, int nVertexType, int nVertCount, int nIndexCount, int nStreamID, bool bUseDynamicBuffer=true, bool bCanReferencePtr=false);
	bool Release();

	bool LockVertexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD Flags);
	bool LockIndexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD Flags);
	bool UnlockVertexBuffer();
	bool UnlockIndexBuffer();

	bool SetVerts(BYTE * pVerts, int nVertCount);
	bool SetIndices(WORD * pIndices, int nIndexCount);

	bool Appear();

	bool ReleaseResource();
	bool RebuildResource();

	inline int GetVertCount() { return m_nVertCount; }
	inline int GetIndexCount() { return m_nIndexCount; }
	inline int GetShowVertCount() { return m_nShowVertCount; }
	inline int GetShowIndexCount() { return m_nShowIndexCount; }
};

typedef A3DStream * PA3DStream;
#endif