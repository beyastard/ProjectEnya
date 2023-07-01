// Copyright (C) Beijing E-Pie Entertainment & Technology Co., Ltd.
// All rights reserved.
// File: AStringArray.h
// Creator: Wei Hua (魏华)
// 字传数组

#ifndef	__ASTRINGARRAY_H__
#define	__ASTRINGARRAY_H__

class	AStringArray
{
protected:
	char	*m_pBuffer;
	int		m_nTotalUnit;
	int		m_nUnitLen;
public:
	AStringArray();
	~AStringArray();
	bool	Init(int nTotalUnit, int nUnitLen);
	bool	Release();
	char *	GetUnit(int nIdx);
	void	ClearUnit(int nIdx);
	bool	StrCpy(int nIdx, char *pszStr);
};

class	ABinStringArray	: public AStringArray
{
public:
	bool	StrCpy(int nIdx, char *pszStr);
};

#endif