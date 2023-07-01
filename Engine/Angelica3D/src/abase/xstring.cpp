#include "xstring.h"

int xstring_lib::StringLength(PCSTR pszStr)
{
	if(pszStr == NULL)
	{
		return 0;
	}
	else
	{
		return strlen(pszStr);
	}
}

int	xstring_lib::FindString(PCSTR pszStr, PCSTR pszSubStr)
{
	const char* pResult	= strstr(pszStr, pszSubStr);
	return (int)pResult - (int)pszStr;
}

// class xstring::AWStringData

int	xstring::AWStringData::m_nNilData[5] = { 0, 2, 0, 0, 0 };

xstring::AWStringData* xstring::AWStringData::Alloc(int nLength)
{
//	int nAllocLength	= ((nLength+sizeof(AWStringData)+1+15) & (0xFFFFFFFF << 4)) - 1;
//	AWStringData* pData	= (AWStringData*) malloc(nAllocLength+1);
	int nAlloc			= nLength+sizeof(AWStringData)+1;
	AWStringData* pData	= (AWStringData*)
		xstring_Allocator::Alloc(nAlloc, nAlloc);

	pData->m_dwHashKey		= 0;
	pData->m_nRefCount		= 1;
	pData->m_nAllocLength	= nAlloc - sizeof(AWStringData) - 1;
	pData->m_nLength		= nLength;
	pData->GetStr()[nLength]= 0;

	return pData;
}

xstring::AWStringData* xstring::AWStringData::Realloc(
	AWStringData* pData, int nNewLength)
{
//	int nAllocLength			= ((nNewLength+sizeof(AWStringData)+1+15) & (0xFFFFFFFF << 4)) - 1;
//	AWStringData* pNewData		= (AWStringData*) malloc(nAllocLength+1);
	int nAlloc				= nNewLength+sizeof(AWStringData)+1;
	AWStringData* pNewData	=
		(AWStringData*) xstring_Allocator::Alloc(nAlloc, nAlloc);

	pNewData->m_dwHashKey		= pData->m_dwHashKey;
	pNewData->m_nRefCount		= 1;
	pNewData->m_nAllocLength	= nAlloc - sizeof(AWStringData) - 1;;

	int nMinLength	= (nNewLength < pData->m_nLength) ? nNewLength : pData->m_nLength;

	memcpy(pNewData->GetStr(), pData->GetStr(), nMinLength);

	pNewData->GetStr()[nMinLength]	= 0;
	pNewData->m_nLength				= nMinLength;

	xstring_Allocator::Free(pData);

	return pNewData;
}

void xstring::AWStringData::Free(AWStringData* pData)
{
	xstring_Allocator::Free(pData);
}

// class xstring

xstring::xstring(PCSTR pszSrc)
{
	int nLength	= xstring_lib::StringLength(pszSrc);
	if(nLength > 0)
	{
		AWStringData* pData = AWStringData::Alloc(nLength);
		memcpy(pData->GetStr(), pszSrc, nLength);
		pData->GetStr()[nLength]	= 0;
		Attach(pData);
	}
	else
	{
		RefAttach(AWStringData::GetNilData());
	}
}

xstring::xstring(const xstring& strSrc, int nLength)
{
	if(nLength >= strSrc.GetLength())
	{
		RefAttach(strSrc.GetData());
	}
	else if(nLength <= 0)
	{
		RefAttach(AWStringData::GetNilData());
	}
	else
	{
		AWStringData* pData	= AWStringData::Alloc(nLength);
		Attach(pData);
		memcpy(pData->GetStr(), (PCSTR)strSrc, nLength);
		pData->GetStr()[nLength]	= '\0';
	}
}

xstring::xstring(PCSTR pszSrc, int nLength)
{
	if(nLength > 0)
	{
		assert(pszSrc != NULL);

		AWStringData* pData = AWStringData::Alloc(nLength);
		memcpy(pData->GetStr(), pszSrc, nLength);
		pData->GetStr()[nLength]	= 0;
		Attach(pData);
	}
	else
	{
		RefAttach(AWStringData::GetNilData());
	}
}

void xstring::Concatenate(xstring& strDst,
	PCSTR pszSrc1, int nLength1, PCSTR pszSrc2, int nLength2)
{
	int nNewLength	= nLength1 + nLength2;
	PSTR pszBuf	= strDst.GetBuffer(nNewLength);
	memcpy(pszBuf, pszSrc1, nLength1);
	memcpy(pszBuf+nLength1, pszSrc2, nLength2);
	strDst.ReleaseBuffer(nNewLength);
}

