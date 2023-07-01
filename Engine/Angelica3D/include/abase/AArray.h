/*
 * FILE: AArrayT.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: JiangLi, 2002-8-29
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2002 Epie-Game, All Rights Reserved.
 */

#ifndef	__AArrayT_H__
#define	__AArrayT_H__

#include "APlatform.h"
#include "ADebug.h"

// 只支持简单类型或者简单结构。不能有“功能重要的”析构和构造函数。
template<typename TYPE> class AArrayT
{
protected:
	TYPE*	m_pDatas;
	int		m_nLength;
	int		m_nAllocLength;

	void operator = (const AArrayT<TYPE>& a) {}
	AArrayT(const AArrayT<TYPE>& a){}
public:
	AArrayT();
	AArrayT(int nCount);
	~AArrayT();

	void SetLength	(int nLength);
	int  GetLength	() const;
	void Release	();
	bool InsertAt	(int nIndex, const TYPE& tData);
	bool RemoveAt	(int nIndex);
	int  Find		(const TYPE& tData) const;
	bool Move		(int nFrom, int nTo);
	bool IsIndexOut	(int nIndex);

	TYPE& operator [] (int nIndex);
	const TYPE& operator [] (int nIndex) const;
};

// 一个字符串的数组，可以用于多行文本编辑，列表框等。
class AArrayString
{
protected:
	AArrayT<char*> m_aData;
public:
	AArrayString();
	~AArrayString();

	void Release		();
	bool InsertAt		(int nIndex, const char* pszStr);
	bool RemoveAt		(int nIndex);
	bool Move			(int nFrom, int nTo);
	bool AddString		(const char* pszStr);
	bool RemoveString	(const char* pszStr);
	bool ReplaceString	(int nIndex, const char* pszNewStr);
	int  GetLength		() const { return m_aData.GetLength(); }
	int  Find			(const char* pszStr);
	LPCSTR GetString	(int nIndex);
};

// class AArrayT

template<typename TYPE>
AArrayT<TYPE>::AArrayT():
m_pDatas		(NULL),
m_nLength		(0),
m_nAllocLength	(0)
{
}

template<typename TYPE>
AArrayT<TYPE>::AArrayT(int nLength)
{
	if(nLength > 0)
	{
		m_pDatas		= (TYPE*) malloc(sizeof(TYPE)*nLength);
		m_nLength		= nLength;
		m_nAllocLength	= nLength;
	}
	else
	{
		m_pDatas		= NULL;
		m_nLength		= 0;
		m_nAllocLength	= 0;
	}
}

template<typename TYPE>
AArrayT<TYPE>::~AArrayT()
{
	Release();
}

template<typename TYPE>
void AArrayT<TYPE>::Release()
{
	if(m_pDatas != NULL)
	{
		free(m_pDatas);
		m_pDatas		= NULL;
		m_nLength		= 0;
		m_nAllocLength	= 0;
	}
}

template<typename TYPE>
int  AArrayT<TYPE>::GetLength() const
{
	return m_nLength;
}

template<typename TYPE>
void AArrayT<TYPE>::SetLength(int nLength)
{
	assert(nLength >= 0);

	if(m_nAllocLength < nLength)
	{
		if(m_nAllocLength > 0)
		{
			m_pDatas		= (TYPE*) realloc(m_pDatas, sizeof(TYPE)*nLength);
		}
		else
		{
			m_pDatas		= (TYPE*) malloc(sizeof(TYPE)*nLength);
		}
		m_nLength		= nLength;
		m_nAllocLength	= nLength;
	}
	else
	{
		m_nLength	= nLength;
	}
}

template<typename TYPE>
TYPE& AArrayT<TYPE>::operator [] (int nIndex)
{
	assert(nIndex >= 0 && nIndex < m_nLength);
	return m_pDatas[nIndex];
}

template<typename TYPE>
const TYPE& AArrayT<TYPE>::operator [] (int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_nLength);
	return m_pDatas[nIndex];
}

template<typename TYPE>
bool AArrayT<TYPE>::InsertAt(int nIndex, const TYPE& tData)
{
	if(nIndex < 0 || nIndex > m_nLength)
	{
		return false;
	}
	if(m_nLength == m_nAllocLength)
	{
		int nAllocLength;
		if(m_nLength < 1024)
		{
			nAllocLength	= (m_nLength + 1) << 1 ;
		}
		else
		{
			nAllocLength	= m_nLength + 1024;
		}
		if(m_pDatas	!= NULL)
		{
			m_pDatas	= (TYPE*)realloc(m_pDatas, sizeof(TYPE)*nAllocLength);
		}
		else
		{
			m_pDatas	= (TYPE*)malloc(sizeof(TYPE)*nAllocLength);
		}
		m_nAllocLength	= nAllocLength;
	}
	if(nIndex < m_nLength)
	{
		MoveMemory(&m_pDatas[nIndex+1], &m_pDatas[nIndex], (m_nLength - nIndex)*sizeof(TYPE));
	}
	m_nLength ++; 
	m_pDatas[nIndex]	= tData;
	return true;
}

template<typename TYPE>
bool AArrayT<TYPE>::RemoveAt(int nIndex)
{
	if(nIndex < 0 || nIndex >= m_nLength)
	{
		return false;
	}
	MoveMemory(&m_pDatas[nIndex], &m_pDatas[nIndex+1], (m_nLength-1)*sizeof(TYPE));
	m_nLength --;
	return true;
}

template<typename TYPE>
int  AArrayT<TYPE>::Find(const TYPE& tData) const
{
	int i;
	for(i = m_nLength - 1; i >= 0; i --)
	{
		if(tData == m_pDatas[i])
		{
			break;
		}
	}
	return i;
}

template<typename TYPE>
bool AArrayT<TYPE>::IsIndexOut(int nIndex)
{
	return (nIndex < 0) || (nIndex >= m_nLength);
}

template<typename TYPE>
bool AArrayT<TYPE>::Move(int nFrom, int nTo)
{
	if( IsIndexOut(nFrom) ||
		IsIndexOut(nTo))
	{
		return false;
	}

	if(nFrom == nTo)
	{
		return true;
	}

	TYPE temp	= m_pDatas[nFrom];

	if(nFrom < nTo)
	{
		MoveMemory(&m_pDatas[nFrom], &m_pDatas[nFrom+1], nTo - nFrom);
	}
	else if(nFrom > nTo)
	{
		MoveMemory(&m_pDatas[nTo+1], &m_pDatas[nTo], nTo - nFrom);
	}

	m_pDatas[nTo]	= temp;

	return true;
}

#endif	// __AArrayT_H__