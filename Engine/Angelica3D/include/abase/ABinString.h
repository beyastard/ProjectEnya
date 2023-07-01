// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: ABinString.h
// Creator: Wei Hua (魏华)
// 头部包含总长度的字串

#ifndef	__ABINSTRING_H__
#define	__ABINSTRING_H__

typedef	short	BINSTRSIZE;

int	BinStrCpy(char *pszDst, int nMaxSize, const char *pszSrc);
int	BinStrCmp(const char *pszStr1, const char *pszStr2);
int	BinStrCatStr(char *pszDst, int nMaxSize, const char *pSrc);								// Cat会改变长度
int	BinStrCatBin(char *pszDs, int nMaxSizet, const void *pSrc, int nSrcLen);
int	BinStrReadStr(const char *pszSrc, int nOffset, char *pszStr);				// 返回下一段数据开始的offset
int	BinStrReadBin(const char *pszSrc, int nOffset, int nSize, void *pBin);		// 同上
int	BinStrGetSize(const char *pszStr, int *pnDataSize, int *pnTotalSize);
inline unsigned long	BinStrHash(const char *s)
{
	BINSTRSIZE		size, i;
	unsigned long	h = 0;
	size	= *(BINSTRSIZE *)s;
	s		+= sizeof(BINSTRSIZE);
	for(i=0;i<size;i++)
	{
		h	= h * 31 + *(unsigned char *)s;
		s	++;
	}
	return h;
}

#endif