xstring xstring::Concatenate(
	PCSTR pszSrc1, int nLength1, PCSTR pszSrc2, int nLength2)
{
	xstring strResult;
	int nNewLength	= nLength1 + nLength2;
	PSTR pszBuf	= strResult.GetBuffer(nNewLength);
	memcpy(pszBuf, pszSrc1, nLength1);
	memcpy(pszBuf+nLength1, pszSrc2, nLength2);
	strResult.ReleaseBuffer(nNewLength);

	return strResult;
}

void xstring::Empty()
{
	GetData()->Release();
	RefAttach(AWStringData::GetNilData());
}

PSTR xstring::GetBuffer(int nMinLength)
{
	assert(nMinLength >= 0);

	AWStringData* pData	= GetData();

	if(pData->m_nRefCount <= 1)
	{
		if(pData->m_nAllocLength < nMinLength)
		{
			pData	= AWStringData::Realloc(pData, nMinLength);
			pData->m_dwHashKey	= 0;
			Attach(pData);
		}
	}
	else
	{
		int	nMaxAlloc	= (nMinLength > pData->m_nAllocLength)
			? nMinLength : pData->m_nAllocLength;
		AWStringData* pNewData	= AWStringData::Alloc(nMaxAlloc);
		pNewData->m_nLength		= pData->m_nLength;
		pNewData->m_dwHashKey	= 0;
		pNewData->GetStr()[pData->m_nLength]	= 0;
		memcpy(pNewData->GetStr(), pData->GetStr(), pData->m_nLength);
		Attach(pNewData);
		pData->Release();
	}

	return m_pszStr;
}

void xstring::ReleaseBuffer(int nNewLength)
{
	assert(nNewLength <= GetData()->m_nAllocLength);

	if(nNewLength < 0)
	{
		nNewLength	= strlen(m_pszStr);
	}

	AWStringData*	pData		= GetData();
	pData->m_dwHashKey			= 0;
	pData->m_nLength			= nNewLength;
	pData->GetStr()[nNewLength]	= '\0';
}

void xstring::SetString(PCSTR pszSrc)
{
	SetString(pszSrc, xstring_lib::StringLength(pszSrc));
}

void xstring::SetString(PCSTR pszSrc, int nLength)
{
	AWStringData* pData	= GetData();
	
	if(nLength <= 0)
	{
		Empty();
	}
	else if(pData->m_nRefCount > 1 || pData->m_nAllocLength < nLength)
	{
		pData->Release();
		AWStringData* pNewData	= AWStringData::Alloc(nLength);
		memcpy(pNewData->GetStr(), pszSrc, nLength);
		pNewData->GetStr()[nLength]	= 0;
		Attach(pNewData);
	}
	else
	{
		memcpy(pData->GetStr(), pszSrc, nLength);
		pData->GetStr()[nLength]	= 0;
		pData->m_nLength			= nLength;
		pData->m_dwHashKey			= 0;
	}
}

void xstring::SetString(const xstring& strSrc)
{
	AWStringData* pData	= GetData();
	AWStringData* pSrc	= strSrc.GetData();
	if(pData != pSrc)
	{
		pData->Release();
		RefAttach(pSrc);
	}
}

void xstring::SetString(const xstring& strSrc, int nLength)
{
	if(nLength == strSrc.GetLength())
	{
		SetString(strSrc);
	}
	else
	{
		SetString((PCSTR)strSrc, nLength);
	}
}

void xstring::Insert(int nIndex, char ch)
{
	assert(nIndex >= 0 && nIndex <= GetLength());

	int nOldLength	= GetLength();
	int nNewLength	= nOldLength + 1;
	PSTR pszBuf	= GetBuffer(nNewLength);
	memmove(pszBuf + nIndex + 1, pszBuf + nIndex, nOldLength - nIndex);
	ReleaseBuffer(nNewLength);
}

void xstring::Remove(int nIndex)
{
	assert(nIndex >= 0 && nIndex < GetLength());

	int nNewLength	= GetLength() - 1;
	PSTR pszBuf	= GetBuffer(nNewLength);
	memmove(pszBuf + nIndex, pszBuf + nIndex + 1, nNewLength - nIndex);
	ReleaseBuffer(nNewLength);
}

