// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ABinString.cpp
// Creator: Wei Hua (魏华)
// 头部包含总长度的字串

#include "ABinString.h"
#include <string.h>

int	BinStrCpy(char *pszDst, int nMaxSize, const char *pszSrc)
{
	int		size;
	size	= *(BINSTRSIZE*)pszSrc + sizeof(BINSTRSIZE);
	if( size>nMaxSize )
	{
		return	-1;
	}
	memcpy(pszDst, pszSrc, size);
	return	size;
}
int	BinStrCmp(const char *pszStr1, const char *pszStr2)
{
	int		size;
	size	= *(BINSTRSIZE*)pszStr1 + sizeof(BINSTRSIZE);
	return	memcmp(pszStr1, pszStr2, size);
}
int	BinStrCatStr(char *pszDst, int nMaxSize, const char *pSrc)
{
	int		size;
	size	= strlen(pSrc) + 1;
	return	BinStrCatBin(pszDst, nMaxSize, pSrc, size);
}
int	BinStrCatBin(char *pszDst, int nMaxSize, const void *pSrc, int nSrcLen)
{
	int		size, tsize;
	tsize	= *(BINSTRSIZE*)pszDst;
	size	= nSrcLen;
	memcpy(pszDst+sizeof(BINSTRSIZE)+tsize, pSrc, size);
	tsize	+= size;
	if( tsize+(int)sizeof(BINSTRSIZE)>nMaxSize )
	{
		return	-1;
	}
	*(BINSTRSIZE*)pszDst	= tsize;

	return	tsize;
}
int	BinStrReadStr(const char *pszSrc, int nOffset, char *pszStr)
{
	int		size;
	const char *	pszBegin;
	pszBegin= pszSrc + sizeof(BINSTRSIZE) + nOffset;
	size	= strlen(pszBegin) + 1;
	memcpy(pszStr, pszBegin, size);
	return	nOffset + size;
}
int	BinStrReadBin(const char *pszSrc, int nOffset, int nSize, void *pBin)
{
	int		size;
	const char *	pszBegin;
	pszBegin= pszSrc + sizeof(BINSTRSIZE) + nOffset;
	size	= nSize;
	memcpy(pBin, pszBegin, size);
	return	nOffset + size;
}
int	BinStrGetSize(const char *pszStr, int *pnDataSize, int *pnTotalSize)
{
	*pnDataSize		= *(BINSTRSIZE*)pszStr;
	*pnTotalSize	= *pnDataSize + sizeof(BINSTRSIZE);
	return	0;
}
