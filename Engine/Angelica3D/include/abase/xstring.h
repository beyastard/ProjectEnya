#ifndef _XSTRING_H_
#define _XSTRING_H_
#include "APlatform.h"
class xstring;
class xstring_lib
{
public:
	static int	FastCompare(const xstring& str1, const xstring& str2);
	static int	Compare(PCSTR pszStr1, PCSTR pszStr2, int nLength);
	static int	Compare(PCSTR pszStr1, PCSTR pszStr2);
	static int	FindString(PCSTR pszStr, PCSTR pszSubStr);
	static int	StringLength(PCSTR pszStr);
};

class xstring
{
public:
	class AWStringData
	{
	protected:
		friend class xstring;
		static int	m_nNilData[5];
	protected:
		DWORD	m_dwHashKey;
		int		m_nRefCount;
		int		m_nAllocLength;	// does not include '\0', but the true alloc size includes '\0'.
		int		m_nLength;		// does not include '\0'.

		int		IncRef();
		int		DecRef();
		PSTR	GetStr();
		void	Release();

		static	AWStringData* GetNilData();
		static	AWStringData* Alloc(int nLength);
		static	AWStringData* Realloc(AWStringData* pData, int nNewLength);
		static	void Free(AWStringData* pData);
	};

protected:
	PSTR	m_pszStr;

	void Attach(AWStringData* pData);
	void RefAttach(AWStringData* pData);
	AWStringData*	GetData() const;

	static void	Concatenate(xstring& strDst, PCSTR pszSrc1, int nLength1, PCSTR pszSrc2, int nLength2);
	static xstring	Concatenate(PCSTR pszSrc1, int nLength1, PCSTR pszSrc2, int nLength2);

public:
	xstring();
	xstring(const xstring& strSrc);
	xstring(const xstring& strSrc, int nLength);
	xstring(PCSTR pszSrc);
	xstring(PCSTR pszSrc, int nLength);
	~xstring();

	DWORD	BuildHash		() const;
	DWORD	GetHash			() const;
	void	Empty			();
	PSTR	GetBuffer		(int nMinLength);
	char	GetAt			(int nIndex) const;
	void	SetAt			(int nIndex, char ch);
	int		GetLength		() const;
	void	ReleaseBuffer	(int nNewLength = -1);
	void	SetString		(PCSTR pszSrc);
	void	SetString		(PCSTR pszSrc, int nLength);
	void	SetString		(const xstring& strSrc);
	void	SetString		(const xstring& strSrc, int nLength);
	int		FindString		(const xstring& strSubStr) const;
	int		FindString		(PCSTR pszSubStr) const;
	int		FindChar		(char ch, int nStart = 0) const;
	void	Insert			(int nIndex, char ch);
	void	Remove			(int nIndex);
	void	Remove			(int nIndex, int nLength);
	void	Append			(PCSTR pszSrc, int nLength);
	void	Append			(PCSTR pszSrc);
	void	Append			(const xstring& strSrc);
	int		Save			(BYTE* pBuffer, int nBufLength);
	int		Load			(const BYTE* pBuffer, int nBufLength);

	void	operator = (const xstring& strSrc);
	void	operator = (PCSTR pszSrc);
	void	operator += (const xstring& strSrc);
	void	operator += (PCSTR pszSrc);
	void	operator += (CHAR pCH);

	operator	PCSTR () const;

	friend xstring operator + (const xstring& strSrc, int n)
	{
		xstring strRet(strSrc);
		int nOldLength	= strRet.GetLength();
		int nNewLength	= sprintf(
			strRet.GetBuffer(nOldLength+16) + nOldLength, "%d", n);
		strRet.ReleaseBuffer(nNewLength + nOldLength);
		return strRet;
	}

	friend xstring operator + (const xstring& strSrc1, const xstring& strSrc2)
	{
		return Concatenate(
			(PCSTR)strSrc1, strSrc1.GetLength(),
			(PCSTR)strSrc2, strSrc2.GetLength());
	}

	friend xstring operator + (const xstring& strSrc1, PCSTR pszSrc2)
	{
		return Concatenate(
			(PCSTR)strSrc1, strSrc1.GetLength(),
			(PCSTR)pszSrc2, xstring_lib::StringLength(pszSrc2));
	}

	friend xstring operator + (PCSTR pszSrc1, const xstring& strSrc2)
	{
		return Concatenate(
			(PCSTR)pszSrc1, xstring_lib::StringLength(pszSrc1),
			(PCSTR)strSrc2, strSrc2.GetLength());
	}