void xstring::Remove(int nIndex, int nLength)
{
	assert(nIndex >= 0 && nIndex < GetLength());

	if(nLength == 0)
	{
		return;
	}
	int nRemove;
	int nOld	= GetLength();
	if(nLength < 0 || nLength+nIndex > nOld)
	{
		nRemove	= nOld - nIndex;
	}
	else
	{
		nRemove	= nLength;
	}

	int nNew	= nLength - nRemove;
	PSTR pszBuf	= GetBuffer(nNew) + nIndex;
	memmove(pszBuf, pszBuf+nRemove, nNew-nIndex);
	ReleaseBuffer(nNew);
}

void xstring::Append(PCSTR pszSrc, int nLength)
{
	int nOldLength	= GetLength();
	int nNewLength	= nOldLength + nLength;
	PSTR pszBuf = GetBuffer(nNewLength);
	memcpy(pszBuf+nOldLength, pszSrc, nLength);
	ReleaseBuffer(nNewLength);
}

int xstring::Save(BYTE* pBuffer, int nBufLength)
{
	int nSaved		= 0;
	int nStrLength	= GetLength();

	if(pBuffer == NULL)
	{
		if(nStrLength < 0xFF)
		{
			return nStrLength + 1;
		}
		else if(nStrLength < 0xFFFF)
		{
			return nStrLength + 3;
		}
		else
		{
			return nStrLength + 7;
		}
	}

	if(nStrLength < 0xFF)
	{
		if(nBufLength < 1)
		{
			return -1;
		}
		if(nStrLength > nBufLength - 1)
		{
			nStrLength	= nBufLength - 1;
		}
		pBuffer[nSaved]	= (BYTE)nStrLength;
		nSaved ++;
	}
	else if(nStrLength < 0xFFFF)
	{
		if(nBufLength < 3)
		{
			return -1;
		}
		if(nStrLength > nBufLength - 3)
		{
			nStrLength	= nBufLength - 3;
		}
		pBuffer[nSaved]				= (BYTE)0xFF;
		nSaved ++;
		*((WORD*)(pBuffer+nSaved))	= (WORD)nStrLength;
		nSaved += 2;
	}
	else
	{
		if(nBufLength < 7)
		{
			return -1;
		}
		if(nStrLength > nBufLength - 7)
		{
			nStrLength	= nBufLength - 7;
		}
		pBuffer[nSaved]				= (BYTE)0xFF;
		nSaved ++;
		*((WORD*)(pBuffer+nSaved))	= (WORD)0xFFFF;
		nSaved += 2;
		*((int*)(pBuffer+nSaved))	= (int)nStrLength;
	}
	memcpy(pBuffer+nSaved, m_pszStr, nStrLength);
	nSaved += nStrLength;

	return nSaved;
}

int xstring::Load(const BYTE* pBuffer, int nBufLength)
{
	if(nBufLength < 1)
	{
		return -1;
	}
	int nLoaded		= 0;
	int nStrLength;
	nStrLength	= pBuffer[nLoaded];
	nLoaded		++;
	if(nStrLength == 0xFF)
	{		
		if(nBufLength < 3)
		{
			return -1;
		}
		nStrLength	= *((WORD*)(pBuffer+nLoaded));
		nLoaded		+= 2;
		if(nStrLength == 0xFFFF)
		{
			if(nBufLength < 7)
			{
				return -1;
			}
			nStrLength	= *((int*)(pBuffer+nLoaded));
			nLoaded		+= 4;
		}
	}
	if(nStrLength < 0)
	{
		return -1;
	}
	if(nStrLength > nBufLength - nLoaded)
	{
		nStrLength = nBufLength - nLoaded;
	}
	memcpy(GetBuffer(nStrLength), pBuffer+nLoaded, nStrLength);
	ReleaseBuffer(nStrLength);
	nLoaded	+= nStrLength;

	return nLoaded;
}

//__declspec(naked) 
DWORD xstring::BuildHash() const
{
/*	__asm
	{
		mov		edx,	[ecx]	// m_pszStr
		xor		eax,	eax
		push	ebx

		pop		ebx
		ret
	}*/
	DWORD	dwHash	= 0;
	PCSTR	pszStr	= m_pszStr;
	for(;*pszStr; pszStr++)
	{
		dwHash = (dwHash << 5) - dwHash + *(BYTE *)pszStr;
	}

	if(dwHash == 0)
	{
		dwHash ++;
	}

	GetData()->m_dwHashKey	= dwHash;
	return dwHash;
}

// class xstring_BlockManager

xstring_BlockManager::xstring_BlockManager(int nBlockSize, int nGroupSize):
m_nBlockSize	(nBlockSize),
m_nGroupSize	(nGroupSize),
m_pBlockFree	(NULL),
m_pGroupHead	(NULL)
{
	assert(nBlockSize >= 4);
}

xstring_BlockManager::~xstring_BlockManager()
{
	AWBlockGroup*	pHead	= m_pGroupHead;
	while(pHead != NULL)
	{
		AWBlockGroup*	pTemp	= pHead;
		pHead	= pHead->m_pNext;
		free(pTemp);
	}
}

PVOID xstring_BlockManager::Alloc()
{
	if(m_pBlockFree == NULL)
	{
		AWBlockGroup*	pGroup	= (AWBlockGroup*)
			malloc(sizeof(AWBlockGroup) + m_nBlockSize*m_nGroupSize);

		m_pBlockFree		= pGroup->GetData();
		int			nStart	= m_nGroupSize-1;
		AWBlock*	pNext	= NULL;
		AWBlock*	pBlock	= (AWBlock*)((BYTE*)m_pBlockFree + m_nBlockSize*nStart);

		for(int i = nStart; i >= 0; i --)
		{
			pBlock->m_pNext	= pNext;
			pNext			= pBlock;
			pBlock			= (AWBlock*)((BYTE*)pBlock - m_nBlockSize);
		}

		pGroup->m_pNext		= m_pGroupHead;
		m_pGroupHead		= pGroup;
	}
	PVOID	pRet	= m_pBlockFree;
	m_pBlockFree	= m_pBlockFree->m_pNext;
	return	pRet;
}

VOID xstring_BlockManager::Free(PVOID pBlock)
{
	((AWBlock*)pBlock)->m_pNext	= m_pBlockFree;
	m_pBlockFree	= (AWBlock*)pBlock;
}

#pragma warning(disable : 4073)
#pragma init_seg(lib)

xstring_BlockManager	xstring_Allocator::m_bm64 (sizeof(xstring_BlockManager*) +  64, 16);
xstring_BlockManager	xstring_Allocator::m_bm128(sizeof(xstring_BlockManager*) + 128, 16);
xstring_BlockManager	xstring_Allocator::m_bm192(sizeof(xstring_BlockManager*) + 192, 16);
xstring_BlockManager	xstring_Allocator::m_bm256(sizeof(xstring_BlockManager*) + 256, 16);
xstring_BlockManager	xstring_Allocator::m_bm320(sizeof(xstring_BlockManager*) + 320,  8);
xstring_BlockManager	xstring_Allocator::m_bm384(sizeof(xstring_BlockManager*) + 384,  8);
xstring_BlockManager	xstring_Allocator::m_bm448(sizeof(xstring_BlockManager*) + 448,  8);
xstring_BlockManager	xstring_Allocator::m_bm512(sizeof(xstring_BlockManager*) + 512,  8);
xstring_BlockManager*	xstring_Allocator::m_bms[8]	=
{
	&m_bm64,
	&m_bm128,
	&m_bm192,
	&m_bm256,
	&m_bm320,
	&m_bm384,
	&m_bm448,
	&m_bm512,
};

PVOID xstring_Allocator::Alloc(int nSize, int& nTrueSize)
{
#ifdef	T_DEBUG
	nTrueSize	= nSize;
	return new BYTE[nSize];
#endif

	int nbmIndex;
	if(nSize <= 0)
	{
		nbmIndex	= 0;
	}
	else
	{
		nbmIndex	= (nSize-1) >> 6;
	}

	PVOID	pRet;

	if(nbmIndex >= 8)
	{
		xstring_BlockManager** pm	= (xstring_BlockManager**)new BYTE[nSize+sizeof(xstring_BlockManager*)];
		*pm			= NULL;
		pRet		= pm+1;
		nTrueSize	= nSize;
	}
	else
	{
		//g_csAlloc.Enter();

		xstring_BlockManager** pm	= (xstring_BlockManager**) m_bms[nbmIndex]->Alloc();

		//g_csAlloc.Leave();
		
		*pm			= m_bms[nbmIndex];
		pRet		= pm+1;
		nTrueSize	= (nbmIndex+1) << 6;
	}


	return pRet;
}

VOID xstring_Allocator::Free(PVOID pMem)
{
#ifdef	T_DEBUG
	delete pMem;
	return;
#endif

	xstring_BlockManager** pm	= (xstring_BlockManager**) pMem;
	pm --;
	if(*pm != NULL)
	{
		//g_csAlloc.Enter();
		(*pm)->Free((PVOID)pm);
		//g_csAlloc.Leave();
	}
	else
	{
		delete (BYTE*)pm;
	}
}