	friend bool operator == (const xstring& strSrc1, const xstring& strSrc2)
	{
		int	nLength1 = strSrc1.GetLength();
		int nLength2 = strSrc2.GetLength();
		if(nLength1 != nLength2)
		{
			return false;
		}
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)strSrc2, nLength1) == 0;
	}

	friend bool operator == (const xstring& strSrc1, PCSTR pszSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)pszSrc2) == 0;
	}

	friend bool operator == (PCSTR pszSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)pszSrc1, (PCSTR)strSrc2) == 0;
	}

	friend bool operator != (const xstring& strSrc1, const xstring& strSrc2)
	{
		int	nLength1 = strSrc1.GetLength();
		int nLength2 = strSrc2.GetLength();
		if(nLength1 != nLength2)
		{
			return true;
		}
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)strSrc2, nLength1) != 0;
	}

	friend bool operator != (const xstring& strSrc1, PCSTR pszSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)pszSrc2) != 0;
	}

	friend bool operator != (PCSTR pszSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)pszSrc1, (PCSTR)strSrc2) != 0;
	}

	friend bool operator <= (const xstring& strSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)strSrc2) <= 0;
	}

	friend bool operator <= (const xstring& strSrc1, PCSTR pszSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)pszSrc2) <= 0;
	}

	friend bool operator <= (PCSTR pszSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)pszSrc1, (PCSTR)strSrc2) <= 0;
	}

	friend bool operator >= (const xstring& strSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)strSrc2) >= 0;
	}

	friend bool operator >= (const xstring& strSrc1, PCSTR pszSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)pszSrc2) >= 0;
	}

	friend bool operator >= (PCSTR pszSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)pszSrc1, (PCSTR)strSrc2) >= 0;
	}

	friend bool operator < (const xstring& strSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)strSrc2) < 0;
	}

	friend bool operator < (const xstring& strSrc1, PCSTR pszSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)pszSrc2) < 0;
	}

	friend bool operator < (PCSTR pszSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)pszSrc1, (PCSTR)strSrc2) < 0;
	}

	friend bool operator > (const xstring& strSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)strSrc2) > 0;
	}

	friend bool operator > (const xstring& strSrc1, PCSTR pszSrc2)
	{
		return xstring_lib::Compare((PCSTR)strSrc1, (PCSTR)pszSrc2) > 0;
	}

	friend bool operator > (PCSTR pszSrc1, const xstring& strSrc2)
	{
		return xstring_lib::Compare((PCSTR)pszSrc1, (PCSTR)strSrc2) > 0;
	}
public:
  //support stl interface
  	typedef char* iterator;
	typedef const char* const_iterator;
	char operator[](int i)
	{
		return GetAt(i);
	}
	const char operator[](int i)const
	{
		return GetAt(i);
	}
	bool empty() const
	{
		return GetLength() == 0;
	}
	const char* c_str()const
	{
		return operator PCSTR();
	}
	iterator begin()
	{
		return GetBuffer(0);
	}
	const_iterator begin() const
	{
		return operator PCSTR();
	}
	iterator end()
	{
		return GetBuffer(0) + GetLength();
	}
	const_iterator end() const
	{
		return PCSTR() + GetLength();
	}
	int length()const
	{
		return GetLength();
	}

};

class xstring_BlockManager
{
public:
	
	class AWBlock
	{
	public:
		AWBlock*	m_pNext;
	};

	class AWBlockGroup
	{
	public:
		AWBlockGroup*	m_pNext;
		AWBlock*	GetData()
		{
			return (AWBlock*)(this+1);
		}
	};

protected:

	friend class xstring_Allocator;

	int				m_nBlockSize;
	int				m_nGroupSize;
	AWBlock*		m_pBlockFree;
	AWBlockGroup*	m_pGroupHead;

public:
	xstring_BlockManager(int nBlockSize, int nGroupSize);
	~xstring_BlockManager();

	PVOID	Alloc();
	VOID	Free(PVOID pBlock);
};

class xstring_Allocator
{
protected:
	static xstring_BlockManager	m_bm64;
	static xstring_BlockManager	m_bm128;
	static xstring_BlockManager	m_bm192;
	static xstring_BlockManager	m_bm256;
	static xstring_BlockManager	m_bm320;
	static xstring_BlockManager	m_bm384;
	static xstring_BlockManager	m_bm448;
	static xstring_BlockManager	m_bm512;

	static xstring_BlockManager*	m_bms[8];
public:
	static PVOID	Alloc(int nSize, int& nTrueSize);
	static PVOID	Realloc(PVOID pMem, int nSize, int& nTrueSize);
	static VOID		Free(PVOID pMem);

public:


};

// class xstring_lib

__inline int xstring_lib::Compare(PCSTR pszStr1, PCSTR pszStr2, int nLength)
{
	return memcmp(pszStr1, pszStr2, nLength);
}
__inline int xstring_lib::Compare(PCSTR pszStr1, PCSTR pszStr2)
{
	return strcmp(pszStr1, pszStr2);
}
__inline int xstring_lib::FastCompare(const xstring& str1, const xstring& str2)
{
	int nLength1	= str1.GetLength();
	int nLength2	= str2.GetLength();
	int	nCmpResult	= nLength1 - nLength2;
	if(nCmpResult == 0)
	{
		nCmpResult	= xstring_lib::Compare(str1, str2);//, nLength1);
	}
	return nCmpResult;
}

// class xstring::AWStringData

__inline int xstring::AWStringData::IncRef()
{
	return (int) InterlockedIncrement((PLONG)&m_nRefCount);
}
__inline int xstring::AWStringData::DecRef()
{
	return (int) InterlockedDecrement((PLONG)&m_nRefCount);
}
__inline PSTR xstring::AWStringData::GetStr()
{
	return (PSTR)(this+1);
}
__inline void xstring::AWStringData::Release()
{
	if(DecRef() <= 0)
	{
		Free(this);
	}
}
__inline xstring::AWStringData* xstring::AWStringData::GetNilData()
{
	return (AWStringData*)m_nNilData;
}

// class xstring

__inline xstring::xstring()
{
	RefAttach(AWStringData::GetNilData());
}
__inline xstring::xstring(const xstring& strSrc)
{
	RefAttach(strSrc.GetData());
}
__inline xstring::~xstring()
{
	GetData()->Release();
}
__inline xstring::AWStringData* xstring::GetData() const
{
	return (AWStringData*)m_pszStr - 1;
}
__inline DWORD xstring::GetHash() const
{
	DWORD	dwHash	= GetData()->m_dwHashKey;
	if(dwHash == 0)
	{
		dwHash	= BuildHash();
	}
	return dwHash;
}
__inline char xstring::GetAt(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < GetLength());
	return m_pszStr[nIndex];
}
__inline void xstring::SetAt(int nIndex, char ch)
{
	assert(nIndex >= 0 && nIndex < GetLength());
	GetBuffer(0);
	m_pszStr[nIndex]	= ch;
}
__inline int xstring::GetLength() const
{
	return GetData()->m_nLength;
}
__inline void xstring::Attach(AWStringData* pData)
{
	m_pszStr	= pData->GetStr();
}
__inline void xstring::RefAttach(AWStringData* pData)
{
	pData->IncRef();
	m_pszStr	= pData->GetStr();
}
__inline int xstring::FindString(const xstring& strSubStr) const
{
	return xstring_lib::FindString(m_pszStr, strSubStr);
}
__inline int xstring::FindString(PCSTR pszSubStr) const
{
	return xstring_lib::FindString(m_pszStr, pszSubStr);
}
__inline int xstring::FindChar(char ch, int nStart) const
{
	if(nStart < 0 || nStart >= GetLength())
	{
		return -1;
	}
	return (int)(strchr(m_pszStr+nStart, ch) - m_pszStr);
}
__inline xstring::operator PCSTR () const
{
	return m_pszStr;
}
__inline void xstring::Append(PCSTR pszSrc)
{
	Append(pszSrc, xstring_lib::StringLength(pszSrc));
}
__inline void xstring::Append(const xstring& strSrc)
{
	Append((PCSTR)strSrc, strSrc.GetLength());
}
__inline void xstring::operator = (const xstring& strSrc)
{
	SetString(strSrc);
}
__inline void xstring::operator = (PCSTR pszSrc)
{
	SetString(pszSrc);
}
__inline void xstring::operator += (const xstring& strSrc)
{
	Append(strSrc);
}
__inline void xstring::operator += (PCSTR pszSrc)
{
	Append(pszSrc);
}
__inline void xstring::operator += (CHAR ch)
{
	Insert(GetLength(),ch);
}
#endif//_XSTRING_H_

